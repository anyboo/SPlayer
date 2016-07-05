#pragma once

#include "CD3d.h"
#include "colorconversion.h"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "d3dx.lib")
#pragma comment(lib, "dxguid.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(c) if(c){c->Release();c=NULL;}else{}
#endif

// Error Message
#define MACRO_DDERROR( hrDDErr, lpszAPI ) DisplayDDError( hrDDErr, lpszAPI, __FILE__, __LINE__ )
#define RADIUS_STEP					0.1
	
typedef enum _tagSURFACETYPE
{
	SURFACETYPE_INVALID			= 0,
	SURFACETYPE_AGPTEXTURE		= 1,
	SURFACETYPE_VIDMEMTEXTURE	= 2,
	SURFACETYPE_3DCANVAS		= 3,
	SURFACETYPE_PRIMARY			= 4,
	SURFACETYPE_MANAGEDTEXTURE	= 5,
}eSURFACETYPE;

//PIXELFORMAT
extern DDPIXELFORMAT DDPF_RGBA16;
extern DDPIXELFORMAT DDPF_RGB32;
extern DDPIXELFORMAT DDPF_RGBA32;
extern DDPIXELFORMAT DDPF_RGB24;
extern DDPIXELFORMAT DDPF_RGB565;
extern DDPIXELFORMAT DDPF_RGB555;
extern DDPIXELFORMAT DDPF_YUY2;


#define _INIT_DIRECTDRAW_STRUCT(x) (ZeroMemory(&x, sizeof(x)), x.dwSize=sizeof(x))

void InitColorConversionFunctions();
BOOL ConvertColorFormat( BYTE*		 _pSrc,
						CD3d::eCOLORFORMAT _eCFSource,
						SIZE		 _sizeSource,
						BYTE*		 _pDst,
						CD3d::eCOLORFORMAT _eCFDest,
						DWORD		 _dwDstPitch,
						DWORD		 _dwScale_factor );


LPDIRECTDRAWSURFACE7 CreateCustomSurface( LPDIRECTDRAW7 _pDD,
											   eSURFACETYPE  _surfaceType,
											   PVOID		 _lpParam,
											   DWORD		 _dwWidth,
											   DWORD		 _dwHeight );

BOOL GetSurfacePointer( LPDIRECTDRAWSURFACE7 const _pDDSurface,
						     RECT* const				_prcTarget,
							 BYTE**						_ppSurf,
							 DWORD*						_pdwPitch );

BOOL ReleaseSurfacePointer( LPDIRECTDRAWSURFACE7 const _pDDSurface );


void SetVertexScreenXYZ( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVECTOR _tVector );
void SetVertexScreenXY( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foX, D3DVALUE _foY );
void SetVertexZValue( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foZValue );
void SetVertexColorValue( D3DTLVERTEX* _aptVertex, int indexFrom, int count, DWORD _dwColor );
void SetVertexTextureXY( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foU, D3DVALUE _foV );

void EnableAlpha(LPDIRECT3DDEVICE7 pd3dDevice);

DWORD GetFontAreaHeight( HFONT _hFont, DWORD _dwWidth, int _nMargin_x, int _nMargin_y );
D3DTLVERTEX InitFontVertex( const D3DXVECTOR4& p, D3DCOLOR color, float tu, float tv );


HRESULT	DisplayDDError( HRESULT _hrDDErr, LPTSTR _lpszAPI, LPSTR _lpszFile, INT _nLineNo );