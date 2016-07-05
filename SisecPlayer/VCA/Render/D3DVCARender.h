#pragma once
#include "VCARender.h"
#include "CD3d.h"
#include "VCAMetaRenderAPI.h"

#define TEXT_ALIGN_CENTRE			0x1
#define TEXT_ALIGN_CENTRE_VERTICAL	0x2

#define COLOR_ALARM					RGB(255,0,0)
#define COLOR_NONALARM				RGB(255,255,0)
#define COLOR_BLACK					RGB(0,0,0)
#define COLOR_BLOB					RGB(0,255,255)
#define COLOR_STATBLOB				RGB(0xFF,0x33,0xFF)
#define COLOR_FIREBLOB				RGB(255,0,0)
#define COLOR_SMOKEBLOB				RGB(255,255,255)
#define COLOR_SCENECHANGE			RGB(255,0,0)

typedef enum 
{
	VCA_COLOR_BLOB					= 0x00000000,
	VCA_COLOR_ALARM,
	VCA_COLOR_NONALARM,
	VCA_COLOR_STATBLOB,
	VCA_COLOR_SMOKEBLOB,
	VCA_COLOR_FIREBLOB,
	VCA_COLOR_SCENECHANGE,
	VCA_NUM_RENDERCOLORS
}VCA_RENDER_COLORS; 


class CVCAD3DRender :public IVCARender
{
	friend class CCalibrationD3DRender;
public:
	CVCAD3DRender(void);
	virtual ~CVCAD3DRender(void);

	BOOL	Setup(HWND hWnd, CVCADataMgr *pDataMgr);
	void	Endup();
	void	OnChangeClientRect(RECT rect);
		
	BOOL	ChangeVCASourceInfo(BITMAPINFOHEADER *pbm);
	BOOL	RenderVCAData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG uRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp);

	void	SetRenderMode(UINT flags)	{ m_RenderMode = flags;}
	UINT	GetRenderMode()				{ return m_RenderMode; }

	BOOL	RenderVCAImage(BYTE *pImage, VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject);
	BOOL	RenderVCAMetaData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, BYTE *pMetadata, int nLength, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp);

	void	SetZonesVisible(BOOL bVisible)
	{
		m_bZonesVisible = bVisible;
	}
	bool	GetZonesVisible(){ return m_bZonesVisible; }
	void	SetColorFormat(CD3d::eCOLORFORMAT eColorFormat)
	{
		m_eColorFormat = eColorFormat;
	}
protected:
	BOOL			m_bSetup;
	HWND			m_hWnd;
	CD3d*			m_pD3d;
	CVCADataMgr*	m_pDataMgr;
	COLORREF		m_crRender[VCA_NUM_RENDERCOLORS];

	CD3d::eCOLORFORMAT	m_eColorFormat;

	UINT			m_RenderMode;
	RECT			m_ClientRect;
	SIZE			m_SourceSize;

	BOOL			m_bDraw;
	VMR_HANDLE		m_hVMR;
	int				m_tamperAlarmFlash;
	BOOL	m_bZonesVisible;

	BOOL	SetupVMR();
	
	void	DrawSingleZone(RECT rcVCAROI, VCA5_APP_ZONE *pZone);
	void	DrawZones(RECT rcVCAROI, VCA5_APP_ZONES *pZones);
	void	DrawCounters(RECT rcVCAROI, VCA5_APP_COUNTERS *pCounters);
	void	DrawSingleCounter(RECT rcVCAROI, VCA5_APP_COUNTER *pCounter);
	void	DrawVCAData(RECT rcVCAROI, VCA5_PACKET_OBJECTS *pObjects, VCA5_PACKET_EVENTS *pEvents, VCA5_PACKET_COUNTS *pCounters);
	void	DrawTargetID(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObjects, COLORREF color);
	void	DrawObjectInformation(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color);
	void	DrawTrail(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color, BOOL bFromCenter);
	void	DrawVCAHeaderMsg(VCA5_PACKET_HEADER* pHeader);
	void	DrawVCAMsg(LPCSTR lpszMsg, COLORREF color, POINT ptStartXY, unsigned int flags);
	void	DrawBoundinxBox(RECT rcVCAROI, VCA5_RECT	bBox, BYTE alpha, COLORREF color);
	void	DrawColorSignature(RECT rcVCAROI, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF color);
	BOOL	GetTextScalingFactor(int height, float& fScalor, UINT& uiFontSize, UINT& uiFontSizeRemain);
	void	GetTextSFBelowHalfD1Height(int height, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain);
	void	GetTextSFAboveHalfD1Height(int height, float &fScalor, UINT &uiFontSize, UINT &uiFontSizeRemain);
	
	void	DrawBlobMap(RECT rcCanvas, VCA5_PACKET_BLOBMAP *pBlobmap, VCA_RENDER_COLORS color);
	void	SetCanvasSize( RECT* pCanvas, RECT* pVCAROI=NULL );
	void	DrawArrow( POINT *pPoints, RECT _rc3DCanvas, SIZE _sizeSrcImage, COLORREF  colour, BYTE alpha);	// RENDER


	void    DrawDirectionArrow( CD3d *pD3D, int x, int y, int startangle, int finishangle );
	void    DrawDirectionArrow22(CD3d *pD3D, int x, int y, int startangle, int finishangle);
	void    DrawDirectionArrow33(CD3d *pD3D, int x, int y, int startangle, int finishangle);
	void    GetZonePie( VCA5_APP_ZONE *pZone, VCA5_POINT *pt );

	void	DrawDirectionArrow2( VCA5_APP_ZONE *pZone, int dir);
	void	DrawCounterCalibration( VCA5_APP_ZONE *pZone, int calibrationWidth);

	void	DrawCountingLinesCounting(CVCAMetaLib *pVCAMetaLib);
	void	DrawCountingLineEvent( VCA5_APP_ZONE* pZone, VCA5_COUNTLINE_EVENT *pInfo, int dir, unsigned char alpha);

	void	DrawThickLine(RECT *p_rc3DCanvas,POINT ptStart,POINT ptEnd,int alpha, COLORREF colour,
					unsigned int uiCanvasWidth, unsigned int uiCanvasDrawHeight, float fLineWidth = 2.0f );


};
