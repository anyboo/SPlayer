#ifndef __VCA5_CORE_LIB_H__
#define __VCA5_CORE_LIB_H__

#pragma pack( push, VCA5CORELIB_H )
#pragma pack(8)
#include <initguid.h>
#include <Unknwnbase.h>
#include "VCATypes.h"

#define VCA5_MAKEVERSION(A,B,C) (((A << 16)&0xFFFF0000) | ((B << 8)&0x0000FF00) | (C&0x000000FF))
#define	VCA5_VERSION	VCA5_MAKEVERSION(1, 4, 2)


// Error Code
/*
 Severity	: [31..29]
			0 = Operation Success
			1 = Warning
			2 = Operarion Fail
			3 = Implementation Error

 Level		: [28..26]
			0 = None
			1 = Kernel
			2 = DLL
			3 = APP

 Handling	: [25..23]		// discarded field
			0 = None
			1 = xxx Driver Reinit
			2 = xxx Reboot
			3 = xxx Report manufacturer

 Reserved	: [22..20]

 Major Code	: [19..8]
	[19..17] : 0(0-511): common
	[19..17] : 4(?-?): Reserved
	[19..17] : 5(?-?): Reserved
	[19..17] : 6(?-?): Reserved
	[19..17] : 7(?-?): Reserved

 Minor Code	: [7..0]
*/

typedef enum {	
				VCA5_ERR_SEVERITY_OPERATION_SUCCESS = 0,
				VCA5_ERR_SEVERITY_WARNING,
				VCA5_ERR_SEVERITY_OPERATION_FAIL,
				VCA5_ERR_SEVERITY_IMPLEMENTATION_ERROR,
} VCA5_ERR_SEVERITY;

#define VCA5_ERR_SEVERITY_FIELD_SIZE		3
#define VCA5_ERR_SEVERITY_FIELD_POS			29

#define VCA5_ERR_LEVEL_FIELD_SIZE			3
#define VCA5_ERR_LEVEL_FIELD_POS			26

#define VCA5_ERR_HANDLING_FIELD_SIZE		3
#define VCA5_ERR_HANDLING_FIELD_POS			23

#define VCA5_ERR_MAJOR_CODE_FIELD_SIZE		12
#define VCA5_ERR_MAJOR_CODE_FIELD_POS		8

#define VCA5_ERR_MINOR_CODE_FIELD_SIZE		8
#define VCA5_ERR_MINOR_CODE_FIELD_POS		0

#define VCA5_DEFINE_ERRCODE(severity, level, handling, majorcode)	(unsigned long)(	\
	((severity & ((1 << VCA5_ERR_SEVERITY_FIELD_SIZE) - 1)) << VCA5_ERR_SEVERITY_FIELD_POS) | \
	((level & ((1 << VCA5_ERR_LEVEL_FIELD_SIZE) - 1)) << VCA5_ERR_LEVEL_FIELD_POS) | \
	((handling & ((1 << VCA5_ERR_HANDLING_FIELD_SIZE) - 1)) << VCA5_ERR_HANDLING_FIELD_POS) | \
	((majorcode & ((1 << VCA5_ERR_MAJOR_CODE_FIELD_SIZE) - 1)) << VCA5_ERR_MAJOR_CODE_FIELD_POS) \
	)

#define VCA5_DEFINE_ERRCODE2(severity, majorcode)					(unsigned long)(	\
	((severity & ((1 << VCA5_ERR_SEVERITY_FIELD_SIZE) - 1)) << VCA5_ERR_SEVERITY_FIELD_POS) | \
	((majorcode & ((1 << VCA5_ERR_MAJOR_CODE_FIELD_SIZE) - 1)) << VCA5_ERR_MAJOR_CODE_FIELD_POS) \
	)

#define VCA5_EXTRACT_ERRCODE2(code)	(unsigned long)( \
	(code & (((1 << VCA5_ERR_SEVERITY_FIELD_SIZE) - 1) << VCA5_ERR_SEVERITY_FIELD_POS)) | \
	(code & (((1 << VCA5_ERR_MAJOR_CODE_FIELD_SIZE) - 1) << VCA5_ERR_MAJOR_CODE_FIELD_POS)) \
	)

#define VCA5_ERRCODE_SEVERITY(code) ( \
	(code >> VCA5_ERR_SEVERITY_FIELD_POS) & ((1 << VCA5_ERR_SEVERITY_FIELD_SIZE) - 1) \
	)

#define VCA5_ERRCODE_LEVEL(code) ( \
	(code >> VCA5_ERR_LEVEL_FIELD_POS) & ((1 << VCA5_ERR_LEVEL_FIELD_SIZE) - 1) \
	)

#define VCA5_ERRCODE_HANDLING(code) ( \
	(code >> VCA5_ERR_HANDLING_FIELD_POS) & ((1 << VCA5_ERR_HANDLING_FIELD_SIZE) - 1) \
	)

#define VCA5_ERRCODE_MAJOR_CODE(code) ( \
	(code >> VCA5_ERR_MAJOR_CODE_FIELD_POS) & ((1 << VCA5_ERR_MAJOR_CODE_FIELD_SIZE) - 1) \
	)

#define VCA5_ERRCODE_MINOR_CODE(code) ( \
	(code >> VCA5_ERR_MINOR_CODE_FIELD_POS) & ((1 << VCA5_ERR_MINOR_CODE_FIELD_SIZE) - 1) \
	)

//################################################################################################
// VCA5_ERR_SEVERITY_SUCCESS
//################################################################################################
// VCA5_ERRCMN_S_OK
// OK
#define VCA5_ERRCMN_S_OK					VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_SUCCESS, 0)
// VCA5_ERRCMN_S_FALSE
#define VCA5_ERRCMN_S_FALSE					VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_SUCCESS, 1)
// VCA5_ERRCMN_S_NO_MORE_ERROR
#define VCA5_ERRCMN_S_NO_MORE_ERROR			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_SUCCESS, 2)

//################################################################################################
// VCA5_ERR_SEVERITY_WARNING
//################################################################################################

//################################################################################################
// VCA5_ERR_SEVERITY_OPERATION_FAIL
//################################################################################################

//------------------------------------------------------------------------------------------------
// 	[19..17] : 0(0-511): common
//------------------------------------------------------------------------------------------------
// VCA5_ERRCMN_F_INSUFFICIENT_RESOURCES
#define VCA5_ERRCMN_F_INSUFFICIENT_RESOURCES	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 0)

// VCA5_ERRCMN_F_OUT_OF_MEMORY
#define VCA5_ERRCMN_F_OUT_OF_MEMORY				VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 1)

// VCA5_ERRCMN_F_OPERATION_UNSUCCESSFUL
#define VCA5_ERRCMN_F_OPERATION_UNSUCCESSFUL	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 2)

// VCA5_ERRCMN_F_CANNOT_GET_ERROR_CODE
#define VCA5_ERRCMN_F_CANNOT_GET_ERROR_CODE		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 3)

// VCA5_ERRCMN_F_NOT_IMPLEMENTED
#define VCA5_ERRCMN_F_NOT_IMPLEMENTED			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 4)

// VCA5_ERRCMN_F_DRIVER_OPEN_FAIL
#define VCA5_ERRCMN_F_DRIVER_OPEN_FAIL			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 5)

// VCA5_ERRCMN_F_INVALID_PARAMETER
#define VCA5_ERRCMN_F_INVALID_PARAMETER			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 6)

// VCA5_ERRCMN_F_IOCTL_INCORRECT_BUFFER_SIZE
#define VCA5_ERRCMN_F_IOCTL_INCORRECT_BUFFER_SIZE	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 7)

// VCA5_ERRCMN_F_EXCEPTION_IN_CALLBACK
#define VCA5_ERRCMN_F_EXCEPTION_IN_CALLBACK		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 8)

// VCA5_ERRCMN_F_INVALID_IOCTL_CODE
#define VCA5_ERRCMN_F_INVALID_IOCTL_CODE		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 9)

// VCA5_ERRCMN_F_INVALID_CALLING_LEVEL
#define VCA5_ERRCMN_F_INVALID_CALLING_LEVEL		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 10)

// VCA5_ERRCMN_F_INVALID_POINTER
#define VCA5_ERRCMN_F_INVALID_POINTER			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 11)

// VCA5_ERRCMN_F_INVALID_FUNCTION_POINTER
#define VCA5_ERRCMN_F_INVALID_FUNCTION_POINTER	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 12)

// VCA5_ERRCMN_F_OBSOLETE_FUNCTION
#define VCA5_ERRCMN_F_OBSOLETE_FUNCTION			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 13)

// VCA5_ERRCMN_F_INVALID_KEY_CODE
#define VCA5_ERRCMN_F_INVALID_KEY_CODE			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 14)

// VCA5_ERRCMN_F_UNSUPPORTED_IMAGE_SIZE
#define VCA5_ERRCMN_F_UNSUPPORTED_IMAGE_SIZE	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 15 )

// VCA5_ERRCMN_F_UNSUPPORTED_COLOR_FORMAT
#define VCA5_ERRCMN_F_UNSUPPORTED_COLOR_FORMAT	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 16 )

// VCA5_ERRCMN_F_INTERNAL_ERROR
#define VCA5_ERRCMN_F_INTERNAL_ERROR			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 17 )

// VCA5_ERRCMN_F_ALREADY_ACTIVATED
#define VCA5_ERRCMN_F_ALREADY_ACTIVATED			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 18 )

// VCA5_ERRCMN_F_UNABLE_TO_GENERATE_GUID
#define VCA5_ERRCMN_F_UNABLE_TO_GENERATE_GUID	VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 19 )

// VCA5_ERRCMN_F_ACCESS_DENIED
#define VCA5_ERRCMN_F_ACCESS_DENIED				VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 20 )

// VCA5_ERRCMN_F_VERSION_MISMATCH
#define VCA5_ERRCMN_F_VERSION_MISMATCH			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 21 )

// VCA5_ERRCMN_F_TOO_MANY_LICENSES
#define VCA5_ERRCMN_F_TOO_MANY_LICENSES			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 22 )

// VCA5_ERRCMN_F_INVALID_HANDLE
#define VCA5_ERRCMN_F_INVALID_HANDLE			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 23 )

// VCA5_ERRCMN_F_NOT_ACTIVATED
#define VCA5_ERRCMN_F_NOT_ACTIVATED				VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 24 )

// VCA5_ERRCMN_F_LICENSE_EXPIRED
#define VCA5_ERRCMN_F_LICENSE_EXPIRED			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 25 )

// VCA5_ERRCMN_F_LICENSE_INVALID
#define VCA5_ERRCMN_F_LICENSE_INVALID			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 26 )

// VCA5_ERRCMN_F_VMM_DETECTED
#define VCA5_ERRCMN_F_VMM_DETECTED				VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 27 )

// VCA5_ERRCMN_F_WRONG_LICENSE_INDEX
#define VCA5_ERRCMN_F_WRONG_LICENSE_INDEX		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 28 )

// VCA5_ERRCMN_F_NO_DAEMON
#define VCA5_ERRCMN_F_NO_DAEMON					VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 29 )

// VCA5_ERRCMN_F_INVALID_PLATFORM
#define VCA5_ERRCMN_F_INVALID_PLATFORM			VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 30)

// VCA5_ERRCMN_F_INVALID_DIGITALSIGN
#define VCA5_ERRCMN_F_INVALID_DIGITALSIGN		VCA5_DEFINE_ERRCODE2(VCA5_ERR_SEVERITY_OPERATION_FAIL, 31)


//
#define VCA5_MAX_ERROR_CODE_MSG_LENGTH		128
#define VCA5_MAX_SESSION_LENGTH				64
typedef struct _VCA5_ERROR_CODE_ITEM {
	unsigned long	ErrorCode;										// code
	long long		TimeStamp;										// time-stamp
	unsigned long	Reserved;										// reserved
	char			AuxMsg[VCA5_MAX_ERROR_CODE_MSG_LENGTH];			// aux message
	char			SessionName[VCA5_MAX_SESSION_LENGTH];			// session name
} VCA5_ERROR_CODE_ITEM;


#define VCA5_MAX_RESERVED_LICENSE_INFO	16
#define VCA5_MAX_RESERVED_HWGUID_INFO	16
#define VCA5_MAX_RESERVED_SYSTEM_INFO	16
#define VCA5_MAX_RESERVED_ENGINE_INFO	16
#define VCA5_MAX_RESERVED_ENGINE_PARAM	16
#define VCA5_MAX_RESERVED_RULE			16
#define VCA5_MAX_RESERVED_INFO			16
#define	VCA5_MAX_RESERVED_TRACKERPARAMS	11
#define VCA5_MAX_STR_LEN				256

#define VCA5_MAX_NUM_RULES				600
#define VCA5_MAX_NUM_ZONES				40
#define VCA5_MAX_NUM_ZONE_POINTS		40
#define VCA5_MAX_NUM_COUNTERS			20
#define VCA5_MAX_NUM_SUBCOUNTERS		20
#define VCA5_MAX_NUM_TRAIL_POINTS		20
#define VCA5_MAX_NUM_CLSOBJECTS			20
#define VCA5_MAX_NUM_PALETTE			10
#define VCA5_MAX_OUTPUT_BUF_SIZE		(50*1024)

#define VCA5_MAX_LICENSE_DESC_LEN		256

#define VCA5_MAX_NUM_OBJECTS			50
#define VCA5_MAX_NUM_LICENSE			64
#define VCA5_MAX_NUM_ENGINE				128

#define	VCA5_ID_ALL						4095
#define VCA5_ID_INVALID					30000

// Video Format
#define VCA5_VIDEO_FORMAT_NTSC_M	VCA_VF_NTSC_M
#define VCA5_VIDEO_FORMAT_PAL_B		VCA_VF_PAL_B

// Color Format
#define	VCA5_COLOR_FORMAT_RGB24		VCA_CF_RGB24				
#define	VCA5_COLOR_FORMAT_RGB16		VCA_CF_RGB16		
#define	VCA5_COLOR_FORMAT_RGB15		VCA_CF_RGB15	//Not support	
#define	VCA5_COLOR_FORMAT_YUY2		VCA_CF_YUY2
#define VCA5_COLOR_FORMAT_UYVY		VCA_CF_UYVY
#define	VCA5_COLOR_FORMAT_Y8		VCA_CF_Y8
#define	VCA5_COLOR_FORMAT_YV12		VCA_CF_YV12


// Rotate Image
#define VCA5_IMAGE_ROTATE_00		0
#define VCA5_IMAGE_ROTATE_90		1
#define VCA5_IMAGE_ROTATE_180		2
#define VCA5_IMAGE_ROTATE_270		3

// Frame Type
#define VCA5_IMAGE_FRAME_AUTO		0
#define VCA5_IMAGE_FRAME_INTERLACE	1
#define VCA5_IMAGE_FRAME_PROGRESS	2


typedef struct _VCA5_POINT {
	unsigned short		x;
	unsigned short		y;
} VCA5_POINT;

typedef struct _VCA5_RECT {
	unsigned short		x;
	unsigned short		y;
	unsigned short		w;
	unsigned short		h;
} VCA5_RECT;


//Image Size
union VCA5_IMAGESIZE {
	unsigned long uAll;
	struct {
		unsigned long nWidth			: 12;	//[0..11]
		unsigned long nHeight			: 12;	//[12..23]
		unsigned long nRotate			: 2;	//[24,25]
		unsigned long nFrameType		: 2;	//[26,27]
		unsigned long reserved			: 4;	//[28..31]
	};
};

#define VCA5_MAKEIMGSIZE(w, h)				((w & 0xFFF) | ((h &0xFFF) << 12))
#define VCA5_GETIMGWIDTH(is)				(is & 0xFFF)
#define VCA5_GETIMGHEIGHT(is)				((is >> 12) & 0xFFF)

#define VCA5_SETIMAGEROTATE(is, r)			(((r&0x3) << 24) | (is&0xFCFFFFFF) )
#define VCA5_GSETIMAGEROTATE(is)			((is >> 24) & 0x03)

#define VCA5_SETIMAGEFRAMETYPE(is, r)		(((r&0x3) << 26) | (is&0xF3FFFFFF) )
#define VCA5_GETIMAGEFRAMETYPE(is)			((is >> 26) & 0x03)


typedef struct _VCA5_HWGUID_INFO {
	char*				szUSN;
	char*				szGUID;
	char*				szDrvDllPath;
	unsigned long		reserved[VCA5_MAX_RESERVED_HWGUID_INFO];
} VCA5_HWGUID_INFO;


typedef struct _VCA5_LICENSE_INFO {
	char*				szUSN;
	char*				szLicense;
	char*				szDrvDllPath;
	unsigned char		nLicenseID;			//Out
	unsigned long		ulNumOfEngine;		//Out
	unsigned long		ulFunction;			//Out
	char				szLicenseDesc[VCA5_MAX_STR_LEN];		//Out
	unsigned long		reserved[VCA5_MAX_RESERVED_LICENSE_INFO];
} VCA5_LICENSE_INFO;


typedef struct _VCA5_SYSTEM_INFO {
	unsigned long		ulNumOfLicense;
	unsigned long		ulNumOfEngine[VCA5_MAX_NUM_LICENSE];
	unsigned long		ulDllVersion;
	unsigned long		reserved[VCA5_MAX_RESERVED_SYSTEM_INFO];
} VCA5_SYSTEM_INFO;


typedef struct _VCA5_ENGINE_INFO {
	unsigned long		ulLicenseNum;
	unsigned long		ulFeatureType;
	unsigned long		ulFunction;
	char				szLicenseDesc[ VCA5_MAX_STR_LEN ];
	unsigned long		reserved[VCA5_MAX_RESERVED_ENGINE_INFO];
} VCA5_ENGINE_INFO;


typedef struct _VCA5_ENGINE_PARAMS {
	unsigned long 		ulVideoFormat;
	unsigned long		ulColorFormat;
	unsigned long		ulImageSize;
	unsigned long		ulFrameRate100;
	unsigned long		ulLicenseCnt;
	unsigned char		ucLicenseId[VCA5_MAX_NUM_LICENSE];
	VCA5_RECT			imageROI;
	unsigned long		reserved[VCA5_MAX_RESERVED_ENGINE_PARAM-5];
} VCA5_ENGINE_PARAMS;


typedef struct _VCA5_XMLCFG_PARAMS {
	unsigned long		ulMedia;					// Save/load to file or buffer
	unsigned long		ulCfgFlags;					// Flags say what type of info to save/load
	char*				pszBufOrFilename;			// The buffer or the filename 
	unsigned long		ulBufLen;					// How long is the buffer (in bytes)
} VCA5_XMLCFG_PARAMS;

#define	VCA5_QR_BASE				1024
#define	VCA5_QR_GETHWGUID			(VCA5_QR_BASE+1)
#define VCA5_QR_GETHWGUIDUSN		VCA5_QR_GETHWGUID
#define VCA5_QR_GETHWGUIDOPEN		(VCA5_QR_BASE+2)

#define VCA5_CMD_SETENGINEPARAMS	1
#define	VCA5_CMD_BASE				256
#define VCA5_CMD_SETZONE			(VCA5_CMD_BASE+0x01)
#define	VCA5_CMD_CLEARZONE			(VCA5_CMD_BASE+0x02)
#define VCA5_CMD_SETRULE			(VCA5_CMD_BASE+0x03)
#define	VCA5_CMD_CLEARRULE			(VCA5_CMD_BASE+0x04)
#define VCA5_CMD_SETSYNCTIME		(VCA5_CMD_BASE+0x06)
#define VCA5_CMD_SETCOUNTER			(VCA5_CMD_BASE+0x07)
#define	VCA5_CMD_CLEARCOUNTER		(VCA5_CMD_BASE+0x08)
#define	VCA5_CMD_SETCOUNTERRES		(VCA5_CMD_BASE+0x0A)
#define	VCA5_CMD_SETCALIBPARAMS		(VCA5_CMD_BASE+0x0B)
#define VCA5_CMD_SETOBJECT			(VCA5_CMD_BASE+0x0D)
#define	VCA5_CMD_CLEAROBJECT		(VCA5_CMD_BASE+0x0E)
#define	VCA5_CMD_SETMETAFMT 		(VCA5_CMD_BASE+0x0F)

#define VCA5_CMD_SETRETRIGTIME		(VCA5_CMD_BASE+0x10)
#define VCA5_CMD_GETVERSIONINFO		(VCA5_CMD_BASE+0x11)
#define VCA5_CMD_GETCOUNTERRES		(VCA5_CMD_BASE+0x12)
#define VCA5_CMD_SETTAMPERPARAMS	(VCA5_CMD_BASE+0x13)
#define VCA5_CMD_SETBIAPARAMS		(VCA5_CMD_BASE+0x14)
#define VCA5_CMD_SETSTABENABLE		(VCA5_CMD_BASE+0x15)

#define VCA5_CMD_SETINHIBIT			(VCA5_CMD_BASE+0x1A)

#define VCA5_CMD_GETZONE			(VCA5_CMD_BASE+0x50)
#define VCA5_CMD_GETRULE			(VCA5_CMD_BASE+0x51)
#define VCA5_CMD_GETCOUNTER			(VCA5_CMD_BASE+0x52)
#define VCA5_CMD_GETOBJECT			(VCA5_CMD_BASE+0x53)

#define VCA5_CMD_GETMETAFMT			(VCA5_CMD_BASE+0x55)
#define VCA5_CMD_GETRETRIGTIME		(VCA5_CMD_BASE+0x56)
#define VCA5_CMD_GETTAMPERPARAMS	(VCA5_CMD_BASE+0x57)
#define VCA5_CMD_GETSTABENABLE		(VCA5_CMD_BASE+0x5A)
#define VCA5_CMD_GETCALIBPARAMS		(VCA5_CMD_BASE+0x5B)
#define VCA5_CMD_GETSCENECHANGEPARAMS	(VCA5_CMD_BASE + 0x5C)


#define VCA5_CMD_SETTRACKERPARAMS	(VCA5_CMD_BASE+0x19)
#define VCA5_CMD_SETSCENECHANGEPARAMS	(VCA5_CMD_BASE + 0x22)


#define VCA5_CMD_SAVECFGXML			(VCA5_CMD_BASE+0x400)
#define VCA5_CMD_LOADCFGXML			(VCA5_CMD_BASE+0x401)

enum {
	VCA5_RULE_TAILGATING_BYTRACKER		= 0x0000,	// trigger tailgating rule by tracker
	VCA5_RULE_TAILGATING_BYLINECOUNTER	= 0x0001,	// trigger tailgating rule by linecounter
};

enum VCA5_ZONE_TYPE_E {
	VCA5_ZONE_TYPE_DETECTION		= 0x0001,
	VCA5_ZONE_TYPE_NONDETECTION		= 0x0003,
};

enum VCA5_ZONE_STYLE_E {
	VCA5_ZONE_STYLE_TRIPWIRE	= 0x0001,
	VCA5_ZONE_STYLE_POLYGON		= 0x0002,
	VCA5_ZONE_STYLE_NOTDEFINED	= 0x0010,
};

enum VCA5_METAFMT_E {
	VCA5_METAFMT_OBJECT			= 0x0001,
	VCA5_METAFMT_EVENT			= 0x0002,
	VCA5_METAFMT_COUNTING 		= 0x0004,
	VCA5_METAFMT_BLOB			= 0x0008,
	VCA5_METAFMT_HEIGHT			= 0x0010,
	VCA5_METAFMT_AREA			= 0x0020,
	VCA5_METAFMT_SPEED			= 0x0040,
	VCA5_METAFMT_TAMPER			= 0x0080,
	VCA5_METAFMT_STAB			= 0x0100,
	VCA5_METAFMT_LINECOUNTER	= 0x0200,
	VCA5_METAFMT_STATISTICS		= 0x0400,
	VCA5_METAFMT_COLORSIG		= 0x0800,
	VCA5_METAFMT_SCENECHANGE	= 0x1000,
	VCA5_METAFMT_SMOKEFIRE		= 0x2000,
	
	VCA5_METAFMT_DEFAULT		= 0x0277,
	VCA5_METAFMT_ALL			= 0xffff
};
	
enum VCA5_RULE_TYPE_E {
	VCA5_RULE_TYPE_PRESENCE 		= 0x0001,
	VCA5_RULE_TYPE_ENTER,
	VCA5_RULE_TYPE_EXIT,
	VCA5_RULE_TYPE_APPEAR,
	VCA5_RULE_TYPE_DISAPPEAR,
	VCA5_RULE_TYPE_STOP,
	VCA5_RULE_TYPE_DWELL,
	VCA5_RULE_TYPE_DIRECTION,
	VCA5_RULE_TYPE_SPEED,
	VCA5_RULE_TYPE_AREA,
	VCA5_RULE_TYPE_TAILGATING,
	VCA5_RULE_TYPE_ABOBJ,
	VCA5_RULE_TYPE_LINECOUNTER_A,
	VCA5_RULE_TYPE_LINECOUNTER_B,
	VCA5_RULE_TYPE_RMOBJ,
	VCA5_RULE_TYPE_COLSIG,
	VCA5_RULE_TYPE_SMOKE,
	VCA5_RULE_TYPE_FIRE,
	VCA5_RULE_TYPE_OTHERS,
	VCA5_RULE_TYPE_NUM
};

enum VCA5_FEATURE_STRUCT_TYPE_E {
	VCA5_FEATURE_DEF_NULL,
	VCA5_FEATURE_DEF_VCASYS
};

enum VCA5_FEATURE_TYPE_E {
	VCA5_FEATURE_PRESENCE		= 0x00000002,
	VCA5_FEATURE_ENTER			= 0x00000004,
	VCA5_FEATURE_EXIT			= 0x00000008,
	VCA5_FEATURE_APPEAR			= 0x00000010,
	VCA5_FEATURE_DISAPPEAR		= 0x00000020,
	VCA5_FEATURE_STOPPED		= 0x00000040,
	VCA5_FEATURE_DWELL			= 0x00000080,
	VCA5_FEATURE_DIRECTION		= 0x00000100,
	VCA5_FEATURE_SPEED			= 0x00000200,
	VCA5_FEATURE_ABOBJ			= 0x00000400,
	VCA5_FEATURE_TAILGATING		= 0x00000800,
	VCA5_FEATURE_LINECOUNTER	= 0x00001000,
	VCA5_FEATURE_SMOKE			= 0x00002000,
	VCA5_FEATURE_FIRE			= 0x00004000,
	VCA5_FEATURE_COUNTING		= 0x00010000,
	VCA5_FEATURE_CALIBRATION	= 0x00020000,
	VCA5_FEATURE_METADATA		= 0x00040000,
	VCA5_FEATURE_PEOPLETRACKING = 0x00080000
};

enum VCA5_COUNTER_TYPE_E {
	VCA5_COUNTER_INCREMENT		= 0x0001,
	VCA5_COUNTER_DECREMENT		= 0x0002,
	VCA5_COUNTER_INSTANT		= 0x0008,
};

enum VCA5_CALIB_STATUS_E {
	VCA5_CALIB_STATUS_CALIBRATED_OVERHEAD   	=	4,
	VCA5_CALIB_STATUS_CALIBRATED_SIDEON   		=	3,
	VCA5_CALIB_STATUS_CALIBRATED				=	2,
	VCA5_CALIB_STATUS_UNCALIBRATED 	  			=	0,
	VCA5_CALIB_STATUS_INVALIDSETUP	  			=	-1,
	VCA5_CALIB_STATUS_FAILED_TOOFEW_MARKERS 	=	-2,
	VCA5_CALIB_STATUS_FAILED_TILT_OUTOFRANGE	=	-3,
	VCA5_CALIB_STATUS_FAILED_HEIGHT_OUTOFRANGE	=	-4,
	VCA5_CALIB_STATUS_FAILED_FOV_OUTOFRANGE		=	-5
};

enum VCA5_XMLCFG_MEDIATYPE {
	VCA5_XMLCFG_BUFFER			= 1,
	VCA5_XMLCFG_FILE			= 2
};

enum VCA5_XMLCFG_FLAGS {
	VCA5_CFGFLAG_CHANNEL		= 0x00000001,
	VCA5_CFGFLAG_ZONE			= 0x00000002,
	VCA5_CFGFLAG_EVENT			= 0x00000004,
	VCA5_CFGFLAG_COUNTER		= 0x00000008,
	VCA5_CFGFLAG_CLASSIFICATION	= 0x00000010,
	VCA5_CFGFLAG_CALIBRATION	= 0x00000020,
	VCA5_CFGFLAG_BIA			= 0x00000040,
	VCA5_CFGFLAG_TAMPER			= 0x00000080,
	VCA5_CFGFLAG_LICENSE		= 0x00000100,
	VCA5_CFGFLAG_PTZTRACKING	= 0x00000200,
	VCA5_CFGFLAG_SCENECHANGE	= 0x00000400,
	VCA5_CFGFLAG_COUNTDOG		= 0x00000800,
	VCA5_CFGFLAG_ALL			= 0xFFFFFFFF
};

enum VCA5_SCENECHANGE_MODE_E {
	VCA5_SCENECHANGE_MODE_DISABLED = 0,
	VCA5_SCENECHANGE_MODE_AUTOMATIC,
	VCA5_SCENECHANGE_MODE_MANUAL
} ;

enum VCA5_PALLET_COLOR_E {
	VCA5_PALLET_COLOR_BLACK = 0,
	VCA5_PALLET_COLOR_BROWN,
	VCA5_PALLET_COLOR_GREY,
	VCA5_PALLET_COLOR_BLUE,
	VCA5_PALLET_COLOR_GREEN,
	VCA5_PALLET_COLOR_CYAN,
	VCA5_PALLET_COLOR_RED,
	VCA5_PALLET_COLOR_MAGENTA,
	VCA5_PALLET_COLOR_YELLOW,
	VCA5_PALLET_COLOR_WHITE,
	VCA5_COLOR_MAX,
};



typedef struct _VCA5_TIME_STRUCT {
	unsigned long		ulSec;
	unsigned long		ulMSec;
} VCA5_TIME_STRUCT;

typedef struct _VCA5_TIMESTAMP {
	unsigned long		ulLowDateTime;
	unsigned long		ulHighDateTime;
} VCA5_TIMESTAMP;

typedef struct _VCA5_TAMPER_INFO {
	unsigned long		ulEnabled;
	unsigned long		ulAlarmTimeout;
	unsigned long		ulAreaThreshold;
	unsigned long		ulFiringThreshold;
	unsigned long		ulLowLightEnabled;
} VCA5_TAMPER_INFO;

typedef struct _VCA5_ZONE {
	unsigned short		usZoneId;	
	unsigned short		usZoneType;		
	unsigned short		usZoneStyle;
	unsigned long		ulTotalPoints;
	VCA5_POINT			pPoints[VCA5_MAX_NUM_ZONE_POINTS];
	char				szZoneName[VCA5_MAX_STR_LEN];
} VCA5_ZONE;

typedef struct _VCA5_RULE_TIMER {
	unsigned long		ulTimeThreshold;
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usFlags;			// 16-bit, extra flags for rules
} VCA5_RULE_TIMER;

typedef struct _VCA5_RULE_DIRECTION {
	short				sStartAngle;		
	short				sFinishAngle;	
} VCA5_RULE_DIRECTION;

typedef struct _VCA5_RULE_SPEED {
	unsigned short		usSpeedLo;
	unsigned short		usSpeedUp;
} VCA5_RULE_SPEED;

typedef struct _VCA5_RULE_AREA {
	unsigned short		usAreaLo;
	unsigned short		usAreaUp;
} VCA5_RULE_AREA;

typedef struct _VCA5_RULE_LINECOUNTER {
	unsigned long		ulCalibrationWidth;	
	unsigned long		ulShadowFilterEnabled;
} VCA5_RULE_LINECOUNTER;


typedef struct _VCA5_RULE_COLSIG {
	unsigned short		usColBin;
	unsigned short		usThreshold;
} VCA5_RULE_COLSIG;

typedef struct _VCA5_RULE_RESERVED {
	unsigned char		ucData[256];			// To keep enough space in the union to accommodate future changes
} VCA5_RULE_RESERVED;

typedef struct _VCA5_RULE {
	unsigned long		ulRuleId;			
	unsigned short		usRuleType;
	unsigned short		usTotalTrkObjs;			
	unsigned char		ucTrkObjs[VCA5_MAX_NUM_CLSOBJECTS+2];
			
	unsigned short		usZoneId;	
	union RULE_DATA {
		VCA5_RULE_TIMER			tTimer;
		VCA5_RULE_DIRECTION		tDirection;
		VCA5_RULE_SPEED			tSpeed;
		VCA5_RULE_AREA			tArea;
		VCA5_RULE_LINECOUNTER	tLineCounter;
		VCA5_RULE_COLSIG		tColorFilter;
		VCA5_RULE_RESERVED		tReserved;
	} tRuleDataEx;
	
	char				szRuleName[VCA5_MAX_STR_LEN];
	unsigned long		reserved[VCA5_MAX_RESERVED_RULE];
} VCA5_RULE;

typedef struct _VCA5_SUBCOUNTER {
	unsigned short		usSubCounterType;
	unsigned short		usTrigId;				
} VCA5_SUBCOUNTER;

typedef struct _VCA5_COUNTER {
	unsigned short		usCounterId;
	unsigned short		usNumSubCounters;
	VCA5_SUBCOUNTER		pSubCounters[VCA5_MAX_NUM_SUBCOUNTERS];
	char				szCounterName[VCA5_MAX_STR_LEN];
} VCA5_COUNTER;

typedef enum _VCA5_CALIB_HEIGHT_UNITS {
	VCA5_HEIGHT_UNITS_METERS	= 0x0000,
	VCA5_HEIGHT_UNITS_FEET		= 0x0001

} VCA5_CALIB_HEIGHT_UNITS;

typedef enum _VCA5_CALIB_SPEED_UNITS {
	VCA5_SPEED_UNITS_KPH		= 0x0000,
	VCA5_SPEED_UNITS_MPH		= 0x0001

} VCA5_CALIB_SPEED_UNITS;

typedef struct _VCA5_CALIB_INFO {
	float 				fHeight;
	float 				fTilt;
	float 				fRoll;
	float 				fFOV;
	int					calibrationStatus;
	unsigned long		ulHeightUnits;
	unsigned long		ulSpeedUnits;
} VCA5_CALIB_INFO;

typedef struct _VCA5_CLSOBJECT {
	short				sClsObjectId;			
	VCA5_RULE_AREA		tAreaSetting;			
	VCA5_RULE_SPEED		tSpeedSetting;
	char				szClsobjectName[VCA5_MAX_STR_LEN];
} VCA5_CLSOBJECT;

typedef struct _VCA5_TRACKERPARAMS {
	unsigned long	ulMinObjAreaPix;		// The minimum tracked object size
	unsigned long	ulSecsToHoldOn;			// The number of seconds to hold on to an object before ditching it
	unsigned long	bAbObjEnabled;			// Enable/disable abandoned object detection
	unsigned long	bMovObjEnabled;			// Enable/disable moving/stationary object detection
	unsigned long	bAdvTraEnabled;			// Enable/disable retail tracker, not implement
	unsigned long	bCntLineEnabled;		// Enable/disable counting line
	unsigned long	bSmokeFireEnabled;		// Enable/disable Smoke&fire
	unsigned long	ulDetectionPoint;		// Detection point of tracked objects: 0:default/1:centroid/2:midbottom
	unsigned long	reserved[VCA5_MAX_RESERVED_TRACKERPARAMS];
}VCA5_TRACKERPARAMS;


typedef struct _VCA5_SCENECHANGE_INFO{
	unsigned long			ulMode;
	VCA5_TAMPER_INFO 		tTamper;		// config when using tamper (manual mode)
	unsigned long			ulDNColorEnabled;// Enable/disable IR cut filter detection (color - grayscale transition)
	unsigned long			ulDNRimEnabled;	// Enable/disable IR cut filter detection (filter rim movement)
} VCA5_SCENECHANGE_INFO;


typedef struct _VCA5_HOG_DETECTOR_INFO{
	float	fHitThreshold;
	float	fScale;
	float	fRatio;
	int		iGroupThreshold;
	
	int		min_target_size_w;
	int		max_target_size_w;
} VCA5_HOG_DETECTOR_INFO;



extern "C" {
VCA_BOOL VCA_API_PREFIX VCA5CreateInstance(void** pUnknown) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Init(unsigned long ulVersion) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5GetLastErrorCode(VCA5_ERROR_CODE_ITEM* pEcode) VCA_API_POSTFIX;
VCA_BOOL VCA_API_PREFIX VCA5QueryInfo(unsigned long ulQueryInfoCmd, unsigned long ulIn, void* pOut) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Activate(unsigned long ulLicenseCnt, VCA5_LICENSE_INFO* pVCA5LicenseInfos) VCA_API_POSTFIX;
VCA_BOOL VCA_API_PREFIX VCA5GetSystemInfo(VCA5_SYSTEM_INFO* pVCA5SystemInfo) VCA_API_POSTFIX;
VCA_BOOL VCA_API_PREFIX VCA5GetEngineInfo(unsigned long ulEngId, VCA5_ENGINE_INFO* pEngineInfo) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Open(unsigned long ulEngId, VCA5_ENGINE_PARAMS* pEngineParam) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Process(unsigned long   ulEngId, 
                                    unsigned char*  pImage,
                                    VCA5_TIMESTAMP* pTimestamp,
                                    unsigned long*  uiLengthResult,
                                    unsigned char*  pResult) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Control(unsigned long   ulEngId,
                                    unsigned long   ulVCA5Cmd,
                                    VCA_ULONGPTR   ulParam0 = 0,
                                    VCA_ULONGPTR   ulParam1 = 0,
                                    VCA_ULONGPTR   ulParam2 = 0,
                                    VCA_ULONGPTR   ulParam3 = 0) VCA_API_POSTFIX;

VCA_BOOL VCA_API_PREFIX VCA5Close(unsigned long ulEngId) VCA_API_POSTFIX;

} // extern "C"


DEFINE_GUID( IID_IVCA5, 0x48c2cd0, 0x4685, 0x43a8, 0xbf, 0x4d, 0xbd, 0xf3, 0xbc, 0xa1, 0xe9, 0x4f);
struct IVCA5 : IUnknown 
{
	virtual VCA_BOOL STDMETHODCALLTYPE VCA5GetLastErrorCode(VCA5_ERROR_CODE_ITEM *pEcode) = 0;
	virtual VCA_BOOL STDMETHODCALLTYPE VCA5QueryInfo(unsigned long ulQueryInfoCmd, unsigned long ulIn, void *pOut) = 0;

	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Activate(unsigned long ulLicenseCnt, VCA5_LICENSE_INFO *pVCA5LicenseInfos) = 0;
	virtual VCA_BOOL STDMETHODCALLTYPE VCA5GetSystemInfo(VCA5_SYSTEM_INFO* pVCA5SystemInfo) = 0;
	virtual VCA_BOOL STDMETHODCALLTYPE VCA5GetEngineInfo(unsigned long ulEngId, VCA5_ENGINE_INFO* pEngineInfo) = 0;
	
	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Open(unsigned long ulEngId, VCA5_ENGINE_PARAMS* pEngineParam) = 0;

	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Process(unsigned long   ulEngId, 
                                                   unsigned char*  pImage,
                                                   VCA5_TIMESTAMP* pTimestamp,
                                                   unsigned long*  uiLengthResult,
                                                   unsigned char*  pResult) = 0;

	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Control(unsigned long   ulEngId,
                                                   unsigned long   ulVCA5Cmd,
                                                   VCA_ULONGPTR    ulParam0 = 0,
                                                   VCA_ULONGPTR    ulParam1 = 0,
                                                   VCA_ULONGPTR    ulParam2 = 0,
                                                   VCA_ULONGPTR    ulParam3 = 0) = 0;

	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Close(unsigned long ulEngId) = 0;	

	virtual VCA_BOOL STDMETHODCALLTYPE VCA5Init(unsigned long ulVersion) = 0;
};


#pragma pack( pop, VCA5CORELIB_H  )

#endif	//__VCA5_CORE_LIB_H__
