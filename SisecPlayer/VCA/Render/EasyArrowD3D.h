#pragma once

#include "EasyArrow.h"
#include "CD3d.h"	// For using d3d


// D3D
class CEasyArrowD3D : public CEasyArrow
{
public:
	CEasyArrowD3D(eEasyArrowStyle arrowStyle = ARROW_STYLE_NORMAL );
	~CEasyArrowD3D( );

public:
	void	SetD3D( CD3d *pD3D );
	void	DrawArrow( RECT _rc3DCanvas, SIZE _sizeSrcImage  );

public:
	CD3d		*m_pD3D;
};

