#pragma once
#include "../../../PlayerFactory/IPlayer.h"

class CPlayerFactoryDiZhiPu :public IPlayerFactory
{
public:
	CPlayerFactoryDiZhiPu(){}
	virtual ~CPlayerFactoryDiZhiPu(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual BOOL IsBelongThisHead(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerDiZhiPu :
	public IPlayer
{
public:
	CPlayerDiZhiPu();
	virtual ~CPlayerDiZhiPu();
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
	BOOL SetPlayedTimeEx(DWORD nTime);
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);
	BOOL SetColor(DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
	BOOL GetColor(DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);
	BOOL  SetDisplayCallback(LONG nID, DisplayCallback displayCallback, void * nUser);
	BOOL GetSystemTime(unsigned long long *pstSystemTime);

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast);
	BOOL FileCutClose();
	int FileCutProcess();

	BOOL FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara);
	BOOL FileConvertClose();
	int FileConvertProcess();
	BOOL FileConvertPause(DWORD nPause);

	static void CALLBACK CoverFileCallBackFun(DWORD CurrentPos, DWORD TotoalPos, long dwUser);//×ª»»½ø¶È
	static void CALLBACK DisplayCBFunBack(long nPort, char * pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long lUser);
public:
	int m_process;
private:
	HWND m_hwnd;
	BOOL m_bPause;
};

