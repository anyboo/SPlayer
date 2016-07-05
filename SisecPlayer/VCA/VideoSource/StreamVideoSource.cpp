#include "stdafx.h"
#include "StreamVideoSource.h"
#include <dshow.h>
#include <aviriff.h>
#include <VCA5CoreLib.h>

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

//------------------------------------------------------------------------------------

CStreamVideoSource::CStreamVideoSource(void)
{
	memset( &m_piStreamer, 0, sizeof( PROCESS_INFORMATION) );

	SetCallback( this );

	m_pAvimh	= new AVIMAINHEADER;
	m_pAvish	= new AVISTREAMHEADER;
	m_pAvibih	= new BITMAPINFOHEADER;

	// Set some sensible defaults
	memset( m_pAvimh, 0, sizeof( AVIMAINHEADER ) );
	memset( m_pAvish, 0, sizeof( AVISTREAMHEADER ) );
	memset( m_pAvibih, 0, sizeof( BITMAPINFOHEADER ) );


	m_pAvimh->dwWidth = DEFAULT_WIDTH;
	m_pAvimh->dwHeight = DEFAULT_HEIGHT;
	m_pAvimh->dwMicroSecPerFrame = 3300000;	// NTSC 33ms

	m_pAvibih->biWidth = DEFAULT_WIDTH;
	m_pAvibih->biHeight = DEFAULT_HEIGHT;

	m_bOpen		= FALSE;
	m_bGotMainHeader = FALSE;
	m_bRun		= FALSE;
	m_uiFrameCount = 0;

	m_pAlignedImg = (unsigned char *)_aligned_malloc( 1920 * 1080 * 3, 16 );

	m_hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hThreadEndEvent	= CreateEvent( NULL, FALSE, FALSE, NULL );

	InitializeCriticalSection(&m_csBufLock);
}

//------------------------------------------------------------------------------------

CStreamVideoSource::~CStreamVideoSource(void)
{
	delete m_pAvimh;
	delete m_pAvish;
	delete m_pAvibih;

	_aligned_free( m_pAlignedImg );

	CloseHandle( m_hEvent );
	CloseHandle( m_hThreadEndEvent );
}

//------------------------------------------------------------------------------------

BOOL CStreamVideoSource::Open(LPCTSTR lpszFile, DWORD dwParam)
{
	if( m_bOpen )
	{
		TRACE( "CStreamVideoSource: Already opened!\n" );
		Close();
	}

	CString sArgs = CString( (TCHAR *)dwParam );

	m_sFilename = CString( lpszFile );

	m_sFilename += CString(" ") + sArgs;

	m_bOpen = TRUE;

	return TRUE;
}

//------------------------------------------------------------------------------------

void CStreamVideoSource::Close()
{
	Stop();

	m_bOpen = FALSE;
}


//------------------------------------------------------------------------------------

void CStreamVideoSource::CleanupChild()
{
	// Clean up the mess from the kids...
	CloseHandle( m_piStreamer.hProcess );
	CloseHandle( m_piStreamer.hThread );
	CloseHandle( m_hChildStdOut_W );
	CloseHandle( m_hChildStdOut_R );

	m_hChildStdOut_R = 0;
	m_hChildStdOut_W = 0;
	
	memset( &m_piStreamer, 0, sizeof( PROCESS_INFORMATION ) );

}

//--------------------------------------------------------------------------------------
BOOL CStreamVideoSource::Control(IVCAVideoSource::eCMD cmd, DWORD_PTR param1, DWORD_PTR param2)
{
	switch( cmd )
	{
		case CMD_SET_VIDEOFORMAT:
		case CMD_SET_COLORFORMAT:
		case CMD_SET_IMGSIZE:
			return TRUE;
			
		case CMD_SET_FRAMERATE:
		//	m_FPS = param1;
			return TRUE;

		case CMD_GET_COLORFORMAT:
			(*(DWORD *) param1) = VCA5_COLOR_FORMAT_YUY2;		// TBD: sort
			return TRUE;

		case CMD_GET_IMGSIZE:{
//			CYUVRec::YUVHEADER *pHeader = m_YUVReader.GetHeader();
			(*(DWORD *) param1) = VCA5_MAKEIMGSIZE(m_pAvimh->dwWidth, m_pAvimh->dwHeight);
			return TRUE;
		 }
		
		case CMD_GET_FRAMERATE:
			(*(DWORD *) param1) = 1000000 / m_pAvimh->dwMicroSecPerFrame;
			return TRUE;
		
		case CMD_GET_VIDEOFORMAT:
			(*(DWORD *) param1) = VCA5_VIDEO_FORMAT_PAL_B;
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------

BOOL CStreamVideoSource::Start()
{
	SECURITY_ATTRIBUTES sa;
	memset( &sa, 0, sizeof( SECURITY_ATTRIBUTES ) );

	sa.nLength				= sizeof( SECURITY_ATTRIBUTES );
	sa.bInheritHandle		= TRUE;
	sa.lpSecurityDescriptor	= NULL;

	// Create a pipe to capture child process STDOUT
	if( !CreatePipe( &m_hChildStdOut_R, &m_hChildStdOut_W, &sa, 0 ) )
	{
		TRACE( "CStreamVideoSource: Unable to create pipe to child\n" );
		return FALSE;
	}

	// Ensure read handle to the pipe from STDOUT is not inherited
	if( !SetHandleInformation( m_hChildStdOut_R, HANDLE_FLAG_INHERIT, 0 ) )
	{
		TRACE( "CStreamVideoSource: Unable to adjust read handle inerit flags\n" );
		return FALSE;
	}

	// Start the child process
	STARTUPINFO si;
	memset( &si, 0, sizeof( STARTUPINFO ) );
	si.hStdOutput	= m_hChildStdOut_W;
	si.dwFlags		= STARTF_USESTDHANDLES;

	if( !CreateProcess( NULL, m_sFilename.GetBuffer(0), NULL, NULL, TRUE,
							NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_piStreamer ) )
	{
		TRACE( "CStreamVideoSource: Unable to create process: %s\n", m_sFilename );
		MessageBox(NULL, _T("Can not run VLC player, please install VLC player or check VLC player path setting"), _T("ERROR"), MB_OK );
		return FALSE;
	}

	m_bOpen = TRUE;

	// Now cue the stream up
	m_bGotMainHeader = FALSE;


	m_bGotVideoFrame = FALSE;
	if( !m_bRun )
	{
		m_bRun = TRUE;
		DWORD threadID;
		CreateThread(NULL,0, CStreamVideoSource::StreamThreadStub, (LPVOID) this,NULL,&threadID );
	}

	return TRUE;
}

//------------------------------------------------------------------------------------

void CStreamVideoSource::Stop()
{
	if( m_bRun )
	{
		m_bRun = FALSE;
		// Wait for stop
		ASSERT( WaitForSingleObject( m_hThreadEndEvent, 10000 ) == WAIT_OBJECT_0 );

		if( m_piStreamer.hProcess )
		{
			TerminateProcess( m_piStreamer.hProcess, 0 );

			CleanupChild();
		}
	}
}

//--------------------------------------------------------------------------------------

void CStreamVideoSource::OnMainHeader( AVIMAINHEADER &avimh, void *pUserData )
{
	*m_pAvimh = avimh;

	m_bGotMainHeader = TRUE;
}

//--------------------------------------------------------------------------------------

void CStreamVideoSource::OnStreamHeader( AVISTREAMHEADER &avish, void *pUserData )
{
	m_bVideoFmtComing = FALSE;

	if( streamtypeVIDEO == avish.fccType )
	{
		*m_pAvish = avish;

		m_bVideoFmtComing = TRUE;
	}
}

//--------------------------------------------------------------------------------------

void CStreamVideoSource::OnStreamFormat( unsigned char *pFmtBuf, unsigned int uiLen, void *pUserData )
{
	if( m_bVideoFmtComing )
	{
		memcpy( m_pAvibih, pFmtBuf, sizeof( BITMAPINFOHEADER ) );
	}
}

//--------------------------------------------------------------------------------------

void CStreamVideoSource::OnNewFrame(DWORD dwFourCC, unsigned char *pBuf, unsigned int uiLen, void *pUserData)
{
	// See if it's a video stream

	if( (FCC('xxdc') & 0xFFFF0000) == (dwFourCC & 0xFFFF0000))
	{

		// Lock the buffer
		EnterCriticalSection(&m_csBufLock);

		// This is quite slow, but our AVI parser is quite basic. Functions down the line need aligned memory
		// so we copy it across here...
		memcpy( m_pAlignedImg, pBuf, uiLen );
		
		m_bGotVideoFrame = TRUE;

		SetEvent( m_hEvent );

		LeaveCriticalSection(&m_csBufLock);
	}
}

//------------------------------------------------------------------------------------

void CStreamVideoSource::OnStreamEnd(void *pUserData)
{
	// Do nothing
}

//-------------------------------------------------------------------------------------

BOOL CStreamVideoSource::GetRawImage(BYTE **pRawData, FILETIME *timestamp, DWORD *bEOF )
{

	// Lock the image here and don't unlock until we get the frame back
	EnterCriticalSection(&m_csBufLock);
	*bEOF = FALSE;

	if( m_bGotVideoFrame )
	{
		// Get the next frame
		*pRawData = m_pAlignedImg;

		// Need Time stamp
		GetSystemTimeAsFileTime(timestamp);
		FileTimeToLocalFileTime(timestamp, timestamp);

		m_bGotVideoFrame = FALSE;
		return TRUE;
	}
	else
	{
		// Nothing.. unlock
		LeaveCriticalSection(&m_csBufLock);

		// Check for process exit
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_piStreamer.hProcess, 0 ) )
		{
			// Child process has disappeared (exited)
			*bEOF = TRUE;

			m_bRun = FALSE;
			CleanupChild();
		}
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------


BOOL CStreamVideoSource::ReleaseRawImage( )
{
	LeaveCriticalSection(&m_csBufLock);

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CStreamVideoSource::Pump()
{
	DWORD dwBytesRead, dwBytesToRead;

	unsigned char *pBuf = GetWritePtr();
	unsigned int uiBufLen = GetBufferSize();

	{
		dwBytesToRead = 10 * 1024;//dwBytesRead;

		if( !ReadFile( m_hChildStdOut_R, pBuf, dwBytesToRead, &dwBytesRead, NULL ) )
		{
			TRACE( "CStreamVideoSource: Unable to ReadFile\n" );
			return FALSE;
		}

		// Process the data
		DataAdded( dwBytesRead, NULL );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------

void CStreamVideoSource::StreamThread()
{
	// Streamer thread
	// Assumes that the delivering process will do the frame scheduling (because we go as fast as we can)
	while( m_bRun )
	{
		// Pump it
		if( !Pump() )
		{
			break;
		}
	}

	SetEvent( m_hThreadEndEvent );
}

//---------------------------------------------------------------------------------------

// static
DWORD CStreamVideoSource::StreamThreadStub(LPVOID lpThis)
{
	((CStreamVideoSource*)lpThis)->StreamThread();

	return 0;
}