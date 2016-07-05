#ifndef __OBJECTMODEL_H__
#define __OBJECTMODEL_H__

#include <d3d.h>
#include <d3dx.h>


#define MAXNUM_OBJECTS 6
typedef struct
{
	D3DXMATRIX		matObjectModel;
	D3DMATERIAL7	materialPerson;

	D3DXVECTOR3 vecRot;
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecPickPos;
	D3DXVECTOR2 vecLogicalScreenPos;

	BOOL		bDragging;
	BOOL		bHighlighted;
	float		fScale;

	float		fMaxZ, fMinZ;
	float		fMaxX, fMinX;
	float		fMaxY, fMinY;

}ObjectModel;

#define	 HUMAN_HEIGHT	1.8f;

#endif