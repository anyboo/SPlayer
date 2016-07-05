#pragma once
#include "D3DVCARender.h"
#include "CD3d.h"

class CCalibrationD3DRender : public IVCARender
{
public:

	CCalibrationD3DRender(HWND hwnd);
	virtual ~CCalibrationD3DRender(void);

	BOOL	Setup(HWND hwnd,CVCADataMgr *pDataMgr);
	void	Endup();
	void	OnChangeClientRect(RECT rect);

	BOOL	SetVCASourceInfo(int width, int height);
	BOOL	ChangeVCASourceInfo(BITMAPINFOHEADER *pbm);
	BOOL	RenderVCAImage(BYTE *pImage, VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject)
	{
		return m_VCARender.RenderVCAImage(pImage, pZone, pRule, pObject);
	}
	BOOL	RenderVCAData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp);
	
	void	SetRenderMode(UINT flags)		{ m_RenderMode = flags; m_VCARender.SetRenderMode(flags);}
	UINT	GetRenderMode()				{ return m_RenderMode; }

	BOOL	RenderVCAMetaData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, BYTE *pMetadata, int nLength, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp){
		return m_VCARender.RenderVCAMetaData(pImage, pbm, ulRotate, pMetadata, nLength, rcImageROI, rcVCAROI, pTimestamp);
	}
	void	SetZonesVisible(BOOL bVisible){
		m_VCARender.SetZonesVisible(bVisible);
	}
	bool	GetZonesVisible(){ return m_VCARender.GetZonesVisible(); }

	void	SetColorFormat(CD3d::eCOLORFORMAT eColorFormat)
	{
		m_VCARender.SetColorFormat(eColorFormat);
	}
protected:
	void	DrawVCAData(VCA5_PACKET_OBJECTS *pObjects, VCA5_PACKET_EVENTS *pEvents, VCA5_PACKET_COUNTS *pCounters);
	void	SetCanvasSize( RECT* pCanvas );
	void	DrawGrid();
	
	UINT	m_RenderMode;
	CVCAD3DRender m_VCARender;

	// duplicated pointers corresponding to member variables of CVCAD3DRender
	CD3d*			m_pD3d;
	CVCADataMgr*	m_pDataMgr;
	RECT*			m_pClientRect;
	HWND m_hWnd;

	BYTE*	m_pBackBufImage;	
	BOOL	m_bPauseImageCopied;

	UINT	GetImageSize();

	void	DrawRulerMeasure(float fLength, int meaUnits, POINT ptStartMsg);
	void	DrawMsg( POINT ptStartMsg,char* pszMsg,int alpha, COLORREF colour);

};
