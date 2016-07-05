#include "stdafx.h"
#include "D3DCalibrationRender.h"
#include "math.h"
#include "VCADataMgr.h"

CCalibrationD3DRender::CCalibrationD3DRender(HWND hwnd) :m_hWnd(hwnd)
{
	m_RenderMode		= RENDER_VCA;
	m_pBackBufImage		= NULL;	
	m_bPauseImageCopied	= FALSE;

}	


CCalibrationD3DRender::~CCalibrationD3DRender(void)
{
	Endup();
}


BOOL	CCalibrationD3DRender::Setup(HWND hwnd,CVCADataMgr *pDataMgr)
{
	m_VCARender.Setup(m_hWnd, pDataMgr);

	m_pD3d = m_VCARender.m_pD3d;
	m_pDataMgr = pDataMgr;
	m_pClientRect = &(m_VCARender.m_ClientRect);

	return TRUE;
}


void	CCalibrationD3DRender::Endup()
{
	if(m_VCARender.m_bSetup){
		m_VCARender.Endup();
		
		m_pD3d = NULL;
		m_pDataMgr	= NULL;
		m_pClientRect = NULL;

		if(m_pBackBufImage){
			_aligned_free(m_pBackBufImage);
			m_pBackBufImage = NULL;
		}
	}
}


void	CCalibrationD3DRender::OnChangeClientRect(RECT rect)
{
	m_VCARender.OnChangeClientRect(rect);

}
BOOL	CCalibrationD3DRender::SetVCASourceInfo(int width, int height)
{
	BITMAPINFOHEADER bm;
	bm.biWidth = width;
	bm.biHeight = height;
	return ChangeVCASourceInfo(&bm);
}

BOOL	CCalibrationD3DRender::ChangeVCASourceInfo(BITMAPINFOHEADER *pbm)
{
	if(!m_VCARender.m_bSetup){
		//TRACE("CCalibrationD3DRender::ChangeVCASourceInfo Not setup before\n");
		//return FALSE;
	}

	if (m_VCARender.m_SourceSize.cx == pbm->biWidth&&m_VCARender.m_SourceSize.cy == pbm->biHeight)
	{
		return true;
	}

	if(m_pBackBufImage){
		_aligned_free(m_pBackBufImage);
		m_pBackBufImage = NULL;
	}

	m_VCARender.m_SourceSize.cx = pbm->biWidth;
	m_VCARender.m_SourceSize.cy = pbm->biHeight;
	/*
	if (MAKEFOURCC('Y','U','Y','2') == pbm->biCompression) {
		m_VCARender.m_eColorFormat = CD3d::CF_YUY2;
	} else if (MAKEFOURCC('Y','V','1','2') == pbm->biCompression) {
		m_VCARender.m_eColorFormat = CD3d::CF_YV12;
	} else if (MAKEFOURCC('U','Y','V','Y') == pbm->biCompression) {
		m_VCARender.m_eColorFormat = CD3d::CF_UYVY;
	} else if( BI_RGB == pbm->biCompression ) {
		if( 16 == pbm->biBitCount )
			m_VCARender.m_eColorFormat = CD3d::CF_BGR565;
		else if( 24 == pbm->biBitCount )
			m_VCARender.m_eColorFormat = CD3d::CF_BGR;
		else
			ASSERT(0);
	} else {
		ASSERT(0);
	}*/
	//m_pBackBufImage = (BYTE *)_aligned_malloc(GetImageSize(), 16); 

	return TRUE;
}


BOOL	CCalibrationD3DRender::RenderVCAData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG	ulRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp)
{
	if((RENDER_VCA == m_RenderMode) || (RENDER_TAMPER == m_RenderMode)) {
		m_VCARender.RenderVCAData(pImage, pbm, ulRotate, pVCAMetaLib, rcImageROI, rcVCAROI, pTimestamp);
		return TRUE;
	}else if(RENDER_CALIBRATION == m_RenderMode) {
		BOOL bPause = IVCARender::CALIB_PAUSE & m_pDataMgr->GetDisplayFlags();
				
		if(bPause) {
			if(!m_bPauseImageCopied){
				memcpy(m_pBackBufImage, pImage, GetImageSize());
				m_bPauseImageCopied	= TRUE;
			}
			m_pD3d->DrawImage(*m_pClientRect, m_VCARender.m_SourceSize, m_VCARender.m_eColorFormat, (CD3d::eROTATE)ulRotate, rcImageROI, m_pBackBufImage);
		}else{
			m_bPauseImageCopied	= FALSE;
			m_pD3d->DrawImage(*m_pClientRect, m_VCARender.m_SourceSize, m_VCARender.m_eColorFormat, (CD3d::eROTATE)ulRotate, rcImageROI, pImage);

			m_VCARender.DrawVCAData(rcVCAROI, pVCAMetaLib->GetObjects(), pVCAMetaLib->GetEvents(), pVCAMetaLib->GetCounts());
		}

		DrawGrid();

		RECT	ScreenRect;
		ScreenRect = *m_pClientRect;
			
		ClientToScreen(m_VCARender.m_hWnd, (POINT*)&ScreenRect.left );
		ClientToScreen(m_VCARender.m_hWnd, (POINT*)&ScreenRect.right );
		m_pD3d->PrimaryPresentation(&ScreenRect, m_pClientRect);
	}

	return TRUE;
}


UINT CCalibrationD3DRender::GetImageSize()
{
	UINT uiNumPix = m_VCARender.m_SourceSize.cx * m_VCARender.m_SourceSize.cy;
	UINT uiNumBytes = 0;

	switch( m_VCARender.m_eColorFormat )
	{
		case CD3d::CF_IYUV:
		case CD3d::CF_YV12:	uiNumBytes = (uiNumPix * 3)/2; break;

		case CD3d::CF_BGR555:
		case CD3d::CF_BGR565:
		case CD3d::CF_UYVY:
		case CD3d::CF_YUY2:	uiNumBytes = (uiNumPix * 4)/2; break;

		case CD3d::CF_BGR: uiNumBytes = uiNumPix * 3;	break;

		default: uiNumBytes = uiNumPix * 2; ASSERT( FALSE ); break;
	}

	return uiNumBytes;
}


void CCalibrationD3DRender::SetCanvasSize( RECT* pCanvas )
{
	unsigned int uiClientWidth, uiClientHeight;
	unsigned int uiCanvasWidth, uiCanvasHeight;
	
	uiClientWidth = m_pClientRect->right - m_pClientRect->left;
	uiClientHeight = m_pClientRect->bottom - m_pClientRect->top;
	
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
}


void CCalibrationD3DRender::DrawGrid()
{
	/// set params if needed	
	float fHeight = m_pDataMgr->GetCalibInfo()->fHeight;
	float fTilt = m_pDataMgr->GetCalibInfo()->fTilt;
	float fFOV = m_pDataMgr->GetCalibInfo()->fFOV;
	float fPan = m_pDataMgr->GetCalibInfo()->fPan;
	float fRoll = m_pDataMgr->GetCalibInfo()->fRoll;

	// Check units
	unsigned long	meaUnits = m_pDataMgr->GetCalibInfo()->ulHeightUnits ;
	if( VCA5_HEIGHT_UNITS_FEET == meaUnits )
	{
		fHeight *= 0.3048f;
	}

	/// render
	SIZE srcSize = {m_pClientRect->right-m_pClientRect->left, m_pClientRect->bottom-m_pClientRect->top};
	m_pD3d->RenderCalibrationGrid(m_pClientRect, fHeight, fTilt, fFOV,fPan, fRoll, srcSize, 
		m_pDataMgr->GetObjectModelCnt(), m_pDataMgr->GetObjectModel(0),
		m_pDataMgr->m_uiCalibDrawOptions&VCA_CALIB_DRAW_HORIZON,
		m_pDataMgr->m_uiCalibDrawOptions&VCA_CALIB_TRANS_GRID, 
		m_pDataMgr->m_uiCalibDrawOptions&VCA_CALIB_RULER_DRAW, m_pDataMgr->m_RulerPos);

	//Draw Ruler Text
	if(m_pDataMgr->m_uiCalibDrawOptions&VCA_CALIB_RULER_DRAW){
		float rulerlength = CD3d::GetLength(m_pDataMgr->m_RulerPos);
		if (rulerlength >= 0.0f){
			DrawRulerMeasure(rulerlength, meaUnits, POINT(m_pDataMgr->m_LastClickedMousePos));
		}
	}
}


void CCalibrationD3DRender::DrawRulerMeasure(float fLength, int meaUnits, POINT ptStartMsg)
{
	char	string[128] = {0,};
	if (meaUnits == VCA5_HEIGHT_UNITS_FEET)
		sprintf( string, "%.2f ft", fLength*3.2808399f);
	else
		sprintf( string, "%.2f m", fLength);

	ptStartMsg.y -= 20;
	DrawMsg( ptStartMsg, string, 255, RGB2(0xFF, 0xFF, 0xFF));
}


void CCalibrationD3DRender::DrawMsg( POINT ptStartMsg,char* pszMsg,int alpha, COLORREF colour)
{

	RECT  rcClient;
	RECT rc3DCanvas;
	float ratio1, ratio2;

	unsigned int uiCanvasDrawWidth, uiCanvasDrawHeight, uiClientWidth, uiClientHeight;
	unsigned int uiCanvasOriginalWidth, uiCanvasOriginalHeight;
	unsigned int uiNativeWidth, uiNativeHeight;


	SIZE sizeCanvas;
	uiNativeWidth	= m_VCARender.m_SourceSize.cx;
	uiNativeHeight	= m_VCARender.m_SourceSize.cy;
	uiClientWidth	= m_pClientRect->right;
	uiClientHeight	= m_pClientRect->bottom;
	uiCanvasOriginalWidth  = m_pD3d->GetCurrent3DCanvasSize().cx;
	uiCanvasOriginalHeight = m_pD3d->GetCurrent3DCanvasSize().cy;


	//___________________________________________________
	//	If the IE client window is smaller than the canvas,
	//	Set the canvas to be the size of the IE client window.
	if ( ( uiClientWidth  <= uiCanvasOriginalWidth  ) &&( uiClientHeight <= uiCanvasOriginalHeight ) )
	{
		uiCanvasDrawWidth  = uiClientWidth;
		uiCanvasDrawHeight = uiClientHeight;
	}
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
			uiCanvasDrawHeight = (unsigned int)((float) uiClientHeight / ratio1);
		}
		else
		{
			uiCanvasDrawWidth  = (unsigned int)((float) uiClientWidth / ratio2);
			uiCanvasDrawHeight = uiCanvasOriginalHeight;
		}
	}

	SetRect( &rc3DCanvas, 0, 0, uiCanvasDrawWidth, uiCanvasDrawHeight );
	
	sizeCanvas.cx = uiCanvasDrawWidth;
	sizeCanvas.cy = uiCanvasDrawHeight;

	m_pD3d->DrawText( rc3DCanvas, sizeCanvas, ptStartMsg, pszMsg, alpha, colour, 0.8f );		
}
