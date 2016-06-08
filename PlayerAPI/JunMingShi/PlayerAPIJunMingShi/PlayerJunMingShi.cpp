#include "stdafx.h"
#include "PlayerJunMingShi.h"

#include "../inc/TLPlay.h"
#pragma comment(lib,"../lib/TLPlay.lib")


char* CPlayerFactoryJunMingShi::Name()
{
	return "¿¡Ã÷ÊÓ";
}

BOOL CPlayerFactoryJunMingShi::IsBelongThis(char *pFile)
{
	if (strstr(pFile, ".ifv"))//
	{
		return true;
	}

	return false;
}

BOOL CPlayerFactoryJunMingShi::Init()
{
	return true;
}

void CPlayerFactoryJunMingShi::Release()
{

}

IPlayer *CPlayerFactoryJunMingShi::CreatePlayer()
{
	return new CPlayerJunMingShi();
}
/////////////

CPlayerJunMingShi::CPlayerJunMingShi()
{
}


CPlayerJunMingShi::~CPlayerJunMingShi()
{
}


BOOL CPlayerJunMingShi::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;
	if (TLPlay_GetPort(&m_nPort))
	{
		TLPlay_SetPlayMode(m_nPort,TL_PLAY_FILE);
		return TLPlay_OpenFile(m_nPort, PFile);
	}
	return false;
}
/*
BOOL CPlayerHik::CloseFile()
{
	return PlayM4_CloseFile(m_nPort);
}
*/
BOOL CPlayerJunMingShi::Play()
{
	return TLPlay_Play(m_nPort, m_hwnd);
}

BOOL CPlayerJunMingShi::Pause(DWORD nPause)
{
	return TLPlay_Pause(m_nPort, nPause);
}

BOOL CPlayerJunMingShi::Stop()
{
	 TLPlay_Stop(m_nPort);
	 TLPlay_Close(m_nPort);
	 return TLPlay_FreePort(m_nPort);
}
/*
BOOL CPlayerHik::Free()
{
	return PlayM4_FreePort(m_nPort);
}*/

BOOL CPlayerJunMingShi::Fast()
{
	return TLPlay_Fast(m_nPort);
}

BOOL CPlayerJunMingShi::Slow()
{
	return TLPlay_Slow(m_nPort);
}

BOOL CPlayerJunMingShi::OneByOne()
{
	return TLPlay_OneByOne(m_nPort);
}

BOOL CPlayerJunMingShi::OneByOneBack()
{
	return false;
}

BOOL CPlayerJunMingShi::PlaySoundShare()
{
	return TLPlay_PlaySound(m_nPort);

}

BOOL  CPlayerJunMingShi::StopSoundShare()
{
	return TLPlay_StopSound();
}

BOOL CPlayerJunMingShi::SetVolume(WORD nVolume)
{
	return TLPlay_SetVolume(m_nPort, nVolume);
}

DWORD CPlayerJunMingShi::GetVolume()
{
	return TLPlay_GetVolume(m_nPort);
}

BOOL  CPlayerJunMingShi::SetPlayPos(float fRelativePos)
{
	return TLPlay_SetPlayPos(m_nPort, fRelativePos);
}

float CPlayerJunMingShi::GetPlayPos()
{
	return TLPlay_GetPlayPos(m_nPort);
}

DWORD CPlayerJunMingShi::GetFileTime()
{
	return TLPlay_GetFileTime(m_nPort)/1000;
}

DWORD CPlayerJunMingShi::GetPlayedTime()
{
	return TLPlay_GetPlayedTime(m_nPort)/1000;
}

BOOL  CPlayerJunMingShi::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return TLPlay_GetPictureSize(m_nPort, pWidth, pHeight);
}

BOOL  CPlayerJunMingShi::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	return TLPlay_SetColor(m_nPort, nBrightness, nContrast, nSaturation, nHue);
}

BOOL  CPlayerJunMingShi::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	return TLPlay_GetColor(m_nPort, pBrightness, pContrast, pSaturation, pHue);
}

BOOL CPlayerJunMingShi::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return false;
}

BOOL  CPlayerJunMingShi::CapturePic(char *pSaveFile, int iType)
{
	if (iType == 0)
	{
		return TLPlay_GetBMP(m_nPort, pSaveFile);
	}
	else
	{
		return TLPlay_GetJPG(m_nPort, pSaveFile, 100);
	}
}