#include "stdafx.h"
#include "PlayerBlueSky.h"
#include <stdio.h>

char* CPlayerFactoryBlueSky::Name()
{
	return "蓝色星际";
}

BOOL CPlayerFactoryBlueSky::IsSupportFastCut()
{
	return true;
}

BOOL CPlayerFactoryBlueSky::IsBelongThis(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);

		if (strncmp(buf, "BLUESKY", 7) == 0)
		{
			return true;
		}
		fclose(pfd);
	}


	return false;
}

BOOL CPlayerFactoryBlueSky::Init()
{
	dvxSdkInit();
	return true;
}

void CPlayerFactoryBlueSky::Release()
{
	dvxSdkDeInit();
}

IPlayer *CPlayerFactoryBlueSky::CreatePlayer()
{
	return new CPlayerBlueSky();
}


//
CPlayerBlueSky::CPlayerBlueSky() :m_iSpeed(0), m_hLocalPlay(0), m_snRate(1), m_bPause(false), m_cutHandle(NULL)
{
}


CPlayerBlueSky::~CPlayerBlueSky()
{
}


BOOL CPlayerBlueSky::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;
	
	int nRet = dvxLocalPlayOpen(PFile, m_hwnd, m_hwnd, 0, &m_hLocalPlay);
	if (nRet != DVX_OK)
	{
		return false;
	}

	dvxPlaySound(m_hLocalPlay);
	nRet = dvxLocalPlaySetPlayMode(m_hLocalPlay, DVX_PLAY_MODE_NORMAL);
	nRet = dvxLocalPlayPlay(m_hLocalPlay);
	if (nRet != DVX_OK)
	{
		return false;
	}
	return true;
}

BOOL CPlayerBlueSky::Play()
{
	return dvxLocalPlayPlay(m_hLocalPlay);
}

BOOL CPlayerBlueSky::Pause(DWORD nPause)
{
	m_bPause = nPause;
	if (nPause)
	{
		dvxLocalPlayPause(m_hLocalPlay);
	}
	else
	{
		dvxLocalPlayPlay(m_hLocalPlay);
	}
	 return true;
}

BOOL CPlayerBlueSky::Stop()
{
	dvxLocalPlayStop(m_hLocalPlay);
	dvxLocalPlayClose(m_hLocalPlay);
	
	return true;
}

BOOL CPlayerBlueSky::Fast()
{
	if (m_iSpeed >= 4)
	{
		return true;
	}
	m_iSpeed++;

	if ( m_hLocalPlay)
	{
		if (m_iSpeed >= 0)
		{
			m_snRate *= 2;
			if (m_iSpeed == 0)
			{
				m_snRate = 1;
			}
			dvxLocalPlaySetRate(m_hLocalPlay, m_snRate, 1);

		}
		else
		{
			m_snRate /= 2;
			if (m_iSpeed == 0)
			{
				m_snRate = 1;
			}
			dvxLocalPlaySetRate(m_hLocalPlay, 1, m_snRate);
		}		
	}
	return true;
}

BOOL CPlayerBlueSky::Slow()
{
	if (m_iSpeed <= -4)
	{
		return true;
	}
	m_iSpeed--;

	if (m_hLocalPlay)
	{
		if (m_iSpeed >= 0)
		{
			m_snRate /= 2;
			if (m_iSpeed == 0)
			{
				m_snRate = 1;
			}
			dvxLocalPlaySetRate(m_hLocalPlay, m_snRate, 1);
		}
		else
		{
			m_snRate *= 2;
			if (m_iSpeed == 0)
			{
				m_snRate = 1;
			}
			dvxLocalPlaySetRate(m_hLocalPlay, 1, m_snRate);
		}

	}
	return true;
}

BOOL CPlayerBlueSky::OneByOne()
{
	return false;
}

BOOL CPlayerBlueSky::OneByOneBack()
{
	return false;
}

BOOL CPlayerBlueSky::PlaySoundShare()
{
	int ret = dvxPlaySound(m_hLocalPlay);
	if (ret == 0)
	{
		return true;
	}
	return false;

}

BOOL  CPlayerBlueSky::StopSoundShare()
{
	int ret = dvxStopSound(m_hLocalPlay);
	if (ret == 0)
	{
		return true;
	}
	return false;

}

BOOL CPlayerBlueSky::SetVolume( WORD nVolume)
{
	int ret = dvxSetVolume(m_hLocalPlay, nVolume);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

DWORD CPlayerBlueSky::GetVolume()
{
	DWORD nVolume=0;
	int ret = dvxGetVolume(m_hLocalPlay, (int*)&nVolume);
	if (ret == 0)
	{
		return nVolume;
	}
	return 0;
}


BOOL  CPlayerBlueSky::SetPlayPos(float fRelativePos)
{
	long long lBeginTime, lEndTime;
	dvxLocalPlayGetTime(m_hLocalPlay, &lBeginTime, &lEndTime);

	if (m_bPause)
	{
		dvxLocalPlayPlay(m_hLocalPlay);
		dvxLocalPlaySeek(m_hLocalPlay, (lEndTime - lBeginTime)*fRelativePos + lBeginTime);
		dvxLocalPlayPause(m_hLocalPlay);
	}
	else
	{
		dvxLocalPlaySeek(m_hLocalPlay, (lEndTime - lBeginTime)*fRelativePos + lBeginTime);
	}

	return true;
}

float CPlayerBlueSky::GetPlayPos()
{
	long long lCurTime;
	dvxLocalPlayGetCurrentTime(m_hLocalPlay, &lCurTime);
//	dvxPlayGetPos
	long long lBeginTime, lEndTime;
	dvxLocalPlayGetTime(m_hLocalPlay, &lBeginTime, &lEndTime);
	return ((float)lCurTime - (float)lBeginTime) / ((float)lEndTime - (float)lBeginTime);
}

DWORD CPlayerBlueSky::GetFileTime()
{
	long long lBeginTime, lEndTime;
	dvxLocalPlayGetTime(m_hLocalPlay, &lBeginTime, &lEndTime);
	return (lEndTime - lBeginTime)/ 1000;
}

DWORD CPlayerBlueSky::GetPlayedTime()
{
	long long ltime;
	dvxLocalPlayGetCurrentTime(m_hLocalPlay, &ltime);
	long long lBeginTime, lEndTime;
	dvxLocalPlayGetTime(m_hLocalPlay, &lBeginTime, &lEndTime);
	return (ltime - lBeginTime )/ 1000;
}

BOOL  CPlayerBlueSky::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	dvxPlayGetResolution(m_hLocalPlay, (int*)pWidth, (int*)pHeight);
	return true;
}

BOOL  CPlayerBlueSky::CapturePic(char *pSaveFile, int iType)
{
	int iRet = dvxPlayPictureEx(m_hLocalPlay, pSaveFile, iType);/*0 bitmap,1 jpg*/
	if (iRet == DVX_OK)
	{
		return true;
	}
	return false;
}

int CPlayerBlueSky::NotifyRoutineCallBack(unsigned int notifyId, LPVOID lpUserData)
{
	//if (notifyId)//是文件播放完事件通知
	CPlayerBlueSky *pPlayerBlueSky = (CPlayerBlueSky*)lpUserData;
	if (pPlayerBlueSky)
	{
		pPlayerBlueSky->m_FileEndCallbackFun(pPlayerBlueSky->m_nID, pPlayerBlueSky->m_pUser);
	}
	return true;
}
BOOL  CPlayerBlueSky::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	m_nID = nID;
	m_FileEndCallbackFun = callBack;
	m_pUser = pUser;
	dvxSetLocalNotifyCallback(m_hLocalPlay, &NotifyRoutineCallBack,this);
	return true;
}

BOOL CPlayerBlueSky::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	int iRet = dvxFileCutStart(srcFileName, destFileName, startTime, endTime, &m_cutHandle);
	if (iRet == DVX_OK)
	{
		return true;
	}
	return false;
}

BOOL CPlayerBlueSky::FileCutClose()
{
	int iRet=dvxFileCutClose(m_cutHandle);
	if (iRet == DVX_OK)
	{
		return true;
	}
	return false;
}

int CPlayerBlueSky::FileCutProcess()
{
	return dvxFileCutProcess(m_cutHandle);
}
