#include "stdafx.h"
#include "../resource.h"
#include "ZoneSettingCtl.h"
#include "../Render/VCARender.h"
//#include <math.h>

//#include "../VCAEngine/VCAEngine.h"
#include "../Render/Triangulation.h"
#include "../../cdialogvca.h"

#define ConvertPOINTSToPOINT(points, point) {point.x = points.x ; point.y = points.y;}


CZoneSettingCtl::CZoneSettingCtl(void)
{
	m_bSetup		= FALSE;
	m_pDataMgr		= NULL;
	m_hWnd			= NULL;
	m_bChangingDirection	= FALSE;
	m_bChangingLineCounter	= FALSE;
		
	Reset();
}

CZoneSettingCtl::~CZoneSettingCtl(void)
{

}

void CZoneSettingCtl::CreateMenu()
{
	


}


BOOL CZoneSettingCtl::Setup(CDialogVCA *pVCADlg)
{
	if(m_bSetup){
		TRACE("CZoneSettingCtl::Setup before \n");
		return TRUE;
	}
	
	m_pVCADialog = pVCADlg;
	m_pDataMgr = pVCADlg->GetVCADataMgr();
	
	m_pDataMgr->RegisterObserver(this);
	m_hWnd = pVCADlg->GetRenderHwnd();

//	CreateMenu();
	Reset();

	m_bSetup = TRUE;
	return TRUE;
}


void CZoneSettingCtl::Reset()
{
	m_bCaptured			= FALSE;
	m_nActiveAreaIdx	= NOT_IN_USE;
	m_eActiveAreaType	= VCA5_APP_AREA_T_NONE;
	m_nActiveNode		= NOT_IN_USE;
	
	memset(&m_CurPoint, 0, sizeof(POINT));
	memset(&m_CurCapPoint, 0, sizeof(POINT));

	m_CurPoint.x = 200;
	m_CurPoint.y = 200;
}


void CZoneSettingCtl::OnLButtonDown(UINT nFlags, POINT PixPoint)
{
	SetFocus( m_hWnd );

	POINT PerPoint = PixPoint;
	PixelToPercent(&PerPoint);
	m_bCaptured		= TRUE;
	
	m_CurCapPoint	= PixPoint;
	PixelToPercent( &m_CurCapPoint );

	::SetCapture( m_hWnd );
	
	int nPrevAreaIdx = m_nActiveAreaIdx;	// for capturing direction Arrows
	VCA5_APP_AREA_T_E ePrevAreaType = m_eActiveAreaType;
	
	//find node
	m_pDataMgr->GetSelectAreaIdx(&m_nActiveAreaIdx, &m_eActiveAreaType);
	if(VCA5_APP_AREA_T_ZONE == m_eActiveAreaType){
		m_nActiveNode		= FindNodeIndex(m_nActiveAreaIdx, &PerPoint);	
	}
	

	if(NOT_IN_USE == m_nActiveAreaIdx && VCA5_APP_AREA_T_ZONE == ePrevAreaType) // for capturing direction Arrows and Counterline
	{	
		m_bChangingDirection = CheckDirectionSetting(nPrevAreaIdx, PerPoint);
		if(m_bChangingDirection == TRUE) 
		{
			m_nActiveAreaIdx = nPrevAreaIdx;
			m_eActiveAreaType = ePrevAreaType;
		}

		m_bChangingLineCounter = CheckDirectionSetting(nPrevAreaIdx, PerPoint);
		if(m_bChangingLineCounter == TRUE) 
		{
			m_nActiveAreaIdx = nPrevAreaIdx;
			m_eActiveAreaType = ePrevAreaType;
		}

	} else if (VCA5_APP_AREA_T_ZONE == m_eActiveAreaType) 
	{
		m_bChangingDirection	= CheckDirectionSetting(m_nActiveAreaIdx, PerPoint);
		m_bChangingLineCounter	= CheckLineCounterSetting(m_nActiveAreaIdx, PerPoint);
	} else {
		m_bChangingDirection	= FALSE;
		m_bChangingLineCounter	= FALSE;
	}


	if( !m_bChangingDirection && !m_bChangingLineCounter )
	{
		//find zone 
		if(NOT_IN_USE == m_nActiveNode){
			FindAreaIndex(&m_nActiveAreaIdx, &m_eActiveAreaType, &PerPoint);
			if (m_eActiveAreaType == VCA5_APP_AREA_T_ZONE) {
				m_nActiveNode		= FindNodeIndex(m_nActiveAreaIdx, &PerPoint);	
			} else {
				m_nActiveNode		= NOT_IN_USE;
			}
		}
	}

	if(!m_bChangingDirection && !m_bChangingLineCounter && NOT_IN_USE == m_nActiveAreaIdx)
	{
		SetAreaMouseClip(m_nActiveAreaIdx, m_hWnd);
	}

	m_pDataMgr->SetSelectArea(m_nActiveAreaIdx, m_eActiveAreaType);
	
	if(VCA5_APP_AREA_T_ZONE == m_eActiveAreaType)
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_ZONE_UPDATE, this);
	else if(VCA5_APP_AREA_T_COUNTER == m_eActiveAreaType)
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_COUNTER_UPDATE, this);
	else 
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_NO_CHANGE, this);

}


void CZoneSettingCtl::OnLButtonUp(UINT nFlags, POINT point)
{
	m_bCaptured				= FALSE;
	m_bChangingDirection	= FALSE;
	m_bChangingLineCounter	= FALSE;

	::ReleaseCapture();
	ClipCursor(NULL);
}


void CZoneSettingCtl::OnLButtonDblClk( UINT nFlags, POINT point )
{
	if( NOT_IN_USE == m_nActiveAreaIdx )
	{
		AddZone( TRUE );
	}
	else
	{
		AddNode();
	}
}


void CZoneSettingCtl::OnMouseMove(UINT nFlags, POINT PixPoint)
{
	//m_CurPoint = PixPoint;
	if( !m_bCaptured ) return;


	m_pDataMgr->Lock();

	
	// Clip point to client window
	RECT rcWnd;
	rcWnd = m_ClientRect;
	PixPoint.x = max( PixPoint.x, 1 );
	PixPoint.y = max( PixPoint.y, 1 );
	PixPoint.x = min( PixPoint.x, rcWnd.right-1 );
	PixPoint.y = min( PixPoint.y, rcWnd.bottom-1 );

	POINT PerPoint = PixPoint;				
	POINT PreviewPerPoint = m_CurCapPoint;	

	PixelToPercent(&PerPoint);				
//	PixelToPercent(&PreviewPerPoint);


	if (VCA5_APP_AREA_T_ZONE == m_eActiveAreaType && NOT_IN_USE != m_nActiveNode) {
		POINT oldPt = m_pDataMgr->GetNode(m_nActiveAreaIdx, m_nActiveNode);
		
		m_pDataMgr->ChangeNode(m_nActiveAreaIdx, m_nActiveNode, &PerPoint);
		
		if( !ValidateZone( m_nActiveAreaIdx ) )//ÇøÓò
		{
			// Put it back
			m_pDataMgr->ChangeNode( m_nActiveAreaIdx, m_nActiveNode, &oldPt );//ÇøÓò
		}
	}else if (m_bChangingDirection) {
		//ChangeDirectionArrows( PerPoint );
	//	m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
	}else if(m_bChangingLineCounter){
		ChangeLineCounter( m_nActiveAreaIdx, PerPoint );
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
	}else if (m_nActiveAreaIdx != NOT_IN_USE) 
	{
		POINT Diff;
		Diff.x = PreviewPerPoint.x - PerPoint.x; Diff.y = PreviewPerPoint.y - PerPoint.y;
		
		if( m_pDataMgr->MoveArea(m_nActiveAreaIdx, m_eActiveAreaType, &Diff) )
		{
			m_CurCapPoint = PerPoint;
		}
	}

	m_pDataMgr->Unlock();
}

void CZoneSettingCtl::OnRButtonDown(UINT nFlags, POINT point)
{
	ShowMenu(point, 255);
}

void CZoneSettingCtl::OnMouseWheel(UINT nFlags, short zDelta, POINT points)
{
	int					iAreaIdx;
	VCA5_APP_AREA_T_E	eAreaType;

	m_pDataMgr->GetSelectAreaIdx( &iAreaIdx, &eAreaType );

	if(m_pDataMgr->IsItACountingLine(iAreaIdx) && m_pDataMgr->GetLineCounterA(iAreaIdx)){
		//m_pDataMgr->ShowLineCounterHelp(FALSE);

		m_pDataMgr->SetLineCounterA( iAreaIdx, FALSE, max(1, m_pDataMgr->GetLineCounterA(iAreaIdx) + 3*zDelta));
		m_pDataMgr->SetLineCounterB( iAreaIdx, FALSE, max(1, m_pDataMgr->GetLineCounterB(iAreaIdx) + 3*zDelta));

		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
	}
}

void CZoneSettingCtl::ShowMenu(POINT point, UINT uiDisplayMenuHiliteItem)
{
/*	ULONG ulFunction = 0;

	CEngine *pEngine = m_pVCACtrl->GetEngine();
	ASSERT( pEngine );

	ulFunction = pEngine->GetFunction();

	HMENU	hShowMenu;
	
	POINT PerPoint = point;
	PixelToPercent(&PerPoint);

	// 1. get active area
	FindAreaIndex(&m_nActiveAreaIdx, &m_eActiveAreaType, &PerPoint);
	if (VCA5_APP_AREA_T_ZONE == m_eActiveAreaType) {
		m_nActiveNode	= FindNodeIndex(m_nActiveAreaIdx, &PerPoint);
	} else {
		m_nActiveNode = NOT_IN_USE;
	}

	m_pDataMgr->SetSelectArea(m_nActiveAreaIdx, m_eActiveAreaType);
	
	if(VCA5_APP_AREA_T_ZONE == m_eActiveAreaType)
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_ZONE_UPDATE, this);
	else if(VCA5_APP_AREA_T_COUNTER == m_eActiveAreaType)
		m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_COUNTER_UPDATE, this);

	
	// 2. set pop-up menu
	ClientToScreen(m_hWnd, &point);
	if(NOT_IN_USE == m_nActiveAreaIdx){
		DWORD DisplayFlags = m_pDataMgr->GetDisplayFlags();
		BOOL bCalib = m_pDataMgr->CheckFeature(VCA5_FEATURE_CALIBRATION);
		BOOL bEnabledCalibMenus = bCalib ? MF_ENABLED : MF_DISABLED|MF_GRAYED;

		DWORD eng = m_pVCACtrl->GetEngine()->GetEngId();
		VCA5_APP_ENGINE_CONFIG* pEngCfg = CVCAConfigure::Instance()->GetEngineConf(eng);

		BOOL bMovObjEnabled = pEngCfg->AdvInfo.TrackerParams.bMovObjEnabled;
		BOOL bAdvTraEnabled = pEngCfg->AdvInfo.TrackerParams.bAdvTraEnabled;
		BOOL bEnabledColorSigMenu = bMovObjEnabled && bAdvTraEnabled;
		UINT uColorSigMenuState = bEnabledColorSigMenu ? MF_ENABLED : MF_DISABLED|MF_GRAYED;;

		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_BLOBS, DisplayFlags & IVCARender::DISPLAY_BLOBS 
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_NON_ALARMS, DisplayFlags & IVCARender::DISPLAY_NON_ALARMS 
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_TARGETID, DisplayFlags & IVCARender::DISPLAY_TARGETID
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_OBJECTHEIGHT, DisplayFlags & IVCARender::DISPLAY_OBJECT_HEIGHT
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_OBJECTSPEED, DisplayFlags & IVCARender::DISPLAY_OBJECT_SPEED
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_OBJECTAREA, DisplayFlags & IVCARender::DISPLAY_OBJECT_AREA
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_OBJECTCLASSIFICATION, DisplayFlags & IVCARender::DISPLAY_OBJECT_CLASSIFICATION
			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_OBJECTCOLORSIGNATURE, (DisplayFlags & IVCARender::DISPLAY_OBJECT_COLOR_SIGNATURE)
			&& bEnabledColorSigMenu ? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );
//		CheckMenuItem( m_hMenuDisplay,	ID_DISPLAY_TAMPER_ALARM, DisplayFlags & IVCARender::DISPLAY_TAMPER_ALARM
//			? MF_CHECKED : MF_UNCHECKED | MF_BYCOMMAND );

		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_BLOBS, (ulFunction)? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_NON_ALARMS, (ulFunction)? MF_ENABLED:MF_DISABLED|MF_GRAYED);
	EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_TARGETID, (ulFunction&VCA5_FEATURE_PRESENCE)? MF_ENABLED:MF_DISABLED|MF_GRAYED);
//		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_TAMPER_ALARM, (ulFunction&VCA5_FEATURE_PRESENCE)? MF_ENABLED:MF_DISABLED|MF_GRAYED);

		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_OBJECTHEIGHT, bEnabledCalibMenus);
		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_OBJECTSPEED, bEnabledCalibMenus);
		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_OBJECTAREA, bEnabledCalibMenus);
		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_OBJECTCLASSIFICATION, bEnabledCalibMenus);
		EnableMenuItem(m_hMenuDisplay, ID_DISPLAY_OBJECTCOLORSIGNATURE, uColorSigMenuState);
			
		//EnableMenuItem(m_hMainMenu, (UINT)m_hMenuCounter, (ulFunction&VCA5_FEATURE_COUNTING)? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		hShowMenu = m_hMainMenu;

	}else if (VCA5_APP_AREA_T_ZONE == m_eActiveAreaType) {
		VCA5_APP_ZONE* pZone = m_pDataMgr->GetZone(m_nActiveAreaIdx);
		if(!pZone) return;

		BOOL bColorFilterMenuEnabled = m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_COLSIG);
		BOOL bColorFilterChecked = bColorFilterMenuEnabled && m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_COLSIG );

		CheckMenuItem( m_hMenuRule, ID_PRESENCE, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_PRESENCE ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_ENTER, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_ENTER ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_EXIT, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_EXIT ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_APPEAR, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_APPEAR ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_DISAPPEAR, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_DISAPPEAR ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_STOP, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_STOP ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_DWELL, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_DWELL ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_DIRECTION, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_DIRECTION ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_SPEED, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_SPEED ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_TAILGATING, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_TAILGATING ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_ABANDONREMOVE, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_RMOBJ ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_COLOR_FILTER, (bColorFilterChecked ? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_SMOKE, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_SMOKE ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		CheckMenuItem( m_hMenuRule, ID_FIRE, (m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, VCA5_RULE_TYPE_FIRE ) 
			? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND );
		
		EnableMenuItem(m_hMenuRule, ID_PRESENCE, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_PRESENCE))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_ENTER, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_ENTER))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_EXIT, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_EXIT))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_APPEAR, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_APPEAR))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_DISAPPEAR, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_DISAPPEAR))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_STOP, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_STOP))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_DWELL, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_DWELL))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_DIRECTION, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_DIRECTION))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_SPEED, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_SPEED))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_TAILGATING, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_TAILGATING))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_ABANDONREMOVE, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_RMOBJ))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_COLOR_FILTER, bColorFilterMenuEnabled ? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_SMOKE, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_SMOKE))? MF_ENABLED:MF_DISABLED|MF_GRAYED);
		EnableMenuItem(m_hMenuRule, ID_FIRE, (m_pDataMgr->CheckFeatureByRuleType(VCA5_RULE_TYPE_FIRE))? MF_ENABLED:MF_DISABLED|MF_GRAYED);

		int iAddNodeIdx = NOT_IN_USE;
		if(IsOnZoneEdge(m_nActiveAreaIdx, &PerPoint, &iAddNodeIdx)){
		hShowMenu = m_hSelectedZoneMenu;
		}else{
			hShowMenu = m_hSelectedZoneMenu_WithOutNode;
		}

	} else if (VCA5_APP_AREA_T_COUNTER == m_eActiveAreaType) {
		TrackPopupMenu(m_hSelectedCounterMenu, TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, 0, m_hWnd, NULL);
		return;
	}

	if ( uiDisplayMenuHiliteItem < 255 ){
		POINT finalPt, tmpPt;
		GetCursorPos( &finalPt );
		tmpPt = point;
		tmpPt.x += 30;
		tmpPt.y += 60;
		SetCursorPos( tmpPt.x, tmpPt.y );
		mouse_event(MOUSEEVENTF_LEFTDOWN, tmpPt.x, tmpPt.y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, tmpPt.x, tmpPt.y, 0, 0);

		SetCursorPos( finalPt.x, finalPt.y );
		mouse_event(MOUSEEVENTF_ABSOLUTE, finalPt.x, finalPt.y, 0, 0);
	}

	bool bRet=TrackPopupMenu(hShowMenu, TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, 0, m_hWnd, NULL);*/
}


void CZoneSettingCtl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam);
	switch (wmId){

	case ID_ADDZONE:
		AddZone(TRUE);
		break;
	case ID_ADDLINE:
		AddZone(FALSE);
		break;
	case ID_REMOVEZONE:
		RemoveZone();
		break;
	case ID_REMOVEZONES:
		RemoveZones();
		break;
	case ID_ADDNODE:
		AddNode();
		break;
	case ID_REMOVENODE:
		RemoveNode();
		break;
	case ID_ADDCOUNTER:
		AddCounter();
		break;
	case ID_REMOVECOUNTER:
		RemoveCounter();
		break;
	case ID_RESETCOUNTER:
		ResetCounter();
		break;
	case ID_REMOVECOUNTERS:
		RemoveCounters();
		break;
	case ID_RESETCOUNTERS:
		ResetCounters();
		break;

	case ID_PRESENCE:
	case ID_ENTER:
	case ID_EXIT:
	case ID_APPEAR:
	case ID_DISAPPEAR:
	case ID_STOP:
	case ID_DWELL:
	case ID_DIRECTION:
	case ID_SPEED:
	case ID_TAILGATING:
	case ID_ABANDONREMOVE:
	case ID_COLOR_FILTER:
	case ID_SMOKE:
	case ID_FIRE:
		SetRule((VCA5_RULE_TYPE_E)(wmId-ID_PRESENCE+1));
		break;

		
	}
}


void CZoneSettingCtl::AddZone(BOOL bZone, bool bLineDirectA, bool bFullScreenColor)
{
	if (m_pDataMgr->GetZones()->ulTotalZones >= 3)
	{
		return;
	}
	m_CurPoint.x += 10;
	m_CurPoint.y += 10;
	if (bFullScreenColor)
	{
		m_CurPoint.x = 0;
		m_CurPoint.y = 0;
	}
	POINT point = m_CurPoint;
	PixelToPercent(&point);

	point.x -= DEFAULT_ZONE_WIDTH >> 1;
	point.y -= DEFAULT_ZONE_HEIGHT >> 1;

	point.x = max(point.x, 0);
	point.y = max(point.y, 0);

	if (bZone)
	{
		if (bFullScreenColor)
		{
			m_nActiveAreaIdx = m_pDataMgr->AddFullScrrenZone(&point);

		}else
		{
			m_nActiveAreaIdx = m_pDataMgr->AddZone(&point);
		}

	}
	else
	{
		m_nActiveAreaIdx = m_pDataMgr->AddLine(&point, bLineDirectA);
	}
	m_eActiveAreaType = VCA5_APP_AREA_T_ZONE;

	m_pDataMgr->SetSelectArea(m_nActiveAreaIdx, m_eActiveAreaType);

	m_pDataMgr->FireEvent(VCA_ZONE_ADD, this);

	if (bFullScreenColor)
	{
		VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone(m_nActiveAreaIdx);
		pZone->uiStatus &= ~VCA5_APP_AREA_STATUS_INITIAL_RULE;
		pZone->uiStatus |= VCA5_APP_AREA_STATUS_NORMAL;

		m_pDataMgr->RemoveRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_PRESENCE);
		SetRule(VCA5_RULE_TYPE_COLSIG);

		VCA5_APP_RULE *pRule = m_pDataMgr->GetRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_COLSIG);
		pRule->ucTicked = true;

	}

	if (!bZone)
	{
//		VCA5_APP_RULE *pRule = m_pDataMgr->GetRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_PRESENCE);
	//	pRule->ucTicked = FALSE;
		VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone(m_nActiveAreaIdx);
		pZone->uiStatus &= ~VCA5_APP_AREA_STATUS_INITIAL_RULE;
		pZone->uiStatus |= VCA5_APP_AREA_STATUS_NORMAL;

		

		m_pDataMgr->RemoveRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_PRESENCE);
	
		if (bLineDirectA)
		{
			//SetRule(VCA5_RULE_TYPE_LINECOUNTER_A);
			SetRule(VCA5_RULE_TYPE_DIRECTION);
			VCA5_APP_RULE *pRule = m_pDataMgr->GetRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_DIRECTION);
			VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone(m_nActiveAreaIdx);
			if (pRule)
			{
				pRule->tRuleDataEx.tDirection.sStartAngle = 0;
				pRule->tRuleDataEx.tDirection.sFinishAngle = 1800;
				//pRule->tRuleDataEx.
				pRule->reserved[0] = 1;
			}
		
		}
		else
		{
			//SetRule(VCA5_RULE_TYPE_LINECOUNTER_A);
			SetRule(VCA5_RULE_TYPE_DIRECTION);
			VCA5_APP_RULE *pRule = m_pDataMgr->GetRule(m_nActiveAreaIdx, VCA5_RULE_TYPE_DIRECTION);
			VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone(m_nActiveAreaIdx);
			if (pRule)
			{
				pRule->tRuleDataEx.tDirection.sStartAngle = 0;
				pRule->tRuleDataEx.tDirection.sFinishAngle = 3599;
				pRule->reserved[0] = 2;
			}
			//pZone->uiStatus |= VCA5_APP_AREA_STATUS_NORMAL;
		}
	}
}


void CZoneSettingCtl::RemoveZone()
{
	if(NOT_IN_USE != m_nActiveAreaIdx && VCA5_APP_AREA_T_ZONE == m_eActiveAreaType){
		//first notiy del for imformation deleted... 
		m_pDataMgr->FireEvent(VCA_ZONE_DEL, this);
		m_pDataMgr->RemoveZone(m_nActiveAreaIdx);
		m_pDataMgr->FireEvent(VCA_ZONE_UPDATE, this);
	}
}

void CZoneSettingCtl::AddNode()
{
	if (NOT_IN_USE == m_nActiveAreaIdx || VCA5_APP_AREA_T_ZONE != m_eActiveAreaType) return;

	POINT point = m_CurPoint;
	PixelToPercent(&point);
	int iAddNodeIdx = NOT_IN_USE;
	if(!IsOnZoneEdge(m_nActiveAreaIdx, &point, &iAddNodeIdx)) return;

	VCA5_APP_ZONE*	pZone =	m_pDataMgr->GetZone(m_nActiveAreaIdx);
	if(pZone->ulTotalPoints+2 >= 40){
	//	AfxMessageBox(_T("Can not Create Node, Support node count is 40"));
		return;
	}


	m_pDataMgr->AddNode(m_nActiveAreaIdx, iAddNodeIdx+1, &point);
//	m_pDataMgr->FireEvent(VCA_ZONE_UPDATE, this);
}

void CZoneSettingCtl::RemoveNode()
{
	if (NOT_IN_USE == m_nActiveAreaIdx || VCA5_APP_AREA_T_ZONE != m_eActiveAreaType) return;

	POINT point = m_CurPoint;
	PixelToPercent(&point);

	int iRemoveNodeIdx = FindNodeIndex(m_nActiveAreaIdx, &point);
	if ( iRemoveNodeIdx != NOT_IN_USE){
		m_pDataMgr->RemoveNode( m_nActiveAreaIdx, iRemoveNodeIdx);
		m_nActiveNode = NOT_IN_USE;
		//m_pDataMgr->FireEvent(VCA_ZONE_UPDATE, this);
	}
}

void CZoneSettingCtl::AddCounter()
{
	//1. Add new zone
	POINT point = m_CurPoint;
	PixelToPercent(&point);
	
	m_nActiveAreaIdx = m_pDataMgr->AddCounter(&point);
	m_eActiveAreaType = VCA5_APP_AREA_T_COUNTER;
	m_nActiveNode = NOT_IN_USE;

	m_pDataMgr->SetSelectArea(m_nActiveAreaIdx, m_eActiveAreaType);
	//m_pDataMgr->FireEvent(VCA_COUNTER_UPDATE, this);
}

void CZoneSettingCtl::RemoveCounter()
{
	if(NOT_IN_USE == m_nActiveAreaIdx || VCA5_APP_AREA_T_COUNTER != m_eActiveAreaType) return;

	m_pDataMgr->RemoveCounter(m_nActiveAreaIdx);
//	m_pDataMgr->FireEvent(VCA_COUNTER_UPDATE, this);
}


void CZoneSettingCtl::RemoveCounters()
{
	//1. found counter index related zone index
	VCA5_APP_COUNTERS*	pCounters = m_pDataMgr->GetCounters();
	ULONG ulTotalCounter;
	ulTotalCounter = pCounters->ulTotalCounters;
	for(int i = ulTotalCounter ; i >= 0  ; i--){
		m_pDataMgr->RemoveCounter(i);
	}

	//m_pDataMgr->FireEvent(VCA_COUNTER_UPDATE, this);
}

void CZoneSettingCtl::ResetCounter()
{
	if(NOT_IN_USE == m_nActiveAreaIdx || VCA5_APP_AREA_T_COUNTER != m_eActiveAreaType) return;

	VCA5_APP_COUNTERS*	pCounters = m_pDataMgr->GetCounters();
//	((CVCAEngine *)m_pVCACtrl->GetEngine())->SetControl(VCA5_CMD_SETCOUNTERRES, pCounters->pCounters[m_nActiveAreaIdx].usCounterId, 0);
}

void CZoneSettingCtl::ResetCounters()
{
	/*
	VCA5_APP_COUNTERS*	pCounters = m_pDataMgr->GetCounters();
	for(ULONG i = 0 ; i < pCounters->ulTotalCounters ; i++){
		((CVCAEngine *)m_pVCACtrl->GetEngine())->SetControl(VCA5_CMD_SETCOUNTERRES, pCounters->pCounters[i].usCounterId, 0);
	}*/
}

void CZoneSettingCtl::RemoveZones()
{
	VCA5_APP_ZONES*	pZones = m_pDataMgr->GetZones();
	ULONG ulTotalZone;
	ulTotalZone = pZones->ulTotalZones;
	for(int i = ulTotalZone-1 ; i >= 0 ; i--){
		m_pDataMgr->RemoveZone(i);
	}

	m_pDataMgr->FireEvent(VCA_ZONE_DEL, this);
	m_pDataMgr->FireEvent(VCA_ZONE_UPDATE, this);	
}


void CZoneSettingCtl::SetRule(VCA5_RULE_TYPE_E eRuleType)
{
	if (VCA5_APP_AREA_T_ZONE != m_eActiveAreaType) return;

	if(m_pDataMgr->GetRuleStatus(m_nActiveAreaIdx, eRuleType)){
		m_pDataMgr->RemoveRule(m_nActiveAreaIdx, eRuleType);
		m_pDataMgr->FireEvent(VCA_RULE_UPDATE, this);
		return;
	}
	
	m_pDataMgr->SetRule(m_nActiveAreaIdx, eRuleType);
	VCA5_APP_RULE	*pRule = m_pDataMgr->GetRule(m_nActiveAreaIdx, eRuleType);
	pRule->ucTicked = TRUE;


	m_pDataMgr->FireEvent(VCA_RULE_UPDATE, this);
}


void CZoneSettingCtl::FireOnEvent(DWORD uiEventType, DWORD uiContext)
{
	
	if ( IVCAConfigureObserver::IsZoneEvent(uiEventType) || IVCAConfigureObserver::IsCounterEvent(uiEventType) ) 
	{
		m_pDataMgr->FindSelectedAreaIdx(&m_nActiveAreaIdx, &m_eActiveAreaType);
		m_nActiveNode	= NOT_IN_USE;
	}
}

void CZoneSettingCtl::PixelToPercent(POINT *pPoint)
{
	PIXELTOPERCENT(pPoint->x, pPoint->x, m_ClientRect.right);
	PIXELTOPERCENT(pPoint->y, pPoint->y, m_ClientRect.bottom);
}

void CZoneSettingCtl::PercentToPixel(POINT *pPoint)
{
	PERCENTTOPIXEL(pPoint->x, pPoint->x, m_ClientRect.right);
	PERCENTTOPIXEL(pPoint->y, pPoint->y, m_ClientRect.bottom);
}

BOOL IsPointInLine(POINT &Start, POINT &End,  POINT &pt)
{
	POINT delta; 

	if(End.x > Start.x){
		End.x	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > Start.x)? Start.x	= 0 :  (Start.x -= MAX_DIFF>>1);
	}else{
		Start.x	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > End.x)? End.x	= 0 :  (End.x -= MAX_DIFF>>1);
	}

	if(End.y > Start.y){
		End.y	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > Start.y)? Start.y	= 0 : (Start.y -= MAX_DIFF>>1);
	}else{
		Start.y	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > End.y)? End.y	= 0 : (End.y -= MAX_DIFF>>1);
	}


	delta.x = End.x - Start.x;
	delta.y = End.y - Start.y;

	double angle = atan2( (double) -delta.y, (double) delta.x );
	double dy = MAX_DIFF * cos(angle);
	double dx = MAX_DIFF * sin(angle);
	
	HRGN 	hRgn;
	POINT	p[5] = {{(int) (Start.x	+ dx), (int) (Start.y + dy)}, 
					{(int) (Start.x	- dx), (int) (Start.y - dy)},
					{(int) (End.x	- dx), (int) (End.y	  - dy)},
					{(int) (End.x	+ dx), (int) (End.y   + dy)},
					{(int) (Start.x	+ dx),(int) (Start.y  + dy)}};
	
	hRgn = CreatePolygonRgn( p, 5, ALTERNATE);
	if (PtInRegion(hRgn, pt.x, pt.y)){
		DeleteObject(hRgn);
		return TRUE;
	}
	
	DeleteObject(hRgn);
	return FALSE;
}


BOOL IsPointInLine(POINT Start, POINT End,  POINT pt, float fWidth)
{
	POINT delta; 
/*
	if(End.x > Start.x){
		End.x	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > Start.x)? Start.x	= 0 :  (Start.x -= MAX_DIFF>>1);
	}else{
		Start.x	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > End.x)? End.x	= 0 :  (End.x -= MAX_DIFF>>1);
	}

	if(End.y > Start.y){
		End.y	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > Start.y)? Start.y	= 0 : (Start.y -= MAX_DIFF>>1);
	}else{
		Start.y	+= MAX_DIFF>>1; 
		((MAX_DIFF>>1) > End.y)? End.y	= 0 : (End.y -= MAX_DIFF>>1);
	}
*/
	delta.x = End.x - Start.x;
	delta.y = End.y - Start.y;

	double angle = atan2( (double) -delta.y, (double) delta.x );
	double dy = fWidth * cos(angle);
	double dx = fWidth * sin(angle);

	
	HRGN 	hRgn;
	POINT	p[5] = {{(int) (Start.x	+ dx), (int) (Start.y + dy)}, 
					{(int) (Start.x	- dx), (int) (Start.y - dy)},
					{(int) (End.x	- dx), (int) (End.y	  - dy)},
					{(int) (End.x	+ dx), (int) (End.y   + dy)},
					{(int) (Start.x	+ dx),(int) (Start.y  + dy)}};
	hRgn = CreatePolygonRgn( p, 5, ALTERNATE);

	if (PtInRegion(hRgn, pt.x, pt.y)){
		DeleteObject(hRgn);
		return TRUE;
	}
	
	DeleteObject(hRgn);
	return FALSE;

}



BOOL CZoneSettingCtl::PtInZone(VCA5_APP_ZONE *pZone, POINT *pPoint)
{
	HRGN hRGN;
	POINT Points[VCA5_MAX_NUM_ZONE_POINTS];
	ULONG i;
	
	if (VCA5_ZONE_STYLE_TRIPWIRE & pZone->usZoneStyle) {
		POINT Start, End;
		for (i = 0; i < pZone->ulTotalPoints-1 ; i++){
			Start.x = pZone->pPoints[i].x; Start.y = pZone->pPoints[i].y;
			End.x	= pZone->pPoints[i+1].x; End.y = pZone->pPoints[i+1].y;
			if (IsPointInLine( Start, End, *pPoint))
				return TRUE;
		}
		return FALSE;
	} else {
		for (i = 0; i < pZone->ulTotalPoints; ++i){
			Points[i].x = pZone->pPoints[i].x; 
			Points[i].y = pZone->pPoints[i].y;
		}

		hRGN = CreatePolygonRgn(Points, pZone->ulTotalPoints, ALTERNATE);
		if (PtInRegion(hRGN, pPoint->x, pPoint->y)) {
			DeleteObject(hRGN);
			return TRUE;
		}
		OffsetRgn(hRGN, MAX_X_DIFF, 0 );
		if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
			DeleteObject(hRGN);
			return TRUE;
		}
		OffsetRgn(hRGN, -MAX_X_DIFF, 0 );
	
		OffsetRgn(hRGN, -MAX_X_DIFF, 0  );
		if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
			DeleteObject(hRGN);
			return TRUE;
		}
		OffsetRgn(hRGN, MAX_X_DIFF, 0);

		OffsetRgn(hRGN, 0, MAX_Y_DIFF );
		if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
			DeleteObject(hRGN);
			return TRUE;
		}
		OffsetRgn(hRGN, 0, -MAX_Y_DIFF );

		OffsetRgn(hRGN, 0, -MAX_Y_DIFF );
		if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
			DeleteObject(hRGN);
			return TRUE;
		}

		DeleteObject(hRGN);
	}
	return FALSE;
}

BOOL CZoneSettingCtl::PtInCounter(VCA5_APP_COUNTER *pCounter, POINT *pPoint)
{
	HRGN hRGN;
	POINT Points[5];
	ULONG i;

	for (i = 0; i < 5; ++i){
		Points[i].x = pCounter->pPoints[i].x; 
		Points[i].y = pCounter->pPoints[i].y;
	}

	hRGN = CreatePolygonRgn(Points, 5, ALTERNATE);
	if (PtInRegion(hRGN, pPoint->x, pPoint->y)) {
		DeleteObject(hRGN);
		return TRUE;
	}
	OffsetRgn(hRGN, MAX_X_DIFF, MAX_Y_DIFF );
	if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
		DeleteObject(hRGN);
		return TRUE;
	}
	OffsetRgn(hRGN, -MAX_X_DIFF, -MAX_Y_DIFF );

	OffsetRgn(hRGN, -MAX_X_DIFF, -MAX_Y_DIFF  );
	if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
		DeleteObject(hRGN);
		return TRUE;
	}
	OffsetRgn(hRGN, MAX_X_DIFF, MAX_Y_DIFF);

	OffsetRgn(hRGN, -MAX_X_DIFF, MAX_Y_DIFF );
	if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
		DeleteObject(hRGN);
		return TRUE;
	}
	OffsetRgn(hRGN, MAX_X_DIFF, -MAX_Y_DIFF );

	OffsetRgn(hRGN, MAX_X_DIFF, -MAX_Y_DIFF );
	if (PtInRegion(hRGN, pPoint->x, pPoint->y)){
		DeleteObject(hRGN);
		return TRUE;
	}

	DeleteObject(hRGN);
	return FALSE;
}

int		CZoneSettingCtl::FindNodeIndex(int ZoneIdx, POINT *pPoint)
{
	VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone(ZoneIdx); 
	if(!pZone) return NOT_IN_USE;

	double	mindistance = (double) NORMALISATOR * NORMALISATOR;
	double	distance;
	LONG	x, y;
	int		iActiveNodeIdx = NOT_IN_USE;
	VCA5_POINT *pNode; 
	
	x = pPoint->x;
	y = pPoint->y;
	
	for (ULONG  i = 0; i < pZone->ulTotalPoints ; ++i){
		pNode = &pZone->pPoints[i];
			
		if ((abs(pNode->x - x)<MAX_X_DIFF) && (abs(pNode->y - y)<MAX_Y_DIFF)){
			distance = (pNode->x - x)*(pNode->x - x) + (pNode->y - y)*(pNode->y - y);
			if ( distance< mindistance ){
				mindistance		= distance;
				iActiveNodeIdx	= i;
			}
		}
	}
	return iActiveNodeIdx;
}

#define CALDISTANCE( p1, p2 ) sqrt( 1.0f*(p1.x-p2.x)*(p1.x-p2.x) + 1.0f*(p1.y-p2.y)*(p1.y-p2.y) )
static double CalPtToAreaDistance( POINT point, VCA5_POINT* pPoints, ULONG totalPoint )
{
	double lDistance = 0;
	for ( unsigned int i = 0; i < totalPoint - 1; i ++ )
	{	
		lDistance += CALDISTANCE( point, pPoints[i] ); 		
	}
	return lDistance;

}

int		CZoneSettingCtl::FindAreaIndex(int *piAreaIdx, VCA5_APP_AREA_T_E *pType ,POINT* point)
{
	ULONG	i;

	VCA5_APP_AREA_T_E Type = VCA5_APP_AREA_T_NONE;
	int		minZoneIdx = NOT_IN_USE;

	int		iZoneIdx[VCA5_MAX_NUM_ZONES];
	VCA5_APP_ZONES* pZones = m_pDataMgr->GetZones();
	ULONG	iNumInsideZones = 0;
	for (i = 0; i < pZones->ulTotalZones; i++) {
		if (PtInZone(&pZones->pZones[i], point)) {
			iZoneIdx[iNumInsideZones++] = i;
		}
	}

	int		minCounterIdx = NOT_IN_USE;
	int		iCounterIdx[VCA5_MAX_NUM_COUNTERS];
	VCA5_APP_COUNTERS* pCounters = m_pDataMgr->GetCounters();
	ULONG	iNumInsideCounters = 0;
	for (i = 0; i < pCounters->ulTotalCounters ; i++) {
		if (PtInCounter(&pCounters->pCounters[i], point)) {
			iCounterIdx[iNumInsideCounters++] = i;
		}
	}

	if ((iNumInsideZones+iNumInsideCounters) > 0){
		double fDistance;
		double minDistace = 1e32;
		for (i = 0; i <  iNumInsideZones; i++){
			fDistance = CalPtToAreaDistance(*point, pZones->pZones[iZoneIdx[i]].pPoints, pZones->pZones[iZoneIdx[i]].ulTotalPoints);
			if ( fDistance < minDistace ){
				minZoneIdx	= iZoneIdx[i];
				minDistace	= fDistance;
				Type		= VCA5_APP_AREA_T_ZONE;
			}
		}

		for (i = 0; i <  iNumInsideCounters; i++){
			fDistance = CalPtToAreaDistance(*point, pCounters->pCounters[iCounterIdx[i]].pPoints, 5);
			if ( fDistance < minDistace ){
				minCounterIdx = iCounterIdx[i];
				minDistace	= fDistance;
				Type		= VCA5_APP_AREA_T_COUNTER;
			}
		}

		if(VCA5_APP_AREA_T_ZONE == Type){
			*piAreaIdx	= minZoneIdx;
			*pType		= VCA5_APP_AREA_T_ZONE;
		}else{
			*piAreaIdx	= minCounterIdx;
			*pType		= VCA5_APP_AREA_T_COUNTER;
		}
		
		return *piAreaIdx;
	}

/*
	
	if (iNumInsideCounters > 0){
		double fDistance;
		double minDistace = 1e32;
		for (i = 0; i <  iNumInsideCounters; i++){
			fDistance = CalPtToAreaDistance(*point, pCounters->pCounters[iCounterIdx[i]].pPoints, 5);
			if ( fDistance < minDistace ){
				minCounterIdx = iCounterIdx[i];
				minDistace	= fDistance;
			}
		}
		*piAreaIdx	= minCounterIdx;
		*pType		= VCA5_APP_AREA_T_COUNTER;
		return minZoneIdx;
	}
*/
	*piAreaIdx	= NOT_IN_USE;
	*pType		= VCA5_APP_AREA_T_NONE;
	return NOT_IN_USE;
}

BOOL CZoneSettingCtl::IsOnZoneEdge( int iZoneIdx, POINT* point, int* pointIdx )
{
	unsigned int	i, numpoints;
	long			x, y, min_x, min_y;
	long			maxx, maxy, minx,miny, min_point, min_distance = 10000;
	float			min_diff = MAX_DIFF;
	VCA5_POINT		*x1,*x2;
	VCA5_APP_ZONE	*pZone;

	if(NOT_IN_USE == iZoneIdx) return FALSE;
	pZone = m_pDataMgr->GetZone( iZoneIdx );

	x = point->x; y = point->y;
	numpoints = pZone->ulTotalPoints-1;

	for ( i = 0; i < numpoints ; i++){
		x1 = &pZone->pPoints[i];
		x2 = &pZone->pPoints[i+1];

		maxx = max(x1->x, x2->x);
		minx = min(x1->x, x2->x);
		maxy = max(x1->y, x2->y);
		miny = min(x1->y, x2->y);

						
		float d1 = (float) sqrt( (float) 1.0f * ( 1.0f * (x2->x - x1->x) * (x2->x - x1->x) + 1.0f * (x2->y - x1->y) * (x2->y - x1->y) ) );
		float d2 = (float) sqrt( (float) 1.0f * ( 1.0f * (x - x2->x) * (x - x2->x) + 1.0f * (y - x2->y) * (y - x2->y) ) );
		float d3 = (float) sqrt( (float) 1.0f * ( 1.0f * (x - x1->x) * (x - x1->x) + 1.0f * (y - x1->y) * (y - x1->y) ) );
		float delta = d2 + d3 -d1;

		if (delta < min_diff)
		{
			min_diff = delta;
			min_point = i;
			min_x = x;
			min_y = y;
		}
	}

	if (min_diff < MAX_DIFF){
		*pointIdx = min_point;
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CZoneSettingCtl::PtInArrow( POINT	*x1, POINT	*x2, POINT cpPoint, int xdiff, int ydiff, int distdiff )
{
	long	x, y ;
	long	maxx, maxy, minx,miny, min_distance = 10000;

	x = cpPoint.x;
	y = cpPoint.y;

	maxx = max(x1->x, x2->x);
	minx = min(x1->x, x2->x);
	maxy = max(x1->y, x2->y);
	miny = min(x1->y, x2->y);

	
	float d1 = (float) sqrt( (float) ( 1.0f * (x2->x - x1->x) * (x2->x - x1->x) + 1.0f * (x2->y - x1->y) * (x2->y - x1->y)) );
	float d2 = (float) sqrt( (float) ( 1.0f * (x - x2->x) * (x - x2->x) + 1.0f * (y - x2->y) * (y - x2->y)) );
	float d3 = (float) sqrt( (float) ( 1.0f * (x - x1->x) * (x - x1->x) + 1.0f * (y - x1->y) * (y - x1->y)) );
	float delta = d2 + d3 -d1;

	if (delta < distdiff)
		return true;
	else
		return false;
}

void CZoneSettingCtl::GetZonePie( VCA5_APP_ZONE *pZone, VCA5_POINT *pt )
{
	int x=0, y=0;

	m_pDataMgr->GetZoneCenter( pZone, pt );
	POINT cpPoint;
	cpPoint.x = pt->x;
	cpPoint.y = pt->y;
	PercentToPixel( &cpPoint );
	pt->x = (USHORT)cpPoint.x;
	pt->y = (USHORT)cpPoint.y;
}

BOOL CZoneSettingCtl::IsInsidePie( VCA5_APP_ZONE *pZone, POINT point )
{
	VCA5_POINT	pt;
/*	CRgn		tmpRgn;
	POINT		cpPoint[4];

	GetZonePie( pZone, &pt );
	cpPoint[0].x = pt.x; 	cpPoint[0].y = pt.y; 
	cpPoint[1] = m_pDataMgr->m_cArrowBottom.m_cpEnd;
	cpPoint[2].x = m_pDataMgr->m_cArrowMiddle.m_pPoints[4].x; 	cpPoint[2].y = m_pDataMgr->m_cArrowMiddle.m_pPoints[4].y;
	cpPoint[3] = m_pDataMgr->m_cArrowTop.m_cpEnd;
	
	tmpRgn.CreatePolygonRgn( cpPoint, 4, ALTERNATE);

	if (tmpRgn.PtInRegion( point))
	{
		tmpRgn.DeleteObject();
		return TRUE;	
	}
	else
	{
		tmpRgn.DeleteObject();
		return FALSE;	
	}*/
	return false;
}

BOOL CZoneSettingCtl::CheckLineCounterSetting(int nZone, POINT point)
{
	if ( NOT_IN_USE ==  nZone ) return FALSE;	

	PercentToPixel( &point );

	// Check directional settings
	VCA5_APP_RULE	*pRuleA, *pRuleB, *pRule;

	pRuleA	= m_pDataMgr->GetRule(nZone, VCA5_RULE_TYPE_LINECOUNTER_A);
	pRuleB	= m_pDataMgr->GetRule(nZone, VCA5_RULE_TYPE_LINECOUNTER_B);
	if ( (  pRuleA && pRuleA->ucTicked ) ||  (  pRuleB && pRuleB->ucTicked )){
		VCA5_APP_ZONE	*pZone = m_pDataMgr->GetZone( nZone );
		VCA5_RULE_LINECOUNTER *pLC;
		
		if(pRuleA && pRuleA->ucTicked){
			pRule	= pRuleA;
		}
		else{
			pRule	= pRuleB;
		}
		pLC	= &(pRule->tRuleDataEx.tLineCounter);

		if (pRule->ucWidthCalibrationEnabled == 0) return FALSE;
					
		FLT_POINT pt1, pt2, ptMiddle, ptMiddle2 ;
		POINT ptBottom, ptTop;
			
		ULONG	ulClientWidth	= m_ClientRect.right - m_ClientRect.left;
		ULONG	ulClientHeight	= m_ClientRect.bottom - m_ClientRect.top;

		for (UINT i = 0; i < pZone->ulTotalPoints-1; i++)
		{
			PERCENTTOPIXEL( pt1.x, pZone->pPoints[i].x, ulClientWidth );
			PERCENTTOPIXEL( pt1.y, pZone->pPoints[i].y, ulClientHeight );
			PERCENTTOPIXEL( pt2.x, pZone->pPoints[i+1].x, ulClientWidth );
			PERCENTTOPIXEL( pt2.y, pZone->pPoints[i+1].y, ulClientHeight );

			double dAngle  = atan2(pt1.x - pt2.x, pt1.y - pt2.y);
			double dCalibrationWidth = (double)pLC->ulCalibrationWidth;
				
			PERCENTTOPIXEL(dCalibrationWidth, pLC->ulCalibrationWidth, ulClientWidth);

			ptMiddle.x = (pt1.x + pt2.x)/2;
			ptMiddle.y = (pt1.y + pt2.y)/2;

			for (int n = 0; n < 2; n++)
			{
				ptMiddle2.x = ptMiddle.x + (n*dCalibrationWidth - dCalibrationWidth/2) * sin( dAngle );
				ptMiddle2.y = ptMiddle.y + (n*dCalibrationWidth - dCalibrationWidth/2) * cos( dAngle );
				/*
				ptTop.x = (ULONG)round((double)ptMiddle2.x + double)25 * cos(dAngle));
				ptTop.y = (ULONG)round(double)ptMiddle2.y - double)25 * sin(dAngle));
				//ptTop.y = (ULONG)round(ptMiddle2.y + 25  * sin( dAngle ));

				ptBottom.x = (ULONG)round(double)ptMiddle2.x - double)25 * cos(dAngle));
				ptBottom.y = (ULONG)round(double)ptMiddle2.y + double)25 * sin(dAngle));*/
				//ptBottom.y = (ULONG)round(ptMiddle2.y - 25  * sin( dAngle ));

				if(IsPointInLine(  ptTop, ptBottom, point, 5.0f )){
					m_iLineCounterLeftRight = n;
					m_iLineCounterSegment = i; 
			//		m_ptLineCounterStart = cpPoint;
					return TRUE;

				}
			}
		}
	}

	return FALSE;
}


BOOL CZoneSettingCtl::CheckDirectionSetting(int nZone, POINT point )
{
	if ( NOT_IN_USE ==  nZone ) return FALSE;

	PercentToPixel( &point );
	// Check directional settings
	VCA5_APP_RULE	*pRule = m_pDataMgr->GetRule(nZone, VCA5_RULE_TYPE_DIRECTION);
	if  (  pRule && pRule->ucTicked ){
		if ( PtInArrow( &(m_pDataMgr->m_cArrowBottom.m_cpStart), &(m_pDataMgr->m_cArrowBottom.GetPolygonPoint( 4 )), point, 1, 1, 1) ){
			m_cpStartDirectionPt = point;
			m_eDirectionStatus = BOTTOM_ARROW;	// 1 for bottom
			m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
			return TRUE;
		}else if ( PtInArrow( &(m_pDataMgr->m_cArrowTop.m_cpStart), &(m_pDataMgr->m_cArrowTop.GetPolygonPoint( 4 )),point, 1, 1, 1) ){
			m_cpStartDirectionPt = point;
			m_eDirectionStatus = TOP_ARROW;	// 0 for top
			m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
			return TRUE;
		}else {
			VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone( nZone );
			if (pZone && IsInsidePie( pZone, point )){
				m_cpStartDirectionPt = point;
				m_eDirectionStatus = WHOLE_DIRECTION;
				m_pDataMgr->FireEvent(IVCAConfigureObserver::VCA_RULE_UPDATE, this);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CZoneSettingCtl::ChangeLineCounter( int iSelectedZoneIdx, POINT point  )
{
	PercentToPixel( &point );

	VCA5_APP_RULE	*pRuleA, *pRuleB, *pRule;

	pRuleA = m_pDataMgr->GetRule( iSelectedZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_A );
	pRuleB = m_pDataMgr->GetRule( iSelectedZoneIdx, VCA5_RULE_TYPE_LINECOUNTER_B );
	if ((pRuleA && pRuleA->ucTicked) || (pRuleB && pRuleB->ucTicked)){
		VCA5_APP_ZONE*	pZone = m_pDataMgr->GetZone( iSelectedZoneIdx );

		VCA5_RULE_LINECOUNTER *pLC;
		if(pRuleA && pRuleA->ucTicked){
			pRule	= pRuleA;
		}
		else{
			pRule	= pRuleB;
		}
		pLC		= &(pRule->tRuleDataEx.tLineCounter);

		FLT_POINT pt1, pt2, ptMiddle;
	//		POINT ptBottom, ptTop;
		ULONG	ulClientWidth	= m_ClientRect.right - m_ClientRect.left;
		ULONG	ulClientHeight	= m_ClientRect.bottom - m_ClientRect.top;

		PERCENTTOPIXEL( pt1.x, pZone->pPoints[m_iLineCounterSegment].x, ulClientWidth);
		PERCENTTOPIXEL( pt1.y, pZone->pPoints[m_iLineCounterSegment].y, ulClientHeight);
		PERCENTTOPIXEL( pt2.x, pZone->pPoints[m_iLineCounterSegment+1].x, ulClientWidth);
		PERCENTTOPIXEL( pt2.y, pZone->pPoints[m_iLineCounterSegment+1].y, ulClientHeight);

		ptMiddle.x = (pt1.x + pt2.x)/2;
		ptMiddle.y = (pt1.y + pt2.y)/2;

				
		float m1 = (float)((pt1.y - pt2.y)/(pt1.x - pt2.x + 0.001f)); 
		float c1 = (float)(pt1.y - m1*pt1.x);

		float m2 = (float)tan(atan2((pt1.y - pt2.y), (pt1.x - pt2.x)) + PI/2);
		float c2 = (float)point.y - m2*(float)point.x;

		FLT_POINT intercept;
		intercept.x = (c2 - c1)/(m1 - m2);
		intercept.y = m1*intercept.x  + c1;

		
		float calibWidth = (float)sqrt((intercept.x -  ptMiddle.x)*(intercept.x -  ptMiddle.x) + (intercept.y -  ptMiddle.y)*(intercept.y -  ptMiddle.y));
		float dist2End = (float)sqrt((intercept.x -  pt1.x)*(intercept.x -  pt1.x) + (intercept.y -  pt1.y)*(intercept.y -  pt1.y));
		float dist2Start = (float)sqrt((intercept.x -  pt2.x)*(intercept.x -  pt2.x) + (intercept.y -  pt2.y)*(intercept.y -  pt2.y));
		float lineLength = (float)sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));

		if ((m_iLineCounterLeftRight && (dist2End > dist2Start)) || (!m_iLineCounterLeftRight && (dist2End < dist2Start)))  calibWidth = 0;

			
		calibWidth = min(calibWidth, lineLength/2);

		PIXELTOPERCENT(calibWidth, 2*calibWidth, ulClientWidth); 

		m_pDataMgr->SetLineCounterA( iSelectedZoneIdx, FALSE, max(1,  (int)calibWidth) );
		m_pDataMgr->SetLineCounterB( iSelectedZoneIdx, FALSE, max(1, (int)calibWidth) );
	}
}


void CZoneSettingCtl::ChangeDirectionArrows( POINT point )
{
	if (VCA5_APP_AREA_T_ZONE != m_eActiveAreaType) return;

	VCA5_POINT pt;
	CString s;
	VCA5_RULE	*pRule = m_pDataMgr->GetRule( m_nActiveAreaIdx, VCA5_RULE_TYPE_DIRECTION );
	
	PercentToPixel( &point );
	GetZonePie( m_pDataMgr->GetZone(m_nActiveAreaIdx) , &pt );	// ÀÇ½É1
	int dx1 = m_cpStartDirectionPt.x - pt.x;
	int dy1 = - (m_cpStartDirectionPt.y - pt.y);
	int dx2 = point.x - pt.x;
	int dy2 = -(point.y - pt.y);
	double angle1 = atan2( (double)dy1, (double)dx1) / PI * 180 ;
	double angle2 = atan2( (double)dy2, (double)dx2) / PI * 180 ;
	
	int angle = (int)((angle2 -angle1) * 10);
	int startangle;
	int finishangle;
	startangle = pRule->tRuleDataEx.tDirection.sStartAngle;
	finishangle = pRule->tRuleDataEx.tDirection.sFinishAngle;

//	s.Format("\n Start=%d, Finish=%d, Diff=%d \n", startangle, finishangle, angle );
//	OutputDebugString(s);

	if ( angle > 1800 )
		angle -= 3600;
	if ( angle < -1800 )
		angle += 3600;

	switch ( m_eDirectionStatus )
	{
		case WHOLE_DIRECTION:
			startangle += angle;
			finishangle += angle;

			break;
		case TOP_ARROW:
			finishangle += angle;
			startangle  -= angle;
			break;

		case BOTTOM_ARROW:
			startangle += angle;
			finishangle -= angle;
			break;
	}
	if ( ValidAngle( &startangle, &finishangle ) )
	{
		m_pDataMgr->SetDirection( m_nActiveAreaIdx, FALSE,  startangle, finishangle);
	}
	m_cpStartDirectionPt = point;
}

BOOL CZoneSettingCtl::ValidAngle( int *sangle, int *fangle ) 
{
	int startangle = *sangle;
	int finishangle = *fangle;

	/* 
	BC:: finishangle-startangle must larger than 1 degree, which means acceptance angle
	needs to be larger than 1 degree, and smaller than 360 degree 
	*/
	if ( ( finishangle >= startangle + 10 ) && ( finishangle - startangle < 3600 ) )
	{
		if (startangle < 0)
		{
			startangle += 3600;
			finishangle += 3600;
		}

		if (startangle > 3600)
		{
			startangle -= 3600;
			finishangle -= 3600;
		} 
		*sangle = startangle;
		*fangle = finishangle;
		return TRUE;
	}
	else
		return FALSE;

}

BOOL CZoneSettingCtl::ValidateZone( int ZoneIdx )
{
	VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone( ZoneIdx );
	Vector2dVector polygonpt, triangleresult;
	int xprecision, yprecision;
	ULONG i, j;

	RECT rcClient;

	if( pZone->usZoneStyle & VCA5_ZONE_STYLE_TRIPWIRE )
	{
		// Not validated
		return TRUE;
	}

	if  ( ( m_ClientRect.right > 0 ) && ( m_ClientRect.bottom > 0 ) )
	{
		xprecision = NORMALISATOR / m_ClientRect.right + 1;
		yprecision = NORMALISATOR / m_ClientRect.bottom + 1;
	}
	else
	{
		xprecision = 1;
		yprecision = 1;
	}

	for ( i = 0; i < pZone->ulTotalPoints - 1; i++ )
	{
		for ( j = i+1; j < pZone->ulTotalPoints - 1; j++ )
		{
			if ( ( abs( pZone->pPoints[i].x - pZone->pPoints[j].x ) < xprecision ) && ( abs( pZone->pPoints[i].y - pZone->pPoints[j].y ) < yprecision ) )
				return FALSE;
		}
	}
	
	for (  i = 0; i < pZone->ulTotalPoints - 1; i++ )
		polygonpt.push_back( Vector2d( pZone->pPoints[i].x,  pZone->pPoints[i].y ) );

	return Triangulate::Process( polygonpt, triangleresult );

}

void CZoneSettingCtl::SetAreaMouseClip( int iAreaIdx, HWND hWnd )
{
	RECT  rcClient;
	if ( !m_bChangingDirection  && !m_bChangingLineCounter )
	{
		VCA5_APP_ZONE *pZone = m_pDataMgr->GetZone( iAreaIdx );
		if ( pZone )
		{
			GetMouseClipFromArea( hWnd, pZone->ulTotalPoints, pZone->pPoints, m_CurCapPoint, &rcClient);
			// Clipcursor
			ClipCursor(&rcClient); 
		}
	}
}

void CZoneSettingCtl::GetMouseClipFromArea( HWND hWnd, unsigned int uiTotalPoints, VCA5_POINT pPoints[], POINT cpPoint, RECT *rcClient)
{
	POINT ptClientUL, ptClientLR;

	SetRect(rcClient,NORMALISATOR, NORMALISATOR, 0, 0);

	for (unsigned int i = 0; i < uiTotalPoints ; i++)
	{
		if ( pPoints[i].x < rcClient->left )
			rcClient->left = pPoints[i].x;
		if ( pPoints[i].x > rcClient->right )
			rcClient->right = pPoints[i].x;
		if ( pPoints[i].y < rcClient->top )
			rcClient->top = pPoints[i].y;
		if ( pPoints[i].y > rcClient->bottom)
			rcClient->bottom = pPoints[i].y;
	}

	rcClient->top = cpPoint.y - rcClient->top;
	rcClient->bottom = NORMALISATOR - (rcClient->bottom - cpPoint.y);
	rcClient->left = cpPoint.x - rcClient->left;
	rcClient->right = NORMALISATOR - (rcClient->right - cpPoint.x);

	PERCENTTOPIXEL( ptClientUL.x, rcClient->left, m_ClientRect.right ); 	
	PERCENTTOPIXEL( ptClientLR.x, rcClient->right, m_ClientRect.right ); 
	PERCENTTOPIXEL( ptClientUL.y, rcClient->top, m_ClientRect.bottom ); 	
	PERCENTTOPIXEL( ptClientLR.y, rcClient->bottom, m_ClientRect.bottom ); 		

	ptClientUL.x++;
	ptClientLR.x++;
	ptClientUL.y++;
	ptClientLR.y++;
	ClientToScreen( hWnd, &ptClientUL); 
	ClientToScreen( hWnd, &ptClientLR); 
	SetRect( rcClient, ptClientUL.x+1, ptClientUL.y+1, ptClientLR.x-1 , ptClientLR.y-1 ); 

}