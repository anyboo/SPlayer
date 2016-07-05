#include "stdafx.h"
#include "Buffer.h"


CBuffer::CBuffer()
{
	
}


CBuffer::~CBuffer(void)
{
	destoryBuffer();
}
/*
void CBuffer::createBuffer(int len,int count)
{
	try
	{
		for (int i = 0; i<count; i++)
		{
			PInfo pInfo = new T_Info;
			pInfo->Data = new unsigned char[len];
			m_List.push_back(pInfo);
		}
	}
	catch (...)
	{

	}
}*/

void  CBuffer::freeBuf(PInfo *pInfo)
{
	delete ((PInfo)(*pInfo))->Data;
	delete *pInfo;
	*pInfo = NULL;
}

PInfo  CBuffer::createBuf(int len)
{
	try
	{
		PInfo pInfo = new T_Info;
		pInfo->Data = new unsigned char[len];
		pInfo->size = len;
		return pInfo;
	}
	catch (...)
	{

	}
}

void CBuffer::destoryBuffer()
{
	PInfo pInfo = getInfoFromList();
	while (pInfo)
	{
		freeBuf(&pInfo);
		pInfo = getInfoFromList();
	}
}
void CBuffer::insertList(PInfo pInfo)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexList);
	m_List.push_back(pInfo);

}

int CBuffer::getCurCount()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexList);
	return m_List.size();
}

PInfo CBuffer::getInfoFromList()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexList);
	PInfo pInfo = NULL;
	if (m_List.size() != 0)
	{
		pInfo = m_List.front();
		m_List.pop_front();
	}
	
	return pInfo;
}

PInfo CBuffer::getFreeInfoFromList(int len)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexList);
	PInfo pInfo = NULL;
	if (m_List.size() != 0)
	{
		pInfo = m_List.front();
		m_List.pop_front();
		if (pInfo->size < len)
		{
			freeBuf(&pInfo);
			return createBuf(len);
		}
	}
	else
	{
		return createBuf(len);
	}

	return pInfo;
}