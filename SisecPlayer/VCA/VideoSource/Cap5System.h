#pragma once

#include "CAP5BoardLibEx.h"
#include "CAP5VideoSource.h"


class CCap5System
{
public:
	enum {STATUS_FREE, STATUS_SETUP, STATUS_RUN};
	enum {MAX_BD = 4, MAX_CH = 32};

	CCap5System(void);
	~CCap5System(void);
	
	BOOL	Setup(TCHAR *szDllName, TCHAR *szModelXMLName);
	void	Endup();
	
	BOOL	Run();
	void	Stop();

	BOOL	RegisterVideoSource(DWORD bd, DWORD ch, CCAP5VideoSource *pVideoSource);
	void	UnRegisterVideoSource(DWORD bd, DWORD ch);
	static	DWORD	WINAPI CAP5ProcessThreadWrapper(LPVOID pParam);

	ICap5*	GetCAP5Interface(){return m_pCAP5API;}
	BOOL	SetFramerate(DWORD bd, DWORD ch, DWORD FPS);
	BYTE	GetFramerate(DWORD bd, DWORD ch);


	DWORD	GetCapBoardsNum() { return m_nCapBoards; }
	CAP5_BOARD_INFO* GetCapBoardInfo() { return m_pCapBoardInfo; }
	BYTE * GetUSN(DWORD bd){return (m_nCapBoards>bd) ? m_USN[bd]:NULL;}
	BYTE * GetUSNs(){return m_USN[0];}

private:

	DWORD	m_Status;
	ICap5*	m_pCAP5API;
	
	DWORD	m_TotalCh;
	CCAP5VideoSource **m_pCAP5VideoSource;

	DWORD	m_nCapBoards;
	CAP5_BOARD_INFO* m_pCapBoardInfo;
		
	UINT	CAP5ProcessThread();
	HANDLE	m_hThread;
	HANDLE	m_hKillEvent;

	DWORD	GetLinearCh(DWORD bd, DWORD ch){
		DWORD LinearCh = 0;
		for(DWORD i=0;i<bd;i++) LinearCh += m_pCapBoardInfo[i].uMaxChannel;
		return LinearCh + ch;
	}

	BYTE	m_FrameRate[MAX_BD][MAX_CH];
	BYTE	m_USN[MAX_BD][16];
};
