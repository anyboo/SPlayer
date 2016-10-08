#include "stdafx.h"
#include "PlayerDL.h"

#include "../inc/netsdk.h"
#pragma comment(lib,"../lib/netsdk.lib")

#include <stdio.h>


int g_iSpeedCmd[] = { NET_DLDVR_PLAYSLOW, NET_DLDVR_PLAY_HALFSPEED,NET_DLDVR_PLAYNORMAL, NET_DLDVR_PLAY_DOUBLESPEED, NET_DLDVR_PLAYFAST, NET_DLDVR_PLAY_OCTUPLESPEED, NET_DLDVR_PLAY_SIXTEENSPEED };

char* CPlayerFactoryDL::Name()
{
	return "大立";
}

BOOL CPlayerFactoryDL::IsBelongThisHead(char *pFile)
{
	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, pFile, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);
		fclose(pfd);

		if (strncmp(buf, "DALI", 4) == 0)//DL
		{
			return true;
		}
		
	}
	return false;
}

BOOL CPlayerFactoryDL::Init()
{
	return NET_DLDVR_Init();
}

void CPlayerFactoryDL::Release()
{
	NET_DLDVR_Cleanup();
}

IPlayer *CPlayerFactoryDL::CreatePlayer()
{
	return new CPlayerDL();
}
//////////

CPlayerDL::CPlayerDL() : m_fileTime(0), m_iSpeed(2), m_bPause(false), m_process(0)
{
}


CPlayerDL::~CPlayerDL()
{
}


BOOL CPlayerDL::OpenFile(char *PFile, HWND hwnd)
{
	m_hwnd = hwnd;

	int handle = NET_DLDVR_PlayFiles(PFile, m_hwnd);
	if (handle == -1)
	{
		return false;
	}
	else
	{
		m_nPort = handle;

		DWORD dBeginTime = 0;
		DWORD dEndTime = 0;
		NET_DLDVR_GetLocalFileTime(PFile, dBeginTime, dEndTime);
		m_dBeginTime = dBeginTime;
		m_fileTime = dEndTime - dBeginTime;
			
	}
	return true;
}

BOOL CPlayerDL::Play()
{

	return true ;
}

BOOL CPlayerDL::Pause(DWORD nPause)
{
	m_bPause = nPause;
	if (nPause)
	{
		NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYPAUSE, 0, 0);
	}
	else
	{
		NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYRESTART, 0, 0);
	}
	return true;
}

BOOL CPlayerDL::Stop()
{
	NET_DLDVR_StopPlayBack(m_nPort);
	return true;
}

BOOL CPlayerDL::Fast()
{
	if (m_iSpeed == 6)
	{
		return false;
	}
	m_iSpeed++;
	
	int cmd = g_iSpeedCmd[m_iSpeed];
	return NET_DLDVR_PlayBackControl(m_nPort, cmd, 0, 0);

}

BOOL CPlayerDL::Slow()
{
	if (m_iSpeed==0)
	{
		return false;
	}
	m_iSpeed--;

	int cmd = g_iSpeedCmd[m_iSpeed];
	return NET_DLDVR_PlayBackControl(m_nPort, cmd, 0, 0);
}


BOOL CPlayerDL::OneByOne()
{
	return NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYFRAME, 0, 0);
}

BOOL CPlayerDL::OneByOneBack()
{
	return false;
}

BOOL CPlayerDL::PlaySoundShare()
{
	return NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYSTARTAUDIO, 0, 0);

}

BOOL  CPlayerDL::StopSoundShare()
{
	return NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYSTOPAUDIO, 0, 0);
}

BOOL CPlayerDL::SetVolume(WORD nVolume)
{
	return NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYAUDIOVOLUME, nVolume, 0);
}

DWORD CPlayerDL::GetVolume()
{
	DWORD volume=0;
	bool bRet = NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_GETPLAYAUDIOVOLUME, 0, (DWORD*)&volume);
	return volume;
}

BOOL  CPlayerDL::SetPlayPos(float fRelativePos)
{
	int iPos = fRelativePos*100;
	if (m_bPause)
	{
		NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYRESTART, 0, 0);
		Sleep(10);
		NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYSETPOS, iPos, 0);
		Sleep(10);
		NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYPAUSE, 0, 0);
	}
	else
	{
		return NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYSETPOS, iPos, 0);
	}
}

float CPlayerDL::GetPlayPos()
{
	int dpos;
	NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYGETPOS, 0, (DWORD*)&dpos);
	return (float)dpos/(float)100;
}

DWORD CPlayerDL::GetFileTime()
{
	return m_fileTime;
}

DWORD CPlayerDL::GetPlayedTime()
{
	DWORD dTime;
	NET_DLDVR_PlayBackControl(m_nPort, NET_DLDVR_PLAYGETTIME, 0, (DWORD*)&dTime);
	return dTime - m_dBeginTime;
}

BOOL  CPlayerDL::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	return false;
}

BOOL  CPlayerDL::SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	return NET_DLDVR_SetColor(m_nPort, nRegionNum, nBrightness, nContrast, nSaturation, nHue);
}

BOOL  CPlayerDL::GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	return NET_DLDVR_GetColor(m_nPort, nRegionNum, pBrightness, pContrast, pSaturation, pHue);
}

BOOL  CPlayerDL::SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser)
{
	return false;
}

BOOL  CPlayerDL::SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser)
{
	bool bRet = NET_DLDVR_SetYUVDataCallBack(m_nPort, DisplayCBFunBack, (LONG)this);
	if (bRet)
	{
		m_DisplayCallbackFun = displayCallback;
		m_DisplayCalUser = nUser;
	}
	return bRet;
}

void CPlayerDL::DisplayCBFunBack(LONG lHandle, DWORD /*dwDataType*/dwFrameRate, BYTE *pBuffer,
	DWORD nWidth, DWORD nHeight, int err, DWORD dwUser)
{
	CPlayerDL *pPlayer = (CPlayerDL*)dwUser;

	if (pPlayer&&pPlayer->m_DisplayCallbackFun)
	{
		DISPLAYCALLBCK_INFO displayInfo;
		displayInfo.pBuf = (char*)pBuffer;
		displayInfo.nBufLen = nWidth*nHeight*3/2;
		displayInfo.nWidth = nWidth;
		displayInfo.nHeight = nHeight;
		displayInfo.nStamp = pPlayer->GetPlayedTime();
		displayInfo.nUser = (long)pPlayer->m_DisplayCalUser;
		pPlayer->m_DisplayCallbackFun(&displayInfo);

	}
}

BOOL  CPlayerDL::CapturePic(char *pSaveFile, int iType)
{
	return NET_DLDVR_CapturePicture(m_nPort, pSaveFile);
}

BOOL CPlayerDL::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast)
{
	return false;
}

BOOL CPlayerDL::FileCutClose()
{
	return false;
}

int CPlayerDL::FileCutProcess()
{
	return 0;
}

INT CPlayerDL::ConvertWMVCallBackFun(INT pos, void* cbpara)//转换进度
{
	CPlayerDL *pPlayer = (CPlayerDL*)cbpara;
	pPlayer->m_process = pos;
	return pos;
}

BOOL CPlayerDL::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	int nRet = NET_DLDVR_MP4ToWMV((char*)srcFileName, (char*)destFileName, ConvertWMVCallBackFun,(void*)this);
	if (nRet>=0)
		return true;
	else
		return false;
}

BOOL CPlayerDL::FileConvertClose()
{
	return false;
}

int CPlayerDL::FileConvertProcess()
{
	return m_process;
}

BOOL CPlayerDL::FileConvertPause(DWORD nPause)
{
	return false;
}