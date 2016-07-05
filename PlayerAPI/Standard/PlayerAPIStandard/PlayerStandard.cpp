#include "stdafx.h"
#include "PlayerStandard.h"
#include <stdio.h>

#include "../inc/ffplayerImp.h"
#pragma comment(lib, "../lib/ffplayer.lib")

char* CPlayerFactoryStandard::Name()
{
	return "StandardFile";
}

BOOL CPlayerFactoryStandard::IsBelongThis(char *pFile)
{
	return true;
}

BOOL CPlayerFactoryStandard::Init()
{
	return FFPlayer_Init();
}

void CPlayerFactoryStandard::Release()
{
	FFPlayer_Release();
}

IPlayer *CPlayerFactoryStandard::CreatePlayer()
{
	return new CPlayerStandard();
}
/////////////

CPlayerStandard::CPlayerStandard()
{
}


CPlayerStandard::~CPlayerStandard()
{
}


BOOL CPlayerStandard::OpenFile(char *PFile,HWND hwnd)
{	
	m_hwnd = hwnd;
	if (FFPlayer_GetPort(&m_nPort))
	{
		return FFPlayer_OpenFile(m_nPort, PFile);
	}
	return false;
}


BOOL CPlayerStandard::Play()
{
	return FFPlayer_Play(m_nPort, m_hwnd);
}

BOOL CPlayerStandard::Pause(DWORD nPause)
{
	return FFPlayer_Pause(m_nPort, nPause);
}

BOOL CPlayerStandard::Stop()
{
	FFPlayer_Stop(m_nPort);
	return FFPlayer_FreePort(m_nPort);
}

BOOL CPlayerStandard::Fast()
{
	return FFPlayer_Fast(m_nPort);
}

BOOL CPlayerStandard::FastNoDelay()
{
	return FFPlayer_FastNoDelay(m_nPort);
}

BOOL CPlayerStandard::Slow()
{
	return FFPlayer_Slow(m_nPort);
}

BOOL CPlayerStandard::OneByOne()
{
	return FFPlayer_OneByOne(m_nPort);
}

BOOL CPlayerStandard::OneByOneBack()
{
	return FFPlayer_OneByOneBack(m_nPort);
}

BOOL CPlayerStandard::PlaySoundShare()
{
	return FFPlayer_PlaySoundShare(m_nPort);

}

BOOL  CPlayerStandard::StopSoundShare()
{
	return FFPlayer_StopSoundShare(m_nPort);
}

BOOL CPlayerStandard::SetVolume(WORD nVolume)
{
	return FFPlayer_SetVolume(m_nPort, nVolume);
}

DWORD CPlayerStandard::GetVolume()
{
	return FFPlayer_GetVolume(m_nPort);
}

BOOL  CPlayerStandard::SetPlayPos(float fRelativePos)
{
	return FFPlayer_SetPlayPos(m_nPort, fRelativePos);
}

float CPlayerStandard::GetPlayPos()
{
	return FFPlayer_GetPlayPos(m_nPort);
}

DWORD CPlayerStandard::GetFileTime()
{
	return FFPlayer_GetFileTime(m_nPort);
}

DWORD CPlayerStandard::GetPlayedTime()
{
	return FFPlayer_GetPlayedTime(m_nPort);
}

BOOL CPlayerStandard::SetPlayedTimeEx(DWORD nTime)
{
	return FFPlayer_SetPlayedTime(m_nPort, nTime);
}

BOOL  CPlayerStandard::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return FFPlayer_GetPictureSize(m_nPort, pWidth, pHeight);
}

BOOL  CPlayerStandard::CapturePic(char *pSaveFile, int iType)
{
	return FFPlayer_CapturePic(m_nPort,pSaveFile,iType);
}

BOOL CPlayerStandard::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return FFPlayer_SetFileEndCallback(m_nPort,callBack,pUser);
}

BOOL  CPlayerStandard::SetDisplayCallback(long nID, DisplayCallback displayCallback, void *pUser)
{
	return FFPlayer_SetDisplayCallback(m_nPort, displayCallback, pUser);

}

BOOL CPlayerStandard::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime)
{
	return FFPlayer_FileCutStart(m_nPort, srcFileName, destFileName, startTime, endTime);
}

BOOL CPlayerStandard::FileCutClose()
{
	return FFPlayer_FileCutClose(m_nPort);
}

int CPlayerStandard::FileCutProcess()
{
	return  FFPlayer_FileCutProcess(m_nPort);
}

BOOL CPlayerStandard::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	return FFPlayer_FileConvertStart(m_nPort,srcFileName, destFileName, startTime, endTime, bConvert, pConvertPara);
}

BOOL CPlayerStandard::FileConvertClose()
{
	 FFPlayer_FileConvertClose(m_nPort);
	return FFPlayer_FreePort(m_nPort);
}

int CPlayerStandard::FileConvertProcess()
{
	return FFPlayer_FileConvertProcess(m_nPort);
}

BOOL CPlayerStandard::FileConvertPause(DWORD nPause)
{
	return FFPlayer_FileConvertPause(m_nPort,nPause);
}