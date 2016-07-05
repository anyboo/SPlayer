// CircularQueue.cpp: implementation of the CCircularQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ImageDataQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageDataQueue::CImageDataQueue(size_t size, size_t max_image, BOOL bForce) : m_nSize(size), m_bForce(bForce)
{
	InitializeCriticalSection(&m_CS); // InitializeCriticalSectionAndSpinlock
	m_nHead			= 0;
	m_nTail			= 0;
	m_nCount		= 0;
	m_MaxImageSize	= max_image + 16;
	m_Datum			= new IMAGEDATA[size];
	m_Images		= new UCHAR[size*m_MaxImageSize];
}

CImageDataQueue::~CImageDataQueue()
{
	delete [] m_Datum;
	delete [] m_Images;
	DeleteCriticalSection(&m_CS);
}

BOOL CImageDataQueue::Add(IMAGEDATA *pImageData)
{
	BOOL ret = FALSE;

	if (m_bForce) {
		EnterCriticalSection(&m_CS);
	} else {
		if (!TryEnterCriticalSection(&m_CS)) return FALSE;
	}
	
	if (FALSE == IsFull())
	{
		CopyMemory(&m_Datum[m_nTail], pImageData, sizeof(IMAGEDATA));
		CopyMemory(&m_Images[m_nTail*m_MaxImageSize], pImageData->pData, pImageData->len);
		m_Datum[m_nTail].pData = &m_Images[m_nTail*m_MaxImageSize];
		m_nTail = (m_nTail+1)%m_nSize;
		++m_nCount;
		ret = TRUE;	
	}
	LeaveCriticalSection(&m_CS);
	return ret;
}

BOOL CImageDataQueue::Remove()
{
	BOOL ret = FALSE;

	EnterCriticalSection(&m_CS);
	if (FALSE == IsEmpty())
	{
		ZeroMemory(&m_Datum[m_nHead], sizeof IMAGEDATA);
		m_nHead = (m_nHead+1)%m_nSize;
		--m_nCount;
		ret = TRUE;
	}
	LeaveCriticalSection(&m_CS);
	return ret; 
}

CImageDataQueue::IMAGEDATA *CImageDataQueue::Get()
{
	IMAGEDATA *ret = NULL;	
	
	EnterCriticalSection(&m_CS);
	if (FALSE == IsEmpty())
	{
		ret = &m_Datum[m_nHead];
	}
	LeaveCriticalSection(&m_CS);
	return ret; 
}

CImageDataQueue::IMAGEDATA *CImageDataQueue::Get(int index)
{
	if(index >= (int)m_nCount) return NULL;

	IMAGEDATA *ret = NULL;

	EnterCriticalSection(&m_CS);
	if (FALSE == IsEmpty())
	{
		size_t i = (m_nHead+index)%m_nSize;
		ret = &m_Datum[i];
	}
	LeaveCriticalSection(&m_CS);
	return ret; 
}
