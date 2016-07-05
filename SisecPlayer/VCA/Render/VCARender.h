#pragma once

//#include "VCAConfigure.h"
#include "VCAMetaLib.h"
#include"VCADataMgr.h"
//class CVCADataMgr;

class IVCARender
{
public:

	virtual ~IVCARender() {;}

	enum {
		RENDER_VCA = 1,
		RENDER_CALIBRATION = 2,
		RENDER_TAMPER = 3
	};

	enum {
		DISPLAY_BLOBS					= 0x00000001, 
		DISPLAY_NON_ALARMS				= 0x00000002 ,
		DISPLAY_TARGETID				= 0x00000004,
		DISPLAY_BGD_IMG					= 0x00000008,
		DISPLAY_OBJECT_HEIGHT			= 0x00000010, 
		DISPLAY_OBJECT_SPEED			= 0x00000020, 
		DISPLAY_OBJECT_AREA				= 0x00000040, 
		DISPLAY_OBJECT_CLASSIFICATION	= 0x00000080,
		DISPLAY_OBJECT_COLOR_SIGNATURE	= 0x00000100,
		DISPLAY_TAMPER_ALARM			= 0x00000200,	
		DISPLAY_STABLIZER_INFO			= 0x00000300,
		DISPLAY_SCENECANGE				= 0x00000800,	

		CALIB_PAUSE						= 0x00010000
	};
	
	virtual BOOL	Setup(HWND hWnd, CVCADataMgr *pDataMgr) = 0;
	virtual void	Endup() = 0;
	virtual void	OnChangeClientRect(RECT rect) = 0;
		
	virtual void	SetRenderMode(UINT flags)	= 0;
	virtual	UINT	GetRenderMode()			= 0;

	virtual BOOL	ChangeVCASourceInfo(BITMAPINFOHEADER *pbm) = 0;

	virtual BOOL	RenderVCAData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp) = 0;
	virtual BOOL	RenderVCAMetaData(BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG ulRotate, BYTE *pMetadata, int nLength, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp) = 0;
};
