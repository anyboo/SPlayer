// CompressedFileVideoSource.h: interface for the CCompressedFileVideoSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPRESSEDFILEVIDEOSOURCE_H__BA79882A_3E4C_4CB7_B782_B2F7630C133C__INCLUDED_)
#define AFX_COMPRESSEDFILEVIDEOSOURCE_H__BA79882A_3E4C_4CB7_B782_B2F7630C133C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VCAVideoSource.h"
#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>

#define SG_CALLBACK

class CCompressedFileVideoSource;

class CSampleGrabberCB : public ISampleGrabberCB
{
public:
	CSampleGrabberCB(CCompressedFileVideoSource *pParent) {m_pParent = pParent;}

	// IUnknown implementation
	STDMETHODIMP_(ULONG) AddRef() {return 2;}
	STDMETHODIMP_(ULONG) Release() {return 1;}
	STDMETHODIMP QueryInterface(REFIID riid, VOID **ppv) {
		if (IID_ISampleGrabber == riid || IID_IUnknown == riid) {
			*ppv = (VOID *) static_cast<ISampleGrabberCB *>(this);
			return S_OK;
		} else {
			return E_NOINTERFACE;
		}
	}

	STDMETHODIMP SampleCB(double time, IMediaSample *pSample);
	STDMETHODIMP BufferCB(double time, BYTE *pBuffer, long iLength);

protected:
	CCompressedFileVideoSource *m_pParent;
};

class CCompressedFileVideoSource : public IVCAVideoSource
{
	friend CSampleGrabberCB;
public:
	CCompressedFileVideoSource();
	virtual ~CCompressedFileVideoSource();
	
	virtual BOOL 	Open(LPCTSTR szFile, DWORD param);
	virtual VOID	Close();
	virtual BOOL	Start();
	virtual VOID	Stop();

	virtual BOOL	Control(IVCAVideoSource::eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);

	virtual BOOL	GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF );
	virtual BOOL	ReleaseRawImage();

	virtual	eSOURCETYPE GetSourceType() {return IVCAVideoSource::COMPRESSEDFILESOURCE;}

	virtual HANDLE GetEvent() { return m_hQueueEvent; }

protected:
	BOOL			m_bOpened;

	IGraphBuilder	*m_pGraphBuilder;
	ISampleGrabber	*m_pSampleGrabber;
	IMediaControl	*m_pMediaControl;
	IMediaSeeking	*m_pMediaSeeking;
	IMediaEvent		*m_pMediaEvent;

	BITMAPINFOHEADER	m_BM;
	REFERENCE_TIME		m_PlayPosition;
	REFERENCE_TIME		m_PlayDuration;
	REFERENCE_TIME		m_DummyPlayPostion;

	BYTE	*m_pRawData;

	BOOL	BuildGraph(LPCTSTR szFile);
	VOID	DestroyGraph();
	BOOL	GetBitmapInfo(BITMAPINFOHEADER &bm, DWORD &fps);

	DWORD	m_FPS;

	CSampleGrabberCB m_CB;
	HANDLE			m_hEvent;
#ifdef SG_CALLBACK
	BYTE		*m_pBytes;
	LONG		m_iLength;
	HANDLE		m_hQueueEvent;
	HANDLE		m_hDequeueEvent;
#endif
	
	void		GetNextTimeStamp(FILETIME *timestamp);
	__int64 	m_StartTimestamp;
#if defined(_DEBUG)
	// For graph spy
	DWORD	m_uGraphRegister;
#endif
};

#endif // !defined(AFX_COMPRESSEDFILEVIDEOSOURCE_H__BA79882A_3E4C_4CB7_B782_B2F7630C133C__INCLUDED_)
