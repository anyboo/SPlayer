#ifndef CMN5BOARDLIB_EX_H
#define CMN5BOARDLIB_EX_H

#pragma pack( push, Cmn5BoardLib_EX_H )
#pragma pack(8)

#	if defined(DVRDLL_EXPORTS)
#		define CMN5_API  __stdcall
#	else
#		define CMN5_API  __declspec(dllimport) __stdcall
#	endif

#if defined(KERNEL_MODE_CODE)
	typedef unsigned long DWORD;
	typedef int BOOL;
#endif

#if !defined(UDA_ULONG_DEFINED)
#	if !defined(_WIN64)
		typedef ULONG		UDA_ULONG;
#	else
		typedef ULONG_PTR	UDA_ULONG;
#	endif
#	define UDA_ULONG_DEFINED
#endif

#define CMN5_SYSTEM_MAX_BOARD				8
#define CMN5_BOARD_MAX_VP					256
#define CMN5_BOARD_MAX_CHANNEL				256
#define CMN5_BOARD_MAX_DIO					256
#define CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE	8
#define CMN5_BOARD_MAX_DIO_ARRAY_SIZE		8

#define CMN5_INVALID_CHANNEL				0xFFFFFFFF		//jb
#define CMN5_ALL_BOARD						0xFF			//jb

// Media type  //jb
#define	CMN5_MEDIA_RAW_VIDEO				0x01	
#define	CMN5_MEDIA_RAW_AUDIO				0x02
#define	CMN5_MEDIA_OVERLAY					0x04
#define	CMN5_MEDIA_CODEC					0x08

//////////////////////////////////////////////////////////////////////////
// System Infomation
#define CMN5_MAX_RESERVED_SYSTEM_INFO		8
#define CMN5_MAX_RESERVED_BOARD_INFO_DESC	4
#define CMN5_MAX_RESERVED_BOARD_INFO		16

#define CMN5_BOARD_INFO_VERSION				1

#if _MSC_VER >= 1300
typedef union _LARGE_INTEGER_CAP {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER_CAP;
#endif

typedef struct _CMN5_SYSTEM_INFO {
	ULONG	uNumOfBoard;
	ULONG	uDllVersion;
	ULONG	uSysVersion;
	ULONG	reserved[CMN5_MAX_RESERVED_SYSTEM_INFO - 3];
} CMN5_SYSTEM_INFO;

typedef struct _CMN5_BOARD_INFO_DESC {
	ULONG	uInfoSize;
	ULONG	uInfoVersion;
	ULONG	reserved[CMN5_MAX_RESERVED_BOARD_INFO_DESC - 2];
} CMN5_BOARD_INFO_DESC;


//////////////////////////////////////////////////////////////////////////
// Error Code
#define CMN5_MAX_ERROR_CODE_MSG_LENGTH		128		//jb
#define CMN5_MAX_SESSION_LENGTH				64		//jb
typedef struct _CMN5_ERROR_CODE_ITEM {
	ULONG			ErrorCode;										// code
#if _MSC_VER >= 1300
	LARGE_INTEGER_CAP	TimeStamp;									// time-stamp
#else
	LARGE_INTEGER	TimeStamp;										// time-stamp
#endif
	ULONG			Reserved;										// reserved
	char			AuxMsg[CMN5_MAX_ERROR_CODE_MSG_LENGTH];	// aux message
	char			SessionName[CMN5_MAX_SESSION_LENGTH];	// session name
} CMN5_ERROR_CODE_ITEM;


//////////////////////////////////////////////////////////////////////////
// Video Format
#define CMN5_VIDEO_FORMAT_NONE		0		//jb
#define CMN5_VIDEO_FORMAT_NTSC_M	1
#define CMN5_VIDEO_FORMAT_PAL_B		2

#define CMN5_DEFAULT_VIDEO_FORMAT CMN5_VIDEO_FORMAT_NTSC_M


//////////////////////////////////////////////////////////////////////////
// Image Size
/* ImageSize define

	DWORD I;
	I[31..28] : reserved....
	I[27	] : Interace Frame(1)/weave Frame(0)
	I[26	] : W double
	I[25	] : H double
	I[24	] : VFLIP
	I[23..12] : H
	I[11.. 0] : W

  ex) 640x480
	0000 xxxxx 640 480
	I = (480<<12) | (640);
*/
union CMN5_IMAGESIZE {
	ULONG uAll;
	struct {
		ULONG nWidth			: 12;	//[0..11]
		ULONG nHeight			: 12;	//[12..23]
		ULONG bInvert			: 1;	//[24]
		ULONG bHDouble			: 1;	//[25]
		ULONG bVDouble			: 1;	//[26]
		ULONG bInteracedFrame	: 1;	//[27]
		ULONG nScaledDown		: 2;	//[29..28]
		ULONG reserved			: 2;	//[30..31]
	};
};

#define CMN5_MAKEIMGSIZE(w, h)		((w & 0xFFF) | ((h &0xFFF) << 12))
#define CMN5_GETIMGWIDTH(is)		(is & 0xFFF)
#define CMN5_GETIMGHEIGHT(is)		((is >> 12) & 0xFFF)
#define CMN5_IMAGESIZE_RESMASK		(0x00FFFFFF)
#define CMN5_IMAGESIZE_AUXMASK		(0xFF000000)
#define CMN5_IMAGESIZE_WDOUBLE		(1<<26)
#define CMN5_IMAGESIZE_HDOUBLE		(1<<25)
#define CMN5_IMAGESIZE_VFLIP		(1<<24)
#define CMN5_IMAGESIZE_SEP_FIELD	(1<<27)
#define CMN5_IMAGESIZE_SCALED_DOWN_CIF		(1<<28)
#define CMN5_IMAGESIZE_SCALED_DOWN_QCIF		(2<<28)

#define CMN5_IMAGESIZE_768X576	CMN5_MAKEIMGSIZE(768, 576)
#define CMN5_IMAGESIZE_768X288	CMN5_MAKEIMGSIZE(768, 288)
#define CMN5_IMAGESIZE_384X288	CMN5_MAKEIMGSIZE(384, 288)
#define CMN5_IMAGESIZE_192X144	CMN5_MAKEIMGSIZE(192, 144)

#define CMN5_IMAGESIZE_720X576	CMN5_MAKEIMGSIZE(720, 576)
#define CMN5_IMAGESIZE_720X288	CMN5_MAKEIMGSIZE(720, 288)
#define CMN5_IMAGESIZE_360X288	CMN5_MAKEIMGSIZE(360, 288)
#define CMN5_IMAGESIZE_180X144	CMN5_MAKEIMGSIZE(180, 144)

#define CMN5_IMAGESIZE_720X480	CMN5_MAKEIMGSIZE(720, 480)
#define CMN5_IMAGESIZE_720X240	CMN5_MAKEIMGSIZE(720, 240)
#define CMN5_IMAGESIZE_360X240	CMN5_MAKEIMGSIZE(360, 240)
#define CMN5_IMAGESIZE_180X120	CMN5_MAKEIMGSIZE(180, 120)

#define CMN5_IMAGESIZE_640X480	CMN5_MAKEIMGSIZE(640, 480)
#define CMN5_IMAGESIZE_640X240	CMN5_MAKEIMGSIZE(640, 240)
#define CMN5_IMAGESIZE_320X240	CMN5_MAKEIMGSIZE(320, 240)
#define CMN5_IMAGESIZE_160X120	CMN5_MAKEIMGSIZE(160, 120)

#define CMN5_IMAGESIZE_704X576	CMN5_MAKEIMGSIZE(704, 576)
#define CMN5_IMAGESIZE_704X288	CMN5_MAKEIMGSIZE(704, 288)
#define CMN5_IMAGESIZE_352X288	CMN5_MAKEIMGSIZE(352, 288)
#define CMN5_IMAGESIZE_176X144	CMN5_MAKEIMGSIZE(176, 144)

#define CMN5_IMAGESIZE_704X480	CMN5_MAKEIMGSIZE(704, 480)
#define CMN5_IMAGESIZE_704X240	CMN5_MAKEIMGSIZE(704, 240)
#define CMN5_IMAGESIZE_352X240	CMN5_MAKEIMGSIZE(352, 240)
#define CMN5_IMAGESIZE_176X120	CMN5_MAKEIMGSIZE(176, 120)

#define CMN5_IMAGESIZE_672X544	CMN5_MAKEIMGSIZE(672, 544)
#define CMN5_IMAGESIZE_672X272	CMN5_MAKEIMGSIZE(672, 272)
#define CMN5_IMAGESIZE_336X272	CMN5_MAKEIMGSIZE(336, 272)
#define CMN5_IMAGESIZE_168X136	CMN5_MAKEIMGSIZE(168, 136)



#define CMN5_MAX_RESOLUTION_COUNT	(1<<6)	//6bit... 64...
#define CMN5_MAX_RESOLUTION_VALUE	(CMN5_MAX_RESOLUTION_COUNT-1)
/* Image Size Description
	ULONG ISD
	
	ISD[23..16]		Supported Video Format
	ISD[15..8]		Subimage,	Each bit represent.
	ISD[7..0 ]		Current Image, Only one bit is available.
*/
	
/* Image Size Description
	ULONG ISD
	
	ISD[9..8 ]		Group /sub/sep id...
	ISD[7..0 ]		Supported Video Format
*/

#define CMN5_IMAGESIZE_IS_GROUP	0x000
#define CMN5_IMAGESIZE_IS_SUB	0x100
#define CMN5_IMAGESIZE_IS_SEP	0x200


#define CMN5_IMAGESIZE_SUPPORT_NTSC_M	(0x01<<CMN5_VIDEO_FORMAT_NTSC_M)
#define CMN5_IMAGESIZE_SUPPORT_PAL_B	(0x01<<CMN5_VIDEO_FORMAT_PAL_B)
#define CMN5_IMAGESIZE_SUPPORT_BOTH		(CMN5_IMAGESIZE_SUPPORT_NTSC_M | CMN5_IMAGESIZE_SUPPORT_PAL_B)

typedef struct _CMN5_IMAGESIZE_DESC{
	ULONG uImageSize;
	ULONG uSizeDesc;
} CMN5_IMAGESIZE_DESC;

typedef struct _CMN5_RESOLUTION_INFO{
	BOOL	fSameGroupOnly;
	ULONG	uSizeCount;
	ULONG	uDefaultNTSCSize;
	ULONG	uDefaultPALSize;
	CMN5_IMAGESIZE_DESC aImgDesc[CMN5_MAX_RESOLUTION_COUNT];
} CMN5_RESOLUTION_INFO;

//////////////////////////////////////////////////////////////////////////////////
// 
#define CMN5_ACTIVATION_CODE_SIZE					16

//////////////////////////////////////////////////////////////////////////////////
// I2C & EEPROM
#define	CMN5_I2C_CMD_READ				1
#define	CMN5_I2C_CMD_WRITE				2
#define	CMN5_I2C_CMD_RANDOM_READ		3
#define	CMN5_I2C_CMD_RANDOM_WRITE		4
#define	CMN5_I2C_CMD_RANDOM_WRITE_WAIT	5

#define	CMN5_I2C_CMD_SETSCL				10		// HP4K. added by jbyoon 2009.04.15 
#define	CMN5_I2C_CMD_SETSDA				11		// HP4K.
#define	CMN5_I2C_CMD_GETSDA				12		// HP4K.
#define	CMN5_I2C_CMD_WRITE2				13		// HP4K. two bytes write (slave,sub)

#define	CMN5_I2C_CMD_SIC_SENDCOMMAND	32		// HP4K. 
#define	CMN5_I2C_CMD_SIC_ACKPOLLING		33		// HP4K. 
#define CMN5_USER_EEPROM_SIZE		16

#define CMN5_UID_ADDR				0x10000

//////////////////////////////////////////////////////////////////////////////////
// process state
//enum Cmn5ProcessState 
#define	CMN5_PS_INIT	1
#define	CMN5_PS_SETUP	2
#define	CMN5_PS_RUN		3

//enum WatchdogCmd {
#define	CMN5_WC_ENABLE						1
#define	CMN5_WC_DISABLE						2
#define	CMN5_WC_SET_TIMEOUT_VALUE			3
#define	CMN5_WC_TRIGGER						4
#define	CMN5_WC_SET_BUZZER_TIMEOUT_VALUE	5
//};

//enum QueryInfoCmd {

#define CMN5_QIC_GET_PROCESS_STATE	 0x1
#define CMN5_QIC_GET_HWINFO_STR		 0x1002
#define CMN5_QIC_GET_EIOINFO		 0x1006 //0x2 for compatibility old define CAP5_QIC_GET_EIOINFO

#define CMN5_QIC_GET_EBOXTYPE		 	 0x1007 // whether External PCIe Box type or not
#define CMN5_QIC_GET_EBOXFANCOUNT    	 0x1100
#define CMN5_QIC_GET_EBOXPOWERSTATUS 	 0x1101
#define CMN5_QIC_GET_EBOXTEMP_EXTERNAL	 0x1102
#define CMN5_QIC_GET_EBOXTEMP_INTERNAL 	 0x1103

// 0x3000 ~ 0x3FFF : CMN5 common command
#define CMN5_QIC_CMD_BASE_START			0x3000		// for info

// 0x3F00 ~ 0x3FFF : CMN5 board specfic command
#define CMN5_QIC_SET_EXT_VIDEOOUT_BASEMODE 0x3F01	// HP4K only. CMN5_EVM_INDIV_MULTI_PIC or CMN5_EVM_INDIV_SINGLE_PIC
#define CMN5_QIC_GET_EXT_VIDEOOUT_BASEMODE 0x3F02	// HP4K only. CMN5_EVM_INDIV_MULTI_PIC or CMN5_EVM_INDIV_SINGLE_PIC
#define CMN5_QIC_SET_DTS_TIMEMODE			0x3F03	// HP4K only
	#define	CMN5_QIC_DTS_LOCAL_TIME		1	// default
	#define	CMN5_QIC_DTS_SYSTEM_TIME	2
#define CMN5_QIC_GET_DTS_TIMEMODE			0x3F04	// HP4K only
#define CMN5_QIC_GET_FANSPEED				0x3F05	// HP4K only

#define CMN5_QIC_CMD_BASE_END			0x3FFF		// for info
//};

// EXT_VIDEOOUT_BASEMODE {
#define CMN5_EVM_DISABLE			1	// disable external video out
#define CMN5_EVM_INDIV_MULTI_PIC	2	// individual multi-picutre mode
#define CMN5_EVM_CASCADE_SINGLE_PIC	4	// cascade single-picture mode
#define CMN5_EVM_INDIV_SINGLE_PIC	8	// individual single-picture mode
//};

typedef struct _CMN5_MULTI_COMMAND {
	ULONG				uCommand;
	ULONG				uBoard;
	ULONG				uChannel;
	union {
		UDA_ULONG			uParam[4];
		struct {
			UDA_ULONG		uParam0;
			UDA_ULONG		uParam1;
			UDA_ULONG		uParam2;
			UDA_ULONG		uParam3;
		};
	};
	BOOL				bRet;
} CMN5_MULTI_COMMAND;

//////////////////////////////////////////////////////////////////////////////////
// DataType
//enum DataType {
#define	CMN5_DT_VIDEO			1
#define	CMN5_DT_AUDIO			2
#define	CMN5_DT_COD				3
#define	CMN5_DT_SENSOR			4
#define	CMN5_DT_VSTATUS			5
#define	CMN5_DT_NET				6
#define	CMN5_DT_OVERLAY			7
#define	CMN5_DT_DEC				8
#define	CMN5_DT_LINK			9


#define	CMN5_DT_VIDEO_EX		0x11
#define	CMN5_DT_AUDIO_EX		0x12
#define	CMN5_DT_COD_EX			0x13
//#define	CMN5_DT_SENSOR_EX		0x14	// Reserved for future use
//#define	CMN5_DT_VSTATUS_EX		0x15	// Reserved for future use
#define	CMN5_DT_NET_EX			0x16
#define	CMN5_DT_DEC_EX			0x18

// Error code
//enum ErrorCode {
#define	CMN5_EC_NO_ERROR		0
#define	CMN5_EC_NO_DATA			1
#define	CMN5_EC_OVERFLOW		2
#define	CMN5_EC_ERROR			3
#define	CMN5_EC_FIRMWARE_ERROR	4
#define	CMN5_EC_FIRMWARE_DEAD	5


//////////////////////////////////////////////////////////////////////////////////
//enum CommonAdjustCmd {
#define	CMN5_CAC_EIO_ENABLE		0x00030010
#define	CMN5_CAC_DIO_DO			0x00030020
#define	CMN5_CAC_DIO_DI			0x00030040
#define	CMN5_CAC_EIO_DITYPE		0x00030080
//};

//////////////////////////////////////////////////////////////////////////////////
// EIO constats
#define CMN5_DIO_DITYPE_RELAY		1
#define CMN5_DIO_DITYPE_VOLTAGE		2

//////////////////////////////////////////////////////////////////////////////////
// EIO structure
typedef struct _CMN5_EIOBOARD_INFO {
	ULONG		uStructSize;

	ULONG		uEIOBoardCount;
	ULONG		uRev[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		uMaxDI[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		uMaxDO[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];
	ULONG		reserved[CMN5_BOARD_MAX_DIO_ARRAY_SIZE*5];
} CMN5_EIOBOARD_INFO;


/*	
#define struct {	
	\ ULONG			uDataType;
	\ ULONG			uSize;
	\ ULONG			uErrCode;
	\ ULONG			uBoardNum;	// Board ID
	} DATA_INFO_HEADER
*/

// Abstract structure
typedef struct _CMN5_DATA_INFO_HEADER {
	ULONG			uDataType;
	ULONG			uSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;				// Board ID
} CMN5_DATA_INFO_HEADER;

typedef struct _CMN5_QUERY_DATA_INFO{
	ULONG			uDataType;
	ULONG			uSize;
	ULONG			uErrCode;

	ULONG			uBoardNum;				// Board ID

	ULONG			reserved[16];
} CMN5_QUERY_DATA_INFO;

// Abstract structure
typedef struct _CMN5_DATA_INFO_DATA{
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;				// Board ID
	ULONG			uChannelNum;			// Channel ID of the Board
	ULONG			uHasNextData;
	UCHAR			*pDataBuffer;			// data buffer address
#if _MSC_VER >= 1300
	LARGE_INTEGER_CAP	TimeTag;				// KeQuerySystemTime()
#else
	LARGE_INTEGER	TimeTag;				// KeQuerySystemTime()
#endif
	ULONG			uBufferIdx;				// internal use! do not touch!
	
} CMN5_DATA_INFO_DATA;

#if _MSC_VER >= 1300
#define _CMN5_DATA_INFO_DATA_EX_CMM_BODY												\
	union {																				\
		ULONG		reservedCmn[32];														\
		struct {																		\
			ULONG			uDataType;													\
			ULONG			uStructSize;												\
			ULONG			uErrCode;													\
																						\
			ULONG			uBoardNum;				/* Board ID						*/	\
			ULONG			uChannelNum;			/* Channel ID of the Board		*/	\
			ULONG			uHasNextData;												\
			UCHAR			*pDataBuffer;			/* data buffer address			*/	\
			LARGE_INTEGER_CAP	TimeTag;				/* KeQuerySystemTime()			*/	\
			ULONG			uBufferIdx;				/* internal use! do not touch!	*/	\
		};																				\
	}
#else
#define _CMN5_DATA_INFO_DATA_EX_CMM_BODY												\
	union {																				\
		ULONG		reservedCmn[32];														\
		struct {																		\
			ULONG			uDataType;													\
			ULONG			uStructSize;												\
			ULONG			uErrCode;													\
																						\
			ULONG			uBoardNum;				/* Board ID						*/	\
			ULONG			uChannelNum;			/* Channel ID of the Board		*/	\
			ULONG			uHasNextData;												\
			UCHAR			*pDataBuffer;			/* data buffer address			*/	\
			LARGE_INTEGER	TimeTag;				/* KeQuerySystemTime()			*/	\
			ULONG			uBufferIdx;				/* internal use! do not touch!	*/	\
		};																				\
	}
#endif

typedef struct _CMN5_DATA_INFO_DATA_EX {
	_CMN5_DATA_INFO_DATA_EX_CMM_BODY;
} CMN5_DATA_INFO_DATA_EX;

typedef struct _CMN5_VIDEO_STATUS_INFO {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;
	
	ULONG VideoStatus[CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE];
	ULONG VideoStatusMask[CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE];
} CMN5_VIDEO_STATUS_INFO;

typedef struct _CMN5_SENSOR_STATUS_INFO{
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;										// Board ID // CMN5_MAKEEIOBOARDID for EIO
	ULONG			SensorStatus[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];	//for sensor detect event..
	ULONG			SensorStatusMask[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];
} CMN5_SENSOR_STATUS_INFO;

#define CMN5_MAKEEIOBOARDID(bdid, eiobdid)		((bdid & 0xFF) | ((eiobdid & 0xFF) << 16))
#define CMN5_GETBOARDID(bdid)					(bdid & 0xFF)
#define CMN5_GETEIOBOARDID(bdid)				((bdid >> 16) & 0xFF)

// Callback Prototype
//enum {
	// method
#define	CMN5_CAPTURE_METHOD_CALLBACK			0
//#define	CMN5_CAPTURE_METHOD_LOCK_AND_WAIT		1	// obsolete
#define	CMN5_CAPTURE_METHOD_QUERY				2

	// capture data type (We do not recommend that you use following names.)
#define	CMN5_CAPTURE_CALLBACK_ONCAPTURE			0x10	// legacy type
#define	CMN5_CAPTURE_CALLBACK_ONVIDEO			0x11	// legacy type
#define	CMN5_CAPTURE_CALLBACK_ONSENSOR			0x12	// legacy type
	// renamed (Use following names instead.)
#define	CMN5_CAPTURE_TYPE_ON_DATA				CMN5_CAPTURE_CALLBACK_ONCAPTURE	// Primary data of all 'Media'.
#define	CMN5_CAPTURE_TYPE_ON_CHANNEL			CMN5_CAPTURE_CALLBACK_ONVIDEO
#define	CMN5_CAPTURE_TYPE_ON_SENSOR				CMN5_CAPTURE_CALLBACK_ONSENSOR
	// In case of EIO Board sensor callback you should be careful to 'ULONG uBoardID' in first parameter
	// which is means uBoardID at bit31 to bit24 and uEIOBoardID at bit23 to bit16.
//};

#define	CMN5_CAPTURE_TYPE_ON_DATA_EX			0x00010010
//#define	CMN5_CAPTURE_TYPE_ON_CHANNEL_EX			0x00010011	// Reserved for future use
//#define	CMN5_CAPTURE_TYPE_ON_SENSOR_EX			0x00010012	// Reserved for future use

//Misc
#define CMN5_MAKELONG(lo,hi)  (((lo)&0xFFFF)|((hi)<<16))

#ifdef __cplusplus
extern "C"
{
#endif
BOOL CMN5_API Cmn5CreateInstance(void **pUnknown);

typedef BOOL (__stdcall *CMN5_CALLBACK_ONE_PARAM)(UDA_ULONG A);
typedef BOOL (__stdcall *CMN5_CALLBACK_TWO_PARAM)(UDA_ULONG A, UDA_ULONG B);
typedef BOOL (__stdcall *CMN5_CALLBACK_THREE_PARAM)(UDA_ULONG A, UDA_ULONG B, UDA_ULONG C);
#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////
#if 1 // HP4K
// CMN Command Set

// CMN5_CMD_TYPE
// [31..31]	: 1 means new cmd, 0 means old cmd
// [24..30]	: media
// [20..23] : level (?,SETUP,RUN,INIT)
// [16..19] : reserved
// [08..15] : user Command Major
// [00..07] : user Command Minor
// ex: 0x80 0 6 0 01 01
//     new cmd, media none, level SETUP|RUN, Major 1, Minor 1
#pragma pack( push, __Cmn5CmdSet )
#pragma pack(1)
typedef union _Cmn5CmdSetU {
	DWORD all;
	struct _Cmn5CmdSet{
		DWORD NewCmd	:1; 
		DWORD Media		:7; 
		DWORD Level		:4; 
		DWORD rev		:4; 
		DWORD Major		:8; 
		DWORD Minor		:8; 
	} u;
}Cmn5CmdSet;
#pragma pack( pop, __Cmn5CmdSet )

#define CMN5_CMD_SET(media, level, major, minor) ((1<<31) | ((media)<<24) | ((level)<<20) | ((major)<<8) | (minor))

#define CMD5_LEVEL_INIT			(1)
#define CMD5_LEVEL_SETUP		(2)
#define CMD5_LEVEL_RUN			(4)
#define CMD5_LEVEL_SETUP_RUN	(CMD5_LEVEL_SETUP|CMD5_LEVEL_RUN)
#define CMD5_LEVEL_ALL			(0xF)

//enum CmnAdjustCmd {
#define	CMN5_AC_BURNTIN_TEXT	CMN5_CMD_SET(0,CMD5_LEVEL_SETUP_RUN,1,1)
		// uParam[0] = nID			{0~MAX_BITXT_NUMBER-1}		// QUERY_BIT_TXT_ATTR	// hp4k 5
		// uParam[1] = CMN5_BURNTIN_TEXT_INFO*
#define	CMN5_AC_BURNTIN_TEXT_CLEAR_ALL	CMN5_CMD_SET(0,CMD5_LEVEL_SETUP_RUN,1,2)
		
#define CMN5_AC_PRIVACY_REGION_CLEAR_ALL CMN5_CMD_SET(0,CMD5_LEVEL_SETUP_RUN,1,11)
#define	CMN5_AC_PRIVACY_REGION_RECT	CMN5_CMD_SET(0,CMD5_LEVEL_SETUP_RUN,1,12)
		// uParam[0] = nID			{0~MAX_PR_NUMBER-1}			// QUERY_PRIVACY_REGION_ATTR	// hp4k 1
		// uParam[1] = CMN5_PRIVACY_REGION_RECT*
#define	CMN5_AC_PRIVACY_REGION_MAP	CMN5_CMD_SET(0,CMD5_LEVEL_SETUP_RUN,1,13)
		// uParam[0] = nID			{0~MAX_PR_NUMBER-1}			// QUERY_PRIVACY_REGION_ATTR	// hp4k 1
		// uParam[1] = nRegionCount {0~MAX_PRMAP_MAXCOUNT-1}	// hp4 1
		// uParam[2] = CMN5_PRIVACY_REGION_MAP*
//}

// eum CMN5_BURNTIN_TEXT_DTS_TYPE {
#define CMN5_BURNTIN_TEXT_DTS_NONE 		(0)
#define CMN5_BURNTIN_TEXT_DTS_MDY_12H 	(1)
#define CMN5_BURNTIN_TEXT_DTS_DMY_12H 	(2)
#define CMN5_BURNTIN_TEXT_DTS_YMD_12H 	(3)
#define CMN5_BURNTIN_TEXT_DTS_MDY_24H 	(4)
#define CMN5_BURNTIN_TEXT_DTS_DMY_24H 	(5)
#define CMN5_BURNTIN_TEXT_DTS_YMD_24H	(6)
//}

// eum CMN5_BURNTIN_TEXT_LOC_TYPE {
#define CMN5_BURNTIN_TEXT_LOC_USER 		(0)		// (posX,posY)
#define CMN5_BURNTIN_TEXT_LOC_LT 		(1)		// left top
#define CMN5_BURNTIN_TEXT_LOC_LB 		(2)		// left bottom
#define CMN5_BURNTIN_TEXT_LOC_RT 		(3)		// right top
#define CMN5_BURNTIN_TEXT_LOC_RB 		(4)		// right bottom
//}

#define CMN5_MAX_BURNTIN_TEXT	100

typedef struct _CMN5_BURNTIN_TEXT_INFO {
	USHORT	InfoVersion;		// 0x1000
	USHORT	bEnable	;			// {0,others}
    UCHAR	fgTransparency;		// Transparency {0~255} 0:black opaque , 255:full transparent	// HP4K not support
    UCHAR	bgTransparency;		// Transparency {0~255} 0:black opaque , 255:full transparent	
    DWORD	fgColor;			// {R,G,B} COLORREF												// HP4K not support
    DWORD	bgColor;			// {R,G,B} COLORREF												// HP4K not support
    USHORT	eLocationType;		// {CMN5_BURNTIN_TEXT_LOC_TYPE}
    USHORT	posX;				// pixel position {0~ImageMaxWidth}
    USHORT	posY;				// pixel position {0~ImageMaxHeight}
    USHORT	eDTSType;			// {CMN5_BURNTIN_TEXT_DTS_TYPE} 
    USHORT	uTextLength;					// unicode text count
    USHORT	aText[CMN5_MAX_BURNTIN_TEXT];	// unicode text	<100
    // V1.0.0.0
    BYTE		rev[256-(CMN5_MAX_BURNTIN_TEXT+12)*2];					// sizeof(CMN5_BURNTIN_TEXT_INFO)==256
} CMN5_BURNTIN_TEXT_INFO;

#define CMN5_PRIVACY_REGION_MAX_RECT	32
typedef struct _CMN5_PRIVACY_REGION_RECT {
	USHORT	InfoVersion;		//0x1000
	USHORT	bEnable;
	USHORT	nCount;				// aRect count		//hp4k 16
	UCHAR	rev[32-3*2];		// sizeof(CMN5_PRIVACY_REGION_RECT) == (32 + 16*CMN5_PRIVACY_REGION_MAX_RECT)
	RECT	aRect[CMN5_PRIVACY_REGION_MAX_RECT];		// pixel position {0~ImageMaxWidth}
}CMN5_PRIVACY_REGION_RECT;

#endif // CMN Command Set
/////////////////////////////////////////////////////////////////


#pragma pack( pop, Cmn5BoardLib_EX_H )

#endif // CMN5BOARDLIB_EX_H