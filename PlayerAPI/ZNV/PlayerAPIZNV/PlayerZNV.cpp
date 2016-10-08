#include "stdafx.h"
#include "PlayerZNV.h"
#include <stdio.h>
#include <time.h>

#include "../inc/PlaySDK.h"
#pragma comment(lib,"../lib/Play_SDK.lib")

int g_Speed[] = { -16, -8, -4, -2, 1, 2, 4, 8, 16 };

char* CPlayerFactoryZNV::Name()
{
	return "中兴力维";
}

BOOL CPlayerFactoryZNV::IsBelongThisHead(char *pFile)
{	
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);
		fclose(pfd);

		if (strncmp(buf, "pu8000", 6) == 0 || strncmp(buf, "8000", 4) == 0 )
		{
			return true;
		}
		
	}
	return false;
}

BOOL CPlayerFactoryZNV::IsBelongThis(char *pFile)
{
	if (strstr(pFile, ".zv") || strstr(pFile, ".znv") || strstr(pFile, ".zte") || strstr(pFile, ".zn"))//判断ZNV文件
	{
		return true;
	}
}

BOOL CPlayerFactoryZNV::Init()
{
	return true;
}

void CPlayerFactoryZNV::Release()
{

}

IPlayer *CPlayerFactoryZNV::CreatePlayer()
{
	return new CPlayerZNV();
}
/////////////

CPlayerZNV::CPlayerZNV() :m_iSpeed(4), m_bPause(false)
{
}


CPlayerZNV::~CPlayerZNV()
{
}


BOOL CPlayerZNV::OpenFile(char *PFile,HWND hwnd)
{
	m_hwnd = hwnd;
	int nRet = Play_GetPort((int*)&m_nPort);
	if (nRet==0)
	{
		nRet = Play_OpenFile(m_nPort, (int)hwnd, PFile);
		if (nRet == 0)
		{
			return true;
		}
	}
	return false;
}

BOOL CPlayerZNV::Play()
{
	int nRet = Play_Play(m_nPort);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerZNV::Pause(DWORD nPause)
{
	int nRet = -1;
	m_bPause = nPause;
	if (nPause)
	{
		nRet = Play_Pause(m_nPort);		
	}
	else
	{
		nRet = Play_Play(m_nPort);
	}

	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerZNV::Stop()
{
	Play_Stop(m_nPort);
	Play_CloseFile(m_nPort);
	Play_FreePort(m_nPort);
	return true;
}


BOOL CPlayerZNV::Fast()
{
	if (m_iSpeed >8)
	{
		return false;
	}

	m_iSpeed++;
	int nRet = Play_Speed(m_nPort, g_Speed[m_iSpeed]);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerZNV::Slow()
{	
	if (m_iSpeed < 0)
	{
		return false;
	}
	m_iSpeed--;
	int nRet = Play_Speed(m_nPort, g_Speed[m_iSpeed]);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerZNV::OneByOne()
{
	///Play_SetStepPlay(m_nPort);
	return false;
}

BOOL CPlayerZNV::OneByOneBack()
{
	return false;
}

BOOL CPlayerZNV::PlaySoundShare()
{
	int nRet= Play_PlaySound(m_nPort);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerZNV::StopSoundShare()
{
	int nRet = Play_StopSound(m_nPort);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerZNV::SetVolume(WORD nVolume)
{
	return false;
}

DWORD CPlayerZNV::GetVolume()
{
	return 0;
}

BOOL  CPlayerZNV::SetPlayPos(float fRelativePos)
{
	int nRet= Play_SetPlayedPos(m_nPort, fRelativePos*100);
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CPlayerZNV::GetPlayPos()
{
	return Play_GetPlayedPos(m_nPort)/100.0;
}

DWORD CPlayerZNV::GetFileTime()
{
	return Play_GetFileTime(m_nPort);
}

DWORD CPlayerZNV::GetPlayedTime()
{
	return Play_GetPlayedTime(m_nPort);
}

BOOL CPlayerZNV::SetPlayedTimeEx(DWORD nTime)
{
	return Play_SetPlayedTime(m_nPort, nTime);
}

BOOL  CPlayerZNV::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return Play_GetVideoInfo(m_nPort, (int*)pWidth, (int*)pHeight);
}


BOOL  CPlayerZNV::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	return false;
}

BOOL  CPlayerZNV::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	return false;
}

void  CPlayerZNV::ZNVFileEndCallback(long lParam)
{
	CPlayerZNV* pPlayer = (CPlayerZNV*)lParam;
	pPlayer->m_FileEndCallbackFun(pPlayer->m_nID,pPlayer->m_pUser);
}

BOOL  CPlayerZNV::SetFileEndCallback(LONG nID, FileEndCallback callBack, void *pUser)
{
	m_nID = nID;
	m_FileEndCallbackFun = callBack;
	m_pUser = pUser;
 //   Play_SetFileEndCallBack(m_nPort, (FileEndCallBack)&ZNVFileEndCallback, (long)pUser);
	return false;
}

BOOL  CPlayerZNV::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	return false;
	m_DisplayCallbackFun = displayCallback;
	m_DisplayCalUser = nUser;
	int nRet = Play_SetYUVCallBack(m_nPort, (Play_VideoYUVCallBack)&DisplayCBFunBack, (long)this);//无效，无数据回调
	if (nRet == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CPlayerZNV::DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, unsigned __int64 nStamp, long nType, long nReserved)
{
	CPlayerZNV* pPlayer = (CPlayerZNV*)nReserved;
	if (pPlayer)
	{
		DISPLAYCALLBCK_INFO displayInfo;
		displayInfo.pBuf = (char*)pBuf;
		displayInfo.nWidth = nWidth;
		displayInfo.nHeight = nHeight;
		displayInfo.nStamp = nStamp;
		displayInfo.nUser = (long)pPlayer->m_DisplayCalUser;
		pPlayer->m_DisplayCallbackFun(&displayInfo);
	}
}

BOOL  CPlayerZNV::GetSystemTime(unsigned long long *systemTime)
{/*
	PLAYM4_SYSTEM_TIME sysTime;
	bool bRet=PlayM4_GetSystemTime(m_nPort, &sysTime);
	struct tm tblock;
	
	tblock.tm_year=sysTime.dwYear-1900;
	tblock.tm_mon = sysTime.dwMon-1;
	tblock.tm_mday = sysTime.dwDay;
	tblock.tm_hour = sysTime.dwHour;
	tblock.tm_min = sysTime.dwMin;
	tblock.tm_sec = sysTime.dwSec;
	*systemTime = mktime(&tblock);

	return bRet;*/
	return false;
}	



BOOL  CPlayerZNV::CapturePic(char *pSaveFile, int iType)
{
	int  nRet = Play_CapturePicture(m_nPort, pSaveFile);
	if (nRet == 0)
	{
		Sleep(1000);
		return true;
	}
	else
	{
		return false;
	}

}

BOOL CPlayerZNV::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerZNV::FileCutClose()
{
	return false;
}

int CPlayerZNV::FileCutProcess()
{
	return 0;
}
