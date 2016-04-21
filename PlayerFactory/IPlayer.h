#pragma once

#ifdef PLAYERAPI_EXPORTS
#define PLAYERAPI __declspec(dllexport)
#else
#define PLAYERAPI __declspec(dllimport)
#endif
//#define PLAYERAPI __declspec(dllexport)

#include "struct_def.h"

PLAYERAPI class IPlayer;
PLAYERAPI class IPlayerFactory
{
public:
	IPlayerFactory(){}
	virtual ~IPlayerFactory(){}
public:
	virtual BOOL IsSupportFastCut(){ return false; }
	virtual BOOL IsBelongThis(char *pFile) = 0;
	virtual char* Name() = 0;

	virtual BOOL Init() = 0;
	virtual void Release() = 0;

	virtual IPlayer *CreatePlayer() = 0;
};

typedef void(CALLBACK*FileEndCallback)(long nID, void *pUser);

PLAYERAPI class IPlayer
{
public:
	IPlayer() :m_nPort(0), m_FileEndCallbackFun(NULL), m_pUser(NULL){}
	virtual ~IPlayer(){}
public:
	virtual BOOL OpenFile(char *PFile, HWND hwnd) = 0;
//	virtual bool CloseFile() = 0;

	virtual BOOL Play() = 0;
	virtual BOOL Pause(DWORD nPause) = 0;
	virtual BOOL Fast() = 0;
	virtual BOOL Slow() = 0;
	virtual BOOL Stop() = 0;
	virtual BOOL OneByOne(){ return false; }
	virtual BOOL OneByOneBack(){ return false; }
	virtual BOOL PlaySoundShare(){ return false; }
	virtual BOOL StopSoundShare(){ return false; }
	virtual BOOL SetVolume(WORD nVolume){ return false; }
	virtual DWORD GetVolume(){ return 0; }

	virtual  BOOL  SetPlayPos(float fRelativePos)=0;
	virtual  float GetPlayPos()=0;
	virtual  DWORD  GetFileTime()=0;
	virtual  DWORD  GetPlayedTime()=0;
	virtual  BOOL   GetPictureSize(LONG *pWidth, LONG *pHeight) = 0;

	virtual BOOL CapturePic(char *pSaveFile,int iType) = 0;
	virtual BOOL SetFileEndCallback(long nID, FileEndCallback callBack, void *pUser){ return false; }

	
	virtual BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime){ return false; }
	virtual BOOL FileCutClose(){ return false; }
	virtual int FileCutProcess(){ return 0; }

	virtual BOOL FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara){ return false; }
	virtual BOOL FileConvertClose(){ return false; }
	virtual int FileConvertProcess(){ return 0; }
	virtual BOOL FileConvertPause(DWORD nPause){ return false; }

protected:
	long m_nPort;

	long m_nID;
	FileEndCallback m_FileEndCallbackFun;
	void *m_pUser;

};
