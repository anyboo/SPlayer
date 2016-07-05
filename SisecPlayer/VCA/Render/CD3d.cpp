/* **********************************************************************************
	import standard function prototype
*/
#include "StdAfx.h"
#include "CD3d.h"
#include "../resource.h"
#include "D3DMisc.h"

BOOL CD3d::m_bInitGlobalSetupCS = FALSE;
CRITICAL_SECTION CD3d::m_GlobalSetupCS;
CD3DAdapterInfo	CD3d::m_D3dAdapterInfo;


// _dwFlags;
#define D3DFONT_BOLD			0x0001
#define D3DFONT_ITALIC			0x0002
#define D3DFONT_SHADOW_LEVEL1	0x0004
#define D3DFONT_SHADOW_LEVEL2	0x0008
#define D3DFONT_SHADOW_LEVEL3	0x0010
#define D3DFONT_ANTIALIAS		0x0020
	

CD3d::CD3d() 
{
	//check global CS
	if(!m_bInitGlobalSetupCS){
		InitializeCriticalSection(&m_GlobalSetupCS);
		InitColorConversionFunctions();
		m_bInitGlobalSetupCS = TRUE;
	}

	m_nUseDDAdapterNo = 0;

	m_pDD			 = 0;
	m_pd3dDevice	 = 0;
	m_dw3DCanvasWidth  = 0;
	m_dw3DCanvasHeight = 0;

	m_pddsPrimary	 = 0;
	m_pD3D			 = 0;
	m_pddsBackbuffer = 0;
	m_pddClipper 	 = 0;
	m_hwndClipper	 = NULL;

	m_eD3dTextureCF		= CF_NOTDEFINED;
	m_pddsTexture		= NULL;
	ZeroMemory( m_apddsTextures,sizeof(m_apddsTextures) );
	m_dwTextureWidth	= 0;
	m_dwTextureHeight	= 0;

	memset( m_pddsBrushTextures, 0, sizeof( m_pddsBrushTextures ) );
	InitializeCriticalSection( &m_crit );

	// About Font
	m_iUserFontSize		= 0;
	m_pddsFontTexture	= NULL;
	m_dwSavedStateBlock		= 0;
	m_dwDrawTextStateBlock	= 0;
	m_IsAllocStateBlock		= FALSE;
	m_bFontChanged			= FALSE;


	// for Draw RECTS and LINES
	m_aptRectAndLineTLV = new D3DTLVERTEX[MAX_TLVERTEXT_RECTANDLINES];
	for( int i=0; i<MAX_TLVERTEXT_RECTANDLINES; i++ ) {
		m_aptRectAndLineTLV[i] = D3DTLVERTEX(D3DVECTOR(0,0,0.9f),0.1f,0xFFFFFFFF,0x00000000,0.0f,0.0f);
	}

	// for Draw IMAGE
	m_aptImageTLV = new D3DTLVERTEX[MAX_TLVERTEXT_IMAGE];
	m_aptImageTLV[0] = D3DTLVERTEX(D3DVECTOR(   0,1024,0.9f),0.1f,0xFFFFFFFF,0x00000000,0.0f,1.0f);
	m_aptImageTLV[1] = D3DTLVERTEX(D3DVECTOR(   0,   0,0.9f),0.1f,0xFFFFFFFF,0x00000000,0.0f,0.0f);
	m_aptImageTLV[2] = D3DTLVERTEX(D3DVECTOR(1024,1024,0.9f),0.1f,0xFFFFFFFF,0x00000000,1.0f,1.0f);
	m_aptImageTLV[3] = D3DTLVERTEX(D3DVECTOR(1024,   0,0.9f),0.1f,0xFFFFFFFF,0x00000000,1.0f,0.0f);

	// for Draw FONT
	m_aptFontTLV = new D3DTLVERTEX[MAX_TLVERTEXT_FONT];
	for(int  i=0; i<MAX_TLVERTEXT_FONT; i++ ) {
		m_aptFontTLV[i] = D3DTLVERTEX(D3DVECTOR(0,0,0.9f),1.0f,0xFFFFFFFF,0x00000000,0.0f,0.0f);
	}

	//
	m_pSphereMesh		= 0;
	m_pCylinderMesh		= 0;
	m_pConeMesh			= 0;
	m_pddsGroundTexture = 0;
	m_pddsRulerTexture	= 0;
	m_fHumanHeight		= HUMAN_HEIGHT;
}


CD3d::~CD3d()
{
	Endup();

	DeleteCriticalSection( &m_crit );
	delete[] m_aptRectAndLineTLV;
	delete[] m_aptImageTLV;
	delete[] m_aptFontTLV;
}


HRESULT CD3d::Setup( INT			_nUseDDAdapterNo,
					   eCOLORFORMAT	_eD3dTextureCF, 
					   HWND			_hwndWindow,
					   INT			_n3DCanvasWidth,
					   INT		    _n3DCanvasHeight,
					   INT			_n3DTextureWidth,
					   INT		    _n3DTextureHeight )
{
	HRESULT hRet = NOERROR;
	BOOL    fRet;

	EnterCriticalSection(&m_GlobalSetupCS);
	
	if( (0 > _nUseDDAdapterNo) || (_nUseDDAdapterNo >= m_D3dAdapterInfo.GetDDAdaptersCnt()) ){ 
		hRet = -1;
		goto EXIT;
	}

	if( !((_eD3dTextureCF == CF_BGR565) ||(_eD3dTextureCF == CF_BGR555) ||(_eD3dTextureCF == CF_BGRX))) {
		hRet = -2;
		goto EXIT;
	}

	fRet = InitDDInterface( _nUseDDAdapterNo, _hwndWindow );
	if( fRet == FALSE ) {
		hRet = -3;
		goto EXIT;
	}

	fRet = InitPrimarySurface();
	if( fRet == FALSE ) {
		hRet = -4;
		goto EXIT;
	}

	// 0, 0  : set to screen width and height of current display mode.
	fRet = Init3DDeviceAnd3DCanvasSurface( _n3DCanvasWidth, _n3DCanvasHeight );
	if( fRet == FALSE ) {
		hRet = -5;
		goto EXIT;
	}

	// 0, 0  : default value as max width and height of texture surface
	fRet = InitImageTextureSurface( _n3DTextureWidth, _n3DTextureHeight, _eD3dTextureCF );
	if( fRet == FALSE ) {
		hRet = -6;
		goto EXIT;
	}
	
	m_nUseDDAdapterNo = _nUseDDAdapterNo;
	m_eD3dTextureCF   = _eD3dTextureCF;

	fRet = InitFont( _T("Arial"),						  
					  (m_iUserFontSize == 0) ? D3D_DEFAULT_FONT_SIZE/2 : m_iUserFontSize,						  
					  D3DFONT_BOLD | D3DFONT_SHADOW_LEVEL2 | D3DFONT_ANTIALIAS | (2<<16)|(2<<24) );
	if( fRet == FALSE ) {
		TRACE("Error _InitFont \r\n");
		hRet = -7;
		goto EXIT;
	}

	fRet = InitBrushTexture( BRT_HATCH, IDB_HATCH );

EXIT:
	if( FAILED(hRet) ) {
		DeinitSurface();
		DeinitDDInterface();
	}

	LeaveCriticalSection(&m_GlobalSetupCS);
	return hRet;
}


VOID CD3d::Endup()
{
	EnterCriticalSection(&m_GlobalSetupCS);

	DeinitBrushTexture( BRT_HATCH );
	DeinitFont();
	DeInitDrawCalib();
	DeinitSurface();
	DeinitDDInterface();
		
	LeaveCriticalSection (&m_GlobalSetupCS);
}


BOOL CD3d::SetDDClipperWindow( HWND _hwnd )
{
	if( m_pddClipper == NULL ) return FALSE;

	HRESULT hr = m_pddClipper->SetHWnd( 0, _hwnd );
	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("Clipper->SetHWnd") );
	}
	else {
		m_hwndClipper = _hwnd;
	}

	return (hr == NOERROR) ? TRUE : FALSE;
}


BOOL CD3d::DrawImage( RECT		 _rc3DCanvas,
						SIZE		 _sizeSrcImage,
						eCOLORFORMAT _eCFSrcImage,
						eROTATE		_eRotate,
						RECT		_rcROI,
						BYTE* const  _pImage )
{
	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	= DRT_IMAGE;
	atDrawRecords[0].colorValue	= 0x30FFFFFF;
	atDrawRecords[0].srcSize 	= _sizeSrcImage;

	// Union
	atDrawRecords[0].img.colorFormat = _eCFSrcImage;
	atDrawRecords[0].img.pSrc	     = _pImage;
	atDrawRecords[0].img.flag	     = 0;
	atDrawRecords[0].img.rotate		= _eRotate;
	atDrawRecords[0].img.roi		= _rcROI;

	DrawRecords( _rc3DCanvas, 0.9f, 1, &atDrawRecords[0] );
	return TRUE;
}


BOOL CD3d::DrawRectList( RECT			_rc3DCanvas,
						   SIZE			_sizeSrcImage,
						   RECT* const	_apRcList,
						   INT			_nRectCount,
						   BYTE			_cbAlpha,
						   COLORREF		_clrRect )
{
	if( 0 < _nRectCount ) {;} else return FALSE;
	if( _apRcList == NULL ) return FALSE;

	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrRect), GetGValue(_clrRect), GetRValue(_clrRect));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	   = DRT_RECTLIST;
	atDrawRecords[0].colorValue	   = dwColor;
	atDrawRecords[0].srcSize 	   = _sizeSrcImage;

	// Union
	atDrawRecords[0].rects.nRect   = _nRectCount;
	atDrawRecords[0].rects.rects   = _apRcList;

	DrawRecords( _rc3DCanvas, 0.9f, 1, &atDrawRecords[0] );
	return TRUE;
}


BOOL CD3d::DrawPolygonList( RECT				_rc3DCanvas,
						      SIZE				_sizeSrcImage,
						      MYPOLYGON* const	_aptPolygonList,
						      INT				_nPolygonCount,
							  eBRUSHTYPE		_brushType,
						      BYTE				_cbAlpha,
						      COLORREF			_clrRect )
{
	if( 0 < _nPolygonCount ) {;} else return FALSE;
	if( _aptPolygonList == NULL ) return FALSE;

	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrRect), GetGValue(_clrRect), GetRValue(_clrRect));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	   = DRT_POLYGONLIST;
	atDrawRecords[0].colorValue	   = dwColor;
	atDrawRecords[0].srcSize 	   = _sizeSrcImage;

	// Union
	atDrawRecords[0].polygons.nPolygon   = _nPolygonCount;
	atDrawRecords[0].polygons.aptPolygon = _aptPolygonList;

	atDrawRecords[0].brushType = _brushType;

	DrawRecords( _rc3DCanvas, 0.9f, 1, &atDrawRecords[0] );

	return TRUE;
}


BOOL CD3d::DrawLineList( RECT			_rc3DCanvas,
						   SIZE			_sizeSrcImage,
						   POINT* const	_apPtList,
						   INT			_nPtCount,
						   BYTE			_cbAlpha,
						   COLORREF		_clrLine )
{
	if( 1 < _nPtCount ) {;} else return FALSE;
	if( _apPtList == NULL ) return FALSE;

	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrLine), GetGValue(_clrLine), GetRValue(_clrLine));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	= DRT_LINESTRIP;
	atDrawRecords[0].colorValue	= dwColor;
	atDrawRecords[0].srcSize 	= _sizeSrcImage;

	// Union
	atDrawRecords[0].pts.nPt	= _nPtCount;
	atDrawRecords[0].pts.pts	= _apPtList;

	DrawRecords( _rc3DCanvas, 0.9f, 1, atDrawRecords );

	return TRUE;
}

BOOL CD3d::DrawText( RECT			_rc3DCanvas,
					   SIZE			_sizeSrcImage,
					   POINT        _ptStartXY,
					   LPCSTR       _lpszText,
					   BYTE			_cbAlpha,
					   COLORREF		_clrFont ,
					   float		_fScale )
{
	if( _lpszText == NULL ) return FALSE;
	if( 0 < strlen(_lpszText) ) {;} else return FALSE;

	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrFont), GetGValue(_clrFont), GetRValue(_clrFont));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	= DRT_TEXT;
	atDrawRecords[0].colorValue	= dwColor;
	atDrawRecords[0].srcSize 	= _sizeSrcImage;

	// Union
	atDrawRecords[0].text.pText	= _lpszText;
	atDrawRecords[0].text.pt	= _ptStartXY;
	atDrawRecords[0].text.fScale = _fScale;

	DrawRecords( _rc3DCanvas, 0.9f, 1, atDrawRecords );

	return TRUE;
}


BOOL CD3d::DrawCircle( RECT _rc3DCanvas, 
						SIZE _sizeSrcImage, 
						POINT _ptCentre, 
						double _fRadius, 
						BOOL _bFill, 
						BYTE _cbAlpha, 
						COLORREF	_clrCircle )
{
	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrCircle), GetGValue(_clrCircle), GetRValue(_clrCircle));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	= DRT_CIRCLE;
	atDrawRecords[0].colorValue	= dwColor;
	atDrawRecords[0].srcSize 	= _sizeSrcImage;

	// Union
	atDrawRecords[0].circle.ptCentre	= _ptCentre;
	atDrawRecords[0].circle.fRadius		= _fRadius;
	atDrawRecords[0].circle.bFill		= _bFill;

	DrawRecords( _rc3DCanvas, 0.9f, 1, atDrawRecords );

	return TRUE;
}


BOOL CD3d::DrawCircularArc( RECT _rc3DCanvas, 
							  SIZE _sizeSrcImage, 
							  POINT _ptCentre, 
							  double _fRadius, 
							  unsigned short _usWidth, 
							  double _startRadius,
							  double _finishRadius,
							  BYTE _cbAlpha, 
							  COLORREF _clrCircle )
{
	DWORD	dwColor = 0;
	dwColor = (DWORD)RGB(GetBValue(_clrCircle), GetGValue(_clrCircle), GetRValue(_clrCircle));
	dwColor |= (DWORD)(_cbAlpha << 24);

	tDRAWRECORD	atDrawRecords[1];
	ZeroMemory( atDrawRecords, sizeof(atDrawRecords) );

	atDrawRecords[0].drawType	= DRT_CIRCULARARC;
	atDrawRecords[0].colorValue	= dwColor;
	atDrawRecords[0].srcSize 	= _sizeSrcImage;

	// Union
	atDrawRecords[0].circularArc.ptCentre	= _ptCentre;
	atDrawRecords[0].circularArc.fRadius	= _fRadius;
	atDrawRecords[0].circularArc.usWidth	= _usWidth;
	atDrawRecords[0].circularArc.dStartRadius	= _startRadius;
	atDrawRecords[0].circularArc.dFinishRadius	= _finishRadius;

	DrawRecords( _rc3DCanvas, 0.9f, 1, atDrawRecords );

	return TRUE;
}


void CD3d::SetFontSize( int iFontSize )
{
	if ( m_iUserFontSize != iFontSize )
	{
		m_bFontChanged = TRUE;
		m_iUserFontSize = iFontSize;
	}
}


BOOL CD3d::GetTextExtent( LPCSTR _lpszText, SIZE* _psizePixel )
{
	if( _lpszText == NULL ) return FALSE;
	if( _psizePixel == NULL ) return FALSE;
	
	float fRowWidth  = 0.0f;
	float fRowHeight = (m_fTexCoords[0][3] - m_fTexCoords[0][1]) * m_dwFontHeight;
	float fWidth	 = 0.0f;
	float fHeight	 = fRowHeight;
	
	while( *_lpszText )
	{
		char c = *_lpszText++;
		
		if( c == '\n' )
		{
			fRowWidth = 0.0f;
			fHeight  += fRowHeight;
		}
		if( c < ' ' ) continue;
		
		float tx1 = m_fTexCoords[c-32][0];
		float tx2 = m_fTexCoords[c-32][2];
		
		fRowWidth += ( tx2 - tx1 ) * m_dwFontWidth;
		fWidth = max( fRowWidth, fWidth );
	}
	
	_psizePixel->cx = (int)fWidth;
	_psizePixel->cy = (int)fHeight;
	
	return TRUE;
}


void CD3d::RenderCalibrationGrid(RECT* pClientRect, 
								 float fCameraHeight, 
								 float fTiltAngle_deg, 
								 float fYFOV_deg, 
								 float fPanAngle_deg, 
								 float fYRoll_deg, 
								 SIZE csVideoSize, 
								 DWORD totalObjectModel ,
								 ObjectModel *pObj,
								 BOOL bSkyEnabled, 
								 BOOL bTransparentGrid, 
								 BOOL bRulerEnabled, 
								 D3DXVECTOR3* rulerPos)
{
	unsigned int w, h;

	w	= pClientRect->right;
	h	= pClientRect->bottom;

	DWORD stateHandle;

	m_pd3dDevice->CreateStateBlock( D3DSBT_ALL, &stateHandle); 

	/// save the current device state
	m_pd3dDevice->CaptureStateBlock(stateHandle );


	m_pd3dDevice->Clear( 0, NULL, /*D3DCLEAR_TARGET |*/ D3DCLEAR_ZBUFFER,
		D3DRGBA(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );

	m_pd3dDevice->SetTexture( 0, NULL );

	//	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);
	//	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS, TRUE);

	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, FALSE);

	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,			D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,			D3DBLEND_INVSRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF,			0 );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,			D3DCULL_NONE);
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,			D3DFILL_SOLID);
	//	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_NORMALIZENORMALS,	TRUE);	/// need this because scaling screws up normals
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,			TRUE );

	
	D3DVIEWPORT7 Viewport =  { 0, 0, w, h, 0.0f, 1.0f };
	m_pd3dDevice->SetViewport( &Viewport );

	//SetupMatricies 
	D3DXVECTOR3 vecGridRot;
	vecGridRot.x = D3DXToRadian(fTiltAngle_deg);
	vecGridRot.z = D3DXToRadian(fPanAngle_deg);
	vecGridRot.y = D3DXToRadian(fYRoll_deg);

	D3DXMATRIX matView, matProjModel_Clipped, matProjModel, matView_noTrans, matTranslation, matModelRotationTilt, matModelTiltLevel, matModelRotationPan, matModelRotationRoll;

	D3DXMatrixPerspectiveFov( &matProjModel, D3DX_PI*fYFOV_deg/180.0f, float(csVideoSize.cy)/float(csVideoSize.cx), 0.1f, 1000.0f );
	D3DXMatrixPerspectiveFov( &matProjModel_Clipped, D3DX_PI*fYFOV_deg/180.0f, float(csVideoSize.cy)/float(csVideoSize.cx), 0.1f, 510.0f );
	
	D3DXMatrixTranslation(&matTranslation, 0, 0, -fCameraHeight);
	D3DXMatrixRotationX(&matModelRotationTilt, vecGridRot.x);
	D3DXMatrixRotationX(&matModelTiltLevel, -D3DX_PI/2);
	D3DXMatrixRotationY(&matModelRotationPan, vecGridRot.z);
	D3DXMatrixRotationZ(&matModelRotationRoll, vecGridRot.y);

	matView_noTrans = matModelTiltLevel*matModelRotationPan*matModelRotationTilt*matModelRotationRoll;
	matView = matTranslation*matView_noTrans;


	m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX *)&matProjModel );
	m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)&matView );


	m_pd3dDevice->BeginScene();

	DrawGroundPlane(bSkyEnabled, bTransparentGrid);

	if (bRulerEnabled)
		DrawRuler(rulerPos);

	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE);
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, D3DRGB(0.4f,0.4f,0.4f) );
	//	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,			D3DCULL_CW);
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_NORMALIZENORMALS,	TRUE);	/// need this because scaling screws up normals
	m_pd3dDevice->SetLight(0, &m_light);
	m_pd3dDevice->LightEnable(0, TRUE);


	for (ULONG i = 0  ; i < totalObjectModel; i++){
		//DrawObject(i);
		DrawPerson(i, &pObj[i]);
	}

	m_pd3dDevice->EndScene();

	/// restore previous state
	m_pd3dDevice->ApplyStateBlock(stateHandle);

	m_pd3dDevice->DeleteStateBlock(stateHandle); 

	m_pd3dDevice->Clear( 0, NULL, /*D3DCLEAR_TARGET |*/ D3DCLEAR_ZBUFFER,
		D3DRGBA(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );
}


BOOL CD3d::PrimaryPresentation( RECT* const _prcScreen, RECT* const _prc3DCanvas )
{

	/*
	여기 이단계에서도 ScaleUpDown이 발생한다.
	*/
	RECT	rcScrn;
	if( NULL == _prcScreen )
	{
		// Clipping Window 전체에 뿌린다고 생각한다.
		HWND	hwnd;
		m_pddClipper->GetHWnd( &hwnd );
		GetClientRect( hwnd, &rcScrn );
		ClientToScreen( hwnd, (POINT*)&rcScrn.left );
		ClientToScreen( hwnd, (POINT*)&rcScrn.right );
	} else {
		rcScrn = *_prcScreen;
	}
	/*
	여기서 멀티모니터(멀티ADAPTER)에 관련된 고려사항이 걸린다.
	ADAPTER 0을 사용하는 경우 두개 모니터에서 나오지만 secondary쪽은 깨져서 나온다.
    ADAPTER 1,2을 사용하는 경우 해당 모니터에서만 나온다.

    MONITOR 좌표계도 고려 대상이다.
	WINDOW7 ADAPTER 0만을 사용해도 잘된다. (1,2번을 할 경우 2번 모니터에 화면이 나오지 않는 문제가 있다)
	*/
	//INT nAdapterNo = m_GetAdapterNoByPosition( rcScrn );
	INT nAdapterNo = 0;

	//ErrDbg(1, "nAdapterNo %d \r\n", nAdapterNo);
	if( -1 == nAdapterNo ) return TRUE;
	if ( ( m_nUseDDAdapterNo != nAdapterNo )  || ( m_bFontChanged ) )
	{
		m_bFontChanged = FALSE;
		Endup();

		Setup( nAdapterNo, m_eD3dTextureCF, m_hwndClipper, m_dw3DCanvasWidth, m_dw3DCanvasHeight, m_dwTextureWidth, m_dwTextureHeight );				
		TRACE("Change Monitor [%d] \n ", nAdapterNo);
		SetDDClipperWindow( m_hwndClipper );
	}

	rcScrn = m_D3dAdapterInfo.AdjustPositionByAdapterNo( rcScrn, nAdapterNo );
	//ErrDbg(1, "S(%04d,%04d) \r\n", rcScrn.left, rcScrn.top);

	if(!m_pddsPrimary) return FALSE;

	EnterCriticalSection(&m_crit);
	HRESULT	hr =
	m_pddsPrimary->Blt( (_prcScreen == NULL) ? &rcScrn : _prcScreen,	// DestRect
						m_pddsBackbuffer,	// SrcSurface (3DCanvas)
						_prc3DCanvas,		// SrcRect
						DDBLT_ASYNC | DDBLT_DONOTWAIT ,
						NULL);
	LeaveCriticalSection(&m_crit);

	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("Blt") );
		if( hr == DDERR_SURFACELOST ) {
			m_pddsBackbuffer->Restore();
			//setup 
			TRACE("Request Setup Result [%d] \n ", hr);
			m_nUseDDAdapterNo = -1;
		}
		return FALSE;
	}
	return TRUE;
}



BOOL CD3d::InitDDInterface( INT _nUseDDAdapterNo, HWND _hwndWindow )
{
	HRESULT	hr;

	GUID*	  pGUID	   = NULL;

	pGUID = NULL;

	DDADAPTERINFO* pD3dAdapterInfo =  m_D3dAdapterInfo.GetDDAdapterInfo(_nUseDDAdapterNo);

	if( pD3dAdapterInfo->fIsAvailableGUID == TRUE ) {
		pGUID = &(pD3dAdapterInfo->tGUID);
	}

	LPDIRECTDRAW7 pDD = NULL;
	hr = ::DirectDrawCreateEx( pGUID, (PVOID*)&pDD, IID_IDirectDraw7, NULL );
	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("DirectDrawCreateEx") );
		return FALSE;
	}

	LPDIRECT3D7   pD3D = NULL;
	hr = pDD->QueryInterface( IID_IDirect3D7, (PVOID*)&pD3D );
	if( FAILED(hr) ) {
		pDD->Release();
		MACRO_DDERROR( hr, _T("QueryInterface(IID_IDirect3D7)") );
		return FALSE;
	}

	hr = pDD->SetCooperativeLevel( _hwndWindow, DDSCL_NORMAL );
	if( FAILED(hr) ) {
		pDD->Release();
		MACRO_DDERROR( hr, _T("SetCooperativeLevel") );
		return FALSE;
	}

	m_pDD	= pDD;
	m_pD3D	= pD3D;

	return TRUE;
}


VOID CD3d::DeinitDDInterface()
{
	SAFE_RELEASE( m_pD3D );
	SAFE_RELEASE( m_pDD );
}


BOOL CD3d::InitPrimarySurface()
{
	if( m_pDD == NULL ) return FALSE;

	LPDIRECTDRAW7		 pDD = m_pDD;
	LPDIRECTDRAWSURFACE7 pddsPrimary = NULL;

	pddsPrimary = CreateCustomSurface( pDD, SURFACETYPE_PRIMARY, NULL/*n/a*/, 0/*n/a*/, 0/*n/a*/ );
	if( pddsPrimary == NULL ) return FALSE;

	HRESULT hr = pDD->CreateClipper( 0, &m_pddClipper, NULL );
	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("CreateClipper") );
		return FALSE;
	}

	hr = pddsPrimary->SetClipper( m_pddClipper );
	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("SetClipper") );
		return FALSE;
	}

	m_pddsPrimary = pddsPrimary;
	return TRUE;
}


BOOL CD3d::Init3DDeviceAnd3DCanvasSurface( INT _nWidth, INT _nHeight )
{
	if( m_pDD == NULL ) return FALSE;

	HRESULT	hr;

	LPDIRECTDRAW7		 pDD			 = m_pDD;
	LPDIRECT3DDEVICE7    pd3dDevice      = NULL;
	LPDIRECTDRAWSURFACE7 pddsBackBuffer  = NULL;

	DDSURFACEDESC2	ddsdtemp;
	_INIT_DIRECTDRAW_STRUCT( ddsdtemp );

	if( 0 == _nWidth || 0 == _nHeight ) {
		// 현재 스크린 해상도 만큼 잡아 버린다.
		// 사실 Canvas는 해상도 보다 클 이유가 없다.
		pDD->GetDisplayMode( &ddsdtemp );
		_nWidth  = ddsdtemp.dwWidth;
		_nHeight = ddsdtemp.dwHeight;
	}

	// 3DDevice에 붙일 Backbuffer surface(3D Canvas)를 만든다. - TextureSurface와는 다르다. 
	pddsBackBuffer = CreateCustomSurface( pDD, 
										   SURFACETYPE_3DCANVAS, 
										   &pd3dDevice,
										   _nWidth,
										   _nHeight );
	BOOL	fRet;
	if( (pd3dDevice != NULL) && (pddsBackBuffer != NULL) ){
		m_pd3dDevice	 = pd3dDevice;
		m_pddsBackbuffer = pddsBackBuffer;

		m_dw3DCanvasWidth  = _nWidth;
		m_dw3DCanvasHeight = _nHeight;

		hr = pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
		fRet = TRUE;

		InitDrawCalib();
	}else{
		if( pddsBackBuffer ) {
			LPDIRECTDRAWSURFACE7 lpAttachedSurf = NULL;
			pddsBackBuffer->EnumAttachedSurfaces( &lpAttachedSurf, lpEnumSurfacesCallback2 );
			pddsBackBuffer->DeleteAttachedSurface( 0, lpAttachedSurf );
			SAFE_RELEASE( lpAttachedSurf );
		}
		SAFE_RELEASE( pddsBackBuffer );
		SAFE_RELEASE( pd3dDevice );
		fRet = FALSE;
	}
	return fRet;
}


BOOL CD3d::InitImageTextureSurface( INT _nWidth, INT _nHeight, eCOLORFORMAT _eD3dTextureCF )
{
	LPDIRECTDRAW7 pDD  = m_pDD;
	LPDIRECTDRAWSURFACE7 apddsTextures[8] = {0,0,0,0,0,0,0,0};

	DDPIXELFORMAT tddPF;
	if(_eD3dTextureCF == CF_BGR565) tddPF = DDPF_RGB565;
	if(_eD3dTextureCF == CF_BGR555) tddPF = DDPF_RGB555;
	if(_eD3dTextureCF == CF_BGRX)   tddPF = DDPF_RGB32;

#if 0
	// Texture를 Backbuffer로 옮길때 성능향상을 하기위해 최소한의 적당한 크기를 사용하기위해
	// 여러 크기로 미리 만들어 두지만 메모리낭비가 심하여 여러 채널을 동시에 다루어야 하는경우
	// 부작용이 생긴다. 하지만 성능은 아무래도 딱 맞게 하는것이 아주 좋다. 크기가 차이가 날수록 심해진다.
	apddsTextures[0] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, 256, 128 );
	apddsTextures[1] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, 256, 256 );
	apddsTextures[2] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, 512, 128 );
	apddsTextures[3] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, 512, 256 );
	apddsTextures[4] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, 512, 512 );
	apddsTextures[5] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF,1024, 256 );
	apddsTextures[6] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF,1024, 512 );
	apddsTextures[7] = _CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF,1024,1024 );
#else
	// 가장 큰 것 1024 x 1024만 연다.
	if( apddsTextures[7] == NULL ) {
		/*
			VIA Chip 마더보드 build-in VGA인 경우 Texture의 크기가 제한적이다. 즉 아무 크기나 되지 않는다.
			아래와 같이 1024로 연다.
		*/
		/*
			SIDE-EFFECT : NOTEBOOK같은 경우 1024만큼 열수가 없을텐데.... 아직 모름;;
		*/

	    // 참고 : _FindSuitableTextureSurface 함수에서 텍스쳐 크기를...
		apddsTextures[7] = CreateCustomSurface( pDD, SURFACETYPE_AGPTEXTURE, &tddPF, _nWidth, _nHeight );
		if( NULL == apddsTextures[7] ) {
			/*
				NOTEBOOK같은 것은 AGP를 지원하지 않을수도 있다. 이런 경우 VideoMemory에 만든다.
			*/
			apddsTextures[7] = CreateCustomSurface( pDD, SURFACETYPE_VIDMEMTEXTURE, &tddPF, _nWidth, _nHeight );
			if( NULL == apddsTextures[7] ) {
				/*
					이것도 안 만들어 지면 어떡하나???
					텍스쳐 크기를 줄여 봐야 하나???
				*/
				TRACE("FAIL! _CreateCustomSurface VIDEOMEM apddsTextures[7] has to Align 16bytes bound?? %Xh \r\n", apddsTextures[7]);
			}
			else {
				TRACE("OK! _CreateCustomSurface VIDEOMEM apddsTextures[7] has to Align 16bytes bound %Xh \r\n", apddsTextures[7]);
			}
		}
		else {
			//TRACE("OK! _CreateCustomSurface AGPTX apddsTextures[7] has to Align 16bytes bound %Xh \r\n", apddsTextures[7]);
		}
	}
#endif

	BOOL	fRet;
	if( apddsTextures[7] ) {
		CopyMemory( m_apddsTextures, apddsTextures, sizeof(m_apddsTextures) );
		fRet = TRUE;

		m_dwTextureWidth  = _nWidth;
		m_dwTextureHeight = _nHeight; 
	}
	else {
		// 안 만들어진게 있으면 어떡하지? 메모리가 부족한가?
		for( int i=0; i<8; i++ ) {
			SAFE_RELEASE( apddsTextures[i] );
		}
		fRet = FALSE;
	}

	return fRet;
}


VOID CD3d::DeinitSurface()
{
	for( int i=0; i<8; i++ ) {
		SAFE_RELEASE( m_apddsTextures[i] );
	}

	if( m_pddsBackbuffer ) {
		HRESULT hRetLost = m_pddsBackbuffer->IsLost();
		if( hRetLost == DD_OK )
		{
			LPDIRECTDRAWSURFACE7 lpAttachedSurf;
			m_pddsBackbuffer->EnumAttachedSurfaces( &lpAttachedSurf, lpEnumSurfacesCallback2 );
			m_pddsBackbuffer->DeleteAttachedSurface( 0, lpAttachedSurf );
			SAFE_RELEASE( lpAttachedSurf );
		}
	}
	SAFE_RELEASE( m_pddsBackbuffer );
	SAFE_RELEASE( m_pd3dDevice );

	if( m_pddsPrimary ) {
		m_pddsPrimary->SetClipper( NULL );
	}
	SAFE_RELEASE( m_pddsPrimary );
	SAFE_RELEASE( m_pddClipper );
}


BOOL CD3d::InitFont( LPCTSTR _lpszFontName, DWORD dwHeight, DWORD dwFlags )
{
	INT	  offset_x    = (char)( (dwFlags>>16) & 0xFF );
	INT	  offset_y    = (char)( (dwFlags>>24) & 0xFF );
	DWORD dwBold      = ( dwFlags & D3DFONT_BOLD )	    ? FW_BOLD  : FW_NORMAL;
	DWORD dwItalic    = ( dwFlags & D3DFONT_ITALIC )    ? TRUE	   : FALSE;
	DWORD dwAntiAlias = ( dwFlags & D3DFONT_ANTIALIAS ) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;

	int shadowlevel = 0;								  // no shadow--> shadow offset is ignored.
	if( dwFlags & D3DFONT_SHADOW_LEVEL1 ) shadowlevel = 1;// blur 1 time.
	else
	if( dwFlags & D3DFONT_SHADOW_LEVEL2 ) shadowlevel = 2;// blur 2 times.
	else
	if( dwFlags & D3DFONT_SHADOW_LEVEL3 ) shadowlevel = 3;// blur 3 times.

	int margin_x = shadowlevel * 2;
	int margin_y = shadowlevel * 2;
	if( shadowlevel < abs(offset_x) ) margin_x += abs( shadowlevel - abs(offset_x) );
	if( shadowlevel < abs(offset_y) ) margin_y += abs( shadowlevel - abs(offset_y) );

	m_def_hint = margin_x + 1;

	m_dwFontWidth = 256;
	if( dwHeight >= 50 ) m_dwFontWidth = 512;

	HFONT hFont = ::CreateFont( dwHeight, 0, 0, 0, dwBold, dwItalic,
							    FALSE,
							    FALSE, 
							    DEFAULT_CHARSET, 
							    OUT_DEFAULT_PRECIS,
							    CLIP_DEFAULT_PRECIS,
							    dwAntiAlias,
							    VARIABLE_PITCH,
							    _lpszFontName );

	DWORD height = GetFontAreaHeight( hFont, m_dwFontWidth, margin_x, margin_y );
	if( height <=   64 ) m_dwFontHeight = 64;
	else
	if( height <=  128 ) m_dwFontHeight = 128;
	else
	if( height <=  256 ) m_dwFontHeight = 256;
	else
	if( height <=  512 ) m_dwFontHeight = 512;
	else
	if( height <= 1024 ) m_dwFontHeight = 1024;
	
	m_pddsFontTexture = CreateCustomSurface( m_pDD, SURFACETYPE_MANAGEDTEXTURE, &DDPF_RGBA16, m_dwFontWidth, m_dwFontHeight );
	if( m_pddsFontTexture == NULL ) {
		DeleteObject( hFont );
		return FALSE;
	}

	// 모든 ASCII 글자를 한판의 Bitmap으로 미리 만들어 둔다.
	BITMAPINFO	bmi = {0,};
	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		=  (int)m_dwFontWidth;
	bmi.bmiHeader.biHeight		= -(int)m_dwFontHeight;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount	= 32;

	// Create a DC and a bitmap for the font
	DWORD*	pdwBmBits;
	HDC 	hDC 	  = CreateCompatibleDC( NULL );
	HBITMAP hbmBitmap = CreateDIBSection( hDC, 
										  &bmi,
										  DIB_RGB_COLORS,
										  (VOID**)&pdwBmBits,
										  NULL,
										  0 );
	SetMapMode(   hDC, MM_TEXT );
	SelectObject( hDC, hbmBitmap );
	SelectObject( hDC, hFont );
	SetTextColor( hDC, RGB(255,255,255) );
	SetBkColor(   hDC, 0x00000000 );
	SetTextAlign( hDC, TA_TOP );

	//먼저 검정색으로 클리어한다.
	ZeroMemory( pdwBmBits, m_dwFontWidth * m_dwFontHeight * 4 );	// 32bit RGB

	DWORD x = 0;
	DWORD y = 0;
	SIZE size = {0,};
	for( TCHAR c = 32; c < 127; c++ )
	{
		::GetTextExtentPoint32( hDC, &c, 1, &size );
		if( (DWORD)( x + size.cx + 1 + margin_x ) > m_dwFontWidth ) {
			x = 0;
			y += size.cy + 1 + margin_y;
		}

		::ExtTextOut( hDC,
				      x + 1 + shadowlevel,
					  y + 1 + shadowlevel,
					  ETO_OPAQUE,
					  NULL,
					  &c, 1, NULL );

		m_fTexCoords[c-32][0] = ((float)(x+0))/m_dwFontWidth;
		m_fTexCoords[c-32][1] = ((float)(y+0))/m_dwFontHeight;
		m_fTexCoords[c-32][2] = ((float)(x+1+size.cx+margin_x))/m_dwFontWidth;
		m_fTexCoords[c-32][3] = ((float)(y+1+size.cy+margin_y))/m_dwFontHeight;
		
		x += size.cx + 1 + margin_x;
	}//for( char c = 32; c < 127; c++ )

	//
	BYTE* blured = new BYTE[ m_dwFontWidth * m_dwFontHeight ];
	for( DWORD i=0; i < m_dwFontWidth * m_dwFontHeight; i++ ) {
		blured[i] = (BYTE)( pdwBmBits[i] & 0xFF );	// AlphaChannel만 긁어온다.
	}

	for( int sl = 0; sl < shadowlevel; sl++ )
	{
		for( y=0; y < m_dwFontHeight; y++ )
		{
			int y1 = y-1;
			int y2 = y+1;

			y1 = max( 0, y1 );
			y2 = min( (INT)m_dwFontHeight-1, y2 );

			for( x=0; x < m_dwFontWidth; x++ )
			{
				int x1 = x - 1,
					x2 = x + 1;

				x1 = max( 0, x1 );
				x2 = min( (INT)m_dwFontWidth - 1, x2 );

				DWORD w = m_dwFontWidth;
				blured[y * w + x] =
				((blured[y1*w+x1]&0xFF) + (blured[y1*w+x]&0xFF) + (blured[y1*w+x2]&0xFF) +
				 (blured[ y*w+x1]&0xFF) + (blured[ y*w+x]&0xFF) + (blured[ y*w+x2]&0xFF) +
				 (blured[y2*w+x1]&0xFF) + (blured[y2*w+x]&0xFF) + (blured[y2*w+x2]&0xFF)+4) / 9;
			}
		}//for( y=0; y < m_dwFontHeight; y++ )
	}//for( int sl = 0; sl < shadowlevel; sl++ )

	// Lock the surface and write the alpha values for the set pixels
	WORD* pDst16;
	DWORD dwPitch;
	if( FALSE == GetSurfacePointer( m_pddsFontTexture, NULL, (BYTE**)&pDst16, &dwPitch ) ) {
		delete[] blured;
		DeleteObject( hbmBitmap );
		DeleteDC( hDC );
		DeleteObject( hFont );
		return FALSE;
	}

	for( y = 0; y < m_dwFontHeight; y++ )
	{
		int y1 = y - offset_y;
		y1 = max( 0, y1 );
		y1 = min( (INT)m_dwFontHeight-1, y1 );

		for( x = 0; x < m_dwFontWidth; x++ )
		{
			int x1 = x - offset_x;
			x1 = max( 0, x1 );
			x1 = min( (INT)m_dwFontWidth-1, x1 );

			DWORD w = m_dwFontWidth;
			int bAlpha = ( (int)blured[ w * y1 + x1 ] + 7 ) >> 4;
			bAlpha = min( 15, bAlpha );

			int b = (BYTE)( pdwBmBits[ w * y + x ] & 0xFF );
			int c = (b) >> 4;
			c = c | ( c << 4 ) | ( c << 8 );

			int bAlpha2 = (b) >> 4;
			bAlpha = max( bAlpha, bAlpha2 );

			*pDst16++ = (WORD)( (bAlpha << 12) | c );
		}
	}
	ReleaseSurfacePointer( m_pddsFontTexture );

	//
	delete[] blured;
	DeleteObject( hbmBitmap );
	DeleteDC( hDC );
	DeleteObject( hFont );

	MakeStateBlock( m_pddsFontTexture );
	m_IsAllocStateBlock = TRUE;

	return TRUE;
}


BOOL CD3d::DeinitFont()
{
	// Delete the state blocks
	if( m_IsAllocStateBlock ) 
	{
		if( m_pd3dDevice )
		{
			if( m_dwSavedStateBlock ) {
				m_pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
			}
			if( m_dwDrawTextStateBlock ) {
				m_pd3dDevice->DeleteStateBlock( m_dwDrawTextStateBlock );
			}
		}
		m_dwSavedStateBlock    = 0L;
		m_dwDrawTextStateBlock = 0L;
		m_IsAllocStateBlock = FALSE;
	}

	SAFE_RELEASE( m_pddsFontTexture );

	return TRUE;
}


BOOL CD3d::InitBrushTexture( eBRUSHTYPE brushType, UINT bitmapId )
{
	// Initialise fill pattern textures
	ASSERT( m_pddsBrushTextures[brushType] == 0 );

	unsigned char bits[ 16 * 16 * 3 + 4];

	m_pddsBrushTextures[brushType] = CreateCustomSurface( m_pDD, SURFACETYPE_MANAGEDTEXTURE, &DDPF_RGBA32, 16, 16 );
	if( !m_pddsBrushTextures[brushType] )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	return true;
	// Load the hatch bitmap
	HBITMAP hBmp =NULL ;// LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(bitmapId));
	ASSERT( hBmp );

	BITMAPINFO bmi;
	memset( &bmi, 0, sizeof( BITMAPINFO ) );
	bmi.bmiHeader.biSize		= sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth		= 16;
	bmi.bmiHeader.biHeight		= 16;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biBitCount	= 24;

	HDC hDC = NULL;// ::GetDC(AfxGetMainWnd()->GetSafeHwnd());
	::GetDIBits( hDC, hBmp, 0, 16, bits, &bmi, DIB_RGB_COLORS );
	::ReleaseDC( NULL, hDC );

	// Lock the surface and transfer the bits...
	unsigned char *pBytes;
	unsigned char ucAlpha = 0xF0;
	DWORD dwPitch;
	if( GetSurfacePointer( m_pddsBrushTextures[brushType], NULL, &pBytes, &dwPitch ) )
	{
		unsigned char *pLine = pBytes;
		for( int y = 0; y < 16; y++, pLine += dwPitch )
		{
			unsigned char *pByte = pLine;
			for( int x = 0; x < 16; x++ )
			{
				int offset = ((y*16) + x) * 3;
				*pByte++ = bits[ offset + 0 ];
				*pByte++ = bits[ offset + 1 ];
				*pByte++ = bits[ offset + 2 ];
				*pByte++ = bits[offset] == 0x00 ? 0x00 : 0xF0;
			}
		}

		ReleaseSurfacePointer( m_pddsBrushTextures[brushType] );

	}
	else
	{
		ASSERT( FALSE );
	}

	return TRUE;
}


BOOL CD3d::DeinitBrushTexture( eBRUSHTYPE brushType )
{
	SAFE_RELEASE( m_pddsBrushTextures[brushType] );

	m_pddsBrushTextures[brushType] = 0;

	return FALSE;
}


BOOL CD3d::MakeStateBlock( LPDIRECTDRAWSURFACE7 const _pDDSurface )
{
	if(m_pd3dDevice == NULL)
	{
		return FALSE;
	}
	
	for( UINT which = 0; which < 2; which++ )
	{
		m_pd3dDevice->BeginStateBlock();
		
		m_pd3dDevice->SetTexture( 0, _pDDSurface );

		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,	TRUE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,	TRUE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF,			0x08 );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC,			D3DCMP_GREATEREQUAL );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,			D3DFILL_SOLID );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,			D3DCULL_CCW );

		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,			TRUE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,		FALSE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CLIPPING,			TRUE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS,		FALSE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CLIPPLANEENABLE,	FALSE );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_VERTEXBLEND,		FALSE );
		//m_pd3dDevice->SetRenderState( D3DRENDERSTATE_INDEXEDVERTEXBLENDENABLE, FALSE );//이 파라미터는 없는것 같다.
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,			FALSE );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	 D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	 D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_NONE);
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	 D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE );

		if( which == 0 )
			m_pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else
			m_pd3dDevice->EndStateBlock( &m_dwDrawTextStateBlock );
	}

	return TRUE;
}


BOOL CD3d::DrawText( float			sx,
					  float			sy,
					  RECT			rc,
					  DWORD			dwColor,
					  const char*	strText,
					  float			fScale,
					  int			Space )
{
	if( m_pd3dDevice == NULL ) return FALSE;

	// Setup renderstate
	m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );

	// Fill vertex buffer
	DWORD		  dwNumVtx = 0;
	D3DTLVERTEX*  pVx = m_aptFontTLV;
	float		  sxOrg = sx;

	if( fScale < 1.0f )
	{
		// Automatically adjust spacing
		Space = (int)(fScale + 0.5f);
	}

	while( *strText )
	{
		TCHAR c = *strText++;
		if( c == '\n' ) {
			sy += ( m_fTexCoords['A'-32][3] - m_fTexCoords['A'-32][1] ) * m_dwFontHeight;
			sx = sxOrg;
			continue;
		}
		if( c < ' ' ) continue;

		float *TexCoords = m_fTexCoords[c-32];
		float tx1 = TexCoords[0];
		float ty1 = TexCoords[1];
		float tx2 = TexCoords[2];//+2.0f/m_dwFontWidth;
		float ty2 = TexCoords[3];//+2.0f/m_dwFontHeight;
		
		float w = (tx2 - tx1) * m_dwFontWidth;
		float h = (ty2 - ty1) * m_dwFontHeight;

		//w -= 8.5f;
        //h -= 8.5f;

		// Apply scaling if specified
		w *= fScale;
		h *= fScale;

		//clip check...
		if( sx     >= rc.left  &&
			sy     >= rc.top   &&
			sx + w <= rc.right &&
			sy + h <= rc.bottom   )
		{
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
			*pVx++ = InitFontVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
			dwNumVtx += 6;

			// MAX_TLVERTEXT_FONT 보다 더 많은 글자를 뿌려야 된다면 일단 여기서 한번 뿌려서 털어낸다.
			if( dwNumVtx > (MAX_TLVERTEXT_FONT - 6) ) {
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, m_aptFontTLV, dwNumVtx, 0 );
				pVx = m_aptFontTLV;
				dwNumVtx = 0L;
			}
		}
		
		sx += ( w - m_def_hint + Space );
	}//while( *strText )
	
	// 위에서 털던 그렇지 않던 MAX_TLVERTEXT_FONT 만큼 안되는 것들을 여기서 그린다.
	if( dwNumVtx > 0 ) {
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, m_aptFontTLV, dwNumVtx, 0 );
	}

	// Restore the modified renderstates
	m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

	return TRUE;
}



HRESULT WINAPI CD3d::lpEnumSurfacesCallback2( LPDIRECTDRAWSURFACE7	lpDDSurface,
											   LPDDSURFACEDESC2		lpDDSurfaceDesc,
											   LPVOID				lpContext )
{
	LPDIRECTDRAWSURFACE7* lppSurf = (LPDIRECTDRAWSURFACE7* )lpContext;
	*lppSurf = lpDDSurface;

	return DDENUMRET_CANCEL;
}


BOOL CD3d::DrawRecords( RECT			_rc3DCanvas,
						 D3DVALUE		_foZValue,
						 ULONG			_nRecordCount,
						 tDRAWRECORD*	_aptDrawRecords )
{
	BOOL	fRet = TRUE;
	if (!m_pd3dDevice)
	{
		assert(m_pd3dDevice);
		return FALSE;
	}
	
	EnterCriticalSection( &m_crit );
	m_pd3dDevice->BeginScene();
	

	for( ULONG i=0; i<_nRecordCount; i++ )
	{
		switch( _aptDrawRecords->drawType )
		{
		case DRT_IMAGE:
			{
				/*
				이 단계에서는 3DCanvas(3DDevice Attached BackBuffer)에 뿌리기전 단계로
				입력 영상에 맞는 적당한 Texture Surface를 선정하고 이 Surface Pointer에
				실제 영상 데이터와 Texture Buffer간 1:1로 옮겨 넣는 부분이다.
				_ConvertColorFormat이라는 놈이 nDownScale_factor를 적용해주기는 하나,
				1:1로 할려고 한다.
				*/

				/* ROI을 적용할 경우 현재 _ConvertColorFormat을 통해서는 COPY을 못하게 되어 있다.
				우선은 전체 영역을 COpy하고 실제 보여 주는 부분만을 Present하도록하는 방식으로 구현 
				시간이 될 경우 _ConvertColorFormat가 ROI을 지원하도록 수정할 예정이다
				*/

				int	nDownScale_factor = 1;	// 1:1
				

				if(_aptDrawRecords->img.roi.right == 0 || _aptDrawRecords->img.roi.bottom == 0){ //ROI => total image
					_aptDrawRecords->img.roi.left	= 0;
					_aptDrawRecords->img.roi.top	= 0;
					_aptDrawRecords->img.roi.right	= _aptDrawRecords->srcSize.cx;	// Source Image Width
					_aptDrawRecords->img.roi.bottom = _aptDrawRecords->srcSize.cy;	// Source Image Height
				}

				int srW = _aptDrawRecords->srcSize.cx;	// Source Image Width
				int srH = _aptDrawRecords->srcSize.cy;	// Source Image Height

				// 1:1로 뿌리고자 적당한 Texture Surface를 찾는다. (m_pddsTexture결정)
				// 결과는 m_dwTextureWidth, m_dwTextureHeight, m_pddsTexture를 사용한다.
				if( FindSuitableTextureSurface( srW, srH ) == FALSE ) {
					TRACE("FAIL!! - _FindSuitableTextureSurface (find %dx%d) \r\n", srW, srH);
					break;
				}

				BYTE*	pSurf;
				DWORD	dwPitch;
				RECT	rcSrc;
				SetRect( &rcSrc, 0, 0, srW, srH );
				if( GetSurfacePointer( m_pddsTexture, &rcSrc, &pSurf, &dwPitch ) )
				{
					// _aptDrawRecords->img.pSrc가 NULL인 경우
					// 현재 TextureSurface의 내용을 그대로 유지 하려고 하는 의도도 있다.
					ConvertColorFormat( _aptDrawRecords->img.pSrc,
										 _aptDrawRecords->img.colorFormat,
										 _aptDrawRecords->srcSize,
										 pSurf,
										 m_eD3dTextureCF,
										 dwPitch,
										 nDownScale_factor );
					ReleaseSurfacePointer( m_pddsTexture );

					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

					///RECT srcRect = { 0, 0, srW, srH };
					DrawImageTexture( m_pddsTexture, m_dwTextureWidth, m_dwTextureHeight, _rc3DCanvas, _aptDrawRecords->img.roi, 
						_aptDrawRecords->colorValue, _foZValue, _aptDrawRecords->img.rotate);
				}
				else {
					fRet = FALSE;
				}
			}
			break;

		case DRT_POINTLIST:
		case DRT_LINELIST:
		case DRT_LINESTRIP:
			{
				//DrawPrimitive...
				POINT *pPt = _aptDrawRecords->pts.pts;

				int trW = _rc3DCanvas.right  - _rc3DCanvas.left;
				int trH = _rc3DCanvas.bottom - _rc3DCanvas.top;
				int srW = _aptDrawRecords->srcSize.cx;
				int srH = _aptDrawRecords->srcSize.cy;

				ULONG n = 0;
				for( ULONG i=0; i < _aptDrawRecords->pts.nPt; i++ ) {
					//clip rect..
					POINT pt = *pPt;

					pt.x = max( 0, pt.x );
					pt.x = min( _aptDrawRecords->srcSize.cx, pt.x );
					pt.y = max( 0, pt.y );
					pt.y = min( _aptDrawRecords->srcSize.cy, pt.y );

					if (srW)
					m_aptRectAndLineTLV[n].dvSX = (float)(pt.x * trW/srW + _rc3DCanvas.left);
					if (srH)
					m_aptRectAndLineTLV[n].dvSY = (float)(pt.y * trH/srH + _rc3DCanvas.top);

					n++;
					pPt++;
				}

				SetVertexColorValue( m_aptRectAndLineTLV, 0, n, _aptDrawRecords->colorValue );
				SetVertexZValue( m_aptRectAndLineTLV, 0, n, _foZValue );
				EnableAlpha(m_pd3dDevice);

				m_pd3dDevice->DrawPrimitive( (D3DPRIMITIVETYPE)_aptDrawRecords->drawType, D3DFVF_TLVERTEX, m_aptRectAndLineTLV, n, NULL );
			}
			break;

		case DRT_POLYGONLIST:
			{
				ULONG n = 0;
				DrawPolygonVCA( _rc3DCanvas, _foZValue, _nRecordCount, _aptDrawRecords );
			}
			break;

		case DRT_RECTLIST:
			{
				//DrawPrimitive...
				RECT *pRc = _aptDrawRecords->rects.rects;

				int trW = _rc3DCanvas.right  - _rc3DCanvas.left;
				int trH = _rc3DCanvas.bottom - _rc3DCanvas.top;
				int srW = _aptDrawRecords->srcSize.cx;
				int srH = _aptDrawRecords->srcSize.cy;

				ULONG n = 0;
				for( ULONG i=0; i < _aptDrawRecords->rects.nRect; i++ )
				{
					//clip rect..
					RECT rc = *pRc;

					rc.left   = max( 0, rc.left );
					rc.top    = max( 0, rc.top );
					rc.right  = min( _aptDrawRecords->srcSize.cx, rc.right );
					rc.bottom = min( _aptDrawRecords->srcSize.cy, rc.bottom );

					// Triangle Vertext를 만든다.
					// 뿌릴 사각형을 삼각형 두개로 표현한다.

					m_aptRectAndLineTLV[n+0].dvSX = (float)(rc.left  * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+0].dvSY = (float)(rc.top   * trH/srH + _rc3DCanvas.top);
					m_aptRectAndLineTLV[n+1].dvSX = (float)(rc.right * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+1].dvSY = (float)(rc.bottom* trH/srH + _rc3DCanvas.top);
					m_aptRectAndLineTLV[n+2].dvSX = (float)(rc.left  * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+2].dvSY = (float)(rc.bottom* trH/srH + _rc3DCanvas.top);
					m_aptRectAndLineTLV[n+3].dvSX = (float)(rc.left  * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+3].dvSY = (float)(rc.top   * trH/srH + _rc3DCanvas.top);
					m_aptRectAndLineTLV[n+4].dvSX = (float)(rc.right * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+4].dvSY = (float)(rc.top   * trH/srH + _rc3DCanvas.top);
					m_aptRectAndLineTLV[n+5].dvSX = (float)(rc.right * trW/srW + _rc3DCanvas.left);
					m_aptRectAndLineTLV[n+5].dvSY = (float)(rc.bottom* trH/srH + _rc3DCanvas.top);
				
					n += 6;
					pRc++;
				}

				SetVertexColorValue( m_aptRectAndLineTLV, 0, n, _aptDrawRecords->colorValue );
				SetVertexZValue( m_aptRectAndLineTLV, 0, n, _foZValue );
				EnableAlpha(m_pd3dDevice);

				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, m_aptRectAndLineTLV, n, NULL );
			}
			break;

		case DRT_TEXT://text
			{
				if( _aptDrawRecords->text.pText && _aptDrawRecords->text.pText[0] )
				{
					int trW = _rc3DCanvas.right  - _rc3DCanvas.left;
					int trH = _rc3DCanvas.bottom - _rc3DCanvas.top;
					int srW = _aptDrawRecords->srcSize.cx;
					int srH = _aptDrawRecords->srcSize.cy;

					POINT pt = _aptDrawRecords->text.pt;
					pt.x = pt.x * trW/(srW+1)+ _rc3DCanvas.left;
					pt.y = pt.y * trH/(srH+1)+ _rc3DCanvas.top;

					DrawText( (float)pt.x,
							   (float)pt.y,
							   _rc3DCanvas,
							   _aptDrawRecords->colorValue,
							   _aptDrawRecords->text.pText, 
							   _aptDrawRecords->text.fScale );
				}
			}
			break;
		case DRT_CIRCLE: //circle
			{
				unsigned short i;
				unsigned short t1;
				POINT p1,p2;
				POINT centre = _aptDrawRecords->circle.ptCentre;
				double radius = _aptDrawRecords->circle.fRadius;
				BOOL fill =  _aptDrawRecords->circle.bFill;
				int srW = _aptDrawRecords->srcSize.cx;
				int srH = _aptDrawRecords->srcSize.cy;
				int alpha =  _aptDrawRecords->colorValue>>24 ;
				MYPOLYGON ptPolygonList;
				tDRAWRECORD tRecord;

				if ((centre.x-radius)<0 || (centre.x+radius)>srW || (centre.y-radius)<0 ||(centre.y+radius)>srH) {
					fRet = -1;
					goto exit;
				}
				
				memcpy( &tRecord, _aptDrawRecords, sizeof( tDRAWRECORD ) );
				tRecord.polygons.aptPolygon = &ptPolygonList;
				tRecord.polygons.nPolygon = 1;

				
				// first 3 points
				{
					p1.x = (LONG) (centre.x - radius);
					p1.y = centre.y;
					ptPolygonList.atpoint[0] = p1;

					i = (unsigned short) (centre.x - radius + 1);
					p1.x = i;
					p2.x = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.x)*(i-centre.x));
					p1.y = centre.y - t1;
					p2.y = centre.y + t1;
					ptPolygonList.atpoint[1] = p1;
					ptPolygonList.atpoint[2] = p2;
					ptPolygonList.cnt_point = 3;

					if ( ( p1.x != p2.x ) || ( p1.y != p2.y ) )
						DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
				}

				// Middle part
				ptPolygonList.cnt_point = 4;
				ptPolygonList.atpoint[2] = p1;
				ptPolygonList.atpoint[3] = p2;
				for (i = (unsigned short) (centre.x - radius + 2); i < centre.x + radius; i++)
				{	
					p1.x = i;
					p2.x = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.x)*(i-centre.x));
					p1.y = centre.y - t1;
					p2.y = centre.y + t1;

					ptPolygonList.atpoint[0] = ptPolygonList.atpoint[3];
					ptPolygonList.atpoint[1] = ptPolygonList.atpoint[2];

					ptPolygonList.atpoint[2] = p1;
					ptPolygonList.atpoint[3] = p2;
					DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
				}

				// last 3 points
				{
					p1.x = (LONG) (centre.x + radius);
					p1.y = centre.y;
					ptPolygonList.atpoint[2] = p1;

					i = (unsigned short) (centre.x + radius - 1);
					p1.x = i;
					p2.x = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.x)*(i-centre.x));
					p1.y = centre.y - t1;
					p2.y = centre.y + t1;
					ptPolygonList.atpoint[0] = p2;
					ptPolygonList.atpoint[1] = p1;
					ptPolygonList.cnt_point = 3;

					if ( ( p1.x != p2.x ) || ( p1.y != p2.y ) )
						DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
				}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
				// first 3 points
				{
					p1.x = centre.x;
					p1.y = (LONG) (centre.y - radius);
					ptPolygonList.atpoint[0] = p1;

					i =  (unsigned short) (centre.y - radius + 1);
					p1.y = i;
					p2.y = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.y)*(i-centre.y));
					p1.x = centre.x - t1;
					p2.x = centre.x + t1;
					ptPolygonList.atpoint[1] = p1;
					ptPolygonList.atpoint[2] = p2;
					ptPolygonList.cnt_point = 3;
					if ( ( p1.x != p2.x ) || ( p1.y != p2.y ) )
						DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
	//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
				}
				
				// Middle part
				ptPolygonList.cnt_point = 4;
				ptPolygonList.atpoint[2] = p1;
				ptPolygonList.atpoint[3] = p2;
				for (i = (unsigned short)(centre.y - radius + 2); i < centre.y + radius; i++)
				{	
					p1.y = i;
					p2.y = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.y)*(i-centre.y));
					p1.x = centre.x - t1;
					p2.x = centre.x + t1;

					ptPolygonList.atpoint[0] = ptPolygonList.atpoint[3];
					ptPolygonList.atpoint[1] = ptPolygonList.atpoint[2];

					ptPolygonList.atpoint[2] = p1;
					ptPolygonList.atpoint[3] = p2;
					DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
				}
				// last 3 points
				{
					p1.x = centre.x;
					p1.y = (LONG) (centre.y + radius);
					ptPolygonList.atpoint[2] = p1;

					i =  (unsigned short) (centre.y - radius + 1);
					p1.y = i;
					p2.y = i;
					t1 = (unsigned short) sqrt(radius*radius-(i-centre.y)*(i-centre.y));
					p1.x = centre.x - t1;
					p2.x = centre.x + t1;
					ptPolygonList.atpoint[1] = p1;
					ptPolygonList.atpoint[2] = p2;
					ptPolygonList.cnt_point = 3;
					if ( ( p1.x != p2.x ) || ( p1.y != p2.y ) )
						DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
	//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
				}
			}
			break;
		//______________________________________________________
		//	draw a circular arc between the two small arrows for zone stuff.
		case DRT_CIRCULARARC:
			{
				CD3d::MYPOLYGON ptPolygonList;
				tDRAWRECORD tRecord;
				POINT		   centre  = _aptDrawRecords->circularArc.ptCentre;
				double		   radius  = _aptDrawRecords->circularArc.fRadius;
				double		   dStartRadius  = _aptDrawRecords->circularArc.dStartRadius;
				double		   dFinishRadius = _aptDrawRecords->circularArc.dFinishRadius;
				unsigned short usWidth = _aptDrawRecords->circularArc.usWidth;
				unsigned short usHalfWidth = (unsigned short) ((float)(usWidth - 1)*0.5);

				int srW = _aptDrawRecords->srcSize.cx;
				int srH = _aptDrawRecords->srcSize.cy;
				int alpha =  _aptDrawRecords->colorValue>>24 ;
				
				double dCurrentRadius = dStartRadius;

				if ((centre.x-radius)<0 || (centre.x+radius)>srW || (centre.y-radius)<0 ||(centre.y+radius)>srH) {
					fRet = -1;
					goto exit;
				}

				memcpy( &tRecord, _aptDrawRecords, sizeof( tDRAWRECORD ) );
				tRecord.polygons.aptPolygon = &ptPolygonList;
				tRecord.polygons.nPolygon = 1;

				ptPolygonList.cnt_point = 4;

				while(dCurrentRadius + RADIUS_STEP < dFinishRadius)
				{
					ptPolygonList.atpoint[0].x = (LONG) (( radius - usHalfWidth ) * cos(dCurrentRadius) + centre.x);
					ptPolygonList.atpoint[0].y = (LONG) (-( radius - usHalfWidth ) * sin(dCurrentRadius) + centre.y);		//-- "-" suggests the screen coordinates is different.

					ptPolygonList.atpoint[1].x = (LONG) (( radius + usHalfWidth ) * cos(dCurrentRadius) + centre.x);
					ptPolygonList.atpoint[1].y = (LONG) (-( radius + usHalfWidth ) * sin(dCurrentRadius) + centre.y);
		
					ptPolygonList.atpoint[2].x = (LONG) (( radius + usHalfWidth ) * cos(dCurrentRadius + RADIUS_STEP) + centre.x);
					ptPolygonList.atpoint[2].y = (LONG) (-( radius + usHalfWidth ) * sin(dCurrentRadius + RADIUS_STEP) + centre.y);
				
					ptPolygonList.atpoint[3].x = (LONG) (( radius - usHalfWidth ) * cos(dCurrentRadius + RADIUS_STEP) + centre.x);
					ptPolygonList.atpoint[3].y = (LONG) (-( radius - usHalfWidth ) * sin(dCurrentRadius + RADIUS_STEP) + centre.y);

					DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
//					m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );

					dCurrentRadius += RADIUS_STEP;
				}

				ptPolygonList.atpoint[0].x = (LONG) (( radius - usHalfWidth ) * cos(dCurrentRadius - RADIUS_STEP) + centre.x);
				ptPolygonList.atpoint[0].y = (LONG) (-( radius - usHalfWidth ) * sin(dCurrentRadius - RADIUS_STEP) + centre.y);

				ptPolygonList.atpoint[1].x = (LONG) (( radius + usHalfWidth ) * cos(dCurrentRadius - RADIUS_STEP) + centre.x);
				ptPolygonList.atpoint[1].y = (LONG) (-( radius + usHalfWidth ) * sin(dCurrentRadius - RADIUS_STEP) + centre.y);
		
				ptPolygonList.atpoint[2].x = (LONG) (( radius + usHalfWidth ) * cos(dFinishRadius) + centre.x);
				ptPolygonList.atpoint[2].y = (LONG) (-( radius + usHalfWidth ) * sin(dFinishRadius) + centre.y);
				
				ptPolygonList.atpoint[3].x = (LONG) (( radius - usHalfWidth ) * cos(dFinishRadius) + centre.x);
				ptPolygonList.atpoint[3].y = (LONG) (-( radius - usHalfWidth ) * sin(dFinishRadius) + centre.y);

				DrawPolygonVCA( _rc3DCanvas, _foZValue, 1, &tRecord );
//				m_DrawPolygonList( _rc3DCanvas, _aptDrawRecords->srcSize, &ptPolygonList, 1, alpha, _aptDrawRecords->colorValue - (alpha<<24) );
			}
			break;
		}//switch( _aptDrawRecords->drawType )

		_aptDrawRecords++;

	}//for( ULONG i=0; i<_nRecordCount; i++ )
exit:	
	m_pd3dDevice->EndScene();
	LeaveCriticalSection( &m_crit );
	

	return fRet;
}

BOOL CD3d::DrawImageTexture( LPDIRECTDRAWSURFACE7 const _pDDSurface, DWORD width, DWORD height,
							 RECT _rc3DCanvas, RECT _rcSrc, DWORD _dwColor, D3DVALUE _foZValue, eROTATE rotate )
{
	/*
	여기서는 이전 단계에서 만들어진 Texture Buffer의 내용을 3DCanvas(3DDevice Attached BackBuffer)
	로 옮긴다. 실제 3D Engine이 가동되고 ScaleUpDown도 발생한다.
	*/
	HRESULT hr;

	// 3DCanvas의 어디로 옮길것인지 결정한다.
	int x1 = _rc3DCanvas.left;
	int y1 = _rc3DCanvas.top;
	int x2 = _rc3DCanvas.right;
	int y2 = _rc3DCanvas.bottom;

	float fx1 = (float)x1 - 0.5f;
	float fy1 = (float)y1 - 0.5f;
	float fx2 = (float)x2 - 0.5f;
	float fy2 = (float)y2 - 0.5f;

	// 입력Texture 좌표.. 일단 pixel좌표를 쓴다.
	float u1 = (float)_rcSrc.left;
	float v1 = (float)_rcSrc.top;
	float u2 = (float)_rcSrc.right;
	float v2 = (float)_rcSrc.bottom;

	SetVertexScreenXYZ( m_aptImageTLV, 0, 1, D3DVECTOR( fx1, fy2, _foZValue ) );
	SetVertexScreenXYZ( m_aptImageTLV, 1, 1, D3DVECTOR( fx1, fy1, _foZValue ) );
	SetVertexScreenXYZ( m_aptImageTLV, 2, 1, D3DVECTOR( fx2, fy2, _foZValue ) );
	SetVertexScreenXYZ( m_aptImageTLV, 3, 1, D3DVECTOR( fx2, fy1, _foZValue ) );

	SetVertexColorValue( m_aptImageTLV, 0, 4, _dwColor );

	u1 /= width;
	v1 /= height;
	u2 /= width;
	v2 /= height;
	if(RT_000 == rotate){
		SetVertexTextureXY( m_aptImageTLV, 0, 1, u1, v2 );
		SetVertexTextureXY( m_aptImageTLV, 1, 1, u1, v1 );
		SetVertexTextureXY( m_aptImageTLV, 2, 1, u2, v2 );
		SetVertexTextureXY( m_aptImageTLV, 3, 1, u2, v1 );
	}else if(RT_090 == rotate){
		SetVertexTextureXY( m_aptImageTLV, 0, 1, u2, v2 );
		SetVertexTextureXY( m_aptImageTLV, 1, 1, u1, v2 );
		SetVertexTextureXY( m_aptImageTLV, 2, 1, u2, v1 );
		SetVertexTextureXY( m_aptImageTLV, 3, 1, u1, v1 );
	}else if(RT_180 == rotate){
		SetVertexTextureXY( m_aptImageTLV, 0, 1, u2, v1 );
		SetVertexTextureXY( m_aptImageTLV, 1, 1, u2, v2 );
		SetVertexTextureXY( m_aptImageTLV, 2, 1, u1, v1 );
		SetVertexTextureXY( m_aptImageTLV, 3, 1, u1, v2 );
	}else{//270
		SetVertexTextureXY( m_aptImageTLV, 0, 1, u1, v1 );
		SetVertexTextureXY( m_aptImageTLV, 1, 1, u2, v1 );
		SetVertexTextureXY( m_aptImageTLV, 2, 1, u1, v2 );
		SetVertexTextureXY( m_aptImageTLV, 3, 1, u2, v2 );
	}

    // Draw the front and back faces of the cube using texture 1
	hr = m_pd3dDevice->SetTexture( 0, _pDDSurface );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

    hr = m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 
									  D3DFVF_TLVERTEX,
									  m_aptImageTLV,
									  4,
									  NULL );
	return 0;
}


BOOL CD3d::FindSuitableTextureSurface( INT _nTargetWidth, INT _nTargetHeight )
{
	BOOL	fFind = FALSE;

	int w = _nTargetWidth;
	int h = _nTargetHeight;

#if 0
	// 적당한것을 고른다.
	if     (w<= 256 && h<= 128){ m_pddsTexture = m_apddsTextures[0]; m_dwTextureWidth= 256;m_dwTextureHeight= 128; fFind = TRUE; }
	else if(w<= 256 && h<= 256){ m_pddsTexture = m_apddsTextures[1]; m_dwTextureWidth= 256;m_dwTextureHeight= 256; fFind = TRUE; }
	else if(w<= 512 && h<= 128){ m_pddsTexture = m_apddsTextures[2]; m_dwTextureWidth= 512;m_dwTextureHeight= 128; fFind = TRUE; }
	else if(w<= 512 && h<= 256){ m_pddsTexture = m_apddsTextures[3]; m_dwTextureWidth= 512;m_dwTextureHeight= 256; fFind = TRUE; }
	else if(w<= 512 && h<= 512){ m_pddsTexture = m_apddsTextures[4]; m_dwTextureWidth= 512;m_dwTextureHeight= 512; fFind = TRUE; }
	else if(w<=1024 && h<= 256){ m_pddsTexture = m_apddsTextures[5]; m_dwTextureWidth=1024;m_dwTextureHeight= 256; fFind = TRUE; }
	else if(w<=1024 && h<= 512){ m_pddsTexture = m_apddsTextures[6]; m_dwTextureWidth=1024;m_dwTextureHeight= 512; fFind = TRUE; }
	else 					   { m_pddsTexture = m_apddsTextures[7]; m_dwTextureWidth=1024;m_dwTextureHeight=1024; fFind = TRUE; }

#else
	//
	// VIA chipset 마더보드의 built-in VGA인 경우 1024 x 1024만 열린다.
	//
	// 가장 크게 잡은 1024만 사용한다.
	//
	fFind = TRUE;
	m_pddsTexture	   = m_apddsTextures[7];

	if( w > (INT)m_dwTextureWidth || h > (INT)m_dwTextureHeight ) {
		fFind = FALSE;
	}
#endif

	return fFind;
}


ULONG CD3d::Triangulate( RECT _rc3DCanvas, tDRAWRECORD*	_aptDrawRecords )
{
	unsigned i, j;
	unsigned tcount;
	Vector2dVector polygonpt, triangleresult;

	MYPOLYGON* aptPoly = _aptDrawRecords->polygons.aptPolygon;

	int trW = _rc3DCanvas.right  - _rc3DCanvas.left;
	int trH = _rc3DCanvas.bottom - _rc3DCanvas.top;
	int srW = _aptDrawRecords->srcSize.cx;
	int srH = _aptDrawRecords->srcSize.cy;
	ULONG n = 0;

	for ( i = 0; i < _aptDrawRecords->polygons.nPolygon; i ++ )
	{
		if ( aptPoly->cnt_point > 3 )
		{
			for ( j = 0; j < aptPoly->cnt_point; j++ )
				polygonpt.push_back( Vector2d( (float) aptPoly->atpoint[j].x,  (float) aptPoly->atpoint[j].y ) );

			if ( !Triangulate::Process( polygonpt, triangleresult ) )
			{
	//			AfxMessageBox(_T("Failed Triangulation"));
			}

			tcount = triangleresult.size()/3;  
		}
		else if ( aptPoly->cnt_point == 3 )
		{
			for ( j = 0; j < 3; j++ )
				triangleresult.push_back( Vector2d( (float) aptPoly->atpoint[j].x, (float) aptPoly->atpoint[j].y ) );

			tcount = 1;
		}
		else
			return FALSE;

		for (j = 0; j < tcount; j++)
		{

			const Vector2d &p0 = triangleresult[j * 3 + 0];
			const Vector2d &p1 = triangleresult[j * 3 + 1];
			const Vector2d &p2 = triangleresult[j * 3 + 2];

			POINT pt0, pt1, pt2;
			pt0.x = (LONG)p0.GetX();
			pt0.y = (LONG)p0.GetY();

			pt1.x = (LONG)p1.GetX();
			pt1.y = (LONG)p1.GetY();

			pt2.x = (LONG)p2.GetX();
			pt2.y = (LONG)p2.GetY();

			pt0.x = max(0, pt0.x);
			pt0.x = min(_aptDrawRecords->srcSize.cx, pt0.x);
			pt0.y = max(0, pt0.y);
			pt0.y = min(_aptDrawRecords->srcSize.cy, pt0.y);

			pt1.x = max(0, pt1.x);
			pt1.x = min(_aptDrawRecords->srcSize.cx, pt1.x);
			pt1.y = max(0, pt1.y);
			pt1.y = min(_aptDrawRecords->srcSize.cy, pt1.y);

			pt2.x = max(0, pt2.x);
			pt2.x = min(_aptDrawRecords->srcSize.cx, pt2.x);
			pt2.y = max(0, pt2.y);
			pt2.y = min(_aptDrawRecords->srcSize.cy, pt2.y);

			// Triangle Vertex를 만든다.
			// 뿌릴 사각형을 삼각형 여러개로 표현한다.
			/*
			m_aptRectAndLineTLV[n + 0].dvSX = (float)(pt0.x * trW / srW + _rc3DCanvas.left);
			m_aptRectAndLineTLV[n + 0].dvSY = (float)(pt0.y * trH / srH + _rc3DCanvas.top);
			m_aptRectAndLineTLV[n + 1].dvSX = (float)(pt1.x * trW / srW + _rc3DCanvas.left);
			m_aptRectAndLineTLV[n + 1].dvSY = (float)(pt1.y * trH / srH + _rc3DCanvas.top);
			m_aptRectAndLineTLV[n + 2].dvSX = (float)(pt2.x * trW / srW + _rc3DCanvas.left);
			m_aptRectAndLineTLV[n + 2].dvSY = (float)(pt2.y * trH / srH + _rc3DCanvas.top);
			m_aptRectAndLineTLV[n + 3].dvSX = (float)(pt0.x * trW / srW + _rc3DCanvas.left);
			m_aptRectAndLineTLV[n + 3].dvSY = (float)(pt0.y * trH / srH + _rc3DCanvas.top);
			*/
			if (srW)
			{
				m_aptRectAndLineTLV[n + 0].dvSX = (float)(pt0.x * trW / srW + _rc3DCanvas.left);
				m_aptRectAndLineTLV[n + 1].dvSX = (float)(pt1.x * trW / srW + _rc3DCanvas.left);
				m_aptRectAndLineTLV[n + 2].dvSX = (float)(pt2.x * trW / srW + _rc3DCanvas.left);
				m_aptRectAndLineTLV[n + 3].dvSX = (float)(pt0.x * trW / srW + _rc3DCanvas.left);
			}
			if (srH )
			{
				m_aptRectAndLineTLV[n + 0].dvSY = (float)(pt0.y * trH / srH + _rc3DCanvas.top);
				m_aptRectAndLineTLV[n + 1].dvSY = (float)(pt1.y * trH / srH + _rc3DCanvas.top);
				m_aptRectAndLineTLV[n + 2].dvSY = (float)(pt2.y * trH / srH + _rc3DCanvas.top);
				m_aptRectAndLineTLV[n + 3].dvSY = (float)(pt0.y * trH / srH + _rc3DCanvas.top);
			}
			n += 4;
		}
		aptPoly++;
	}
	return n;
}



BOOL CD3d::UpdateBrushAlpha( eBRUSHTYPE brushType, BYTE _cbAlpha )
{
	// Lock the surface and transfer the bits...
	unsigned char *pBytes;

	DWORD dwPitch;
	if( GetSurfacePointer( m_pddsBrushTextures[brushType], NULL, &pBytes, &dwPitch ) )
	{
		unsigned char *pLine = pBytes;
		for( int y = 0; y < 16; y++, pLine += dwPitch )
		{
			unsigned int *pWord = (unsigned int *)pLine;

			// Modify the alpha slightly coz the textures don't show up so well
			unsigned int uiAlpha = _cbAlpha;

			uiAlpha = min( uiAlpha + 64, 255 );

			uiAlpha <<= 24;

			for( int x = 0; x < 16; x++, pWord++ )
			{
				// Simple test - if byte 0 is != 0 then we change the alpha
				// otherwise we assume it's totally transparent
				if( *pWord & 0x00FFFFFF )
				{
					*pWord = (*pWord & 0x00FFFFFF) + uiAlpha;
				}
			}
		}

		ReleaseSurfacePointer( m_pddsBrushTextures[brushType] );

	}
	else
	{
		ASSERT( FALSE );
	}

	return TRUE;
}


BOOL CD3d::DrawPolygonVCA( RECT			_rc3DCanvas,
							D3DVALUE		_foZValue,
							ULONG			_nRecordCount,
							tDRAWRECORD*	_aptDrawRecords )
{
	ULONG n,i;

	n = Triangulate( _rc3DCanvas, _aptDrawRecords );

	SetVertexColorValue( m_aptRectAndLineTLV, 0, n, _aptDrawRecords->colorValue );
	SetVertexZValue( m_aptRectAndLineTLV, 0, n, _foZValue );
	EnableAlpha(m_pd3dDevice);

	// See if we have to apply a texture
	if( BRT_SOLID != _aptDrawRecords->brushType )
	{
		ASSERT( _aptDrawRecords->brushType < MAXCOUNT_BRUSHTEXTURES );	// Overflow!

		UpdateBrushAlpha( _aptDrawRecords->brushType, (BYTE)(_aptDrawRecords->colorValue >> 24) );

		m_pd3dDevice->SetTexture( 0, m_pddsBrushTextures[ _aptDrawRecords->brushType ] );

		// Add offset for correct texel-pixel mapping
		for( i = 0; i < n; i++ )
		{
			_D3DTLVERTEX *pVtx = &m_aptRectAndLineTLV[i];

			SetVertexTextureXY( pVtx, 0, 1, pVtx->dvSX/BRUSH_TEXTURESIZE, pVtx->dvSY/BRUSH_TEXTURESIZE );

			pVtx->dvSX -= 0.5f;
			pVtx->dvSY -= 0.5f;
		}
	}

	for ( i = 0; i < n / 4; i ++ )
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_aptRectAndLineTLV[i*4], 4, NULL );

	m_pd3dDevice->SetTexture( 0, NULL );


	// BW - if we drew with a brush, then draw a line without texture round the edge to box it all in...
	if( BRT_SOLID != _aptDrawRecords->brushType )
	{
		MYPOLYGON *pPoly = _aptDrawRecords->polygons.aptPolygon;

		int l = 0;
		for( i = 0; i < _aptDrawRecords->polygons.nPolygon; i++, pPoly++ )
		{
			// Rescale the original co-ordinates into line segments

			int trW = _rc3DCanvas.right  - _rc3DCanvas.left;
			int trH = _rc3DCanvas.bottom - _rc3DCanvas.top;
			int srW = _aptDrawRecords->srcSize.cx;
			int srH = _aptDrawRecords->srcSize.cy;

			for( ULONG j = 0; j < pPoly->cnt_point; j++ )
			{
				m_aptRectAndLineTLV[l].dvSX = (float)(pPoly->atpoint[j].x * trW/srW + _rc3DCanvas.left);
				m_aptRectAndLineTLV[l].dvSY = (float)(pPoly->atpoint[j].y * trH/srH + _rc3DCanvas.top);
				l++;
			}

			// Connect the last segment
			m_aptRectAndLineTLV[l].dvSX = m_aptRectAndLineTLV[0].dvSX;
			m_aptRectAndLineTLV[l].dvSY = m_aptRectAndLineTLV[0].dvSY;
			l++;

		}

		m_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, D3DFVF_TLVERTEX, m_aptRectAndLineTLV, l, NULL );
	}

	return TRUE;
}



HRESULT CD3d::InitDrawCalib()
{
	///// sphere
	if (S_OK != D3DXCreateSphere(m_pd3dDevice, 0.1f, 10, 10, 0, &m_pSphereMesh)) return S_FALSE;

	///// cylinder
	if (S_OK != D3DXCreateCylinder(m_pd3dDevice, 0.1f, 0.1f, 1.0f, 10, 5, 0, &m_pCylinderMesh)) return S_FALSE;

	/// cone
	if (S_OK != D3DXCreateCylinder(m_pd3dDevice, 0.15f, 0.25f, 1.0f, 10, 5, 0, &m_pConeMesh)) return S_FALSE;

/*	
	memset(&m_materialPerson[0], 0, sizeof(D3DMATERIAL7));
	m_materialPerson[0].specular.r = 1.0f;
	m_materialPerson[0].specular.g = 1.0f;
	m_materialPerson[0].specular.b = 1.0f;
	m_materialPerson[0].specular.a = 0.0f;
	m_materialPerson[0].power = 40.0f;
	m_materialPerson[0].diffuse.r = 0.2f;
	m_materialPerson[0].diffuse.g = 0.2f;
	m_materialPerson[0].diffuse.b = 0.8f;
	m_materialPerson[0].diffuse.a = 1.0f;
	m_materialPerson[0].ambient.r = 0.2f;
	m_materialPerson[0].ambient.g = 0.2f;
	m_materialPerson[0].ambient.b = 0.8f;
	m_materialPerson[0].ambient.a = 1.0f;

	memset(&m_materialPerson[1], 0, sizeof(D3DMATERIAL7));
	m_materialPerson[1].specular.r = 1.0f;
	m_materialPerson[1].specular.g = 1.0f;
	m_materialPerson[1].specular.b = 1.0f;
	m_materialPerson[1].specular.a = 0.0f;
	m_materialPerson[1].power = 40.0f;
	m_materialPerson[1].diffuse.r = 0.8f;
	m_materialPerson[1].diffuse.g = 0.2f;
	m_materialPerson[1].diffuse.b = 0.2f;
	m_materialPerson[1].diffuse.a = 1.0f;
	m_materialPerson[1].ambient.r = 0.8f;
	m_materialPerson[1].ambient.g = 0.2f;
	m_materialPerson[1].ambient.b = 0.2f;
	m_materialPerson[1].ambient.a = 1.0f;
*/
	//// light
	ZeroMemory(&m_light, sizeof(D3DLIGHT7));
	m_light.dltType			= D3DLIGHT_DIRECTIONAL;
	m_light.dcvDiffuse.r	= m_light.dcvSpecular.r = 1.0f;
	m_light.dcvDiffuse.g	= m_light.dcvSpecular.g = 1.0f;
	m_light.dcvDiffuse.b	= m_light.dcvSpecular.b = 1.0f;
	m_light.dvPosition.x	= 0.0f;
	m_light.dvPosition.y	= 0.0f;
	m_light.dvPosition.z	= 0.0f;
	m_light.dvDirection.x	= -0.5f;
	m_light.dvDirection.y	= 0.5f;
	m_light.dvDirection.z	= -1.0f;

	// Don't attenuate.
	m_light.dvAttenuation0 = 1.0f; 
	m_light.dvRange        = D3DLIGHT_RANGE_MAX;

	m_vGroundVerticies[0] = D3DVERTEX(D3DVECTOR(-10.0f,  10.0f, 0.0f), D3DVECTOR(0.0f, 0.0f, 1.0f), -5.0f,	5.0f);
	m_vGroundVerticies[1] = D3DVERTEX(D3DVECTOR( 10.0f,  10.0f, 0.0f), D3DVECTOR(0.0f, 0.0f, 1.0f),  5.0f,	5.0f);
	m_vGroundVerticies[2] = D3DVERTEX(D3DVECTOR(-10.0f, -10.0f, 0.0f), D3DVECTOR(0.0f, 0.0f, 1.0f), -5.0f, -5.0f);
	m_vGroundVerticies[3] = D3DVERTEX(D3DVECTOR( 10.0f, -10.0f, 0.0f), D3DVECTOR(0.0f, 0.0f, 1.0f),  5.0f, -5.0f);


	m_vHorizonVerticies[0] =  D3DLVERTEX(D3DVECTOR(-510.0f,  510.0f, 0.0f), 0xFF00FF00, 0, 0.0f,	0.0f);
	m_vHorizonVerticies[1] =  D3DLVERTEX(D3DVECTOR(510.0f,  510.0f, 0.0f), 0xFF00FF00, 0, 0.0f,		0.0f);
	m_vHorizonVerticies[2] =  D3DLVERTEX(D3DVECTOR(510.0f,  -510.0f, 0.0f), 0xFF00FF00, 0, 0.0f,	0.0f);
	m_vHorizonVerticies[3] =  D3DLVERTEX(D3DVECTOR(-510.0f,  -510.0f, 0.0f), 0xFF00FF00, 0, 0.0f,	0.0f);
	m_vHorizonVerticies[4] =  D3DLVERTEX(D3DVECTOR(-510.0f,  510.0f, 0.0f), 0xFF00FF00, 0, 0.0f,	0.0f);

	InitGroundTexture();
	LoadTextureFromResource(&m_pddsRulerTexture, IDB_RULER, 16);

	D3DXUninitialize();
	return S_OK;
}


HRESULT CD3d::DeInitDrawCalib()
{
	SAFE_RELEASE(m_pSphereMesh);
	SAFE_RELEASE(m_pCylinderMesh);
	SAFE_RELEASE(m_pConeMesh);
	SAFE_RELEASE(m_pddsGroundTexture );
	SAFE_RELEASE(m_pddsRulerTexture );
		
	return S_OK;
}


void CD3d::DrawGroundPlane(BOOL bSkyEnabled, BOOL bTransparentGrid)
{
	D3DXMATRIX matLocal, matRotZ;
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_ANISOTROPIC );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_LINEAR );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXANISOTROPY, m_d2dDevDesc.dwMaxAnisotropy); 


	if (bSkyEnabled)
	{
		//D3DXMATRIX matRotation, matProj;
		//D3DXMatrixRotationX(&matRotation, m_vecGridRot.x);
		//D3DXMatrixIdentity(&matLocal);

		m_pd3dDevice->SetTexture( 0, NULL );

		//m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *) &m_matView_noTrans);
		//m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &matLocal);
		m_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, D3DFVF_LVERTEX , m_vHorizonVerticies, 5, 0);
		//m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *) &m_matView);
	}

	if (m_pddsGroundTexture)
	{
		/// set up texture render to mutiply texture alpha with constant TEXTUREFACTOR alpha 
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE  );
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, bTransparentGrid?  0x40000000 : 0xFF000000 ); // set total alpha for this texture
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);  
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		// wrap texture to repeate grid
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);

		m_pd3dDevice->SetTexture( 0, m_pddsGroundTexture );

		for (int j = 25; j >= -25; j--){
			for (int i = -20; i <= 20; i++)
			{
				D3DXMatrixTranslation(&matLocal, i*20.0f, j*20.0f, 0.0f);
				m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &matLocal);
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_vGroundVerticies, 4, 0 );
			}
		}
		// disable alpha multiply
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	}
}


void CD3d::DrawPerson(int iModelIdx, ObjectModel *pObj)
{

	D3DXMATRIX matModelRotation, matTranslation, matScaling, matRotation;
	D3DXMATRIX matLocal, matModel;
	D3DMATERIAL7 *pMaterialPerson;

	//SetupModelMatrix(iModelIdx);
	{
		D3DXMATRIX matRotationY, matTranslation, matModelTranslation, matTranslation2;
	
		D3DXMatrixRotationY(&matRotationY, pObj->vecRot.z);
		D3DXMatrixTranslation(&matTranslation, 0, 0, -m_fHumanHeight/2);
		D3DXMatrixTranslation(&matTranslation2, 0, 0, m_fHumanHeight/2);

		matRotationY = matTranslation*matRotationY*matTranslation2;

		D3DXMatrixTranslation(&matModelTranslation, pObj->vecPos.x, pObj->vecPos.y, 0);
		pObj->matObjectModel = matRotationY*matModelTranslation;
	}

	matModel		= pObj->matObjectModel;
	pMaterialPerson = &pObj->materialPerson;
	if ( pObj->bHighlighted){
		pMaterialPerson->emissive.r = min(0.3f, pMaterialPerson->emissive.r+0.05f);
		pMaterialPerson->emissive.g = min(0.3f, pMaterialPerson->emissive.g+0.05f);
		pMaterialPerson->emissive.b = min(0.3f, pMaterialPerson->emissive.b+0.05f);
	}else{
		pMaterialPerson->emissive.r = max(0.0f, pMaterialPerson->emissive.r-0.05f);
		pMaterialPerson->emissive.g = max(0.0f, pMaterialPerson->emissive.g-0.05f);
		pMaterialPerson->emissive.b = max(0.0f, pMaterialPerson->emissive.b-0.05f);
	}

	m_pd3dDevice->SetMaterial(pMaterialPerson);
	m_pd3dDevice->SetTexture( 0, NULL );

	/// head
	D3DXMatrixScaling(&matScaling, 1.5f, 1.5f, 1.5f); 
	D3DXMatrixTranslation(&matTranslation, 0, 0, m_fHumanHeight-0.125f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	/// feet
	float legangle = D3DXToRadian(8);
	float leglength = 0.85f;
	float feetpos = 0.17f;
	float legwidth = 0.9f;

	D3DXMatrixScaling(&matScaling, legwidth, legwidth, legwidth); 
	D3DXMatrixTranslation(&matTranslation, -feetpos, 0.0f, 0.1f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	D3DXMatrixTranslation(&matTranslation, feetpos, 0.0f, 0.1f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	/// legs
	D3DXMatrixScaling(&matScaling, legwidth, legwidth, leglength); 
	D3DXMatrixTranslation(&matTranslation, feetpos, 0.0f, 0.1f);
	D3DXMatrixRotationY(&matRotation, -legangle);
	matLocal = matScaling*matRotation*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pCylinderMesh->Draw();

	D3DXMatrixTranslation(&matTranslation, -feetpos, 0.0f, 0.1f);
	D3DXMatrixRotationY(&matRotation, legangle);
	matLocal = matScaling*matRotation*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pCylinderMesh->Draw();

	/// arms & hands
	float armangle = D3DXToRadian(13);
	float armlength = 0.55f;
	float handpos	= 0.3f;
	float handheight = 0.9f;
	float armwidth = 0.6f;

	D3DXMatrixScaling(&matScaling, armwidth, armwidth, armwidth); 
	D3DXMatrixTranslation(&matTranslation, -handpos, 0.0f, handheight);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	D3DXMatrixTranslation(&matTranslation, handpos, 0.0f, handheight);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	D3DXMatrixScaling(&matScaling, armwidth, armwidth, armlength); 
	D3DXMatrixTranslation(&matTranslation, handpos, 0.0f, handheight);
	D3DXMatrixRotationY(&matRotation, -armangle);
	matLocal = matScaling*matRotation*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pCylinderMesh->Draw();

	D3DXMatrixTranslation(&matTranslation, -handpos, 0.0f, handheight);
	D3DXMatrixRotationY(&matRotation, armangle);
	matLocal = matScaling*matRotation*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pCylinderMesh->Draw();

	/// body
	D3DXMatrixScaling(&matScaling, 1.0f, 0.5f, 0.65f); 
	D3DXMatrixTranslation(&matTranslation, 0.0f, 0.0f, 0.8f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pConeMesh->Draw();

	D3DXMatrixScaling(&matScaling, 1.0f, 0.5f, 0.05f); 
	D3DXMatrixTranslation(&matTranslation, 0.0f, 0.0f, 1.5f);
	D3DXMatrixRotationY(&matRotation, D3DX_PI);
	matLocal = matScaling*matRotation*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pConeMesh->Draw();

	/* uncomment this for a female version ;)

	/// titties
	D3DXMatrixScaling(&matScaling, 1.0f, 1.0f, 1.0f); 
	D3DXMatrixTranslation(&matTranslation, -0.1f, -0.1f, 1.3f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	D3DXMatrixScaling(&matScaling, 1.0f, 1.0f, 1.0f);
	D3DXMatrixTranslation(&matTranslation, 0.1f, -0.1f, 1.3f);
	matLocal = matScaling*matTranslation;                                                                           
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &(matLocal*matModel));
	m_pSphereMesh->Draw();

	*/
}


BOOL CD3d::InitGroundTexture()
{
	const int		bmpsize = 64;
	DWORD	transparent_colour = 0x0000FF01;

	LPDIRECTDRAWSURFACE7 pddsTemp;

	D3DXInitialize();
	
	m_pd3dDevice->GetCaps(&m_d2dDevDesc);

	unsigned char bits[ bmpsize * bmpsize * 3 + 4];

	pddsTemp = CreateCustomSurface( m_pDD, SURFACETYPE_MANAGEDTEXTURE, &DDPF_RGBA32, bmpsize, bmpsize );
	if( !pddsTemp )
	{
	//	ASSERT( FALSE );
		return FALSE;
	}

	return true;
	// Load the bitmap
	HBITMAP hBmp = NULL;// LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_GROUND));
	if( hBmp == NULL)
	{
		TCHAR buf[100];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, buf, 100, 0);
		MessageBox( NULL, buf,
			TEXT("Load"), MB_OK|MB_ICONERROR );
		return FALSE;
	}

	BITMAPINFO bmi;
	memset( &bmi, 0, sizeof( BITMAPINFO ) );
	bmi.bmiHeader.biSize		= sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth		= bmpsize;
	bmi.bmiHeader.biHeight		= bmpsize;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biBitCount	= 24;

	HDC hDC = NULL;// ::GetDC(AfxGetMainWnd()->GetSafeHwnd());
	::GetDIBits( hDC, hBmp, 0, bmpsize, bits, &bmi, DIB_RGB_COLORS );
	::ReleaseDC( NULL, hDC );
	DeleteObject(hBmp);

	// Lock the surface and transfer the bits...
	unsigned char *pBytes;
	unsigned char ucAlpha = 0xF0;
	DWORD dwPitch;
	if( GetSurfacePointer( pddsTemp, NULL, &pBytes, &dwPitch ) )
	{
		unsigned char *pLine = pBytes;
		for( int y = 0; y < bmpsize; y++, pLine += dwPitch )
		{
			unsigned char *pByte = pLine;
			for( int x = 0; x < bmpsize; x++ )
			{
				int offset = ((y*bmpsize) + x) * 3;
				*pByte++ = bits[ offset + 0 ];
				*pByte++ = bits[ offset + 1 ];
				*pByte++ = bits[ offset + 2 ];
				*pByte++ = ((*(unsigned*)&bits[offset]) & 0x00FFFFFF) == transparent_colour ? 0x00:0xFF;
			}
		}

		ReleaseSurfacePointer( pddsTemp );

	}
	else
	{
		ASSERT( FALSE );
	}

	DWORD width = bmpsize;
	DWORD height = bmpsize;
	D3DX_SURFACEFORMAT fmt = D3DX_SF_A8R8G8B8;

	//VERIFY(SUCCEEDED (D3DXCreateTexture(m_pd3dDevice, NULL, &width, &height, &fmt, NULL, &m_pddsGroundTexture,  NULL)));
	//VERIFY(SUCCEEDED( D3DXLoadTextureFromSurface(m_pd3dDevice, m_pddsGroundTexture, D3DX_DEFAULT, pddsTemp, NULL, NULL, D3DX_FT_DEFAULT)));
	pddsTemp->Release();
	return TRUE;
}


BOOL CD3d::LoadTextureFromResource(LPDIRECTDRAWSURFACE7 *ppddsTexture, DWORD idRes, DWORD bmpsize)
{
	unsigned char *bits = new unsigned char[ bmpsize * bmpsize * 3 + 4];
	D3DX_SURFACEFORMAT fmt = D3DX_SF_R8G8B8; 
	return false;
		// Load the bitmap
	HBITMAP hBmp = NULL;// LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(idRes));
//	ASSERT( hBmp );

	BITMAPINFO bmi;
	memset( &bmi, 0, sizeof( BITMAPINFO ) );
	bmi.bmiHeader.biSize		= sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth		= bmpsize;
	bmi.bmiHeader.biHeight		= bmpsize;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biBitCount	= 24;

	HDC hDC = NULL;// ::GetDC(AfxGetMainWnd()->GetSafeHwnd());
	::GetDIBits( hDC, hBmp, 0, bmpsize, bits, &bmi, DIB_RGB_COLORS );
	::ReleaseDC( NULL, hDC );

	DeleteObject(hBmp);

	if (!SUCCEEDED (D3DXCreateTexture(m_pd3dDevice, NULL, &bmpsize, &bmpsize, &fmt, NULL, ppddsTexture,  NULL)))
	{
		delete [] bits;
		return FALSE;
	}

	fmt = D3DX_SF_R8G8B8;
	if(!SUCCEEDED( D3DXLoadTextureFromMemory(m_pd3dDevice, *ppddsTexture, D3DX_DEFAULT, bits, NULL, fmt, bmpsize*3, NULL, D3DX_FT_DEFAULT )))
	{
		delete [] bits;
		return FALSE;
	}

	delete [] bits;
	return TRUE;
}


float CD3d::GetLength(D3DXVECTOR3 *pPos)
{
	float dx = pPos[0].x - pPos[1].x;
	float dy = pPos[0].y - pPos[1].y;
	return sqrt(dx*dx + dy*dy);
}


void CD3d::DrawRuler(D3DXVECTOR3 *pPos)
{
	D3DVERTEX vRulerVerticies[4];
	D3DXMATRIX matLocal;

	float length = GetLength(pPos)*2.5f;

	float x1 = pPos[0].x;
	float y1 = pPos[0].y;
	float x2 = pPos[1].x;
	float y2 = pPos[1].y;

	float fTheta = atan2( (float)(y2-y1), (float)(x2-x1) );
	float fx = sin(fTheta);
	float fy = cos(fTheta);
	float fxOff, fyOff;

	fxOff = 0.2f * fx;
	fyOff = 0.2f * fy;

	vRulerVerticies[0] = D3DVERTEX(D3DXVECTOR3(x1 - fxOff, y1 + fyOff, 0.0005f), D3DVECTOR(0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
	vRulerVerticies[1] = D3DVERTEX(D3DXVECTOR3(x2 - fxOff, y2 + fyOff, 0.0005f), D3DVECTOR(0.0f, 0.0f, 1.0f), length, 0.0f);
	vRulerVerticies[2] = D3DVERTEX(D3DXVECTOR3(x1 + fxOff, y1 - fyOff, 0.0005f), D3DVECTOR(0.0f, 0.0f, 1.0f), 0.0f, 1.0f);
	vRulerVerticies[3] = D3DVERTEX(D3DXVECTOR3(x2 + fxOff, y2 - fyOff, 0.0005f), D3DVECTOR(0.0f, 0.0f, 1.0f), length, 1.0f);

	D3DXMatrixIdentity(&matLocal);
	m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *)&matLocal );
	
//	m_pd3dDevice->SetMaterial(&m_materialPerson[0]);
	m_pd3dDevice->SetTexture( 0, m_pddsRulerTexture );

	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, vRulerVerticies, 4, 0);
}



