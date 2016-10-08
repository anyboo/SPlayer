#pragma once
#include "../../../PlayerFactory/IPlayer.h"

class CPlayerFactoryZhongWei :public IPlayerFactory
{
public:
	CPlayerFactoryZhongWei(){}
	virtual ~CPlayerFactoryZhongWei(){}
public:
	//virtual BOOL IsBelongThis(char *pFile);
	virtual BOOL IsBelongThisHead(char *pFile);
	virtual char* Name();

	virtual BOOL Init();
	virtual void Release();

	virtual IPlayer *CreatePlayer();
};

class CPlayerZhongWei :
	public IPlayer
{
public:
	CPlayerZhongWei();
	virtual ~CPlayerZhongWei();
	BOOL OpenFile(char *PFile, HWND hwnd);
	//BOOL CloseFile() ;

	BOOL Play();
	BOOL Pause(DWORD nPause);
	BOOL Stop();
	BOOL Fast();
	BOOL Slow();

	BOOL  SetPlayPos(float fRelativePos);
	float GetPlayPos();
	DWORD  GetFileTime();
	DWORD  GetPlayedTime();

//	BOOL Free();
private:
	HWND m_hwnd;
	int m_fps;//无GETFPS接口，默认为25，有可能不准
};

