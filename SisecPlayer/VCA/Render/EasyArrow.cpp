#include "stdafx.h"
#include "math.h"
#include "EasyArrow.h"

CEasyArrow::CEasyArrow(eEasyArrowStyle arrowStyle)
{
	m_cpStart.x = 0; m_cpStart.y = 0;
	m_cpEnd.x = 0; m_cpEnd.y = 0;
	m_arrowStyle	= arrowStyle;
	memset(&m_pPoints[0], 0, sizeof(m_pPoints));

	if (arrowStyle == ARROW_STYLE_NORMAL) m_iNumPoints = 7;
	else if (arrowStyle == ARROW_STYLE_DOUBLE) m_iNumPoints = 10;
	else m_iNumPoints = 7;
}

CEasyArrow::~CEasyArrow()
{
}

void CEasyArrow::SetArrowWidth( int iWidth )
{
	m_iWidth = iWidth;
}

void CEasyArrow::SetColor( BYTE _cbAlpha, COLORREF _clrArrow )
{
	m_bAlpha = _cbAlpha;
	m_bColour= _clrArrow;
}

void CEasyArrow::SetPolygonPoint( int index, int x, int y )
{
	m_pPoints[index].x = x;
	m_pPoints[index].y = y;
}

void CEasyArrow::SetPosition( POINT cpStart, POINT cpEnd )
{
	m_cpStart.x = (long)cpStart.x;
	m_cpStart.y = (long)cpStart.y;
	m_cpEnd.x = (long)cpEnd.x;
	m_cpEnd.y = (long)cpEnd.y;

	FLT_POINT delta;
	delta.x = cpEnd.x - cpStart.x;
	delta.y = cpEnd.y - cpStart.y;
	double angle = atan2( -delta.y, delta.x );
	double dy = m_iWidth * cos(angle);
	double dx = m_iWidth * sin(angle);

	switch(m_arrowStyle)
	{
	case ARROW_STYLE_NORMAL:
		{
			SetPolygonPoint( 0, (int) round(cpStart.x + dx),	(int) round(cpStart.y + dy ) );
			SetPolygonPoint( 1, (int) round(cpStart.x - dx),	(int) round(cpStart.y - dy ) );
			SetPolygonPoint( 2, (int) round(cpEnd.x - dx),			(int) round(cpEnd.y - dy) );
			SetPolygonPoint( 3, (int) round(cpEnd.x - 2 * dx),		(int) round(cpEnd.y - 2 * dy) );
			SetPolygonPoint( 4, (int) round(cpEnd.x + 3 * dy),		(int) round(cpEnd.y - 3 * dx) );
			SetPolygonPoint( 5, (int) round(cpEnd.x + 2 * dx),		(int) round(cpEnd.y + 2 * dy) );
			SetPolygonPoint( 6, (int) round(cpEnd.x + dx),			(int) round(cpEnd.y + dy) );
		}
		break;
	case ARROW_STYLE_DOUBLE:
		{
			SetPolygonPoint( 0, (int) round(cpStart.x + dx),			(int) round(cpStart.y + dy) );
			SetPolygonPoint( 1, (int) round(cpStart.x + 2 * dx),		(int) round(cpStart.y + 2 * dy) );
			SetPolygonPoint( 2, (int) round(cpStart.x - 3 * dy),		(int) round(cpStart.y + 3 * dx) );
			SetPolygonPoint( 3, (int) round(cpStart.x - 2 * dx),		(int) round(cpStart.y - 2 * dy) );
			SetPolygonPoint( 4, (int) round(cpStart.x - dx),			(int) round(cpStart.y - dy) );
 
			SetPolygonPoint( 5, (int) round(cpEnd.x - dx),			(int) round(cpEnd.y - dy) );
			SetPolygonPoint( 6, (int) round(cpEnd.x - 2 * dx),		(int) round(cpEnd.y - 2 * dy ) );
			SetPolygonPoint( 7, (int) round(cpEnd.x + 3 * dy),		(int) round(cpEnd.y - 3 * dx ) );
			SetPolygonPoint( 8, (int) round(cpEnd.x + 2 * dx),		(int) round(cpEnd.y + 2 * dy ) );
			SetPolygonPoint( 9, (int) round(cpEnd.x + dx ),			(int) round(cpEnd.y + dy) );
		
		}
		break;
	}

}