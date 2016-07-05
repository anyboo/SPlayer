#ifndef CAP5BOARDLIB_EX_H
#define CAP5BOARDLIB_EX_H

#include "Cmn5BoardLibEx.h"

#pragma pack( push, Cap5BaordLibEx_H )
#pragma pack(8)

extern "C" const IID IID_ICap5 ;

//IID for Cap5
// {CFAA53A4-C037-413f-AFF1-AB87B1BBB403}
DEFINE_GUID( IID_ICap5, 0xcfaa53a4, 0xc037, 0x413f, 0xaf, 0xf1, 0xab, 0x87, 0xb1, 0xbb, 0xb4, 0x3 );

typedef struct _CAP5_BOARD_INFO {
	ULONG		uBoardID;
	ULONG		uModelID;
	ULONG		uSlotNumber;

	ULONG		uMaxVP;
	ULONG		uMaxChannel;
	ULONG		uMaxDO;
	ULONG		uMaxDI;

	const CMN5_RESOLUTION_INFO *pResInfo;	//132*sizeof(ULONG)

	ULONG		uHWRevision;		//add 2005, 8,2 
	ULONG		reserved[CMN5_MAX_RESERVED_BOARD_INFO - 9];
} CAP5_BOARD_INFO;

typedef struct _CAP5_DATA_INFO {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;			// Board ID[0..3]
	ULONG			uChannelNum;		// Channel ID of the Board
	ULONG			uHasNextData;
	UCHAR			*pDataBuffer;		// image buffer address
#if _MSC_VER >= 1300
	LARGE_INTEGER_CAP	TimeTag;			// KeQuerySystemTime()
#else
	LARGE_INTEGER	TimeTag;			// KeQuerySystemTime()
#endif
	ULONG			uBufferIdx;			// internal use! do not touch!
	
	//-- Cap5 extended
	ULONG			uImageSize;
	ULONG			uImageFormat;			
} CAP5_DATA_INFO;

typedef struct _CAP5_DATA_INFO_EX {
	_CMN5_DATA_INFO_DATA_EX_CMM_BODY;
	union {
		ULONG			reserved[32];
		struct {
			ULONG		uImageSize;
			ULONG		uImageFormat;
#if 1	// HP4K only
			ULONG		uColorFormat;	
			// in case of yuv packed format. // ex) CAP5_COLOR_FORMAT_YV12
			UCHAR		*pYData;		
			UCHAR		*pUData;
			UCHAR		*pVData;
			ULONG		uYPitch;
			ULONG		uUVPitch;
			ULONG		uSequenceNumber;
#endif
		};
	};
} CAP5_DATA_INFO_EX;

// HP4K {
#define CAP5_MAX_IMGSZ  16   	// QCIF,CIF,2CIF,D1 
#define CAP5_MAX_VIDEOFORMAT 2  // NTSC , PAL 
#define CAP5_INVALID_VALUE				0xFFFFFFFF		

typedef struct _CAP5_FRAMERATE_INFO {
	ULONG 	uBoardID;  
	ULONG     uMaxWeight[CAP5_MAX_VIDEOFORMAT]; 	 // VideoFormat [0:NTSC 1:PAL] 
	ULONG     aImgsizeWeight[CAP5_MAX_IMGSZ];   		// VideoFormat [0:NTSC 1:PAL] , ImageSize [0:QCIF 1:CIF 2:HALF 3:D1 4:... Reserved ]   if Value is 0xFFFFFFFF than means invalid value 
	ULONG     uWeightDivider;  //
	ULONG     nImageCount; 
	ULONG	reserved[3];
} CAP5_FRAMERATE_INFO;
// }

//enum ColorFormatValues {	
#define	CAP5_COLOR_FORMAT_RGB24		0x11					
#define	CAP5_COLOR_FORMAT_RGB16		0x22		// Not implemented
#define	CAP5_COLOR_FORMAT_RGB15		0x33		// Not implemented
#define	CAP5_COLOR_FORMAT_YUY2		0x44
#define	CAP5_COLOR_FORMAT_Y8		0x66
#define	CAP5_COLOR_FORMAT_YUV12		0x88		// I420 , IYUV	YUV 4:1:1
#define	CAP5_COLOR_FORMAT_I420		CAP5_COLOR_FORMAT_YUV12
#define	CAP5_COLOR_FORMAT_IYUV		CAP5_COLOR_FORMAT_YUV12
#define	CAP5_COLOR_FORMAT_YUV9		0x99
#define	CAP5_COLOR_FORMAT_YV12		0xAA		// YVU 4:1:1
//};

#define CAP5_DEFAULT_COLOR_FORMAT CAP5_COLOR_FORMAT_YUY2

//enum FrameRateCmd {
#define	CAP5_FRC_AUTO_EVEN			1
#define	CAP5_FRC_USER_FIXED			2
#define CAP5_FRC_USER_FIXED_ACCURATE		CAP5_FRC_USER_FIXED
#define CAP5_FRC_USER_FIXED_D1_ACCURATE 	3
//};

//enum VideoPropertyCmd {
#define	CAP5_VPC_VIDEO_FORMAT	(1<<0)
#define	CAP5_VPC_COLOR_FORMAT	(1<<1)
#define	CAP5_VPC_IMAGE_SIZE		(1<<2)
#define	CAP5_VPC_SWITCHING_LEVEL	(1<<3)
#define CAP5_VPC_NTSC_7_5_IRE_SETUP  (1<<4)    // Currently, HP3000 was tested only.

// HP4K
#define HP4K_VPC_STREAMING			0x01080000
#define CAP5_VPC_ENABLE_CHANNEL		0x00080001 // HP4K only
//define CAP5_VPC_IMAGE_SIZE		0x00080002 // HP4K only
#define CAP5_VPC_FRAMERATE			0x00080003 // HP4K only
//};

//enum VideoAdjustCmd {
#define	CAP5_VAC_BRIGHTNESS			0x00000001
#define	CAP5_VAC_CONTRAST			0x00000002
#define	CAP5_VAC_SATURATION_U		0x00000004
#define	CAP5_VAC_SATURATION_V		0x00000008
#define	CAP5_VAC_HUE				0x00000010
#define	CAP5_VAC_SHARPNESS			0x00000020
#define	CAP5_VAC_AUD_GAIN			0x00000080		// HP4K only
#define CAP5_VAC_COLORADJUSTALL     0x00008000
	
#define	CAP5_VAC_VM_SEL_CHANNEL		0x00030001 // exclusively used only at MP204
#define	CAP5_VAC_SEL_EXT_VIDEO_OUT	0x00030002 // exclusively used only at specific model(0x10A7)
#define CAP5_VAC_AGC_MODE			0x00030004 // Param0 : AGC command,  
#define CAP5_VAC_AGC_LEVEL			0x00030008 // Param0 : AGC Level,  
#define CAP5_VAC_IMAGE_SIZE			0x00030100 // exclusively used only at ECPR Core, same as CAP5_VPC_IMAGE_SIZE Parameters
#define	CAP5_VAC_AGC_ON				0x00030200
#define	CAP5_VAC_VIDEO_OFFSET		0x00030400
#define	CAP5_VAC_REGISTER			( (1<<24) | (2<<20) | 0x3F3 )	// reserved command.
// HP4K
#define CAP5_VAC_ENABLE_CHANNEL		0x00180001 // HP4K only
//define CAP5_VAC_IMAGE_SIZE		0x00080002 // HP4K only
#define CAP5_VAC_FRAMERATE			0x00180003 // HP4K only

#define CAP5_VAC_INFOLED_MODE		0x00031000 // exclusively used only at BHM4000EX 


// QueryInfo
#define CAP5_QR_XLINECODE			0x1000	// reserved command.
#define CAP5_QR_GET_PROCESS_STATE	0x1001	// reserved command.
#define CAP5_QR_GET_HWINFO_STR		0x1002	// reserved command.
#define CAP5_QR_SET_SDRAM			0x1003	// reserved command.
#define CAP5_QR_GET_SDRAM			0x1004	// reserved command.
#define CAP5_QR_EXT_VIDEOOUT_MODE	0x1005
#define CAP5_QR_GET_EIOINFO			0x1006
#define CAP5_QR_GET_DACTYPE			0x1007   // 0:NONE,1:AD5301,2:AD5311.AD5321 DAC type in NCP Series.
#define CAP5_QR_GET_FRAMERATEINFO   0x1007 	 // HP4K

// QueryInfo: Attached RS485 Serial Port 
#define CAP5_QR_GET_RS485_COUNT 			0x1008
#define CAP5_QR_GET_RS485_PORTNUMBER 		0x1009
#define CAP5_QR_GET_RS485_DEVICENAME 		0x100A
#define CAP5_QR_GET_RS485_FRIENDLYNAME	0x100B

// 0x1100 ~ 0x11FF : board specific api
// HP4K 2009.06.09
#define CAP5_QR_HP4K_READ_FLASH		0x1100	
#define CAP5_QR_HP4K_WRITE_FLASH	0x1101	
#define CAP5_QR_HP4K_FORCED_CLOSE	0x1102	
#define CAP5_QR_AVAILABLE_EXT_VIDEOOUT_MODE	CAP5_QR_EXT_VIDEOOUT_MODE	// HP4K

#define CAP5_QR_GET_HWCODE			0x2001
#define	CAP5_QR_GET_HWCODE2			0x2002
#define	CAP5_QR_GET_VCACODE			0x2003
#define	CAP5_QR_GET_USN				0x2004
#define	CAP5_QR_GET_VCALICENSE		0x2005

// 0x3000 ~ 0x3FFF : CMN5 QueryInfoCmd
// {
// };

//AGC fucntion 
#define CAP5_SW_AGC_AUTO			0	// event driven
#define CAP5_SW_AGC_USER			1	// user fix
#define CAP5_SW_AGC_CONTINUOUS		2	// always calculate
#define CAP5_SW_AGC_DISABLE			3	// all channel fixed to default.


//External Video Out Mode
#define EVM_DISABLE					1	// disable external video out
#define EVM_SPLIT_OVERLAY			2	// split overlay mode
#define EVM_CASCADE_SPOT			4	// cascade spot mode
#define EVM_INDIV_SPOT				8	// individual spot mode


//Info LED Mode  // exclusively used only at BHM4000EX 
#define CAP5_ILM_DISABLE				1			// disable info led user mode
#define CAP5_ILM_ENABLE					2			// enable  info led user mode
#define CAP5_ILM_LED_ON					3			// led on 
#define CAP5_ILM_LED_OFF				4			// led off 
#define CAP5_ILM_LED_BLINK				5			// blink led, period range (40 ~ 5000 ms)


//Switch Level
#define CAP5_VPC_FAST_SWITCHING		0
#define CAP5_VPC_STABLE_SWITCHING	1
#define CAP5_VPC_AGC_SWITCHING		2

// renamed
#define CAP5_EVM_DISABLE			EVM_DISABLE			// disable external video out
#define CAP5_EVM_INDIV_MULTI_PIC	EVM_SPLIT_OVERLAY	// individual multi-picutre mode
#define CAP5_EVM_INDIV_SINGLE_PIC	EVM_INDIV_SPOT		// individual single-picture mode
#define CAP5_EVM_CASCADE_SINGLE_PIC	EVM_CASCADE_SPOT	// cascade single-picture mode


extern "C" {
//////////////////////////////////////////////////////////////////////////////////
// APIs
BOOL CMN5_API Cap5GetLastErrorCode(CMN5_ERROR_CODE_ITEM *pEcode);
BOOL CMN5_API Cap5QueryInfo(ULONG uQueryInfoCmd, ULONG uIn, void *pOut);

BOOL CMN5_API Cap5GetSystemInfo(CMN5_SYSTEM_INFO *pInfo);
BOOL CMN5_API Cap5GetBoardInfo(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, CAP5_BOARD_INFO *pInfo);

BOOL CMN5_API Cap5Activate(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]);

BOOL CMN5_API Cap5Setup(void);
BOOL CMN5_API Cap5Endup(void);
BOOL CMN5_API Cap5Run(void);
BOOL CMN5_API Cap5Stop(void);

BOOL CMN5_API Cap5VideoEnable(ULONG uBdID, ULONG uCh, BOOL bEnable);

BOOL CMN5_API Cap5SetCaptureMethod(ULONG uFuncType, ULONG uMethod);
BOOL CMN5_API Cap5SetCallback(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext);

BOOL CMN5_API Cap5QueryData(void *pData, ULONG uTimeOut);
BOOL CMN5_API Cap5ReleaseData(const void *pData);
BOOL CMN5_API Cap5SetEventHandle(ULONG uType, HANDLE hHandle);
BOOL CMN5_API Cap5GetEventHandle(ULONG uType, HANDLE *pHandle);
BOOL CMN5_API Cap5GetEventData(ULONG uType, void *pData);

BOOL CMN5_API Cap5SetVideoFormat(ULONG uBdID, ULONG uFormat);
BOOL CMN5_API Cap5GetVideoFormat(ULONG uBdID, ULONG *puFormat);
BOOL CMN5_API Cap5SetColorFormat(ULONG uBdID, ULONG uFormat);
BOOL CMN5_API Cap5GetColorFormat(ULONG uBdID, ULONG *puFormat);
BOOL CMN5_API Cap5SetImageSize(ULONG uBdID, ULONG uCh, ULONG uImageSize);
BOOL CMN5_API Cap5GetImageSize(ULONG uBdID, ULONG uCh, ULONG *puImageSize);

BOOL CMN5_API Cap5SetFrameRate(ULONG uBdID, ULONG uFrameRateCmd, const UCHAR *pFPS, ULONG uLength);

BOOL CMN5_API Cap5SetVideoProperty(ULONG uBdID, ULONG uCh, ULONG uVideoPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Cap5GetVideoProperty(ULONG uBdID, ULONG uCh, ULONG uVideoPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);
BOOL CMN5_API Cap5SetVideoAdjust(ULONG uBdID, ULONG uCh, ULONG uVideoAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Cap5GetVideoAdjust(ULONG uBdID, ULONG uCh, ULONG uVideoAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Cap5SetMultiVideoProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cap5GetMultiVideoProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cap5SetMultiVideoAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cap5GetMultiVideoAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

BOOL CMN5_API Cap5GetVideoStatus(ULONG uBdID, ULONG uMaxCh, ULONG *pbwData);

BOOL CMN5_API Cap5SetDO(ULONG uBdID, ULONG uMaxDO,const ULONG *pbwData);
BOOL CMN5_API Cap5GetDO(ULONG uBdID, ULONG uMaxDO, ULONG *pbwData);
BOOL CMN5_API Cap5GetDI(ULONG uBdID, ULONG uMaxDI, ULONG *pbwData);
BOOL CMN5_API Cap5SetExtVideoOut(ULONG uBdID, ULONG uCh);
BOOL CMN5_API Cap5GetExtVideoOut(ULONG uBdID, ULONG *puCh);
BOOL CMN5_API Cap5SetWatchdog(ULONG uBdID, ULONG uWatchdogCmd, ULONG uParam0, ULONG uParam1);

BOOL CMN5_API Cap5I2CTransfer(ULONG uBdID, ULONG uVPID, ULONG uDevID, ULONG uI2CCmd, ULONG uAddr, UCHAR *pData);
BOOL CMN5_API Cap5WriteUserEEPROM(ULONG uBdID, ULONG uVPID,const UCHAR *pData);
BOOL CMN5_API Cap5ReadUserEEPROM(ULONG uBdID, ULONG uVPID, UCHAR *pData);


// *********************************************************************** 
// Vendoer Specific IO API
// *********************************************************************** 
BOOL CMN5_API Cap5SetSpecialIOMode(ULONG uBdID, ULONG uData);
BOOL CMN5_API Cap5GetSpecialIOMode(ULONG uBdID, ULONG *puData);
BOOL CMN5_API Cap5WriteSpecialIOData(ULONG uBdID, ULONG uData);
BOOL CMN5_API Cap5ReadSpecialIOData(ULONG uBdID, ULONG *puData);

} // extern "C"

#if !defined(KERNEL_MODE_CODE)

// Interface definition for ICap5
interface ICap5 : IUnknown
{
	STDMETHOD_(BOOL, Cap5GetLastErrorCode)(CMN5_ERROR_CODE_ITEM *pEcode) PURE;
	STDMETHOD_(BOOL, Cap5QueryInfo)(ULONG uQueryInfoCmd, ULONG uIn, void *pOut) PURE;

	STDMETHOD_(BOOL, Cap5GetSystemInfo)(CMN5_SYSTEM_INFO *pInfo) PURE;
	STDMETHOD_(BOOL, Cap5GetBoardInfo)(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, CAP5_BOARD_INFO *pInfo) PURE;

	STDMETHOD_(BOOL, Cap5Activate)(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]) PURE;

	STDMETHOD_(BOOL, Cap5Setup)(void) PURE;
	STDMETHOD_(BOOL, Cap5Endup)(void) PURE;
	STDMETHOD_(BOOL, Cap5Run)(void) PURE;
	STDMETHOD_(BOOL, Cap5Stop)(void) PURE;

	STDMETHOD_(BOOL, Cap5VideoEnable)(ULONG uBdID, ULONG uCh, BOOL bEnable) PURE;

	STDMETHOD_(BOOL, Cap5SetCaptureMethod)(ULONG uFuncType, ULONG uMethod) PURE;
	STDMETHOD_(BOOL, Cap5SetCallback)(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext) PURE;

	STDMETHOD_(BOOL, Cap5QueryData)(void *pData, ULONG uTimeOut) PURE;
	STDMETHOD_(BOOL, Cap5ReleaseData)(const void *pData) PURE;
	STDMETHOD_(BOOL, Cap5SetEventHandle)(ULONG uType, HANDLE hHandle) PURE;
	STDMETHOD_(BOOL, Cap5GetEventHandle)(ULONG uType, HANDLE *pHandle) PURE;
	STDMETHOD_(BOOL, Cap5GetEventData)(ULONG uType, void *pData) PURE;

	STDMETHOD_(BOOL, Cap5SetVideoFormat)(ULONG uBdID, ULONG uFormat) PURE;
	STDMETHOD_(BOOL, Cap5GetVideoFormat)(ULONG uBdID, ULONG *puFormat) PURE;
	STDMETHOD_(BOOL, Cap5SetColorFormat)(ULONG uBdID, ULONG uFormat) PURE;
	STDMETHOD_(BOOL, Cap5GetColorFormat)(ULONG uBdID, ULONG *puFormat) PURE;
	STDMETHOD_(BOOL, Cap5SetImageSize)(ULONG uBdID, ULONG uCh, ULONG uImageSize) PURE;
	STDMETHOD_(BOOL, Cap5GetImageSize)(ULONG uBdID, ULONG uCh, ULONG *puImageSize) PURE;

	STDMETHOD_(BOOL, Cap5SetFrameRate)(ULONG uBdID, ULONG uFrameRateCmd, const UCHAR *pFPS, ULONG uLength) PURE;

	STDMETHOD_(BOOL, Cap5SetVideoProperty)(ULONG uBdID, ULONG uCh, ULONG uVideoPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Cap5GetVideoProperty)(ULONG uBdID, ULONG uCh, ULONG uVideoPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;
	STDMETHOD_(BOOL, Cap5SetVideoAdjust)(ULONG uBdID, ULONG uCh, ULONG uVideoAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Cap5GetVideoAdjust)(ULONG uBdID, ULONG uCh, ULONG uVideoAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;

	STDMETHOD_(BOOL, Cap5SetMultiVideoProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cap5GetMultiVideoProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cap5SetMultiVideoAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cap5GetMultiVideoAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;

	STDMETHOD_(BOOL, Cap5GetVideoStatus)(ULONG uBdID, ULONG uMaxCh, ULONG *pbwData) PURE;

	STDMETHOD_(BOOL, Cap5SetDO)(ULONG uBdID, ULONG uMaxDO,const ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cap5GetDO)(ULONG uBdID, ULONG uMaxDO, ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cap5GetDI)(ULONG uBdID, ULONG uMaxDI, ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cap5SetExtVideoOut)(ULONG uBdID, ULONG uCh) PURE;
	STDMETHOD_(BOOL, Cap5GetExtVideoOut)(ULONG uBdID, ULONG *puCh) PURE;
	STDMETHOD_(BOOL, Cap5SetWatchdog)(ULONG uBdID, ULONG uWatchdogCmd, ULONG uParam0, ULONG uParam1) PURE;

	STDMETHOD_(BOOL, Cap5I2CTransfer)(ULONG uBdID, ULONG uVPID, ULONG uDevID, ULONG uI2CCmd, ULONG uAddr, UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Cap5WriteUserEEPROM)(ULONG uBdID, ULONG uVPID,const UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Cap5ReadUserEEPROM)(ULONG uBdID, ULONG uVPID, UCHAR *pData) PURE;

	// *********************************************************************** 
	// Vendoer Specific IO API
	// *********************************************************************** 
	STDMETHOD_(BOOL, Cap5SetSpecialIOMode)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Cap5GetSpecialIOMode)(ULONG uBdID, ULONG *puData) PURE;
	STDMETHOD_(BOOL, Cap5WriteSpecialIOData)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Cap5ReadSpecialIOData)(ULONG uBdID, ULONG *puData) PURE;
};

#endif

#pragma pack( pop, Cap5BaordLibEx_H )

#endif // CAP4BOARDLIB_EX_H