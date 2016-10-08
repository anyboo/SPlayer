#include "stdafx.h"
#include "PlayerDiZhiPu.h"

#include <stdio.h>

#include "../inc/H264Play.h"
#pragma comment(lib,"../lib/H264Play.lib")


char* CPlayerFactoryDiZhiPu::Name()
{
	return "迪智浦";
}

BOOL CPlayerFactoryDiZhiPu::IsBelongThisHead(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);
		fclose(pfd);

		if (strncmp(buf, "264DV", 4) == 0)
		{
			return true;
		}		
	}

	return false;
}


BOOL CPlayerFactoryDiZhiPu::IsBelongThis(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);
		fclose(pfd);

		if (strncmp(buf, "HISI", 4) == 0)//“泰康伟业.h264”，字节头"HISI",不在此播放，返回false,让标准库播放
		{
			return false;
		}
	}

	if (strstr(pFile, ".h264") || strstr(pFile, ".H264"))//判断h264文件，有些播不了“泰康伟业.h264”，字节头"HISI"
	{
		return true;
	}

	return false;
}

BOOL CPlayerFactoryDiZhiPu::Init()
{
	return true;
}

void CPlayerFactoryDiZhiPu::Release()
{

}

IPlayer *CPlayerFactoryDiZhiPu::CreatePlayer()
{
	return new CPlayerDiZhiPu();
}

/////////////
CPlayerDiZhiPu::CPlayerDiZhiPu() :m_bPause(false),m_process(0)
{
}


CPlayerDiZhiPu::~CPlayerDiZhiPu()
{
}


BOOL CPlayerDiZhiPu::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;
	if (H264_PLAY_GetPort(&m_nPort))
	{
		return H264_PLAY_OpenFile(m_nPort, PFile);
	}
	return false;
}

BOOL CPlayerDiZhiPu::Play()
{
	return H264_PLAY_Play(m_nPort, m_hwnd);
}

BOOL CPlayerDiZhiPu::Pause(DWORD nPause)
{
	m_bPause = nPause;
	return H264_PLAY_Pause(m_nPort, nPause);
}

BOOL CPlayerDiZhiPu::Stop()
{
	H264_PLAY_Stop(m_nPort);
	H264_PLAY_CloseFile(m_nPort);
	return H264_PLAY_FreePort(m_nPort);
}


BOOL CPlayerDiZhiPu::Fast()
{
	return H264_PLAY_Fast(m_nPort);
}

BOOL CPlayerDiZhiPu::Slow()
{
	return H264_PLAY_Slow(m_nPort);
}

BOOL CPlayerDiZhiPu::OneByOne()
{
	return H264_PLAY_NextFrame(m_nPort);
}

BOOL CPlayerDiZhiPu::OneByOneBack()
{
	return H264_PLAY_PrevFrame(m_nPort);
}

BOOL CPlayerDiZhiPu::PlaySoundShare()
{
	return H264_PLAY_PlaySoundShare(m_nPort);

}

BOOL  CPlayerDiZhiPu::StopSoundShare()
{
	return H264_PLAY_StopSoundShare(m_nPort);
}

BOOL CPlayerDiZhiPu::SetVolume(WORD nVolume)
{
	return H264_PLAY_SetVolume(m_nPort, nVolume);
}

DWORD CPlayerDiZhiPu::GetVolume()
{
	return H264_PLAY_GetVolume(m_nPort);
}

BOOL  CPlayerDiZhiPu::SetPlayPos(float fRelativePos)
{
	if (m_bPause)
	{
		H264_PLAY_Pause(m_nPort, 0);
		Sleep(100);
		H264_PLAY_SetPlayPos(m_nPort, fRelativePos);
		Sleep(100);
		return H264_PLAY_Pause(m_nPort, 1);
	}
	else
	{
		return H264_PLAY_SetPlayPos(m_nPort, fRelativePos);
	}
}

float CPlayerDiZhiPu::GetPlayPos()
{
	return H264_PLAY_GetPlayPos(m_nPort);
}

DWORD CPlayerDiZhiPu::GetFileTime()
{
	return H264_PLAY_GetFileTime(m_nPort);
}

BOOL  CPlayerDiZhiPu::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return H264_PLAY_GetPictureSize(m_nPort, pWidth, pHeight);
}

BOOL  CPlayerDiZhiPu::GetSystemTime(unsigned long long *pstSystemTime)
{
	*pstSystemTime = H264_PLAY_GetCurTimeStamp(m_nPort);
	return true;
}

DWORD CPlayerDiZhiPu::GetPlayedTime()
{
	return H264_PLAY_GetPlayedTime(m_nPort);
}

BOOL CPlayerDiZhiPu::SetPlayedTimeEx(DWORD nTime)
{
	return H264_PLAY_SetPlayedTimeEx(m_nPort, nTime);
}

BOOL  CPlayerDiZhiPu::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return H264_PLAY_SetFileEndCallBack(m_nPort, (FileCallBack)callBack, (long)pUser);
}

BOOL  CPlayerDiZhiPu::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	bool bRet =  H264_PLAY_SetDisplayCallBack(m_nPort, (DisplayCallBack)&DisplayCBFunBack, (LONG)this);
	if (bRet)
	{
		m_DisplayCallbackFun = displayCallback;
		m_DisplayCalUser = nUser;
	}
	return bRet;
}

void CPlayerDiZhiPu::DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long lUser)
{
	CPlayerDiZhiPu *pPlayer = (CPlayerDiZhiPu*)lUser;

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

BOOL  CPlayerDiZhiPu::CapturePic(char *pSaveFile, int iType)
{
	return H264_PLAY_CatchPic(m_nPort, pSaveFile);
}

BOOL  CPlayerDiZhiPu::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	return H264_PLAY_SetColor(m_nPort, nRegionNum, nBrightness, nContrast, nSaturation, nHue);
}

BOOL  CPlayerDiZhiPu::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	return H264_PLAY_GetColor(m_nPort, nRegionNum, (long*)pBrightness, (long*)pContrast, (long*)pSaturation, (long*)pHue);
}

BOOL CPlayerDiZhiPu::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerDiZhiPu::FileCutClose()
{
	return false;
}

int CPlayerDiZhiPu::FileCutProcess()
{
	return 0;
}

void CPlayerDiZhiPu::CoverFileCallBackFun(DWORD CurrentPos, DWORD TotoalPos, long dwUser)//转换进度
{
	CPlayerDiZhiPu *pPlayer = (CPlayerDiZhiPu*)dwUser;
	pPlayer->m_process=CurrentPos * 100 / TotoalPos;
}

BOOL CPlayerDiZhiPu::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	H264_PLAY_ConvertFile(srcFileName, destFileName, 1, CoverFileCallBackFun, (DWORD)this);
	return true;
}

BOOL CPlayerDiZhiPu::FileConvertClose()
{
	return false;
}

int CPlayerDiZhiPu::FileConvertProcess()
{
	return m_process;
}

BOOL CPlayerDiZhiPu::FileConvertPause(DWORD nPause)
{
	return false;
}