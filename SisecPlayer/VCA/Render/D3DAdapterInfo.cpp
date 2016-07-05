#include "StdAfx.h"
#include "D3DAdapterInfo.h"


CD3DAdapterInfo::CD3DAdapterInfo() 
{
	m_aptDDAdapterInfo = NULL;
	m_nDDAdapterCount  = 0;
	EnumDDAdapter();

}


CD3DAdapterInfo::~CD3DAdapterInfo()
{
	if( m_aptDDAdapterInfo ){
		delete[] m_aptDDAdapterInfo;
	}
}


BOOL WINAPI CD3DAdapterInfo::lpWrapAdapterEnumCallback( GUID*	_pGUID,
											   LPSTR	_strDesc,
											   LPSTR	_strName,
											   VOID*	_lpContext,
											   HMONITOR	_hMonitor )
{
	CD3DAdapterInfo* pcThis = (CD3DAdapterInfo*)_lpContext;
	return pcThis->AdapterEnumCallback( _pGUID, _strDesc, _strName, _hMonitor );
}


BOOL CD3DAdapterInfo::AdapterEnumCallback( GUID*	  _pGUID,
								 LPSTR	  _strDesc,
								 LPSTR	  _strName,
								 HMONITOR _hMonitor )
{
	if( m_aptDDAdapterInfo ) {
		if( _pGUID == NULL ) {
			m_aptDDAdapterInfo[m_nDDAdapterCount].fIsAvailableGUID = FALSE;	
		}
		else {
			m_aptDDAdapterInfo[m_nDDAdapterCount].fIsAvailableGUID = TRUE;	
			m_aptDDAdapterInfo[m_nDDAdapterCount].tGUID = *_pGUID;
		}

		lstrcpyA( m_aptDDAdapterInfo[m_nDDAdapterCount].szDesc, _strDesc );
		lstrcpyA( m_aptDDAdapterInfo[m_nDDAdapterCount].szName, _strDesc );
		m_aptDDAdapterInfo[m_nDDAdapterCount].hMonitor = _hMonitor;

		//ErrDbg(1, "m_aptDDAdapterInfo[%d].szDesc %s \n", m_nDDAdapterCount, m_aptDDAdapterInfo[m_nDDAdapterCount].szDesc );
		//ErrDbg(1, "m_aptDDAdapterInfo[%d].szName %s \n", m_nDDAdapterCount, m_aptDDAdapterInfo[m_nDDAdapterCount].szName );
		//ErrDbg(1, "m_aptDDAdapterInfo[%d].hMonitor %X \n", m_nDDAdapterCount, m_aptDDAdapterInfo[m_nDDAdapterCount].hMonitor );
	}

	m_nDDAdapterCount += 1;
	return TRUE;
}

HRESULT CD3DAdapterInfo::EnumDDAdapter()
{
	HRESULT	hr;

	if( m_aptDDAdapterInfo ) delete[] m_aptDDAdapterInfo;
	m_aptDDAdapterInfo = NULL;
	m_nDDAdapterCount = 0;
    hr = ::DirectDrawEnumerateExA( lpWrapAdapterEnumCallback,
								  this,
						          DDENUM_ATTACHEDSECONDARYDEVICES |
						          DDENUM_DETACHEDSECONDARYDEVICES |
							      DDENUM_NONDISPLAYDEVICES );
	if( FAILED(hr) ) {
		TRACE( _T("DirectDrawEnumerateEx Error [%X]"), hr );
		return FALSE;
	}
	if( m_nDDAdapterCount == 0 ) return -1;

	m_aptDDAdapterInfo = new DDADAPTERINFO[m_nDDAdapterCount];
	m_nDDAdapterCount = 0;
    hr = ::DirectDrawEnumerateExA( lpWrapAdapterEnumCallback,
								  this,
						          DDENUM_ATTACHEDSECONDARYDEVICES |
						          DDENUM_DETACHEDSECONDARYDEVICES |
							      DDENUM_NONDISPLAYDEVICES );
	return hr;
}

	
DDADAPTERINFO* CD3DAdapterInfo::GetDDAdapterInfo( INT nDDAdapterNo )
{ 
	if( (0 > nDDAdapterNo) || (nDDAdapterNo >= m_nDDAdapterCount) ) return NULL;
	return &m_aptDDAdapterInfo[nDDAdapterNo];
}


INT CD3DAdapterInfo::GetAdapterNoByPosition( RECT const _rcScreen )
{
	INT	nNo = 0;

	for( INT i=1; i < m_nDDAdapterCount; i++ ) {
		HMONITOR hMonitor;
		MONITORINFO mi = {0,};
		mi.cbSize = sizeof(MONITORINFO);

		hMonitor = m_aptDDAdapterInfo[i].hMonitor;
		GetMonitorInfo( hMonitor, &mi );
		
		INT	x = _rcScreen.left;
		INT	y = _rcScreen.top;
		if( (mi.rcMonitor.left <= x && x <= mi.rcMonitor.right) &&
			(mi.rcMonitor.top  <= y && y <= mi.rcMonitor.bottom)   ) {
			nNo = i;
			break;
		}
	}

	return nNo;
}


RECT CD3DAdapterInfo::AdjustPositionByAdapterNo( RECT const _rcScreen, INT _nNo )
{
	RECT	rcRet = {0,};

	rcRet = _rcScreen;
	do 
	{
		if( 1 == m_nDDAdapterCount ) {
			break;
		}

		if( -1 == _nNo ) {
			break;
		}

		HMONITOR hMonitor;
		MONITORINFO mi = {0,};
		mi.cbSize = sizeof(MONITORINFO);
		hMonitor = m_aptDDAdapterInfo[_nNo].hMonitor;
		GetMonitorInfo( hMonitor, &mi );

		rcRet.left  = _rcScreen.left - mi.rcMonitor.left;
		rcRet.right = _rcScreen.right - mi.rcMonitor.left;

		rcRet.top    = _rcScreen.top  - mi.rcMonitor.top;
		rcRet.bottom = _rcScreen.bottom - mi.rcMonitor.top;

	}while(0);

	return rcRet;
}


