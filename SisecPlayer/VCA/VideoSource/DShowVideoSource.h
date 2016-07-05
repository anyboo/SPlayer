#pragma once
#include "vcavideosource.h"
#include "DshowCap.h"
#include <atlstr.h>



class CImageDataQueue;

class CDShowVideoSource :
	public IVCAVideoSource
{
public:
	CDShowVideoSource(void);
	virtual ~CDShowVideoSource(void);


	//-------------------------------------------------------
	// IVCAVideoSource
	BOOL	Open( LPCTSTR lpszFile, DWORD dwParam );
	void	Close();
	BOOL	Start();
	void	Stop();
	HANDLE	GetEvent() { return m_hEvent; }

	BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);
	BOOL	GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF);
	BOOL	ReleaseRawImage();

	eSOURCETYPE GetSourceType() {return DSHOWSOURCE;}

private:
	//-------------------------------------------------------
	static BOOL __stdcall DShowCbFunc( BITMAPINFOHEADER &bih, unsigned char *pData, unsigned int uiLen, __int64 timeStamp, void *pUserData );
	void OnNewFrame( BITMAPINFOHEADER &bih, unsigned char *pData, unsigned int uiLen, __int64 timeStamp );

private:

	unsigned char		*m_pAlignedImg;

	CString				m_sFilename;

	HANDLE				m_hEvent;

	BOOL				m_bOpen;
	DShowCap			*m_pCap;
	BITMAPINFOHEADER	m_bih;
//	BOOL				m_bGotOneFrame;
	int					m_iDeviceIdx;
	int					m_iMediaFormatIdx;

	CImageDataQueue*	m_pQueue;

	CRITICAL_SECTION	m_csBufLock;
};
