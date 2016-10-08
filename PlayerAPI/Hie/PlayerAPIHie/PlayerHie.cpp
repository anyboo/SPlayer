#include "stdafx.h"
#include "PlayerHie.h"

#include <stdio.h>

#include "../inc/HIE_MP4Play.h"
#pragma comment(lib,"../lib/HIE_MP4Play.lib")


char* CPlayerFactoryHie::Name()
{
	return "HIE";
}
BOOL CPlayerFactoryHie::IsBelongThisHead(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[32];
		memset(buf, 0, 32);
		fread(buf, 31, 1, pfd);
		fclose(pfd);

		char *pHead = strstr(buf, "MPG4");//为NULL
		if (buf[4] == 'M' && buf[5] == 'P' && buf[6] == 'G' && buf[7] == '4')
		{	
			return true;
		}
	}
	return false;

}
BOOL CPlayerFactoryHie::IsBelongThis(char *pFile)
{
	if (strstr(pFile, ".HE4") || strstr(pFile, ".hie264") || strstr(pFile, ".mg4"))//判断h264文件
	{
		return true;
	}
	return false;
}

BOOL CPlayerFactoryHie::Init()
{
	return true;
}

void CPlayerFactoryHie::Release()
{

}

IPlayer *CPlayerFactoryHie::CreatePlayer()
{
	return new CPlayerHie();
}

/////////////
CPlayerHie::CPlayerHie() :m_bPause(false),m_process(0)
{
}


CPlayerHie::~CPlayerHie()
{
}


BOOL CPlayerHie::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;
	if (MP4Play_LockNewPort(&m_nPort))
	{
		return MP4Play_OpenFile(m_nPort, PFile);
	}
	return false;
}

BOOL CPlayerHie::Play()
{
	return MP4Play_Play(m_nPort, m_hwnd);
}

BOOL CPlayerHie::Pause(DWORD nPause)
{
	m_bPause = nPause;
	return MP4Play_Pause(m_nPort, nPause);
}

BOOL CPlayerHie::Stop()
{
	MP4Play_Stop(m_nPort);
	MP4Play_CloseFile(m_nPort);
	return MP4Play_UnlockPort(m_nPort);
}


BOOL CPlayerHie::Fast()
{
	return MP4Play_Fast(m_nPort);
}

BOOL CPlayerHie::Slow()
{
	return MP4Play_Slow(m_nPort);
}

BOOL CPlayerHie::OneByOne()
{
	return MP4Play_OneByOne(m_nPort);

}

BOOL CPlayerHie::OneByOneBack()
{
	return MP4Play_OneByOneBack(m_nPort);
}

BOOL CPlayerHie::PlaySoundShare()
{
	return MP4Play_PlaySoundShare(m_nPort);

}

BOOL  CPlayerHie::StopSoundShare()
{
	return MP4Play_StopSoundShare(m_nPort);
}

BOOL CPlayerHie::SetVolume(WORD nVolume)
{
	return MP4Play_SetVolume(m_nPort, nVolume);
}

DWORD CPlayerHie::GetVolume()
{
	return  MP4Play_GetVolume(m_nPort);
}

BOOL  CPlayerHie::SetPlayPos(float fRelativePos)
{
	if (m_bPause)
	{
		MP4Play_Pause(m_nPort, 0);
		Sleep(100);
		MP4Play_SetPlayPos(m_nPort, fRelativePos);
		Sleep(100);
		return  MP4Play_Pause(m_nPort, 1);
	}
	else
	{
		return  MP4Play_SetPlayPos(m_nPort, fRelativePos);
	}
}

float CPlayerHie::GetPlayPos()
{
	return MP4Play_GetPlayPos(m_nPort);
}

DWORD CPlayerHie::GetFileTime()
{
	return  MP4Play_GetFileTime(m_nPort);
}

BOOL  CPlayerHie::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return  MP4Play_GetPictureSize(m_nPort, pWidth, pHeight);
}

BOOL  CPlayerHie::GetSystemTime(unsigned long long *pstSystemTime)
{
	*pstSystemTime = MP4Play_GetAbsoluteTime(m_nPort);
	return true;
}

DWORD CPlayerHie::GetPlayedTime()
{
	return MP4Play_GetPlayedTime(m_nPort);
}

BOOL CPlayerHie::SetPlayedTimeEx(DWORD nTime)
{
	return MP4Play_SetPlayedTimeEx(m_nPort, nTime);
}

BOOL  CPlayerHie::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	//return MP4Play_SetFileRefCallBack(m_nPort, callBack, (long)pUser);
	return false;
}

BOOL  CPlayerHie::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	//bool bRet = MP4Play_SetDisplayCallBack(m_nPort, (DisplayCallBack)&DisplayCBFunBack, (LONG)this);

/*	bool bRet = MP4Play_SetDecCallBackUser
	if (bRet)
	{
		m_DisplayCallbackFun = displayCallback;
		m_DisplayCalUser = nUser;
	}
	return bRet;*/
	return false;
}

void CPlayerHie::DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long lUser)
{
	CPlayerHie *pPlayer = (CPlayerHie*)lUser;

	if (pPlayer&&pPlayer->m_DisplayCallbackFun)
	{
		DISPLAYCALLBCK_INFO displayInfo;
		displayInfo.pBuf = pBuf;
		displayInfo.nBufLen = nSize;
		displayInfo.nWidth = nWidth;
		displayInfo.nHeight = nHeight;
		displayInfo.nStamp = pPlayer->GetPlayedTime();
		displayInfo.nUser = (long)pPlayer->m_DisplayCalUser;
		pPlayer->m_DisplayCallbackFun(&displayInfo);

	}
}

BOOL  CPlayerHie::CapturePic(char *pSaveFile, int iType)
{
	//return MP4Play_CatchPic(m_nPort, pSaveFile);
	return false;
}

BOOL  CPlayerHie::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	return MP4Play_SetColor(m_nPort, nRegionNum, nBrightness, nContrast, nSaturation, nHue);
}

BOOL  CPlayerHie::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	return MP4Play_GetColor(m_nPort, nRegionNum, (int*)pBrightness, (int*)pContrast, (int*)pSaturation, (int*)pHue);
}

BOOL CPlayerHie::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerHie::FileCutClose()
{
	return false;
}

int CPlayerHie::FileCutProcess()
{
	return 0;
}

void CPlayerHie::CoverFileCallBackFun(DWORD CurrentPos, DWORD TotoalPos, long dwUser)//转换进度
{
	CPlayerHie *pPlayer = (CPlayerHie*)dwUser;
	pPlayer->m_process=CurrentPos * 100 / TotoalPos;
}

BOOL CPlayerHie::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	return false;
}

BOOL CPlayerHie::FileConvertClose()
{
	return false;
}

int CPlayerHie::FileConvertProcess()
{
	return m_process;
}

BOOL CPlayerHie::FileConvertPause(DWORD nPause)
{
	return false;
}