#pragma once

#include "VCAVideoSource.h"
#include "ImageDataQueue.h"

class CCap5System;
class CCAP5VideoSource : public IVCAVideoSource
{
public:
	CCAP5VideoSource(void);
	~CCAP5VideoSource(void);

	BOOL	Open(LPCTSTR pszFile, DWORD param);
	void	Close();
	BOOL	Start(){return TRUE;}
	void	Stop(){}
	BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);

	BOOL	GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF );
	BOOL	ReleaseRawImage(){return m_pQueue->Remove();}

	eSOURCETYPE GetSourceType(){return CAP5SOURCE;}

	BOOL	InsertImage(BYTE *pRAWData, FILETIME timestamp, DWORD len);

	HANDLE	GetEvent(){return m_hEvent;}

private:
	CCap5System*	m_pCAP5System;
	CImageDataQueue* m_pQueue;
	DWORD			m_BdId, m_ChId;
	HANDLE			m_hEvent;
};
