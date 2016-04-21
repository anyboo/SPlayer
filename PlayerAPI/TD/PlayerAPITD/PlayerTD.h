#pragma once
#include "../../../PlayerFactory/IPlayer.h"
#include <stdio.h>
#include<list>

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

	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();
	static void  EndCallback(long nID);
	static std::list<CPlayerTD*>  m_sPlayerTDCallBack;
private:
	int m_iSpeed;
	BOOL m_bPause;

};

