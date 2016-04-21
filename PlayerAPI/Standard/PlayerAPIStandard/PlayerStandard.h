#pragma once
#include "../../../PlayerFactory/IPlayer.h"


class CPlayerFactoryStandard :public IPlayerFactory
{
public:
	CPlayerFactoryStandard(){}
	virtual ~CPlayerFactoryStandard(){}
public:
	virtual BOOL IsBelongThis(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release() ;

	virtual IPlayer *CreatePlayer();
};

class CPlayerStandard :
	public IPlayer
{
public:
	CPlayerStandard();
	virtual ~CPlayerStandard();
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

	BOOL  CapturePic(char *pSaveFile, int iType);
	BOOL  SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser);

	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime);
	BOOL FileCutClose();
	int FileCutProcess();

	BOOL FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara);
	BOOL FileConvertClose();
	int FileConvertProcess();
	BOOL FileConvertPause(DWORD nPause);
private:
	HWND m_hwnd;
};

