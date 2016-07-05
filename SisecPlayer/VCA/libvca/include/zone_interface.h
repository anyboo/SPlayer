#ifndef _ZONE_INTERFACE_
#define _ZONE_INTERFACE_

#include "vca_zonedef.h"

#define _VCA_MAIN_CONFIG	0x0000
#define _VCA_ZONE_CONFIG	0x0001
#define _VCA_CALIB_CONFIG	0x0002
#define _MD_ZONE_CONFIG		0x0003
#define _VCA_TAMPER_CONFIG  0x0004
#define _VCA_PTZ_CONFIG		0x0005 
#define _VCA_SCENECHANGE_CONFIG  0x0006

class CD3d;

interface IZoneCallBack
{
	virtual int	AddZone_Callback( int iZoneId, VCA_ZONE_T *pZone ) = 0;
	virtual int ChangeZone_Callback( int iZoneId, VCA_ZONE_T *pZone ) = 0;	// Zone movded & zone selection changed
	virtual int	RemoveZone_Callback( int iZoneId ) = 0;
};

interface IZoneInterface
{
public:
	virtual int	m_AddZone( VCA_ZONE_T *pZone ) = 0 ;
	virtual int	m_ChangeZone( int iZoneId, VCA_ZONE_T *pZone ) = 0;
	virtual int	m_RemoveZone( int iZoneId ) = 0 ;
	virtual	int	m_RemoveAllZones() = 0;
	virtual	int	m_GetZoneNum() = 0;
	virtual	VCA_ZONE_T *m_GetZoneCfg( int iZoneId ) = 0;
	virtual	int	m_DrawAllZones( CD3d *pD3D, RECT rcCanvas, unsigned int uiClientWidth, unsigned int uiClientHeight ) = 0;
	virtual	int m_RegisterCallBackInterface( IZoneCallBack *pCallBackFunc ) = 0;
};

#endif // _ZONE_INTERFACE_