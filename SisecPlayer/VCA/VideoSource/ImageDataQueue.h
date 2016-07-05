// CircularQueue.h: interface for the CCircularQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCULARQUEUE_H__BF382596_11D6_45B9_BC15_642D5B0CB38C__INCLUDED_)
#define AFX_CIRCULARQUEUE_H__BF382596_11D6_45B9_BC15_642D5B0CB38C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#define IMAGESIZE	(720*(576+16)*2)

class CImageDataQueue
{
public:
	typedef struct{
		BYTE *		pData;
		FILETIME	TimeStamp;
		DWORD		len;
	} IMAGEDATA;

	CImageDataQueue(size_t size, size_t max_image, BOOL bForce=FALSE);
	~CImageDataQueue();

	BOOL Add(IMAGEDATA *pData);
	BOOL Remove();
	IMAGEDATA *Get();
	IMAGEDATA *Get(int index);
	BOOL IsFull(){
		return (m_nSize == m_nCount);
	}
	BOOL IsEmpty(){
		return (0 == m_nCount);	
	}
	size_t GetCount(){
		return m_nCount;
	}

private:
	IMAGEDATA *m_Datum;
	UCHAR *m_Images;

	CRITICAL_SECTION m_CS;
	size_t	m_nHead, m_nTail, m_nCount;
	size_t	m_MaxImageSize;
	
	BOOL m_bForce;
	const size_t m_nSize;
};

#endif // !defined(AFX_CIRCULARQUEUE_H__BF382596_11D6_45B9_BC15_642D5B0CB38C__INCLUDED_)
