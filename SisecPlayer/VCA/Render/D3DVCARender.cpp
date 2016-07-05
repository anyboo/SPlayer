#include "stdafx.h"
#include "D3DVCARender.h"
#include "VCADataMgr.h"
//#include "math.h"
#include "EasyArrowD3D.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

*/

#pragma comment(lib,"./VCA/Render/VCAMetaRender.lib")

CVCAD3DRender::CVCAD3DRender(void)
{
	m_bSetup	= FALSE;
	m_hWnd		= NULL;
	m_pD3d		= NULL;
	m_pDataMgr	= NULL;
	m_hVMR		= NULL;
	m_bDraw		= FALSE;
	m_RenderMode= RENDER_VCA;
	m_bZonesVisible = true;

	m_tamperAlarmFlash = 0;

	//m_eColorFormat = CD3d::CF_YV12;
	m_eColorFormat = CD3d::CF_IYUV;

	// Set some default colors
	m_crRender[VCA_COLOR_ALARM]			= COLOR_ALARM;
	m_crRender[VCA_COLOR_NONALARM]		= COLOR_NONALARM;
	m_crRender[VCA_COLOR_BLOB]			= COLOR_BLOB;
	m_crRender[VCA_COLOR_STATBLOB]		= COLOR_STATBLOB;
	m_crRender[VCA_COLOR_SMOKEBLOB]		= COLOR_SMOKEBLOB;
	m_crRender[VCA_COLOR_FIREBLOB]		= COLOR_FIREBLOB;
	m_crRender[VCA_COLOR_SCENECHANGE]	= COLOR_SCENECHANGE;
}

CVCAD3DRender::~CVCAD3DRender(void)
{
	Endup();
}


BOOL	CVCAD3DRender::SetupVMR()
{
	VMR_ERROR_E eError = VMR_InitVCAMetaRender( &m_hVMR );
	if ( eError != VMRERR_SUCCESS ){
		MessageBox( NULL, _T("FAIL to init VMR!"), _T("ERROR"), MB_ICONERROR | MB_OK );
		return FALSE;
	}

	VMR_SetMode( m_hVMR,  VMR_ZNCTCFG);
	VMR_SetRenderingFlags( m_hVMR, VRF_ZONES|VRF_OBJECTS|VRF_NON_ALARMS);

	return TRUE;
}



BOOL	CVCAD3DRender::Setup(HWND hWnd, CVCADataMgr *pDataMgr)
{
	if(m_bSetup){
		TRACE("CVCAD3DRender setup before\n");
		return TRUE;
	}

	m_pD3d = new CD3d();
	if(m_pD3d->Setup(0) != S_OK){
		MessageBox(NULL, _T("Can not setup D3D "),_T("ERROR"),MB_OK);
		delete m_pD3d;
		m_pD3d = NULL;
		return FALSE;
	}
	m_pD3d->SetDDClipperWindow(hWnd);
	
	m_pDataMgr	= pDataMgr;
	m_hWnd		= hWnd;
	
	
	m_pDataMgr->m_cArrowMiddle.SetArrowWidth( (int)ARROW_WIDTH );
	m_pDataMgr->m_cArrowTop.SetArrowWidth((int)(ARROW_WIDTH));
	m_pDataMgr->m_cArrowBottom.SetArrowWidth((int)(ARROW_WIDTH));
//	m_pDataMgr->m_cArrowTop.SetArrowWidth( (int)(ARROW_WIDTH*0.8) );
//	m_pDataMgr->m_cArrowBottom.SetArrowWidth( (int)(ARROW_WIDTH*0.8) );

	m_pDataMgr->m_cArrowMiddle22.SetArrowWidth((int)ARROW_WIDTH);
	m_pDataMgr->m_cArrowTop22.SetArrowWidth((int)(ARROW_WIDTH));
	m_pDataMgr->m_cArrowBottom22.SetArrowWidth((int)(ARROW_WIDTH));


	m_pDataMgr->m_cArrowMiddle33.SetArrowWidth((int)ARROW_WIDTH);
	m_pDataMgr->m_cArrowTop33.SetArrowWidth((int)(ARROW_WIDTH));
	m_pDataMgr->m_cArrowBottom33.SetArrowWidth((int)(ARROW_WIDTH));

	m_bSetup	= SetupVMR();

	return m_bSetup;
}


void	CVCAD3DRender::Endup()
{
	if(m_bSetup){
		if(m_pD3d){
			delete m_pD3d;
			m_pD3d = NULL;
		}
		m_hWnd		= NULL;
		m_pDataMgr	= NULL;
		m_bSetup	= FALSE;
	}
}

void	CVCAD3DRender::OnChangeClientRect(RECT rect)
{
	RECT ClientRect(rect);
	
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	if (width!=0 && 0 != height!=0) {
		m_ClientRect = rect;
		m_bDraw = TRUE;
	} else {
		m_bDraw = FALSE;
	}
	
}

BOOL	CVCAD3DRender::ChangeVCASourceInfo(BITMAPINFOHEADER *pbm)
{
	if(!m_bSetup){
		TRACE("CVCAD3DRender::ChangeVCASourceInfo Not setup before\n");
		return FALSE;
	}

	m_SourceSize.cx = pbm->biWidth;
	m_SourceSize.cy = pbm->biHeight;
	/*
	if (MAKEFOURCC('Y','U','Y','2') == pbm->biCompression) {
		m_eColorFormat = CD3d::CF_YUY2;
	} else if (MAKEFOURCC('Y','V','1','2') == pbm->biCompression) {
		m_eColorFormat = CD3d::CF_YV12;
	} else {
		ASSERT(0);
	}*/
	return TRUE;
}

BOOL	CVCAD3DRender::RenderVCAImage(BYTE *pImage, VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject)
{
	ULONG uRotate = 0;
	RECT rcImageROI = { 0 };

	m_pD3d->DrawImage(m_ClientRect, m_SourceSize, m_eColorFormat, (CD3d::eROTATE) uRotate, rcImageROI, pImage);

	RECT rcVCAROI = { 0 };
	RECT rcCanvas;
	SetCanvasSize(&rcCanvas, &rcVCAROI);

	if (pZone&&pRule&&pRule->usRuleType != VCA5_RULE_TYPE_COLSIG)//不是颜色检索才画区域
	{
		DrawSingleZone(rcVCAROI, pZone);
	}

	if (pRule&&pRule->usRuleType == VCA5_RULE_TYPE_DIRECTION)
	{
		VCA5_POINT pt;
		GetZonePie(pZone, &pt);
		DrawDirectionArrow22(m_pD3d, pt.x, pt.y,
	pRule->tRuleDataEx.tDirection.sStartAngle, pRule->tRuleDataEx.tDirection.sFinishAngle);
	}

	bool bSupportFeature = 1;
	if ((pZone&&pZone->usZoneStyle & VCA5_ZONE_STYLE_TRIPWIRE) && bSupportFeature){/// counting line bollocks

		VCA5_RULE_LINECOUNTER *pLC_A, *pLC_B;
		int Aticked = 0;
		int Bticked = 0;

		//	VCA5_APP_RULE	*pRuleA = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_LINECOUNTER_A);
		//VCA5_APP_RULE	*pRuleB = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_LINECOUNTER_B);
		if (pRule->usRuleType == VCA5_RULE_TYPE_LINECOUNTER_A)
		{
			Aticked = 1;
		}
		if (pRule->usRuleType == VCA5_RULE_TYPE_LINECOUNTER_B)
		{
			Bticked = 2;
		}

		if (Aticked || Bticked)
		{
			/*		int calibwidth = 0;
					if (Bticked)
					calibwidth = pLC_B->ulCalibrationWidth;
					else
					calibwidth = pLC_A->ulCalibrationWidth;*/

			DrawDirectionArrow2(pZone, Aticked + Bticked);

		}
	}

	if (pObject)
	{
		COLORREF Color = RGB(0xFF, 0, 0);
		DrawBoundinxBox(rcVCAROI, pObject->bBox, 0xFF, Color);
		DrawColorSignature(rcVCAROI, pObject, 0xFF, Color);
		DrawObjectInformation(rcVCAROI, pObject, 0xFF, Color);
		DrawTrail(rcVCAROI, pObject, 0xFF, Color, m_pDataMgr->GetDrawTrailMode());
		DrawTargetID(rcVCAROI, pObject, Color);
	}


	RECT	ScreenRect;
	ScreenRect = m_ClientRect;

	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.left);
	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.right);

	m_pD3d->PrimaryPresentation(&ScreenRect, &m_ClientRect);
	return true;
	
}

BOOL	CVCAD3DRender::RenderVCAData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG uRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp)
{
	if(!m_bSetup){
		TRACE("CVCAD3DRender::ChangeVCASourceInfo Not setup before\n");
		return FALSE;
	}
	if (!m_bDraw) {
	  // return FALSE;
	}
	
	m_pD3d->DrawImage(m_ClientRect, m_SourceSize, m_eColorFormat, (CD3d::eROTATE) uRotate, rcImageROI ,pImage);
	
	RECT rcCanvas;
	SetCanvasSize( &rcCanvas, &rcVCAROI );

	if (RENDER_VCA == m_RenderMode){
		m_pDataMgr->Lock();
		if (pVCAMetaLib)
		{
			DrawCountingLinesCounting(pVCAMetaLib);
		}
		if (m_bZonesVisible)
		{
			DrawZones(rcVCAROI, m_pDataMgr->GetZones());
			DrawCounters(rcVCAROI, m_pDataMgr->GetCounters());
		}

		m_pDataMgr->Unlock();
		if (pVCAMetaLib)
		{
			VCA5_PACKET_HEADER*	pHeader = pVCAMetaLib->GetHeader();
			DrawVCAData(rcVCAROI, pVCAMetaLib->GetObjects(), pVCAMetaLib->GetEvents(), pVCAMetaLib->GetCounts());
			DrawVCAHeaderMsg(pHeader);

			//if((m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_BLOBS) &&  (VCA5_PACKET_STATUS_OK == pHeader->ulVCAStatus)){
			if ((m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_BLOBS)){

				DrawBlobMap(rcCanvas, pVCAMetaLib->GetBlobMap(), VCA_COLOR_BLOB);
				DrawBlobMap(rcCanvas, pVCAMetaLib->GetStationaryBlobMap(), VCA_COLOR_STATBLOB);
				DrawBlobMap(rcCanvas, pVCAMetaLib->GetSmokeMap(), VCA_COLOR_SMOKEBLOB);
				DrawBlobMap(rcCanvas, pVCAMetaLib->GetFireMap(), VCA_COLOR_FIREBLOB);
			}
		}
	}
	if (pVCAMetaLib)
	{
		//Display Tamper info
		if (m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_TAMPER_ALARM){
			DrawBlobMap(rcCanvas, pVCAMetaLib->GetTamperInfo(), VCA_COLOR_BLOB);
		}


		if (m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_SCENECANGE){
			DrawBlobMap(rcCanvas, pVCAMetaLib->GetSceneChangeInfo(), VCA_COLOR_SCENECHANGE);
			//Draw Learning scene
			VCA5_PACKET_HEADER*	pHeader = pVCAMetaLib->GetHeader();
			if (pHeader->ulVCAStatus == VCA5_PACKET_STATUS_LEARNING_SCENE) DrawVCAHeaderMsg(pHeader);
		}



		//Draw Tamper Alarm
		ULONG *pTamperAlarm = pVCAMetaLib->GetTamperAlarm();
		if (*pTamperAlarm != 0){
			ULONG	ulClientWidth = m_ClientRect.right - m_ClientRect.left;
			ULONG	ulClientHeight = m_ClientRect.bottom - m_ClientRect.top;

			if (m_tamperAlarmFlash < 15){
				POINT ptStartXY;
				ptStartXY.x = ulClientWidth / 2 + ulClientWidth / 9;
				ptStartXY.y = ulClientHeight / 2;

				DrawVCAMsg("**TAMPERING DETECTED**", RGB(255, 0, 0), ptStartXY, TEXT_ALIGN_CENTRE | TEXT_ALIGN_CENTRE_VERTICAL);
			}
			m_tamperAlarmFlash = (m_tamperAlarmFlash + 1) % 30;
			(*pTamperAlarm)--;
		}
		else{
			m_tamperAlarmFlash = 0;
		}
	}
/*
	if(pTimestamp){
		SYSTEMTIME stTime;
		FileTimeToSystemTime((FILETIME *)pTimestamp, &stTime); 
		char strTime[32];
		POINT ptStartXY;
		ULONG	ulClientWidth	= m_ClientRect.right - m_ClientRect.left;
		ULONG	ulClientHeight	= m_ClientRect.bottom - m_ClientRect.top;

		ptStartXY.x = ulClientWidth/2 + ulClientWidth/9;
		ptStartXY.y = ulClientHeight/2;

		sprintf(strTime, "%02d:%02d:%02d.%03d", stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);
		DrawVCAMsg(strTime, RGB(255, 255, 0) ,ptStartXY, TEXT_ALIGN_CENTRE |  TEXT_ALIGN_CENTRE_VERTICAL);

	}
*/


	RECT	ScreenRect;
	ScreenRect = m_ClientRect;
		
	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.left );
	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.right );

	m_pD3d->PrimaryPresentation(&ScreenRect, &m_ClientRect);

	return TRUE;
}


BOOL	CVCAD3DRender::RenderVCAMetaData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, BYTE *pMetadata, int nLength, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp)
{
	if(!m_bSetup){
		TRACE("CVCAD3DRender::ChangeVCASourceInfo Not setup before\n");
		return FALSE;
	}
	if (!m_bDraw) {
	   return FALSE;
	}

	m_pD3d->DrawImage(m_ClientRect, m_SourceSize, m_eColorFormat, (CD3d::eROTATE) ulRotate, rcImageROI, pImage);

	m_pDataMgr->Lock();
	DrawZones(rcVCAROI, m_pDataMgr->GetZones());
	DrawCounters(rcVCAROI, m_pDataMgr->GetCounters());
	m_pDataMgr->Unlock();

	int nRenderFlags = VRF_OBJECTS;
	DWORD	DisplayFlags = m_pDataMgr->GetDisplayFlags();
	if(DisplayFlags& DISPLAY_BLOBS)
		nRenderFlags	|= VRF_BLOBS;
	if(DisplayFlags & DISPLAY_NON_ALARMS)
		nRenderFlags	|= VRF_NON_ALARMS;
/*	if(DisplayFlags & DISPLAY_OBJECT_SPEED)
		nRenderFlags	|= DISPLAY_OBJECT_SPEED;
	if(DisplayFlags & DISPLAY_OBJECT_HEIGHT)
		nRenderFlags	|= VRF_OBJECT_HEIGHT;
	if(DisplayFlags & DISPLAY_OBJECT_AREA)
		nRenderFlags	|= VRF_OBJECT_AREA;
	if(DisplayFlags & DISPLAY_OBJECT_CLASSIFICATION)
		nRenderFlags	|= VRF_OBJECT_CLASS;
*/	

	if( pMetadata && nLength)
	{
		VMR_SetRenderingFlags( m_hVMR, nRenderFlags);

		//3. Set VCAMeate Data
		VMR_SetMetaData(m_hVMR, pMetadata, nLength);
		//4. Render
		VMR_Render(m_hVMR, m_pD3d->GetBackDDS(), &m_ClientRect);
	}



	RECT	ScreenRect;
	ScreenRect = m_ClientRect;
		
	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.left );
	ClientToScreen(m_hWnd, (POINT*)&ScreenRect.right );
	m_pD3d->PrimaryPresentation(&ScreenRect, &m_ClientRect);

	return TRUE;
}


#define PULSE_LENGTH 8

int PULSE (int n)
{
	int ret;

	if (n < PULSE_LENGTH/2)
		ret =  255*n/(PULSE_LENGTH/2);
	else
		ret = 255 - 127*(n - PULSE_LENGTH/2)/(PULSE_LENGTH/2);

	return ret;
}


void CVCAD3DRender::DrawCountingLinesCounting(CVCAMetaLib *pVCAMetaLib)
{
//	CVCAMetaLib::VCA_OBJECTS	*pObjects = m_pMetaLib->GetObjects();
	int n;
	std::list<CNT_LINE_EVENTHIST>::iterator it;
	
//	if (!(m_pDataMgr->GetDisplayFlags() & VCA_DISPLAY_ZONES)) return;

	// need to process these even if not drawing
//	if ( ( m_eAPIStatus != VCA_HTTPAPI_SAVING ) && ( m_eAPIStatus != VCA_HTTPAPI_LOADING ) ){
		m_pDataMgr->UpdateCountingLineEvents(pVCAMetaLib->GetCountLineEvents());
			

//	}

	std::list<CNT_LINE_EVENTHIST>*  countLineEventsA = m_pDataMgr->GetCountingLineEventsA();
	std::list<CNT_LINE_EVENTHIST>*  countLineEventsB = m_pDataMgr->GetCountingLineEventsB();

	
	/// draw them
	int iAreaIdx;
	VCA5_APP_AREA_T_E eAreaType;
	m_pDataMgr->GetSelectAreaIdx( &iAreaIdx, &eAreaType );
	if(eAreaType == VCA5_APP_AREA_T_COUNTER) return;
	VCA5_APP_ZONE*	pZone =	m_pDataMgr->GetZone(iAreaIdx);
	if(pZone == NULL || (pZone->usZoneStyle != VCA5_ZONE_STYLE_TRIPWIRE)) return;

	for (int i = 0; i < VCA5_MAX_NUM_ZONES ; i++)
	{
		n = 1;
		for (it = countLineEventsA[i].begin(); it != countLineEventsA[i].end(); it++, n++)
		{
			CNT_LINE_EVENTHIST *pEvntHist = &(*it);
			if (i == pZone->usZoneId)
			{		 
				if (pEvntHist->age > PULSE_LENGTH)
					DrawCountingLineEvent(pEvntHist->pZone, &pEvntHist->info, n, 128/*255*(1 - (float)n/10)*/);
				else
					DrawCountingLineEvent(pEvntHist->pZone, &pEvntHist->info, n, PULSE(pEvntHist->age));
			}
			pEvntHist->age++;
		}
		
		n = 1;
		for (it = countLineEventsB[i].begin(); it != countLineEventsB[i].end(); it++, n++)
		{
			CNT_LINE_EVENTHIST *pEvntHist = &(*it); 
			if (i == pZone->usZoneId)
			{
				if (pEvntHist->age > PULSE_LENGTH)
					DrawCountingLineEvent(pEvntHist->pZone, &pEvntHist->info, -n, 128/*255*(1 - (float)n/10)*/);
				else
					DrawCountingLineEvent(pEvntHist->pZone, &pEvntHist->info, -n, PULSE(pEvntHist->age));
			}
			pEvntHist->age++;

		}
	}
}


void CVCAD3DRender::DrawCountingLineEvent( VCA5_APP_ZONE* pZone, VCA5_COUNTLINE_EVENT *pInfo, int dir, unsigned char alpha)
{

	float ratio1, ratio2;

	unsigned int uiCanvasDrawWidth, uiCanvasDrawHeight;
	unsigned int uiCanvasOriginalWidth, uiCanvasOriginalHeight;
	unsigned int uiClientWidth, uiClientHeight;

	RECT rcCanvas;
	uiClientWidth	= m_ClientRect.right - m_ClientRect.left;
	uiClientHeight	= m_ClientRect.bottom - m_ClientRect.top;
	SIZE	sizeImage = { uiClientWidth, uiClientHeight };


	uiCanvasOriginalWidth  = m_pD3d->GetCurrent3DCanvasSize().cx;
	uiCanvasOriginalHeight = m_pD3d->GetCurrent3DCanvasSize().cy;

	//___________________________________________________
	//	If the IE client window is smaller than the canvas,
	//	Set the canvas to be the size of the IE client window.
	if ( ( uiClientWidth  <= uiCanvasOriginalWidth  ) &&
		 ( uiClientHeight <= uiCanvasOriginalHeight ) )
	{
		uiCanvasDrawWidth  = uiClientWidth;
		uiCanvasDrawHeight = uiClientHeight;
	}
	//___________________________________________________
	//	If the IE client window is bigger than the canvas,
	else
	{
		//_______________________________________________
		//	Find the biggest ratio between IE client window and 
		//	the canvas size in horizontal and vertical direction.
		ratio1 = (float) uiClientWidth  / uiCanvasOriginalWidth;
		ratio2 = (float) uiClientHeight / uiCanvasOriginalHeight;

		if ( ratio1 > ratio2 )
		{
			uiCanvasDrawWidth  = uiCanvasOriginalWidth;
			uiCanvasDrawHeight = (unsigned int) (uiClientHeight / ratio1);
		}
		else
		{
			uiCanvasDrawWidth  = (unsigned int) (uiClientWidth / ratio2);
			uiCanvasDrawHeight = uiCanvasOriginalHeight;
		}
	}

#ifdef _VMR_EXPORT
	rcCanvas = m_rcCanvas;
#else
	SetRect( &rcCanvas, 0, 0, uiCanvasDrawWidth, uiCanvasDrawHeight );
#endif


	FLT_POINT ptMiddle, pt1, pt2, ptBegin, ptEnd;
	POINT ptA, ptB;


	for ( unsigned int i = 0; i < pZone->ulTotalPoints-1; i++)
	{
		PERCENTTOPIXEL( pt1.x, pZone->pPoints[i].x, uiClientWidth );
		PERCENTTOPIXEL( pt1.y, pZone->pPoints[i].y, uiClientHeight );
		PERCENTTOPIXEL( pt2.x, pZone->pPoints[i+1].x, uiClientWidth );
		PERCENTTOPIXEL( pt2.y, pZone->pPoints[i+1].y, uiClientHeight );

		ptMiddle.x = float(pt1.x + pt2.x)/2;
		ptMiddle.y = float(pt1.y + pt2.y)/2;

		double dAngle  = atan2(pt1.x - pt2.x, pt1.y - pt2.y);

		double	dEventWidth;
		PERCENTTOPIXEL( dEventWidth, pInfo->usWidth, uiClientWidth);

		double	dEventStart;
		PERCENTTOPIXEL( dEventStart, pInfo->usPos, uiClientWidth);

//		assert(dEventWidth != 0);

		ptBegin.x = ptMiddle.x - (dEventWidth/2) * sin( dAngle );
		ptBegin.y = ptMiddle.y -  (dEventWidth/2) * cos( dAngle );

		ptEnd.x = ptMiddle.x + (dEventWidth/2) * sin( dAngle );
		ptEnd.y = ptMiddle.y + (dEventWidth/2) * cos( dAngle );
/*
		ptBegin.x = pt1.x - dEventStart * sin( dAngle );
		ptBegin.y = pt1.y -  dEventStart * cos( dAngle );

		ptEnd.x = ptBegin.x - dEventWidth * sin( dAngle );
		ptEnd.y = ptBegin.y - dEventWidth * cos( dAngle );
*/
	
		int colour = 0;
		if (pInfo->usNum == 0)
			colour = RGB(0, 0, 0);
		else
			colour =  RGB(255,255,255);

		if (pInfo->usNum <= 1)
		{
			ptA.x = (LONG)round(ptBegin.x + 8*dir  * cos( dAngle ));
			ptA.y = (LONG)round(ptBegin.y - 8*dir  * sin( dAngle ));

			ptB.x = (LONG)round(ptEnd.x +  8*dir* cos( dAngle ));
			ptB.y = (LONG)round(ptEnd.y -  8*dir* sin( dAngle ));


			DrawThickLine( &rcCanvas, ptA, ptB, alpha, colour, uiCanvasDrawWidth, uiCanvasDrawHeight );
		}
		else 
		{
			double seperatorWidth = 20;
			double sectionWidth = (dEventWidth - seperatorWidth*(pInfo->usNum-1))/(double)pInfo->usNum;
			FLT_POINT ptSectionBegin = ptBegin;

			for (int n = 0; n < pInfo->usNum; n++)
			{
				ptA.x = (LONG)round(ptSectionBegin.x + 8*dir* cos( dAngle ));
				ptA.y = (LONG)round(ptSectionBegin.y - 8*dir* sin( dAngle ));

				ptB.x = (LONG)round(ptSectionBegin.x + sectionWidth*sin(dAngle) + 8*dir* cos( dAngle ));
				ptB.y = (LONG)round(ptSectionBegin.y + sectionWidth*cos(dAngle) - 8*dir* sin( dAngle ));

				DrawThickLine( &rcCanvas, ptA, ptB, alpha, colour, uiCanvasDrawWidth, uiCanvasDrawHeight );

				ptSectionBegin.x += (sectionWidth + seperatorWidth)*sin(dAngle);
				ptSectionBegin.y += (sectionWidth + seperatorWidth)*cos(dAngle);
			}
		}
	}
}

void CVCAD3DRender::DrawDirectionArrow2( VCA5_APP_ZONE *pZone, int dir)
{
	CEasyArrowD3D arrow(dir == 3 ? ARROW_STYLE_DOUBLE : ARROW_STYLE_NORMAL);
	unsigned int		i;
	double	dAngle;
	RECT	rc3DCanvas = m_ClientRect;

	long lCurrentWidth	= m_ClientRect.right - m_ClientRect.left;
	long lCurrentHeight	= m_ClientRect.bottom - m_ClientRect.top;

	SIZE	sizeImage = { lCurrentWidth, lCurrentHeight };

	arrow.SetD3D( m_pD3d );
	arrow.SetArrowWidth( ARROW_WIDTH );
	arrow.SetColor( 192, RGB2(255, 255, 255) );

	for ( i = 0; i < pZone->ulTotalPoints-1; i++)
	{
		FLT_POINT pt1, pt2, ptFoot, ptHead;
		
		PERCENTTOPIXEL( pt1.x, pZone->pPoints[i].x, lCurrentWidth );
		PERCENTTOPIXEL( pt1.y, pZone->pPoints[i].y, lCurrentHeight );
		PERCENTTOPIXEL( pt2.x, pZone->pPoints[i+1].x, lCurrentWidth );
		PERCENTTOPIXEL( pt2.y, pZone->pPoints[i+1].y, lCurrentHeight );

		dAngle  = atan2(pt1.x - pt2.x, pt1.y - pt2.y);

		ptFoot.x = (pt1.x + pt2.x)/2;
		ptFoot.y = (pt1.y + pt2.y)/2;

		if (dir == 1)
		{	
			ptHead.x = ptFoot.x + ARROW_LENGTH  * cos( dAngle );
			ptHead.y = ptFoot.y - ARROW_LENGTH  * sin( dAngle );
		}
		else if (dir == 2)
		{
			ptHead.x = ptFoot.x - ARROW_LENGTH  * cos( dAngle );
			ptHead.y = ptFoot.y + ARROW_LENGTH  * sin( dAngle );
		}
		else if (dir == 3)
		{
			ptHead.x = ptFoot.x + ARROW_LENGTH  * cos( dAngle );
			ptHead.y = ptFoot.y - ARROW_LENGTH  * sin( dAngle );
	
			ptFoot.x -= ptHead.x - ptFoot.x;
			ptFoot.y -= ptHead.y - ptFoot.y;
		}

		
		POINT ptFoot2, ptHead2;

		ptFoot2.x = (LONG)ptFoot.x;
		ptFoot2.y = (LONG)ptFoot.y;
		ptHead2.x = (LONG)ptHead.x;
		ptHead2.y = (LONG)ptHead.y;

		arrow.SetPosition( ptFoot2, ptHead2 );
		arrow.DrawArrow( rc3DCanvas, sizeImage );
	
		ptFoot.x = (pt1.x + pt2.x)/2;
		ptFoot.y = (pt1.y + pt2.y)/2;

		if (dir & 1)
		{
			POINT head;
			ptHead.x = ptFoot.x + (ARROW_LENGTH + 20)  * cos( dAngle );
			ptHead.y = ptFoot.y -  (ARROW_LENGTH + 20)  * sin( dAngle );

			head.x = (LONG)round(ptHead.x);
			head.y = (LONG)round(ptHead.y);

			DrawVCAMsg( "A", RGB(255,255,255), head, TEXT_ALIGN_CENTRE |  TEXT_ALIGN_CENTRE_VERTICAL);
		
		}
		if (dir & 2)
		{
			POINT head;
			ptHead.x = ptFoot.x - (ARROW_LENGTH + 20)  * cos( dAngle );
			ptHead.y = ptFoot.y + (ARROW_LENGTH + 20)  * sin( dAngle );

			head.x = (LONG)round(ptHead.x);
			head.y = (LONG)round(ptHead.y);
			DrawVCAMsg( "B", RGB(255,255,255), head, TEXT_ALIGN_CENTRE |  TEXT_ALIGN_CENTRE_VERTICAL);
		}

/*
		if (m_helpStatus == HLP_SHOW_ARROW)
		{
			m_ptHelpPnt.x = round(ptFoot.x);
			m_ptHelpPnt.y = round(ptFoot.y);	
		}
*/
	}

}

void CVCAD3DRender::DrawCounterCalibration( VCA5_APP_ZONE *pZone, int calibrationWidth)
{

	float ratio1, ratio2;

	unsigned int lCurrentWidth, lCurrentHeight;
	unsigned int uiCanvasDrawWidth, uiCanvasDrawHeight;
	unsigned int uiCanvasOriginalWidth, uiCanvasOriginalHeight;
	RECT rcCanvas;

	lCurrentWidth	= m_ClientRect.right - m_ClientRect.left;
	lCurrentHeight	= m_ClientRect.bottom - m_ClientRect.top;

	SIZE sizeImage = { lCurrentWidth, lCurrentHeight };

	uiCanvasOriginalWidth  = m_pD3d->GetCurrent3DCanvasSize().cx;
	uiCanvasOriginalHeight = m_pD3d->GetCurrent3DCanvasSize().cy;


	//___________________________________________________
	//	If the IE client window is smaller than the canvas,
	//	Set the canvas to be the size of the IE client window.
	if ( ( lCurrentWidth  <= uiCanvasOriginalWidth  ) &&
		 ( lCurrentHeight <= uiCanvasOriginalHeight ) )
	{
		uiCanvasDrawWidth  = lCurrentWidth;
		uiCanvasDrawHeight = lCurrentHeight;
	}
	//___________________________________________________
	//	If the IE client window is bigger than the canvas,
	else
	{
		//_______________________________________________
		//	Find the biggest ratio between IE client window and 
		//	the canvas size in horizontal and vertical direction.
		ratio1 = (float) lCurrentWidth  / uiCanvasOriginalWidth;
		ratio2 = (float) lCurrentHeight / uiCanvasOriginalHeight;

		if ( ratio1 > ratio2 )
		{
			uiCanvasDrawWidth  = uiCanvasOriginalWidth;
			uiCanvasDrawHeight = (int) ((float)lCurrentHeight / ratio1);
		}
		else
		{
			uiCanvasDrawWidth  = (int) ((float)lCurrentWidth / ratio2);
			uiCanvasDrawHeight = uiCanvasOriginalHeight;
		}
	}

#ifdef _VMR_EXPORT
	rcCanvas = m_rcCanvas;
#else
	SetRect( &rcCanvas, 0, 0, uiCanvasDrawWidth, uiCanvasDrawHeight );
#endif


	FLT_POINT pt1, pt2, ptMiddle, ptMiddle2;
	POINT ptBottom, ptTop;
	
	double totalLineLength = 0;
	

	for ( unsigned int i = 0; i < pZone->ulTotalPoints-1; i++)
	{
		PERCENTTOPIXEL( pt1.x, pZone->pPoints[i].x, lCurrentWidth  );
		PERCENTTOPIXEL( pt1.y, pZone->pPoints[i].y, lCurrentHeight );
		PERCENTTOPIXEL( pt2.x, pZone->pPoints[i+1].x, lCurrentWidth  );
		PERCENTTOPIXEL( pt2.y, pZone->pPoints[i+1].y, lCurrentHeight );


		double dAngle  = atan2(pt1.x - pt2.x, pt1.y - pt2.y);

		
		double lineLength = sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));

	
		lineLength *=	65535.0/(double)lCurrentWidth; 


		calibrationWidth = (int)min(calibrationWidth, lineLength);

		double	dCalibrationWidth;
		PERCENTTOPIXEL(dCalibrationWidth, calibrationWidth, lCurrentWidth);
		
//		assert (calibrationWidth != 0);
	

		ptMiddle.x = (pt1.x + pt2.x)/2;
		ptMiddle.y = (pt1.y + pt2.y)/2;

	
		for (int n = 0; n < 2; n++)
		{
			ptMiddle2.x = ptMiddle.x + (n*dCalibrationWidth - dCalibrationWidth/2) * sin( dAngle );
			ptMiddle2.y = ptMiddle.y + (n*dCalibrationWidth - dCalibrationWidth/2) * cos( dAngle );

			ptTop.x = (LONG)round(ptMiddle2.x + 25  * cos( dAngle ));
			ptTop.y = (LONG)round(ptMiddle2.y - 25  * sin( dAngle ));

			ptBottom.x = (LONG)round(ptMiddle2.x - 25  * cos( dAngle ));
			ptBottom.y = (LONG)round(ptMiddle2.y + 25  * sin( dAngle ));

			/*
			if (n == 0 && i == 0 && m_helpStatus == HLP_SHOW_CALIB )
			{
				m_ptHelpPnt.x = round(ptMiddle2.x);
				m_ptHelpPnt.y = round(ptMiddle2.y);
			}
			*/
		
		
			DrawThickLine( &rcCanvas, ptBottom, ptTop, 128, pZone->uiColor, uiCanvasDrawWidth, uiCanvasDrawHeight );

			/// side lines
			double newWidth = min(lineLength, 1.5*calibrationWidth);
			PERCENTTOPIXEL(newWidth, newWidth, lCurrentWidth);

			ptMiddle2.x = ptMiddle.x + (n*newWidth - newWidth/2) * sin( dAngle );
			ptMiddle2.y = ptMiddle.y + (n*newWidth - newWidth/2) * cos( dAngle );

			ptTop.x = (LONG)round(ptMiddle2.x + 10  * cos( dAngle ));
			ptTop.y = (LONG)round(ptMiddle2.y - 10  * sin( dAngle ));

			ptBottom.x = (LONG)round(ptMiddle2.x - 10  * cos( dAngle ));
			ptBottom.y = (LONG)round(ptMiddle2.y + 10  * sin( dAngle ));


			DrawThickLine( &rcCanvas, ptBottom, ptTop, 128, pZone->uiColor, uiCanvasDrawWidth, uiCanvasDrawHeight, 1.0f );

			
			ptMiddle2.x = ptMiddle.x + 0.5*(n*dCalibrationWidth - dCalibrationWidth/2) * sin( dAngle );
			ptMiddle2.y = ptMiddle.y + 0.5*(n*dCalibrationWidth - dCalibrationWidth/2) * cos( dAngle );

			ptTop.x = (LONG)round(ptMiddle2.x + 10  * cos( dAngle ));
			ptTop.y = (LONG)round(ptMiddle2.y - 10  * sin( dAngle ));

			ptBottom.x = (LONG)round(ptMiddle2.x - 10  * cos( dAngle ));
			ptBottom.y = (LONG)round(ptMiddle2.y + 10  * sin( dAngle ));

			DrawThickLine( &rcCanvas, ptBottom, ptTop, 128, pZone->uiColor, uiCanvasDrawWidth, uiCanvasDrawHeight, 1.0f );

	//		m_pD3D->m_DrawCircle( rcCanvas, sizeImage, ptTop, 6, 1, 128, RGB2(160,160,160) ); 	/// draw bell-end
	//		m_pD3D->m_DrawCircle( rcCanvas, sizeImage, ptTop, 4, 1, 255, RGB2(255,255,255) );
		}
	}
}



void	CVCAD3DRender::DrawZones(RECT rcVCAROI, VCA5_APP_ZONES *pZones)
{
	for (ULONG i = 0; i < pZones->ulTotalZones; ++i) {
		VCA5_APP_ZONE *pZone = &(pZones->pZones[i]);
		BOOL bSupportFeature;
		DrawSingleZone(rcVCAROI, pZone);

		//if(!(pZone->uiStatus & VCA5_APP_AREA_STATUS_SELECTED)) continue;
		
		bSupportFeature = m_pDataMgr->CheckFeature(VCA5_FEATURE_DIRECTION);
		if 	((m_pDataMgr->CheckZoneAngle(i) == ANGLE_PART) && bSupportFeature) {

			VCA5_APP_RULE	*pRule = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_DIRECTION );
			if (pRule && pRule->ucTicked) {
				VCA5_POINT pt;
				GetZonePie(pZone, &pt);

				double dAngle = 0;
				long lCurrentWidth = m_ClientRect.right - m_ClientRect.left;
				long lCurrentHeight = m_ClientRect.bottom - m_ClientRect.top;
				for (int j = 0; j < pZone->ulTotalPoints - 1; j++)
				{
					FLT_POINT pt1, pt2, ptFoot, ptHead;

					PERCENTTOPIXEL(pt1.x, pZone->pPoints[j].x, lCurrentWidth);
					PERCENTTOPIXEL(pt1.y, pZone->pPoints[j].y, lCurrentHeight);
					PERCENTTOPIXEL(pt2.x, pZone->pPoints[j + 1].x, lCurrentWidth);
					PERCENTTOPIXEL(pt2.y, pZone->pPoints[j + 1].y, lCurrentHeight);

					//dAngle = atan2(pt1.x - pt2.x, pt1.y - pt2.y) / PI * 180;;
					dAngle = atan2(pt1.y - pt2.y, pt1.x - pt2.x) / PI * 180;;
				}
				int dif = 900;
				if (pRule->reserved[0] == 2)
				{
					dif = 1799;
				}
				pRule->tRuleDataEx.tDirection.sStartAngle = (90 - dAngle) * 10 - dif;
				pRule->tRuleDataEx.tDirection.sFinishAngle = (90 - dAngle) * 10 + dif;
				if (i == 0)
				{
				DrawDirectionArrow(m_pD3d, pt.x, pt.y,
					pRule->tRuleDataEx.tDirection.sStartAngle, pRule->tRuleDataEx.tDirection.sFinishAngle);
				}
				else if (i == 1)
				{
					DrawDirectionArrow22(m_pD3d, pt.x, pt.y,
						pRule->tRuleDataEx.tDirection.sStartAngle, pRule->tRuleDataEx.tDirection.sFinishAngle);
				}
				else if (i == 2)
				{
					DrawDirectionArrow33(m_pD3d, pt.x, pt.y,
						pRule->tRuleDataEx.tDirection.sStartAngle, pRule->tRuleDataEx.tDirection.sFinishAngle);
				}
			}
		}

		bSupportFeature = m_pDataMgr->CheckFeature(VCA5_FEATURE_LINECOUNTER);
		if ((pZone->usZoneStyle & VCA5_ZONE_STYLE_TRIPWIRE) && bSupportFeature){/// counting line bollocks
			
			VCA5_RULE_LINECOUNTER *pLC_A, *pLC_B;
			int Aticked = 0;
			int Bticked = 0;
			
			VCA5_APP_RULE	*pRuleA = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_LINECOUNTER_A );
			VCA5_APP_RULE	*pRuleB = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_LINECOUNTER_B );

			if 	( pRuleA  && (pZone->usZoneType == VCA5_ZONE_TYPE_DETECTION) ){
				pLC_A = &(pRuleA->tRuleDataEx.tLineCounter);
				if(pRuleA->ucTicked)Aticked = 1; 
			}
			if 	( pRuleB  && (pZone->usZoneType == VCA5_ZONE_TYPE_DETECTION) ){
				pLC_B = &(pRuleB->tRuleDataEx.tLineCounter);
				if(pRuleB->ucTicked)Bticked = 2; 
			}


			if ( Aticked || Bticked)
			{
				int calibwidth = 0;
				if (Bticked)
					calibwidth = pLC_B->ulCalibrationWidth;
				else  
					calibwidth = pLC_A->ulCalibrationWidth;
				
				//m_helpStatus = HLP_SHOW_ARROW;
				DrawDirectionArrow2( pZone, Aticked + Bticked);

				if (pRuleB->ucWidthCalibrationEnabled || pRuleA->ucWidthCalibrationEnabled)
				{
					//m_helpStatus = HLP_SHOW_CALIB;
					DrawCounterCalibration( pZone, calibwidth);

					//if (!m_bShownCalibHelp && m_bShownArrowHelp)
					//	m_pDataMgr->ShowLineCounterHelp(TRUE);

					//m_bShownCalibHelp = TRUE;
					pRuleB->ucWidthCalibrationEnabled = 1;
					pRuleA->ucWidthCalibrationEnabled = 1;
				
				}
				pLC_A->ulCalibrationWidth = pLC_B->ulCalibrationWidth = calibwidth;
			//	pRuleA->ucWidthCalibrationEnabled = pRuleB->ucWidthCalibrationEnabled;
				
			}
		}
	}
}

void	CVCAD3DRender::DrawSingleZone(RECT rcVCAROI, VCA5_APP_ZONE *pZone)
{	
	CD3d::MYPOLYGON	Polygon;
	BYTE alpha = 64;
	SIZE Size = {m_ClientRect.right, m_ClientRect.bottom};
	CD3d::eBRUSHTYPE eBrushType = (pZone->usZoneType == VCA5_ZONE_TYPE_NONDETECTION) ? CD3d::BRT_HATCH : CD3d::BRT_SOLID ;

	if (pZone->uiStatus & VCA5_APP_AREA_STATUS_NOTIFIED ||
		pZone->uiStatus & VCA5_APP_AREA_STATUS_SELECTED ) {
		alpha = 128;
	}

//	CRect rcVCAROI1(rcVCAROI);
	RECT rcVCAROI1 = rcVCAROI;
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top; 
	
	if ((!width)|| !height) {
		//rcVCAROI1.CopyRect(&m_ClientRect);
		rcVCAROI1 = m_ClientRect;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	} else {
		rcVCAROI1.right = min(rcVCAROI1.right-1, m_SourceSize.cx);
		rcVCAROI1.bottom = min(rcVCAROI1.bottom-1, m_SourceSize.cy);
		PIXELTOPERCENT(rcVCAROI1.left, rcVCAROI1.left, m_SourceSize.cx);
		PIXELTOPERCENT(rcVCAROI1.top, rcVCAROI1.top, m_SourceSize.cy);
		PIXELTOPERCENT(rcVCAROI1.right, rcVCAROI1.right, m_SourceSize.cx);
		PIXELTOPERCENT(rcVCAROI1.bottom, rcVCAROI1.bottom, m_SourceSize.cy);

		PERCENTTOPIXEL(rcVCAROI1.left, rcVCAROI1.left, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI1.top, rcVCAROI1.top, m_ClientRect.bottom);
		PERCENTTOPIXEL(rcVCAROI1.right, rcVCAROI1.right, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI1.bottom, rcVCAROI1.bottom, m_ClientRect.bottom);
	}

	Polygon.cnt_point = pZone->ulTotalPoints;
	for (ULONG i = 0; i < Polygon.cnt_point; ++i) {
		PERCENTTOPIXEL(Polygon.atpoint[i].x, pZone->pPoints[i].x, width);
		PERCENTTOPIXEL(Polygon.atpoint[i].y, pZone->pPoints[i].y, height);
		Polygon.atpoint[i].x += rcVCAROI1.left;
		Polygon.atpoint[i].y += rcVCAROI1.top;
	}

	// draw zone
	if (VCA5_ZONE_STYLE_POLYGON == pZone->usZoneStyle) {
		Polygon.cnt_point = pZone->ulTotalPoints - 1;
		m_pD3d->DrawPolygonList(m_ClientRect, Size, &Polygon, 1, eBrushType, alpha, pZone->uiColor);

	} else if (VCA5_ZONE_STYLE_TRIPWIRE == pZone->usZoneStyle) {
		for (ULONG i = 0; i < Polygon.cnt_point-1; ++i) {
			CD3d::MYPOLYGON TripWire;	
			int x1, y1, x2, y2;

			x1 = Polygon.atpoint[i].x;
			y1 = Polygon.atpoint[i].y;
			x2 = Polygon.atpoint[i+1].x;
			y2 = Polygon.atpoint[i+1].y;

			float fTheta = atan2( (float)(y2-y1), (float)(x2-x1) );
			float fx = sin(fTheta);
			float fy = cos(fTheta);
			float fxOff, fyOff;
			fxOff = 2 * fx;
			fyOff = 2 * fy;

			TripWire.cnt_point = 4;
			TripWire.atpoint[0].x = (LONG) (x1 - fxOff);
			TripWire.atpoint[0].y = (LONG) (y1 + fyOff);
			TripWire.atpoint[1].x = (LONG) (x1 + fxOff);
			TripWire.atpoint[1].y = (LONG) (y1 - fyOff);
			TripWire.atpoint[2].x = (LONG) (x2 + fxOff);
			TripWire.atpoint[2].y = (LONG) (y2 - fyOff);
			TripWire.atpoint[3].x = (LONG) (x2 - fxOff);
			TripWire.atpoint[3].y = (LONG) (y2 + fyOff);

			m_pD3d->DrawPolygonList(m_ClientRect, Size, &TripWire, 1, CD3d::BRT_SOLID, alpha, pZone->uiColor);
		}

	} 

	if (pZone->uiStatus & VCA5_APP_AREA_STATUS_SELECTED) {

		for (ULONG i = 0; i < Polygon.cnt_point; ++i)	{
			m_pD3d->DrawCircle(m_ClientRect, Size, Polygon.atpoint[i], 6, 1, alpha, pZone->uiColor);
			m_pD3d->DrawCircle(m_ClientRect, Size, Polygon.atpoint[i], 4, 1, alpha, RGB(255,255,255) );
		}
	}
}

void	CVCAD3DRender::DrawCounters(RECT rcVCAROI, VCA5_APP_COUNTERS *pCounters)
{
	for (ULONG i = 0; i < pCounters->ulTotalCounters; ++i) {
		DrawSingleCounter(rcVCAROI, &pCounters->pCounters[i]);
	}	
}

void	CVCAD3DRender::DrawSingleCounter(RECT rcVCAROI, VCA5_APP_COUNTER *pCounter)
{
	char strTemp[VCA5_MAX_STR_LEN];
	USES_CONVERSION;
	CD3d::MYPOLYGON	Polygon;

	RECT rcVCAROI1 = rcVCAROI;
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top;

	if ((!width) || !height) {
		//rcVCAROI1.CopyRect(&m_ClientRect);
		rcVCAROI1 = m_ClientRect;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	} else {
		rcVCAROI1.right = min(rcVCAROI1.right-1, m_SourceSize.cx);
		rcVCAROI1.bottom = min(rcVCAROI1.bottom-1, m_SourceSize.cy);
		PIXELTOPERCENT(rcVCAROI1.left, rcVCAROI1.left, m_SourceSize.cx);
		PIXELTOPERCENT(rcVCAROI1.top, rcVCAROI1.top, m_SourceSize.cy);
		PIXELTOPERCENT(rcVCAROI1.right, rcVCAROI1.right, m_SourceSize.cx);
		PIXELTOPERCENT(rcVCAROI1.bottom, rcVCAROI1.bottom, m_SourceSize.cy);

		PERCENTTOPIXEL(rcVCAROI1.left, rcVCAROI1.left, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI1.top, rcVCAROI1.top, m_ClientRect.bottom);
		PERCENTTOPIXEL(rcVCAROI1.right, rcVCAROI1.right, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI1.bottom, rcVCAROI1.bottom, m_ClientRect.bottom);
	}

	height = m_ClientRect.bottom - m_ClientRect.top;
	float	fScalor = 0.0f;
	UINT uiFontSize = 0;
	UINT uiFontSizeRemain = 0;	

	BYTE alpha = 255;
	SIZE Size = {m_ClientRect.right, m_ClientRect.bottom};

	Polygon.cnt_point = pCounter->ulTotalPoints;
	for (ULONG i = 0; i < Polygon.cnt_point; ++i) {
		PERCENTTOPIXEL(Polygon.atpoint[i].x, pCounter->pPoints[i].x, width);
		PERCENTTOPIXEL(Polygon.atpoint[i].y, pCounter->pPoints[i].y, height);
		Polygon.atpoint[i].x += rcVCAROI1.left;
		Polygon.atpoint[i].y += rcVCAROI1.top;
	}

	GetTextScalingFactor(height, fScalor, uiFontSize, uiFontSizeRemain);

	fScalor+=0.1f;

	POINT tmpPoint = Polygon.atpoint[Polygon.cnt_point-1];
//#if defined(_UNICODE)
//	size_t len;
//	wcstombs_s(&len, strTemp, sizeof(strTemp), pCounter->pName, _tcslen(pCounter->pName));
//#else
	strcpy_s(strTemp, sizeof(strTemp), pCounter->szCounterName);
//#endif
	m_pD3d->DrawText(m_ClientRect, Size, tmpPoint, strTemp, alpha, pCounter->uiColor, fScalor);
	sprintf_s(strTemp, _countof(strTemp), "%d", pCounter->iCounterResult);
	tmpPoint.y += (LONG)(fScalor*30);
	m_pD3d->DrawText(m_ClientRect, Size, tmpPoint, strTemp, alpha, pCounter->uiColor, fScalor);

	if (pCounter->uiStatus & VCA5_APP_AREA_STATUS_SELECTED) {
		alpha = 128;

		for (ULONG i = 0; i < Polygon.cnt_point; ++i)	{
			m_pD3d->DrawCircle(m_ClientRect, Size, Polygon.atpoint[i], 6, 1, alpha, pCounter->uiColor);
			m_pD3d->DrawCircle(m_ClientRect, Size, Polygon.atpoint[i], 4, 1, alpha, RGB(255,255,255) );
		}
	}
}


void	CVCAD3DRender::DrawBoundinxBox(RECT rcVCAROI, VCA5_RECT	bBox, BYTE alpha, COLORREF color)
{
	CD3d::MYPOLYGON	Bound[4];
	LONG Xmin;
	LONG Ymin;
	LONG Xmax;
	LONG Ymax;

	RECT rcVCAROI1(rcVCAROI);
	//if( rcVCAROI1.Size() == SIZE(0, 0) ) {
	if (rcVCAROI1.bottom - rcVCAROI1.top == 0 || rcVCAROI1.right - rcVCAROI1.left) {
		//rcVCAROI1.SetRect(0, 0, m_SourceSize.cx, m_SourceSize.cy);
		rcVCAROI1.left = 0;
		rcVCAROI1.top = 0;
		rcVCAROI1.bottom = m_SourceSize.cy;
		rcVCAROI1.right = m_SourceSize.cx;
	}
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top;
	/*
	PERCENTTOPIXEL( Xmin, (bBox.x - bBox.w/2), rcVCAROI1.Width());
	PERCENTTOPIXEL( Ymin, (bBox.y - bBox.h/2), rcVCAROI1.Height());
	PERCENTTOPIXEL( Xmax, (bBox.x + bBox.w/2), rcVCAROI1.Width());
	PERCENTTOPIXEL( Ymax, (bBox.y + bBox.h/2), rcVCAROI1.Height());*/
	PERCENTTOPIXEL(Xmin, (bBox.x - bBox.w / 2), width);
	PERCENTTOPIXEL(Ymin, (bBox.y - bBox.h / 2), height);
	PERCENTTOPIXEL(Xmax, (bBox.x + bBox.w / 2), width);
	PERCENTTOPIXEL(Ymax, (bBox.y + bBox.h / 2), height);


	Xmin = max(0, Xmin);
	Ymin = max(0, Ymin);
	Xmax = min(width-1, Xmax);
	Ymax = min(height-1, Ymax);
	
	Xmin += rcVCAROI1.left;
	Ymin += rcVCAROI1.top;
	Xmax += rcVCAROI1.left;
	Ymax += rcVCAROI1.top;

	Bound[0].atpoint[0].x  = Xmin;
	Bound[0].atpoint[0].y  = Ymin;
	Bound[0].atpoint[1].x  = Xmax;
	Bound[0].atpoint[1].y  = Ymin;
	Bound[1].atpoint[0].x  = Xmax;
	Bound[1].atpoint[0].y  = Ymin;
	Bound[1].atpoint[1].x  = Xmax;
	Bound[1].atpoint[1].y  = Ymax;
	Bound[2].atpoint[0].x  = Xmin;
	Bound[2].atpoint[0].y  = Ymax;
	Bound[2].atpoint[1].x  = Xmax;
	Bound[2].atpoint[1].y  = Ymax;
	Bound[3].atpoint[0].x  = Xmin;
	Bound[3].atpoint[0].y  = Ymin;
	Bound[3].atpoint[1].x  = Xmin;
	Bound[3].atpoint[1].y  = Ymax;

	for (DWORD k = 0; k < 4; ++k){
		m_pD3d->DrawLineList(m_ClientRect, m_SourceSize, &Bound[k].atpoint[0], 2, 0xFF, color);
	}
}



void	CVCAD3DRender::DrawVCAData(RECT rcVCAROI, VCA5_PACKET_OBJECTS *pObjects, VCA5_PACKET_EVENTS *pEvents, VCA5_PACKET_COUNTS *pCounters)
{

	// draw target's bound box
	for (DWORD i = 0; i < pObjects->ulTotalObject ; ++i) {
		VCA5_PACKET_OBJECT *pObject = &pObjects->Objects[i];
		COLORREF Color = RGB(0xFF, 0xFF, 0);

		// is target notified?
		BOOL bIsEventedObject = FALSE;
		for (DWORD j = 0; j < pEvents->ulTotalEvents ; ++j) {
			if (pObject->ulId == pEvents->Events[j].ulObjId) {
				Color = RGB(0xFF, 0, 0);
				bIsEventedObject = TRUE;
				break;
			}
		}
		
		// don't draw yellow box if DISPLAY_NON_ALARMS menu isn't checked, and if it's not AlarmedObject
		DWORD DisplayFlags = m_pDataMgr->GetDisplayFlags();
		if( !(DisplayFlags & IVCARender::DISPLAY_NON_ALARMS) && !(bIsEventedObject) ) {
			continue;
		}

		DrawBoundinxBox(rcVCAROI, pObject->bBox, 0xFF, Color);

		DrawColorSignature(rcVCAROI, pObject, 0xFF, Color);

		DrawObjectInformation(rcVCAROI, pObject, 0xFF, Color);

		DrawTrail(rcVCAROI, pObject, 0xFF, Color, m_pDataMgr->GetDrawTrailMode() );

		//if(DisplayFlags & IVCARender::DISPLAY_TARGETID) {
			DrawTargetID(rcVCAROI, pObject, Color);
		//}
	}


	// draw target's VCApresence event box
//	if((pObjects->ulTotalObject == 0) && pEvents->ulTotalEvents){
	//when Disappear did not display so redraw bounding box
	
	for (DWORD i = 0; i < pEvents->ulTotalEvents ; ++i) {
		if(VCA5_RULE_TYPE_DISAPPEAR == pEvents->Events[i].ulRuleId){
			COLORREF Color = Color = RGB(0xFF, 0, 0);
			DrawBoundinxBox(rcVCAROI, pEvents->Events[i].bBox, 0xFF, Color);
		}
	}
	

//	}
}


void CVCAD3DRender::DrawColorSignature(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF color)
{
	const unsigned int uiDisplayFlags = m_pDataMgr->GetDisplayFlags();
	float colSigSum = 0.0;

	if( ! ( uiDisplayFlags & IVCARender::DISPLAY_OBJECT_COLOR_SIGNATURE ) )
		return;

	for( int n = 0; n < VCA5_APP_PALETTE_SIZE; ++n)
		colSigSum += (float) pObject->colSigs[n];

	if ( colSigSum <= 0.0 )
		return;

	POINT ptBoundaryLeftTop;
	const SIZE canvasSize = m_pD3d->GetCurrent3DCanvasSize();
	RECT rcVCAROI1 = rcVCAROI;
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top;

	if ((!width) || !height) {
		//rcVCAROI1 m_SourceSize.cx, m_SourceSize.cy);
		rcVCAROI1.right = m_SourceSize.cx;
		rcVCAROI1.bottom = m_SourceSize.cy;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	}

	PERCENTTOPIXEL(ptBoundaryLeftTop.y, (pObject->bBox.y - pObject->bBox.h / 2), width);
	PERCENTTOPIXEL(ptBoundaryLeftTop.x, (pObject->bBox.x + pObject->bBox.w / 2), height);

	ptBoundaryLeftTop.y = max(0, ptBoundaryLeftTop.y);
	ptBoundaryLeftTop.x = min(width-1, ptBoundaryLeftTop.x);

	ptBoundaryLeftTop.y += rcVCAROI1.top;
	ptBoundaryLeftTop.x += rcVCAROI1.left;

	long circleRadius;
	POINT circleCentre;
	static const float fRadiusRateOnSource = 0.05f;
	const float fScaleX = (float)canvasSize.cx / m_SourceSize.cx;
	const float fScaleY = (float)canvasSize.cy / m_SourceSize.cy;

	circleRadius = LONG( min( m_SourceSize.cx, m_SourceSize.cy ) * fRadiusRateOnSource );
	circleCentre.x = ptBoundaryLeftTop.x + circleRadius;
	circleCentre.y = ptBoundaryLeftTop.y - circleRadius;

	//First draw connecting lines between bounding box a colour sig pie chart
	static const float fDiagonalOfSquare = 1.41421356237f;
	static const float fOutsideOfCircle = fDiagonalOfSquare - 1;
	const float fConnectorLength = circleRadius * fOutsideOfCircle;
	POINT pieConnector[2];

	pieConnector[0].x = ptBoundaryLeftTop.x;
	pieConnector[0].y = ptBoundaryLeftTop.y;
	pieConnector[1].x = LONG( ptBoundaryLeftTop.x + fConnectorLength );
	pieConnector[1].y = LONG( ptBoundaryLeftTop.y - fConnectorLength );

	circleCentre.x = LONG( circleCentre.x * fScaleX );
	circleCentre.y = LONG( circleCentre.y * fScaleY );
	circleRadius = LONG( circleRadius * min( fScaleX, fScaleY ) );

	pieConnector[0].x = LONG( pieConnector[0].x * fScaleX );
	pieConnector[0].y = LONG( pieConnector[0].y * fScaleY );
	pieConnector[1].x = LONG( pieConnector[1].x * fScaleX );
	pieConnector[1].y = LONG( pieConnector[1].y * fScaleY );

	m_pD3d->DrawLineList( m_ClientRect, canvasSize, pieConnector, 2, alpha, color );


	//Now draw pie chart itself
	int pies[VCA5_APP_PALETTE_SIZE];
	static const float fDegreeOfTotalAngle = 360.0f;
	float colSigRunningTotal = 0.0;
	static const COLORREF  colourpies[VCA5_APP_PALETTE_SIZE] = {
		RGB(0,0,0),			RGB(150,75,0),		RGB(100,100,100),
		RGB(0,0,200),		RGB(0,150,0),		RGB(0,255,255),
		RGB(255,0,0),		RGB(200,0,200),		RGB(255,255,0),
		RGB(255,255,255)
	};

	for(int n=0; n<VCA5_APP_PALETTE_SIZE-1; ++n)
	{
		colSigRunningTotal += (float)pObject->colSigs[n] / colSigSum; 
		pies[n] = (int) (colSigRunningTotal * fDegreeOfTotalAngle);
	}
	
	pies[VCA5_APP_PALETTE_SIZE-1] = (LONG)fDegreeOfTotalAngle;


	CD3d::MYPOLYGON circlePolygon;
	int piestartpoint = 0;
	POINT pieOutLine[73];

	for(int col=0; col<VCA5_APP_PALETTE_SIZE; col++)
	{
		//draw pie chart slices in 5 degree segments
		int piefinishpoint = pies[col]/5;
		int numsteps = piefinishpoint - piestartpoint;

		// if a pie crosses half way point, then split into two for drawing
		if(piefinishpoint>72&&piestartpoint<72)
		{
			piefinishpoint=72;
			numsteps = piefinishpoint-piestartpoint;
			col--;	
		}

		circlePolygon.cnt_point = numsteps + 2;
		circlePolygon.atpoint[0].x = circleCentre.x;
		circlePolygon.atpoint[0].y = circleCentre.y;

		int nextPoint = 1;

		for( int n=piestartpoint; n<=piefinishpoint; ++n )
		{
			static const float fRadianPerDegree = 0.0174532925f;
			const float fRadianOfAngle = 5*n*fRadianPerDegree;
			POINT &circlePt = circlePolygon.atpoint[nextPoint];

			circlePt.x = LONG(circleCentre.x - sinf(fRadianOfAngle) * circleRadius);
			circlePt.y = LONG(circleCentre.y + cosf(fRadianOfAngle) * circleRadius);
			pieOutLine[nextPoint-1] = circlePt;
			++nextPoint;
		}

		m_pD3d->DrawPolygonList( m_ClientRect, canvasSize,
			&circlePolygon, 1, CD3d::BRT_SOLID, alpha >> 1, colourpies[col] );

		m_pD3d->DrawLineList( m_ClientRect, canvasSize,
			pieOutLine, nextPoint-1, alpha, color );
		
		piestartpoint = piefinishpoint;
	}
}


void	CVCAD3DRender::DrawTargetID(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, COLORREF color)
{
	POINT	ptStartXY;
	char	string[8];

	RECT rcVCAROI1(rcVCAROI);
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top;

	if ((!width) || !height) {
		//rcVCAROI1.CopyRect(&m_ClientRect);
		rcVCAROI1.top = 0;
		rcVCAROI1.left = 0;
		rcVCAROI1.bottom = m_SourceSize.cy;
		rcVCAROI1.right = m_SourceSize.cx;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	}

	PERCENTTOPIXEL( ptStartXY.x, (pObject->bBox.x - pObject->bBox.w/2), width);
	PERCENTTOPIXEL( ptStartXY.y, (pObject->bBox.y - pObject->bBox.h/2), height);

	ptStartXY.x += rcVCAROI.left;
	ptStartXY.y += rcVCAROI.top;

	height = m_ClientRect.bottom - m_ClientRect.top;
	float	fScalor = 0.0f;
	UINT uiFontSize = 0;
	UINT uiFontSizeRemain = 0;	

	GetTextScalingFactor(height, fScalor, uiFontSize, uiFontSizeRemain);

	ptStartXY.x -= (LONG)(fScalor * 3.0f);
	ptStartXY.y -= (LONG)(uiFontSizeRemain);

	sprintf_s( string, sizeof(string), "%d", pObject->ulId );

	m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, 128, color, fScalor+0.2f );
}

BOOL	CVCAD3DRender::GetTextScalingFactor(int height, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
{
	if( height<120 || height>920 ) {
		fScalor = 0.5f;
		uiFontSize = 40;
		uiFontSizeRemain = 10;
		return FALSE; // unsupported image height
	}

	int nScaledHeight = (height-20)/100;

	if(m_SourceSize.cy <= 288) {
		GetTextSFBelowHalfD1Height(nScaledHeight, fScalor, uiFontSize, uiFontSizeRemain);
	}
	else {
		GetTextSFAboveHalfD1Height(nScaledHeight, fScalor, uiFontSize, uiFontSizeRemain);
	}
	
	return TRUE;
}

void	CVCAD3DRender::GetTextSFBelowHalfD1Height(int nScaledHeight, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
{
	switch(nScaledHeight)
	{
	case 1:
		fScalor = 0.5f;
		uiFontSize = 10;
		uiFontSizeRemain = 4;
		break;
	case 2:
		fScalor = 0.55f;
		uiFontSize = 10;
		uiFontSizeRemain = 3;
		break;
	case 3:
		fScalor = 0.6f;
		uiFontSize = 7;
		uiFontSizeRemain = 3;
		break;
	case 4:
		fScalor = 0.65f;
		uiFontSize = 7;
		uiFontSizeRemain = 3;
		break;
	case 5:
		fScalor = 0.7f;
		uiFontSize = 6;
		uiFontSizeRemain = 2;
		break;
	case 6:
		fScalor = 0.75f;
		uiFontSize = 6;
		uiFontSizeRemain = 2;
		break;
	case 7:
		fScalor = 0.8f;
		uiFontSize = 6;
		uiFontSizeRemain = 2;
		break;
	case 8:
		fScalor = 0.85f;
		uiFontSize = 5;
		uiFontSizeRemain = 1;
		break;
	case 9:
		fScalor = 0.9f;
		uiFontSize = 4;
		uiFontSizeRemain = 1;
		break;
	}
}

void	CVCAD3DRender::GetTextSFAboveHalfD1Height(int nScaledHeight, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
{
	switch(nScaledHeight)
	{
	case 1:
		fScalor = 0.5f;
		uiFontSize = 30;
		uiFontSizeRemain = 10;
		break;
	case 2:
		fScalor = 0.55f;
		uiFontSize = 20;
		uiFontSizeRemain = 7;
		break;
	case 3:
		fScalor = 0.6f;
		uiFontSize = 20;
		uiFontSizeRemain = 7;
		break;
	case 4:
		fScalor = 0.65f;
		uiFontSize = 15;
		uiFontSizeRemain = 5;
		break;
	case 5:
		fScalor = 0.7f;
		uiFontSize = 15;
		uiFontSizeRemain = 5;
		break;
	case 6:
		fScalor = 0.75f;
		uiFontSize = 13;
		uiFontSizeRemain = 5;
		break;
	case 7:
		fScalor = 0.8f;
		uiFontSize = 13;
		uiFontSizeRemain = 5;
		break;
	case 8:
		fScalor = 0.85f;
		uiFontSize = 13;
		uiFontSizeRemain = 3;
		break;
	case 9:
		fScalor = 0.9f;
		uiFontSize = 10;
		uiFontSizeRemain = 3;
		break;
	}
}

static int GetInfoNumToShow(DWORD dwDisplayFlags, int calibrationStatus)
{
	int nCntInfoNumToShow = 0;
	if((dwDisplayFlags & IVCARender::DISPLAY_OBJECT_HEIGHT)&&(calibrationStatus != VCA5_CALIB_STATUS_CALIBRATED_OVERHEAD)  ){
		++nCntInfoNumToShow;
	}
	if(dwDisplayFlags & IVCARender::DISPLAY_OBJECT_SPEED){
		++nCntInfoNumToShow;
	}
	if(dwDisplayFlags & IVCARender::DISPLAY_OBJECT_AREA){
		++nCntInfoNumToShow;
	}
	
	return nCntInfoNumToShow;	// not include DISPLAY_OBJECT_CLASSIFICATION
}

static void AdjustStartXY(POINT &ptStartXY, float	fScalor, UINT uiFontSize, UINT uiFontSizeRemain, DWORD dwDisplayFlags, int calibrationStatus)
{
	POINT	ptOrgStartXY = ptStartXY;
	ptStartXY.x -= (LONG)(fScalor * 4.0f);
	ptStartXY.y -= uiFontSize + uiFontSizeRemain;

	int nCntInfoNumToShow = GetInfoNumToShow(dwDisplayFlags, calibrationStatus);

	int topPntObjMsg = ptStartXY.y - (nCntInfoNumToShow*uiFontSize);
	if(topPntObjMsg <= 0) {
		ptStartXY = ptOrgStartXY;
		ptStartXY.y += nCntInfoNumToShow*uiFontSize;
	}
}

void	CVCAD3DRender::DrawObjectInformation(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color)
{
	char	string[1024] = {0,};
	
	int height = m_ClientRect.bottom - m_ClientRect.top;
	float	fScalor = 0.0f;
	UINT uiFontSize = 0;
	UINT uiFontSizeRemain = 0;	

	GetTextScalingFactor(height, fScalor, uiFontSize, uiFontSizeRemain);

	LONG Xmin, Ymin;
	//CRect rcVCAROI1(rcVCAROI);
	RECT rcVCAROI1(rcVCAROI);
	int width = rcVCAROI1.right - rcVCAROI1.left;
	height = rcVCAROI1.bottom - rcVCAROI1.top;

	if ((!width) || !height) {
		//rcVCAROI1.CopyRect(&m_ClientRect);
		rcVCAROI1.top=0;
		rcVCAROI1.left = 0;
		rcVCAROI1.bottom = m_SourceSize.cy;
		rcVCAROI1.right = m_SourceSize.cx;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	}
//	if( rcVCAROI1.Size() == CSize(0, 0) ) {
	//	rcVCAROI1.SetRect(0, 0, m_SourceSize.cx, m_SourceSize.cy);
	//}

	PERCENTTOPIXEL( Xmin, (pObject->bBox.x - pObject->bBox.w/2), width);
	PERCENTTOPIXEL( Ymin, (pObject->bBox.y - pObject->bBox.h/2), height);
	
	Xmin = max(0, Xmin);
	Ymin = max(0, Ymin);
	Xmin += rcVCAROI1.left;
	Ymin += rcVCAROI1.top;
	
	POINT	ptStartXY = {Xmin, Ymin};

	DWORD dwDisplayFlags = m_pDataMgr->GetDisplayFlags();
	int calibrationStatus = m_pDataMgr->GetCalibInfo()->calibrationStatus;
	AdjustStartXY(ptStartXY, fScalor, uiFontSize, uiFontSizeRemain, dwDisplayFlags, calibrationStatus);

	char* sSpeedUnits;
	char* sHeightUnits;
	char* sAreaUnits;

	VCA5_CALIB_INFO *pCalibInfo = m_pDataMgr->GetCalibInfo();
	if( VCA5_HEIGHT_UNITS_METERS == pCalibInfo->ulHeightUnits ){
		sHeightUnits = "m";
		sAreaUnits = "sqm";
	}
	else{
		sHeightUnits = "ft";
		sAreaUnits = "sqft";
	}

	if( VCA5_SPEED_UNITS_KPH == pCalibInfo->ulSpeedUnits ){
		sSpeedUnits = "km/h";
	}
	else{
		sSpeedUnits = "mph";
	}

	//Do not display Object information if calibaraion does not work. 
	if(calibrationStatus <= 0) return;

	if((m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_OBJECT_HEIGHT)&&(calibrationStatus != VCA5_CALIB_STATUS_CALIBRATED_OVERHEAD)  ){
		if( 0xFFFF == pObject->ulCalibHeight )
		{
			sprintf_s( string, _countof(string), "---" );
		}
		else
		{
			sprintf_s( string, _countof(string), "%1.1f %s", (float)pObject->ulCalibHeight/10.0f, sHeightUnits);	
		}
		m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color,fScalor);
		ptStartXY.y -= uiFontSize;
	}
	if(m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_OBJECT_SPEED){
		if( 0xFFFF == pObject->ulCalibSpeed )
		{
			sprintf_s( string, _countof(string), "---" );
		}
		else
		{
			sprintf_s( string, _countof(string), "%d  %s", (int)pObject->ulCalibSpeed, sSpeedUnits);
		}

		m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color,fScalor);
		ptStartXY.y -= uiFontSize;
	}
	if(m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_OBJECT_AREA){
		if( 0xFFFF == pObject->ulCalibArea )
		{
			sprintf_s( string, _countof(string), "---" );
		}
		else
		{
			float fArea = (float)pObject->ulCalibArea;
			if( VCA5_HEIGHT_UNITS_METERS == pCalibInfo->ulHeightUnits )
			{
				fArea /= 10.0f;
			}

			sprintf_s( string, _countof(string), "%.1f  %s", fArea, sAreaUnits);
		}

		m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color,fScalor);
		ptStartXY.y -= uiFontSize;
	}
	if(m_pDataMgr->GetDisplayFlags() & IVCARender::DISPLAY_OBJECT_CLASSIFICATION){
		if(pObject->iClassificationId == -1){ // CLSOBJECT_UNCLASSIFIED = -1
			sprintf_s( string, _countof(string), "%s", "Unclassified");
			m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color,fScalor);
		}else if(pObject->iClassificationId == -2){ // CLSOBJECT_UNKNOWN = -2
			sprintf_s( string, _countof(string), "%s", "Unknown");
			m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color,fScalor);
		}else if(-1 <= pObject->iClassificationId && pObject->iClassificationId < VCA5_MAX_NUM_CLSOBJECTS){
			USES_CONVERSION;
			VCA5_APP_CLSOBJECT*	pClsObject = m_pDataMgr->GetClsObject(pObject->iClassificationId);
			if(pClsObject){
				sprintf_s( string, _countof(string), "%s", pClsObject->szClsobjectName);
				m_pD3d->DrawText( m_ClientRect, m_SourceSize, ptStartXY, string, alpha, Color, fScalor);
			}
		}
	}

}


void	CVCAD3DRender::DrawTrail(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color, BOOL bFromCenter)
{
	CD3d::MYPOLYGON ptTrailLineList[VCA5_MAX_NUM_TRAIL_POINTS+1];	//-- Trail

	unsigned int uiTrailX[ VCA5_MAX_NUM_TRAIL_POINTS + 1];	
	unsigned int uiTrailY[ VCA5_MAX_NUM_TRAIL_POINTS + 1];	
	int i;

	LONG Xmin, Xmax, Ymin, Ymax;
	//CRect rcVCAROI1(rcVCAROI);

	RECT rcVCAROI1(rcVCAROI);
	int width = rcVCAROI1.right - rcVCAROI1.left;
	int height = rcVCAROI1.bottom - rcVCAROI1.top;

	if ((!width) || !height) {
		//rcVCAROI1.CopyRect(&m_ClientRect);
		rcVCAROI1.top = 0;
		rcVCAROI1.left = 0;
		rcVCAROI1.bottom = m_SourceSize.cy;
		rcVCAROI1.right = m_SourceSize.cx;
		width = rcVCAROI1.right - rcVCAROI1.left;
		height = rcVCAROI1.bottom - rcVCAROI1.top;
	}

	PERCENTTOPIXEL( Xmin, (pObject->bBox.x - pObject->bBox.w/2), width);
	PERCENTTOPIXEL( Xmax, (pObject->bBox.x + pObject->bBox.w/2),width);
	PERCENTTOPIXEL( Ymin, (pObject->bBox.y - pObject->bBox.h/2),height);
	PERCENTTOPIXEL( Ymax, (pObject->bBox.y + pObject->bBox.h/2),height);

	Xmin = max(0, Xmin);
	Xmax = min(width-1, Xmax);
	Ymax = min(height-1, Ymax);

	Xmin += rcVCAROI.left;
	Xmax += rcVCAROI.left;
	Ymax += rcVCAROI.top;

	LONG Ystart = bFromCenter ? (Ymin + Ymax) / 2 : Ymax;
	POINT	ptStartXY = {(Xmin + Xmax) / 2, Ystart};

	for( i = 0; i < pObject->trail.usNumTrailPoints; i++ )
	{
		PERCENTTOPIXEL( uiTrailX[i], pObject->trail.trailPoints[i].x, width );
		PERCENTTOPIXEL( uiTrailY[i], pObject->trail.trailPoints[i].y, height );
		uiTrailX[i] += rcVCAROI1.left;
		uiTrailY[i] += rcVCAROI1.top;
	}

	//	Start point of the trail
	uiTrailX[pObject->trail.usNumTrailPoints] = ptStartXY.x;
	uiTrailY[pObject->trail.usNumTrailPoints] = ptStartXY.y;
	
	//	Construct all the lineList (No. = MAX_NUM_TRAIL_POINTS - 1)
	for ( i = 0; i < pObject->trail.usNumTrailPoints; i++ )
	{
		ptTrailLineList[i].atpoint[0].x = uiTrailX[i];
		ptTrailLineList[i].atpoint[0].y = uiTrailY[i];
		ptTrailLineList[i].atpoint[1].x = uiTrailX[i + 1];
		ptTrailLineList[i].atpoint[1].y = uiTrailY[i + 1];
	}
	
	//	Draw the trails
	for ( i = 0; i < pObject->trail.usNumTrailPoints; i++ )
	{
		m_pD3d->DrawLineList( m_ClientRect, m_SourceSize, ptTrailLineList[i].atpoint, 2, alpha, Color );
	}

}

void	CVCAD3DRender::DrawVCAHeaderMsg(VCA5_PACKET_HEADER* pHeader)
{
	// Draw a mesage in the bottom left corner
	POINT ptStartXY;
	char* sMsg = NULL;
	SIZE	srcImage = {m_ClientRect.right-m_ClientRect.left, m_ClientRect.bottom-m_ClientRect.top};
	COLORREF color;

	ptStartXY.x = 10;
	ptStartXY.y = srcImage.cy-20;
	
	switch(pHeader->ulVCAStatus){
		case VCA5_PACKET_STATUS_LEARNING_SCENE:
			sMsg = "Learning scene...";
			color = RGB( 0xff, 0xff, 0xff );
			break;
		case VCA5_PACKET_STATUS_UNLICENSED:
			sMsg = "VCA not licensed!";
			color = RGB( 0xff, 0x00, 0x00 );
			break;
		case VCA5_PACKET_STATUS_LICENSE_EXPIRED:
			sMsg = "VCA license expired!";
			color = RGB( 0xff, 0x00, 0x00 );
			break;
	}

	if(sMsg)
		DrawVCAMsg(sMsg, color, ptStartXY, TEXT_ALIGN_CENTRE_VERTICAL);
}

void CVCAD3DRender::DrawBlobMap(RECT rcCanvas, VCA5_PACKET_BLOBMAP *pBlobmap, VCA_RENDER_COLORS color)
{
	//-----------------------------------
	// Draw the blob map if necessary
	RECT	rcList[1024];
	unsigned int uiClientWidth, uiClientHeight;
	
	uiClientWidth = m_ClientRect.right - m_ClientRect.left;
	uiClientHeight = m_ClientRect.bottom - m_ClientRect.top;
	SIZE	sizeImage = { uiClientWidth, uiClientHeight };

	for( ULONG y = 0; y < pBlobmap->ulHeight; y++ ){
		RECT *pRect = rcList;
		int iNumRects = 0;

		for( ULONG x = 0; x < pBlobmap->ulWidth; x++ ){
			if( pBlobmap->pBlobMap[ (y * pBlobmap->ulWidth) + x ] ){
				pRect->left = (x * uiClientWidth)/pBlobmap->ulWidth;
				pRect->top = (y * uiClientHeight)/pBlobmap->ulHeight;
				pRect->right = ((x+1) * uiClientWidth)/pBlobmap->ulWidth;
				pRect->bottom = ((y+1) * uiClientHeight)/pBlobmap->ulHeight;

				iNumRects++;
				pRect++;
			}
		}
		m_pD3d->DrawRectList( rcCanvas, sizeImage, rcList, iNumRects, 128, m_crRender[color] );
	}
}


void CVCAD3DRender::SetCanvasSize( RECT* pCanvas, RECT* pVCAROI )
{
	if(!pVCAROI) {
		unsigned int uiClientWidth, uiClientHeight;
		unsigned int uiCanvasWidth, uiCanvasHeight;

		uiClientWidth = m_ClientRect.right - m_ClientRect.left;
		uiClientHeight = m_ClientRect.bottom - m_ClientRect.top;

		uiCanvasWidth  = m_pD3d->GetCurrent3DCanvasSize().cx;	
		uiCanvasHeight = m_pD3d->GetCurrent3DCanvasSize().cy;	

		if ( ( uiClientWidth  <= uiCanvasWidth  ) &&
			( uiClientHeight <= uiCanvasHeight ) )	{
				SetRect( pCanvas, 0, 0, uiClientWidth, uiClientHeight );
		}else{
			float ratio1, ratio2;

			ratio1 = (float) uiClientWidth  / uiCanvasWidth;
			ratio2 = (float) uiClientHeight / uiCanvasHeight;

			if ( ratio1 > ratio2 )		{
				SetRect( pCanvas, 0, 0, uiCanvasWidth, (int)((float) uiClientHeight / ratio1) );
			}
			else		{
				SetRect( pCanvas, 0, 0, (int)((float) uiClientWidth / ratio2) , uiCanvasHeight );
			}
		}
	} else {
		RECT rcVCAROI1 ( *pVCAROI);
		int width = rcVCAROI1.right - rcVCAROI1.left;
		int height = rcVCAROI1.bottom - rcVCAROI1.top;

		if ((!width) || !height) {
			//rcVCAROI1.CopyRect(&m_ClientRect);
			rcVCAROI1 = m_ClientRect;
			width = rcVCAROI1.right - rcVCAROI1.left;
			height = rcVCAROI1.bottom - rcVCAROI1.top;
		} else {
			rcVCAROI1.right = min(rcVCAROI1.right-1, m_SourceSize.cx);
			rcVCAROI1.bottom = min(rcVCAROI1.bottom-1, m_SourceSize.cy);
			PIXELTOPERCENT(rcVCAROI1.left, rcVCAROI1.left, m_SourceSize.cx);
			PIXELTOPERCENT(rcVCAROI1.top, rcVCAROI1.top, m_SourceSize.cy);
			PIXELTOPERCENT(rcVCAROI1.right, rcVCAROI1.right, m_SourceSize.cx);
			PIXELTOPERCENT(rcVCAROI1.bottom, rcVCAROI1.bottom, m_SourceSize.cy);

			PERCENTTOPIXEL(rcVCAROI1.left, rcVCAROI1.left, m_ClientRect.right);
			PERCENTTOPIXEL(rcVCAROI1.top, rcVCAROI1.top, m_ClientRect.bottom);
			PERCENTTOPIXEL(rcVCAROI1.right, rcVCAROI1.right, m_ClientRect.right);
			PERCENTTOPIXEL(rcVCAROI1.bottom, rcVCAROI1.bottom, m_ClientRect.bottom);
		}
		*pCanvas = rcVCAROI1;
	}
	
}

void CVCAD3DRender::DrawDirectionArrow(CD3d *pD3D, int x, int y, int startangle, int finishangle)
{
	int		x1, y1;
	double	start, finish, middle;
	double	SweepAngle, StartAngle;
	RECT	rc3DCanvas;
	POINT	ptCentre;
	POINT	pt, pt1;

	RECT rcCanvas;
	SetCanvasSize(&rcCanvas);

	SIZE	sizeImage = { m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top };
	SetRect(&rc3DCanvas, 0, 0, rcCanvas.right - rcCanvas.left, rcCanvas.bottom - rcCanvas.top);

	if (finishangle > startangle)
		SweepAngle = finishangle - startangle;
	else
		SweepAngle = finishangle - startangle + 3600;

	SweepAngle /= 10;
	StartAngle = 360.0f - finishangle* 1.0f / 10.0f;

	start = (double)startangle  * PI / 1800.0f;
	finish = (double)finishangle * PI / 1800.0f;

	if (finish > start)
		middle = (start + finish) / 2;
	else
		middle = (start + 2 * PI + finish) / 2;

	//_______________________________________________________
	//	I. Draw the cicular arc
	ptCentre.x = x;
	ptCentre.y = y;

	//pD3D->DrawCircularArc( rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 5, start, finish, 192, RGB(  0,   0,   0) );
	//pD3D->DrawCircularArc( rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 3, start, finish, 192, RGB(255, 255, 255) );

	//_______________________________________________________
	//	II. Draw one of the small arrow

	x1 = (int)(x + floor(ARROW_LENGTH * cos(start) + 0.5));
	y1 = (int)(y - floor(ARROW_LENGTH * sin(start) + 0.5));

	pt.x = x; pt.y = y;
	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowBottom.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowBottom.SetColor(150, RGB(255, 255, 255));
	if ((finishangle - startangle) > 1900)
	{
		DrawArrow(m_pDataMgr->m_cArrowBottom.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowBottom.m_bColour, m_pDataMgr->m_cArrowBottom.m_bAlpha);
	}

	//_______________________________________________________
	//	III. Draw the other of the small arrow
	x1 = (int) (x + ARROW_LENGTH * cos( finish ));
	y1 = (int) (y - ARROW_LENGTH * sin( finish ));
	
	pt1.x = x1; pt1.y = y1; 
	m_pDataMgr->m_cArrowTop.SetPosition( pt, pt1 );
	m_pDataMgr->m_cArrowTop.SetColor( 150, RGB(255, 255, 255) );
	//DrawArrow( m_pDataMgr->m_cArrowTop.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowTop.m_bColour, m_pDataMgr->m_cArrowTop.m_bAlpha );

	//_______________________________________________________
	//	IV. Draw the middle big arrow
	x1 = (int) (x + ARROW_LENGTH  * cos( middle ));
	y1 = (int) (y - ARROW_LENGTH * sin( middle ));
	
	pt1.x = x1; pt1.y = y1; 
	m_pDataMgr->m_cArrowMiddle.SetPosition( pt, pt1 );
	m_pDataMgr->m_cArrowMiddle.SetColor( 192, RGB(255, 255, 255) );
	DrawArrow( m_pDataMgr->m_cArrowMiddle.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowMiddle.m_bColour, m_pDataMgr->m_cArrowMiddle.m_bAlpha );
	
	//_______________________________________________________
	//	V. Draw the central cirle
	//pD3D->DrawCircle( rc3DCanvas, sizeImage, ptCentre, 5, 1, 192, RGB(0  , 0  , 0  ) );
//	pD3D->DrawCircle( rc3DCanvas, sizeImage, ptCentre, 4, 1, 192, RGB(255, 255, 255) );
}

void CVCAD3DRender::DrawDirectionArrow22(CD3d *pD3D, int x, int y, int startangle, int finishangle)
{
	int		x1, y1;
	double	start, finish, middle;
	double	SweepAngle, StartAngle;
	RECT	rc3DCanvas;
	POINT	ptCentre;
	POINT	pt, pt1;

	RECT rcCanvas;
	SetCanvasSize(&rcCanvas);

	SIZE	sizeImage = { m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top };
	SetRect(&rc3DCanvas, 0, 0, rcCanvas.right - rcCanvas.left, rcCanvas.bottom - rcCanvas.top);

	if (finishangle > startangle)
		SweepAngle = finishangle - startangle;
	else
		SweepAngle = finishangle - startangle + 3600;

	SweepAngle /= 10;
	StartAngle = 360.0f - finishangle* 1.0f / 10.0f;

	start = (double)startangle  * PI / 1800.0f;
	finish = (double)finishangle * PI / 1800.0f;

	if (finish > start)
		middle = (start + finish) / 2;
	else
		middle = (start + 2 * PI + finish) / 2;

	//_______________________________________________________
	//	I. Draw the cicular arc
	ptCentre.x = x;
	ptCentre.y = y;

	//pD3D->DrawCircularArc(rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 5, start, finish, 192, RGB(0, 0, 0));
	//pD3D->DrawCircularArc(rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 3, start, finish, 192, RGB(255, 255, 255));

	//_______________________________________________________
	//	II. Draw one of the small arrow

	x1 = (int)(x + floor(ARROW_LENGTH * cos(start) + 0.5));
	y1 = (int)(y - floor(ARROW_LENGTH * sin(start) + 0.5));

	pt.x = x; pt.y = y;
	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowBottom22.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowBottom22.SetColor(150, RGB(255, 255, 255));
	if ((finishangle - startangle) > 1900)
	{
		DrawArrow(m_pDataMgr->m_cArrowBottom22.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowBottom22.m_bColour, m_pDataMgr->m_cArrowBottom22.m_bAlpha);
	}
	//_______________________________________________________
	//	III. Draw the other of the small arrow
	x1 = (int)(x + ARROW_LENGTH * cos(finish));
	y1 = (int)(y - ARROW_LENGTH * sin(finish));

	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowTop22.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowTop22.SetColor(150, RGB(255, 255, 255));
//	DrawArrow(m_pDataMgr->m_cArrowTop22.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowTop22.m_bColour, m_pDataMgr->m_cArrowTop22.m_bAlpha);

	//_______________________________________________________
	//	IV. Draw the middle big arrow
	x1 = (int)(x + ARROW_LENGTH *  cos(middle));
	y1 = (int)(y - ARROW_LENGTH *  sin(middle));

	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowMiddle22.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowMiddle22.SetColor(192, RGB(255, 255, 255));
	DrawArrow(m_pDataMgr->m_cArrowMiddle22.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowMiddle22.m_bColour, m_pDataMgr->m_cArrowMiddle22.m_bAlpha);

	//_______________________________________________________
	//	V. Draw the central cirle
	//pD3D->DrawCircle(rc3DCanvas, sizeImage, ptCentre, 5, 1, 192, RGB(0, 0, 0));
	//pD3D->DrawCircle(rc3DCanvas, sizeImage, ptCentre, 4, 1, 192, RGB(255, 255, 255));
}


void CVCAD3DRender::DrawDirectionArrow33(CD3d *pD3D, int x, int y, int startangle, int finishangle)
{
	int		x1, y1;
	double	start, finish, middle;
	double	SweepAngle, StartAngle;
	RECT	rc3DCanvas;
	POINT	ptCentre;
	POINT	pt, pt1;

	RECT rcCanvas;
	SetCanvasSize(&rcCanvas);

	SIZE	sizeImage = { m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top };
	SetRect(&rc3DCanvas, 0, 0, rcCanvas.right - rcCanvas.left, rcCanvas.bottom - rcCanvas.top);

	if (finishangle > startangle)
		SweepAngle = finishangle - startangle;
	else
		SweepAngle = finishangle - startangle + 3600;

	SweepAngle /= 10;
	StartAngle = 360.0f - finishangle* 1.0f / 10.0f;

	start = (double)startangle  * PI / 1800.0f;
	finish = (double)finishangle * PI / 1800.0f;

	if (finish > start)
		middle = (start + finish) / 2;
	else
		middle = (start + 2 * PI + finish) / 2;

	//_______________________________________________________
	//	I. Draw the cicular arc
	ptCentre.x = x;
	ptCentre.y = y;

	//pD3D->DrawCircularArc(rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 5, start, finish, 192, RGB(0, 0, 0));
//	pD3D->DrawCircularArc(rc3DCanvas, sizeImage, ptCentre, ARROW_LENGTH * 0.7, 3, start, finish, 192, RGB(255, 255, 255));

	//_______________________________________________________
	//	II. Draw one of the small arrow

	x1 = (int)(x + floor(ARROW_LENGTH * cos(start) + 0.5));
	y1 = (int)(y - floor(ARROW_LENGTH *sin(start) + 0.5));

	pt.x = x; pt.y = y;
	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowBottom33.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowBottom33.SetColor(150, RGB(255, 255, 255));
	if ((finishangle - startangle) > 1900)
	{
		DrawArrow(m_pDataMgr->m_cArrowBottom33.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowBottom33.m_bColour, m_pDataMgr->m_cArrowBottom33.m_bAlpha);
	}
	//_______________________________________________________
	//	III. Draw the other of the small arrow
	x1 = (int)(x + ARROW_LENGTH * cos(finish));
	y1 = (int)(y - ARROW_LENGTH * sin(finish));

	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowTop33.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowTop33.SetColor(150, RGB(255, 255, 255));
//	DrawArrow(m_pDataMgr->m_cArrowTop33.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowTop33.m_bColour, m_pDataMgr->m_cArrowTop33.m_bAlpha);

	//_______________________________________________________
	//	IV. Draw the middle big arrow
	x1 = (int)(x + ARROW_LENGTH * 1.5 * cos(middle));
	y1 = (int)(y - ARROW_LENGTH * 1.5 * sin(middle));

	pt1.x = x1; pt1.y = y1;
	m_pDataMgr->m_cArrowMiddle33.SetPosition(pt, pt1);
	m_pDataMgr->m_cArrowMiddle33.SetColor(192, RGB(255, 255, 255));
	DrawArrow(m_pDataMgr->m_cArrowMiddle33.m_pPoints, rc3DCanvas, sizeImage, m_pDataMgr->m_cArrowMiddle33.m_bColour, m_pDataMgr->m_cArrowMiddle33.m_bAlpha);

	//_______________________________________________________
	//	V. Draw the central cirle
//	pD3D->DrawCircle(rc3DCanvas, sizeImage, ptCentre, 5, 1, 192, RGB(0, 0, 0));
//	pD3D->DrawCircle(rc3DCanvas, sizeImage, ptCentre, 4, 1, 192, RGB(255, 255, 255));
}

void CVCAD3DRender::GetZonePie( VCA5_APP_ZONE *pZone, VCA5_POINT *pt )
{
	int x=0, y=0;
	
	m_pDataMgr->GetZoneCenter( pZone, pt );
	POINT point;
	point.x = pt->x;
	point.y = pt->y;

	PERCENTTOPIXEL( point.x, point.x,  m_ClientRect.right-m_ClientRect.left );
	PERCENTTOPIXEL( point.y, point.y,  m_ClientRect.bottom-m_ClientRect.top );
	pt->x = (USHORT) point.x;
	pt->y = (USHORT) point.y;
}

void CVCAD3DRender::DrawArrow(POINT *pPoints, RECT _rc3DCanvas, SIZE _sizeSrcImage, COLORREF  color, BYTE alpha)
{
	CD3d::MYPOLYGON tPolygon;
	long lWidth, lHeight;

	lWidth = _rc3DCanvas.right - _rc3DCanvas.left;
	lHeight = _rc3DCanvas.bottom - _rc3DCanvas.top;

	tPolygon.cnt_point = 7;

	for ( int i = 0; i < 7; i ++ )
	{
//		PERCENTTOPIXEL( tPolygon.atpoint[i].x, m_pPoints[i].x, lWidth );
//		PERCENTTOPIXEL( tPolygon.atpoint[i].y, m_pPoints[i].y, lHeight );
		tPolygon.atpoint[i] = pPoints[i];
	}
	m_pD3d->DrawPolygonList( _rc3DCanvas, _sizeSrcImage, &tPolygon, 1, CD3d::BRT_SOLID, alpha, color );


	tPolygon.atpoint[7] = tPolygon.atpoint[0];
//	m_pD3d->m_DrawLineList( _rc3DCanvas, _sizeSrcImage, &tPolygon.atpoint[0], tPolygon.cnt_point+1, alpha, RGB(0,0,0) );
}


void CVCAD3DRender::DrawVCAMsg(LPCSTR lpszMsg, COLORREF color, POINT ptStartXY, unsigned int flags)
{
	// Draw a mesage in the bottom left corner
	SIZE	clientSize = {m_ClientRect.right-m_ClientRect.left, m_ClientRect.bottom-m_ClientRect.top};
	float	fScalor = 0.0f;
	UINT	uiFontSize = 0;
	UINT	uiFontSizeRemain = 0;	

	GetTextScalingFactor(clientSize.cy, fScalor, uiFontSize, uiFontSizeRemain);
	fScalor+=0.3f;

	if (flags){
		SIZE textSize;
		m_pD3d->GetTextExtent( lpszMsg, &textSize);
		if (flags & TEXT_ALIGN_CENTRE){
			textSize.cx = (LONG)(textSize.cx*fScalor);
			//textSize.cx = (ULONG)(fScalor*(float)textSize.cx);
			//ptStartXY.x = (clientSize.cx > textSize.cx)?((clientSize.cx - textSize.cx)/2) : 0; 
			ptStartXY.x = ptStartXY.x < textSize.cx/2 ? 0:ptStartXY.x-textSize.cx/2;
		}
			
		if (flags & TEXT_ALIGN_CENTRE_VERTICAL) ptStartXY.y -= textSize.cy/2;
			
	}

	m_pD3d->DrawText( m_ClientRect, clientSize, ptStartXY, lpszMsg, 0xff, color, fScalor);
}

void CVCAD3DRender::DrawThickLine(RECT *p_rc3DCanvas, POINT ptStart, POINT ptEnd,int alpha,  COLORREF colour,
										 unsigned int uiCanvasDrawWidth, unsigned int uiCanvasDrawHeight, float fLineWidth)
{
	int x1, y1, x2, y2;
	SIZE sizeCanvas;
	sizeCanvas.cx = uiCanvasDrawWidth;
	sizeCanvas.cy = uiCanvasDrawHeight;

	x1 = ptStart.x;
	y1 = ptStart.y;
	x2 = ptEnd.x;
	y2 = ptEnd.y;

	float fTheta = atan2( (float)(y2-y1), (float)(x2-x1) );
	float fx = sin(fTheta);
	float fy = cos(fTheta);
	float fxOff, fyOff;

	fxOff = fLineWidth * fx;
	fyOff = fLineWidth * fy;

	CD3d::MYPOLYGON tripWire;

	tripWire.cnt_point = 4;

	tripWire.atpoint[0].x = (LONG)(x1 - fxOff);
	tripWire.atpoint[0].y = (LONG)(y1 + fyOff);
	tripWire.atpoint[1].x = (LONG)(x1 + fxOff);
	tripWire.atpoint[1].y = (LONG)(y1 - fyOff);

	tripWire.atpoint[2].x = (LONG)(x2 + fxOff);
	tripWire.atpoint[2].y = (LONG)(y2 - fyOff);
	tripWire.atpoint[3].x = (LONG)(x2 - fxOff);
	tripWire.atpoint[3].y = (LONG)(y2 + fyOff);

	m_pD3d->DrawPolygonList( *p_rc3DCanvas, sizeCanvas, &tripWire, 1, CD3d::BRT_SOLID, alpha, colour );
}
