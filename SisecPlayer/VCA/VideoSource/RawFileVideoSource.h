// RawFileVideoSource.h: interface for the CRawFileVideoSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAWFILEVIDEOSOURCE_H__88592B80_ADEE_4847_8DC8_03475EEDF54A__INCLUDED_)
#define AFX_RAWFILEVIDEOSOURCE_H__88592B80_ADEE_4847_8DC8_03475EEDF54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VCAVideoSource.h"
#include "YUVRec.h"
#define DUMMY_DATA_OUT	100

class CRawFileVideoSource  : public IVCAVideoSource
{
public:
	CRawFileVideoSource():m_bOpen(FALSE), m_ReadIndex(0), m_pData(NULL){}
	virtual ~CRawFileVideoSource(){Close();}

	BOOL	Open(LPCTSTR pszFile, DWORD param);
	void	Close();
	BOOL	Start(){return TRUE;}
	void	Stop(){return;}

	BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2);
	BOOL	GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF );
	BOOL	ReleaseRawImage(){return TRUE;}

	eSOURCETYPE GetSourceType() {return RAWFILESOURCE;}

	HANDLE	GetEvent(){return m_hEvent;}
private:

	BOOL	m_bOpen;
	DWORD	m_ReadIndex;
	BYTE	*m_pData;
	
	DWORD	m_DummyIndex;
	CYUVRec	m_YUVReader;

	DWORD	m_FPS;
	HANDLE	m_hEvent;

};


#endif // !defined(AFX_RAWFILEVIDEOSOURCE_H__88592B80_ADEE_4847_8DC8_03475EEDF54A__INCLUDED_)
