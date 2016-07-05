
#include "stdafx.h"
#include "EasyArrowD3D.h"

/*******************************************************/
// D3D related drawing here
/*******************************************************/

CEasyArrowD3D::CEasyArrowD3D( eEasyArrowStyle arrowStyle)
: CEasyArrow(arrowStyle)
{
	m_pD3D = NULL;
}

CEasyArrowD3D::~CEasyArrowD3D()
{
}

void CEasyArrowD3D::SetD3D( CD3d *pD3D )
{
	m_pD3D = pD3D;
}

void CEasyArrowD3D::DrawArrow( RECT _rc3DCanvas, SIZE _sizeSrcImage )
{
	CD3d::MYPOLYGON tPolygon;
	long lWidth, lHeight;

	lWidth = _rc3DCanvas.right - _rc3DCanvas.left;
	lHeight = _rc3DCanvas.bottom - _rc3DCanvas.top;

	tPolygon.cnt_point = m_iNumPoints;

	for ( int i = 0; i < m_iNumPoints; i ++ )
	{
//		PERCENTTOPIXEL( tPolygon.atpoint[i].x, m_pPoints[i].x, lWidth );
//		PERCENTTOPIXEL( tPolygon.atpoint[i].y, m_pPoints[i].y, lHeight );
		tPolygon.atpoint[i] = m_pPoints[i];
	}
	m_pD3D->DrawPolygonList( _rc3DCanvas, _sizeSrcImage, &tPolygon, 1, CD3d::BRT_SOLID, m_bAlpha, m_bColour );


	tPolygon.atpoint[m_iNumPoints] = tPolygon.atpoint[0];
	m_pD3D->DrawLineList( _rc3DCanvas, _sizeSrcImage, &tPolygon.atpoint[0], tPolygon.cnt_point+1, m_bAlpha, RGB(0,0,0) );
}
