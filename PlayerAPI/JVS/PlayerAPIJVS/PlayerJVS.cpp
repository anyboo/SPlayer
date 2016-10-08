#include "stdafx.h"
#include "PlayerJVS.h"

#include "../inc/JVSDKP.h"
#pragma comment(lib,"../lib/JVSDKP.lib")

#include <stdio.h>


char* CPlayerFactoryZhongWei::Name()
{
	return "中维世纪";
}

BOOL CPlayerFactoryZhongWei::IsBelongThisHead(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);
		fclose(pfd);

		if (strncmp(buf, "JVS", 3) == 0)//
		{
			return true;
		}
	}

	return false;
}


BOOL CPlayerFactoryZhongWei::Init()
{
	JVSP_InitSDK(false);
	return true;
}

void CPlayerFactoryZhongWei::Release()
{
	JVSP_ReleaseSDK();
}

IPlayer *CPlayerFactoryZhongWei::CreatePlayer()
{
	return new CPlayerZhongWei();
}
/////////////

CPlayerZhongWei::CPlayerZhongWei():m_fps(25)
{
}


CPlayerZhongWei::~CPlayerZhongWei()
{
}


BOOL CPlayerZhongWei::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;

	int handle = JVSP_CreatePlayer(true);
	if (handle == -1)
	{
		return false;
	}
	else
	{
		m_nPort = handle;
		//return JVSP_OpenFile(m_nPort, PFile);
		return JVSP_OpenFile(m_nPort, PFile);
		//JVS_FILE_HEADER headerData;
		//JVSP_GetHeaderData(m_nPort, &headerData);
		//char buf[256];
		//sprintf_s(buf, "%d", headerData.dwFLAGS);
		return true;
	}
	return false;
}

BOOL CPlayerZhongWei::Play()
{
	RECT rc;
	::GetWindowRect(m_hwnd, &rc);

	JVSP_SetVideoPreview(m_nPort, m_hwnd, &rc, TRUE);
	return JVSP_Play(m_nPort);
}

BOOL CPlayerZhongWei::Pause(DWORD nPause)
{
	if (nPause)
	{
		JVSP_Pause(m_nPort);
	}
	else
	{
		Play();
	}
	return true;
}

BOOL CPlayerZhongWei::Stop()
{
	JVSP_Stop(m_nPort);
	JVSP_ReleasePlayer(m_nPort);
	return true;
}

BOOL CPlayerZhongWei::Fast()
{
	JVSP_SetFPS(m_nPort, 1, m_fps*2);
	return true;
}

BOOL CPlayerZhongWei::Slow()
{
	JVSP_SetFPS(m_nPort,0, m_fps/2);
	return true;
}

BOOL  CPlayerZhongWei::SetPlayPos(float fRelativePos)
{
	unsigned long  frameCount = JVSP_GetTotalFrames(m_nPort);
	unsigned long frameNum = frameCount*fRelativePos;
	unsigned long uKeyFrame;
	JVSP_GetKeyFramePos(m_nPort,&uKeyFrame, &frameNum);
	return JVSP_GotoKeyFrame(m_nPort, uKeyFrame,0);
}

float CPlayerZhongWei::GetPlayPos()
{
	int curFrameNum = JVSP_GetCurFrameNum(m_nPort);
	int frameCount = JVSP_GetTotalFrames(m_nPort);
	float fpos = (float)curFrameNum / (float)frameCount;
	return fpos;
}

DWORD CPlayerZhongWei::GetFileTime()
{
	unsigned int  count = JVSP_GetTotalFrames(m_nPort);
	unsigned long uTime = (count + m_fps / 2) / m_fps;
	return uTime;
}

DWORD CPlayerZhongWei::GetPlayedTime()
{
	int curFrameNum = JVSP_GetCurFrameNum(m_nPort);
	unsigned long uTime = ((float)(curFrameNum + m_fps / 2)) / (float)m_fps;
	return uTime;
}