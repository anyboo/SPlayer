// CVCAConfigure.h: interface for the CVCAConfigure class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include <VCA5CoreLib.h>

#define VCA5_APP_MAX_NUM_TARGET				100
#define VCA5_APP_MAX_NUM_EVENT				50
#define VCA5_APP_MAX_NUM_COUNTER_OUT		20
#define VCA5_APP_MAX_NUM_COUNTERS			20
#define VCA5_APP_MAX_NUM_SUBCOUNTERS		20
#define VCA5_APP_MAX_TYPE_COUNTERS			5		
#define VCA5_APP_MAX_NUM_OPTIONSPERRULE		(5)

enum VCA5_APP_AREA_T_E{
	VCA5_APP_AREA_T_NONE,
	VCA5_APP_AREA_T_ZONE,
	VCA5_APP_AREA_T_COUNTER,
};


enum VCA5_APP_COUNTER_STATUS_E{
	VCA5_APP_COUNTER_STATUS_NOT_USED	= 0x00000001,
};


// status for VCA5_APP_ZONE and VCA5_APP_COUNTER
enum VCA5_APP_AREA_STATUS_E{
	VCA5_APP_AREA_STATUS_NOT_USED		= 0x00000001,
	VCA5_APP_AREA_STATUS_HIDE			= 0x00000001,
	VCA5_APP_AREA_STATUS_NORMAL			= 0x00000002,
	VCA5_APP_AREA_STATUS_SELECTED		= 0x00000004,
	VCA5_APP_AREA_STATUS_NOTIFIED		= 0x00000008,
	VCA5_APP_AREA_STATUS_EVENT_CHANGED	= 0x00000010,
	VCA5_APP_AREA_STATUS_INITIAL_RULE	= 0x00000020,
};

#define SetAreaStatusBit( pArea, status ) (pArea)->uiStatus |= (status);
#define ClearAreaStatusBit( pArea, status ) (pArea)->uiStatus &= ~(status);



//////////////////////////////////////////////////////////////////////////
// VCA5_APP_ZONE

#define VCA5_APP_MAX_NAME	40

// macro for zone position
#define NORMALISATOR	65535	// Everything is normalized to 16bits (2Bytes) in the new tracker

#define GETPERCENT( percent ) percent * NORMALISATOR / 100

#define PERCENTASSERT( value) \
	value = ( value > NORMALISATOR ) ? NORMALISATOR: value; \
	value = ( value < 0 ) ? 0: value;

#ifndef PIXELTOPERCENT
#define PIXELTOPERCENT( normalizeValue, pixelValue, maxValue ) \
	normalizeValue = (unsigned short) ( (pixelValue * NORMALISATOR) / ( maxValue -1) ); \
	PERCENTASSERT( normalizeValue );
#endif

#ifndef PERCENTTOPIXEL
#define PERCENTTOPIXEL( pixelValue, normalizeValue, maxValue ) \
	pixelValue = (( maxValue -1 ) * normalizeValue) / NORMALISATOR;
#endif

#define DEFAULT_ZONE_WIDTH		GETPERCENT(20)	// 20%
#define DEFAULT_ZONE_HEIGHT 	GETPERCENT(20)	// 20%

#define MAX_ZONE_WIDTH		GETPERCENT(100)	// 100%
#define MAX_ZONE_HEIGHT 	GETPERCENT(100)	// 100%

struct VCA5_APP_ZONE : VCA5_ZONE {
	DWORD		uiStatus;
	DWORD		uiColor;	// 0x00bbggrr
} ;

struct VCA5_APP_ZONES {
	ULONG ulTotalZones;
	VCA5_APP_ZONE pZones[VCA5_MAX_NUM_ZONES];
};

//////////////////////////////////////////////////////////////////////////
// VCA5_APP_RULE

enum VCA5_APP_RULE_STATUS_E{
	VCA5_APP_RULE_STATUS_EXPANDED		= 0x00000001,
	VCA5_APP_RULE_STATUS_TICKED			= 0x00000002,
};

#define TRKOBJ_INCLUDE			0
#define TRKOBJ_EXCLUDE			1

#ifndef _vca_rule_h_
#define GET_TRKOBJ_LOGIC( trkobj )	( ( trkobj & 0xC0 ) >> 6 )	// high 2 bits
#define GET_TRKOBJ_ID( trkobj )		( trkobj & 0x3F )			// lo 6 bits
#define MAKE_TRKOBJ( logic, id )	( ( ( logic & 0x03 ) << 6 ) | ( id & 0x3F ) )
#endif

struct VCA5_APP_RULE_COLSIG {
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	unsigned short		usColBin; 			
	unsigned short		usThres;
};

struct VCA5_APP_RULE : VCA5_RULE {
	BYTE		ucUsed;
	BYTE		ucTicked;
	BYTE		ucExpanded;
	BYTE		ucWidthCalibrationEnabled;	// for enable/disable width calibration on counting line
	BYTE		ucShadowFilterEnabled;		// for enable/disable shadow filter on counting line
};

struct VCA5_APP_RULES {
	ULONG			ulTotalRules;
	VCA5_APP_RULE	pRules[VCA5_MAX_NUM_RULES];
};

//////////////////////////////////////////////////////////////////////////
// VCA5_APP_COUNTER

enum VCA5_APP_SUBCOUNTER_STATUS_E{
	VCA5_APP_SUBCOUNTER_STATUS_USED			= 0x00000001,	//Ticked and selected Sub count
	VCA5_APP_SUBCOUNTER_STATUS_TICKED		= 0x00000002,
};

#define DEFAULT_COUNTER_WIDTH		GETPERCENT(10)	// 10%
#define DEFAULT_COUNTER_HEIGHT		GETPERCENT(10)	// 10%


struct VCA5_APP_COUNTER : VCA5_COUNTER {
	DWORD		uiStatus;
	int			iCounterResult;
	DWORD		uiSubCounterStatuses[VCA5_MAX_NUM_SUBCOUNTERS];
	ULONG		ulTotalPoints;
	VCA5_POINT	pPoints[VCA5_MAX_NUM_ZONE_POINTS];
	DWORD		uiColor;	// 0x00bbggrr
//	TCHAR		pName[VCA5_APP_MAX_NAME];
};

struct VCA5_APP_COUNTERS {
	ULONG				ulTotalCounters;
	VCA5_APP_COUNTER	pCounters[VCA5_MAX_NUM_COUNTERS];
};

#define VCA5_APP_CLSOBJECT_UNCLASSIFIED_ID	0x3F

struct VCA5_APP_CALIB_INFO : VCA5_CALIB_INFO {
	float 		fPan;
};

struct VCA5_APP_CLSOBJECT : VCA5_CLSOBJECT {
	//TCHAR		pName[VCA5_APP_MAX_NAME];
	BOOL		bEnable;
};

struct VCA5_APP_CLSOBJECTS {
	ULONG				ulTotalClsObjects;
	VCA5_APP_CLSOBJECT	pClsObjects[VCA5_MAX_NUM_CLSOBJECTS];
};

struct VCA5_APP_ADVANCED_INFO
{
	BOOL	bEnableStab;
	ULONG	ulRetrigTime;

	VCA5_TRACKERPARAMS	TrackerParams;
};


typedef struct
{
	VCA5_APP_ZONES			Zones;
	VCA5_APP_RULES			Rules;
	VCA5_APP_COUNTERS		Counters;
	VCA5_APP_CALIB_INFO		CalibInfo;
	VCA5_APP_CLSOBJECTS		ClsObjects;

//	Advanced settings
	VCA5_TAMPER_INFO		TamperInfo;
	VCA5_SCENECHANGE_INFO	SceneChangeInfo;
	VCA5_APP_ADVANCED_INFO	AdvInfo;

//	License info
	ULONG					ulFunction;

}VCA5_APP_ENGINE_CONFIG;


class CVCAConfigure
{
private:
	CVCAConfigure();
	~CVCAConfigure();
public:
	static CVCAConfigure *Instance();
	void	DestroySelf() {delete this;}
	
	HRESULT		LoadEngine( DWORD dwEngId, LPTSTR lpszFilename );
	HRESULT		LoadEngine( DWORD dwEngId, char *pszBuf, unsigned int uiBufLen );

	HRESULT		SaveEngine( DWORD dwEngId, LPTSTR lpszFilename );
	HRESULT		SaveEngine( DWORD dwEngId, char *pszBuf, unsigned int &uiBufLen );
	
	DWORD		GetEngCnt(){return m_EngCnt;}
	void		SetEngCnt(DWORD eng){m_EngCnt = eng;}

	VCA5_APP_ENGINE_CONFIG* GetEngineConf(DWORD eng)
	{
		return (eng >= VCA5_MAX_NUM_ENGINE)?NULL:&m_EngineConfig[eng];
	}

private:
	HRESULT		DoLoad( DWORD dwEngId );
	HRESULT		LoadZones( DWORD dwEngId, VCA5_APP_ZONES *pZones );
	HRESULT		LoadRules( DWORD dwEngId, VCA5_APP_RULES *pRules );
	HRESULT		LoadCounters( DWORD dwEngId, VCA5_APP_COUNTERS *pCounters );
	HRESULT		LoadCalibration( DWORD dwEngId, VCA5_APP_CALIB_INFO *pCalibInfo );
	HRESULT		LoadObjects( DWORD dwEngId, VCA5_APP_CLSOBJECTS *pObjs );
	HRESULT		LoadTamper( DWORD dwEngId, VCA5_TAMPER_INFO *pInfo );
	HRESULT		LoadSceneChange( DWORD dwEngId, VCA5_SCENECHANGE_INFO *pInfo );
	HRESULT		LoadAdvanced( DWORD dwEngId, VCA5_APP_ADVANCED_INFO *pInfo );
	HRESULT		LoadFunction( DWORD dwEngId, ULONG *pFunction);
	HRESULT		UnSquishIds( VCA5_APP_ENGINE_CONFIG *pCfg );

	HRESULT		DoSave( DWORD dwEngId );
	HRESULT		SaveZones( DWORD dwEngId, VCA5_APP_ZONES *pZones );
	HRESULT		SaveRules( DWORD dwEngId, VCA5_APP_RULES *pRules );
	HRESULT		SaveCounters( DWORD dwEngId, VCA5_APP_COUNTERS *pCounters );
	HRESULT		SaveCalibration( DWORD dwEngId, VCA5_APP_CALIB_INFO *pCalibInfo );
	HRESULT		SaveObjects( DWORD dwEngId, VCA5_APP_CLSOBJECTS *pObjs );
	HRESULT		SaveTamper( DWORD dwEngId, VCA5_TAMPER_INFO *pInfo );
	HRESULT		SaveSceneChange( DWORD dwEngId, VCA5_SCENECHANGE_INFO *pInfo );
	HRESULT		SaveAdvanced( DWORD dwEngId, VCA5_APP_ADVANCED_INFO *pInfo );
	HRESULT		SquishIds( VCA5_APP_ENGINE_CONFIG *pCfg);


private:

	BOOL					m_bLoad;
	DWORD					m_EngCnt;

	VCA5_APP_ENGINE_CONFIG*	m_EngineConfig;

	static CVCAConfigure	*m_pInstance;
};

