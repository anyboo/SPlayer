#pragma once
#include "../../../PlayerFactory/IPlayer.h"

class CPlayerFactoryZNV :public IPlayerFactory
{
public:
	CPlayerFactoryZNV(){}
	virtual ~CPlayerFactoryZNV(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual BOOL IsBelongThisHead(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release() ;

	virtual IPlayer *CreatePlayer();
};


class CPlayerZNV :
	public IPlayer
{
public:
	CPlayerZNV();
	virtual ~CPlayerZNV();
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
	static void  DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight,  unsigned __int64 nStamp, long nType, long nReserved);
	static void  ZNVFileEndCallback(long lParam);

private:
	HWND m_hwnd;
	int m_iSpeed;
	BOOL m_bPause;

};

