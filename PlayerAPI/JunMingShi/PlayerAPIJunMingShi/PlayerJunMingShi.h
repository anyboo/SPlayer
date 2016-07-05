#pragma once
#include "../../../PlayerFactory/IPlayer.h"
#include "../inc/TLPlay.h"
class CPlayerFactoryJunMingShi :public IPlayerFactory
{
public:
	CPlayerFactoryJunMingShi(){}
	virtual ~CPlayerFactoryJunMingShi(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerJunMingShi :
	public IPlayer
{
public:
	CPlayerJunMingShi();
	virtual ~CPlayerJunMingShi();
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
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);
	BOOL SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
	BOOL GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);
	BOOL  SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser);

	BOOL  CapturePic(char *pSaveFile, int iType);

	static void CALLBACK DecCBFun(long nPort, unsigned char* pBuf, long nSize, DECFRAME_INFO* pFrameInfo, unsigned int dwContext);
private:
	HWND m_hwnd;
};

