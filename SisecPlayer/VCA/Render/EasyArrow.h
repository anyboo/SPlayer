#pragma once
/*
#ifndef round
#define round(x) floor((x) + 0.5)
#endif
*/
//Pixel based arrow parameters
#define ARROW_PEN_WIDTH	2  //GETPERCENT(0.3)
#define ARROW_WIDTH		3  //GETPERCENT(0.5)
#define	ARROW_LENGTH	25 //GETPERCENT(3.8)
#define	ARC_LENGTH		25 //GETPERCENT(3.8)

#define PI				3.1415926535f

struct FLT_POINT
{
	double x;	
	double y;
};

typedef enum
{
	ANGLE_NOTDEFINED = 1,
	ANGLE_FULL,
	ANGLE_PART,
} ANGLE_STATUS;


typedef enum 
{
	ARROW_STYLE_NORMAL,
	ARROW_STYLE_DOUBLE
}eEasyArrowStyle;


class CEasyArrow
{
public:
	CEasyArrow(eEasyArrowStyle arrowStyle = ARROW_STYLE_NORMAL);
	~CEasyArrow();

public:
	void	SetArrowWidth( int iWidth );
	void	SetColor( BYTE _cbAlpha, COLORREF _clrArrow );		// EASYARROW
	void	SetPolygonPoint( int index, int x, int y );		// EASYARROW
	POINT	GetPolygonPoint( int index ) { POINT pt = {m_pPoints[index].x, m_pPoints[index].y }; return pt;	};
	void	SetPosition( POINT cpStart, POINT cpEnd );	// RENDER


public:
	POINT		m_cpStart;
	POINT		m_cpEnd;
	int			m_iWidth;
	POINT		m_pPoints[11];
	BYTE		m_bAlpha;
	COLORREF	m_bColour;
	eEasyArrowStyle m_arrowStyle;
	int			m_iNumPoints;
};


