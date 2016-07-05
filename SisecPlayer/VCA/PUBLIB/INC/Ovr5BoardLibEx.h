#ifndef OVR5BOARDLIB_EX_H
#define OVR5BOARDLIB_EX_H

#include "Cmn5BoardLibEx.h"

#pragma pack( push, Ovr5BaordLibEx_H )
#pragma pack(8)

extern "C" const IID IID_IOvr5 ;

//IID for Ovr5
// {3710E024-27D3-4421-B173-7CDB57B60424}
DEFINE_GUID(IID_IOvr5, 0x3710e024, 0x27d3, 0x4421, 0xb1, 0x73, 0x7c, 0xdb, 0x57, 0xb6, 0x4, 0x24);

// {F5862BD5-EF58-4e62-92FC-4135C806094E}
DEFINE_GUID(IID_IOvr5_V2,0xf5862bd5, 0xef58, 0x4e62, 0x92, 0xfc, 0x41, 0x35, 0xc8, 0x6, 0x9, 0x4e);


#ifndef KERNEL_MODE_CODE
#include <ddraw.h>
#endif

typedef struct _OVR5_BOARD_INFO{
	ULONG		uBoardID;
	ULONG		uModelID;
	ULONG		uSlotNumber;

	ULONG		uMaxChannel;
	ULONG		uMaxPip;

	ULONG		uVideoDecoderType;
	ULONG		uMuxType;

	const CMN5_RESOLUTION_INFO* pResInfo;	//132*sizeof(ULONG)

	ULONG		reserved[CMN5_MAX_RESERVED_BOARD_INFO - 8];
} OVR5_BOARD_INFO;


//enum OvrPropertyCmd {
#define OVR5_OPC_MULTI_OVR_MODE		0x03000001
#define OVR5_OPC_MULTI_PIC_MODE		OVR5_OPC_MULTI_OVR_MODE
#define OVR5_OPC_BG_COLOR			0x03000002
#define OVR5_OPC_TX_MODE			0x03000008
#define OVR5_OPC_DT_MODE			0x03000010

#define	OVR5_OPC_SET_BASIS_SIZE		0x03040001	//DMP200 ONLY //uParam0:width, //uParam1:height

#define OVR5_OPC_VIDEO_FORMAT		0x03000080	// HP4K
#define OVR5_OPC_OVR_3x3_MODE		0x03000081	// HP4K // Obsolete
	#define OPC_OVR_3x3_MODE_NONE		0	// HP4K
	#define OPC_OVR_3x3_MODE_QCIF		1	// HP4K
	#define OPC_OVR_3x3_MODE_CIF		2	// HP4K
#define OVR5_OPC_CIF3X3_ENABLE		0x03000082	// HP4K
//};

//enum OvrAdjustCmd {
#define	OVR5_OAC_BRIGHTNESS		0x02000001
#define	OVR5_OAC_CONTRAST		0x02000002
#define	OVR5_OAC_SATURATION_U	0x02000004
#define	OVR5_OAC_SATURATION_V	0x02000008
#define	OVR5_OAC_HUE			0x02000010
#define	OVR5_OAC_SHARPNESS		0x02000020
#define	OVR5_OAC_BORDER			0x02010001

#define	OVR5_OAC_OSD_ENABLE		0x02010002	//uParam0 : OSD type (OVR5_OSD_TVOUT, OVR5_OSD_OVERLAY, OVR5_OSD_RAW_IMAGE), uParam1 : bEnable
#define	OVR5_OAC_OSD_PALETTE	0x02010004	//uParam0 : Palette size, uParam1 : DWORD type array pointer, 
#define	OVR5_OAC_OSD_UPDATE		0x02010008	//uParam0 : tDestination, uParam1 : pData, uParam2 : pRect, uParam3 : bDelayUpdate
#define	OVR5_OAC_TRANSFER_MODE	0x02010010	//uParam0 : OvrTransferMode
#define	OVR5_OAC_TX_FIELD_SINGLEVIEW	0x02010020	//uParam0 : TRUE : Field,  FALSE : Frame
#define OVR5_OAC_SURFACE_UPDATE_MODE	0x02010040
#define OVR5_OAC_VIDEO_OFFSET	0x02010080
#define OVR5_OAC_CASCADE_RESET  0x02010100 

#define	OVR5_OAC_MULTI_PIC_MODE	0x02020001	//HP4K ONLY	
											// == OVR5_OPC_MULTI_PIC_MODE
											//uParam0 : OvrMultOvrMode
											//uParam0 : OVR5_OMM_SELECT_MULTI_PIC
											//uParam1[0..7] EVO	External Video Output
											//uParam1[8..15] PCO PC Overlay

#define	OVR5_OAC_EXT_MON_SEL	0x03000001
#define OVR5_OAC_IMP_VIEW_BOARD	0x03000002
#define OVR5_OAC_FREEZE			0x03000004	//uParam0 : Freeze On/Off			

#define	OVR5_OAC_SET_OSD		0x03040001	//DMP200 ONLY //uParam0 : OSDbitmap
#define	OVR5_OAC_VOUT_SWITCH	0x03040002	//DMP200 ONLY //uParam0 : BOOL bSwitch
#define	OVR5_OAC_EVO_MARGIN		0x03040003	//DMP200 ONLY //uParam0 : BOOL bSwitch
//};


//enum OvrTransferMode
#define	OVR5_TRANSFER_FRAME			1
#define	OVR5_TRANSFER_FIELD			2
#define	OVR5_TRANSFER_HALF_FRAME	3
//};

//enum OvrScreenMode {
#define	OVR5_OSM_SCREEN_MODE_M720 	1
#define	OVR5_OSM_SCREEN_MODE_H648 	2
//};

//enum OvrMultOvrMode {
#define OVR5_OMM_SPLIT				1
#define OVR5_OMM_CASCADE			2
#define OVR5_OMM_COMBINATION		3

// renamed
#define OVR5_OMM_SPLIT_MULTI_PIC		OVR5_OMM_SPLIT		// split multi-picture mode
#define OVR5_OMM_CASCADE_MULTI_PIC		OVR5_OMM_CASCADE	// cascade multi-picture mode
#define OVR5_OMM_COMB_MULTI_PIC			OVR5_OMM_COMBINATION	// combination multi-picture mode
#define OVR5_OMM_INDIV_MULTI_PIC		4			// individual multi-picture mode
#define OVR5_OMM_ADV_INDIV_MULTI_PIC	5			// advanced individual multi-picture mode	
#define OVR5_OMM_TARGET_SEL_MULTI_PIC	6			// select output target mode. HP4K Only


#define	OVR5_TX_DIRECT_GRAPHICS			0
#define	OVR5_TX_INDIRECT_GRAPHICS_EVENT	1
#define	OVR5_TX_DIRECT_GRAPHICS_EVENT	2
#define OVR5_TX_DIRECT_MEMORY			3

#define	OVR5_SUM_INTERLACED				(1 << 0)
#define OVR5_SUM_PROGRESSIVE			(2 << 0)
#define OVR5_SUM_DEINTERLACER_MASK		(0x0000FF00)
#define OVR5_SUM_DEINTERLACER_NONE		(1 << 8)
#define OVR5_SUM_DEINTERLACER_DOUBLING	(2 << 8)
#define OVR5_SUM_MULTI_VIEW				(1 << 16)
#define OVR5_SUM_SINGLE_VIEW			(2 << 16)

//enum QueryInfoCmd {
#define  OVR_QR_BASE 0x2000 
#define  OVR5_QR_AVAILABLE_OVERLAY_MULTI_PIC_MODE  OVR_QR_BASE + 1
// 0x3000 ~ 0x3FFF : CMN5 QueryInfoCmd

// };
//}
//////////////////////////////////////////////////////////////////////////////////
// OVR_SPLIT_INFO

#define OVR5_OSF_UPDATE_EVO 		(1<<16) 	// Used at Ovr5Split

#define OVR5_MAX_RECT			64
#define OVR5_MAX_CHANNEL		64
#define OVR5_MAX_PIP			16		// Actual number of PIP channel should be extracted from OVR5_BOARD_INFO

typedef struct _OVR5_SPLIT_CONF{
	ULONG	uNumRects;
	SIZE	szBoundSize;
	RECT	rcRect[OVR5_MAX_RECT];
	ULONG	aChannel[OVR5_MAX_CHANNEL];
} OVR5_SPLIT_CONF;

typedef struct _OVR5_PIP_INFO{
	ULONG	uNumPip;
	RECT	rcRect[OVR5_MAX_PIP];
	ULONG	aChannel[OVR5_MAX_PIP];
} OVR5_PIP_INFO;

typedef struct _OVR5_SPLIT_INFO{
	OVR5_SPLIT_CONF		scSplitConf;
	OVR5_PIP_INFO		piPipInfo;
	RECT				rcOverlaySrcRect;
} OVR5_SPLIT_INFO;


// OVR5_OPC_DT_MODE
#define	OVR5_DT_SINGLE_FIELD_ODD		0
#define	OVR5_DT_SINGLE_FIELD_EVEN		1
#define	OVR5_DT_MULTI_SPLIT_FRAME		2
#define	OVR5_DT_MULTI_LINEAR_FRAME		3

#define OVR5_DATA_ITEM_RESERVED			8	
#define OVR5_DATA_INFO_RESERVED			16 	

typedef struct _OVR5_DATA_ITEM {
	ULONG			uBoardNum;	// Board ID[0..3]
	ULONG			uOvrDataType;		// data type 
	_OVR5_SPLIT_INFO	*pSplitInfo;	// Current Split Info
	UCHAR			*pDataBuffer;		// image buffer address
	ULONG			uBufferIdx;			// internal use! do not touch!
	
	ULONG			reserved[OVR5_DATA_ITEM_RESERVED];	
}OVR5_DATA_ITEM;


typedef struct _OVR5_DATA_INFO {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
		
	ULONG			uItemCnt;
	OVR5_DATA_ITEM	uOvrDataItems[CMN5_SYSTEM_MAX_BOARD];	// data type 
	ULONG			reserved[OVR5_DATA_INFO_RESERVED];
	
} OVR5_DATA_INFO;


// OVR_SPLIT_INFO
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// OVR_OSD_INFO

#define OVR5_OSD_TVOUT		0x00000001
#define OVR5_OSD_OVERLAY	0x00000002
#define OVR5_OSD_RAW_IMAGE	0x00000004 

#define OVR5_QR_OSD_INFO	0x2000	// 
typedef struct {
	DWORD	bSupport;	//bitwise OVR_OSD_type
	SIZE	szNTSC;			
	SIZE	szPAL;			
	DWORD	PaletteSize;
	BYTE	ClearByte;	
	DWORD	Reserved[4];
} OVR5_OSD_INFO;


//////////////////////////////////////////////////////////////////////////////////
// Query Command for OVR5
#define OVR5_QIC_OSD_INFO			OVR5_QR_OSD_INFO
#define OVR5_QIC_CASCADE_MULTI_EVO  0x2001

//////////////////////////////////////////////////////////////////////////////////

#ifndef KERNEL_MODE_CODE

typedef struct _OVR5_OVERLAY_OBJ_INFO{
	LPDIRECTDRAW7			pDD;
	LPDIRECTDRAWSURFACE7	pDDSOverlay;
	LPDIRECTDRAWSURFACE7	pDDSOverlayBack;
	ULONG					reserved[13];
} OVR5_OVERLAY_OBJ_INFO;


extern "C"
{
//////////////////////////////////////////////////////////////////////////////////
// APIs

// Control
BOOL CMN5_API Ovr5Activate(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]);

BOOL CMN5_API Ovr5Setup(void);
BOOL CMN5_API Ovr5Endup(void);
BOOL CMN5_API Ovr5Run(void);
BOOL CMN5_API Ovr5Stop(void);

BOOL CMN5_API Ovr5CaptureEnable(ULONG uBdID, BOOL bEnable);

// Video format
BOOL CMN5_API Ovr5SetScreenMode(ULONG uBdID, ULONG uScreenMode);
BOOL CMN5_API Ovr5SetVideoFormat(ULONG uBdID, ULONG uFormat);

// Image Processor
BOOL CMN5_API Ovr5Split(ULONG uBdID, OVR5_SPLIT_INFO* pSplitInfo);
BOOL CMN5_API Ovr5EnableChannel(ULONG uBdID, ULONG uCh, BOOL bEnable);

// Video Decoder
BOOL CMN5_API Ovr5SetOvrProperty(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Ovr5GetOvrProperty(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Ovr5SetOvrAdjust(ULONG uBdID, ULONG uCh, ULONG uOvrAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Ovr5GetOvrAdjust(ULONG uBdID, ULONG uCh, ULONG uOvrAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Ovr5SetMultiOvrProperty(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Ovr5GetMultiOvrProperty(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

BOOL CMN5_API Ovr5SetMultiOvrAdjust(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Ovr5GetMultiOvrAdjust(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

// DirectDraw
BOOL CMN5_API Ovr5SetOverlayObjectInfo(OVR5_OVERLAY_OBJ_INFO* pInfo);

// Error code
BOOL CMN5_API Ovr5GetErrorCode(CMN5_ERROR_CODE_ITEM* item);

// User EEPROM
BOOL CMN5_API Ovr5ReadUserEEPROM(ULONG uBdID, ULONG uVPID, UCHAR *pData);
BOOL CMN5_API Ovr5WriteUserEEPROM(ULONG uBdID, ULONG uVPID,const UCHAR *pData);

// Information
BOOL CMN5_API Ovr5GetLastErrorCode(CMN5_ERROR_CODE_ITEM *pEcode);

BOOL CMN5_API Ovr5QueryInfo(ULONG uQueryInfoCmd, ULONG uIn, void *pOut);

BOOL CMN5_API Ovr5GetSystemInfo(CMN5_SYSTEM_INFO *pInfo);
BOOL CMN5_API Ovr5GetBoardInfo(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, OVR5_BOARD_INFO *pInfo);

BOOL CMN5_API Ovr5SetOvrProperty(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Ovr5GetOvrProperty(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Ovr5SetMultiOvrProperty(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Ovr5GetMultiOvrProperty(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

BOOL CMN5_API Ovr5GetEventData(ULONG uType, void *pData);
BOOL CMN5_API Ovr5ReleaseData(const void *pData);
BOOL CMN5_API Ovr5SetEventHandle(ULONG uType, HANDLE hHandle);
BOOL CMN5_API Ovr5GetEventHandle(ULONG uType, HANDLE *pHandle);



} // extern "C"

// Interface definition for IOvr5
interface IOvr5 : IUnknown
{
// Control
	STDMETHOD_(BOOL, Ovr5Activate)(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]) PURE ;

	STDMETHOD_(BOOL, Ovr5Setup)(void) PURE;
	STDMETHOD_(BOOL, Ovr5Endup)(void) PURE;
	STDMETHOD_(BOOL, Ovr5Run)(void) PURE;
	STDMETHOD_(BOOL, Ovr5Stop)(void) PURE;
	
	STDMETHOD_(BOOL, Ovr5CaptureEnable)(ULONG uBdID, BOOL bEnable) PURE;
	
	// Video format
	STDMETHOD_(BOOL, Ovr5SetScreenMode)(ULONG uBdID, ULONG uScreenMode) PURE;
	STDMETHOD_(BOOL, Ovr5SetVideoFormat)(ULONG uBdID, ULONG uFormat) PURE;
	
	// Image Processor
	STDMETHOD_(BOOL, Ovr5Split)(ULONG uBdID, OVR5_SPLIT_INFO* pSplitInfo) PURE;
	STDMETHOD_(BOOL, Ovr5EnableChannel)(ULONG uBdID, ULONG uCh, BOOL bEnable) PURE;
	
	// Video Decoder
	STDMETHOD_(BOOL, Ovr5SetOvrAdjust)(ULONG uBdID, ULONG uCh, ULONG uOvrAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Ovr5GetOvrAdjust)(ULONG uBdID, ULONG uCh, ULONG uOvrAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;

	STDMETHOD_(BOOL, Ovr5SetMultiOvrAdjust)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Ovr5GetMultiOvrAdjust)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	
	// DirectDraw
	STDMETHOD_(BOOL, Ovr5SetOverlayObjectInfo)(OVR5_OVERLAY_OBJ_INFO* pInfo) PURE;
	
	// Error code
	STDMETHOD_(BOOL, Ovr5GetErrorCode)(CMN5_ERROR_CODE_ITEM* item) PURE;
	
	// User EEPROM
	STDMETHOD_(BOOL, Ovr5ReadUserEEPROM)(ULONG uBdID, ULONG uVPID, UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Ovr5WriteUserEEPROM)(ULONG uBdID, ULONG uVPID,const UCHAR *pData) PURE;
	
	// Information
	STDMETHOD_(BOOL, Ovr5GetLastErrorCode)(CMN5_ERROR_CODE_ITEM *pEcode) PURE;
	
	STDMETHOD_(BOOL, Ovr5QueryInfo)(ULONG uQueryInfoCmd, ULONG uIn, void *pOut) PURE;
	
	STDMETHOD_(BOOL, Ovr5GetSystemInfo)(CMN5_SYSTEM_INFO *pInfo) PURE;
	STDMETHOD_(BOOL, Ovr5GetBoardInfo)(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, OVR5_BOARD_INFO *pInfo) PURE;

	STDMETHOD_(BOOL, Ovr5SetOvrProperty)(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Ovr5GetOvrProperty)(ULONG uBdID, ULONG uCh, ULONG uOvrPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;

	STDMETHOD_(BOOL, Ovr5SetMultiOvrProperty)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Ovr5GetMultiOvrProperty)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
};


//add user event interface 
interface IOvr5_V2 : public IOvr5
{
	STDMETHOD_(BOOL, Ovr5SetEventHandle)(ULONG uType, HANDLE hHandle) PURE;
	STDMETHOD_(BOOL, Ovr5GetEventHandle)(ULONG uType, HANDLE *pHandle) PURE;
	
	STDMETHOD_(BOOL, Ovr5GetEventData)(ULONG uType, void *pData) PURE;
	STDMETHOD_(BOOL, Ovr5ReleaseData)(const void *pData) PURE;
};


#endif

#pragma pack( pop, Ovr5BaordLibEx_H )

#endif // OVR5BOARDLIB_EX_H