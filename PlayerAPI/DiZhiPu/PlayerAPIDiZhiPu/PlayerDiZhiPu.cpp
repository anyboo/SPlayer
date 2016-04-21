#include "stdafx.h"
#include "PlayerDiZhiPu.h"

#include <stdio.h>

#include "../inc/H264Play.h"
#pragma comment(lib,"../lib/H264Play.lib")


char* CPlayerFactoryDiZhiPu::Name()
{
	return "迪智浦";
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

		if (strncmp(buf, "264DV", 4) == 0)
		{
			return true;
		}
		fclose(pfd);
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

DWORD CPlayerDiZhiPu::GetPlayedTime()
{
	return H264_PLAY_GetPlayedTime(m_nPort);
}

BOOL  CPlayerDiZhiPu::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return H264_PLAY_SetFileEndCallBack(m_nPort, (FileCallBack)callBack, (long)pUser);
}

BOOL  CPlayerDiZhiPu::CapturePic(char *pSaveFile, int iType)
{
	return H264_PLAY_CatchPic(m_nPort, pSaveFile);
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
	H264_PLAY_ConvertFile(srcFileName, destFileName, 0, CoverFileCallBackFun, (DWORD)this);
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