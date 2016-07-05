#include "stdafx.h"
#include "VirtualVideoSource.h"
#include "VCAConfig.h"
#include <VCA5CoreLib.h>
#include <mmsystem.h>

// Max num frames to store
#define MAX_Q_LEN 60

//------------------------------------------------------------------------------------

CVirtualVideoSource::CVirtualVideoSource(void)
{
	m_bOpen			= FALSE;
	m_hEvent		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_pCurSample	= NULL;

	memset( &m_bih, 0, sizeof( BITMAPINFOHEADER ) );

	m_hNewFrameEvent		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hEndStreamThread		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hStreamThreadEnded	= CreateEvent( NULL, FALSE, FALSE, NULL );
	
	InitializeCriticalSection(&m_csLock);
}

//------------------------------------------------------------------------------------

CVirtualVideoSource::~CVirtualVideoSource(void)
{
	CloseHandle( m_hEvent );
}

//------------------------------------------------------------------------------------

BOOL CVirtualVideoSource::Open(LPCTSTR lpszFile, DWORD dwParam)
{
	if( m_bOpen )
	{
		TRACE( "CStreamVideoSource: Already opened!\n" );
		Close();
	}

	m_bOpen = TRUE;


	return TRUE;
}

//------------------------------------------------------------------------------------

void CVirtualVideoSource::Close()
{
	m_bOpen = FALSE;
}



//--------------------------------------------------------------------------------------
BOOL CVirtualVideoSource::Control(IVCAVideoSource::eCMD cmd, DWORD_PTR param1, DWORD_PTR param2)
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
		{
			ULONG ulColorFmt = VCA5_COLOR_FORMAT_YUY2;
			if( m_bih.biCompression == mmioFOURCC('Y','U','Y','2') )
			{
				ulColorFmt = VCA5_COLOR_FORMAT_YUY2;
			}
			else
			if( m_bih.biCompression == mmioFOURCC('Y','V','1','2') )
			{
				ulColorFmt = VCA5_COLOR_FORMAT_YV12;
			}
			else
			if( m_bih.biCompression == mmioFOURCC('U','Y','V','Y') )
			{
				ulColorFmt = VCA5_COLOR_FORMAT_UYVY;
			}
			else
			if( m_bih.biCompression == BI_RGB )
			{
				if( m_bih.biBitCount == 16 )
				{
					ulColorFmt = VCA5_COLOR_FORMAT_RGB16;
				}
				else
				if( m_bih.biBitCount == 15 )
				{
					ulColorFmt = VCA5_COLOR_FORMAT_RGB15;
				}
				else
				if( m_bih.biBitCount == 24 )
				{
					ulColorFmt = VCA5_COLOR_FORMAT_RGB24;
				}
				else
				{
					// Not supported
				//	ASSERT( FALSE );
				}
			}

			(*(DWORD *) param1) = ulColorFmt;
			return TRUE;
		}

		case CMD_GET_IMGSIZE:{
			(*(DWORD *) param1) = VCA5_MAKEIMGSIZE(m_bih.biWidth, m_bih.biHeight);
			return TRUE;
		 }
		
		case CMD_GET_FRAMERATE:
			(*(DWORD *) param1) = 30; // Make it up - we'll adjust once we know
			return TRUE;
		
		case CMD_GET_VIDEOFORMAT:
			(*(DWORD *) param1) = VCA5_VIDEO_FORMAT_PAL_B;
			return TRUE;

		case CMD_ADD_MEDIASAMPLE:
			IVCAMediaSample * pSamp = (IVCAMediaSample *)param1;
			OnNewFrame( pSamp );
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------

BOOL CVirtualVideoSource::Start()
{
	// Start the streaming thread
//	AfxBeginThread( StreamingThreadStub, (LPVOID)this );

	return TRUE;
}

//------------------------------------------------------------------------------------

void CVirtualVideoSource::Stop()
{
	// Stop streaming thread
//	SetEvent( m_hEndStreamThread );
//
//	VERIFY( WaitForSingleObject( m_hStreamThreadEnded, 5000 ) == WAIT_OBJECT_0 );
}



//-------------------------------------------------------------------------------------

BOOL CVirtualVideoSource::GetRawImage(BYTE **pRawData, FILETIME *timestamp, DWORD *bEOF)
{
	BOOL bRet = FALSE;

	EnterCriticalSection(&m_csLock);
	*bEOF = FALSE;

	if( m_samples.size() )
	{
		ASSERT( !m_pCurSample );

		m_pCurSample = m_samples.front();
		m_samples.pop();

		// Check for change in settings
		VCACONFIG_MEDIASAMPLEHDR hdr;

		if( SUCCEEDED( m_pCurSample->GetHeader( hdr ) ) )
		{
	//		CString s;
	//		s.Format( _T("GETTING TS: %I64d Q-LEN: %d\n"), hdr.timeStamp, m_samples.size() );
	//		OutputDebugString( s );

			timestamp->dwLowDateTime = (DWORD)(hdr.timeStamp & 0x00000000FFFFFFFF);
			timestamp->dwHighDateTime = (DWORD)(hdr.timeStamp >> 32);

			BITMAPINFOHEADER *pBih = &(hdr.mediaHdr.videoHdr.bmih);
			if( pBih->biWidth != m_bih.biWidth || pBih->biHeight != m_bih.biHeight ||
				pBih->biCompression != m_bih.biCompression || pBih->biBitCount != m_bih.biBitCount )
			{
				// update
				m_bih = *pBih;
			}
		}

		int iLen;
		m_pCurSample->GetDataPtr( (void **)pRawData, iLen );

		bRet = TRUE;
	}
	else
	{
		//ASSERT( FALSE );
		bRet = FALSE;

	}

	LeaveCriticalSection(&m_csLock);

	return bRet;
}

BOOL CVirtualVideoSource::GetBufferLen( unsigned int *puiMaxLen, unsigned int *puiCurLen )
{
	// Return the maximum length of the buffer and the current size
	EnterCriticalSection(&m_csLock);

	*puiMaxLen = MAX_Q_LEN;
	*puiCurLen = (unsigned int)m_samples.size();

	LeaveCriticalSection(&m_csLock);

	return TRUE;
}

//--------------------------------------------------------------------------------------

void CVirtualVideoSource::OnNewFrame( IVCAMediaSample *pSamp )
{
	// New video frame came in

	EnterCriticalSection(&m_csLock);

	pSamp->AddRef();

	VCACONFIG_MEDIASAMPLEHDR hdr;
	pSamp->GetHeader( hdr );


//	TRACE( _T("ADDING TS: %I64d\n"), hdr.timeStamp );

	// Check Q len
	while( m_samples.size() > MAX_Q_LEN )
	{
		IVCAMediaSample *pOld = m_samples.front();
		pOld->Release();
		m_samples.pop();
	}

	// Add it to the back of the queue
	m_samples.push( pSamp );

//	TRACE(_T("Q is %d FRAME LONG\n"), m_samples.size() );

	// Signal the event

	SetEvent( m_hEvent );

	LeaveCriticalSection(&m_csLock);
}

//--------------------------------------------------------------------------------------

/*
//static
UINT CVirtualVideoSource::StreamingThreadStub( LPVOID lpThis )
{
	((CVirtualVideoSource *)lpThis)->StreamingThread();

	return 0;
}

//--------------------------------------------------------------------------------------

void CVirtualVideoSource::StreamingThread()
{
	// Main streaming thread that schedules frames from the queue
	unsigned int uiSleepTime = 33;			// Assume 30fps to start with
	const unsigned int IDEAL_BUF_LEN = 15;	// Keep 1/2s video in the buffer
	const unsigned int MAX_BUF_LEN = 30;	// Drop frames if the buffer length exceeds this
	unsigned int uiTimeCheck = 0;

	__int64 iRefClock = 0;
	__int64 iStreamClock = 0;

	HANDLE hEvents[2];
	hEvents[0] = m_hEndStreamThread;
	hEvents[1] = m_hNewFrameEvent;

	BOOL bRun = TRUE;

	while( bRun )
	{
		switch( WaitForMultipleObjects( 2, hEvents, FALSE, INFINITE ) )
		{
			case WAIT_OBJECT_0:
			{
				bRun = FALSE;
			}
			break;

			case WAIT_OBJECT_0+1:
			{
				EnterCriticalSection(&m_csLock);

				if( m_samples.size() )
				{
					if( !iRefClock ) iRefClock = ((__int64)GetTickCount()) * 10000;
					__int64 iRefDiff = (((__int64)GetTickCount()) * 10000) - iRefClock;

					IVCAMediaSample *pSamp = m_samples.front();

					VCACONFIG_MEDIASAMPLEHDR hdr;
					pSamp->GetHeader( hdr );

					__int64 ts = hdr.timeStamp.dwHighDateTime;
					ts <<= 32;
					ts |= hdr.timeStamp.dwLowDateTime;


					if( !iStreamClock ) iStreamClock = ts;

					__int64 iStreamDiff = ts - iStreamClock;

					// Now, the time we have to wait is the difference between stream and reference time (stream time in the future)
					int iDiff = (int)(iStreamDiff/10000) - (int)(iRefDiff/10000);

					TRACE( _T("REFDIFF: %I64d STREAMDIFF: %I64d in ms: refdiff:%d streamdiff:%d FINALDIFF:%d\n"),
							iRefDiff, iStreamDiff, (int)(iRefDiff/10000), (int)(iStreamDiff/10000), iDiff );


					if( abs(iDiff) > 1000 )
					{
						// Way off the mark here, reset
						TRACE( _T("-------RESETTING----------\n"));

						iRefClock = 0;
						iStreamClock = 0;
					}
					else
					if( iDiff < 0 )
					{
						TRACE( _T("TOO FAST, slowing down\n"));
						// We are going to fast (this frame should have been displayed already)
						// Slow down
						iRefClock += 5 * 10000;
					}
					else
					if( iDiff > 100 )
					{
						TRACE( _T("TOO SLOW, speeding up\n"));
						// Going too slow
						iRefClock -= 5 * 10000;
					}

					// Update waiting time
					uiSleepTime = max( min( 1000, iDiff ), 10 );

					// Wait for this long
					TRACE(_T("Q:%d WAIT:%d\n"), m_samples.size(), uiSleepTime );
					switch( WaitForSingleObject( m_hEndStreamThread, uiSleepTime ) )
					{
						case WAIT_OBJECT_0:
						{
							bRun = FALSE;
						}
						break;

						case WAIT_TIMEOUT:
						{
							// Deliver the frame
							SetEvent( m_hEvent );
						}
						break;
					}
				}
				else
				{
					ASSERT( FALSE );
				}

				LeaveCriticalSection(&m_csLock);

			}
			break;
		}
	}

	SetEvent( m_hStreamThreadEnded );
}
*/
//--------------------------------------------------------------------------------------

BOOL CVirtualVideoSource::ReleaseRawImage( )
{
	// Done with it now
	EnterCriticalSection(&m_csLock);

	ASSERT( m_pCurSample );

	m_pCurSample->Release();
	m_pCurSample = NULL;

	LeaveCriticalSection(&m_csLock);

	return TRUE;
}