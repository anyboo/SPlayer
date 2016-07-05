#pragma once
#include "StdAfx.h"
#include "DShowVideoSource.h"
#include <VCA5CoreLib.h>
#include "ImageDataQueue.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))

//--------------------------------------------------------------------------------

CDShowVideoSource::CDShowVideoSource( )
{
	m_hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	m_pAlignedImg = (unsigned char *)_aligned_malloc( 1024 * 1024 * 2, 16 );

	m_pCap = new DShowCap();
	m_pCap->RegisterCallback( DShowCbFunc, this );

	m_bOpen = FALSE;

	memset( &m_bih, 0, sizeof( BITMAPINFOHEADER ) );

	// Put some reasonable defaults in, just to bootstrap
	m_bih.biWidth = DEFAULT_WIDTH;
	m_bih.biHeight = DEFAULT_HEIGHT;

	m_iDeviceIdx = 0;
	m_iMediaFormatIdx = 0;

	m_pQueue		= NULL;
	InitializeCriticalSection(&m_csBufLock);
}

//--------------------------------------------------------------------------------

CDShowVideoSource::~CDShowVideoSource()
{
	_aligned_free( m_pAlignedImg );
	m_pCap->RegisterCallback( DShowCbFunc, NULL);
	delete m_pCap;

	delete m_pQueue;

	m_pCap = NULL;
}


//--------------------------------------------------------------------------------

BOOL CDShowVideoSource::Open( LPCTSTR lpszFile, DWORD dwParam )
{
	if(m_bOpen){
		TRACE("DShowVideoSource: Already open!\n");
		return FALSE;
	}

	CAPDEVICE devices[ MAX_DEVICES ];
	int iNumDevices = 0;
	int iDeviceInx = 0;
		
	if(!SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ))){
		TRACE("!! DShowVideoSource : GetCapDevices Failed !! \n");
		return FALSE;
	}

	iDeviceInx = dwParam;//_tstoi(lpszFile);
	if(iDeviceInx >= iNumDevices){
		TRACE("DShowVideoSource :  Device index[%d] exceed max NumDevice [%d]\n", iDeviceInx ,iNumDevices);
		return FALSE;
	}
	
	m_bOpen		= TRUE;
	m_iDeviceIdx= iDeviceInx;

	m_sFilename = CString( lpszFile );

	return TRUE;
}

//--------------------------------------------------------------------------------

void CDShowVideoSource::Close()
{
	Stop();
	m_bOpen = FALSE;
}

//--------------------------------------------------------------------------------

BOOL CDShowVideoSource::Start()
{
	HRESULT hr = E_FAIL;
	
	if( m_sFilename.GetLength() )
	{
		// URI
		hr = m_pCap->StartURI( m_sFilename );
	}
	else
	{
		hr = m_pCap->StartCap( m_iDeviceIdx, m_iMediaFormatIdx );
	}

	return SUCCEEDED( hr );
}

//--------------------------------------------------------------------------------

void CDShowVideoSource::Stop()
{
	m_pCap->StopCap();
}

//--------------------------------------------------------------------------------

void CDShowVideoSource::OnNewFrame( BITMAPINFOHEADER &bih, unsigned char *pData, unsigned int uiLen, __int64 timeStamp )
{
	EnterCriticalSection(&m_csBufLock);

	if(m_pQueue == NULL) {
		m_pQueue		= new CImageDataQueue(60, bih.biWidth * bih.biHeight * (bih.biBitCount/8));
	}

	/*
	int h, width_size;
	memcpy( &m_bih, &bih, sizeof( BITMAPINFOHEADER ) );
	
	//
	if((bih.biCompression == 0) && (bih.biHeight > 0)){
		width_size= bih.biWidth*bih.biBitCount/8;
		for(h = 0 ; h < bih.biHeight ; h++){
			memcpy( m_pAlignedImg + h*width_size, (pData + uiLen) - h*width_size, width_size);
		}
	}else{
		memcpy( m_pAlignedImg, pData, uiLen );
	}

	m_bGotOneFrame = TRUE;

	*/


	CImageDataQueue::IMAGEDATA ImageData = {pData, *((FILETIME *)&timeStamp), uiLen};

	// TBD - if it's full, start removing stuff from the front and replacing at the back
	m_pQueue->Add(&ImageData);

	SetEvent(m_hEvent);

	LeaveCriticalSection(&m_csBufLock);
}

//-------------------------------------------------------------------------------

BOOL CDShowVideoSource::GetRawImage( BYTE **ppRawData, FILETIME *timestamp, DWORD *bEOF )
{
	BOOL bOk = FALSE;
	*bEOF = FALSE;
	
	//does not ready Queue before OnNewFrame, some old pc or notebook
	if(!m_pQueue){
		return FALSE;
	}

	EnterCriticalSection(&m_csBufLock);

	CImageDataQueue::IMAGEDATA 	*pImageData = m_pQueue->Get();

	if(pImageData)
	{
		*ppRawData = pImageData->pData;
		*timestamp= pImageData->TimeStamp;
		bOk = TRUE;
	}

	LeaveCriticalSection(&m_csBufLock);

	return bOk;
}

//-------------------------------------------------------------------------------

BOOL CDShowVideoSource::ReleaseRawImage()
{
	m_pQueue->Remove();

	return TRUE;
}

//-------------------------------------------------------------------------------

BOOL CDShowVideoSource::Control( IVCAVideoSource::eCMD cmd, DWORD_PTR param1, DWORD_PTR param2 )
{
	CAPDEVICE devices[ MAX_DEVICES ];
	int iNumDevices = 0;

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
				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					int fmt = 0;

					BITMAPINFOHEADER *pBih = &(devices[ m_iDeviceIdx ].viFormats[m_iMediaFormatIdx].bmiHeader);

					switch( pBih->biCompression )
					{
					case BI_RGB:
						{
							switch( pBih->biBitCount )
							{
							case 16: fmt = VCA5_COLOR_FORMAT_RGB16; break;
							case 24: fmt = VCA5_COLOR_FORMAT_RGB24; break;
							}
						}
						break;

					case FCC('YUY2'): fmt = VCA5_COLOR_FORMAT_YUY2; break;

					case FCC('IYUV'):
					case FCC('I420'):
					case FCC('YV12'): fmt = VCA5_COLOR_FORMAT_YV12; break;

					case FCC('UYVY'): fmt = VCA5_COLOR_FORMAT_UYVY; break;
					}

					(*(DWORD *) param1) = fmt;
				}
				else
				{
					return FALSE;
				}
				return TRUE;
			}

		case CMD_GET_IMGSIZE:
			{

				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					BITMAPINFOHEADER *pBih = &(devices[ m_iDeviceIdx ].viFormats[m_iMediaFormatIdx].bmiHeader);

					(*(DWORD *) param1) = VCA5_MAKEIMGSIZE( pBih->biWidth, pBih->biHeight );
				}
				else
				{
					return FALSE;
				}

				return TRUE;
			}
		
		case CMD_GET_FRAMERATE:
			{


				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					(*(DWORD *) param1) = (DWORD)(10000000 / devices[ m_iDeviceIdx ].viFormats[m_iMediaFormatIdx].AvgTimePerFrame);
				}
				else
				{
					(*(DWORD *) param1) = 25;
				}
				return TRUE;
			}
		
		case CMD_GET_VIDEOFORMAT:
			(*(DWORD *) param1) = VCA5_VIDEO_FORMAT_PAL_B;
			return TRUE;

		case CMD_GET_NUM_DEVICES:
			{
				// See if there are any devices
				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					*((int *)param1) = iNumDevices;
					return TRUE;
				}

				return FALSE;
			}
			break;

		case CMD_GET_DEVICE:
			{
				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					TCHAR *pT = (TCHAR *)param2;
					if( (int)param1 < iNumDevices )
					{
						_tcscpy_s( pT, 128, devices[param1].tszName );
						return TRUE;
					}
				}

				return FALSE;
			}
			break;

		case CMD_SET_DEVICE:
			{
				m_iDeviceIdx = (int)param1;
			}
			break;

		case CMD_GET_NUM_MEDIAFORMATS:
			{
				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					if( (int)param1 < iNumDevices )
					{
						int *pI = (int *)param2;
						*pI = devices[param1].iNumFormats;
					}
				}
			}
			break;

		case CMD_GET_MEDIAFORMAT:
			{
				if( SUCCEEDED( m_pCap->GetCapDevices( devices, iNumDevices ) ) )
				{
					if( (int)param1 < iNumDevices )
					{
						if( *((int*)param2) < devices[param1].iNumFormats )
						{
							VIDEOINFOHEADER *pHdr = (VIDEOINFOHEADER *)&(devices[param1].viFormats[*((int*)param2)]);

							TCHAR tszFourCC[256];
							switch( pHdr->bmiHeader.biCompression  )
							{
							case BI_RGB:
								{
									_stprintf_s( tszFourCC, _countof( tszFourCC ), _T("RGB%d"), pHdr->bmiHeader.biBitCount );
								}
								break;

							default:
								{
									char *pszFourCC = (char *)&(pHdr->bmiHeader.biCompression);
									_stprintf_s( tszFourCC, _countof( tszFourCC ), _T("%c%c%c%c"), pszFourCC[0], pszFourCC[1], pszFourCC[2], pszFourCC[3] );
								}
							}

							TCHAR *pT = (TCHAR *)param2;
							_stprintf_s( pT, 128, _T("%s: %d x %d @ %dfps"), tszFourCC, pHdr->bmiHeader.biWidth,
									pHdr->bmiHeader.biHeight, 10000000 / pHdr->AvgTimePerFrame );
						}
					}
				}
			}
			break;

		case CMD_SET_MEDIAFORMAT:
			{
				m_iMediaFormatIdx = (int)param1;
			}
			break;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------
// Callback function

// static
BOOL __stdcall CDShowVideoSource::DShowCbFunc( BITMAPINFOHEADER &bih, unsigned char *pData, unsigned int uiLen, __int64 timeStamp, void *pUserData )
{
	((CDShowVideoSource *)pUserData)->OnNewFrame( bih, pData, uiLen, timeStamp );

	return TRUE;
}

