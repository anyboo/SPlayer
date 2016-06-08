#pragma once
#include "../../../PlayerFactory/IPlayer.h"
#include <stdio.h>
#include<list>
#include "../inc/publicDef.h"

class CPlayerFactoryTD :public IPlayerFactory
{
public:
	CPlayerFactoryTD(){}
	virtual ~CPlayerFactoryTD(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerTD :
	public IPlayer
{
public:
	CPlayerTD();
	virtual ~CPlayerTD();
	BOOL OpenFile(char *PFile,HWND hwnd);
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
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);
	BOOL SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
	BOOL GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);
	BOOL  SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();
public:
	static void  EndCallback(long nID);
	static void DecCallback(unsigned int _ulID, unsigned char *_cData, int _iLen, const FRAME_INFO *_pFrameInfo, int _iUser);
	static std::list<CPlayerTD*>  m_sPlayerTDCallBack;
private:
	int m_iSpeed;
	BOOL m_bPause;

};

