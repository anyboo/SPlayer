#ifndef OVR2BOARDLIB_H
#define OVR2BOARDLIB_H

#ifndef KERNEL_MODE_CODE
#include <ddraw.h>

#if defined(OVR2LIB_EXPORTS)
#	define OVR2_API	extern "C"
#else
#	define OVR2_API	extern "C" __declspec(dllimport) 
#endif

#endif

#define CALLING_API	__stdcall

#if !defined(UDA_ULONG_DEFINED)
#	if !defined(_WIN64)
		typedef ULONG		UDA_ULONG;
#	else
		typedef ULONG_PTR	UDA_ULONG;
#	endif
#	define UDA_ULONG_DEFINED
#endif

#pragma pack( push, Ovr2BoardLib_h )
#pragma pack(1)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define, enum, struct for both OvrOemBoardLib.h and Ovr2BoardLib.h

#ifndef DVRDLL_H
#define DVRDLL_H

/*
	(2003-09-23)
	Renamed conflicted definitons and typedefs with capture device.
*/

//______________________________________________________________________________________________
// Definition of ErrorCodeItem

#if !defined (CAP_OVR_AUD)													//DRV <-> DLL <-> APP Common

#ifndef ERRORCODEITEM_DEFINITION
#define ERRORCODEITEM_DEFINITION


#define MAX_ERRCMN_ERROR_CODE_MSG_LENGTH		128
#define MAX_ERRCMN_SESSION_LENGTH				64

#endif // ERRORCODEITEM_DEFINITION

struct ErrorCodeItem {
	ULONG			ErrorCode;										// code
	LARGE_INTEGER	TimeStamp;										// time-stamp
	ULONG			Reserved;										// reserved
	char			AuxMsg[MAX_ERRCMN_ERROR_CODE_MSG_LENGTH + 1];	// aux message
	char			SessionName[MAX_ERRCMN_SESSION_LENGTH + 1];		// session name
};

typedef enum
{
	SM_SCALE_1_1	= 0,
	SM_SCALE_1_2,
	SM_SCALE_1_3,
	SM_SCALE_1_4,
	SM_SCALE_2_3,
	SM_SCALE_3_4,
	SM_SCALE_NONE
} DISPLAY_SCALEMODE;

#endif //CAP_OVR_AUD

typedef enum
{
	DEC_VIDEO_FORMAT		= 0,
	DEC_BRIGHTNESS,
	DEC_CONTRAST,
	DEC_HUE,
	DEC_SATURATION_U,
	DEC_SATURATION_V,
	DEC_VDELAY,
	DEC_HDELAY,
	DEC_ADDR_DATA,
	DEC_SHARPNESS,
	DEC_MAX_ATTR,
} DEC_ATTRS;

enum OVR_DEC_TYPE {
	OVR_VIDEO_DEC_TW98 = 0,
	OVR_VIDEO_DEC_BT835,
	OVR_VIDEO_DEC_SAA7113H,
	OVR_VIDEO_DEC_TW9903,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// End of #define, enum, struct for both OvrOemBoardLib.h and Ovr2BoardLib.h
#endif // DVRDLL_H

enum OvrVideoForamt {
	OVR_VIDEO_FORMAT_NTSC = 0,
	OVR_VIDEO_FORMAT_PAL
};

enum OvrScreenMode {
	OVR_SCREENMODE_M720 = 0,
	OVR_SCREENMODE_H648
};

enum OvrTransferMode {
	OVR_TRANSFER_FRAME = 1,
	OVR_TRANSFER_FIELD,
	OVR_TRANSFER_HALF_FRAME
};

enum OvrMultOvrMode {
	OVR_OMM_SPLIT		= 1,
	OVR_OMM_CASCADE
};

enum OvrTxTargetMode {
	OVR_TX_DIRECT_GRAPHICS		= 0,
	OVR_TX_INDIRECT_GRAPHICS_EVENT,
	OVR_TX_DIRECT_GRAPHICS_EVENT
};


#define OVR_MAX_RECT	16

//VIEWINFO TYPE FLAGS..
#define OVR_VITF_LATTICE	0x00000001
#define OVR_VITF_BIGIMAGE	0x00000002
#define OVR_VITF_WIDEIMAGE	0x00000004

struct OVR_SPLIT_CONF {
	ULONG	nRects;
	ULONG	Flags;
	SIZE	BoundSize;
	RECT	Rect[OVR_MAX_RECT];
};

#define OVR_MAX_PIP		4		// Actual number of PIP channel should be extracted from OVR_SYSTEM_INFO

struct OVR_PIP_INFO {
	ULONG	nPip;
	ULONG	Channel[OVR_MAX_PIP];
	RECT	Rect[OVR_MAX_PIP];
};

#define OVR_MAX_CHANNEL	16

struct OVR_SPLIT_INFO {
	OVR_SPLIT_CONF		SplitConf;
	ULONG				Channel[OVR_MAX_CHANNEL];
	OVR_PIP_INFO		PipInfo;
	RECT				OverlaySrcRect;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions introduced in RTIV model

#define OVR_ACTIVATION_CODE_SIZE	16

//! The size of user accessible region in the EEPROM.
#define OVR_USER_EEPROM_SIZE		16

/*!
 * System information
 */
typedef struct {
	ULONG	SystemId;
	ULONG	ModelId;
	ULONG	NumOfVideoDecoder;
	ULONG	TypeOfVideoDecoder;
	ULONG	NumOfPipChannelSupported;
	ULONG	BoardType;
	ULONG	SysVersion;
	ULONG	DllVersion;
	UCHAR	Reserved[16];
} OVR_SYSTEM_INFO;

/*!
 * System information descriptor. This structure must be filled before getting system information.
 */
typedef struct {
	ULONG	InfoSize;
	ULONG	InfoName;
	ULONG	InfoVersion;
	ULONG	Reserved;
} OVR_SYSTEM_INFO_DESC;

#define OVR_SYSTEM_INFO_NAME		0x56495452	// reversed 'RTIV'
#define OVR_SYSTEM_INFO_VERSION		1

#define OVR_OVERLAY_SURFACE_WIDTH	720
#define OVR_OVERLAY_SURFACE_HEIGHT	580


//OSD Function
#define OVR_PT_BORDER		0x1000

#define OVR_OSD_TVOUT		0x00000001
#define OVR_OSD_OVERLAY		0x00000002
#define OVR_OSD_RAW_IMAGE	0x00000004 


#define	OVR_OSD_ENABLE		0x02010002	//uParam0 : OSD type (OVR_OSD_TVOUT, OVR_OSD_OVERLAY, OVR_OSD_RAW_IMAGE), uParam1 : bEnable
#define	OVR_OSD_PALETTE		0x02010004	//uParam0 : Palette size, uParam1 : DWORD type array pointer, 
#define	OVR_OSD_UPDATE		0x02010008	//uParam0 : tDestination, uParam1 : pData, uParam2 : pRect, uParam3 : bDelayUpdate
#define OVR_TRANSFER_MODE	0x02010010	//uParam0 : OvrTransferMode
#define	OVR_TX_FIELD_SINGLEVIEW	0x02010020	//uParam0 : TRUE : Field,  FALSE : Frame


#define OVR_MULTI_OVR_MODE	0x03000001
#define OVR_BG_COLOR		0x03000002
#define OVR_FREEZE			0x03000004	//uParam0 : Freeze On/Off			
#define OVR_TX_TARGET_MODE	0x03000008	//uParam0 : OvrTxTargetMode


#define	OVR_QR_OSD_INFO		0x00002000	
typedef struct {
	DWORD	bSupport;	//bitwise OVR_OSD_type
	SIZE	szNTSC;			
	SIZE	szPAL;			
	DWORD	PaletteSize;
	BYTE	ClearByte;	
	DWORD	Reserved[4];
} OVR_OSD_INFO;




// End of definitions introduced in RTIV model
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// API

// Control
OVR2_API BOOL 	CALLING_API Ovr2Setup(UCHAR* pActivationCode);
OVR2_API BOOL 	CALLING_API Ovr2Endup(void);
OVR2_API BOOL 	CALLING_API Ovr2Run(void);
OVR2_API BOOL 	CALLING_API Ovr2Stop(void);
OVR2_API BOOL 	CALLING_API Ovr2CaptureStart(void);
OVR2_API BOOL 	CALLING_API Ovr2CaptureStop(void);

// Video format
OVR2_API BOOL 	CALLING_API Ovr2SetVideoFormat(DWORD videoFormat, DWORD screenMode);

// Image Processor
OVR2_API BOOL	CALLING_API Ovr2Split(OVR_SPLIT_INFO* pSplitInfo);
OVR2_API BOOL	CALLING_API Ovr2EnableChannel(DWORD channel, BOOL bEnable);

// Video Decoder
OVR2_API BOOL	CALLING_API Ovr2SetDecAttrs(DWORD camera, DEC_ATTRS cmd, DWORD data);
OVR2_API BOOL	CALLING_API Ovr2GetDecAttrs(DWORD camera, DEC_ATTRS cmd, DWORD* data);

// DirectDraw
OVR2_API BOOL	CALLING_API Ovr2AdaptOverlaySurface(LPDIRECTDRAWSURFACE7 pDDSOverlay);

// Error code
OVR2_API BOOL	CALLING_API Ovr2GetErrorCode(ErrorCodeItem* item);

// User EEPROM
OVR2_API BOOL	CALLING_API Ovr2ReadUserEEPROM(UCHAR* pData);
OVR2_API BOOL	CALLING_API Ovr2WriteUserEEPROM(UCHAR* pData);

// Device Information
OVR2_API BOOL	CALLING_API Ovr2GetSystemInfo(OVR_SYSTEM_INFO_DESC* pDesc, void* pInfo);

OVR2_API BOOL	CALLING_API Ovr2SetProperty(ULONG uCh, ULONG uCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
//
//Properties Cmd
//uParam0[in, out] : width 
//uParam1[in, out] : height
#define OVR2_CANVAS_SIZE	0x00000001		// uCh: Don't Care

OVR2_API BOOL	CALLING_API Ovr2GetProperty(ULONG uCh, ULONG uCmd, UDA_ULONG* uParam0, UDA_ULONG* uParam1, UDA_ULONG* uParam2, UDA_ULONG* uParam3);




// OSD Border 
//Border Flag
#define	OVR_BORDER_DRAW_EDGE	0x00000001
// 0x00RGB	:RGB32(0,R,G,B)
OVR2_API BOOL	CALLING_API Ovr2SetBorder(BOOL bEnabe, DWORD width, DWORD dwRGB, DWORD BorderFlag);

//  OSD GDI fuction
#define OSD_OBJECT_RECT		1	// 
OVR2_API BOOL	CALLING_API Ovr2OSDDrawObject(DWORD uType, DWORD uX, DWORD uY, DWORD uEX, DWORD uEY, 
												DWORD uRGB, DWORD uAlpha, DWORD bwProperty);

// OVR_OSD_IMAGE.uImageType 
#define OSD_IMAGE_TYPE_NONE		0								
#define OSD_IMAGE_TYPE_YUY2		1
#define OSD_IMAGE_TYPE_RGB32	2


// uBitDepth : RX3, OVR3500 supply 4bit Depth
OVR2_API BOOL	CALLING_API Ovr2OSDSetPalette(DWORD uImageType, DWORD uBitDepth, DWORD nNumofPalette, DWORD *pColor);
					

//BItmap bitwize Property
// OVR_OSD_IMAGE.bwProperty
#define OVR_OSD_USE_COLORMASK		(1<<0)
#define OVR_OSD_USE_PALETTE			(1<<1)

typedef struct  {
	DWORD 	uVersion;		// 0x0100
	DWORD 	uLength;		// Structure size: Total Size = 4*20
	
	DWORD 	uImageType;		// YUY2,RGB32,PALETTE
	
	DWORD	uSrcX, uSrcY;
	DWORD 	uSrcWidth, uSrcHeight;
	DWORD	uSrcPitch;			// 
	DWORD 	uDstX, uDstY; 
	DWORD 	uDstWidth, uDstHeight;	//not implement.. 
	 
	BYTE 	*pImageData;	// RawImageData, palette: uBitDepth(4bit index value) 
	
	DWORD 	uAlpha;			// 0~255 
	DWORD 	bwProperty;

	DWORD 	dwColorMASK;	// COLORMASK, PALETTE
	
	DWORD	uRev[4];		// Total Size = 4*20 (This Size Fixed 80 Byte...)
}OVR_OSD_IMAGE;

OVR2_API BOOL	CALLING_API Ovr2OSDDrawImage(OVR_OSD_IMAGE *pOsd);



#pragma pack( pop, Ovr2BoardLib_h )

#endif // OVR2BOARDLIB_H