#pragma once
#include "../../../PlayerFactory/IPlayer.h"

#include "../inc/HBPlaySDK.h"
#pragma comment(lib,"../lib/HBPlaySDK.lib")

class CPlayerFactoryHB :public IPlayerFactory
{
public:
	CPlayerFactoryHB(){}
	virtual ~CPlayerFactoryHB(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerHB :
	public IPlayer
{
public:
	CPlayerHB();
	virtual ~CPlayerHB();
	BOOL OpenFile(char *PFile, HWND hwnd);
	//BOOL CloseFile() ;

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
	BOOL SetPlayedTimeEx(DWORD nTime);
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);
	BOOL SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
	BOOL GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);
	BOOL  SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();

	static void  DecCBFun(IN HHBPLAY2 hPlay, IN const HBPLAY2_FRAME* pFrame, IN PVOID pContext);
private:
	HWND m_hwnd;
	HHBPLAY2 m_hPlay;
	float m_fSpeed;
};

