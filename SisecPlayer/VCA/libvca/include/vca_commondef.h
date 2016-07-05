/*
 * Copyright © 2011 by UDP Analytics.
 *
 * All rights reserved. Property of UDP Analytics. Restricted rights to use,
 * duplicate or disclose this code are granted through contract.
 */

/*
 * Please use a tab size of 8.
 */

/*
 * This file uses the Qt style of documentation. Read about it here:
 * http://www.stack.nl/~dimitri/doxygen/manual.html
 */

/*! \file	vca_commondef.h
 *  \brief	The VCA tracker include file for the common definitions.
 */
#ifndef _VCA_COMMONDEF_
#define _VCA_COMMONDEF_

#include "vca_normmacros.h"

#define PALETTE_SIZE 		10

#define UID_RESERVED		0
#define UID_DEFAULT			1

#define DEFAULT_ALARM_DWELL	2000	// 2 seconds
#define DEFAULT_FADE_TIME	1000	// 1 second

#define MEAUNIT_METRIC		1
#define MEAUNIT_IMPERIAL	2

#define MAX_NUM_NAME		32
#define ID_ALL			4095
#define ID_INVALID		30000

#define	MAX_DIFF		GETPERCENT(2) //6
#define MAX_X_DIFF		GETPERCENT(2) //4
#define	MAX_Y_DIFF		GETPERCENT(2) //4

#define PI				3.1415926535f

#define RADIUS_ACTPT	10
#define MAXZONECOLORS	7

#define DEFAULT_PIE_HEIGHT	25
#define DEFAULT_PIE_WIDTH	25

//Pixel based arrow parameters
#define ARROW_PEN_WIDTH	2  //GETPERCENT(0.3)
#define ARROW_WIDTH		3  //GETPERCENT(0.5)
#define	ARROW_LENGTH	25 //GETPERCENT(3.8)
#define	ARC_LENGTH		25 //GETPERCENT(3.8)

#define NOT_IN_USE	-1
#define VCA_ID_AUTO	4095

#define VCA_SUCCESS	1
#define VCA_FAILED	0

#ifndef BOOL
#define BOOL	int
#define FALSE	0
#define	TRUE	1
#endif

#define VCA_EMPTY_SETTINGS		"UNDEFINED"
#define VCA_NA_STRING			"N/A"

#define VCA_DETECTIONPOINT_DEFAULT	0	// Choose centroid if calibrated overhead, midbottom if side on or uncalibrated
#define VCA_DETECTIONPOINT_CENTROID	1	// Centroid
#define VCA_DETECTIONPOINT_MIDBOTTOM	2	// Mid Bottom

// All color related macros
#define RGB2(r,g,b)	((r<<16)+(g<<8)+b)
#define ARGB2(a,r,g,b)	((a<<24)+(r<<16)+(g<<8)+b)
#define GETBVALUE(rgb)  ((BYTE) (rgb)) 
#define GETGVALUE(rgb)  ((BYTE) (((WORD) (rgb)) >> 8)) 
#define GETRVALUE(rgb)	((BYTE) ((rgb) >> 16)) 
#define GETALPHA(argb)	((BYTE) ((argb) >> 24)) 

// define all the VCA Commands for ActiveX, refer to vca_command.h for detais
#define VCA_CMD_ADD( cmd ) cmd
enum _CMD_VCA
{
	#include "vca_command.h"
};

typedef enum
{
	ANGLE_NOTDEFINED = 1,
	ANGLE_FULL,
	ANGLE_PART,
}
ANGLE_STATUS;

// Metadata format
typedef enum
{
	VCA_METAFMT_OBJECT		= 0x0001,
	VCA_METAFMT_EVENT		= 0x0002,
	VCA_METAFMT_COUNTING 		= 0x0004,
	VCA_METAFMT_BLOB		= 0x0008,
	VCA_METAFMT_HEIGHT		= 0x0010,
	VCA_METAFMT_AREA		= 0x0020,
	VCA_METAFMT_SPEED		= 0x0040,
	VCA_METAFMT_TAMPER		= 0x0080,
	VCA_METAFMT_STAB		= 0x0100,
	VCA_METAFMT_LINECOUNTER		= 0x0200,
	VCA_METAFMT_STATISTICS		= 0x0400,
	VCA_METAFMT_COLORSIG		= 0x0800,
	VCA_METAFMT_SCENECHANGE		= 0x1000,
	VCA_METAFMT_SMOKEFIRE		= 0x2000,

	VCA_METAFMT_COMPACTDATA		= 0x8000,	// Not sending empty metadata packets 

	VCA_METAFMT_DEFAULT		= 0x0277,	// What the user gets as default
	VCA_METAFMT_ALL			= 0xffff
}
VCA_METAFMT_T;

// Drawing types, using gdi+ or d3d
typedef enum
{
	DRAWING_GDIPLUS = 1,
	DRAWING_D3D		= 2,
}
DRAWING_UTIL;

// All definitions of updating data between different ActiveX using COM interface
typedef enum
{
	VCA_NO_CHANGE			= 0x00000000,
	VCA_ZONE_ADD			= 0x00000001,
	VCA_ZONE_CHANGE			= 0x00000002,
	VCA_ZONE_DEL			= 0x00000004,
	VCA_ZONE_UPDATE			= 0x00000008,

	VCA_EVENT_ADD			= 0x00000010,
	VCA_EVENT_CHANGE		= 0x00000020,
	VCA_EVENT_DEL			= 0x00000040,
	VCA_EVENT_UPDATE		= 0x00000080,
	
	VCA_COUNTER_ADD			= 0x00000100,
	VCA_COUNTER_CHANGE		= 0x00000200,
	VCA_COUNTER_DEL			= 0x00000400,
	VCA_COUNTER_UPDATE		= 0x00000800,

	VCA_COUNTEROUT_UPDATE	= 0x00001000,

	VCA_CLSOBJECT_ADD		= 0x00010000,
	VCA_CLSOBJECT_CHANGE	= 0x00020000,
	VCA_CLSOBJECT_DEL		= 0x00040000,
	VCA_CLSOBJECT_UPDATE	= 0x00080000,

	VCA_LOAD_ALL			= 0x00100000,
	VCA_SAVE_ALL			= 0x00200000,

	VCA_CALIB_UPDATE		= 0x01000000
	

}
VCA_UPDATE_STATUS;

#define ID_AXVCALIB			0x0001
#define	ID_AXSETTINGSTREE	0x0002
#define	ID_AXALARMLOG		0x0003
#define	ID_AXDATAMGR		0x0004

// Struct for saving blobmaps
typedef struct
{
	unsigned int	uiWidth;
	unsigned int	uiHeight;
	unsigned int	uiMapAge;
	unsigned char	*pBlobMap;
	unsigned char	*pCurPtr;
}
BLOBMAP;

// Struct for saving tamperinfo
typedef struct
{
	unsigned int	uAlarm;
	BLOBMAP		tampermap;
}
TAMPERINFO;

// struct for counting line events
typedef struct
{
	unsigned int	uiId;
	unsigned short usPos;
	unsigned short usWidth;
	unsigned short usNum;

} VCA_COUNTLINE_EVENT;	


typedef enum
{
	VCA_HTTPAPI_NORMAL	= 0x0000,
	VCA_HTTPAPI_LOADING	= 0x0001,
	VCA_HTTPAPI_SAVING	= 0x0002,
}
VCA_HTTPAPI_STATUS_E;

typedef enum
{
	VCA_EVT_NOTHING		= 0x00000000,
	VCA_EVT_CONFIG		= 0x00000001,
	VCA_EVT_EVENT		= 0x00000002,
	VCA_EVT_COUNTING	= 0x00000004	
}
VCA_EVT_TYPE_E;

typedef enum
{
	VCA_EVT_CFG_NOTHING		= 0x00000000,
	VCA_EVT_CFG_GENERAL		= 0x00000001,
	VCA_EVT_CFG_ZONE		= 0x00000002,
	VCA_EVT_CFG_RULE		= 0x00000004,
	VCA_EVT_CFG_COUNTER		= 0x00000008,
	VCA_EVT_CFG_CLASSIFICATION	= 0x00000010,
	VCA_EVT_CFG_CALIBRATION		= 0x00000020,
	VCA_EVT_CFG_TAMPER		= 0x00000040,
	VCA_EVT_CFG_BIA			= 0x00000080,
	VCA_EVT_CFG_ABOBJ		= 0x00000100,
	VCA_EVT_CFG_PTZTRACKING		= 0x00000200,
	VCA_EVT_CFG_SCENECHANGE		= 0x00000400
}
VCA_EVT_CONFIG_TYPE_E;

typedef enum
{
	VCA_DISPLAY_NOTHING			= 0x00000000,
	VCA_DISPLAY_BLOBS			= 0x00000001,
	VCA_DISPLAY_OBJECTS			= 0x00000002,
	VCA_DISPLAY_NON_ALARMS		= 0x00000004,
	VCA_DISPLAY_ZONES			= 0x00000008,
	VCA_DISPLAY_COUNTERS		= 0x00000010,
	VCA_DISPLAY_OBJECT_SPEED	= 0x00000020,
	VCA_DISPLAY_OBJECT_HEIGHT	= 0x00000040,
	VCA_DISPLAY_OBJECT_AREA		= 0x00000080,
	VCA_DISPLAY_OBJECT_CLASS	= 0x00000100,
	VCA_DISPLAY_OBJECT_MULTICOLOR	= 0x00000200,	// drawing objects in multiple colors
	VCA_DISPLAY_OBJECT_WAIT_TIME    = 0x00000400,
	VCA_DISPLAY_OBJECT_COLSIG	= 0x00000800,

	VCA_DISPLAY_COUNTER_NAME	= 0x00010000,
	VCA_DISPLAY_COUNTER_VALUE	= 0x00020000,

	VCA_DISPLAY_SYSTEM		= 0x10000000,

	VCA_DISPLAY_DEFAULT		= 0x10000102
}
VCA_DISPLAY_FLAGS;

// common parts for zone + counter
typedef enum
{
	VCA_AREA_HIDE		= 0x0000,
	VCA_AREA_NORMAL		= 0x0001,
	VCA_AREA_SELECTED	= 0x0002,
	VCA_AREA_NOTIFIED	= 0x0004
}
VCA_AREA_STATUS_E;

// For automatic frame rate detection
typedef enum
{
	VCA_FRAME_RATE_AUTO_DETECT	= 0x0000FFFF
}
VCA_FRAME_RATE_E;

#define IsUIDNew( pObj ) ( (pObj)->uiUID == UID_RESERVED )

#define IsAreaUsed( pArea ) ( pArea->ucUsed == 1 )
#define IsAreaSelected( pArea ) ( ( pArea->ucUsed == 1 ) && ( pArea->ucStatus & VCA_AREA_SELECTED ) )
#define IsAreaNotified( pArea ) ( ( pArea->ucUsed == 1 ) && ( pArea->ucStatus & VCA_AREA_NOTIFIED ) )

#define SetAreaStatus( pArea, status ) \
	if ( IsAreaUsed( pArea ) ) \
		pArea->ucStatus |= status;

#define ClearAreaStatus( pArea, status ) \
	if ( IsAreaUsed( pArea ) ) \
		pArea->ucStatus &= ~status;


#endif
