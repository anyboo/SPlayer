#pragma once
#include "../../../PlayerFactory/IPlayer.h"

#include "../inc/windowsPlayM4.h"
#include<list>

class CPlayerFactoryHik :public IPlayerFactory
{
public:
	CPlayerFactoryHik(){}
	virtual ~CPlayerFactoryHik(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual BOOL IsBelongThisHead(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release() ;

	virtual IPlayer *CreatePlayer();
};

//typedef void (CALLBACK* DisplayCBFun)(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long nReserved);
typedef void (CALLBACK* DisplayCBFun)(DISPLAY_INFO *pstDisplayInfo);

class CPlayerHik :
	public IPlayer
{
public:
	CPlayerHik();
	virtual ~CPlayerHik();
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
	BOOL GetPictureSize(LONG *pWidth, LONG *pHeight);

	BOOL  SetFileEndCallback(LONG nID, FileEndCallback callBack, void *pUser);
	//BOOL  SetDecCallback(LONG nPort, void (CALLBACK* DecCBFun)(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nUser), long nUser);
	BOOL  SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser);
	BOOL GetSystemTime(unsigned long long *pstSystemTime);

	BOOL SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
	BOOL GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();
private:
	BOOL  SavePic(char * pBuf, long nSize, long nWidth, long nHeight, long nType);
	//void AddDisplayCallbackList(long nPort);
	//void RemoveDisplayCallback(long nPort);

	//static void  DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long nReserved);
	static void  DisplayCBFunBackEx(DISPLAY_INFO *pstDisplayInfo);
	//static std::list<CPlayerHik*>  m_sPlayerHik;
	char m_saveFile[256];
	bool m_bPic;
	int m_iType;

private:
	HWND m_hwnd;
};

