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

/*! \file	vca_ruledef.h
 *  \brief	The VCA tracker include file for the rule definitions.
 */
#ifndef _vca_rule_h_
#define _vca_rule_h_

#define MAX_NUM_CLSOBJECTS		20
#define	MAX_NUM_DIRECTIONFILTERS	2
#define MAX_NUM_SPEEDFILTERS		2
#define MAX_NUM_COLSIGFILTERS		1
#define MAX_NUM_AREAFILTERS		2
#define MAX_NUM_OPTIONSPERRULE		5
#define MAX_RULEDATA_SIZE		40

#define IsRuleIdValid(id)		((id >= 0) && (id < MAX_NUM_RULESPEREVENT))
#define IsRuleUsed(pRule)		(pRule->ucUsed == 1)

#define TRKOBJ_INCLUDE			0
#define TRKOBJ_EXCLUDE			1

#define GET_TRKOBJ_LOGIC(trkobj)	((trkobj & 0xC0) >> 6)			// high 2 bits
#define GET_TRKOBJ_ID( trkobj)		(trkobj & 0x3F)				// lo 6 bits
#define MAKE_TRKOBJ(logic, id)		(((logic & 0x03) << 6) | (id & 0x3F))

#define GETRULENAME(rule)		(VCA_RULE_STR[rule])

/***************************************************************************/
/*               _    _ ______ _      _      ____  _                       */
/*              | |  | |  ____| |    | |    / __ \| |                      */
/*              | |__| | |__  | |    | |   | |  | | |                      */
/*              |  __  |  __| | |    | |   | |  | | |                      */
/*              | |  | | |____| |____| |___| |__| |_|                      */
/*              |_|  |_|______|______|______\____/(_)                      */
/*                                                                         */
/*  DO NOT CHANGE THE ORDER OF THE RULES IN THIS FILE. IT WILL BREAK STUFF */
/*                                                                         */
/*  PLEASE KEEP THE STRING TABLE VCA_RULE_STR UPDATED WITH THE ENUM        */
/*                                                                         */
/*  ADD NEW RULES BEFORE VCA_RULE_BASIC_PRESENCE (WHERE IT SAYS)           */
/***************************************************************************/

typedef enum
{
// Stock rules
	VCA_RULE_PRESENCE = 0x0001,
	VCA_RULE_ENTER,
	VCA_RULE_EXIT,
	VCA_RULE_APPEAR,
	VCA_RULE_DISAPPEAR,
	VCA_RULE_STOP,
	VCA_RULE_LOITERING,
	VCA_RULE_DIRECTIONFILTER,
	VCA_RULE_SPEED,
	VCA_RULE_AREA,
	VCA_RULE_TAILGATING,
	VCA_RULE_ABOBJ,		// EE: rule for abandoned object, obsolete
	VCA_RULE_LINECOUNTER_A,	// EE: rule for counting line (one direction)
	VCA_RULE_LINECOUNTER_B,	// EE: rule for counting line (the other direction)
	VCA_RULE_RMOBJ,		// rule for removed object, obsolete
	VCA_RULE_COLSIG,
	VCA_RULE_SMOKE,
	VCA_RULE_FIRE,
	VCA_RULE_ABRMOBJ,	// rule for abandoned/removed object
// Predefined rule attributes, could be used for custom rules when combined with other attributes

	/* ADD NEW RULES HERE - _NOT_ BEFORE */

	VCA_RULE_BASIC_PRESENCE,
	VCA_RULE_BASIC_DISPRESENCE,
	VCA_RULE_BASIC_ENTER,
	VCA_RULE_BASIC_EXIT,
	VCA_RULE_BASIC_APPEAR,
	VCA_RULE_BASIC_DISAPPEAR,
	VCA_RULE_BASIC_STATIONARY,
	VCA_RULE_BASIC_DIRECTION,
	VCA_RULE_BASIC_TIMER,
	VCA_RULE_BASIC_SPEED,
	VCA_RULE_BASIC_AREA,
	VCA_RULE_BASIC_COLSIG,
	VCA_RULE_OTHERS,
	VCA_RULE_NUM
}
VCA_RULE_E;

#if defined( _DM36X_ARCHITECTURE ) || defined( PCLINUX )
__attribute__((unused))
#endif
static const char * VCA_RULE_STR[] =
{
	"Null",
// Stock Rules
	"Presence",
	"Enter",
	"Exit",
	"Appear",
	"Disappear",
	"Stopped",
	"Dwell",
	"Direction Filter",
	"Speed Filter",
	"Area Filter",
	"Tailgating",
	"Abandoned Object",
	"Cnt Line - Dir A",
	"Cnt Line - Dir B",
	"Removed Object",
	"Color Filter",
	"Smoke Detection",
	"Fire Detection",
	"Abandoned/Removed Object",
// Basic Rules
	"Basic Presence",
	"Basic Dispresence",
	"Basic Enter",
	"Basic Exit",
	"Basic Appear",
	"Basic Disappear",
	"Basic Stationary",
	"Basic Direction",
	"Basic Timer",
	"Basic Speed",
	"Basic Area",
	"Basic Color Filter",
	"Others"
};

// Rule type messages for both metadata and TCP event messages
static const char * VCA_RULETYPE[] =
{
	"null",
// Stock Rules
	"presence",
	"enter",
	"exit",
	"appear",
	"disappear",
	"stop",
	"dwell",
	"direction",
	"speed",
	"area",
	"tailgating",
	"abobj",
	"linecountera",
	"linecounterb",
	"rmobj",
	"colsig",
	"smoke",
	"fire",
	"abrmobj"
};

#define VCA_RULE_PRESENCE_STRING		"Presence"
#define VCA_RULE_ENTER_STRING			"Enter"
#define VCA_RULE_EXIT_STRING			"Exit"
#define VCA_RULE_APPEAR_STRING			"Appear"
#define VCA_RULE_DISAPPEAR_STRING		"Disappear"
#define VCA_RULE_STOP_STRING			"Stopped"
#define VCA_RULE_LOITERING_STRING		"Dwell"
#define VCA_RULE_DIRECTIONFILTER_STRING		"Direction Filter"
#define VCA_RULE_SPEED_STRING			"Speed Filter"
#define VCA_RULE_AREA_STRING			"Area Filter"
#define VCA_RULE_TAILGATING_STRING		"Tailgating"
#define VCA_RULE_ABOBJ_STRING			"Abandoned Object"
#define VCA_RULE_LINECOUNTER_A_STRING		"Cnt Line - Dir A"
#define VCA_RULE_LINECOUNTER_B_STRING		"Cnt Line - Dir B"
#define	VCA_RULE_RMOBJ_STRING			"Removed Object"
#define VCA_RULE_COLSIG_STRING			"Color Filter"
#define VCA_RULE_SMOKE_STRING			"Smoke Detection"
#define VCA_RULE_FIRE_STRING			"Fire Detection"
#define VCA_RULE_ABRMOBJ_STRING			"Abandoned/Removed Object"
#define VCA_RULE_BASIC_PRESENCE_STRING		"Basic Presence"
#define VCA_RULE_BASIC_DISPRESENCE_STRING	"Basic Dispresence"
#define VCA_RULE_BASIC_ENTER_STRING		"Basic Enter"
#define VCA_RULE_BASIC_EXIT_STRING		"Basic Exit"
#define VCA_RULE_BASIC_APPEAR_STRING		"Basic Appear"
#define VCA_RULE_BASIC_DISAPPEAR_STRING		"Basic Disappear"
#define VCA_RULE_BASIC_STATIONARY_STRING	"Basic Stationary"
#define VCA_RULE_BASIC_DIRECTION_STRING		"Basic Direction"
#define VCA_RULE_BASIC_TIMER_STRING		"Basic Timer"
#define VCA_RULE_BASIC_SPEED_STRING		"Basic Speed"
#define VCA_RULE_BASIC_AREA_STRING		"Basic Area"
#define	VCA_RULE_BASIC_COLSIG_STRING		"Basic Color Filter"
#define VCA_RULE_OTHERS_STRING			"Others"


typedef struct
{
	unsigned short		usZoneId;	// 16-bit
	unsigned short		usZoneExt;	// 16-bit
}
VCA_RULE_BASIC_T;

/**************************************************************************
	LINECOUNTER supports

	Stock rules:
	VCA_RULE_LINECOUNTER,
 **************************************************************************/
typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiCalibrationWidth;		// 32-bit
	unsigned int		uiShadowFilterEnabled;		// 32-bit

}
VCA_RULE_LINECOUNTER_T;

/**************************************************************************
	DIRECTIONFILTER supports

	Stock rules:
	VCA_RULE_DIRECTIONFILTER,

	Basic primitives:
	VCA_RULE_BASIC_DIRECTION,
 **************************************************************************/
typedef struct
{
	short int		startangle;	// 16-bit, accuracy is 0.1 degree, for 180degree, value is 1800
	short int		finishangle;	// 16-bit
}
VCA_RULE_DIRECTIONFILTER_T;

typedef struct
{
	unsigned short			usZoneId;		// 16-bit
	unsigned short			usZoneExt;		// 16-bit
	unsigned int			uiTotalFilters;		// 32-bit
	VCA_RULE_DIRECTIONFILTER_T	pDirections[MAX_NUM_DIRECTIONFILTERS];
}
VCA_RULE_DIRECTIONFILTERS_T;

/**************************************************************************
	COLSIGFILTER supports 

	Stock rules:
	VCA_RULE_COLSIG,

	Basic primitives:
	VCA_RULE_BASIC_COLSIG

 **************************************************************************/

typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	unsigned short		usColBin; 			
	unsigned short		usThres;
}
VCA_RULE_COLSIG_T;

#define VCA_RULE_TAILGATING_BYTRACKER		0x0000		// trigger tailgating rule by tracker
#define VCA_RULE_TAILGATING_BYLINECOUNTER	0x0001		// trigger tailgating rule by linecounter
/**************************************************************************
	TIMER supports

	Stock rules:
	VCA_RULE_STOP,
	VCA_RULE_LOITERING,
	VCA_RULE_ABOBJ,
	VCA_RULE_TAILGATING,

	Basic primitives:
	VCA_RULE_BASIC_TIMER,
 **************************************************************************/
typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usFlags;			// 16-bit, extra flags for rules
	unsigned int		uiTimeThreshold;		// 32-bit, Seconds
}
VCA_RULE_TIMER_T;


/**************************************************************************
	SPEED supports

	Stock rules:
	VCA_RULE_SPEED,

	Basic primitives:
	VCA_RULE_BASIC_SPEED,
 **************************************************************************/
typedef struct
{
	double			fSpeedLo; 			// 64-bit, lower bound speed (in kph or mph)
	double			fSpeedUp;			// 64-bit, upper bound speed (in kph or mph)
}
VCA_RULE_SPEED_T;

typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	VCA_RULE_SPEED_T	pSpeeds[MAX_NUM_SPEEDFILTERS];	// MAX_NUM_SPEEDFILTERS * 32-bit
}
VCA_RULE_SPEEDS_T;

// DSP structure below

typedef struct
{
	unsigned short		usSpeedLo; 			// 16-bit, lower bound speed (in kph or mph)
	unsigned short		usSpeedUp;			// 16-bit, upper bound speed (in kph or mph)
}
VCA_RULE_SPEED_DSP_T;

typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	VCA_RULE_SPEED_DSP_T	pSpeeds[MAX_NUM_SPEEDFILTERS];	// MAX_NUM_SPEEDFILTERS * 32-bit
}
VCA_RULE_SPEEDS_DSP_T;

/**************************************************************************
	AREA supports

	Stock rules:
	VCA_RULE_AREA,

	Basic primitives:
	VCA_RULE_BASIC_AREA,
 **************************************************************************/
typedef struct
{
	double			fAreaLo; 			// 64-bit, lower bound area (in 1/10 sq meters or sq feet)
	double			fAreaUp;			// 64-bit, upper bound area (in 1/10 sq meters or sq feet)
}
VCA_RULE_AREA_T;

typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	VCA_RULE_AREA_T		pAreas[MAX_NUM_AREAFILTERS];	// MAX_NUM_AREAFILTERS * 32-bit
}
VCA_RULE_AREAS_T;

// DSP structure below

typedef struct
{
	unsigned short		usAreaLo; 			// 16-bit, lower bound area (in 1/10 sq meters or sq feet)
	unsigned short		usAreaUp;			// 16-bit, upper bound area (in 1/10 sq meters or sq feet)
}
VCA_RULE_AREA_DSP_T;

typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneExt;			// 16-bit
	unsigned int		uiTotalFilters;			// 32-bit
	VCA_RULE_AREA_DSP_T	pAreas[MAX_NUM_AREAFILTERS];	// MAX_NUM_AREAFILTERS * 32-bit
}
VCA_RULE_AREAS_DSP_T;



/*************************************************************************/
// Rules structure for AxCtrl & Linux arm application
typedef struct
{
	VCA_RULE_E		eRuleType;
	short int		iTotalTrkObjs;
	unsigned char		ucTrkObjs[MAX_NUM_CLSOBJECTS+2];	// high bits are logic and low bits are id
	unsigned char		ucUsed;
	unsigned int		uiRuleDataSize;
	char			pRuleData[MAX_RULEDATA_SIZE];
	unsigned char	ucWidthCalibrationEnabled;					// for enable/disable width calibration on counting line
	unsigned char	ucShadowFilterEnabled;						// for enable/disable shadow filter on counting line
}
VCA_RULE_T;

/*************************************************************************/
// Rules structure for DSP, 32-bit aligment applied
typedef struct
{
	short int		iRuleType;				// 16-bit
	short int		iTotalTrkObjs;				// 16-bit
	unsigned char		ucTrkObjs[MAX_NUM_CLSOBJECTS+2];	// 8-bit * 22 = 176-bit
	union RULE_DATA
	{
		VCA_RULE_BASIC_T		tBasic;
		VCA_RULE_DIRECTIONFILTERS_T	tDirections;
		VCA_RULE_TIMER_T		tTimer;
		VCA_RULE_SPEEDS_DSP_T		tSpeeds;
		VCA_RULE_AREAS_DSP_T		tAreas;
		VCA_RULE_LINECOUNTER_T		tLineCounter;
		VCA_RULE_COLSIG_T		tColSig;
	}tRuleData;
}
VCA_RULE_DSP_T;

#endif // _vca_rule_h_
