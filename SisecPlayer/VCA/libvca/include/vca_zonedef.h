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

/*! \file	vca_zonedef.h
 *  \brief	The VCA tracker include file for the zone definitions.
 */
#ifndef _vca_zonedef_h_
#define _vca_zonedef_h_

#include "vca_commondef.h"

/*
	Frig for temp advanced rule, including new double-zone trigger & setting logic between rules to either logic OR or logic AND.
	In order to enable it, please uncomment the following line and rebuilding everthing (codec,apps,activex
*/
//#define TMP_ADVANCED_RULE

#define MAX_NUM_ZONE_POINTS		40
#define MAX_NUM_ZONES			40

#define DEFAULT_ZONE_WIDTH	GETPERCENT(20)	// 20%
#define DEFAULT_ZONE_HEIGHT 	GETPERCENT(20)	// 20%

#define DEFAULT_COUNTER_WIDTH	GETPERCENT(10)	// 10%
#define DEFAULT_COUNTER_HEIGHT	GETPERCENT(10)	// 10%

// All Zone Verfication Macros
//static inline int IsZoneIdValid(int id) { return (( id >= 0) && (id < MAX_NUM_ZONES)); }
#define IsZoneIdValid(id)	(( id >= 0) && (id < MAX_NUM_ZONES))
#define IsNodeIdValid(id)	(( id >= 0) && (id < MAX_NUM_ZONE_POINTS))
#define IsZoneExported(pZone)	((pZone)->ucExported == 1)
#define IsZoneUsed(pZone)	((pZone)->ucUsed == 1)

#define IsZonePolygon(pZone)	((pZone)->usZoneStyle & VCA_ZONE_POLYGON)
#define IsZoneTripwire(pZone)	((pZone)->usZoneStyle & VCA_ZONE_TRIPWIRE)
#ifdef TMP_ADVANCED_RULE
#define IsZoneMultiwire(pZone)	(IsZoneIdValid((pZone)->usMultiWireId))
#endif

// All point values are normalized to 16bits, which is 0-65535
typedef struct
{
	unsigned short	x;		// 16-bit
	unsigned short	y;		// 16-bit
}
VCA_POINT_T;

typedef enum
{
	VCA_ALARM_ZONE		= 0x0001,
	VCA_PREALARM_ZONE,
	VCA_NONDETECTION_ZONE
}
VCA_ZONE_TYPE_E;

typedef enum
{
	VCA_ZONE_TRIPWIRE	= 0x0001,
	VCA_ZONE_POLYGON	= 0x0002,
	MD_ZONE_RECTANGLE	= 0x0004,
}
VCA_ZONE_STYLE_E;

typedef struct
{
	unsigned short		usZoneId;			// [0, MAX_NUM_ZONES-1]
	unsigned short		usZoneType;			// VCA_ZONE_TYPE_E
	unsigned short		usZoneStyle;			// VCA_ZONE_STYLE_E
	char			strZoneName[MAX_NUM_NAME];	// Zone Name
	unsigned int		uiColor;			// RGB color
	unsigned char		ucEnable;			// Zone enabled?
	unsigned char		ucDisplay;			// Zone visable?
	unsigned int		uiTotalPoints;			// [0, MAX_NUM_ZONE_POINTS-1]
	VCA_POINT_T		pPoints[MAX_NUM_ZONE_POINTS];	// Zone points
#ifdef TMP_ADVANCED_RULE
	unsigned short		usMultiWireId;
	unsigned char		ucZoneRuleLogic;
#endif
// remaining part is only used by ActiveX
	unsigned char		ucStatus;			// VCA_AREA_STATUS_E
	unsigned char		ucUsed;
	unsigned char		ucEventChanged;
// UID and user data
	unsigned int		uiUID;				// UID
	char			strUserData[MAX_NUM_NAME];	// User data
}
VCA_ZONE_T;

typedef struct
{
	unsigned int		uiTotalZones;
	VCA_ZONE_T		pZones[MAX_NUM_ZONES];
}
VCA_ZONES_T;

/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/
typedef struct
{
	unsigned short		usZoneId;			// 16-bit
	unsigned short		usZoneType;			// 16-bit
	unsigned short		usZoneStyle;			// 16-bit
	unsigned short		usPadding;			// 16-bit
	char			strZoneName[MAX_NUM_NAME];	// Zone Name
	unsigned int		uiTotalPoints;			// 32-bit
	VCA_POINT_T		pPoints[MAX_NUM_ZONE_POINTS];	// 32-bit * MAX_NUM_ZONE_POINTS
#ifdef TMP_ADVANCED_RULE
	unsigned short		usMultiWireId;			// 16-bit
	unsigned short		usZoneRuleLogic;		// 16-bit
#endif
}
VCA_ZONE_DSP_T;

typedef struct
{
	unsigned int		uiTotalZones;			// 32-bit
	VCA_ZONE_DSP_T		pZones[MAX_NUM_ZONES];
}
VCA_ZONES_DSP_T;

#endif // _vca_zonedef_h_

