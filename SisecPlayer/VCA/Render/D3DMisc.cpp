/* **********************************************************************************
	import standard function prototype
*/
#include "StdAfx.h"
#include "D3DMisc.h"

DDPIXELFORMAT DDPF_RGBA16 = {
	sizeof(DDPIXELFORMAT),
	DDPF_RGB|DDPF_ALPHAPIXELS ,
	0,
	16,
	0x00000F00,0x000000F0,0x0000000F,0x0000F000,
};

DDPIXELFORMAT DDPF_RGB32 = {
	sizeof(DDPIXELFORMAT),
	DDPF_RGB,
	0,
	32,
	0x00FF0000,0x0000FF00,0x000000FF,0x00000000,
};

DDPIXELFORMAT DDPF_RGBA32 = {
	sizeof( DDPIXELFORMAT ),
	DDPF_RGB|DDPF_ALPHAPIXELS,
	0,
	32,
	0x00FF0000,0x0000FF00,0x000000FF,0xFF000000,
};

DDPIXELFORMAT DDPF_RGB24 = {
	sizeof(DDPIXELFORMAT),
	DDPF_RGB,
	0,
	24,
	0x00FF0000,0x0000FF00,0x000000FF,0x00000000,
};

DDPIXELFORMAT DDPF_RGB565 = {
	sizeof(DDPIXELFORMAT),
	DDPF_RGB,
	0,
	16,
	0x0000F800,0x000007E0,0x0000001F,0x00000000,
};

DDPIXELFORMAT DDPF_RGB555 = {
	sizeof(DDPIXELFORMAT),
	DDPF_RGB,
	0,
	16,
	0x00007C00,0x000003E0,0x0000001F,0x00000000,
};

DDPIXELFORMAT DDPF_YUY2 = {
	sizeof(DDPIXELFORMAT),
	DDPF_FOURCC,
	MAKEFOURCC('Y','U','Y','2'),
	0,
	0,0,0,0
};


void InitColorConversionFunctions()
{
	BOOL sse2_support = IsSSE2Supported();

	if( sse2_support ) {
		cc_YUY2_To_RGB32			= YUY2_To_RGB32			;
		cc_YUY2_To_RGB32_Resize_2	= YUY2_To_RGB32_Resize_2_SSE;//YUY2_To_RGB32_Resize_2;	
		cc_YUY2_To_RGB555			= YUY2_To_RGB555		;	
		cc_YUY2_To_RGB555_Resize_2	= YUY2_To_RGB555_Resize_2_2;
		cc_YUY2_To_RGB565			= YUY2_To_RGB565			;
		cc_YUY2_To_RGB565_Resize_2	= YUY2_To_RGB565_Resize_2_2;
		//cc_YUY2_To_RGB565_Resize_2	= YUY2_To_RGB565_Resize_2_SSE;
	}else{
		cc_YUY2_To_RGB32			= YUY2_To_RGB32_SSE			;
		cc_YUY2_To_RGB32_Resize_2	= YUY2_To_RGB32_Resize_2_SSE;	
		cc_YUY2_To_RGB555			= YUY2_To_RGB555_SSE		;	
		cc_YUY2_To_RGB555_Resize_2	= YUY2_To_RGB555_Resize_2_SSE;
		cc_YUY2_To_RGB565			= YUY2_To_RGB565_SSE		;	
		cc_YUY2_To_RGB565_Resize_2	= YUY2_To_RGB565_Resize_2_SSE;
	}
}


BOOL ConvertColorFormat( BYTE*		 _pSrc,
					    CD3d::eCOLORFORMAT _eCFSource,
						SIZE		 _sizeSource,
						BYTE*		 _pDst,
						CD3d::eCOLORFORMAT _eCFDest,
						DWORD		 _dwDstPitch,
						DWORD		 _dwScale_factor )
{
	if( _pSrc == NULL ) return FALSE;

	//_pDst의 BGR565는 모두 BGRX로 바뀔 예정
	const DWORD pixel_bytes[] = { 0,2,4,3,2,2 };	// bps
	//ErrDbg(1, "_ConvertColorFormat _pSrc %Xh, _pDst %Xh \r\n", _pSrc, _pDst);

	if( _eCFDest == _eCFSource )
	{
		if( _dwScale_factor == 1 ) {
			HSH_CopySameFormatImage( _pDst, _pSrc, _dwDstPitch, _sizeSource.cx, _sizeSource.cy, pixel_bytes[_eCFSource] );
			return TRUE;
		}
	}

	if( _eCFSource != CD3d::CF_YUY2 &&
		_eCFSource != CD3d::CF_YV12 &&
		_eCFSource != CD3d::CF_IYUV &&
		_eCFSource != CD3d::CF_UYVY &&
		_eCFSource != CD3d::CF_BGR &&
		_eCFSource != CD3d::CF_BGR565 ) { return FALSE; }

	switch( _eCFSource )
	{
	case CD3d::CF_UYVY:
		{
			if( _eCFDest == CD3d::CF_BGR565 )
			{
				UYVY_To_RGB565(_pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else
			{
				return FALSE;
			}
		}
		break;

	case CD3d::CF_YUY2:
		if( _eCFDest == CD3d::CF_YUY2 )
		{
			if( _dwScale_factor == 1 ) { HSH_CopyYUYVImage   ( _pDst, _pSrc, _dwDstPitch, _sizeSource.cx, _sizeSource.cy ); }
			else
			if( _dwScale_factor == 2 ) { HSH_CopyYUYVImageBy2( _pDst, _pSrc, _dwDstPitch, _sizeSource.cx, _sizeSource.cy ); }
			else
			if( _dwScale_factor == 4 ) { HSH_CopyYUYVImageBy4( _pDst, _pSrc, _dwDstPitch, _sizeSource.cx, _sizeSource.cy ); }
			else { return FALSE; }
		}
		else
		if( _eCFDest == CD3d::CF_BGR555 )
		{
			if( _dwScale_factor == 1 ) {
				if( _sizeSource.cx % 8 ) YUY2_To_RGB555_SSE(_pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
				else					 cc_YUY2_To_RGB555( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else
			if( _dwScale_factor == 2 ) {
				cc_YUY2_To_RGB555_Resize_2( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else { return FALSE; }
		}
		else
		if( _eCFDest == CD3d::CF_BGR565 )
		{
			if( _dwScale_factor == 1 ) {

				if( _sizeSource.cx % 8 ) YUY2_To_RGB565_SSE( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
				else					 cc_YUY2_To_RGB565( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else
			if( _dwScale_factor == 2 ) {
				cc_YUY2_To_RGB565_Resize_2( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else { return FALSE; }
		}
		else
		if( _eCFDest == CD3d::CF_BGRX )
		{
			if( _dwScale_factor == 1 ) {
				if( _sizeSource.cx % 8 ) YUY2_To_RGB32_SSE( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
				else					 cc_YUY2_To_RGB32( _pSrc, _pDst, _dwDstPitch, _sizeSource.cx, _sizeSource.cy );
			}
			else
			if( _dwScale_factor == 2 ) {
				cc_YUY2_To_RGB32_Resize_2(_pSrc,_pDst,_dwDstPitch,_sizeSource.cx,_sizeSource.cy);
			}
			else { return FALSE; }
		}
		else { return FALSE; }
		break;

	case CD3d::CF_IYUV:
		if( _eCFDest == CD3d::CF_BGR565 )
		{
			//포인터에 대한 추가적인 변수가 필요하다..
			BYTE* u_src = _pSrc+_sizeSource.cx * _sizeSource.cy;
			BYTE* v_src = _pSrc+_sizeSource.cx * _sizeSource.cy * 5 / 4;
			if( _dwScale_factor == 1 ) {
				YUY12_To_RGB565_mmx(_pDst,_dwDstPitch,_pSrc,u_src,v_src,_sizeSource.cx,_sizeSource.cx/2,_sizeSource.cx,_sizeSource.cy);
			}
			else
			if( _dwScale_factor == 2 ) {
				YUY12_To_RGB565_mmx_resize_2(_pDst,_dwDstPitch,_pSrc,u_src,v_src,_sizeSource.cx,_sizeSource.cx/2,_sizeSource.cx,_sizeSource.cy);
			}
			else { return FALSE; }
		}
		else { return FALSE; }
		break;

	case CD3d::CF_YV12:
		if( _eCFDest == CD3d::CF_BGR565 )
		{
			//포인터에 대한 추가적인 변수가 필요하다..
			BYTE* u_src = _pSrc+_sizeSource.cx * _sizeSource.cy * 5 / 4;
			BYTE* v_src = _pSrc+_sizeSource.cx * _sizeSource.cy;
			if( _dwScale_factor == 1 ) {
				YUY12_To_RGB565_mmx(_pDst,_dwDstPitch,_pSrc,u_src,v_src,_sizeSource.cx,_sizeSource.cx/2,_sizeSource.cx,_sizeSource.cy);
			}
			else
			if( _dwScale_factor== 2 ) {
				YUY12_To_RGB565_mmx_resize_2(_pDst,_dwDstPitch,_pSrc,u_src,v_src,_sizeSource.cx,_sizeSource.cx/2,_sizeSource.cx,_sizeSource.cy);
			}
			else{ return FALSE; }
		}
		else { return FALSE; }
		break;

	case CD3d::CF_BGR:
		if( _eCFDest == CD3d::CF_BGR565 )
		{
			// Very simple RGB24 to BGR565 conversion...
			unsigned char *pS = _pSrc;
			for( int y = 0; y < _sizeSource.cy; y++ )
			{
				unsigned short *pD = (unsigned short *)(((unsigned char *)_pDst) + (y*_dwDstPitch));
				for(int  x = 0; x < _sizeSource.cx; x++, pD++ )
				{
					unsigned short r, g, b;

					b = (*pS++ >>3);
					g = ((*pS++ >> 2) << 5);
					r = ((*pS++ >> 3) << 11);
					
					*pD = b | g | r;
				}
			}
		}
		else { return FALSE; }
	}//switch( _eCFSource )

	return TRUE;
}


HRESULT WINAPI lpEnumZBufferCallback( DDPIXELFORMAT* pddpf, VOID* pddpfDesired )
{
    // For this tutorial, we are only interested in z-buffers, so ignore any
    // other formats (e.g. DDPF_STENCILBUFFER) that get enumerated. An app
    // could also check the depth of the z-buffer (16-bit, etc,) and make a
    // choice based on that, as well. For this tutorial, we'll take the first
    // one we get.
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );
 
        // Return with D3DENUMRET_CANCEL to end the search.
        return D3DENUMRET_CANCEL;
    }
 
    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}


LPDIRECTDRAWSURFACE7 CreateCustomSurface( LPDIRECTDRAW7	_pDD,
												 eSURFACETYPE	_eSurfaceType,
												 PVOID			_lpParam,
												 DWORD			_dwWidth,
												 DWORD			_dwHeight )
{
	HRESULT hr;

	DDSURFACEDESC2	ddsd;
	_INIT_DIRECTDRAW_STRUCT( ddsd );

	TCHAR	szForDebug[128];

	switch(_eSurfaceType) {
	case SURFACETYPE_AGPTEXTURE: {
		wsprintf(szForDebug, _T("SURFACETYPE_AGPTEXTURE %dx%d"), _dwWidth, _dwHeight);
		ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		ddsd.dwWidth         = _dwWidth;
		ddsd.dwHeight        = _dwHeight;
		ddsd.ddpfPixelFormat = *(DDPIXELFORMAT*)_lpParam;
		ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY|DDSCAPS_NONLOCALVIDMEM;
		break;
		}
	case SURFACETYPE_VIDMEMTEXTURE: {
		wsprintf(szForDebug, _T("SURFACETYPE_VIDMEMTEXTURE %dx%d"), _dwWidth, _dwHeight);
		ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		ddsd.dwWidth         = _dwWidth;
		ddsd.dwHeight        = _dwHeight;
		ddsd.ddpfPixelFormat = *(DDPIXELFORMAT*)_lpParam;
		ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM ;
		break;
		}
	case SURFACETYPE_MANAGEDTEXTURE: {
		wsprintf(szForDebug, _T("SURFACETYPE_MANAGEDTEXTURE %dx%d"), _dwWidth, _dwHeight);
		ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_TEXTURESTAGE|DDSD_PIXELFORMAT;
		ddsd.dwWidth         = _dwWidth;
		ddsd.dwHeight        = _dwHeight;
		ddsd.ddpfPixelFormat = *(DDPIXELFORMAT*)_lpParam;
		ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		break;
		}
	case SURFACETYPE_3DCANVAS: {
		wsprintf(szForDebug, _T("SURFACETYPE_3DCANVAS %dx%d"), _dwWidth, _dwHeight);
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth		= _dwWidth;
		ddsd.dwHeight		= _dwHeight;
		ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM ;//DDSCAPS_OFFSCREENPLAIN
		break;
		}
	case SURFACETYPE_PRIMARY: {
		lstrcpy(szForDebug, _T("SURFACETYPE_PRIMARY"));
		ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		break;
		}

	default:
		return NULL;
	}

	LPDIRECTDRAWSURFACE7  pddsTexture = NULL;
	do	// for easy return coding
	{
		hr = _pDD->CreateSurface( &ddsd, &pddsTexture, NULL );
		if( FAILED(hr) ) {
			pddsTexture = NULL;
			lstrcat( szForDebug, _T(" CreateSurface") );
			MACRO_DDERROR( hr, szForDebug );
			break;
		}

		if( _eSurfaceType == SURFACETYPE_3DCANVAS )
		{
			LPDIRECT3D7		pD3D;
			hr = _pDD->QueryInterface( IID_IDirect3D7, (PVOID*)&pD3D );
			if( FAILED(hr) ) {
				SAFE_RELEASE( pddsTexture );
				MACRO_DDERROR( hr, _T("IID_IDirect3D7 QueryInterface") );
				break;
			}

			DDPIXELFORMAT pixFormat;
			pD3D->EnumZBufferFormats( IID_IDirect3DHALDevice, 
									  lpEnumZBufferCallback, 
									  &pixFormat );

			ddsd.dwFlags		 = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
			ddsd.dwWidth		 = _dwWidth;
			ddsd.dwHeight		 = _dwHeight;
			ddsd.ddpfPixelFormat = pixFormat;
			ddsd.ddsCaps.dwCaps  = DDSCAPS_ZBUFFER|DDSCAPS_VIDEOMEMORY;

			LPDIRECTDRAWSURFACE7  pddsZBuffer = NULL;
			hr = _pDD->CreateSurface( &ddsd, &pddsZBuffer, NULL );
			if( FAILED(hr) ) {
				SAFE_RELEASE( pddsTexture );
				MACRO_DDERROR( hr, _T("CreateSurface ZBuffer") );
				break;
			}
				
			hr = pddsTexture->AddAttachedSurface( pddsZBuffer );
			SAFE_RELEASE( pddsZBuffer );
			if( FAILED(hr) ) {
				SAFE_RELEASE( pddsTexture );
				MACRO_DDERROR( hr, _T("AddAttachedSurface") );
				break;
			}

			LPDIRECT3DDEVICE7	pD3DDevice;
			hr = pD3D->CreateDevice( IID_IDirect3DHALDevice, pddsTexture, (LPDIRECT3DDEVICE7*)&pD3DDevice );
			if( FAILED(hr) ) {
				SAFE_RELEASE( pddsTexture );
				MACRO_DDERROR( hr, _T("CreateDevice IID_IDirect3DHALDevice") );
				break;
			}
			SAFE_RELEASE( pD3D );

			pD3DDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0, 1.0, 0 );
			*(LPDIRECT3DDEVICE7*)_lpParam = pD3DDevice;
			break;
		}//if( _eSurfaceType == SURFACETYPE_3DCANVAS )
	}while(0);

	return pddsTexture;
}


BOOL GetSurfacePointer( LPDIRECTDRAWSURFACE7 const _pDDSurface,
							   RECT* const				  _prcTarget,
							   BYTE**					  _ppSurf,
							   DWORD*					  _pdwPitch )
{
	DDSURFACEDESC2	ddsd; _INIT_DIRECTDRAW_STRUCT( ddsd );

	HRESULT hr = _pDDSurface->IsLost();
	if( hr == DDERR_SURFACELOST ) {
		hr = _pDDSurface->Restore();
		if( FAILED(hr) ) {
			MACRO_DDERROR( hr, _T("Restore") );
			return FALSE;
		}
	}

	hr = _pDDSurface->Lock( _prcTarget,
							&ddsd, 
							DDLOCK_SURFACEMEMORYPTR |
							DDLOCK_WAIT | 
							DDLOCK_NOSYSLOCK |
							DDLOCK_WRITEONLY,
							NULL );
	if( FAILED(hr) ) {
		MACRO_DDERROR( hr, _T("Lock") );
		return FALSE;
	}

	*_ppSurf = (BYTE*)ddsd.lpSurface;
	*_pdwPitch = ddsd.lPitch;

	return TRUE;
}


BOOL ReleaseSurfacePointer( LPDIRECTDRAWSURFACE7 const _pDDSurface )
{
	_pDDSurface->Unlock( NULL );
	return TRUE;
}


void SetVertexScreenXYZ( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVECTOR _tVector )
{
	for( int i = indexFrom; i < indexFrom + count; i++ ) {
		_aptVertex[i].dvSX = _tVector.dvX;
		_aptVertex[i].dvSY = _tVector.dvY;
		_aptVertex[i].dvSZ = _tVector.dvZ;
	}
}

void SetVertexScreenXY( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foX, D3DVALUE _foY )
{
	for( int i = indexFrom; i < indexFrom + count; i++ ) {
		_aptVertex[i].dvSX = _foX;
		_aptVertex[i].dvSY = _foY;
	}
}

void SetVertexZValue( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foZValue )
{
	for( int i = indexFrom; i < indexFrom + count; i++ ) {
		_aptVertex[i].dvSZ = _foZValue;
	}
}

void SetVertexColorValue( D3DTLVERTEX* _aptVertex, int indexFrom, int count, DWORD _dwColor )
{
	for( int i = indexFrom; i < indexFrom + count; i++ ) {
		_aptVertex[i].dcColor = _dwColor;
	}
}

void SetVertexTextureXY( D3DTLVERTEX* _aptVertex, int indexFrom, int count, D3DVALUE _foU, D3DVALUE _foV )
{
	for( int i = indexFrom; i < indexFrom + count; i++ ) {
		_aptVertex[i].dvTU = _foU;
		_aptVertex[i].dvTV = _foV;
	}
}


void EnableAlpha(LPDIRECT3DDEVICE7 pd3dDevice)
{
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,			D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF,			0x08 );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC,			D3DCMP_GREATEREQUAL );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,			D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,			D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,			TRUE );
}

DWORD GetFontAreaHeight( HFONT _hFont, DWORD _dwWidth, int _nMargin_x, int _nMargin_y )
{
	HDC   hDC = GetDC(NULL);
	HFONT hOldfont = (HFONT)SelectObject( hDC, _hFont );

	TCHAR str[4];
	int  x=0, y=0;
	SIZE size = {0,};

	// ASCII from 'space'(32) to ''(127)
	for( char c = 32; c < 127; c++ ) {
		str[0] = c;
		::GetTextExtentPoint32( hDC, str, 1, &size );

		if( (DWORD)( x + size.cx + 1 + 2 ) > _dwWidth ) {
			x = 0;
			y += size.cy + 1 + _nMargin_y;
		}
		x += size.cx + 1 + _nMargin_x;
	}

	SelectObject( hDC, hOldfont );
	ReleaseDC( NULL, hDC );
	
	return ( y + size.cy + 1 );
}


D3DTLVERTEX InitFontVertex( const D3DXVECTOR4& p, D3DCOLOR color, float tu, float tv )
{
	D3DTLVERTEX v;
	v.dvSX    = p.x;
	v.dvSY    = p.y;
	v.dvSZ    = p.z;
	v.dvRHW   = p.w;

	v.dcColor = color;
	v.dcSpecular = 0;
	v.dvTU    = tu;
	v.dvTV    = tv;

	return v;
}


HRESULT	DisplayDDError( HRESULT _hrDDErr, LPTSTR _lpszAPI, LPSTR _lpszFile, INT _nLineNo )
{
	TCHAR	sz[64] = {0,};

	switch( _hrDDErr )
	{
	case E_NOINTERFACE: wsprintf(sz, _T("E_NOINTERFACE")); break;
	case DDERR_INVALIDRECT: wsprintf(sz, _T("DDERR_INVALIDRECT")); break;
	case DDERR_NOBLTHW: wsprintf(sz, _T("DDERR_NOBLTHW")); break;
	case DDERR_NOCLIPLIST: wsprintf(sz, _T("DDERR_NOCLIPLIST")); break;
	case DDERR_NODDROPSHW: wsprintf(sz, _T("DDERR_NODDROPSHW")); break;
	case DDERR_NOMIRRORHW: wsprintf(sz, _T("DDERR_NOMIRRORHW")); break;
	case DDERR_NORASTEROPHW: wsprintf(sz, _T("DDERR_NORASTEROPHW")); break;
	case DDERR_NOROTATIONHW: wsprintf(sz, _T("DDERR_NOROTATIONHW")); break;
	case DDERR_NOSTRETCHHW: wsprintf(sz, _T("DDERR_NOSTRETCHHW")); break;
	case DDERR_CANNOTATTACHSURFACE: wsprintf(sz, _T("DDERR_CANNOTATTACHSURFACE")); break;
	case DDERR_SURFACEALREADYATTACHED: wsprintf(sz, _T("DDERR_SURFACEALREADYATTACHED")); break;
	case DDERR_INVALIDCAPS: wsprintf(sz, _T("DDERR_INVALIDCAPS")); break;
	case DDERR_INVALIDPIXELFORMAT: wsprintf(sz, _T("DDERR_INVALIDPIXELFORMAT")); break;
	case DDERR_NOALPHAHW: wsprintf(sz, _T("DDERR_NOALPHAHW")); break;
	case DDERR_NOEMULATION: wsprintf(sz, _T("DDERR_NOEMULATION")); break;
	case DDERR_NOFLIPHW: wsprintf(sz, _T("DDERR_NOFLIPHW")); break;
	case DDERR_NOMIPMAPHW: wsprintf(sz, _T("DDERR_NOMIPMAPHW")); break;
	case DDERR_NOOVERLAYHW: wsprintf(sz, _T("DDERR_NOOVERLAYHW")); break;
	case DDERR_NOZBUFFERHW: wsprintf(sz, _T("DDERR_NOZBUFFERHW")); break;
	case DDERR_OUTOFVIDEOMEMORY: wsprintf(sz, _T("DDERR_OUTOFVIDEOMEMORY")); break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS: wsprintf(sz, _T("DDERR_PRIMARYSURFACEALREADYEXISTS")); break;
	case DDERR_UNSUPPORTEDMODE: wsprintf(sz, _T("DDERR_UNSUPPORTEDMODE")); break;
	case DDERR_SURFACEBUSY: wsprintf(sz, _T("DDERR_SURFACEBUSY")); break;
	case DDERR_SURFACELOST: wsprintf(sz, _T("DDERR_SURFACELOST")); break;
	case DDERR_WASSTILLDRAWING: wsprintf(sz, _T("DDERR_WASSTILLDRAWING")); break;
	case DDERR_IMPLICITLYCREATED: wsprintf(sz, _T("DDERR_IMPLICITLYCREATED")); break;
	case DDERR_INCOMPATIBLEPRIMARY: wsprintf(sz, _T("DDERR_INCOMPATIBLEPRIMARY")); break;
	case DDERR_NOEXCLUSIVEMODE: wsprintf(sz, _T("DDERR_NOEXCLUSIVEMODE")); break;
	case DDERR_UNSUPPORTED: wsprintf(sz, _T("DDERR_UNSUPPORTED")); break;
	case DDERR_WRONGMODE: wsprintf(sz, _T("DDERR_WRONGMODE")); break;
	case DDERR_INVALIDCLIPLIST: wsprintf(sz, _T("DDERR_INVALIDCLIPLIST")); break;
	case DDERR_INVALIDSURFACETYPE: wsprintf(sz, _T("DDERR_INVALIDSURFACETYPE")); break;
	case DDERR_NOCLIPPERATTACHED: wsprintf(sz, _T("DDERR_NOCLIPPERATTACHED")); break;
	case DDERR_NOCOOPERATIVELEVELSET: wsprintf(sz, _T("DDERR_NOCOOPERATIVELEVELSET")); break;
	case DDERR_EXCLUSIVEMODEALREADYSET: wsprintf(sz, _T("DDERR_EXCLUSIVEMODEALREADYSET")); break;
	case DDERR_HWNDALREADYSET: wsprintf(sz, _T("DDERR_HWNDALREADYSET")); break;
	case DDERR_HWNDSUBCLASSED: wsprintf(sz, _T("DDERR_HWNDSUBCLASSED")); break;
	case DDERR_INVALIDOBJECT: wsprintf(sz, _T("DDERR_INVALIDOBJECT")); break;
	case DDERR_DIRECTDRAWALREADYCREATED: wsprintf(sz, _T("DDERR_DIRECTDRAWALREADYCREATED")); break;
	case DDERR_GENERIC: wsprintf(sz, _T("DDERR_GENERIC")); break;
	case DDERR_INVALIDDIRECTDRAWGUID: wsprintf(sz, _T("DDERR_INVALIDDIRECTDRAWGUID")); break;
	case DDERR_INVALIDPARAMS: wsprintf(sz, _T("DDERR_INVALIDPARAMS")); break;
	case DDERR_NODIRECTDRAWHW: wsprintf(sz, _T("DDERR_NODIRECTDRAWHW")); break;
	case DDERR_OUTOFMEMORY: wsprintf(sz, _T("DDERR_OUTOFMEMORY")); break;
	default: wsprintf(sz, _T("Unknown")); break;
	}

	TRACE("%s - %s(%d) at %s(%d line) \r\n", _lpszAPI, sz, LOWORD(_hrDDErr), _lpszFile, _nLineNo );
	return 0;
}

