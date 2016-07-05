#pragma once

#define __D3DRM_H__

#include <qedit.h>
#include <dshow.h>
#include "wxdebug.h"
#include "combase.h"


typedef BOOL (__stdcall *DShowCapCallback)(BITMAPINFOHEADER &bih, unsigned char *pData, unsigned int uiLen, __int64 iTimestamp, void *pUserData );

#define MAX_DEVICE_LEN 256
#define MAX_DEVICES 64
#define MAX_NUM_FORMATS 64

struct CAPDEVICE
{
	int iIndex;
	TCHAR tszName[ MAX_DEVICE_LEN ];
	int iNumFormats;
	VIDEOINFOHEADER	viFormats[ MAX_NUM_FORMATS ];
	void	*lpReserved;
};

class DShowCap : public ISampleGrabberCB
{

public:
	DShowCap(void);
	~DShowCap(void);
	
	HRESULT RegisterCallback( DShowCapCallback cbFunc, void *pUserData );
	HRESULT StartCap( int iDeviceIdx, int iFormatIdx );
	HRESULT StartURI( LPCTSTR lpszURI );
	HRESULT StopCap();
	HRESULT GetCapDevices( CAPDEVICE *pDevices, int &iNumDevices );

public:
	virtual ULONG STDMETHODCALLTYPE AddRef() { return ++m_cRef; }
	virtual ULONG STDMETHODCALLTYPE Release() { return --m_cRef; }
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );

protected:
	HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION ePinDir, IPin **ppPin);
	HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath);
	HRESULT Run();
	HRESULT EnumDevices();
	HRESULT DeleteAllFilters();

	HRESULT FixupGraph();

protected:
	virtual HRESULT STDMETHODCALLTYPE BufferCB( double SampleTime, BYTE *pBuffer,long BufferLen ) {ASSERT( false ); return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE SampleCB( double SampleTime, IMediaSample *pSample );

protected:
	DShowCapCallback		m_cbFunc;

	IGraphBuilder			*m_pGraph;
	ICaptureGraphBuilder2	*m_pCapBuilder;
	IBaseFilter				*m_pGrabberFilter;
	IBaseFilter				*m_pRenderer;
	ISampleGrabber			*m_pSampleGrabber;
	IMediaControl			*m_pControl;

	IBaseFilter				*m_pSrcFilter;
	IBaseFilter				*m_pVideoRenderer;
	IBaseFilter				*m_pNullRenderer;
	IVMRMixerBitmap			*m_pMixerBitmap;
	IVMRFilterConfig		*m_pVMRFilterConfig;
	IBaseFilter				*m_pColourConverter;
	IBaseFilter				*m_pPinTeeFilter;
	IMediaEvent				*m_pMediaEvent;
	IMediaSeeking			*m_pSeeking;
	IReferenceClock			*m_pClock;
	IMediaFilter			*m_pMediaFilter;
	IVideoWindow			*m_pVidWin;
	IBasicVideo				*m_pBasicVid;

	ULONG					m_cRef;
	void					*m_pUserData;

	CAPDEVICE				m_Devices[ MAX_DEVICES ];
	int						m_iNumDevices;

};
