#pragma once
#include "VCAVideoSource.h"
#include <queue>

interface IVCAMediaSample;

class CVirtualVideoSource :
	public IVCAVideoSource
{
public:
	CVirtualVideoSource(void);
	virtual ~CVirtualVideoSource(void);


	//-------------------------------------------------------
	// IVCAVideoSource
	BOOL	Open( LPCTSTR lpszFile, DWORD dwParam );
	void	Close();
	BOOL	Start();
	void	Stop();
	HANDLE	GetEvent() { return m_hEvent; }
	//--------------------------------------------------------

	BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);
	BOOL	GetRawImage(BYTE **pRawData, FILETIME *timestamp, DWORD *bEOF);
	BOOL	ReleaseRawImage();

	BOOL	GetBufferLen( unsigned int *puiMaxLen, unsigned int *puiCurLen );

	eSOURCETYPE GetSourceType() {return VIRTUALSOURCE;}

protected:
	void OnNewFrame( IVCAMediaSample *pSamp );

//	static UINT StreamingThreadStub( LPVOID lpThis );
//	void StreamingThread();

protected:
	HANDLE	m_hEvent;
	HANDLE	m_hNewFrameEvent;
	CRITICAL_SECTION	m_csLock;
	std::queue< IVCAMediaSample *> m_samples;
	IVCAMediaSample		*m_pCurSample;

	BOOL				m_bOpen;

	BITMAPINFOHEADER	m_bih;

	HANDLE				m_hEndStreamThread;
	HANDLE				m_hStreamThreadEnded;

};
