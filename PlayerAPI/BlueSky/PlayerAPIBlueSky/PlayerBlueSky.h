#pragma once
#include "../../../PlayerFactory/IPlayer.h"

#include "../inc/dvxSdk.h"
#include "../inc/dvxSdkType.h"
#pragma comment(lib,"../lib/dvxSDK.lib")

class CPlayerFactoryBlueSky :public IPlayerFactory
{
public:
	CPlayerFactoryBlueSky(){}
	virtual ~CPlayerFactoryBlueSky(){}
public:
	virtual BOOL IsSupportFastCut();
	virtual BOOL IsBelongThis(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerBlueSky :
	public IPlayer
{
public:
	CPlayerBlueSky();
	virtual ~CPlayerBlueSky();
	BOOL OpenFile(char *PFile, HWND hwnd);

	BOOL Play();
	BOOL Pause(DWORD nPause);
	BOOL Stop();
	BOOL Fast();
	BOOL Slow();
	BOOL OneByOne();
	BOOL OneByOneBack();
	BOOL PlaySoundShare();
	BOOL StopSoundShare();
	BOOL SetVolume(WORD nVolume);
	DWORD GetVolume();

	BOOL  SetPlayPos(float fRelativePos);
	float GetPlayPos();
	DWORD  GetFileTime();
	DWORD  GetPlayedTime();
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);

	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();
private:
	static int NotifyRoutineCallBack(unsigned int notifyId, LPVOID lpUserData);
private:
	HWND m_hwnd;
	LocalPlayHandle m_hLocalPlay;
	int m_iSpeed;
	int m_snRate;
	BOOL m_bPause;

	FileCutHandle m_cutHandle;
};

