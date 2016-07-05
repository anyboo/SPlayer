#ifndef CAP3BOARDLIB_H
#define CAP3BOARDLIB_H

#pragma pack( push, CAP3BOARDLIB_H )
#pragma pack(8)

// #if defined(STDCALL) all API is __stdcall
#	if defined(DVRDLL_EXPORTS)
#		define DVRCAP_API  extern "C"
#	else
#		define DVRCAP_API  extern "C" __declspec(dllimport) 
#	endif
#	define CALLING_API	__stdcall
// #endif

#if defined(KERNEL_MODE_CODE)
	typedef unsigned long DWORD;
#endif

#if !defined(UDA_ULONG_DEFINED)
#	if !defined(_WIN64)
		typedef ULONG		UDA_ULONG;
#	else
		typedef ULONG_PTR	UDA_ULONG;
#	endif
#	define UDA_ULONG_DEFINED
#endif
//______________________________________________________________________________________________
// Definition of System info V1

#define CAP_SYSTEM_MAXBOARD_V1		4
#define CAP_BOARD_MAXVP_V1			4
#define CAP_BOARD_MAXCHANNEL_V1		16
#define CAP_BOARD_MAXDIO_V1			16

#define CAP_MAXRESERVEDINFO			16

#define CAP_BOARD_MAX_DIO_ARRAY_SIZE	8
typedef struct {
	UCHAR	VPId;		// VP id
	UCHAR	reserved[CAP_MAXRESERVEDINFO - 1];
} CAP_VP_INFO_V1;		// 16*1 => 16bytes 

typedef struct {
	DWORD	BoardHWId;
	UCHAR	MaxVP;
	UCHAR	MaxChannel;
	UCHAR	MaxDO;
	UCHAR	MaxDI;
	UCHAR	TypeInfo;							// [0]=>1:noSwitching,0:switching model.
	UCHAR	HWRevision;
	UCHAR	reserved[CAP_MAXRESERVEDINFO - 10];
	CAP_VP_INFO_V1 VPInfo[CAP_BOARD_MAXVP_V1];
} CAP_BOARD_INFO_V1;	// 4*VP + 16*1 => 80bytes

typedef struct {
	DWORD	SystemId;		// ?
	DWORD	MaxBoard;
	DWORD	SysVersion;
	DWORD	DllVersion;
	DWORD	reserved[CAP_MAXRESERVEDINFO - 4];
	CAP_BOARD_INFO_V1 BoardInfo[CAP_SYSTEM_MAXBOARD_V1];
} CAP_SYSTEM_INFO_V1;	// 4*BD + 16*4 => 384bytes

//______________________________________________________________________________________________
// Definition of System info V2

#define CAP_SYSTEM_MAXBOARD_V2		4
#define CAP_BOARD_MAXVP_V2			16
#define CAP_BOARD_MAXCHANNEL_V2		16
#define CAP_BOARD_MAXDIO_V2			16

#define CAP_MAXRESERVED_VPINFO_V2	4

typedef struct {
	UCHAR	VPId;			// VP id
	UCHAR	reserved[CAP_MAXRESERVED_VPINFO_V2 - 1];
} CAP_VP_INFO_V2;	// 4*1 => 4bytes 

typedef struct {
	DWORD			BoardHWId;
	UCHAR			MaxVP;
	UCHAR			MaxChannel;
	UCHAR			MaxDO;
	UCHAR			MaxDI;
	UCHAR			TypeInfo;							// [0]=>1:noSwitching,0:switching model.
	UCHAR			HWRevision;
	UCHAR			reserved[CAP_MAXRESERVEDINFO - 10];
	CAP_VP_INFO_V2	VPInfo[CAP_BOARD_MAXVP_V2];
} CAP_BOARD_INFO_V2;	// 16*VP + 16*1 => 80bytes

typedef struct {
	DWORD			SystemId;		// ?
	DWORD			MaxBoard;
	DWORD			SysVersion;
	DWORD			DllVersion;
	DWORD			reserved[CAP_MAXRESERVEDINFO - 4];
	CAP_BOARD_INFO_V2 BoardInfo[CAP_SYSTEM_MAXBOARD_V2];
} CAP_SYSTEM_INFO_V2;	// 4*BD + 16*4 => 384bytes

//______________________________________________________________________________________________
// Definition of System info Descriptor

typedef struct {
	DWORD	InfoSize;
	DWORD	InfoName;
	DWORD	InfoVersion;
	DWORD	reserved;
} CAP_SYSTEM_INFO_DESC;

#if (USE_CAP_INFO_VERSION == 1)
	#define CAP_SYSTEM_MAXBOARD		CAP_SYSTEM_MAXBOARD_V1
	#define CAP_BOARD_MAXVP			CAP_BOARD_MAXVP_V1
	#define CAP_BOARD_MAXCHANNEL	CAP_BOARD_MAXCHANNEL_V1
	#define CAP_BOARD_MAXDIO		CAP_BOARD_MAXDIO_V1	
	#define CAP_VP_INFO				CAP_VP_INFO_V1
	#define CAP_BOARD_INFO			CAP_BOARD_INFO_V1
	#define CAP_SYSTEM_INFO			CAP_SYSTEM_INFO_V1
#else
	#define CAP_SYSTEM_INFO_NAME	0x5355455A	// 'SUEZ'
	#define CAP_SYSTEM_INFO_VERSION	2

	#define CAP_SYSTEM_MAXBOARD		CAP_SYSTEM_MAXBOARD_V2
	#define CAP_BOARD_MAXVP			CAP_BOARD_MAXVP_V2
	#define CAP_BOARD_MAXCHANNEL	CAP_BOARD_MAXCHANNEL_V2	
	#define CAP_BOARD_MAXDIO		CAP_BOARD_MAXDIO_V2	
	#define CAP_VP_INFO				CAP_VP_INFO_V2
	#define CAP_BOARD_INFO			CAP_BOARD_INFO_V2
	#define CAP_SYSTEM_INFO			CAP_SYSTEM_INFO_V2
#endif
// End of System info extension
//______________________________________________________________________________________________


#define CAP_INVALID_CHANNEL			0xff
#define CAP_ALL_BOARD				0xf	
#define CAP_SN_SIZE					16
// VideoFormat 설정할때 Bdid를 이값으로 사용하면 모든 보드가 동일한 값으로설정

//////////////////////////////////////////////////////////////////////////////////
// Capture Image Info
typedef struct {
	unsigned char	BoardNum;				// Board ID[0..3]
	unsigned char	ChannelNum;				// Channel ID of the Board
	unsigned char	ImageFormat;			
	unsigned char	ImageSize;
	unsigned char	CaptureProperty;
	unsigned char	HasNextImage;
	unsigned char*	pImageBuffer;			// image address
	LARGE_INTEGER	TimeTag;				// KeQuerySystemTime()
	unsigned short	VideoStatus[CAP_SYSTEM_MAXBOARD];		// Board[0..3]
	unsigned short	SensorStatus[CAP_SYSTEM_MAXBOARD];
	void*			pRISCnImage;			// internal use! do not touch!
} CAP_IMAGE_INFO;


#define MAX_ERRCMN_ERROR_CODE_MSG_LENGTH		128
#define MAX_ERRCMN_SESSION_LENGTH				64
struct ErrorCodeItem {
	DWORD			ErrorCode;									// code
	LARGE_INTEGER	TimeStamp;									// time-stamp
	DWORD			Reserved;									// reserved
	char			AuxMsg[MAX_ERRCMN_ERROR_CODE_MSG_LENGTH + 1];	// aux message
	char			SessionName[MAX_ERRCMN_SESSION_LENGTH + 1];		// session name
};


typedef struct {
	ULONG		uStructSize;

	ULONG		uEIOBoardCount;
	ULONG		uRev[CAP_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		uMaxDI[CAP_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		uMaxDO[CAP_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		uEIOMode[CAP_BOARD_MAX_DIO_ARRAY_SIZE];	// 0 : Legacy DIO, 1 : EIO
	ULONG		reserved[CAP_BOARD_MAX_DIO_ARRAY_SIZE*4];
} CAP_EIOBOARD_INFO;
//////////////////////////////////////////////////////////////////////////////////

enum ColorFormatValues {	
	CAP_COLOR_FORMAT_RGB24	=	0x11,					// RX Not implemented
	CAP_COLOR_FORMAT_RGB16	=	0x22,					// All Not implemented
	CAP_COLOR_FORMAT_RGB15	=	0x33,					// All Not implemented
	CAP_COLOR_FORMAT_YUY2	=	0x44,
	CAP_COLOR_FORMAT_Y8		=	0x66,					// RX Not implemented
	CAP_COLOR_FORMAT_YUV12	=	0x88,					// RX Not implemented
	CAP_COLOR_FORMAT_YUV9	=	0x99,					// RX Not implemented
	CAP_COLOR_FORMAT_YV12	=	0xAA,					// YVU12, Bt878 not support(made by RISC cmd)
};
#define CAP_DEFAULT_COLOR_FORMAT CAP_COLOR_FORMAT_YUY2

enum VideoForamtValues { 
	CAP_VIDEO_FORMAT_AUTO	=	0,						// Not implemented
	CAP_VIDEO_FORMAT_NTSC_M,
	CAP_VIDEO_FORMAT_NTSC_J,							// Not implemented
	CAP_VIDEO_FORMAT_PAL_B,
	CAP_VIDEO_FORMAT_PAL_M,								// Not implemented
	CAP_VIDEO_FORMAT_PAL_N,								// Not implemented
	CAP_VIDEO_FORMAT_SECAM,								// Not implemented
	CAP_VIDEO_FORMAT_PAL_NC								// Not implemented
};
#define CAP_DEFAULT_VIDEO_FORMAT CAP_VIDEO_FORMAT_NTSC_M

enum ImageSizeValues { 
	CAP_NTSC_SIZE_720X480	=	14,						
	CAP_NTSC_SIZE_720X240	=	15,
	CAP_NTSC_SIZE_360X240	=	16,
	CAP_NTSC_SIZE_180x120	=	26,

	CAP_NTSC_SIZE_704X480	=	17,						
	CAP_NTSC_SIZE_704X240	=	18,
	CAP_NTSC_SIZE_352X240	=	19,
	CAP_NTSC_SIZE_176x120	=	27,

	CAP_NTSC_SIZE_640X480	=	0,						
	CAP_NTSC_SIZE_640X240	=	1,
	CAP_NTSC_SIZE_320X240	=	2,
	CAP_NTSC_SIZE_240X160	=	3,		// Not implemented
	CAP_NTSC_SIZE_160X120	=	4,		// Not implemented

	CAP_PAL_SIZE_768X576	=	10,		// RX3 Not Support
	CAP_PAL_SIZE_768X288	=	12,		// RX3 Not Support
	CAP_PAL_SIZE_384X288	=	11,		// RX3 Not Support
	CAP_PAL_SIZE_192x144	=	28, 

	CAP_PAL_SIZE_720X576	=	5,								
	CAP_PAL_SIZE_720X288	=	13,
	CAP_PAL_SIZE_360X288	=	7,
	CAP_PAL_SIZE_180x144	=	29,

	CAP_PAL_SIZE_704X576	=	20,								
	CAP_PAL_SIZE_704X288	=	21,
	CAP_PAL_SIZE_352X288	=	22,		
	CAP_PAL_SIZE_176x144	=	30, 

	CAP_PAL_SIZE_672X544	=	23,								
	CAP_PAL_SIZE_672X272	=	24,
	CAP_PAL_SIZE_336X272	=	25,		// last
	CAP_PAL_SIZE_168X136	=	31,		// last
		
	CAP_PAL_SIZE_640X480	=	9,		// RX3 Not Support
	CAP_PAL_SIZE_640X240	=	6,		// RX3 Not Support
	CAP_PAL_SIZE_320X240	=	8,		// RX3 Not Support
	CAP_PAL_SIZE_160x120	=	32,
	
	CAP_MAX_SUPPORT_IMAGESIZE =	33,
};
#define CAP_NTSC_SIZE_640X240_DOUBLE	(CAP_NTSC_SIZE_640X240 | 0x80)
#define	CAP_PAL_SIZE_640X240_DOUBLE		(CAP_PAL_SIZE_640X240 | 0x80)
#define	CAP_PAL_SIZE_768X288_DOUBLE		(CAP_PAL_SIZE_768X288 | 0x80)
#define	CAP_PAL_SIZE_720X288_DOUBLE		(CAP_PAL_SIZE_720X288 | 0x80)

#define CAP_DEFAULT_IMAGE_SIZE CAP_NTSC_SIZE_320X240

//////////////////////////////////////////////////////////////////////////////////
// capture property
enum {
	CAP_CAPTURE_PORPERTY_VERTICAL_FLOP		= 0x01,		
		// 영상이 뒤집혀서 저장된다.(RGB모드에서유용)
	CAP_CAPTURE_PORPERTY_HDOUBLE			= 0x02,		// Not implemented
		// 가로방향으로 영상이 두배 커진다. (320x240) =>(640x240)으로 사용가능.
		// DLL에서 변환된다.
	CAP_CAPTURE_PORPERTY_VDOUBLE			= 0x04,		// Not implemented
		// 세로방향으로 영상이 두배 커진다. (320x240) =>(320x480)으로 사용가능.
		// DLL에서 변환된다.
	CAP_CAPTURE_PORPERTY_SEPARATED_FIELD	= 0x08,		// Not implemented
		// 프레임캡쳐인경우, 필드가 분리되어 캡쳐된다.
	CAP_CAPTURE_PORPERTY_OVERLAY			= 0x10,		// Not implemented
		// 오버레이 영상을 만든다.
};

//////////////////////////////////////////////////////////////////////////////////
// Capture method
enum {
	// method
	CAPTURE_METHOD_CALLBACK				=0,
	CAPTURE_METHOD_LOCK_AND_WAIT		=1,
	CAPTURE_METHOD_QUERY				=2,

	// call-back type
	CAPTURE_CALLBACK_ONCAPTURE			=0x10,
	CAPTURE_CALLBACK_ONVIDEO			=0x11,
	CAPTURE_CALLBACK_ONSENSOR			=0x12
};

//////////////////////////////////////////////////////////////////////////////////
// Sequence Method
enum {
	CAP_AUTO_SEQUENCE	=0,
	CAP_FIXED_SEQUENCE	=1,
	CAP_USER_SEQUENCE	=2
};
#define CAP_MAXSEQUENCE_CHANNEL		49

//////////////////////////////////////////////////////////////////////////////////
// RS485

/* baud-rate	*/
enum {
	BAUD_RATE_19200BPS			=0x10,
	BAUD_RATE_9600BPS			=0x11,
	BAUD_RATE_4800BPS			=0x12,
	BAUD_RATE_2400BPS			=0x13,
	BAUD_RATE_1200BPS			=0x14
};

/* serial port mode	*/
#define DATA_BIT_SIZE_8			0x11	/* 8-bit UART baud-rate variable	*/
#define DATA_BIT_SIZE_9			0x12	/* 9-bit UART baud-rate variable	*/

/* serial port mode	*/
#define ZERO_PARITY				0x00
#define STICK_PARITY			0x01

//////////////////////////////////////////////////////////////////////////////////
// I2C & EEPROM

enum {
	CAP_I2C_READ	= 1,
	CAP_I2C_WRITE,
	CAP_I2C_RANDOM_READ,
	CAP_I2C_RANDOM_WRITE,
	CAP_I2C_RANDOM_WRITE_NOWAIT
};

typedef struct {
	DWORD Cmd;
	DWORD BdID;
	UCHAR DevID;
	UCHAR Addr;
	UCHAR Data;
} CAP_I2C_CMD;

#define CAP_USER_EEPROM_START_ADDR	0x00
#define CAP_USER_EEPROM_SIZE		16
#define CAP_UID_ADDR				0x10000

#define CAP_MAX_SPEED_SEQUENCE		0
#define CAP_ACCURATE_SEQUENCE		1
#define CAP_D1_ACCURATE_SEQUENCE	2
#define CAP_D1_ACCURATE_SEQUENCE2	3


// SW AGC
#define CAP_SW_AGC_AUTO			0	// event driven
#define CAP_SW_AGC_USER			1	// user fix
#define CAP_SW_AGC_CONTINUOUS	2	// always calculate
#define CAP_SW_AGC_DISABLE		3	// all channel fixed to default.


// Get Frame Rate Flag 
// Array type
#define CAP_FRAMERATE_RETURN_TYPE_0		0x01	//x100

#define CAP_FRAMERATE_AVR				0x0100	
#define CAP_FRAMERATE_MIN				0x0200
#define CAP_FRAMERATE_MAX				0x0400



// cap adjust command
#define	VAC_EIO_ENABLE			0x00030010 // SetCommand	; N/A
											   // GetCommand	; Not used

#define	VAC_EIO_DO				0x00030020 // S(G)etCommand ; Param0 : DO bitmask

#define	VAC_EIO_DI				0x00030040 // SetCommand    ; Not used
											   // GetCommand    ; Param0 : DI bitmask

#define	VAC_EIO_DITYPE			0x00030080 // SetCommand    ; Param0 : 0 ; DITYPE VOLTAGE
											   //						   1 ; DITYPE RELAY
											   // GetCommand    ; Not used


#define MAKEEIOBOARDID(bdid, eiobdid)		((bdid & 0xFF) | ((eiobdid & 0xFF) << 16))
#define GETBOARDID(bdid)					(bdid & 0xFF)
#define GETEIOBOARDID(bdid)				((bdid >> 16) & 0xFF)


// QueryInfo
#define QR_XLINECODE			0x1000
#define QR_GET_PROCESS_STATE	0x1001
#define QR_GET_HWINFO_STR		0x1002	// p0(in) #bd, p1(out) str ptr
#define QR_EXT_VIDEOOUT_MODE	0x1005
#define QR_GET_EIOINFO			0x1006	// refer to CAP5_EIOBOARD_INFO
#define QR_GET_DACTYPE			0x1007   // 0:NONE,1:AD5301,2:AD5311.AD5321 DAC type in NCP Series.
// *********************************************************************** 
// Callback Prototype
// *********************************************************************** 
typedef BOOL (CALLING_API *FUNC_ON_THREEDWORD)(UDA_ULONG A, UDA_ULONG B, UDA_ULONG C);
typedef BOOL (CALLING_API *FUNC_ON_TWODWORD)(UDA_ULONG A, UDA_ULONG B);
typedef BOOL (CALLING_API *FUNC_ON_ONEDWORD)(UDA_ULONG A);
// Cap3SetFuncOnCapture
// Cap3SetFuncOnSensor
// Cap3SetFuncOnVideoPresent
//onCapture		(DWORD mixedCh, DWORD pImageAddr);	
	// bdid = mixedCh>>4, ch = mixedCh&0xF;
//onCaputre		(DWORD ImageInfo);					
	// pImgeInfo = (CAP_IMAGE_INFO*) ImageInfo;	// needs static pointer conversoin
//onVideoLoss	(DWORD mixedCh, DWORD bPresent);
//onSensor		(DWORD bdid, DWORD bwSensor);

// Cap3SetOnCallBacks:
// OnCapture(DWORD A, DWORD B)
	// CAP_IMAGE_INFO* pImageInfo = (CAP_IMAGE_INFO*) A;
	// MyContext *pContext = (MyContext *) B;
// OnVideo(DWORD A, DWORD B)
	// bdId = (A>>24)&0xF;  bdCh = (A>>16)&0xF; bPresent = A&1;
	// MyContext *pContext = (MyContext *) B;
// OnSensor(DWORD A, DWORD B)
	// bdId = (A>>24)&0xF;  bwState = A&0xFFFF;
	// MyContext *pContext = (MyContext *) B;

// *********************************************************************** 
// Process Control API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3Control(DWORD cmd, void* inD, DWORD inSize, void* outD, DWORD outSize, DWORD* rsBytes);
DVRCAP_API BOOL CALLING_API Cap3CheckSN(DWORD bdid, UCHAR* sN);
DVRCAP_API BOOL	CALLING_API	Cap3Setup();
DVRCAP_API BOOL CALLING_API	Cap3Endup(void);
DVRCAP_API BOOL CALLING_API	Cap3Run(void);
DVRCAP_API BOOL CALLING_API	Cap3Stop(void);

// *********************************************************************** 
// Call-Back API   		                                              }
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API	Cap3SetFuncOnCapture(FUNC_ON_TWODWORD func);
DVRCAP_API BOOL CALLING_API	Cap3SetFuncOnCaptureEx(FUNC_ON_ONEDWORD func);
DVRCAP_API BOOL CALLING_API	Cap3SetFuncOnSensor(FUNC_ON_TWODWORD func);
DVRCAP_API BOOL CALLING_API	Cap3SetFuncOnVideoPresent(FUNC_ON_TWODWORD func);
DVRCAP_API BOOL CALLING_API	Cap3SetOnCallBacks(DWORD funcType, FUNC_ON_ONEDWORD fStart, FUNC_ON_TWODWORD fCall, FUNC_ON_ONEDWORD fStop, void *pContext);

// *********************************************************************** 
// Capture Method API   		                                              }
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API	Cap3SetCaptureMethod(DWORD method);
DVRCAP_API BOOL CALLING_API Cap3QueryWaitImage(CAP_IMAGE_INFO *pInfo);
DVRCAP_API BOOL CALLING_API Cap3ReleaseImage(CAP_IMAGE_INFO *pInfo);

// *********************************************************************** 
// Video Format & Color Setting
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API	Cap3SetVideoFormat(DWORD bdid, DWORD format);
DVRCAP_API BOOL CALLING_API	Cap3SetColorFormat(DWORD bdid, DWORD format);
DVRCAP_API BOOL CALLING_API Cap3SetCaptureProperty(DWORD bdid, DWORD property);
DVRCAP_API BOOL CALLING_API	Cap3SetImageSize(DWORD bdid, DWORD channel, DWORD imagesize);

DVRCAP_API BOOL CALLING_API	Cap3SetBrightness(DWORD bdid, DWORD channel, DWORD brightness);
DVRCAP_API BOOL CALLING_API	Cap3SetContrast(DWORD bdid, DWORD channel, DWORD contrast);
DVRCAP_API BOOL CALLING_API	Cap3SetHue(DWORD bdid, DWORD channel, DWORD hue);
DVRCAP_API BOOL CALLING_API	Cap3SetSaturationU(DWORD bdid, DWORD channel, DWORD contrast);
DVRCAP_API BOOL	CALLING_API	Cap3SetSaturationV(DWORD bdid, DWORD channel, DWORD contrast);

DVRCAP_API BOOL CALLING_API	Cap3GetVideoFormat(DWORD bdid, DWORD *pFormat);
DVRCAP_API BOOL CALLING_API	Cap3GetColorFormat(DWORD bdid, DWORD *pFormat);
DVRCAP_API BOOL CALLING_API	Cap3GetImageSize(DWORD bdid, DWORD channel, DWORD *pD);

DVRCAP_API BOOL CALLING_API	Cap3GetBrightness(DWORD bdid, DWORD channel, DWORD *pD);
DVRCAP_API BOOL CALLING_API	Cap3GetContrast(DWORD bdid, DWORD channel, DWORD *pD);
DVRCAP_API BOOL CALLING_API	Cap3GetHue(DWORD bdid, DWORD channel, DWORD *pD);
DVRCAP_API BOOL CALLING_API	Cap3GetSaturationU(DWORD bdid, DWORD channel, DWORD *pD);
DVRCAP_API BOOL CALLING_API	Cap3GetSaturationV(DWORD bdid, DWORD channel, DWORD *pD);


DVRCAP_API BOOL	CALLING_API	Cap3SetAdjust(DWORD bdid, DWORD channel, DWORD cmd, UDA_ULONG param0, UDA_ULONG param1, UDA_ULONG param2, UDA_ULONG param3);
DVRCAP_API BOOL	CALLING_API	Cap3GetAdjust(DWORD bdid, DWORD channel, DWORD cmd, UDA_ULONG *param0, UDA_ULONG *param1, UDA_ULONG *param2, UDA_ULONG *param3);
// *********************************************************************** 
// DIO & Video Status API
// *********************************************************************** 
DVRCAP_API BOOL	CALLING_API	Cap3SetCaptureEnable(DWORD bdid, DWORD channel, DWORD v);
DVRCAP_API BOOL CALLING_API	Cap3GetVideoStatus(DWORD bdid, DWORD* list);
DVRCAP_API BOOL CALLING_API	Cap3SetDO(DWORD bdid, DWORD v);
DVRCAP_API BOOL CALLING_API Cap3GetDO(DWORD bdid, DWORD *pD);
DVRCAP_API BOOL CALLING_API Cap3GetDI(DWORD bdid, DWORD *pD);
DVRCAP_API BOOL CALLING_API Cap3SetExtVideoOut(DWORD bdid, DWORD ch);
DVRCAP_API BOOL CALLING_API Cap3GetExtVideoOut(DWORD bdid, DWORD *pD);

// *********************************************************************** 
// I2C & UserEEprom API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3I2CTransfer(DWORD bdid, DWORD btid, DWORD devid, DWORD cmd, DWORD addr, UCHAR *pData);
DVRCAP_API BOOL CALLING_API Cap3WriteUserEEPROM(DWORD bdid, DWORD btid, UCHAR *data);
DVRCAP_API BOOL CALLING_API Cap3ReadUserEEPROM(DWORD bdid, DWORD btid, UCHAR *data);

// *********************************************************************** 
// RS485 API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3SetRS485BaudRate(DWORD bdid, DWORD data);
DVRCAP_API BOOL CALLING_API Cap3SetRS485DataBitSize(DWORD bdid, DWORD data);
DVRCAP_API BOOL CALLING_API Cap3SetRS485Bit8(DWORD bdid, DWORD data);
DVRCAP_API BOOL CALLING_API Cap3SetRS485TxData(DWORD bdid, UCHAR* pD, DWORD n);
DVRCAP_API BOOL CALLING_API Cap3GetRS485RxData(DWORD bdid, UCHAR *data,DWORD maxrdSize, DWORD *rdSize);

// *********************************************************************** 
// Watchdog API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3WatchdogEnable(DWORD on);
DVRCAP_API BOOL CALLING_API Cap3SetWatchdogTimeout(DWORD second);
DVRCAP_API BOOL CALLING_API Cap3SetBuzzerTimeout(DWORD uBdID,DWORD uSec);
DVRCAP_API BOOL CALLING_API Cap3WatchdogEnableEx(DWORD uBdID, DWORD on);
DVRCAP_API BOOL CALLING_API Cap3SetWatchdogTimeoutEx(DWORD uBdID, DWORD second);

// *********************************************************************** 
// UserSequence API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3SetSequenceMethod(DWORD bdid, DWORD m);
DVRCAP_API BOOL CALLING_API Cap3SetUserSequenceList(DWORD bdid, DWORD btid, UCHAR* seqList);
DVRCAP_API BOOL CALLING_API Cap3AddUserSequenceList(DWORD bdid, DWORD btid, UCHAR* seqList);
DVRCAP_API BOOL CALLING_API Cap3GetUserSequenceList(DWORD bdid, DWORD btid, UCHAR* seqList);

// *********************************************************************** 
// Driver Information API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3GetSystemInfo(CAP_SYSTEM_INFO_V1 *pInfo);
DVRCAP_API BOOL CALLING_API Cap3GetSystemInfoEx(CAP_SYSTEM_INFO_DESC *pDesc, void *pInfo);
DVRCAP_API BOOL CALLING_API Cap3GetLastErrorCode(ErrorCodeItem *pEcode);
DVRCAP_API BOOL CALLING_API Cap3QueryInfo(DWORD cmd, UDA_ULONG *Param0, UDA_ULONG *Param1, UDA_ULONG *Param2, UDA_ULONG *Param3);

DVRCAP_API BOOL CALLING_API Cap3BtRegWrite(DWORD bdid, DWORD btid, DWORD addr, DWORD data);
DVRCAP_API BOOL CALLING_API Cap3BtRegRead(DWORD bdid, DWORD btid, DWORD addr, DWORD *pData);

//DVRCAP_API BOOL CALLING_API Cap3CheckFramrateConstancy(DWORD nTolerance, DWORD maxFrame, DWORD nGroup, UCHAR *pFPS);
//DVRCAP_API BOOL CALLING_API Cap3MakeFrameListGroup(DWORD nTolerance, DWORD maxFPS, DWORD nGroup, UCHAR *pFPS, UCHAR (*paFPSGroup)[CAP_BOARD_MAXCHANNEL]);
//DVRCAP_API BOOL CALLING_API Cap3CheckChannelConstancy(DWORD nTolerance, DWORD maxFrame, DWORD nGroup, UCHAR (*paSeq)[CAP_BOARD_MAXCHANNEL]);
//DVRCAP_API BOOL CALLING_API Cap3MakeUserSequenceList(DWORD bAccurate, DWORD maxFrame, UCHAR *pFPS, UCHAR *pSeqList);
DVRCAP_API BOOL CALLING_API Cap3SetBoardFrameRate(DWORD bdid, DWORD bAccurate, DWORD maxTolerance, DWORD maxFrame, DWORD nGroup, UCHAR *pFPS);
DVRCAP_API BOOL CALLING_API Cap3GetExpectedFrameRate(DWORD bdid, DWORD flag, DWORD MaxCh, DWORD *pFPS);
// *********************************************************************** 
// SW AGC API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3SetSWAGCMode(DWORD bdid, DWORD ch, DWORD mode);
DVRCAP_API BOOL CALLING_API Cap3GetSWAGCMode(DWORD bdid, DWORD ch, DWORD *pData);
DVRCAP_API BOOL CALLING_API Cap3SetSWAGCLevel(DWORD bdid, DWORD ch, DWORD data);
DVRCAP_API BOOL CALLING_API Cap3GetSWAGCLevel(DWORD bdid, DWORD ch, DWORD *pData);

// *********************************************************************** 
// Vendoer Specific IO API
// *********************************************************************** 
DVRCAP_API BOOL CALLING_API Cap3SetSpecialIOMode(DWORD bdid,DWORD data);
DVRCAP_API BOOL CALLING_API Cap3GetSpecialIOMode(DWORD bdid,DWORD *pData);
DVRCAP_API BOOL CALLING_API Cap3WriteSpecialIOData(DWORD bdid,DWORD data);
DVRCAP_API BOOL CALLING_API Cap3ReadSpecialIOData(DWORD bdid,DWORD *pData);

#pragma pack( pop, CAP3BOARDLIB_H )
#endif