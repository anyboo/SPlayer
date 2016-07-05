#pragma once
#include "VCAConfigure.h"
//#include "AppConfigure.h"

#include "EasyArrow.h"
#include "ObjectModel.h"
#include "VCAMetaLib.h"
//#include <afxmt.h>
#include <vector>
#include <map>
#include <list>

#define	MAX_DIFF		GETPERCENT(2) //6
#define	MAX_X_DIFF		GETPERCENT(2) //4
#define	MAX_Y_DIFF		GETPERCENT(2) //4

#define NOT_IN_USE	-1

#define MAXZONECOLORS	7

typedef enum
{
	VCA_OP_ZONE		=	0x0001,
	VCA_OP_COUNTER,
	VCA_OP_NONE
}
VCA_OP_E;

struct CNT_LINE_EVENTHIST
{
	VCA5_COUNTLINE_EVENT info;
	VCA5_APP_ZONE *pZone;
	int age;
};


#define VCA_CALIB_GRID_PICK			0x00000001
#define VCA_CALIB_GRID_DRAG			0x00000002
#define VCA_CALIB_GRID_ENDDRAG		0x00000004
#define VCA_CALIB_GRID_NODRAW		0x00000008
#define VCA_CALIB_GRID_MOUSEWHEEL   0x00000010
#define VCA_CALIB_MOUSEOVER			0x00000020
#define VCA_CALIB_RULER_DRAW		0x00000040


#define	VCA_CALIB_DRAW_HORIZON		0x00000100
#define	VCA_CALIB_INSERT_OBJ		0x00000200
#define	VCA_CALIB_TRANS_GRID		0x00000400


#define VCA5_DEFAULT_RETRIGEVENT_TIME	5	
#define	VCA5_DEFAULT_MIN_OBJECT_PIXEL	10
#define	VCA5_DEFAULT_HOLDONTIME_OBJECT	30


class IVCAConfigureObserver;

class CVCADataMgr
{
public:
	enum {MAX_OBSERVVER_CNT = 4};
	
	CVCADataMgr(void);
	~CVCADataMgr(void);

	void	Reset();
	
	void Lock()		{ /*m_cs.Lock();*/ }
	void Unlock()	{ /*m_cs.Unlock();*/ }

	VCA5_APP_ZONES*			GetZones(){return &m_Zones;}
	VCA5_APP_COUNTERS*		GetCounters(){return &m_Counters;}	
	VCA5_APP_CLSOBJECTS*	GetClsObjects(){return &m_ClsObjects;}
//	BOOL				GetCameraStabEnable() { return m_bCamStabEnable; }
//	ULONG				GetRetriggerTime()	{ return m_ulRetriggerTime; }
	VCA5_APP_ADVANCED_INFO*		GetAdvancedInfo() { return &m_AdvInfo; }
	VCA5_TAMPER_INFO*			GetTamperInfo() { return &m_TamperInfo;}
	VCA5_SCENECHANGE_INFO*		GetSceneChangeInfo() { return &m_SceneChangeInfo;}
//	VCA5_APP_ENGINE_INFO*		GetEngineInfo() { return &m_EngineInfo; }
	

	VCA5_APP_ZONE*		GetZone(ULONG idx) {
		if (idx < m_Zones.ulTotalZones) return &m_Zones.pZones[idx];
		return NULL;
	}
	VCA5_APP_RULE*		GetRule(ULONG uZoneIdx, VCA5_RULE_TYPE_E eRule) {
		if (uZoneIdx < VCA5_MAX_NUM_ZONES && (eRule >= VCA5_RULE_TYPE_PRESENCE && eRule < VCA5_RULE_TYPE_NUM)) {
			//Polygon does not support count line.
			if(m_Zones.pZones[uZoneIdx].usZoneStyle == VCA5_ZONE_STYLE_POLYGON){
				if( (VCA5_RULE_TYPE_LINECOUNTER_A == eRule) || (VCA5_RULE_TYPE_LINECOUNTER_B == eRule)) return FALSE;
			}
			return 	&m_Rules[uZoneIdx].pRules[eRule];
		} else {
			return NULL;
		}
	}
	VCA5_APP_COUNTER*	GetCounter(ULONG idx) {
		if (idx < m_Counters.ulTotalCounters) return &m_Counters.pCounters[idx];
		return NULL;
	}
	VCA5_APP_CLSOBJECT*	GetClsObject(ULONG idx) {
		if (idx < m_ClsObjects.ulTotalClsObjects) return &m_ClsObjects.pClsObjects[idx];
		return NULL;
	}

	VCA5_APP_RULE*		GetRuleByInternalId(ULONG id) {
		ULONG	ulZoneId = id / VCA5_RULE_TYPE_NUM;
		ULONG	ulRuleIdx = id % VCA5_RULE_TYPE_NUM;
		int		iZoneIdx;
		
		iZoneIdx = GetZoneIdx(ulZoneId);
		if (iZoneIdx != NOT_IN_USE) {
			return GetRule(iZoneIdx, (VCA5_RULE_TYPE_E) ulRuleIdx);
		} else {
			return NULL;
		}
	}

	VCA5_APP_RULE*		GetRuleByRealId( ULONG id )
	{
		std::map< int, int >::iterator it = m_RuleIdMap.find( id );

		if( it != m_RuleIdMap.end() )
		{
			return GetRuleByInternalId( it->second );
		}

		return NULL;
	}

	VCA5_APP_ZONE*		GetZoneById(ULONG id) {
		for(ULONG i = 0 ; i < m_Zones.ulTotalZones ;i++){
			if(m_Zones.pZones[i].usZoneId == id) return &m_Zones.pZones[i];
		}
		return NULL;
	}

	ULONG	GetZoneIdxById(ULONG id){
		for(ULONG i = 0 ; i < m_Zones.ulTotalZones ;i++){
			if(m_Zones.pZones[i].usZoneId == id) return i;
		}
		return NOT_IN_USE;
	}

	VCA5_APP_CLSOBJECT*	GetClsObjectById(SHORT id) {
		for (ULONG i = 0; i < m_ClsObjects.ulTotalClsObjects; i++) {
			if (m_ClsObjects.pClsObjects[i].sClsObjectId == id) return &m_ClsObjects.pClsObjects[i];
		}
		return NULL;
	}
	
	ULONG				GetInternalId( USHORT usZoneId, VCA5_RULE_TYPE_E eRule) {
		return usZoneId*VCA5_RULE_TYPE_NUM + eRule;
	}
	

	VCA5_APP_ZONE* GetSelectZone() {
		if (m_iSelectAreaIdx != NOT_IN_USE && m_eSelectAreaType == VCA5_APP_AREA_T_ZONE) {
			return &m_Zones.pZones[m_iSelectAreaIdx];
		} else {
			return NULL;
		}
	}
	void	SetData(VCA5_APP_ZONES* pZones, VCA5_APP_RULES* pRules, VCA5_APP_COUNTERS* pCounters, 
		VCA5_APP_CALIB_INFO* pCalibInfo, VCA5_APP_CLSOBJECTS* pClsObjects);

	void	GetData(VCA5_APP_ZONES* pZones, VCA5_APP_RULES* pDstRules, VCA5_APP_COUNTERS* pCounters, 
		VCA5_APP_CALIB_INFO* pCalibInfo, VCA5_APP_CLSOBJECTS* pClsObjects);


	void	SetAdvancedInfo( VCA5_APP_ADVANCED_INFO *pAdvInfo )
	{
		memcpy( &m_AdvInfo, pAdvInfo, sizeof( VCA5_APP_ADVANCED_INFO ) );
	}

	void	SetTamperInfo(VCA5_TAMPER_INFO* pTamperInfo) { 
		memcpy(&m_TamperInfo, pTamperInfo, sizeof(VCA5_TAMPER_INFO));
	}		

	void	SetSceneChangeInfo(VCA5_SCENECHANGE_INFO* pSceneChangeInfo) { 
		memcpy(&m_SceneChangeInfo, pSceneChangeInfo, sizeof(VCA5_SCENECHANGE_INFO));
	}		

	/*
	void	SetEngineInfo( VCA5_APP_ENGINE_INFO *pEngineInfo )
	{
		memcpy( &m_EngineInfo, pEngineInfo, sizeof( VCA5_APP_ENGINE_INFO ) );
	}
	*/
	int		AddFullScrrenZone(POINT *pPoint);
	int		AddZone(POINT *pPoint);
	int		AddLine(POINT *pPoint,bool bDirectA=false);
	BOOL	AddNode(int ZoneIdx, int Nodeidx, POINT *point);
	BOOL	RemoveNode(int ZoneIdx, int Nodeidx);
	BOOL	RemoveZone(int iZoneIdx);

	int		AddCounter(POINT *pPoint);
	BOOL	RemoveCounter(int iCounterIdx);

	void	RemoveAll();
	
	BOOL	MoveArea(int iAreaIdx, VCA5_APP_AREA_T_E eAreaType, POINT *pDiff);
	BOOL	ChangeNode(int iZoneIdx, int iPointIdx, POINT *pPoint);
	POINT	GetNode( int iZoneIdx, int iPointIdx );
	void	GetZoneCenter( VCA5_APP_ZONE *pZone, VCA5_POINT *pt );
	ANGLE_STATUS	CheckZoneAngle( int iZoneIdx );			

	int		GetZoneIdx(VCA5_APP_ZONE *pZone);
	int		GetZoneIdx(ULONG id);
	int		GetNumTickedCounter(VCA5_APP_COUNTER *pCounter);
	
	void	FindSelectedAreaIdx(int *piAreaIdx, VCA5_APP_AREA_T_E *pType);

	void	SetAreaStatus(int iAreaIdx, VCA5_APP_AREA_T_E eType, VCA5_APP_AREA_STATUS_E eStatus);
	void	ClearAreaStatus(int iAreaIdx, VCA5_APP_AREA_T_E eType, VCA5_APP_AREA_STATUS_E eStatus);

	void	GetSelectAreaIdx( int *piAreaIdx, VCA5_APP_AREA_T_E *pType )	{ *piAreaIdx = m_iSelectAreaIdx; *pType = m_eSelectAreaType; };
	void	GetNotifyAreaIdx( int *piAreaIdx, VCA5_APP_AREA_T_E *pType )	{ *piAreaIdx = m_iNotifyAreaIdx; *pType = m_eNotifyAreaType; };
	void	SetSelectArea(int iAreaIdx, VCA5_APP_AREA_T_E eType);
	void	SetNotifyArea(int iAreaIdx, VCA5_APP_AREA_T_E eType);

	void	ClearAllNotifyAreas();
	void	ClearAllSelectAreas();

	void	SetSelectedNode( int idx );
	int		GetSelectNodeIdx() { return m_iSelectNodeIdx; }

	void	RegisterObserver(IVCAConfigureObserver *pObserver);
	void	UnregisterObserver( IVCAConfigureObserver *pObserver );
	void	FireEvent(DWORD uiEvent, IVCAConfigureObserver *pActor, DWORD uiContext = NULL);

	BOOL	GetRuleStatus( int zoneIdx, VCA5_RULE_TYPE_E eRuleType );
	BOOL	SetPresence( int iZoneIdx ){return SetRule_BASICT( iZoneIdx, VCA5_RULE_TYPE_PRESENCE);}
	BOOL	SetEnter( int iZoneIdx){ return SetRule_BASICT( iZoneIdx, VCA5_RULE_TYPE_ENTER);}
	BOOL	SetExit( int iZoneIdx){ return SetRule_BASICT( iZoneIdx, VCA5_RULE_TYPE_EXIT);}
	BOOL	SetAppear( int iZoneIdx){return SetRule_BASICT( iZoneIdx, VCA5_RULE_TYPE_APPEAR);}
	BOOL	SetDisappear( int iZoneIdx){return SetRule_BASICT( iZoneIdx, VCA5_RULE_TYPE_DISAPPEAR);}
	BOOL	SetStop( int iZoneIdx, BOOL bInit = TRUE, int iTime = 5){
			return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_STOP, bInit, iTime);}
	BOOL	SetDwell( int iZoneIdx, BOOL bInit = TRUE, int iTime = 20){
		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_DWELL, bInit, iTime);}
	BOOL	SetDirection( int iZoneIdx, BOOL bInit = FALSE, int startangle = 0, int finishangle = 0);
	BOOL	SetSpeed( int iZoneIdx, BOOL bInit = TRUE, USHORT usSpeedLo = 0, USHORT usSpeedUp = 200 );
//	BOOL	SetAbObj( int iZoneIdx, BOOL bInit = TRUE, int iTime = 20){
//		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_ABOBJ, bInit, iTime);}
	BOOL	SetRmObj( int iZoneIdx, BOOL bInit = TRUE, int iTime = 20){
		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_RMOBJ, bInit, iTime);}
	BOOL	SetColFilter( int iZoneIdx, BOOL bInit = TRUE, unsigned short usColBin = 0, unsigned short usThres = 0 );
	BOOL	RemoveColFilter( int iZoneIdx );
	BOOL	SetTailgating( int iZoneIdx, BOOL bInit = TRUE, int iTime = 5, int iTriggerType = 0){
		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_TAILGATING, bInit, iTime);}
	BOOL	SetLineCounterA(int iZoneIdx, BOOL bInit = TRUE, ULONG ulCalibrationWidth = 0, ULONG ulShadowFilterEnabled = 0){
		return SetRule_LINECOUNTERT(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_A, bInit, ulCalibrationWidth, ulShadowFilterEnabled);}		
	BOOL	SetLineCounterB(int iZoneIdx, BOOL bInit = TRUE, ULONG ulCalibrationWidth = 0, ULONG ulShadowFilterEnabled = 0){
		return SetRule_LINECOUNTERT(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_B, bInit, ulCalibrationWidth, ulShadowFilterEnabled);}	
	BOOL	SetSmoke( int iZoneIdx, BOOL bInit = TRUE, int iTime = 1){
		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_SMOKE, bInit, iTime);}
	BOOL	SetFire( int iZoneIdx, BOOL bInit = TRUE, int iTime = 1){
		return SetRule_TIMERT( iZoneIdx, VCA5_RULE_TYPE_FIRE, bInit, iTime);}

	BOOL	RemoveRule( int iZoneIdx, VCA5_RULE_TYPE_E eRule ); 
	BOOL	SetRule(int iZoneIdx, VCA5_RULE_TYPE_E eRule);

	VCA5_APP_CALIB_INFO*	GetCalibInfo() { return &m_CalibInfo; };
	void	SetChangeCalibInfo(BOOL bChange){m_bChangeCalibInfo = bChange;}
	BOOL	IsChangeCalibInfo(){return m_bChangeCalibInfo;}
	
	
	BOOL	CheckFeature(int Feature);
	BOOL	CheckFeatureByRuleType(int RuleType);

	COLORREF	GetColor(DWORD id);
	LPCTSTR		GetColorName(DWORD id);

	void	SetDisplayFlags(DWORD DisplayFlags);
	DWORD	GetDisplayFlags(){return m_DisplayFlags;}

	BOOL	IsItACountingLine(int iZoneId);
	ULONG	GetLineCounterA( int iZoneIdx) ;
	ULONG	GetLineCounterB( int iZoneIdx) ;

	void	UpdateCountingLineEvents(VCA5_COUNTLINE_EVENTS* countLineEvents);
	void	ClearCountingLineEvents();

	std::list<CNT_LINE_EVENTHIST>* GetCountingLineEventsA() { return m_countLineEventsA;}
	std::list<CNT_LINE_EVENTHIST>* GetCountingLineEventsB() { return m_countLineEventsB;}

	int					GetObjectModelCnt(){return m_iNumObjects;}
	ObjectModel*		GetObjectModel(int index){
		return (index>=m_iNumObjects)?NULL:&m_objectModels[index];
	}
	BOOL	InsertObjectModel(D3DXVECTOR3 v);
	void	ResetObjectModel();
	BOOL	m_bResetObjectModel;

	BOOL	GetDrawTrailMode()							{	return m_bCentroidTrail;	}
	VOID	SetDrawTrailMode(BOOL bCentroidTrail)		{	m_bCentroidTrail = bCentroidTrail;	}

public:

	CEasyArrow		m_cArrowTop;
	CEasyArrow		m_cArrowBottom;
	CEasyArrow		m_cArrowMiddle;
	ULONG			m_uiCalibDrawOptions;
	POINT			m_LastClickedMousePos;
	D3DXVECTOR3		m_RulerPos[2];

	CEasyArrow		m_cArrowTop22;
	CEasyArrow		m_cArrowBottom22;
	CEasyArrow		m_cArrowMiddle22;

	CEasyArrow		m_cArrowTop33;
	CEasyArrow		m_cArrowBottom33;
	CEasyArrow		m_cArrowMiddle33;
	
private:

	VCA5_APP_ZONES		m_Zones;
	VCA5_APP_RULES		m_Rules[VCA5_MAX_NUM_ZONES];
	VCA5_APP_COUNTERS	m_Counters;
	VCA5_APP_CLSOBJECTS	m_ClsObjects;
	VCA5_TAMPER_INFO	m_TamperInfo;
	VCA5_SCENECHANGE_INFO	m_SceneChangeInfo;
	VCA5_APP_ADVANCED_INFO	m_AdvInfo;
//	VCA5_APP_ENGINE_INFO	m_EngineInfo;

	VCA5_APP_AREA_T_E	m_eSelectAreaType;
	int					m_iSelectAreaIdx;
	VCA5_APP_AREA_T_E	m_eNotifyAreaType;
	int					m_iNotifyAreaIdx;
	int					m_iSelectNodeIdx;
	BOOL				m_bCentroidTrail;

	DWORD				m_DisplayFlags;
	std::map< int, int >	m_RuleIdMap;

//	CCriticalSection	m_cs;
	std::vector< IVCAConfigureObserver *>	m_Observers;

	static struct Color {
		COLORREF	Value;
		LPCTSTR		pName;
	} m_Colors[MAXZONECOLORS];

	BOOL			m_bChangeCalibInfo;
	VCA5_APP_CALIB_INFO	m_CalibInfo;
	int				m_iNumObjects;
	ObjectModel		m_objectModels[MAXNUM_OBJECTS];

	void	PreLoadRules();

	int		FindNextZoneID();
	int		FindNextCounterID();
	
	BOOL	SetRule_BASICT( int iZoneIdx, VCA5_RULE_TYPE_E eRule);
	BOOL	SetRule_TIMERT( int iZoneIdx, VCA5_RULE_TYPE_E eRule, BOOL bInit, int iTime);
	BOOL	SetRule_LINECOUNTERT( int iZoneIdx, VCA5_RULE_TYPE_E eRule, BOOL bInit, ULONG ulCalibrationWidth, ULONG ulShadowFilterEnabled);
	BOOL	AddDefaultZoneRule(int iZoneidx, BOOL bInit, BOOL bAddZone );

	std::list< CNT_LINE_EVENTHIST> m_countLineEventsA[VCA5_MAX_NUM_ZONES], m_countLineEventsB[VCA5_MAX_NUM_ZONES];
};
