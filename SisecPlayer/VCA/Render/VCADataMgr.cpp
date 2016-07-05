#include "stdafx.h"
#include "VCADataMgr.h"
#include "../Render/VCARender.h"
//#include "../VCADialog/VCAConfigureObserver.h"

void	InitObjectModel(ObjectModel *pObjectModel, float r, float g, float b)
{
	pObjectModel->fMinX = -0.35f;
	pObjectModel->fMaxX = 0.35f;
	pObjectModel->fMinY = -0.15f;
	pObjectModel->fMaxY = 0.15f;
	pObjectModel->fMinZ = 0.0f;
	pObjectModel->fMaxZ = HUMAN_HEIGHT;

	pObjectModel->materialPerson.specular.r = 1.0f;
	pObjectModel->materialPerson.specular.g = 1.0f;
	pObjectModel->materialPerson.specular.b = 1.0f;
	pObjectModel->materialPerson.specular.a = 0.0f;
	pObjectModel->materialPerson.power = 40.0f;
	pObjectModel->materialPerson.diffuse.r = r;
	pObjectModel->materialPerson.diffuse.g = g;
	pObjectModel->materialPerson.diffuse.b = b;
	pObjectModel->materialPerson.diffuse.a = 1.0f;
	pObjectModel->materialPerson.ambient.r = r;
	pObjectModel->materialPerson.ambient.g = g;
	pObjectModel->materialPerson.ambient.b = b;
	pObjectModel->materialPerson.ambient.a = 1.0f;
}


CVCADataMgr::CVCADataMgr(void)
{
//	m_nObserverCnt	= 0;
	memset(&m_Zones, 0, sizeof(VCA5_APP_ZONES));
	memset(&m_Rules, 0, sizeof(m_Rules));
	memset(&m_Counters, 0, sizeof(VCA5_APP_COUNTERS));
	memset(&m_CalibInfo, 0 ,sizeof(VCA5_APP_CALIB_INFO));
	memset(&m_TamperInfo, 0 ,sizeof(VCA5_TAMPER_INFO));

	m_CalibInfo.fHeight	= 10;
	m_CalibInfo.fTilt	= 30;
	m_CalibInfo.fFOV	= 40;
	m_CalibInfo.calibrationStatus	= VCA5_CALIB_STATUS_INVALIDSETUP;
	m_CalibInfo.ulHeightUnits		= VCA5_HEIGHT_UNITS_METERS;
	m_CalibInfo.ulSpeedUnits		= VCA5_SPEED_UNITS_KPH;
	m_bChangeCalibInfo	= FALSE;

//	m_bCamStabEnable	= FALSE;
//	m_ulRetriggerTime	= 5;

	m_AdvInfo.bEnableStab	= FALSE;
	m_AdvInfo.ulRetrigTime	= VCA5_DEFAULT_RETRIGEVENT_TIME;

	m_AdvInfo.TrackerParams.ulMinObjAreaPix	= VCA5_DEFAULT_MIN_OBJECT_PIXEL;
	m_AdvInfo.TrackerParams.ulSecsToHoldOn	= VCA5_DEFAULT_HOLDONTIME_OBJECT;

	m_iSelectNodeIdx	= NOT_IN_USE;
	m_bCentroidTrail	 = FALSE;
	
	memset(&m_ClsObjects, 0 ,sizeof(VCA5_APP_CLSOBJECTS));

	m_iNumObjects	= 2;
	m_bResetObjectModel	= TRUE;
	memset(m_objectModels, 0, sizeof(m_objectModels));
	
	InitObjectModel(&m_objectModels[0], 0.8f, 0.2f, 0.2f);
	InitObjectModel(&m_objectModels[1], 0.2f, 0.2f, 0.8f);
	InitObjectModel(&m_objectModels[2], 0.2f, 0.8f, 0.2f);
	InitObjectModel(&m_objectModels[3], 0.2f, 0.8f, 0.8f);
	InitObjectModel(&m_objectModels[4], 0.8f, 0.8f, 0.2f);
	InitObjectModel(&m_objectModels[5], 0.8f, 0.2f, 0.8f);

	m_DisplayFlags	= IVCARender::DISPLAY_NON_ALARMS;
	Reset();
}

CVCADataMgr::~CVCADataMgr(void)
{
}


void	CVCADataMgr::Reset()
{
	m_iSelectAreaIdx = NOT_IN_USE;
	m_iNotifyAreaIdx = NOT_IN_USE;

	m_eSelectAreaType	= VCA5_APP_AREA_T_NONE;	 
	m_eNotifyAreaType	= VCA5_APP_AREA_T_NONE;
	m_uiCalibDrawOptions= 0;	

	ULONG i;
	for(i = 0; i < m_Zones.ulTotalZones; i++ ){
		SetAreaStatusBit((&m_Zones.pZones[i]), VCA5_APP_AREA_STATUS_NORMAL);
		ClearAreaStatusBit((&m_Zones.pZones[i]), VCA5_APP_AREA_STATUS_SELECTED|VCA5_APP_AREA_STATUS_NOTIFIED);
	}
	for(i = 0; i < m_Counters.ulTotalCounters; i++ ){
		SetAreaStatusBit((&m_Counters.pCounters[i]), VCA5_APP_AREA_STATUS_NORMAL);
		ClearAreaStatusBit((&m_Counters.pCounters[i]), VCA5_APP_AREA_STATUS_SELECTED|VCA5_APP_AREA_STATUS_NOTIFIED);
	}
}

//////////////////////////////////////////////////////////////////////
// IVCAObserver related methods
//////////////////////////////////////////////////////////////////////
struct CVCADataMgr::Color CVCADataMgr::m_Colors[MAXZONECOLORS] = {
	{RGB(255,0,0), _T("Red")}, 
	{RGB(0,255,0), _T("Green")},
	{RGB(0,0,255), _T("Blue")},
	{RGB(255,255,0), _T("Yellow")},
	{RGB(255,0,255), _T("Fuchsia")},
	{RGB(0,255,255), _T("Aqua")},
	{RGB(255,165,0), _T("Orange")}
};

void	CVCADataMgr::RegisterObserver(IVCAConfigureObserver *pObserver)
{
//	if(m_nObserverCnt>=MAX_OBSERVVER_CNT){
//		TRACE("Observer register overlow Max[%d] Current[%d] \n", MAX_OBSERVVER_CNT, m_nObserverCnt);
//		return ;
//	}
//	m_pObservers[m_nObserverCnt++] = pObserver;

#ifdef _DEBUG
	std::vector< IVCAConfigureObserver *>::iterator it;
	for( it = m_Observers.begin(); it != m_Observers.end(); it++ )
	{
		if( (*it) == pObserver )
		{
			// Added twice!
			ASSERT( FALSE );
			break;
		}
	}
#endif // _DEBUG

	m_Observers.push_back( pObserver );
}

void	CVCADataMgr::UnregisterObserver(IVCAConfigureObserver *pObserver)
{
	BOOL bFound = FALSE;
	std::vector< IVCAConfigureObserver *>::iterator it;
	
	for( it = m_Observers.begin(); it != m_Observers.end(); it++ )
	{
		if( *it == pObserver )
		{
			m_Observers.erase( it );
			bFound = TRUE;
			break;
		}
	}

	// Not there!!!
	ASSERT( bFound );
}

void	CVCADataMgr::FireEvent(DWORD uiEventType, IVCAConfigureObserver *pActor, DWORD uiContext /*=NULL*/)
{
//	for(DWORD i = 0 ; i < m_nObserverCnt ; i++){
//		if(m_pObservers[i] != pActor) m_pObservers[i]->FireOnEvent(uiEventType, uiContext);
//	}
/*	std::vector< IVCAConfigureObserver *>::iterator it;
	for( it = m_Observers.begin(); it != m_Observers.end(); it++ )
	{
		if( *it != pActor )
		{
			(*it)->FireOnEvent( uiEventType, uiContext );
		}
	}*/
}

BOOL	CVCADataMgr::CheckFeature(int Feature)
{
	//Check calibration setting
/*	if(VCA5_FEATURE_CALIBRATION == Feature){
		if((m_EngineInfo.ulFunction&Feature))
			return (m_CalibInfo.calibrationStatus > VCA5_CALIB_STATUS_UNCALIBRATED)?TRUE:FALSE;
		else
			return FALSE;
	}
	return (m_EngineInfo.ulFunction&Feature);*/
	return true;
	return false;
}

BOOL	CVCADataMgr::CheckFeatureByRuleType(int RuleType)
{/*
	if(RuleType < VCA5_RULE_TYPE_AREA)
		return ((m_EngineInfo.ulFunction&(1<<RuleType)) && m_AdvInfo.TrackerParams.bMovObjEnabled);
	else if(RuleType == VCA5_RULE_TYPE_TAILGATING)
		return ((m_EngineInfo.ulFunction&VCA5_FEATURE_TAILGATING) && 
		(m_AdvInfo.TrackerParams.bMovObjEnabled || m_AdvInfo.TrackerParams.bCntLineEnabled));
	else if((RuleType == VCA5_RULE_TYPE_ABOBJ) || (RuleType == VCA5_RULE_TYPE_RMOBJ))	
		return ((m_EngineInfo.ulFunction&VCA5_FEATURE_ABOBJ) && m_AdvInfo.TrackerParams.bAbObjEnabled);
	else if(RuleType == VCA5_RULE_TYPE_LINECOUNTER_A || RuleType == VCA5_RULE_TYPE_LINECOUNTER_B)
		return ((m_EngineInfo.ulFunction&VCA5_FEATURE_LINECOUNTER) && m_AdvInfo.TrackerParams.bCntLineEnabled);
	else if(RuleType == VCA5_RULE_TYPE_SMOKE)
		return (m_EngineInfo.ulFunction&VCA5_FEATURE_SMOKE && m_AdvInfo.TrackerParams.bSmokeFireEnabled);
	else if(RuleType == VCA5_RULE_TYPE_FIRE)
		return (m_EngineInfo.ulFunction&VCA5_FEATURE_FIRE && m_AdvInfo.TrackerParams.bSmokeFireEnabled);
	else if(RuleType == VCA5_RULE_TYPE_COLSIG)
		return (m_EngineInfo.ulFunction&VCA5_FEATURE_PEOPLETRACKING
		&& m_AdvInfo.TrackerParams.bMovObjEnabled && m_AdvInfo.TrackerParams.bAdvTraEnabled);
		*/
	return FALSE;
}

void	CVCADataMgr::SetDisplayFlags(DWORD DisplayFlags)
{
	m_DisplayFlags = DisplayFlags;
}

COLORREF	CVCADataMgr::GetColor(DWORD id)
{
	return m_Colors[id % MAXZONECOLORS].Value;
}

LPCTSTR	CVCADataMgr::GetColorName(DWORD id)
{
	return m_Colors[id % MAXZONECOLORS].pName;
}

BOOL	CVCADataMgr::MoveArea(int iAreaIdx, VCA5_APP_AREA_T_E eAreaType, POINT *pDiff)
{
	VCA5_POINT *pPoints;
	ULONG		ulNumPoints;

	// 1. get zone or counter
	if (eAreaType == VCA5_APP_AREA_T_ZONE) {
		VCA5_APP_ZONE *pZone = GetZone(iAreaIdx);
		if (NULL == pZone) return FALSE;
		
		pPoints = &pZone->pPoints[0];
		ulNumPoints = pZone->ulTotalPoints;
	} else if (eAreaType == VCA5_APP_AREA_T_COUNTER){
		VCA5_APP_COUNTER *pCounter = GetCounter(iAreaIdx);
		if (NULL == pCounter) return FALSE;

		pPoints = &pCounter->pPoints[0];
		ulNumPoints = pCounter->ulTotalPoints;
	} else {
		return FALSE;
	}
	
	// 2. move points
	VCA5_POINT pTempPoints[VCA5_MAX_NUM_ZONE_POINTS];
	CopyMemory(&pTempPoints[0], pPoints, sizeof(VCA5_POINT)*ulNumPoints);
	for (ULONG i = 0; i < ulNumPoints; ++i) {
		LONG x = (LONG)pTempPoints[i].x - pDiff->x;
		LONG y = (LONG)pTempPoints[i].y - pDiff->y;
		if (x>=0 && x<=(NORMALISATOR-1) && y>=0 && y<=(NORMALISATOR-1)) {
			pTempPoints[i].x = (USHORT) x;
			pTempPoints[i].y = (USHORT) y;
		} else {
			return FALSE;
		}
	}

	// 3. copy the points to zone or counter
	CopyMemory(pPoints, &pTempPoints[0], sizeof(VCA5_POINT)*ulNumPoints);
	return TRUE;
}

BOOL	CVCADataMgr::ChangeNode(int iZoneIdx, int iPointIdx, POINT *pPoint)
{

	VCA5_APP_ZONE *pZone = GetZone(iZoneIdx);
	if ((!pZone) || (pZone->usZoneStyle & VCA5_ZONE_STYLE_NOTDEFINED) || ((ULONG)iPointIdx >= pZone->ulTotalPoints))
		return FALSE;
	
	VCA5_POINT*	pNode = &pZone->pPoints[iPointIdx];
	pNode->x = (short) pPoint->x;
	pNode->y = (short) pPoint->y;

	if ( pZone->usZoneStyle & VCA5_ZONE_STYLE_POLYGON ){
		// For polygon zones, first point is always the same as last point
		if (pNode == &pZone->pPoints[0]) {
			pZone->pPoints[pZone->ulTotalPoints-1] = *pNode;
		} else 	if (pNode == (&pZone->pPoints[0] + pZone->ulTotalPoints-1)) {
			pZone->pPoints[0] = *pNode;
		}
	}
	return TRUE;
}

POINT CVCADataMgr::GetNode( int iZoneIdx, int iPointIdx )
{
	POINT pt;

	VCA5_APP_ZONE *pZone = GetZone(iZoneIdx);
	if ((!pZone) || (pZone->usZoneStyle & VCA5_ZONE_STYLE_NOTDEFINED) || ((ULONG)iPointIdx >= pZone->ulTotalPoints))
	{
		pt.x = 0; pt.y = 0;
	}
	else
	{	
		pt.x = pZone->pPoints[iPointIdx].x;
		pt.y = pZone->pPoints[iPointIdx].y;
	}

	return pt;
}

int	CVCADataMgr::GetZoneIdx(VCA5_APP_ZONE *pZone)
{
	for (ULONG i = 0; i < m_Zones.ulTotalZones; ++i) {
		if (m_Zones.pZones[i].usZoneId == pZone->usZoneId) {
			return (int) i;
		}
	}
	return NOT_IN_USE;
}

int	CVCADataMgr::GetZoneIdx(ULONG id)
{
	for (ULONG i = 0; i < m_Zones.ulTotalZones; ++i) {
		if (m_Zones.pZones[i].usZoneId == id) {
			return (int) i;
		}
	}
	return NOT_IN_USE;
}

int CVCADataMgr::GetNumTickedCounter(VCA5_APP_COUNTER *pCounter)
{
	int iNum = 0;

	for (ULONG i = 0; i < pCounter->usNumSubCounters; ++i) {
		if (pCounter->uiSubCounterStatuses[i] & VCA5_APP_SUBCOUNTER_STATUS_TICKED) {
			iNum++;
		}
	}
	return iNum;
}

void	CVCADataMgr::FindSelectedAreaIdx(int *piAreaIdx, VCA5_APP_AREA_T_E *pType)
{
	ULONG i;
	for(i = 0; i < m_Zones.ulTotalZones; i++ ){
		if(m_Zones.pZones[i].uiStatus&VCA5_APP_AREA_STATUS_SELECTED){
			*piAreaIdx	= i;
			*pType		= VCA5_APP_AREA_T_ZONE;
			return;
		}
	}

	for(i = 0; i < m_Counters.ulTotalCounters; i++ ){
		if(m_Counters.pCounters[i].uiStatus&VCA5_APP_AREA_STATUS_SELECTED){
			*piAreaIdx	= i;
			*pType		= VCA5_APP_AREA_T_COUNTER;
			return;
		}
	}

	*piAreaIdx	= NOT_IN_USE;
	*pType		= VCA5_APP_AREA_T_NONE;
}

void CVCADataMgr::SetAreaStatus(int iAreaIdx, VCA5_APP_AREA_T_E eType, VCA5_APP_AREA_STATUS_E eStatus)
{
	switch( eType ){
	case VCA5_APP_AREA_T_ZONE:{
		VCA5_APP_ZONE	*pZone = GetZone( iAreaIdx );
		if ( pZone ){
			SetAreaStatusBit( pZone, eStatus );
		}
						 }break;
	case VCA5_APP_AREA_T_COUNTER:{
		VCA5_APP_COUNTER *pCounter = GetCounter( iAreaIdx );
		if ( pCounter ){
			SetAreaStatusBit( pCounter, eStatus );
		}
							}break;
	}
}

void CVCADataMgr::ClearAreaStatus( int iAreaIdx, VCA5_APP_AREA_T_E eType, VCA5_APP_AREA_STATUS_E eStatus )
{
	switch( eType ){
	case VCA5_APP_AREA_T_ZONE:{
		VCA5_APP_ZONE	*pZone = GetZone( iAreaIdx );
		if ( pZone ){
			ClearAreaStatusBit( pZone, eStatus );
		}
						 }break;
	case VCA5_APP_AREA_T_COUNTER:{
		VCA5_APP_COUNTER *pCounter = GetCounter( iAreaIdx );
		if ( pCounter ){
			ClearAreaStatusBit( pCounter, eStatus );
		}
							}break;
	}
}

void CVCADataMgr::SetSelectArea(int iAreaIdx, VCA5_APP_AREA_T_E eType)
{
//	if (((eType == m_eSelectAreaType) && (iAreaIdx == m_iSelectAreaIdx))) return;

	ClearAreaStatus(m_iSelectAreaIdx, m_eSelectAreaType, VCA5_APP_AREA_STATUS_SELECTED);
	ClearAreaStatus(m_iNotifyAreaIdx, m_eNotifyAreaType, VCA5_APP_AREA_STATUS_NOTIFIED);
	m_iSelectAreaIdx	= iAreaIdx;
	m_eSelectAreaType	= eType;
	SetAreaStatus(m_iSelectAreaIdx, m_eSelectAreaType, VCA5_APP_AREA_STATUS_SELECTED);
}

void CVCADataMgr::SetNotifyArea(int iAreaIdx, VCA5_APP_AREA_T_E eType)
{	
//	if (( eType == m_eNotifyAreaType ) && ( iAreaIdx == m_iNotifyAreaIdx )) return;

	ClearAreaStatus( m_iNotifyAreaIdx, m_eNotifyAreaType, VCA5_APP_AREA_STATUS_NOTIFIED);
	m_iNotifyAreaIdx	= iAreaIdx;
	m_eNotifyAreaType	= eType;
	SetAreaStatus(m_iNotifyAreaIdx, m_eNotifyAreaType, VCA5_APP_AREA_STATUS_NOTIFIED);
}

void	CVCADataMgr::ClearAllNotifyAreas( )
{
	ULONG i;
	VCA5_APP_ZONE *pZone;
	VCA5_APP_COUNTER *pCounter;

	for (i = 0; i < m_Zones.ulTotalZones; i++ ){
		pZone = &m_Zones.pZones[i];
		if (pZone->uiStatus == VCA5_APP_AREA_STATUS_NOTIFIED)
			 pZone->uiStatus =  VCA5_APP_AREA_STATUS_NORMAL;
	}

	for (i = 0; i < m_Counters.ulTotalCounters; i++ ){
		pCounter = &m_Counters.pCounters[i];
		if (pCounter->uiStatus == VCA5_APP_AREA_STATUS_NOTIFIED)
			pCounter->uiStatus =  VCA5_APP_AREA_STATUS_NORMAL;
	}
}

int		CVCADataMgr::AddFullScrrenZone(POINT *pPoint)
{
	USES_CONVERSION;
	if (m_Zones.ulTotalZones >= VCA5_MAX_NUM_ZONES) return NOT_IN_USE;

	int id = FindNextZoneID();
	int idx = (int)m_Zones.ulTotalZones;
	if (NOT_IN_USE == id){ return NOT_IN_USE; }

	VCA5_APP_ZONE &Zone = m_Zones.pZones[idx];

	Zone.usZoneId = (USHORT)id;
	Zone.usZoneStyle = VCA5_ZONE_STYLE_POLYGON;
	Zone.usZoneType = VCA5_ZONE_TYPE_DETECTION;
	_snprintf_s(Zone.szZoneName, _countof(Zone.szZoneName), VCA5_APP_MAX_NAME, "Zone %u", Zone.usZoneId);
	Zone.uiColor = GetColor(Zone.usZoneId);

	// set points
	long startx = pPoint->x, starty = pPoint->y;
	long endx = startx + MAX_ZONE_WIDTH, endy = starty + MAX_ZONE_HEIGHT;
	int	tmpx = endx, tmpy = endy;

	PERCENTASSERT(endx);
	PERCENTASSERT(endy);

	if (endx != tmpx){
		startx = GETPERCENT(100) - MAX_ZONE_WIDTH;
		endx = GETPERCENT(100);
	}

	if (endy != tmpy){
		starty = GETPERCENT(100) - MAX_ZONE_HEIGHT;
		endy = GETPERCENT(100);
	}

	Zone.pPoints[0].x = (USHORT)startx;
	Zone.pPoints[0].y = (USHORT)starty;
	Zone.pPoints[1].x = (USHORT)endx;
	Zone.pPoints[1].y = (USHORT)starty;
	Zone.pPoints[2].x = (USHORT)endx;
	Zone.pPoints[2].y = (USHORT)endy;
	Zone.pPoints[3].x = (USHORT)startx;
	Zone.pPoints[3].y = (USHORT)endy;
	Zone.pPoints[4].x = (USHORT)startx;
	Zone.pPoints[4].y = (USHORT)starty;
	Zone.ulTotalPoints = 5;
	++m_Zones.ulTotalZones;

	AddDefaultZoneRule(idx, TRUE, TRUE);
	Zone.uiStatus |= VCA5_APP_AREA_STATUS_INITIAL_RULE;
	return idx;
}

int		CVCADataMgr::AddZone(POINT *pPoint)
{
	USES_CONVERSION;
	if (m_Zones.ulTotalZones >= VCA5_MAX_NUM_ZONES) return NOT_IN_USE;
	
	int id = FindNextZoneID();
	int idx = (int) m_Zones.ulTotalZones;
	if(NOT_IN_USE == id){return NOT_IN_USE;}

	VCA5_APP_ZONE &Zone = m_Zones.pZones[idx];
	
	Zone.usZoneId		= (USHORT) id;
	Zone.usZoneStyle	= VCA5_ZONE_STYLE_POLYGON;
	Zone.usZoneType		= VCA5_ZONE_TYPE_DETECTION;
	_snprintf_s(Zone.szZoneName, _countof(Zone.szZoneName), VCA5_APP_MAX_NAME, "Zone %u", Zone.usZoneId);
	Zone.uiColor		= GetColor(Zone.usZoneId );	

	// set points
	long startx = pPoint->x, starty = pPoint->y;
	long endx = startx + DEFAULT_ZONE_WIDTH , endy = starty + DEFAULT_ZONE_HEIGHT;
	int	tmpx = endx, tmpy = endy;

	PERCENTASSERT( endx );
	PERCENTASSERT( endy );

	if 	( endx != tmpx ){
		startx = GETPERCENT(100) - DEFAULT_ZONE_WIDTH;
		endx = GETPERCENT(100);
	}

	if 	( endy != tmpy ){
		starty = GETPERCENT(100) - DEFAULT_ZONE_HEIGHT;
		endy = GETPERCENT(100);
	}

	Zone.pPoints[0].x = (USHORT)startx;
	Zone.pPoints[0].y = (USHORT)starty;
	Zone.pPoints[1].x = (USHORT)endx;
	Zone.pPoints[1].y = (USHORT)starty;
	Zone.pPoints[2].x = (USHORT)endx;
	Zone.pPoints[2].y = (USHORT)endy;
	Zone.pPoints[3].x = (USHORT)startx;
	Zone.pPoints[3].y = (USHORT)endy;
	Zone.pPoints[4].x = (USHORT)startx;
	Zone.pPoints[4].y = (USHORT)starty;
	Zone.ulTotalPoints = 5;
	++m_Zones.ulTotalZones;

	AddDefaultZoneRule(idx, TRUE, TRUE);
	Zone.uiStatus |= VCA5_APP_AREA_STATUS_INITIAL_RULE;
	return idx;
}

int		CVCADataMgr::AddLine(POINT *pPoint,bool bDirectA)
{
	if (m_Zones.ulTotalZones >= VCA5_MAX_NUM_ZONES) return NOT_IN_USE;

	int id = FindNextZoneID();
	if(NOT_IN_USE == id){return NOT_IN_USE;}
	int idx = (int) m_Zones.ulTotalZones;

	VCA5_APP_ZONE &Zone = m_Zones.pZones[idx];

	Zone.usZoneId		= (USHORT) id;
	Zone.usZoneStyle	= VCA5_ZONE_STYLE_TRIPWIRE;
	Zone.usZoneType		= VCA5_ZONE_TYPE_DETECTION;;

	_snprintf_s(Zone.szZoneName, _countof(Zone.szZoneName), VCA5_APP_MAX_NAME, "Zone %u", Zone.usZoneId);
	Zone.uiColor		= GetColor(Zone.usZoneId );	

	long startx = pPoint->x, starty = pPoint->y;
	long endy = starty + DEFAULT_ZONE_HEIGHT;
	int	tmpy = endy;
	PERCENTASSERT( endy );
	if 	( endy != tmpy ){
		starty = GETPERCENT(100) - DEFAULT_ZONE_HEIGHT;
		endy = GETPERCENT(100);
	}

	Zone.pPoints[0].x = (USHORT)startx;
	Zone.pPoints[0].y = (USHORT)starty;
	Zone.pPoints[1].x = (USHORT)startx;
	Zone.pPoints[1].y = (USHORT)endy;
	Zone.ulTotalPoints = 2;
	++m_Zones.ulTotalZones;

	AddDefaultZoneRule(idx, TRUE, TRUE);
	Zone.uiStatus |= VCA5_APP_AREA_STATUS_INITIAL_RULE;
	return idx;
}


BOOL	CVCADataMgr::RemoveZone(int iZoneIdx)
{
	VCA5_APP_ZONE *pDeletedZone = GetZone( iZoneIdx );
	if ( !pDeletedZone ) return FALSE;

	Lock();

	ULONG count;

	VCA5_APP_RULES *pDstRule = &m_Rules[iZoneIdx];
	VCA5_APP_RULES *pSrcRule = &m_Rules[iZoneIdx+1];
	count = m_Zones.ulTotalZones-iZoneIdx-1;
	MoveMemory(pDstRule, pSrcRule, count*sizeof(VCA5_APP_RULES));

	VCA5_APP_ZONE *pDST = &m_Zones.pZones[iZoneIdx];
	VCA5_APP_ZONE *pSRC = &m_Zones.pZones[iZoneIdx+1];
	count = m_Zones.ulTotalZones-iZoneIdx-1;
	MoveMemory(pDST, pSRC, count*sizeof(VCA5_APP_ZONE));

	if(count == 0){
		memset(pDstRule, 0, sizeof(VCA5_APP_RULES));
		memset(pDST, 0, sizeof(VCA5_APP_ZONE));
	}

	m_Zones.ulTotalZones--;

	Unlock();
	return TRUE;
}



BOOL	CVCADataMgr::AddNode(int ZoneIdx, int Nodeidx, POINT *point)
{
	VCA5_APP_ZONE *pZone = GetZone(ZoneIdx);
	if(!pZone || Nodeidx >= (int) pZone->ulTotalPoints) return FALSE;
	if(pZone->usZoneStyle & VCA5_ZONE_STYLE_NOTDEFINED) return FALSE; 
	for ( int i = (int) pZone->ulTotalPoints; i > Nodeidx; i--){
		pZone->pPoints[i].x = pZone->pPoints[i-1].x;
		pZone->pPoints[i].y = pZone->pPoints[i-1].y;
	}

	pZone->pPoints[Nodeidx].x = (USHORT)point->x;
	pZone->pPoints[Nodeidx].y = (USHORT)point->y;
	pZone->ulTotalPoints++ ;
	return TRUE;
}

BOOL	CVCADataMgr::RemoveNode(int ZoneIdx, int Nodeidx)
{
	VCA5_APP_ZONE *pZone = GetZone(ZoneIdx);
	if(!pZone || Nodeidx >= (int) pZone->ulTotalPoints) return FALSE;
	if(pZone->usZoneStyle & VCA5_ZONE_STYLE_NOTDEFINED) return FALSE; 
	if((pZone->ulTotalPoints <= 4) && (pZone->usZoneStyle&VCA5_ZONE_STYLE_POLYGON)) return FALSE;
	if((pZone->ulTotalPoints <= 2) && (pZone->usZoneStyle&VCA5_ZONE_STYLE_TRIPWIRE))return FALSE;
			
	Lock();

	for(ULONG i = Nodeidx; i < pZone->ulTotalPoints-1;  i++){
		pZone->pPoints[i].x = pZone->pPoints[i+1].x;
		pZone->pPoints[i].y = pZone->pPoints[i+1].y;
	}
	pZone->ulTotalPoints--;

	if((Nodeidx == 0) && (pZone->usZoneStyle & VCA5_ZONE_STYLE_POLYGON)){
		pZone->pPoints[pZone->ulTotalPoints-1].x = pZone->pPoints[0].x;
		pZone->pPoints[pZone->ulTotalPoints-1].y = pZone->pPoints[0].y;
	}

	Unlock();
	return TRUE;
}

int		CVCADataMgr::AddCounter(POINT *pPoint)
{
	USES_CONVERSION;
	if(m_Counters.ulTotalCounters >= VCA5_MAX_NUM_COUNTERS) return FALSE;

	int ICounterId	= FindNextCounterID();
	if (NOT_IN_USE == ICounterId) return NOT_IN_USE;

	// 1. calculate points
	long startx = pPoint->x, starty = pPoint->y;
	long endx = startx + DEFAULT_COUNTER_WIDTH , endy = starty + DEFAULT_COUNTER_HEIGHT;
	int	tmpx = endx, tmpy = endy;

	PERCENTASSERT( endx );
	PERCENTASSERT( endy );

	if 	( endx != tmpx ){
		startx = GETPERCENT(100) - DEFAULT_COUNTER_WIDTH;
		endx = GETPERCENT(100);
	}

	if 	( endy != tmpy ){
		starty = GETPERCENT(100) - DEFAULT_COUNTER_HEIGHT;
		endy = GETPERCENT(100);
	}

	// 2. add new counter
	VCA5_APP_COUNTER &Counter = m_Counters.pCounters[m_Counters.ulTotalCounters];
	memset( &Counter, 0 , sizeof (VCA5_APP_COUNTER));
	Counter.usCounterId		= (USHORT) ICounterId;
	Counter.usNumSubCounters = 0;
	Counter.uiStatus		= VCA5_APP_COUNTER_STATUS_NOT_USED;
	
	_snprintf_s(Counter.szCounterName, _countof(Counter.szCounterName), VCA5_APP_MAX_NAME, "Counter %d", ICounterId);
	Counter.uiColor = GetColor(Counter.usCounterId );	

	Counter.pPoints[0].x = (USHORT)startx;
	Counter.pPoints[0].y = (USHORT)starty;
	Counter.pPoints[1].x = (USHORT)endx;
	Counter.pPoints[1].y = (USHORT)starty;
	Counter.pPoints[2].x = (USHORT)endx;
	Counter.pPoints[2].y = (USHORT)endy;
	Counter.pPoints[3].x = (USHORT)startx;
	Counter.pPoints[3].y = (USHORT)endy;
	Counter.pPoints[4].x = (USHORT)startx;
	Counter.pPoints[4].y = (USHORT)starty;
	Counter.ulTotalPoints = 5;

	m_Counters.ulTotalCounters++;

	return m_Counters.ulTotalCounters-1;
}


BOOL CVCADataMgr::RemoveCounter(int iCounterIdx)
{
	if(iCounterIdx >= (int) m_Counters.ulTotalCounters || m_Counters.ulTotalCounters == 0) return FALSE;

	Lock();

	VCA5_APP_COUNTER *pDST = &m_Counters.pCounters[iCounterIdx];
	VCA5_APP_COUNTER *pSRC = &m_Counters.pCounters[iCounterIdx+1];
	DWORD count = (m_Counters.ulTotalCounters-iCounterIdx-1);

	MoveMemory(pDST, pSRC, count*sizeof(VCA5_APP_COUNTER));
	m_Counters.ulTotalCounters--;

	Unlock();
	return TRUE;
}


void CVCADataMgr::RemoveAll()
{
	Lock();
	memset(&m_Zones, 0, sizeof(VCA5_APP_ZONES));
	memset(&m_Rules, 0, sizeof(m_Rules));
	memset(&m_Counters, 0, sizeof(VCA5_APP_COUNTERS));
	Reset();
	Unlock();
}


void CVCADataMgr::SetSelectedNode(int idx)
{
	m_iSelectNodeIdx = idx;
}

void	CVCADataMgr::PreLoadRules()
{
	VCA5_APP_ZONE *pZone;

	for (ULONG i = 0; i < m_Zones.ulTotalZones ; i++){
		pZone = GetZone(i);
		if(pZone) AddDefaultZoneRule(i, TRUE, FALSE);
	}
}


BOOL	CVCADataMgr::AddDefaultZoneRule(int iZoneidx, BOOL bInit, BOOL bAddZone)
{
	SetPresence( iZoneidx); 
	SetEnter( iZoneidx);
	SetExit( iZoneidx);
	SetAppear( iZoneidx);
	SetDisappear( iZoneidx);
	SetStop( iZoneidx, bInit);
	SetDwell( iZoneidx, bInit, 20);
	SetDirection( iZoneidx, bInit, 0, 1800);
	SetSpeed( iZoneidx, bInit, 0, 200);
//	SetAbObj( iZoneidx, bInit, 20);
	SetRmObj( iZoneidx, bInit, 20);
	SetTailgating( iZoneidx, bInit, 5,0 );
	SetLineCounterA( iZoneidx, bInit, 4000);
	SetLineCounterB( iZoneidx, bInit, 4000);
	SetColFilter( iZoneidx, bInit, 0, 0 );
	SetSmoke( iZoneidx, bInit);
	SetFire( iZoneidx, bInit);


	if(bAddZone){
		VCA5_APP_RULE* pRule	= GetRule(iZoneidx, VCA5_RULE_TYPE_PRESENCE);
		VCA5_APP_ZONE* pZone	= GetZone(iZoneidx);
		pRule->ucTicked			= TRUE;
		sprintf_s(pRule->szRuleName, 32, "%.10s-%.20s", pZone->szZoneName, "Presence");

	}
	return TRUE;
}


BOOL	CVCADataMgr::GetRuleStatus( int zoneIdx, VCA5_RULE_TYPE_E eRuleType )
{
	VCA5_APP_RULE* pRule = GetRule(zoneIdx, eRuleType);
	return pRule->ucTicked;
}

BOOL	CVCADataMgr::SetDirection( int iZoneIdx, BOOL bInit, int startangle, int finishangle)
{	
	VCA5_APP_RULE *pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_DIRECTION);
	if(!pRule) return FALSE;

	if (bInit) {
		pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, VCA5_RULE_TYPE_DIRECTION);
		pRule->ucTicked		= FALSE;
		pRule->ucExpanded	= FALSE;
		pRule->usRuleType	= VCA5_RULE_TYPE_DIRECTION;
		pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	}

	pRule->tRuleDataEx.tDirection.sStartAngle	= startangle;
	pRule->tRuleDataEx.tDirection.sFinishAngle	= finishangle;
	return TRUE;
}

BOOL CVCADataMgr::SetColFilter( int iZoneIdx, BOOL bInit, unsigned short usColBin, unsigned short usThres )
{
	VCA5_APP_RULE *pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_COLSIG);
	if(!pRule) return FALSE;

	if (bInit) {
		pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, VCA5_RULE_TYPE_COLSIG);
		pRule->ucTicked		= FALSE;
		pRule->ucExpanded	= FALSE;
		pRule->usRuleType	= VCA5_RULE_TYPE_COLSIG;
		pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	}

	pRule->tRuleDataEx.tColorFilter.usColBin = usColBin;
	pRule->tRuleDataEx.tColorFilter.usThreshold = usThres;

	return TRUE;
}

BOOL CVCADataMgr::RemoveColFilter( int iZoneIdx )
{
	return RemoveRule( iZoneIdx, VCA5_RULE_TYPE_COLSIG );
}

BOOL	CVCADataMgr::SetSpeed( int iZoneIdx, BOOL bInit, USHORT usSpeedLo, USHORT usSpeedUp)
{
	VCA5_APP_RULE *pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_SPEED);
	if(!pRule) return FALSE;

	if (bInit) {
		pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, VCA5_RULE_TYPE_SPEED);
		pRule->ucTicked		= FALSE;
		pRule->ucExpanded	= FALSE;
		pRule->usRuleType	= VCA5_RULE_TYPE_SPEED;
		pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	}
		
	pRule->tRuleDataEx.tSpeed.usSpeedLo = usSpeedLo;
	pRule->tRuleDataEx.tSpeed.usSpeedUp = usSpeedUp;
	return TRUE;
}

BOOL	CVCADataMgr::SetRule_BASICT( int iZoneIdx, VCA5_RULE_TYPE_E eRule) 
{
	VCA5_APP_RULE *pRule = GetRule((ULONG) iZoneIdx, eRule);
	if(!pRule) return FALSE;

	pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, eRule);
	pRule->ucTicked		= FALSE;
	pRule->ucExpanded	= FALSE;
	pRule->usRuleType	= eRule;
	pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	sprintf( pRule->szRuleName, "Rule %d", pRule->ulRuleId );
	
	return TRUE;
}

BOOL	CVCADataMgr::SetRule_TIMERT( int iZoneIdx, VCA5_RULE_TYPE_E eRule, BOOL bInit, int iTime)
{
	VCA5_APP_RULE *pRule = GetRule((ULONG) iZoneIdx, eRule);
	if(!pRule) return FALSE;

	if(bInit){
		pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, eRule);
		pRule->ucTicked		= FALSE;
		pRule->ucExpanded	= FALSE;
		pRule->usRuleType	= eRule;
		pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	}
	pRule->tRuleDataEx.tTimer.ulTimeThreshold = iTime;
	return TRUE;
}

BOOL	CVCADataMgr::SetRule_LINECOUNTERT( int iZoneIdx, VCA5_RULE_TYPE_E eRule, BOOL bInit, ULONG ulCalibrationWidth, ULONG ulShadowFilterEnabled)
{
	VCA5_APP_RULE *pRule = GetRule((ULONG) iZoneIdx, eRule);
	if(!pRule) return FALSE;

	ulCalibrationWidth = min(0xFFFF, ulCalibrationWidth);
	
	if(bInit){
		pRule->ulRuleId		= GetInternalId(GetZone(iZoneIdx)->usZoneId, eRule);
		pRule->ucTicked		= FALSE;
		pRule->ucExpanded	= FALSE;
		pRule->usRuleType	= eRule;
		pRule->usZoneId		= GetZone(iZoneIdx)->usZoneId;
	}
	pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth = ulCalibrationWidth;
	pRule->tRuleDataEx.tLineCounter.ulShadowFilterEnabled = ulShadowFilterEnabled;

	return TRUE;
}

BOOL	CVCADataMgr::SetRule( int iZoneIdx, VCA5_RULE_TYPE_E eRuleType ) 
{
	if(VCA5_RULE_TYPE_PRESENCE == eRuleType)	return SetPresence(iZoneIdx);
	else if(VCA5_RULE_TYPE_ENTER == eRuleType) return SetEnter(iZoneIdx);
	else if(VCA5_RULE_TYPE_EXIT == eRuleType) return SetExit(iZoneIdx);
	else if(VCA5_RULE_TYPE_APPEAR == eRuleType) return SetAppear(iZoneIdx);
	else if(VCA5_RULE_TYPE_DISAPPEAR == eRuleType) return SetDisappear(iZoneIdx);
	else if(VCA5_RULE_TYPE_STOP == eRuleType) return SetStop(iZoneIdx);
	else if(VCA5_RULE_TYPE_DWELL == eRuleType) return SetDwell(iZoneIdx);
	else if(VCA5_RULE_TYPE_DIRECTION == eRuleType) return SetDirection(iZoneIdx);
	else if(VCA5_RULE_TYPE_SPEED == eRuleType) return SetSpeed(iZoneIdx);
	else if(VCA5_RULE_TYPE_TAILGATING == eRuleType) return SetTailgating(iZoneIdx);
	//else if(VCA5_RULE_TYPE_ABOBJ == eRuleType) return SetAbObj(iZoneIdx);
	else if(VCA5_RULE_TYPE_LINECOUNTER_A == eRuleType) return SetLineCounterA(iZoneIdx);
	else if(VCA5_RULE_TYPE_LINECOUNTER_B == eRuleType) return SetLineCounterB(iZoneIdx);
	else if(VCA5_RULE_TYPE_RMOBJ == eRuleType) return SetRmObj(iZoneIdx);
	else if(VCA5_RULE_TYPE_COLSIG == eRuleType) return SetColFilter(iZoneIdx);
	else if(VCA5_RULE_TYPE_SMOKE == eRuleType) return SetSmoke(iZoneIdx);
	else if(VCA5_RULE_TYPE_FIRE == eRuleType) return SetFire(iZoneIdx);

	return FALSE;
}

BOOL	CVCADataMgr::RemoveRule( int iZoneIdx, VCA5_RULE_TYPE_E eRule ) 
{
	VCA5_APP_RULE *pRule = GetRule(iZoneIdx, eRule);
	if (pRule) {
		pRule->ucTicked	= pRule->ucUsed = pRule->ucExpanded = 0;
		return TRUE;
	} else {
		return FALSE;
	}
}

int		CVCADataMgr::FindNextZoneID()
{
	int i,j, id;
	BOOL bFind;
	id = m_Zones.ulTotalZones;
	for(i = 0; i < (int) m_Zones.ulTotalZones; i++){
		bFind = FALSE;
		for(j = 0; j < (int) m_Zones.ulTotalZones; j++){
			if(i == m_Zones.pZones[j].usZoneId){
				bFind = TRUE;
				break;
			}
		}
		if(!bFind){
			id = i;
			break;
		}
	}
	return id;
}

int		CVCADataMgr::FindNextCounterID()
{
	int i,j;
	BOOL bFind;
	int  id = m_Counters.ulTotalCounters;
	for(i = 0; i < (int) m_Counters.ulTotalCounters; i++){
		bFind = FALSE;
		for(j = 0; j < (int) m_Counters.ulTotalCounters; j++){
			if(i == m_Counters.pCounters[j].usCounterId){
				bFind = TRUE;
				break;
			}
		}
		if(!bFind){
			id = i;
			break;
		}
	}
	return id;
}

void CVCADataMgr::GetZoneCenter( VCA5_APP_ZONE *pZone, VCA5_POINT *pt )
{
	int x=0, y=0;
	int i;

	if (pZone->ulTotalPoints == 1)
	{
		pt->x = 0;
		pt->y = 0;
	}
	else
	{
		int iNumPoints;
		if (pZone->usZoneStyle & VCA5_ZONE_STYLE_POLYGON )
			iNumPoints =  pZone->ulTotalPoints - 1;
		else
			iNumPoints = pZone->ulTotalPoints;
		for ( i = 0; i < iNumPoints; i++)
		{
			x += pZone->pPoints[i].x;
			y += pZone->pPoints[i].y;
		}
		x /= iNumPoints;
		y /= iNumPoints;

		pt->x = x;
		pt->y = y;
	}
}

ANGLE_STATUS CVCADataMgr::CheckZoneAngle( int iZoneIdx )
{
	VCA5_APP_RULE	*pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_DIRECTION);
	if (pRule) {
		if ( pRule->tRuleDataEx.tDirection.sFinishAngle - pRule->tRuleDataEx.tDirection.sStartAngle >= 3600 ) {
			return ANGLE_FULL;
		} else {
			return ANGLE_PART;
		}
	}
	return ANGLE_NOTDEFINED;
}



BOOL	CVCADataMgr::IsItACountingLine(int iZoneIdx)
{
	VCA5_APP_RULE	*pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_A);
	if(pRule && pRule->ucTicked) return TRUE;

	pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_B);
	if(pRule && pRule->ucTicked) return TRUE;

	return FALSE;
}

ULONG	CVCADataMgr::GetLineCounterA( int iZoneIdx)
{
	VCA5_APP_RULE	*pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_A);
	if(pRule && pRule->ucTicked){
		return pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth;
	}
	return 0;
}

ULONG	CVCADataMgr::GetLineCounterB( int iZoneIdx) 
{
	VCA5_APP_RULE	*pRule = GetRule(iZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_B);
	if(pRule && pRule->ucTicked){
		return pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth;
	}
	return 0;
}


void	CVCADataMgr::SetData(VCA5_APP_ZONES* pZones, VCA5_APP_RULES* pRules, VCA5_APP_COUNTERS* pCounters, 
							VCA5_APP_CALIB_INFO* pCalibInfo, VCA5_APP_CLSOBJECTS* pClsObjects)
{
	memcpy(&m_Zones, pZones, sizeof(VCA5_APP_ZONES));
	memcpy(&m_Counters, pCounters, sizeof(VCA5_APP_COUNTERS));
	memcpy(&m_CalibInfo, pCalibInfo, sizeof(VCA5_APP_CALIB_INFO));
	memcpy(&m_ClsObjects, pClsObjects, sizeof(VCA5_APP_CLSOBJECTS));
	
	// BW - Check for uncalibrated state and if uncalibrated, apply some sensible defaults
	if( VCA5_CALIB_STATUS_UNCALIBRATED == m_CalibInfo.calibrationStatus )
	{
		m_CalibInfo.fFOV			= 40.0f;
		m_CalibInfo.fHeight			= 10.0f;
		m_CalibInfo.fTilt			= 30.0f;
		m_CalibInfo.fPan			= 0.0f;
		m_CalibInfo.fRoll			= 0.0f;
		m_CalibInfo.ulHeightUnits	= VCA5_HEIGHT_UNITS_METERS;
	}

	m_RuleIdMap.clear();

	// Set Rules
	USHORT	usZoneID;
	USHORT	usType;
	int		iZoneIdx;

	//std::map< int, int > backIdMap;

	memset(&m_Rules, 0, sizeof(m_Rules));
	PreLoadRules();
	for (ULONG i = 0; i < pRules->ulTotalRules; i++)
	{
		usZoneID = pRules->pRules[i].usZoneId;
		usType = pRules->pRules[i].usRuleType;
		
		iZoneIdx = GetZoneIdx(usZoneID);
		if (iZoneIdx != NOT_IN_USE)
		{
			int iRealId = pRules->pRules[i].ulRuleId;
			// BW - build rule id map
			m_Rules[iZoneIdx].pRules[usType] = pRules->pRules[i];
			m_Rules[iZoneIdx].pRules[usType].ulRuleId = GetInternalId( usZoneID, (VCA5_RULE_TYPE_E)usType );
			m_Rules[iZoneIdx].pRules[usType].ucTicked = TRUE;

			m_RuleIdMap.insert( std::pair< int, int > ( iRealId, m_Rules[iZoneIdx].pRules[usType].ulRuleId) );
			// Stick it in the other way round too
	//		backIdMap.insert( std::pair< int, int > ( m_Rules[iZoneIdx].pRules[usType].ulRuleId, iRealId ) );
		}
	}

	// Go through and fix up counter ids
	for( ULONG i = 0; i < m_Counters.ulTotalCounters; i++ )
	{
		VCA5_APP_COUNTER *pCtr = &(m_Counters.pCounters[i]);

		for( ULONG j = 0; j < pCtr->usNumSubCounters; j++ )
		{
			VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);
			std::map< int, int >::iterator mapIt = m_RuleIdMap.find( pSc->usTrigId );
			if( mapIt != m_RuleIdMap.end() )
			{
				// Assign new id
				pSc->usTrigId = mapIt->second;
			}
			else
			{
				// Cannot find!
				// if remove zone or rule then it can not find rule
				///ASSERT( FALSE );
			}
		}
	}
}

void	CVCADataMgr::GetData(VCA5_APP_ZONES *pZones, VCA5_APP_RULES *pDstRules, VCA5_APP_COUNTERS *pCounters, VCA5_APP_CALIB_INFO *pCalibInfo, VCA5_APP_CLSOBJECTS *pClsObjects)
{
	memcpy(pZones, &m_Zones, sizeof(VCA5_APP_ZONES));
	
	// Build an id mapping so the counters can be updated
	std::map< int, int > idMap;

	//arrage rules
	ULONG i,j,k;
	memset(pDstRules, 0, sizeof(VCA5_APP_RULES));
	k = 0;
	for (i = 0; i < m_Zones.ulTotalZones; i++)
	{
		//remove rule related VCA5_ZONE_TYPE_NONDETECTION
		if(VCA5_ZONE_TYPE_DETECTION == m_Zones.pZones[i].usZoneType)
		{
			for (j = 0; j < VCA5_RULE_TYPE_NUM; j++)
			{
				VCA5_APP_RULE *pRule = GetRule(i, (VCA5_RULE_TYPE_E) j);
				if (pRule && pRule->ucTicked)
				{
					int iInternalId = pRule->ulRuleId;

					idMap.insert( std::pair< int, int > (iInternalId, k) );

					memcpy(&pDstRules->pRules[k], pRule, sizeof(VCA5_APP_RULE));

					pDstRules->pRules[k].ulRuleId = k;

					k++;
				}
			}
		}
	}
	pDstRules->ulTotalRules = k;


	memcpy(pCounters, &m_Counters, sizeof(VCA5_APP_COUNTERS));
	VCA5_APP_COUNTER  DstCounter;

	// Now go through and re-assign counter ids
	for( i = 0; i < pCounters->ulTotalCounters; i++ )
	{
		VCA5_APP_COUNTER *pCtr = &(pCounters->pCounters[i]);
		memcpy(&DstCounter, pCtr, sizeof(VCA5_APP_COUNTER));

		// Examine subcounters
		k = 0;
		for(USHORT j = 0; j < pCtr->usNumSubCounters; j++ )
		{
			VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);
			std::map< int, int >::iterator mapIt = idMap.find( pSc->usTrigId );
			if( mapIt != idMap.end() )
			{
				pSc->usTrigId = mapIt->second;
				memcpy(&DstCounter.pSubCounters[k], pSc,sizeof(VCA5_SUBCOUNTER));
				k++;
			}
		}

		DstCounter.usNumSubCounters = (USHORT)k;
		memcpy(pCtr, &DstCounter, sizeof(VCA5_APP_COUNTER));
	}


	memcpy(pCalibInfo, &m_CalibInfo, sizeof(VCA5_APP_CALIB_INFO));
	memcpy(pClsObjects, &m_ClsObjects, sizeof(VCA5_APP_CLSOBJECTS));
}


#define HIST_LENGTH 5
#define MAX_EVENT_AGE 25*60

void CVCADataMgr::UpdateCountingLineEvents(VCA5_COUNTLINE_EVENTS* countLineEvents)
{
	unsigned int n;
	std::list<CNT_LINE_EVENTHIST>::iterator it;


	/// add new ones
	for (n = 0; n < countLineEvents->ulTotalCountLineEvents; n++){
		VCA5_COUNTLINE_EVENT *pEvent = &countLineEvents->CountLineEvents[n];
		if (pEvent == NULL) continue;


		VCA5_APP_RULE*	pRule = GetRuleByRealId( pEvent->uiId );
		if(!pRule) continue;
		VCA5_APP_ZONE*	pZone = GetZoneById( pRule->usZoneId );
		
		if (pZone == NULL || pRule == NULL) continue;

		if (VCA5_RULE_TYPE_LINECOUNTER_A == pRule->usRuleType){	
			CNT_LINE_EVENTHIST evntHist; 

			evntHist.info = *pEvent;
			evntHist.pZone = pZone;
			evntHist.age = 0;

			m_countLineEventsA[pZone->usZoneId].push_front(evntHist);
		}else if (VCA5_RULE_TYPE_LINECOUNTER_B == pRule->usRuleType){
			CNT_LINE_EVENTHIST evntHist; 

			evntHist.info =  *pEvent;
			evntHist.pZone = pZone;
			evntHist.age = 0;

			m_countLineEventsB[pZone->usZoneId].push_front(evntHist);
		}
	}
	
	/// erase old ones
	for (n = 0; n < VCA5_MAX_NUM_ZONES; n++)
	{
		while(m_countLineEventsA[n].size() > HIST_LENGTH) m_countLineEventsA[n].pop_back();
		while(m_countLineEventsB[n].size() > HIST_LENGTH) m_countLineEventsB[n].pop_back();

		for (it = m_countLineEventsA[n].begin(); it != m_countLineEventsA[n].end();)
		{
			std::list<CNT_LINE_EVENTHIST>::iterator it2 = it;
			it++;  
			if (it2->age > MAX_EVENT_AGE)
				m_countLineEventsA[n].erase(it2);

		}
		for (it = m_countLineEventsB[n].begin(); it != m_countLineEventsB[n].end();)
		{
			std::list<CNT_LINE_EVENTHIST>::iterator it2 = it;
			it++; 
			if (it2->age > MAX_EVENT_AGE)
				m_countLineEventsB[n].erase(it2);
		}
	}

}

void CVCADataMgr::ClearCountingLineEvents()
{
//	int iEventIdx, iRuleIdx;

	Lock();
	for (int n = 0; n < VCA5_MAX_NUM_ZONES; n++)
	{
		m_countLineEventsA[n].clear();
		m_countLineEventsB[n].clear();
/*
		//if line change clear the events
		VCA5_APP_RULE*	pRuleA = GetRule( m_Zones.pZones[n].usZoneId,  VCA5_RULE_TYPE_LINECOUNTER_A);
		VCA5_APP_RULE*	pRuleB = GetRule( m_Zones.pZones[n].usZoneId,  VCA5_RULE_TYPE_LINECOUNTER_B);

		if(pRuleA || pRuleB){
			if (memcmp(&m_Zones.pZones[n].pPoints, &m_pNativeZones->pZones[n].pPoints, MAX_NUM_ZONE_POINTS*sizeof(VCA_POINT_T))
				|| memcmp(&m_pEvents->pEvents[iEventIdx].pRules[0], &m_pNativeEvents->pEvents[iEventIdx].pRules[0], sizeof(VCA_RULE_T)) )
			{
				m_countLineEventsA[n].clear();
				m_countLineEventsB[n].clear();
			}
		}else{
			m_countLineEventsA[n].clear();
			m_countLineEventsB[n].clear();
		}
*/
	}
	Unlock();
}


BOOL	CVCADataMgr::InsertObjectModel(D3DXVECTOR3 pos)
{
	if (m_iNumObjects < MAXNUM_OBJECTS)
	{
		m_objectModels[m_iNumObjects].vecPos = pos;
		m_iNumObjects++;
		return TRUE;
	}

	return FALSE;
}


void	CVCADataMgr::ResetObjectModel()
{
	m_iNumObjects		= 2;
	m_bResetObjectModel	= TRUE;
}