#include "StdAfx.h"
#include "DShowCap.h"
#pragma warning( disable : 4430 )

#include "streams.h"

#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))

DShowCap::DShowCap(void) 
{
	int i = 100;
	m_cRef = 1;

	m_pSampleGrabber = NULL;
	m_pSrcFilter = NULL;
	m_pNullRenderer = NULL;

	m_cbFunc = NULL;

	HRESULT hr = E_FAIL;

	CoInitialize( NULL );


	// Create an instance of the graphbuilder
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
							IID_IGraphBuilder, (void**) &m_pGraph );

	if( SUCCEEDED( hr ) )
	{
		// Get the IMediaControl interface
		hr = m_pGraph->QueryInterface( IID_IMediaControl, (void**) &m_pControl );
	}

	if( SUCCEEDED( hr ) )
	{
		hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
								IID_ICaptureGraphBuilder2, (void **)&m_pCapBuilder );
	}

	if( SUCCEEDED( hr ) )
	{
		hr = m_pCapBuilder->SetFiltergraph( m_pGraph );
	}

	EnumDevices();

}

DShowCap::~DShowCap(void)
{
	for( int i = 0; i < m_iNumDevices; i++ )
	{
		((IUnknown *)m_Devices[i].lpReserved)->Release();
	}

	DeleteAllFilters();

	m_pGraph->Release();
}

HRESULT DShowCap::RegisterCallback( DShowCapCallback cbFunc, void *pUserData )
{
	m_cbFunc = cbFunc;
	m_pUserData = pUserData;

	return S_OK;
}

HRESULT DShowCap::EnumDevices()
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	m_iNumDevices = 0;
	memset( m_Devices, 0, MAX_DEVICES * sizeof( CAPDEVICE ) );


	// Find out what we've got...
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;

	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
							IID_ICreateDevEnum, (void **)&pDevEnum );

	if( SUCCEEDED( hr ) )
	{
		hr = pDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnum, 0 );
	}

	if( SUCCEEDED( hr ) && pEnum )
	{
		// Enumerate through all the monikers...
		IMoniker *pMoniker = NULL;
		while( pEnum->Next( 1, &pMoniker, NULL ) == S_OK )
		{
			IPropertyBag *pPropBag;
		
			hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pPropBag );

			if( FAILED( hr ) )
			{
				pMoniker->Release();
				continue;
			}

			// Get friendly name...
			VARIANT varName;
			VariantInit( &varName );
			hr = pPropBag->Read( L"Description", &varName, 0 );

			if( FAILED( hr ) )
			{
				// Try friendly name instead
				hr = pPropBag->Read( L"FriendlyName", &varName, 0 );
			}

			if( SUCCEEDED( hr ) )
			{
				IBaseFilter *pSrcFilter = NULL;
				IAMStreamConfig *pStreamConfig = NULL;
				int iNumCaps;
				int iSize;
				CAPDEVICE *pCapDevice = &m_Devices[ m_iNumDevices ];

				memset( pCapDevice, 0, sizeof( CAPDEVICE ) );

				TCHAR s[256];
				_stprintf_s( s, _countof(s), _T("Found video capture: %s\n"), OLE2T( varName.bstrVal ) );
				OutputDebugString(s);

				pCapDevice->lpReserved = (void *)pMoniker;
				_stprintf_s( m_Devices[ m_iNumDevices ].tszName, _countof(pCapDevice->tszName), OLE2T( varName.bstrVal ) );

				VariantClear( &varName );

				// Find out the supported capabilities...
				pSrcFilter = NULL;
				hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void **)&pSrcFilter );

				if( SUCCEEDED( hr ) )
				{
					hr = m_pGraph->AddFilter( pSrcFilter, L"CapFilter" );
				}

				if( SUCCEEDED( hr ) )
				{
					hr = m_pCapBuilder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSrcFilter, IID_IAMStreamConfig, (void **)&pStreamConfig );
				}

				if( SUCCEEDED( hr ) )
				{
					hr = pStreamConfig->GetNumberOfCapabilities( &iNumCaps, &iSize );
				}

				BYTE *pFormats = new BYTE[ iSize ];
				if(iNumCaps > MAX_NUM_FORMATS) iNumCaps = MAX_NUM_FORMATS;

				if( SUCCEEDED( hr ) )
				{
					for( int i = 0; i < iNumCaps; i++ )
					{
						AM_MEDIA_TYPE *pmt = NULL;

						hr = pStreamConfig->GetStreamCaps( i, &pmt, pFormats );

						if( SUCCEEDED( hr ) )
						{
							VIDEOINFOHEADER *pHdr = (VIDEOINFOHEADER *)pmt->pbFormat;

							memcpy( &pCapDevice->viFormats[ pCapDevice->iNumFormats ], pHdr, sizeof( VIDEOINFOHEADER ) );

							pCapDevice->iNumFormats++;
						}

						if( pmt )
						{
							DeleteMediaType( pmt );
						}
					}
				}

				delete [] pFormats;

				if( pStreamConfig )
				{
					pStreamConfig->Release();
				}


				if( pSrcFilter )
				{
					m_pGraph->RemoveFilter( pSrcFilter );
					pSrcFilter->Release();
					pSrcFilter = NULL;
				}

				m_Devices[ m_iNumDevices ].iIndex = m_iNumDevices++;
			}

			pPropBag->Release();
	//		pMoniker->Release();
		}
	}

	return hr;
}

HRESULT DShowCap::GetCapDevices( CAPDEVICE *pDevices, int &iNumDevices )
{
	HRESULT hr = S_OK;

	memcpy( pDevices, m_Devices, m_iNumDevices * sizeof( CAPDEVICE ) );
	iNumDevices = m_iNumDevices;

	return hr;
}

HRESULT DShowCap::StartCap( int iDeviceIdx, int iFormatIdx )
{
	USES_CONVERSION;
	HRESULT hr = E_FAIL;

	if( (iDeviceIdx < 0) || (iDeviceIdx >= m_iNumDevices) )
	{
		return hr;
	}

	if( (iFormatIdx < 0) || (iFormatIdx >= m_Devices[iDeviceIdx].iNumFormats ) )
	{
		return hr;
	}

	DeleteAllFilters();

	// Bind to the Moniker
	IMoniker *pMoniker = (IMoniker *)m_Devices[iDeviceIdx].lpReserved;
	hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void **)&m_pSrcFilter );

	if( SUCCEEDED( hr ) )
	{
		hr = m_pGraph->AddFilter( m_pSrcFilter, L"CapFilter" );
	}

	// Set the media type
	if( SUCCEEDED( hr ) )
	{
		// Get the interface
		IAMStreamConfig *pStreamConfig = NULL;
		hr = m_pCapBuilder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pSrcFilter, IID_IAMStreamConfig, (void **)&pStreamConfig );

		if( SUCCEEDED( hr ) )
		{
			int iSize, iCount;
			hr = pStreamConfig->GetNumberOfCapabilities( &iCount, &iSize );

			if( SUCCEEDED( hr ) )
			{
				AM_MEDIA_TYPE *pmt = NULL;
				BYTE *pBuf = new BYTE[ iSize ];

				hr = pStreamConfig->GetStreamCaps( iFormatIdx, &pmt, pBuf );
				ASSERT( SUCCEEDED( hr ) );

				if( SUCCEEDED( hr ) )
				{
					hr = pStreamConfig->SetFormat( pmt );
					ASSERT( SUCCEEDED( hr ) );
				}

				if( pmt )
				{
					DeleteMediaType( pmt );
				}

				delete [] pBuf;
			}

			pStreamConfig->Release();
			pStreamConfig = NULL;
		}
	}

	// Add a null Renderer
	{
		if( SUCCEEDED( hr ) )
		{
			hr = CoCreateInstance( CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
									IID_IBaseFilter, (void **)&m_pNullRenderer );
		}

		if( SUCCEEDED( hr ) )
		{
			m_pGraph->AddFilter( m_pNullRenderer, L"Null Renderer" );
		}

	}


	SaveGraphFile( m_pGraph, L"D:\\thegraph2.grf" );
	// Render the stream
	if( SUCCEEDED( hr ) )
	{
		hr = m_pCapBuilder->RenderStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pSrcFilter, 0, m_pNullRenderer );
	}

	hr = FixupGraph();

	// Now start it running
	if( SUCCEEDED( hr ) )
	{
		hr = Run();
	}

	return hr;
}

HRESULT DShowCap::StartURI( LPCTSTR lpszURI )
{
	HRESULT hr;

	DeleteAllFilters();

	hr = m_pGraph->AddSourceFilter( lpszURI, L"SrcFilter", &m_pSrcFilter );

	// Add a null Renderer
	{
		if( SUCCEEDED( hr ) )
		{
			hr = CoCreateInstance( CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
									IID_IBaseFilter, (void **)&m_pNullRenderer );
		}

		if( SUCCEEDED( hr ) )
		{
			m_pGraph->AddFilter( m_pNullRenderer, L"Null Renderer" );
		}

	}

	// Render the stream
	if( SUCCEEDED( hr ) )
	{
		IPin *pSrcPin = NULL;
		hr = GetPin( m_pSrcFilter, PINDIR_OUTPUT, &pSrcPin );

		if( SUCCEEDED( hr ) )
		{
			hr = m_pGraph->Render( pSrcPin );

			pSrcPin->Release();
		}
	}

	if( SUCCEEDED( hr ) )
	{
		hr = FixupGraph();
	}

	if( SUCCEEDED( hr ) )
	{
		hr = Run();
	}

	return hr;
}

HRESULT DShowCap::FixupGraph()
{
	HRESULT hr;

	// Now insert a sample grabber just before the video renderer so we can provide callbacks

	// First of all, create it
	hr = CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
							IID_IBaseFilter, (void**) &m_pGrabberFilter );

	if( SUCCEEDED( hr ) )
	{
		hr = m_pGrabberFilter->QueryInterface( IID_ISampleGrabber, (void**) &m_pSampleGrabber );
	}

	AM_MEDIA_TYPE mediaType;

	if( SUCCEEDED( hr ) )
	{
		// Set the media type to be YUY2

		memset( &mediaType, NULL, sizeof( AM_MEDIA_TYPE ) );

		mediaType.majortype		= MEDIATYPE_Video;
//		mediaType.subtype		= MEDIASUBTYPE_YUY2;//RGB24;
//		mediaType.formattype	= GUID_NULL;

		hr = m_pSampleGrabber->SetMediaType( &mediaType );
	}

	if( SUCCEEDED( hr ) )
	{
		// Put this filter into the filter graph
		hr = m_pGraph->AddFilter( m_pGrabberFilter, L"Sample Grabber" );
	}

	// Set the callback to me myself
	if( SUCCEEDED( hr ) )
	{
		hr = m_pSampleGrabber->SetCallback( this, 0 );	// Pass me back the original sample
	}


	// Now disconnect the capture filter output pin and stick the sample grabber in between
	{
		IPin *pUpstream, *pDownstream;
		IPin *pIn, *pOut;

		hr = GetPin( m_pGrabberFilter, PINDIR_INPUT, &pIn );
		hr = GetPin( m_pGrabberFilter, PINDIR_OUTPUT, &pOut );
		hr = GetPin( m_pSrcFilter, PINDIR_OUTPUT, &pUpstream );
		hr = pUpstream->ConnectedTo( &pDownstream );

		hr = pUpstream->Disconnect();
		hr = pDownstream->Disconnect();

		hr = m_pGraph->Connect( pUpstream, pIn );
		hr = m_pGraph->Connect( pOut, pDownstream );
	}


	SaveGraphFile( m_pGraph, L"D:\\thegraph9.grf" );

	return hr;
}


HRESULT DShowCap::GetPin(IBaseFilter *pFilter, PIN_DIRECTION ePinDir, IPin **ppPin)
{
	IEnumPins* pEnum	= NULL;
	IPin* pPin			= NULL;
	HRESULT hr			= E_UNEXPECTED;

	if( NULL == ppPin )
	{
		return E_POINTER;
	}

	hr = pFilter->EnumPins( &pEnum );

	if( FAILED( hr ) )
	{
		return hr;
	}

	while( pEnum->Next( 1, &pPin, 0 ) == S_OK )
	{
		PIN_DIRECTION eThisPinDir;
		hr = pPin->QueryDirection( &eThisPinDir );

		if( FAILED( hr ) )
		{
			pPin->Release();
			pEnum->Release();
			return hr;
		}

		if( ePinDir == eThisPinDir )
		{
			// Found the right one
			*ppPin = pPin;
			pEnum->Release();
			return S_OK;
		}

		// Release the pin for use next time round
		pPin->Release();
	}

	// No more pins - no match
	pEnum->Release();

	return E_FAIL;
}

HRESULT DShowCap::DeleteAllFilters( )
{
	HRESULT hr = E_FAIL;
	IEnumFilters *pEnum = NULL;

	hr = m_pGraph->EnumFilters( &pEnum );

	if( SUCCEEDED( hr ) )
	{
		IBaseFilter *pFilter = NULL;
		ULONG ulFetched = 0;
		hr = pEnum->Next( 1, &pFilter, &ulFetched );

		while( ulFetched )
		{
			FILTER_INFO info;
			pFilter->QueryFilterInfo( &info );
			TRACE( _T("DELETING FILTER %s\n"), info.achName );


			m_pGraph->RemoveFilter( pFilter );
			pFilter->Release();

			hr = pEnum->Reset();
			hr = pEnum->Next( 1, &pFilter, &ulFetched );
		}

	}

	if( m_pSampleGrabber )	m_pSampleGrabber->Release();
	if( m_pSrcFilter )		m_pSrcFilter->Release();
	if( m_pNullRenderer )	m_pNullRenderer->Release();

	m_pSampleGrabber	= NULL;
	m_pSrcFilter		= NULL;
	m_pNullRenderer		= NULL;

	return hr;
}

HRESULT DShowCap::SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) 
{
    const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
    HRESULT hr;
    
    IStorage *pStorage = NULL;
    hr = StgCreateDocfile(
        wszPath,
        STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
        0, &pStorage);
    if(FAILED(hr)) 
    {
        return hr;
    }

    IStream *pStream;
    hr = pStorage->CreateStream(
        wszStreamName,
        STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
        0, 0, &pStream);
    if (FAILED(hr)) 
    {
        pStorage->Release();    
        return hr;
    }

    IPersistStream *pPersist = NULL;
    pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
    hr = pPersist->Save(pStream, TRUE);
    pStream->Release();
    pPersist->Release();
    if (SUCCEEDED(hr)) 
    {
        hr = pStorage->Commit(STGC_DEFAULT);
    }
    pStorage->Release();
    return hr;
}

// CUnknown overrides
HRESULT STDMETHODCALLTYPE DShowCap::QueryInterface( REFIID riid, void **ppvObject )
{
	if( riid == IID_ISampleGrabberCB )
	{
		*ppvObject = (ISampleGrabberCB*)this;
		return S_OK;
	}

	return S_FALSE;
}

HRESULT DShowCap::Run()
{
	HRESULT hr = E_UNEXPECTED;

	if( m_pControl )
	{
		return m_pControl->Run();
	}

	return hr;
}

HRESULT DShowCap::StopCap()
{
	HRESULT hr = E_UNEXPECTED;

	if( m_pControl )
	{
		hr = m_pControl->Stop();
	}

	return hr;
}

HRESULT DShowCap::SampleCB( double SampleTime, IMediaSample *pSample )
{


	// Just pass this back to the client
	if( m_cbFunc )
	{
		unsigned char* pBytes;
		unsigned int uLength;

		pSample->GetPointer( &pBytes );
		uLength = pSample->GetActualDataLength( );

		// Find out the media type so we can pass this information back
		AM_MEDIA_TYPE mediaType;
		m_pSampleGrabber->GetConnectedMediaType( &mediaType );

		if( FORMAT_VideoInfo == mediaType.formattype )
		{
			VIDEOINFOHEADER* pHeader = (VIDEOINFOHEADER*) mediaType.pbFormat;

			switch( pHeader->bmiHeader.biCompression )
			{
				case FCC('YV12'):
				case FCC('YUY2'):
					break;

				case FCC('IYUV'):
				case FCC('I420'):
					pHeader->bmiHeader.biCompression = FCC('YV12');
					break;

			}

			__int64 startTime = 0;
			__int64 endTime = 0;
			HRESULT hr = pSample->GetMediaTime( &startTime, &endTime );

			if( FAILED( hr )  || (endTime  - startTime) < 1000)
			{
				// Duff timestamp, use a different method
				FILETIME 	filetime;
				GetSystemTimeAsFileTime(&filetime);
				FileTimeToLocalFileTime(&filetime, &filetime);

				startTime = filetime.dwHighDateTime;
				startTime <<= 32;
				startTime |= filetime.dwLowDateTime;
			}

//			TRACE( _T("CALLING BACK: TS=%I64d\n"), startTime );

			BOOL bOk = m_cbFunc( pHeader->bmiHeader, pBytes, uLength, startTime, m_pUserData );
		}
		else
		{
			// Unknown format
			ASSERT( false );
		}

		FreeMediaType( mediaType );
	}
	else
	{
		// Forget to set the sink pointer?
		ASSERT( false );
	}

//	pSample->Release();

	return S_OK;
}
