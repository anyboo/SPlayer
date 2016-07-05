// CompressedFileVideoSource.cpp: implementation of the CCompressedFileVideoSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <objbase.h>
#include "CompressedVideoSource.h"
#include <crtdbg.h>
#include <malloc.h>
#include "VCA5CoreLib.h"

//////////////////////////////////////////////////////////////////////
// Utiities 
//////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(ptr) {if (ptr) {ptr->Release(); ptr = NULL;}}
#pragma warning(disable: 4311 4312)

#define DEFAULT_FPS	25

#include <stdio.h>
#include <assert.h>
//
// ERR
//
void ERR(LPCTSTR lpszFormat, ...)
{
    va_list args ;
    va_start(args, lpszFormat) ;

	int nBuf ;
    TCHAR szBuffer[1024] ;  // Large buffer for very long filenames (like with HTTP)

	nBuf = _vstprintf_s(szBuffer, 1024, lpszFormat, args) ;

	// was there an error? was the expanded string too long?
	assert(nBuf >= 0) ;

	OutputDebugString(szBuffer) ;
//	MessageBox(NULL, szBuffer, "DirectShow", MB_OK) ;

    va_end(args) ;
}

#define JIF(x) { if (FAILED(hr=(x))) \
	{ ERR(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n"), hr); return hr; }}

#define LIF(x) { if (FAILED(hr=(x))) \
	{ ERR(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n"), hr); }}

#define FIF(x) { if (FAILED(hr=(x))) \
	{ ERR(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n"), hr); return FALSE; }}




// render filter (downstream)
HRESULT ConnectFilters(IGraphBuilder* pGB, IBaseFilter* pUpFilter, IBaseFilter* pDownFilter, int nMAX=3)
{
	HRESULT hr;
	// Get the first output pin of the new source filter. Audio sources 
	// typically have only one output pin, so for most audio cases finding 
	// any output pin is sufficient.

	IEnumPins *pUpEnumPins = NULL, *pDownEnumPins = NULL;
	IPin *ppinOut = NULL, *pDownstreamPin;
	IPin *ppinIn = NULL, *pUpstreamPin;
	PIN_DIRECTION direction = PINDIR_INPUT;
	bool bIsAnySuccess = false;

	JIF(pUpFilter->EnumPins(&pUpEnumPins));

	while (pUpEnumPins->Next(1, &ppinOut, 0) == S_OK)
	{
		JIF(ppinOut->QueryDirection(&direction));
		
		if (direction == PINDIR_OUTPUT)
		{
			ppinOut->ConnectedTo(&pDownstreamPin);
			
			if (pDownstreamPin == NULL)
			{
				// Downstream filter: pin enumeration
				JIF(pDownFilter->EnumPins(&pDownEnumPins));

				while (pDownEnumPins->Next(1, &ppinIn, 0) == S_OK)
				{
					JIF(ppinIn->QueryDirection(&direction));
					
					if (direction == PINDIR_INPUT)
					{
						ppinIn->ConnectedTo(&pUpstreamPin);
						
						if (pUpstreamPin == NULL)
						{
							//
							// We have two pins. Connect it
							//

							// Prevent infinite loop which is created
							// when two dynamically created filters are connected
							if (nMAX-- > 0)
							{
								hr = pGB->Connect(ppinOut, ppinIn);
							}

							if (hr == S_OK)
								bIsAnySuccess |= true;
							else if (hr == VFW_S_PARTIAL_RENDER) {
								bIsAnySuccess |= false;
							} else if (hr == E_ABORT) {
								bIsAnySuccess |= false;
							} else if (hr == E_POINTER) {
								bIsAnySuccess |= false;
							} else if (hr == VFW_E_CANNOT_CONNECT) {
								bIsAnySuccess |= false;
							} else if (hr == VFW_E_NOT_IN_GRAPH) {
								bIsAnySuccess |= false;
							}
						}
						SAFE_RELEASE(pUpstreamPin);
					}
					SAFE_RELEASE(ppinIn);
				}
				SAFE_RELEASE(pDownEnumPins);
			}
			// end of downstream
			SAFE_RELEASE(pDownstreamPin);
		}
		SAFE_RELEASE(ppinOut);
	}
	SAFE_RELEASE(pUpEnumPins);
	// end of upstream

	return (bIsAnySuccess == false ? E_FAIL : S_OK);
}

#if defined(_DEBUG)
// For graph spy
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
   IMoniker * pMoniker;
   IRunningObjectTable *pROT;
   WCHAR wsz[128];
   HRESULT hr;

   if (FAILED(GetRunningObjectTable(0, &pROT))) {
       return E_FAIL;
   }

   hr = CreateItemMoniker(L"!", wsz, &pMoniker);
   if (SUCCEEDED(hr)) {
       hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
       pMoniker->Release();
   }
   pROT->Release();
   return hr;
}

void RemoveGraphFromRot(DWORD pdwRegister)
{
   IRunningObjectTable *pROT;

   if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
       pROT->Revoke(pdwRegister);
       pROT->Release();
   }
} 
#endif

//////////////////////////////////////////////////////////////////////
// CSampleGrabberCB
//////////////////////////////////////////////////////////////////////


HRESULT CSampleGrabberCB::SampleCB(double time, IMediaSample *pSample) {
#ifdef SG_CALLBACK
	HRESULT hr;
	REFERENCE_TIME start;

	JIF(pSample->GetPointer(&m_pParent->m_pBytes));
	m_pParent->m_iLength = (LONG) pSample->GetActualDataLength();
	pSample->GetMediaTime(&start, &m_pParent->m_PlayPosition);
	
	SetEvent(m_pParent->m_hQueueEvent);
	if (m_pParent->m_bOpened) {
		WaitForSingleObject(m_pParent->m_hDequeueEvent, INFINITE);
	}
#endif
	return S_OK;
}

HRESULT CSampleGrabberCB::BufferCB(double time, BYTE *pBuffer, long iLength) {
	return S_OK;
}



//////////////////////////////////////////////////////////////////////
// CCompressedFileVideoSource
//////////////////////////////////////////////////////////////////////
CCompressedFileVideoSource::CCompressedFileVideoSource() : m_CB(this)
{
	CoInitialize(NULL);
	m_pGraphBuilder		= NULL;
	m_pSampleGrabber	= NULL;
	m_pMediaControl		= NULL;
	m_pMediaSeeking		= NULL;
	
	m_bOpened	= FALSE;
	m_pRawData	= NULL;
#if defined(_DEBUG)
	m_uGraphRegister = 0;
#endif

#ifdef SG_CALLBACK
	m_pBytes	= NULL;
	m_iLength	= 0;
#endif

	m_FPS	= DEFAULT_FPS;
}

CCompressedFileVideoSource::~CCompressedFileVideoSource()
{
	Close();
	CoUninitialize();
}

BOOL CCompressedFileVideoSource::Open(LPCTSTR szFile, DWORD param)
{
	if (m_bOpened) Close();

#ifdef SG_CALLBACK	
	m_hQueueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hDequeueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif

	if (BuildGraph(szFile) == FALSE) {
		ERR(_T("CCompressedFileVideoSource::Open, Cannot create a filter graph\n"));
		return FALSE;
	}

	if (GetBitmapInfo(m_BM, m_FPS) == FALSE) {
		ERR(_T("CCompressedFileVideoSource::Open, Cannot get a bitmap information\n"));
		return FALSE;
	}

	//if video file does not have fps information.
	if(m_FPS == 0 )m_FPS = DEFAULT_FPS;

	m_pRawData = (BYTE *) _aligned_malloc(m_BM.biWidth*m_BM.biHeight*4, 16);

	m_bOpened = TRUE;
	return TRUE;
}

VOID CCompressedFileVideoSource::Close() 
{
	if (FALSE == m_bOpened ) return;

	m_bOpened = FALSE;
#ifdef SG_CALLBACK
	SetEvent(m_hDequeueEvent);
#endif

	Stop();
	DestroyGraph();
	
#ifdef SG_CALLBACK
	CloseHandle(m_hDequeueEvent);
	CloseHandle(m_hQueueEvent);
#endif
	if (m_pRawData) {
		_aligned_free(m_pRawData);
	}

}

void CCompressedFileVideoSource::Stop() 
{
//	m_pMediaControl->Stop(); //thread call this function, blocking... need remove or comment.
	m_PlayDuration = 0;
	m_PlayPosition = 1;
}

BOOL CCompressedFileVideoSource::Start()
{
	m_PlayPosition = 1;
	HRESULT hr = m_pMediaControl->Run();

	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	FileTimeToLocalFileTime(&time, &time);

	memcpy(&m_StartTimestamp, &time, sizeof(FILETIME));
	
	return (S_OK == hr);
}

inline static void GetVideoFramesPerSecond( const AM_MEDIA_TYPE * pVT, long *plFramesPerSecond )
{
	long nTenMillion    = 10000000;
	long lAvgFrameDuration  = 0;

	if( pVT->formattype == FORMAT_VideoInfo) {
		lAvgFrameDuration = (LONG) ((VIDEOINFOHEADER *)(pVT->pbFormat))->AvgTimePerFrame;
		*plFramesPerSecond = ( lAvgFrameDuration != 0 ) ? ((long)( nTenMillion / lAvgFrameDuration )) : 0;
	}
}


BOOL CCompressedFileVideoSource::GetBitmapInfo(BITMAPINFOHEADER &bm, DWORD &fps)
{
	if (m_pSampleGrabber == NULL) return FALSE;

	HRESULT			hr;
	AM_MEDIA_TYPE	mt;
	ZeroMemory(&mt, sizeof(mt));

	hr = m_pSampleGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr)) {
		return FALSE;
	}
	VIDEOINFOHEADER	*pHeader = (VIDEOINFOHEADER *) mt.pbFormat;
	if (pHeader == NULL) {
		return FALSE;
	}

	CopyMemory(&bm, &(pHeader->bmiHeader), sizeof(bm));
	fps = ( pHeader->AvgTimePerFrame != 0 ) ? ((long)( 10000000 / pHeader->AvgTimePerFrame )) : 0;
	
	CoTaskMemFree((VOID *) mt.pbFormat);
	return TRUE;
}



BOOL CCompressedFileVideoSource::BuildGraph(LPCTSTR pszFile)
{
	HRESULT hr;
	
	FIF(::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID *) &m_pGraphBuilder));
	
	IBaseFilter *pFilterFileSourceAsync = NULL;
	
	//Check ASF/WMV file
	if(_tcslen(pszFile) > 4){
		if( (_tcsicmp(&pszFile[_tcslen(pszFile)-4], _T(".WMV")) == 0) || (_tcsicmp(&pszFile[_tcslen(pszFile)-4], _T(".ASF")) == 0)){
			LIF(CoCreateInstance(CLSID_WMAsfReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilterFileSourceAsync)) ;
		}else{
			LIF(CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilterFileSourceAsync)) ;
		}
	}
	LIF(m_pGraphBuilder->AddFilter(pFilterFileSourceAsync, L"File Source (Async.)"));

	
	IBaseFilter *pFilterSplitter = NULL;
	if (_tcslen(pszFile) > 4 && _tcsicmp(&pszFile[_tcslen(pszFile)-4], _T(".AVI")) == 0) {
		LIF(CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilterSplitter)) ;
		LIF(m_pGraphBuilder->AddFilter(pFilterSplitter, L"AVI Splitter"));
	} else if (_tcslen(pszFile) > 4 && _tcsicmp(&pszFile[_tcslen(pszFile)-4], _T(".VOB")) == 0) {
		LIF(CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilterSplitter)) ;
		LIF(m_pGraphBuilder->AddFilter(pFilterSplitter, L"AVI Splitter"));
	}

	// Create Sample grabber filter
	IBaseFilter *pGrabber;
	FIF(::CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *) &pGrabber));
	FIF(pGrabber->QueryInterface(IID_ISampleGrabber, (LPVOID *) &m_pSampleGrabber));
	FIF(m_pGraphBuilder->AddFilter(pGrabber, L"SampleGrabber"));

	AM_MEDIA_TYPE	mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype	= MEDIATYPE_Video;
	mt.subtype		= MEDIASUBTYPE_YUY2;
	mt.formattype	= FORMAT_VideoInfo;
	FIF(m_pSampleGrabber->SetMediaType(&mt));
#ifdef SG_CALLBACK
	FIF(m_pSampleGrabber->SetBufferSamples(FALSE));
	FIF(m_pSampleGrabber->SetOneShot(FALSE));
	FIF(m_pSampleGrabber->SetCallback(&m_CB, 0));
#else
	FIF(m_pSampleGrabber->SetBufferSamples(TRUE));
	FIF(m_pSampleGrabber->SetOneShot(TRUE));
#endif	

	// Create NullRenderer
	IBaseFilter *pFilterNullRenderer = NULL;
    FIF(CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilterNullRenderer)) ;
	FIF(m_pGraphBuilder->AddFilter(pFilterNullRenderer, L"NullRenderer"));

	// Load source
	IFileSourceFilter *pFSF = NULL;
	//WCHAR	name[MAX_PATH];
	
	//MultiByteToWideChar(CP_ACP, 0, pszFile, -1, name, MAX_PATH);
	FIF(pFilterFileSourceAsync->QueryInterface(IID_IFileSourceFilter, (void **)&pFSF));
	if (pFSF != NULL)
	{
		LIF(pFSF->Load(pszFile, NULL));
	}
	SAFE_RELEASE(pFSF);
	
		
	// Connect Filters
	if (pFilterSplitter) {
		FIF(ConnectFilters(m_pGraphBuilder, pFilterFileSourceAsync, pFilterSplitter));
		FIF(ConnectFilters(m_pGraphBuilder, pFilterSplitter, pGrabber));
	} else {
		FIF(ConnectFilters(m_pGraphBuilder, pFilterFileSourceAsync, pGrabber));
	}
	FIF(ConnectFilters(m_pGraphBuilder, pGrabber, pFilterNullRenderer));

	// Release filters
	SAFE_RELEASE(pFilterFileSourceAsync);
	SAFE_RELEASE(pFilterSplitter);
	SAFE_RELEASE(pGrabber);
	SAFE_RELEASE(pFilterNullRenderer);

	// Set time format, etc
	FIF(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *) &m_pMediaControl));
	FIF(m_pMediaControl->QueryInterface(IID_IMediaSeeking, (LPVOID *) &m_pMediaSeeking));
	FIF(m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (VOID **) &m_pMediaEvent));

	FIF(m_pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME));
	FIF(m_pMediaSeeking->GetDuration(&m_PlayDuration));
	m_DummyPlayPostion = 0;

#ifdef _DEBUG
    hr = AddGraphToRot(m_pGraphBuilder, &m_uGraphRegister) ;
    if (FAILED(hr))
    {
        ERR(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr) ;
        m_uGraphRegister = 0 ;
    }
#endif
	return TRUE;
}

VOID CCompressedFileVideoSource::DestroyGraph()
{
#if defined(_DEBUG)
	if (m_uGraphRegister) {
		RemoveGraphFromRot(m_uGraphRegister);
	}
#endif
	SAFE_RELEASE(m_pSampleGrabber);
	SAFE_RELEASE(m_pMediaSeeking);
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pMediaEvent);
	SAFE_RELEASE(m_pGraphBuilder);
}


void CCompressedFileVideoSource::GetNextTimeStamp(FILETIME *timestamp)
{
	m_StartTimestamp += ((1000*1000*10)/m_FPS);
	memcpy(timestamp, &m_StartTimestamp, sizeof(FILETIME));
}


BOOL CCompressedFileVideoSource::GetRawImage(BYTE **pRawData, FILETIME *timestamp, DWORD *bEOF )
{
	if (pRawData == NULL) return FALSE;
	if (m_bOpened == FALSE) return FALSE;

#ifdef SG_CALLBACK
	HRESULT hr;

		
	if (0 != m_DummyPlayPostion) { // return dummy frames
		if (100 < m_DummyPlayPostion) {
			REFERENCE_TIME pos = 0;
			FIF(m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning));
			FIF(m_pMediaControl->Run());
			*bEOF = 0;
			m_DummyPlayPostion = 0;
			return FALSE;

		} else {

			*pRawData = m_pRawData;
			*bEOF = 0;
			m_DummyPlayPostion++;

			Sleep(1000/m_FPS);
			SetEvent( m_hQueueEvent );
			GetNextTimeStamp(timestamp);

			return TRUE;
		}

	} else {
		long code;
		LONG_PTR param1, param2;

		// Is the graph completed?
		while (SUCCEEDED(m_pMediaEvent->GetEvent(&code, &param1, &param2, 0))) {
			m_pMediaEvent->FreeEventParams(code, param1, param2);

			if (EC_COMPLETE == code || EC_USERABORT == code) {
				ZeroMemory(m_pRawData, m_BM.biHeight*m_BM.biWidth*4);
				*pRawData = m_pRawData;
				*bEOF = 0;
				m_DummyPlayPostion++;

				
				SetEvent( m_hQueueEvent );
				GetNextTimeStamp(timestamp);
				return TRUE;
			}
		}

		// return frames
//		DWORD obj = WaitForSingleObject(m_hQueueEvent, 200);
//		if (WAIT_OBJECT_0 == obj) {
			CopyMemory(&m_pRawData[0], m_pBytes, m_iLength);
			*pRawData = m_pRawData;
			*bEOF = 0;
			GetNextTimeStamp(timestamp);

			return TRUE;
//		} else {
//			*bEOF = 0;
//			return FALSE;
//		}
		
	}
		
#else
	if (m_PlayPosition >= m_PlayDuration) {
		if (m_PlayPosition == m_PlayDuration) {
			ZeroMemory(m_pRawData, m_BM.biHeight*m_BM.biWidth*2);
		}

		if (m_PlayPosition >= m_EndDuration) {
			*bEOF = 1;
			m_PlayPosition = 0;
			return FALSE;
		} else {
			*pRawData = m_pRawData;
			*bEOF = 0;
			m_PlayPosition++;
			return TRUE;
		}
	}

	BOOL	rs = TRUE;
	HRESULT hr;
	long	length;
	long	code;
	
	
	FIF(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *) &m_pMediaControl));
	FIF(m_pMediaControl->QueryInterface(IID_IMediaSeeking, (LPVOID *) &m_pMediaSeeking));
	FIF(m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *) &m_pMediaEvent));

	// Get a frame
	REFERENCE_TIME StopPostion = m_PlayPosition;
	do {
		hr = m_pMediaSeeking->SetPositions(&m_PlayPosition, AM_SEEKING_AbsolutePositioning|AM_SEEKING_SeekToKeyFrame, 
			&m_PlayPosition, AM_SEEKING_AbsolutePositioning|AM_SEEKING_SeekToKeyFrame);
		hr = m_pMediaControl->Run();
		if (FAILED(hr)) {
			ERR("CCompressedFileVideoSource::GetRawImage, Error at IMediaControl::Run()\n");
			return FALSE;
		}

		hr = m_pMediaEvent->WaitForCompletion(INFINITE, &code);
		TRACE("Play Position: %u, Stop Position: %u\n", m_PlayPosition, StopPostion);
		hr = m_pSampleGrabber->GetCurrentBuffer(&length, NULL);	
		if (S_OK == hr) {
			break;
		} else if (VFW_E_WRONG_STATE == hr) {
			++StopPostion;
		} else {
			rs = FALSE;
			goto exit;
		}
	} while (TRUE);

	hr = m_pSampleGrabber->GetCurrentBuffer(&length, (long *) m_pRawData);
	if (FAILED(hr)) {
		ERR("CCompressedFileVideoSource::GetRawImage, Error at IMediaEvent::GetCurrentBuffer()\n");
		return FALSE;
	}

	*pRawData = m_pRawData;
	*bEOF = 0;

	m_PlayPosition++;

exit:
	SAFE_RELEASE(m_pMediaSeeking);
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pMediaEvent);
	return rs;
#endif
}

BOOL CCompressedFileVideoSource::ReleaseRawImage()
{
	SetEvent(m_hDequeueEvent);
	return TRUE;
}
	
BOOL CCompressedFileVideoSource::Control(IVCAVideoSource::eCMD cmd, DWORD_PTR param1, DWORD_PTR param2)
{
	switch(cmd){
		case CMD_SET_VIDEOFORMAT:
		case CMD_SET_COLORFORMAT:
		case CMD_SET_IMGSIZE:
			return TRUE;

		case CMD_SET_FRAMERATE:
			if(0 != param1) m_FPS = (DWORD)param1;
			return TRUE;
	
		case CMD_GET_COLORFORMAT:
			(*(DWORD *) param1) = VCA5_COLOR_FORMAT_YUY2;
			return TRUE;

		case CMD_GET_IMGSIZE:{
			(*(DWORD *) param1) = VCA5_MAKEIMGSIZE(m_BM.biWidth, m_BM.biHeight);
			return TRUE;
		 }

		case CMD_GET_FRAMERATE:
			(*(DWORD *) param1) = m_FPS;
			return TRUE;
		
		case CMD_GET_VIDEOFORMAT:
			(*(DWORD *) param1) = VCA5_VIDEO_FORMAT_NTSC_M;
			return TRUE;
		
		default:
			return FALSE;
	}
}
