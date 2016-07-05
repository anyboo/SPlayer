#include "stdafx.h"
#include "PlayerViss.h"
#include <stdio.h>
#include <time.h>
#include <atlconv.h>
#include <tchar.h>

char* CPlayerFactoryViss::Name()
{
	return "Viss";
}

BOOL CPlayerFactoryViss::IsBelongThis(char *pFile)
{
	if (strstr(pFile, ".ps") || strstr(pFile, ".mpg") )//ÅÐ¶ÏVissÎÄ¼þ
	{
		return true;
	}
	return false;
}

BOOL CPlayerFactoryViss::Init()
{
	return true;
}

void CPlayerFactoryViss::Release()
{

}

IPlayer *CPlayerFactoryViss::CreatePlayer()
{
	return new CPlayerViss();
}
/////////////

CPlayerViss::CPlayerViss() :m_fSpeed(1.0)
{
}


CPlayerViss::~CPlayerViss()
{
}


BOOL CPlayerViss::OpenFile(char *PFile,HWND hwnd)
{
	m_hwnd = hwnd;
	HRESULT nRet=Viss_OpenFile(&m_nPort, PFile);
	if (nRet>=0)
	{	
		return true;	
	}
	return false;
}

BOOL CPlayerViss::Play()
{
	int nRet = Viss_Play(m_nPort,m_hwnd);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerViss::Pause(DWORD nPause)
{
	int nRet= Viss_Pause(m_nPort, nPause);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerViss::Stop()
{
	Viss_Stop(m_nPort);
	Viss_CloseFile(m_nPort);
	return true;
}


BOOL CPlayerViss::Fast()
{
	m_fSpeed *= 2.0;
	if (m_fSpeed>64.0)
	{
		m_fSpeed = 64.0;
	}
	int nRet = Viss_SetPlaySpeed(m_nPort, m_fSpeed);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerViss::Slow()
{
	m_fSpeed /= 2.0;
	if (m_fSpeed<(1.0 / 64.0))
	{
		m_fSpeed = 1.0 / 64.0;
	}
	int nRet = Viss_SetPlaySpeed(m_nPort, m_fSpeed);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerViss::OneByOne()
{
	///Play_SetStepPlay(m_nPort);
	return false;
}

BOOL CPlayerViss::OneByOneBack()
{
	return false;
}

BOOL CPlayerViss::PlaySoundShare()
{
	int nRet = Viss_PlaySound(m_nPort);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerViss::StopSoundShare()
{
	int nRet = Viss_StopSound(m_nPort);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerViss::SetVolume(WORD nVolume)
{
	int nRet = Viss_SetVolume(m_nPort, nVolume);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD CPlayerViss::GetVolume()
{
	long nVolume;
	return Viss_GetVolume(m_nPort, &nVolume);
}

BOOL  CPlayerViss::SetPlayPos(float fRelativePos)
{
	int nRet = Viss_SetPlayPos(m_nPort, fRelativePos * 100);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CPlayerViss::GetPlayPos()
{
	long nPos=0;
	Viss_GetPlayPos(m_nPort, &nPos);
	return nPos / 100.0;
}

DWORD CPlayerViss::GetFileTime()
{
	long nTotalTime = 0;
	Viss_GetFileTotalTime(m_nPort, &nTotalTime);
	return nTotalTime;
}

DWORD CPlayerViss::GetPlayedTime()
{
	long nPosTime = 0;
	Viss_GetPlayTimePos(m_nPort, &nPosTime);
	return nPosTime;
}

BOOL CPlayerViss::SetPlayedTimeEx(DWORD nTime)
{
	return Viss_SetPlayTimePos(m_nPort, nTime);
}

BOOL  CPlayerViss::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	//return Play_GetVideoInfo(m_nPort, (int*)pWidth, (int*)pHeight);
	return false;
}


BOOL  CPlayerViss::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	int nRet=Viss_SetColor(m_nPort, nBrightness*2, nContrast*2, nSaturation*2, nHue*2);
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerViss::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	int nRet = Viss_GetColor(m_nPort, (long*)pBrightness, (long*)pContrast, (long*)pSaturation, (long*)pHue);
	*pBrightness /= 2;
	*pContrast /= 2;
	*pSaturation /= 2;
	*pHue /= 2;
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


BOOL  CPlayerViss::SetFileEndCallback(LONG nID, FileEndCallback callBack, void *pUser)
{
	Viss_SetFileEndCallback(m_nPort, callBack, pUser);
	return true;
}

BOOL  CPlayerViss::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	m_DisplayCallbackFun = displayCallback;
	m_DisplayCalUser = nUser;
	int nRet = Viss_SetDecodeCallback(m_nPort, &VissDecCallback,this );//Viss_SetDecodeCallbackEx
	//int nRet = Viss_SetDecodeCallbackEx(m_nPort, &VissDecCallback, NULL,this);//
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void CPlayerViss::VissDecCallback(long hHandle, FrameInfo* pFrame, void* pUser)
{
	if (YUV420 == pFrame->Type)
	{
		CPlayerViss* pPlayer = (CPlayerViss*)pUser;

		DISPLAYCALLBCK_INFO displayInfo;
		displayInfo.pBuf = (char*)pFrame->Buffer;
		displayInfo.nWidth = pFrame->Width;
		displayInfo.nHeight = pFrame->Height;
		displayInfo.nStamp = pFrame->Timestamp;
		displayInfo.nUser = (long)pPlayer->m_DisplayCalUser;
		pPlayer->m_DisplayCallbackFun(&displayInfo);
	}
}

BOOL  CPlayerViss::GetSystemTime(unsigned long long *systemTime)
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



BOOL  CPlayerViss::CapturePic(char *pSaveFile, int iType)
{
	//USES_CONVERSION;
	int  nRet = Viss_CapPic(m_nPort, LPCTSTR(pSaveFile));
	if (nRet >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOL CPlayerViss::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerViss::FileCutClose()
{
	return false;
}

int CPlayerViss::FileCutProcess()
{
	return 0;
}
