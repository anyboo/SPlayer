#include "stdafx.h"

#include "ffplayerImp.h"

#include "ffplayer.h"

#include <mutex>
#include <list>

struct FFPlayer_Info{
	int pid;// 播放器ID
	ffplayer *pPlayer;
};

static std::list<FFPlayer_Info*> s_ffplayerList;
std::recursive_mutex s_mutexffplayers;
static int nIndex = 0;

BOOL FFPlayer_Init()
{
	
	avcodec_register_all();
	
	return true;
}

BOOL  FFPlayer_Release()
{
	return false;
}

FFPlayer_Info * FindPlayer(LONG pID)//查找对应播放器
{
	std::list<FFPlayer_Info*>::iterator iter = s_ffplayerList.begin();
	while (iter != s_ffplayerList.end())
	{
		FFPlayer_Info *pPlayerInfo = (FFPlayer_Info*)*iter;
		if (pID == pPlayerInfo->pid)
		{
			return pPlayerInfo;
		}
		iter++;
	}
	return NULL;
}

BOOL FFPlayer_GetPort(LONG *nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	nIndex++;
	std::list<FFPlayer_Info*>::iterator iter = s_ffplayerList.begin();
	while (iter != s_ffplayerList.end())
	{
		FFPlayer_Info *pffplayerInfo = (FFPlayer_Info*)*iter;
		if (nIndex == pffplayerInfo->pid)
		{
			return FFPlayer_GetPort(nID);
		}
		iter++;
	}
	*nID = nIndex;
	return true;
}

BOOL FFPlayer_FreePort(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	std::list<FFPlayer_Info*>::iterator iter = s_ffplayerList.begin();
	while (iter != s_ffplayerList.end())
	{
		FFPlayer_Info *pffplayerInfo = (FFPlayer_Info*)*iter;
		if (nID == pffplayerInfo->pid)
		{
			delete pffplayerInfo->pPlayer;
			pffplayerInfo->pPlayer = NULL;
			delete pffplayerInfo;
			s_ffplayerList.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}

BOOL FFPlayer_OpenFile(LONG nID, char *pFile)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = new FFPlayer_Info;
	pPlayerInfo->pPlayer = new ffplayer();
	pPlayerInfo->pid = nID;
	s_ffplayerList.push_back(pPlayerInfo);
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->OpenFile(pFile);
	}
	return false;
}

BOOL FFPlayer_Play(LONG nID,HWND hwnd)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->Play(hwnd);
	}
	return false;
}

BOOL FFPlayer_Pause(LONG nID, DWORD nPause)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->Pause(nPause);
	}
	return false;
}

BOOL FFPlayer_Stop(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->Stop();
	}
	return false;
}

BOOL FFPlayer_Fast(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->Fast();
	}
	return false;
}

BOOL FFPlayer_Slow(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->Slow();
	}
	return false;
}

BOOL FFPlayer_OneByOne(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->OneByOne();
	}
	return false;
}

BOOL FFPlayer_OneByOneBack(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->OneByOneBack();
	}
	return false;
}
BOOL FFPlayer_PlaySoundShare(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->PlaySoundShare();
	}
	return false;
}

BOOL FFPlayer_StopSoundShare(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->StopSoundShare();
	}
	return false;
}

BOOL FFPlayer_SetVolume(LONG nID, DWORD nVolume)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->SetVolume(nVolume);
	}
	return false;
}

DWORD FFPlayer_GetVolume(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->GetVolume();
	}
	return 0;
}

BOOL  FFPlayer_SetPlayPos(LONG nID, float fRelativePos)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->SetPlayPos(fRelativePos);
	}
	return false;
}

float  FFPlayer_GetPlayPos(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->GetPlayPos();
	}
	return 0.0;
}

DWORD FFPlayer_GetFileTime(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->GetFileTime();
	}
	return 0;
}

DWORD  FFPlayer_GetPlayedTime(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->GetPlayedTime();
	}
	return 0;
}

BOOL FFPlayer_GetPictureSize(LONG nID, LONG *pWidth, LONG *pHeight)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->GetPictureSize(pWidth,pHeight);
	}
	return false;
}

BOOL FFPlayer_CapturePic(LONG nID, char *pSaveFile, int iType)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->CapturePic(pSaveFile, iType);
	}
	return false;
}

BOOL FFPlayer_SetFileEndCallback(LONG nID, FileEndCallback fileCallBack, void *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo->pPlayer)
	{
		return pPlayerInfo->pPlayer->SetFileEndCallback(nID,fileCallBack, pUser);
	}
	return false;
}

//剪切与转换调用同一接口
BOOL FFPlayer_FileCutStart(LONG nID, const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime)
{
	return FFPlayer_FileConvertStart(nID, srcFileName, destFileName, startTime, endTime, 0, NULL);
	/*std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo->pPlayer)
	{
		return pPlayerInfo->pPlayer->FileConvertStart(srcFileName, destFileName, startTime, endTime,0,NULL);
		//return pPlayerInfo->pPlayer->FileCutStart(srcFileName, destFileName, startTime, endTime);
	}
	return false;*/
}

BOOL FFPlayer_FileCutClose(LONG nID)
{
	/*
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo->pPlayer)
	{
	//	return pPlayerInfo->pPlayer->FileCutClose();
		return pPlayerInfo->pPlayer->FileConvertClose();
	}
	return false;*/
	return FFPlayer_FileConvertClose(nID);
}

int FFPlayer_FileCutProcess(LONG nID)
{
	/*
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo->pPlayer)
	{
		return pPlayerInfo->pPlayer->FileConvertProcess();
		//return pPlayerInfo->pPlayer->FileCutProcess();
	}
	return false;*/
	return FFPlayer_FileConvertProcess(nID);
}


BOOL FFPlayer_FileCutPause(LONG nID, DWORD nPause)
{
	/*
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);
	if (pPlayerInfo->pPlayer)
	{
	//	return pPlayerInfo->pPlayer->FileCutPause(nPause);
		return pPlayerInfo->pPlayer->FileConvertPause(nPause);
	}
	return false;*/
	return FFPlayer_FileConvertPause(nID, nPause);
}

BOOL FFPlayer_FileConvertStart(LONG nID, const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = new FFPlayer_Info;
	pPlayerInfo->pPlayer = new ffplayer();
	pPlayerInfo->pid = nID;
	s_ffplayerList.push_back(pPlayerInfo);
	if (pPlayerInfo->pPlayer)
	{
		return pPlayerInfo->pPlayer->FileConvertStart(srcFileName, destFileName, startTime, endTime, bConvert, pConvertPara);
	}
	return false;
}

BOOL FFPlayer_FileConvertClose(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->FileConvertClose();
	}
	return false;
}

int FFPlayer_FileConvertProcess(LONG nID)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);;
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->FileConvertProcess();
	}
	return false;
}

BOOL FFPlayer_FileConvertPause(LONG nID,DWORD nPause)
{
	std::lock_guard<std::recursive_mutex> lock(s_mutexffplayers);
	FFPlayer_Info *pPlayerInfo = FindPlayer(nID);
	if (pPlayerInfo)
	{
		return pPlayerInfo->pPlayer->FileConvertPause(nPause);
	}
	return false;
}

