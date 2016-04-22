#include "stdafx.h"
#include "PlayerTD.h"

#include "../inc/Interface.h"
#pragma comment(lib,"../lib/PlaySdkM4.lib")


#include <Shlwapi.h> //abs 取绝对值


char* CPlayerFactoryTD::Name()
{
	return "天地伟业";
}

BOOL CPlayerFactoryTD::IsBelongThis(char *pFile)
{
	if (strstr(pFile, ".sdv"))//判断是天地伟业 标准文件格式
	{
		return true;
	}

	return false;
}

BOOL CPlayerFactoryTD::Init()
{
	return true;
}

void CPlayerFactoryTD::Release()
{

}

IPlayer *CPlayerFactoryTD::CreatePlayer()
{
	return new CPlayerTD();
}
/////////////

CPlayerTD::CPlayerTD() :m_iSpeed(0), m_bPause(false)
{
}


CPlayerTD::~CPlayerTD()
{
}


BOOL CPlayerTD::OpenFile(char *PFile,HWND hwnd)
{
	TC_CreateSystem(hwnd);
	m_nPort = TC_CreatePlayerFromFile(hwnd, PFile);
	if (m_nPort >= 0)
	{
		return true;
	}
	return false;
}
/*
BOOL CPlayerTD::CloseFile()
{
	return true;
}*/

BOOL CPlayerTD::Play()
{
	TC_Play(m_nPort);
	m_iSpeed = 0;
	TC_FastForward(m_nPort, m_iSpeed);
	return true;
}

BOOL CPlayerTD::Pause(DWORD nPause)
{
	m_bPause = nPause;
	if (nPause)
	{
		 TC_Pause(m_nPort);
	}
	else
	{
		TC_Play(m_nPort);
		if (m_iSpeed >= 0)
		{
			TC_FastForward(m_nPort, m_iSpeed);
		}
		else
		{
			TC_SlowForward(m_nPort, abs(m_iSpeed));
		}
	}
	return true;
}

BOOL CPlayerTD::Stop()
{
	 TC_Stop(m_nPort);
	 TC_DeletePlayer(m_nPort);
	 return TC_DeleteSystem();
}
/*
BOOL CPlayerTD::Free()
{
	TC_DeletePlayer(m_nPort);
	return TC_DeleteSystem();
	
}*/

BOOL CPlayerTD::Fast()
{
	m_iSpeed++;
	int iRet = -1;
	 if (m_iSpeed >= 4)
	{
		m_iSpeed = 4;
	}
	if (m_iSpeed >= 0)
	{
		iRet=TC_FastForward(m_nPort, m_iSpeed);
	}
	else
	{
		iRet=TC_SlowForward(m_nPort, abs(m_iSpeed));
	}
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::Slow()
{
	m_iSpeed--;

	if (m_iSpeed<= -4)
	{
		m_iSpeed = -4;
	}
	int iRet = -1;
	if (m_iSpeed >= 0)
	{
		iRet=TC_FastForward(m_nPort, m_iSpeed);
	}
	else
	{
		iRet=TC_SlowForward(m_nPort, abs(m_iSpeed));
	}
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerTD::SetPlayPos(float fRelativePos)
{
	int frameCount = TC_GetFrameCount(m_nPort);
	int frameNum = frameCount*fRelativePos;
	TC_Seek(m_nPort, frameNum);//这里变为正常速度了
	Sleep(100);
	int iRet = -1;
	if (m_bPause)
	{
		TC_Pause(m_nPort);
	}
	else//拉动进度条后，依然按之前设置的速度播放
	{
		if (m_iSpeed <= -4)
		{
			m_iSpeed = -4;
		}
		if (m_iSpeed >= 0)
		{
			iRet=TC_FastForward(m_nPort, m_iSpeed);
		}
		else
		{
			iRet=TC_SlowForward(m_nPort, abs(m_iSpeed));
		}
	}
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::OneByOne()
{
	int iRet= TC_StepForward(m_nPort);
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::OneByOneBack()
{
	int iRet = TC_StepBackward(m_nPort);
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::PlaySoundShare()
{
	int iRet= TC_PlayAudio(m_nPort);
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerTD::StopSoundShare()
{
	int iRet = TC_StopAudio(m_nPort);
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::SetVolume(WORD nVolume)
{
	int iRet = TC_SetAudioVolumeEx(m_nPort, (short)nVolume);
	if (0 == iRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD CPlayerTD::GetVolume()
{
	unsigned short dVolume;
	int iRet = TC_GetAudioVolumeEx(m_nPort,&dVolume);
	return dVolume;
}

float CPlayerTD::GetPlayPos()
{
	int curFrameNum = TC_GetPlayingFrameNum(m_nPort);
	int frameCount = TC_GetFrameCount(m_nPort);
	float fpos = (float)curFrameNum / (float)frameCount;
	return fpos;
}

DWORD CPlayerTD::GetFileTime()
{
	int frameCount = TC_GetFrameCount(m_nPort);
	int frameRate = TC_GetFrameRate(m_nPort);
	DWORD dTime = (frameCount + frameRate / 2) / frameRate;
	return dTime;
	
}

DWORD CPlayerTD::GetPlayedTime()
{
	int curFrameNum = TC_GetPlayingFrameNum(m_nPort);
	int frameRate = TC_GetFrameRate(m_nPort);
	//return TC_GetPlayTime(m_nPort)/1000;
	return (curFrameNum + frameRate / 2) / frameRate;
}

BOOL  CPlayerTD::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	int rate = 0;
	 TC_GetVideoParam(m_nPort, (int*)pWidth, (int*)pHeight,&rate);
	 return true;
}

void  CPlayerTD::EndCallback(long nID)
{
	std::list<CPlayerTD*>::iterator iter = m_sPlayerTDCallBack.begin();
	//m_FileEndCallbackFun()
	while (iter != m_sPlayerTDCallBack.end())
	{
		CPlayerTD *pPlayer = (CPlayerTD*)*iter;
		if (pPlayer->m_nPort == nID)
		{
			pPlayer->m_FileEndCallbackFun(pPlayer->m_nID, pPlayer->m_pUser);
			m_sPlayerTDCallBack.erase(iter);
			break;
		}
		iter++;
	}
}

std::list<CPlayerTD*>  CPlayerTD::m_sPlayerTDCallBack;

BOOL  CPlayerTD::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	m_nID = nID;
	m_FileEndCallbackFun = callBack;
	m_pUser = pUser;
	TC_RegisterNotifyPlayToEnd(pfCBPlayEnd(EndCallback));
	m_sPlayerTDCallBack.push_back(this);
}

BOOL  CPlayerTD::CapturePic(char *pSaveFile, int iType)
{
	int iRet=TC_CaptureBmpPic(m_nPort, pSaveFile);
	if (iRet>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerTD::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerTD::FileCutClose()
{
	return false;
}

int CPlayerTD::FileCutProcess()
{
	return 0;
}
