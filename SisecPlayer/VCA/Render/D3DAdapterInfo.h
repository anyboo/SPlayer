#pragma once
#include <d3d.h>

typedef struct 
{
	BOOL	  fIsAvailableGUID;
	GUID	  tGUID;
	char	  szDesc[255];
	char	  szName[255];
	HMONITOR  hMonitor;
}DDADAPTERINFO;

class CD3DAdapterInfo
{
public:
	
	CD3DAdapterInfo();
	~CD3DAdapterInfo();

	INT	 GetDDAdaptersCnt(){return m_nDDAdapterCount;}
	DDADAPTERINFO* GetDDAdapterInfo( INT _nDDAdapterNo );	// _nDDAdapterNo; Zerobase

	INT  GetAdapterNoByPosition( RECT const _rcScreen );
	RECT AdjustPositionByAdapterNo( RECT const _rcScreen, INT _nNo );

private:

	HRESULT EnumDDAdapter();
	static BOOL WINAPI lpWrapAdapterEnumCallback( GUID*	_pGUID,
											   LPSTR	_strDesc,
											   LPSTR	_strName,
											   VOID*	_lpContext,
											   HMONITOR	_hMonitor );
	BOOL AdapterEnumCallback( GUID*	_pGUID,
							   LPSTR	_strDesc,
							   LPSTR	_strName,
							   HMONITOR	_hMonitor );

	DDADAPTERINFO*		m_aptDDAdapterInfo;
	INT					m_nDDAdapterCount;

};
