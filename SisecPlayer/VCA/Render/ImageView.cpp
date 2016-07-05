// AlarmViewrDlg.cpp : implementation file
//
#include "stdafx.h"
#include "ImageView.h"
#include "colorconversion.h"
#include "../Render/CD3d.h"
#include "../common/APPConfigure.h"

// CAlarmViewDlg dialog

#define DIR_SNAPSHOTS_A	"snapshots"
#define DIR_SNAPSHOTS_T	_T("snapshots")

static LPCSTR g_strFilterItem[VCA5_RULE_TYPE_NUM-1] =
{
	"Presence",
	"Enter",
	"Exit",
	"Appear",
	"Disappear",
	"Stopped",
	"Dwell",
	"Direction",
	"Speed",
	"Area",
	"Tailgating",
	"Abandone/Remove",
	"Linecounter_A",
	"Linecounter_B",
	"Abandone/Remove",
	"Colour Signiture",
	"Smoke",
	"Fire",
};

static void CleanDirectory(TCHAR* szPath);



CImageView::CImageView()
{
	m_pD3d	= NULL;
	m_nCurImageIdx = 0;
	m_pBuffer = NULL;
	m_dwBufferLength = 0;
	m_pImageBuf	= (BYTE *)_aligned_malloc(MAXWIDTH_D3DCANVAS * MAXHEIGHT_D3DCANVAS * 4, 16);


	m_pD3d = new CD3d();
	if (m_pD3d->Setup(0, CD3d::CF_BGR565, NULL, jpeg.bm.biWidth, jpeg.bm.biHeight, jpeg.bm.biWidth, jpeg.bm.biHeight) != S_OK){
		MessageBox(_T("Can not setup D3D "), _T("ERROR"), MB_OK);
		delete m_pD3d;
		m_pD3d = NULL;
		return FALSE;
	}
	m_pD3d->SetDDClipperWindow(m_hWnd);


//	ShowCurrentImage();
}

CImageView::~CImageView()
{
	if(m_pD3d){
		delete m_pD3d;
		m_pD3d = NULL;
	}
	if (m_dwBufferLength) {
		_aligned_free(m_pBuffer);
		m_pBuffer = NULL;
		m_dwBufferLength = 0;
	}
	if (m_pImageBuf) {
		_aligned_free(m_pImageBuf);
		m_pImageBuf = NULL;
	}
}


void CImageView::DrawImage(int width, int height, BYTE* pImage, BITMAPINFOHEADER bm)
{
	
	SIZE	sourceSize = {width, height};


	RECT rcROI = {0,0,sourceSize.cx, sourceSize.cy};

	m_pD3d->DrawImage(m_ClientRect, sourceSize, CD3d::CF_YUY2, CD3d::RT_000, rcROI, pImage);
}


void CImageView::ShowImage(QString &strPathName)
{
	const CString &Path = strPathName;
	Jpeg jpeg;
	ZeroMemory(&jpeg, sizeof(jpeg));

	if (!m_JpegCodec.GetJpegInformation((LPCTSTR) Path, &jpeg)) return;
	if (m_dwBufferLength < jpeg.dwLength) {
		if (m_pBuffer) _aligned_free(m_pBuffer);
		m_pBuffer = (BYTE *) _aligned_malloc(jpeg.dwLength, 16);
		m_dwBufferLength = jpeg.dwLength;
	}

	jpeg.pBuffer = (BYTE *) m_pBuffer;
	if (!m_JpegCodec.Decode((LPCTSTR) Path, &jpeg)) {
		return;
	}

	int width = (jpeg.bm.biWidth<= 360)?(2*jpeg.bm.biWidth):(jpeg.bm.biWidth);
	int height = (jpeg.bm.biHeight<= 288)?(2*jpeg.bm.biHeight):(jpeg.bm.biHeight);

	DWORD dwEngId = _ttoi(m_strEngId.GetBuffer());

	CRect	rcVCAROI;
	VCA5_RECT	rcROIEng = CAPPConfigure::Instance()->GetAPPEngineInfo( dwEngId )->tSourceData.rcROI;

	rcVCAROI.left	= rcROIEng.x;
	rcVCAROI.top	= rcROIEng.y;
	rcVCAROI.right	= rcROIEng.x + rcROIEng.w;
	rcVCAROI.bottom	= rcROIEng.y + rcROIEng.h;

	if( rcVCAROI.Size() == CSize(0, 0) ) {
		rcVCAROI.CopyRect(&m_ClientRect);
	} else {
		rcVCAROI.right = min(rcVCAROI.right-1, width);
		rcVCAROI.bottom = min(rcVCAROI.bottom-1, height);
		PIXELTOPERCENT(rcVCAROI.left, rcVCAROI.left, width);
		PIXELTOPERCENT(rcVCAROI.top, rcVCAROI.top, height);
		PIXELTOPERCENT(rcVCAROI.right, rcVCAROI.right, width);
		PIXELTOPERCENT(rcVCAROI.bottom, rcVCAROI.bottom, height);

		PERCENTTOPIXEL(rcVCAROI.left, rcVCAROI.left, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI.top, rcVCAROI.top, m_ClientRect.bottom);
		PERCENTTOPIXEL(rcVCAROI.right, rcVCAROI.right, m_ClientRect.right);
		PERCENTTOPIXEL(rcVCAROI.bottom, rcVCAROI.bottom, m_ClientRect.bottom);
	}

	m_ClientRect.left = 0;
	m_ClientRect.top = 0;
	m_ClientRect.right = width;
	m_ClientRect.bottom = height;

	AdjustDlgRect(width, height);
	RelocateControls(width, height);
	
	
	DrawImage(jpeg.bm.biWidth, jpeg.bm.biHeight, jpeg.pImage, jpeg.bm);
	DrawSingleZone(rcVCAROI, jpeg.pZone);
	DrawObject(rcVCAROI, jpeg.bm.biWidth, jpeg.bm.biHeight, jpeg.pObject);
	if(jpeg.pRule) DrawTimeStampAndRuleType(jpeg.bm.biWidth, jpeg.bm.biHeight, jpeg.i64TimeStamp, jpeg.pRule->usRuleType);

	
	RECT	ScreenRect;
	ScreenRect = m_ClientRect;

	ClientToScreen((POINT*)&ScreenRect.left );
	ClientToScreen((POINT*)&ScreenRect.right );

	m_pD3d->PrimaryPresentation(&ScreenRect, &m_ClientRect);
}

void CImageView::DrawSingleZone(RECT rcVCAROI, VCA5_APP_ZONE *pZone)
{
	if(!pZone) return;

	CD3d::MYPOLYGON	Polygon;
	BYTE alpha = 64;
	SIZE Size = {m_ClientRect.right, m_ClientRect.bottom};
	CD3d::eBRUSHTYPE eBrushType = (pZone->usZoneType == VCA5_ZONE_TYPE_NONDETECTION) ? CD3d::BRT_HATCH : CD3d::BRT_SOLID ;

	if (pZone->uiStatus & VCA5_APP_AREA_STATUS_NOTIFIED ||
		pZone->uiStatus & VCA5_APP_AREA_STATUS_SELECTED ) {
			alpha = 128;
	}

	CRect rcVCAROI1(rcVCAROI);

	if( rcVCAROI1.Size() == CSize(0, 0) ) {
		rcVCAROI1.CopyRect(&m_ClientRect);
	}

	Polygon.cnt_point = pZone->ulTotalPoints;
	for (ULONG i = 0; i < Polygon.cnt_point; ++i) {
		PERCENTTOPIXEL(Polygon.atpoint[i].x, pZone->pPoints[i].x, rcVCAROI1.Width());
		PERCENTTOPIXEL(Polygon.atpoint[i].y, pZone->pPoints[i].y, rcVCAROI1.Height());
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


void CImageView::DrawObject(RECT rcCanvas, LONG width, LONG height, VCA5_PACKET_OBJECT *pObject)
{
	if(!pObject) return;

	COLORREF Color = RGB(0xFF, 0, 0);
	SIZE	SourceSize = {width, height};

	DrawBoundinxBox(m_ClientRect, SourceSize, rcCanvas, pObject->bBox, 0xFF, Color);
	
//	if((0 == pObject->ulCalibArea)&& (0 == pObject->ulCalibSpeed)) return;
//	DrawObjectInformation(m_ClientRect, SourceSize, pObject, 0xFF, Color);
}


void CImageView::DrawTimeStampAndRuleType(LONG width, LONG height, __int64 i64TimeStamp, int iruleType)
{
	char Text[128];
	SIZE size = {width, height};
	POINT pt = {0, 0};

	CTime cTime = CTime( (time_t) i64TimeStamp );
	struct tm	osTime;
	cTime.GetLocalTm( &osTime );


	sprintf_s(Text, _countof(Text), "%4d/%02d/%02d %02d:%02d:%02d : RuleType [%s]",
		osTime.tm_year+1900, osTime.tm_mon+1, osTime.tm_mday, osTime.tm_hour, osTime.tm_min, osTime.tm_sec, g_strFilterItem[iruleType-1]);


	m_pD3d->DrawText(m_ClientRect, size, pt, &Text[0], 128, RGB(0, 255, 0));
}



void CImageView::DrawBoundinxBox(RECT clientRect, SIZE SourceSize, RECT rcVCAROI, VCA5_RECT	bBox, BYTE alpha, COLORREF color)
{
	CD3d::MYPOLYGON	Bound[4];
	LONG Xmin;
	LONG Ymin;
	LONG Xmax;
	LONG Ymax;

	CRect rcVCAROI1(rcVCAROI);
	if( rcVCAROI1.Size() == CSize(0, 0) ) {
		rcVCAROI1.SetRect(0, 0, SourceSize.cx, SourceSize.cy);
	}

	PERCENTTOPIXEL( Xmin, (bBox.x - bBox.w/2), rcVCAROI1.Width());
	PERCENTTOPIXEL( Ymin, (bBox.y - bBox.h/2), rcVCAROI1.Height());
	PERCENTTOPIXEL( Xmax, (bBox.x + bBox.w/2), rcVCAROI1.Width());
	PERCENTTOPIXEL( Ymax, (bBox.y + bBox.h/2), rcVCAROI1.Height());

	Xmin = max(0, Xmin);
	Ymin = max(0, Ymin);
	Xmax = min(rcVCAROI1.Width()-1, Xmax);
	Ymax = min(rcVCAROI1.Height()-1, Ymax);

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
		m_pD3d->DrawLineList(clientRect, SourceSize, &Bound[k].atpoint[0], 2, 0xFF, color);
	}
}

static void	GetTextSFBelowHalfD1Height(int nScaledHeight, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
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

static void	GetTextSFAboveHalfD1Height(int nScaledHeight, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
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


static BOOL	GetTextScalingFactor(int height, SIZE SourceSize, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain)
{
	if( height<120 || height>920 ) {
		fScalor = 0.5f;
		uiFontSize = 40;
		uiFontSizeRemain = 10;
		return FALSE; // unsupported image height
	}

	int nScaledHeight = (height-20)/100;

	if(SourceSize.cy <= 288) {
		GetTextSFBelowHalfD1Height(nScaledHeight, fScalor, uiFontSize, uiFontSizeRemain);
	}
	else {
		GetTextSFAboveHalfD1Height(nScaledHeight, fScalor, uiFontSize, uiFontSizeRemain);
	}

	return TRUE;
}

void	CImageView::DrawObjectInformation(RECT clientRect, SIZE SourceSize, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color)
{
	char	string[1024] = {0,};
	
	int height = CRect(&clientRect).Height();
	float	fScalor = 0.0f;
	UINT uiFontSize = 0;
	UINT uiFontSizeRemain = 0;	

	GetTextScalingFactor(height, SourceSize, fScalor, uiFontSize, uiFontSizeRemain);

	LONG Xmin, Ymin;
	
	PERCENTTOPIXEL( Xmin, (pObject->bBox.x - pObject->bBox.w/2), SourceSize.cx);
	PERCENTTOPIXEL( Ymin, (pObject->bBox.y - pObject->bBox.h/2), SourceSize.cy);
	
	Xmin = max(0, Xmin);
	Ymin = max(0, Ymin);
	
	POINT	ptStartXY = {Xmin, Ymin};

	ptStartXY.x -= (LONG)(fScalor * 4.0f);
	ptStartXY.y -= uiFontSize + uiFontSizeRemain;

	int nCntInfoNumToShow = 2;

	int topPntObjMsg = ptStartXY.y - (nCntInfoNumToShow*uiFontSize);
	if(topPntObjMsg <= 0) {
		POINT	ptOrgStartXY = {Xmin, Ymin};
		ptStartXY = ptOrgStartXY;
		ptStartXY.y += nCntInfoNumToShow*uiFontSize;
	}

	sprintf_s( string, _countof(string), "%1.1f m", (float)pObject->ulCalibHeight/10.0f );	
	m_pD3d->DrawText( clientRect, SourceSize, ptStartXY, string, alpha, Color,fScalor);
	ptStartXY.y -= uiFontSize;
	
	sprintf_s( string, _countof(string), "%d  km/h", (int)pObject->ulCalibSpeed);
	m_pD3d->DrawText( clientRect, SourceSize, ptStartXY, string, alpha, Color,fScalor);
	ptStartXY.y -= uiFontSize;

	sprintf_s( string, _countof(string), "%.1f  sqm", (int)pObject->ulCalibArea/10.0f);
	m_pD3d->DrawText( clientRect, SourceSize, ptStartXY, string, alpha, Color,fScalor);
}
