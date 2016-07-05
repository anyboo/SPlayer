#pragma once
#include "VCAVideoSource.h"
#include "AviDemux.h"


class CStreamVideoSource :
	public IVCAVideoSource, public AviDemux, public AviDemuxCallback
{
public:
	CStreamVideoSource(void);
	virtual ~CStreamVideoSource(void);


	//-------------------------------------------------------
	// IVCAVideoSource
	BOOL	Open( LPCTSTR lpszFile, DWORD dwParam );
	void	Close();
	BOOL	Start();
	void	Stop();
	HANDLE	GetEvent() { return m_hEvent; }

	BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);
	BOOL	GetRawImage(BYTE **pRawData, FILETIME *timestamp, DWORD *bEOF);
	BOOL	ReleaseRawImage();

	eSOURCETYPE GetSourceType() {return STREAMSOURCE;}

	//-------------------------------------------------------
	// AviDemuxCallback
	virtual void OnMainHeader( AVIMAINHEADER &avimh, void *pUserData );
	virtual void OnStreamHeader( AVISTREAMHEADER &avish, void *pUserData );
	virtual void OnStreamFormat( unsigned char *pFmtBuf, unsigned int uiLen, void *pUserData );
	virtual void OnNewFrame( DWORD dwFourCC, unsigned char *pBuf, unsigned int uiLen, void *pUserData );
	virtual void OnStreamEnd( void *pUserData );

private:
	static	DWORD	WINAPI StreamThreadStub(LPVOID pParam);
	void StreamThread();
	BOOL Pump();
	void CleanupChild();

private:
	BOOL	m_bOpen;

	PROCESS_INFORMATION	m_piStreamer;
	HANDLE	m_hChildStdOut_R, m_hChildStdOut_W;
	HANDLE	m_hThreadEndEvent;

	AVIMAINHEADER		*m_pAvimh;
	AVISTREAMHEADER		*m_pAvish;
	BITMAPINFOHEADER	*m_pAvibih;

	BOOL				m_bVideoFmtComing;
	BOOL				m_bGotMainHeader;
	BOOL				m_bGotVideoFrame;

	unsigned char		*m_pAlignedImg;

	CString				m_sFilename;

	HANDLE				m_hEvent;
	BOOL				m_bRun;
	unsigned int		m_uiFrameCount;

	CRITICAL_SECTION	m_csBufLock;

};
