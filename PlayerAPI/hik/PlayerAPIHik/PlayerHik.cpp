#include "stdafx.h"
#include "PlayerHik.h"
#include <stdio.h>

#include "../inc/windowsPlayM4.h"
#pragma comment(lib,"../lib/PlayCtrl.lib")

char* CPlayerFactoryHik::Name()
{
	return "海康";
}

BOOL CPlayerFactoryHik::IsBelongThis(char *pFile)
{
	
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);

		if (strncmp(buf, "IMKH", 4) == 0 || strncmp(buf, "4HKH", 4)==0)
		{
			return true;
		}
		fclose(pfd);
	}

	/*
	long nport;

	if (PlayM4_GetPort(&nport))
	{
		if (PlayM4_OpenFile(nport, pFile))//这个会导致3GP的文件也被它打开，
		{
			if (PlayM4_Play(nport, 0))
			{


				int dTotalTime = PlayM4_GetFileTime(nport);
				PlayM4_Stop(nport);
				PlayM4_CloseFile(nport);
				PlayM4_FreePort(nport);
				if (dTotalTime > 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			PlayM4_Stop(nport);
		}
		PlayM4_CloseFile(nport);
		PlayM4_FreePort(nport);
	}
	*/
	return false;
}

BOOL CPlayerFactoryHik::Init()
{
	return true;
}

void CPlayerFactoryHik::Release()
{

}

IPlayer *CPlayerFactoryHik::CreatePlayer()
{
	return new CPlayerHik();
}
/////////////

CPlayerHik::CPlayerHik() 
{
}


CPlayerHik::~CPlayerHik()
{
}


BOOL CPlayerHik::OpenFile(char *PFile,HWND hwnd)
{
	m_hwnd = hwnd;
	if (PlayM4_GetPort(&m_nPort))
	{
		return PlayM4_OpenFile(m_nPort, PFile);
	}
	return false;
}
/*
BOOL CPlayerHik::CloseFile()
{
	return PlayM4_CloseFile(m_nPort);
}
*/
BOOL CPlayerHik::Play()
{
	return PlayM4_Play(m_nPort, m_hwnd);
}

BOOL CPlayerHik::Pause(DWORD nPause)
{
	return PlayM4_Pause(m_nPort,nPause);
}

BOOL CPlayerHik::Stop()
{
	 PlayM4_Stop(m_nPort);
	 PlayM4_CloseFile(m_nPort);
	return PlayM4_FreePort(m_nPort);
}
/*
BOOL CPlayerHik::Free()
{
	return PlayM4_FreePort(m_nPort);
}*/

BOOL CPlayerHik::Fast()
{
	return PlayM4_Fast(m_nPort);
}

BOOL CPlayerHik::Slow()
{

	return PlayM4_Slow(m_nPort);
}

BOOL CPlayerHik::OneByOne()
{
	return PlayM4_OneByOne(m_nPort);
}

BOOL CPlayerHik::OneByOneBack()
{
	return PlayM4_OneByOneBack(m_nPort);
}

BOOL CPlayerHik::PlaySoundShare()
{
	return PlayM4_PlaySoundShare(m_nPort);

}

BOOL  CPlayerHik::StopSoundShare()
{
	return PlayM4_StopSoundShare(m_nPort);
}

BOOL CPlayerHik::SetVolume(WORD nVolume)
{
	return PlayM4_SetVolume(m_nPort,nVolume);
}

DWORD CPlayerHik::GetVolume()
{
	return PlayM4_GetVolume(m_nPort);
}

BOOL  CPlayerHik::SetPlayPos(float fRelativePos)
{
	return PlayM4_SetPlayPos(m_nPort, fRelativePos);
}

float CPlayerHik::GetPlayPos()
{
	return PlayM4_GetPlayPos(m_nPort);
}

DWORD CPlayerHik::GetFileTime()
{
	return PlayM4_GetFileTime(m_nPort);
}

DWORD CPlayerHik::GetPlayedTime()
{
	return PlayM4_GetPlayedTime(m_nPort);
}

BOOL  CPlayerHik::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return PlayM4_GetPictureSize(m_nPort,pWidth,pHeight);
}

BOOL  CPlayerHik::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return PlayM4_SetFileEndCallback(m_nPort, callBack, pUser);
}



std::list<CPlayerHik*>  CPlayerHik::m_sPlayerHik;
void CPlayerHik::DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long nReserved)
{
	std::list<CPlayerHik*>::iterator iter = CPlayerHik::m_sPlayerHik.begin();
	//m_FileEndCallbackFun()
	while (iter != CPlayerHik::m_sPlayerHik.end())
	{
		CPlayerHik *pPlayer = (CPlayerHik*)*iter;
		if (pPlayer->m_nPort == nPort)
		{
			pPlayer->SavePic(pBuf, nSize, nWidth, nHeight, nType);
			CPlayerHik::m_sPlayerHik.erase(iter);
			break;
		}
		iter++;
	}
}

BOOL  CPlayerHik::SavePic(char * pBuf, long nSize, long nWidth, long nHeight, long nType)
{
	if (m_iType == 0)
	{
		return PlayM4_ConvertToBmpFile((char*)pBuf, nSize, nWidth, nHeight, nType, m_saveFile);
	}
	else
	{

		PlayM4_ConvertToJpegFile((char*)(char*)pBuf, nSize, nWidth, nHeight, nType, m_saveFile);
	}
}

BOOL  CPlayerHik::CapturePic(char *pSaveFile, int iType)
{

	//设置显示回调
	PlayM4_SetDisplayCallBack(m_nPort, (DisplayCBFun)&DisplayCBFunBack);

	memset(m_saveFile, 0, 256);
	strncpy_s(m_saveFile, pSaveFile, strlen(pSaveFile)>256 ? 256 : strlen(pSaveFile));
	m_iType = iType;

	char *pimageBuf=new char[2800 * 2800 * 3];
	memset(pimageBuf, 255, 2800 * 2800 * 3);
	DWORD imageSize = 0;
	LONG width = 0;
	LONG height = 0;
	bool ret = false;
	PlayM4_GetPictureSize(m_nPort, &width, &height);
	if (iType == 0)
	{
		ret = PlayM4_GetBMP(m_nPort, (PBYTE)pimageBuf, 2800 * 2800 * 3, &imageSize);
		if (ret)
		{
			m_sPlayerHik.push_back(this);
			//ret = PlayM4_ConvertToBmpFile((char*)pimageBuf, imageSize, width, height, T_RGB32, pSaveFile);
		}
	}
	else if (iType == 1)
	{
		ret = PlayM4_GetBMP(m_nPort, (PBYTE)pimageBuf, 2800 * 2800 * 3, &imageSize);
		if (ret)
		{
			m_sPlayerHik.push_back(this);
			//ret = PlayM4_ConvertToJpegFile((char*)pimageBuf, imageSize, width, height, T_RGB32, pSaveFile);//存的文件为空字节
		}
	}
	delete pimageBuf;
	pimageBuf = NULL;
	return ret;
}

BOOL CPlayerHik::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerHik::FileCutClose()
{
	return false;
}

int CPlayerHik::FileCutProcess()
{
	return 0;
}
