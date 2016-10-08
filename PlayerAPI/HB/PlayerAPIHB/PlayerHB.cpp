#include "stdafx.h"
#include "PlayerHB.h"

#include <stdio.h>


char* CPlayerFactoryHB::Name()
{
	return "汉邦高科";
}

BOOL CPlayerFactoryHB::IsBelongThisHead(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);

		if (strncmp(buf, "HBGK", 4) == 0)
		{
			return true;
		}
		fclose(pfd);
	}


	return false;
}

BOOL CPlayerFactoryHB::Init()
{
	return true;
}

void CPlayerFactoryHB::Release()
{

}

IPlayer *CPlayerFactoryHB::CreatePlayer()
{
	return new CPlayerHB();
}

////////

CPlayerHB::CPlayerHB() : m_fSpeed(1.0)
{
}


CPlayerHB::~CPlayerHB()
{
}


BOOL CPlayerHB::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;

	HRESULT  result = HB_PLAY2_OpenFileA(&m_hPlay, PFile);
	if (result != HBPLAY2_OK)
	{
		return false;
	}
	HBPLAY2_VIEWPORT viewPort;
	viewPort.dwSize = sizeof(HBPLAY2_VIEWPORT);
	viewPort.dwID = 1;
	viewPort.hDestWnd = hwnd;
	result = HB_PLAY2_AddViewport(m_hPlay, &viewPort);
	if (result != HBPLAY2_OK)
	{
		return false;
	}
	return true;
}

BOOL CPlayerHB::Play()
{
	HB_PLAY2_Play(m_hPlay);
	return true ;
}

BOOL CPlayerHB::Pause(DWORD nPause)
{
	if ( nPause)
	{
		HB_PLAY2_Pause(m_hPlay);
	}
	else
	{
		HB_PLAY2_Play(m_hPlay);
	}
	return true;
}

BOOL CPlayerHB::Stop()
{
	HB_PLAY2_Stop(m_hPlay);
	HB_PLAY2_Close(m_hPlay);
	return true;
}

BOOL CPlayerHB::Fast()
{
	m_fSpeed *= 2.0;
	if (m_fSpeed>64.0)
	{
		m_fSpeed = 64.0;
	}
	HRESULT hRet = HB_PLAY2_SetSpeed(m_hPlay, m_fSpeed);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerHB::Slow()
{
	m_fSpeed /= 2.0;
	if (m_fSpeed<(1.0 / 64.0))
	{
		m_fSpeed = 1.0 / 64.0;
	}
	HRESULT hRet = HB_PLAY2_SetSpeed(m_hPlay, m_fSpeed);
	 if (HBPLAY2_OK == hRet)
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}


BOOL CPlayerHB::OneByOne()
{
	HRESULT hRet = HB_PLAY2_SeekNextIndex(m_hPlay);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerHB::OneByOneBack()
{
	HRESULT hRet = HB_PLAY2_SeekPreviousIndex(m_hPlay);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerHB::PlaySoundShare()
{
	DWORD flags;
	HB_PLAY2_GetEnableFlag(m_hPlay, &flags);
	HRESULT hRet = HB_PLAY2_SetEnableFlag(m_hPlay, flags | HBPLAY2_ENABLE_AUDIO_CODEC | HBPLAY2_ENABLE_SOUND);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerHB::StopSoundShare()
{
	DWORD flags;
	HB_PLAY2_GetEnableFlag(m_hPlay, &flags);
	HRESULT hRet = HB_PLAY2_SetEnableFlag(m_hPlay, flags | ~HBPLAY2_ENABLE_AUDIO_CODEC |~ HBPLAY2_ENABLE_SOUND);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerHB::SetVolume(WORD nVolume)
{
	HRESULT hRet = HB_PLAY2_SetVolume(m_hPlay, nVolume);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD CPlayerHB::GetVolume()
{
	DWORD dVolume=0;
	HRESULT hRet = HB_PLAY2_GetVolume(m_hPlay, &dVolume);
	return dVolume;
}

BOOL  CPlayerHB::SetPlayPos(float fRelativePos)
{
	HRESULT hRet = HB_PLAY2_SeekByRatio(m_hPlay, fRelativePos);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CPlayerHB::GetPlayPos()
{
	DWORD dCurTime = 0;
	DWORD dFileTime = 0;
	HB_PLAY2_GetPlayDuration(m_hPlay, &dCurTime);
	HB_PLAY2_GetFileDuration(m_hPlay, &dFileTime);
	return (float)dCurTime / (float)dFileTime;
}

DWORD CPlayerHB::GetFileTime()
{
	DWORD dTime;
	HB_PLAY2_GetFileDuration(m_hPlay, &dTime);
	return dTime/1000;
}

DWORD CPlayerHB::GetPlayedTime()
{
	DWORD dTime;
	HB_PLAY2_GetPlayDuration(m_hPlay, &dTime);
	return dTime/1000;
}

BOOL CPlayerHB::SetPlayedTimeEx(DWORD nTime)
{
	HRESULT hRet = HB_PLAY2_SeekByTime(m_hPlay, nTime);
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL  CPlayerHB::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return HB_PLAY2_GetPictureSize(m_hPlay, (int*)pWidth, (int*)pHeight);
}

BOOL  CPlayerHB::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	HBPLAY2_COLOR_SPACE color;
	color.dwBrightness = nBrightness;
	color.dwContrast = nContrast;
	color.dwSaturation = nSaturation;
	color.dwHue = nHue;
	color.dwSize = sizeof(HBPLAY2_COLOR_SPACE);
	HRESULT hRet = HB_PLAY2_SetVideoColor(m_hPlay, &color);

	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	return false;
}

BOOL  CPlayerHB::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	HBPLAY2_COLOR_SPACE color;	
	color.dwSize = sizeof(HBPLAY2_COLOR_SPACE);

	HRESULT hRet = HB_PLAY2_GetVideoColor(m_hPlay, &color);
	*pBrightness=color.dwBrightness;
	*pContrast=color.dwContrast;
	*pSaturation=color.dwSaturation;
	*pHue=color.dwHue
		;
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	return false;
}

BOOL  CPlayerHB::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	HB_PLAY2_RegisterFileEndedCallback(m_hPlay,(PHB_PLAY2_FILE_ENDED_PROC)callBack, pUser);
	return true;
}


BOOL CPlayerHB::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	return false;
	bool bRet = HB_PLAY2_RegisterDecodeCallback(m_hPlay, 0, (PHB_PLAY2_DECODE_PROC)&DecCBFun, (PVOID)this);
	if (bRet)
	{
		m_DisplayCallbackFun = displayCallback;
		m_DisplayCalUser = nUser;
	}
	return bRet;
}

void  CPlayerHB::DecCBFun(IN HHBPLAY2 hPlay, IN const HBPLAY2_FRAME* pFrame, IN PVOID pContext)
{
	CPlayerHB *pPlayer = (CPlayerHB*)pContext;

	if (pPlayer&&pPlayer->m_DisplayCallbackFun&&
		pFrame->FrameType == HBPLAY2_FRAME_VIDEO_I || pFrame->FrameType == HBPLAY2_FRAME_VIDEO_P || pFrame->FrameType == HBPLAY2_FRAME_VIDEO_B || pFrame->FrameType == HBPLAY2_FRAME_VIDEO_E )
	{
		DISPLAYCALLBCK_INFO displayInfo;
		displayInfo.pBuf = (char*)pFrame->u.Video.pBuffer;
		displayInfo.nBufLen = pFrame->u.Video.dwBufferLength;
		displayInfo.nWidth = pFrame->u.Video.nWidth;
		displayInfo.nHeight = pFrame->u.Video.nHeight;
		displayInfo.nStamp = pPlayer->GetPlayedTime();
		displayInfo.nStamp = pFrame->u.Video.TimeStamp.wSecond;
		displayInfo.nUser = (long)pPlayer->m_DisplayCalUser;
		pPlayer->m_DisplayCallbackFun(&displayInfo);

	}
}

BOOL  CPlayerHB::CapturePic(char *pSaveFile, int iType)
{
	HRESULT hRet = 0;
	if (iType == 0)
	{
		hRet = HB_PLAY2_SnapshotToBmpFileA(m_hPlay, pSaveFile);
	}
	else
	{
		hRet = HB_PLAY2_SnapshotToJpegFileA(m_hPlay, pSaveFile, 100);
	}
	if (HBPLAY2_OK == hRet)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CPlayerHB::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerHB::FileCutClose()
{
	return false;
}

int CPlayerHB::FileCutProcess()
{
	return 0;
}
