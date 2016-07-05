#pragma once

#define D3D_OVERLOADS
#include <d3d.h>
#include <d3dx.h>

#include "triangulation.h"	
#include "ObjectModel.h"
#include "D3DAdapterInfo.h"

// D3D Canvas Size
#define	MAXWIDTH_D3DCANVAS		(1920)		// 0	current display mode resolution
#define	MAXHEIGHT_D3DCANVAS		(1080)		// 0	current display mode resolution

// Image TextureSurface Size
#define	MAXWIDTH_D3DTEXTURE		(1920)
#define	MAXHEIGHT_D3DTEXTURE	(1080)

#define MAXCOUNT_POLYGONEDGE	80		
#define D3D_DEFAULT_FONT_SIZE	52

#define MAXCOUNT_BRUSHTEXTURES	2
#define BRUSH_TEXTURESIZE		16.0f

#define RGB2(r,g,b)	((r<<16)+(g<<8)+b)


class CD3d
{
public:
	typedef enum {
		CF_NOTDEFINED = 0,
		CF_YUY2,
		CF_BGRX,	//rgb32
		CF_BGR,		//rgb24
		CF_BGR565,
		CF_BGR555,
		CF_YV12,
		CF_IYUV,
		CF_UYVY
	}eCOLORFORMAT;

	typedef enum {
			RT_000 = 0,
			RT_090,
			RT_180,	
			RT_270,	
	}eROTATE;

	typedef struct	{
		ULONG	cnt_point;
		POINT	atpoint[MAXCOUNT_POLYGONEDGE];
	}MYPOLYGON;

	typedef enum 	{
		BRT_SOLID		= 0,
		BRT_HATCH,
	}eBRUSHTYPE;

	
	CD3d();
	~CD3d();
	
	HRESULT Setup( INT	nUseDDAdapterNo,
					eCOLORFORMAT	eD3dTextureCF	= CF_BGR565, 
					HWND			hwndWindow		= NULL,
					INT			n3DCanvasWidth   = MAXWIDTH_D3DCANVAS,
					INT		    n3DCanvasHeight  = MAXHEIGHT_D3DCANVAS,
					INT			n3DTextureWidth  = MAXWIDTH_D3DTEXTURE,
					INT		    n3DTextureHeight = MAXHEIGHT_D3DTEXTURE );
	
	VOID	Endup();
	BOOL	SetDDClipperWindow( HWND _hwnd );
	
	BOOL	DrawImage( RECT			_rc3DCanvas,		// Target
					SIZE			_sizeSrcImage,		// Src
					eCOLORFORMAT	_eCFSrcImage,		// CF_YUY2, CF_YV12,
					eROTATE			_eRotate,
					RECT			_rcROI,
					BYTE* const		_pImage );

	BOOL	DrawRectList(	RECT		 _rc3DCanvas,
						SIZE			_sizeSrcImage,
						RECT* const		_apRcList,
						INT				_nRectCount,
						BYTE			_cbAlpha,
						COLORREF		_clrRect );

	BOOL	DrawPolygonList( RECT				_rc3DCanvas,
							SIZE				_sizeSrcImage,
							MYPOLYGON* const	_aptPolygonList,
							INT					_nPolygonCount,
							eBRUSHTYPE			_brushType,
						    BYTE				_cbAlpha,
						    COLORREF			_clrRect );
	
	BOOL	DrawLineList( RECT			_rc3DCanvas,
						 SIZE			_sizeSrcImage,
						 POINT* const	_apPtList,
						 INT			_nPtCount,
						 BYTE			_cbAlpha,
						 COLORREF		_clrLine );

	BOOL	DrawText( RECT		_rc3DCanvas,
				     SIZE		_sizeSrcImage,
				     POINT      _ptStartXY,
				     LPCSTR		_lpszText,
				     BYTE		_cbAlpha,
				     COLORREF	_clrFont,
					 float		_fScale = 1.0f );
	
	BOOL	DrawCircle(	RECT		_rc3DCanvas,
						SIZE		_sizeSrcImage,
						POINT		 _ptCentre,
						double		_fRadius,
						BOOL		_bFill,
						BYTE		_cbAlpha,
						COLORREF	_clrCircle );

	BOOL	DrawCircularArc(	RECT		_rc3DCanvas,
							SIZE		_sizeSrcImage,
							POINT		 _ptCentre,
							double		_fRadius,
							unsigned short _usWidth, 
							double		_startRadius,
							double		_finishRadius,
							BYTE		_cbAlpha,
							COLORREF	_clrCircle );

	
	VOID	SetFontSize( int iFontSize );
	BOOL	GetTextExtent( LPCSTR _lpszText, SIZE* _psizePixel );

	VOID	RenderCalibrationGrid(RECT*	pClientRect, 
								float	fCameraHeight, 
								float	fTiltAngle_deg, 
								float	fYFOV_deg, 
								float	fPanAngle_deg, 
								float	fYRoll_deg, 
								SIZE	csVideoSize, 
								DWORD	totalObjectModel ,
								ObjectModel *pObj,
								BOOL	bSkyEnabled, 
								BOOL	bTransparentGrid, 
								BOOL	bRulerEnabled, 
								D3DXVECTOR3* rulerPos);

	LPDIRECTDRAWSURFACE7 GetBackDDS() { return m_pddsBackbuffer; };
	BOOL	PrimaryPresentation( RECT* const _prcScreen, RECT* const _prc3DCanvas );

	SIZE GetCurrent3DCanvasSize(){
		SIZE sizeRet = { m_dw3DCanvasWidth, m_dw3DCanvasHeight };
		return sizeRet;
	}

	SIZE GetCurrent3DTextureSize(){
		SIZE sizeRet = { m_dwTextureWidth, m_dwTextureHeight };
		return sizeRet;
	}
	
	static float	GetLength(D3DXVECTOR3 *pPos);

private:
	static CD3DAdapterInfo	m_D3dAdapterInfo;
	static BOOL m_bInitGlobalSetupCS;
	static CRITICAL_SECTION m_GlobalSetupCS;

	INT						m_nUseDDAdapterNo;
	LPDIRECTDRAW7			m_pDD;

	LPDIRECT3DDEVICE7    m_pd3dDevice;
	DWORD				 m_dw3DCanvasWidth;
	DWORD				 m_dw3DCanvasHeight;
	LPDIRECTDRAWSURFACE7 m_pddsPrimary;
	LPDIRECT3D7          m_pD3D;
	LPDIRECTDRAWSURFACE7 m_pddsBackbuffer;
	LPDIRECTDRAWCLIPPER  m_pddClipper;
	HWND				 m_hwndClipper;

	eCOLORFORMAT		 m_eD3dTextureCF;
	LPDIRECTDRAWSURFACE7 m_apddsTextures[8];
		
	LPDIRECTDRAWSURFACE7 m_pddsTexture;
	DWORD				 m_dwTextureWidth;
	DWORD				 m_dwTextureHeight;

	LPDIRECTDRAWSURFACE7 m_pddsBrushTextures[MAXCOUNT_BRUSHTEXTURES];

	CRITICAL_SECTION	m_crit;

	BOOL	InitDDInterface( INT _nUseDDAdapterNo, HWND _hwndWindow );
	VOID	DeinitDDInterface();

	BOOL	InitPrimarySurface();
	BOOL	Init3DDeviceAnd3DCanvasSurface( INT _nWidth, INT _nHeight );
	
	BOOL	InitImageTextureSurface( INT _nWidth, INT _nHeight, eCOLORFORMAT _eD3dTextureCF );
	VOID	DeinitSurface();
	
	BOOL	InitFont( LPCTSTR _lpszFontName, DWORD _dwHeight, DWORD _dwFlags );
	BOOL	DeinitFont();

	BOOL	InitBrushTexture( eBRUSHTYPE brushType, UINT bitmapId );
	BOOL	DeinitBrushTexture( eBRUSHTYPE brushType );

	static HRESULT WINAPI lpEnumSurfacesCallback2( LPDIRECTDRAWSURFACE7	lpDDSurface,
												    LPDDSURFACEDESC2		lpDDSurfaceDesc,
													LPVOID					lpContext );

private:// About Font
	INT		m_iUserFontSize;
	int		m_def_hint;
	DWORD	m_dwFontWidth;
	DWORD	m_dwFontHeight;
	float	m_fTexCoords[127-32+1][4];
	BOOL	m_bFontChanged;

	LPDIRECTDRAWSURFACE7	m_pddsFontTexture;

	DWORD	m_dwSavedStateBlock;
	DWORD	m_dwDrawTextStateBlock;
	BOOL	m_IsAllocStateBlock;
	BOOL	MakeStateBlock( LPDIRECTDRAWSURFACE7 const _pDDSurface );

private://Draw Implement
	private:
	#define				MAX_TLVERTEXT_IMAGE			(4)
	D3DTLVERTEX*		m_aptImageTLV;		// Triangle Vertext for Image

	#define				MAX_TLVERTEXT_RECTANDLINES	(0x4000)
	D3DTLVERTEX*		m_aptRectAndLineTLV;// Triangle Vertext for RECT and LINES

	#define				MAX_TLVERTEXT_FONT			(50 * 6)
	D3DTLVERTEX*		m_aptFontTLV;		// Triangle Vertext for Font

	typedef enum
	{
		DRT_POINTLIST	= D3DPT_POINTLIST,	// 1
		DRT_LINELIST	= D3DPT_LINELIST,	// 2
		DRT_LINESTRIP	= D3DPT_LINESTRIP,	// 3

		DRT_IMAGE		= 7,
		DRT_TEXT		= 8,
		DRT_RECTLIST	= 9,
		DRT_POLYGONLIST	= 10,
		DRT_CIRCLE		= 11,
		DRT_CIRCULARARC	= 12,
	}eDRAWRECORDTYPE;

	typedef struct
	{
		eDRAWRECORDTYPE drawType;
		DWORD			colorValue;
		SIZE			srcSize; //source size or source dimension...
		eBRUSHTYPE		brushType;
		union {
			//image
			struct {
				eCOLORFORMAT colorFormat;
				BYTE* pSrc;
				RECT  roi;
				DWORD flag;
				eROTATE rotate;
			}img;

			//POLYGONs
			struct {
				ULONG		nPolygon;
				MYPOLYGON*	aptPolygon;
			}polygons;

			//RECTs
			struct {
				ULONG nRect;
				RECT* rects;
			}rects;

			
			struct {
				ULONG  nPt;
				POINT* pts;
			}pts;

			//text
			struct {
				LPCSTR	pText;
				POINT	pt;		// start XY
				float	fScale;
			}text;

			//circle
			struct {
				POINT	ptCentre;		// centre of circle
				double	fRadius;		// radius
				BOOL	bFill;
			}circle;

			struct {
				POINT			ptCentre;		// centre of circle
				double			fRadius;		// radius
				unsigned short	usWidth;		// The width of the arc
				double			dStartRadius;
				double			dFinishRadius;
			}circularArc;
		};
	}tDRAWRECORD;


	BOOL DrawRecords( RECT			_rcTarget,
					   D3DVALUE		_foZValue,
					   ULONG		_nRecordCount,
					   tDRAWRECORD*	_aptDrawRecords );
	BOOL DrawImageTexture( LPDIRECTDRAWSURFACE7 const _pDDSurface, DWORD width, DWORD height,
							 RECT _rc3DCanvas, RECT _rcSrc, DWORD _dwColor, D3DVALUE _foZValue, eROTATE rotate = RT_000);
	

	BOOL	DrawText( float x, float y,RECT rc, DWORD dwColor,const char* strText,float fScale, int Space=0 );
	BOOL	DrawPolygonVCA( RECT			_rc3DCanvas,
							 D3DVALUE		_foZValue,
							 ULONG			_nRecordCount,
							 tDRAWRECORD*	_aptDrawRecords );
	ULONG	Triangulate( RECT _rc3DCanvas, tDRAWRECORD*	_aptDrawRecords );

	BOOL	UpdateBrushAlpha( eBRUSHTYPE brushType, BYTE _cbAlpha );
	BOOL	LoadTextureFromResource(LPDIRECTDRAWSURFACE7 *ppddsTexture, DWORD idRes, DWORD bmpsize);
	BOOL	FindSuitableTextureSurface( INT _nTargetWidth, INT _nTargetHeight );


private:	//Calibration
	HRESULT	InitDrawCalib();
	HRESULT DeInitDrawCalib();
	BOOL	InitGroundTexture();

	void	DrawPerson(int iModelIdx, ObjectModel *pObj);
	void	DrawGroundPlane(BOOL bSkyEnabled, BOOL bTransparentGrid);
	void	DrawRuler(D3DXVECTOR3 *pPos);
	
	ID3DXSimpleShape		*m_pSphereMesh;
	ID3DXSimpleShape		*m_pCylinderMesh;
	ID3DXSimpleShape		*m_pConeMesh;
//	D3DMATERIAL7			m_materialPerson[MAXNUM_OBJECTS];
	D3DDEVICEDESC7			m_d2dDevDesc;
	D3DLIGHT7				m_light;

	D3DVERTEX				m_vGroundVerticies[4];
	D3DLVERTEX				m_vHorizonVerticies[6];
	LPDIRECTDRAWSURFACE7	m_pddsGroundTexture;
	LPDIRECTDRAWSURFACE7	m_pddsRulerTexture;
	
	float					m_fHumanHeight;
};
