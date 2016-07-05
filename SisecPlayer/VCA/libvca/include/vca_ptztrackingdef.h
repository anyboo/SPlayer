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

/*! \file	vca_ptztrackingdef.h
 *  \brief	The VCA tracker include file for the PTZ tracking definitions.
 */
#ifndef _vca_ptztrackingdef_h
#define _vca_ptztrackingdef_h

#include "vca_zonedef.h"	// VCA_POINT_T

// default values for auto-tracker
#define DEFAULT_AUTOTRACKER_MAXTRACKINGTIME	60
#define DEFAULT_AUTOTRACKER_OVERRIDETIMEOUT	30
#define DEFAULT_AUTOTRACKER_HOLDONTIME		5	
#define MAX_AUTOTRACKER_TRIGGERRULES		10
#define MAX_AUTOTRACKER_BBPOINTS		5

#define MAX_NUM_STARTUP_CMDS			50
#define MAX_LENGTH_STARTUP_CMD			128
#define MAX_NUM_PTZT_PROFILES			20
#define MAX_LENGTH_PROFILE_FILENAME		32
#define NUM_VCA_PTZCALIB_STATUS			13

typedef enum
{
	VCA_TRACKING_STOPPED = 0,
	VCA_TRACKING_RUNNING
}
VCA_TRACKING_STATUS_E;

typedef enum
{
	VCA_AUTOTRACKER_LAYON_NONE = 0,
	VCA_AUTOTRACKER_LAYON_ID,
	VCA_AUTOTRACKER_LAYON_BOX
}
VCA_AUTOTRACKER_LAYONTYPE_E;

typedef struct
{
	int		nEnable;			// Auto-tracker on/off
	int		nStatus;			// Auto-tracker status stopped(0), runnning(1)
	unsigned int	uiMaxTrackingTime;		// Max auto-tracking time (seconds)
	int		nOverride;			// Inhibit auto-tracking when user sends a manual PTZ command
	unsigned int	uiOverrideTimeout;		// Timeout for manual override (seconds)
	unsigned int	uiHoldonTime;			// Maximum hold-on time when target is lost or becomes stationary (seconds)
	int		nTriggerAnything;		// Trigger by any target get tracked
	int		iTriggerRuleIds[MAX_AUTOTRACKER_TRIGGERRULES];	// The list of rule ids that trigger the auto-tracking
/*
	int		nManualLayon;			// Manual lay-on type
	union 
	{
		unsigned int	uiId;					// when nManualLayon == VCA_AUTOTRACKER_LAYON_ID
		VCA_POINT_T	pPoints[MAX_AUTOTRACKER_BBPOINTS];	// when nManualLayon == VCA_AUTOTRACKER_LAYON_BOX
	}uLayonSettings;
*/
	char 			szProfile[MAX_LENGTH_PROFILE_FILENAME];		// PTZ profile name
}
VCA_PTZT_AUTOTRACKER_T;

typedef struct
{
	int			nEnable;		// PTZ support on/off
	int			nHomePreset;		// PTZ preset number for VCA home position
	int			nHomeTrackingOnly;	// Only do VCA at home position
	int			nReturn2Home;		// Return to home position if no user PTZ overrides
	unsigned int		uiReturnTimeout;	// timeout (seconds)
	VCA_PTZT_AUTOTRACKER_T	tAUTOTRACKERinfo;	// VCA Auto-tracker info
}
VCA_PTZT_T;

/// command from autotracking module for controlling ptz 
typedef struct _VCAT_PTZ_DATA
{
	float	pan;
	float	tilt;
	float	zoom;
}
VCA_PTZT_DATA;

typedef enum _VCA_PTZT_CMD
{
	VCA_PTZT_CMD_DISABLED = 0,
	VCA_PTZT_CMD_SPEED,
	VCA_PTZT_CMD_ABSPOS,
	VCA_PTZT_CMD_GOTOHOME,
	VCA_PTZT_CMD_RELPOS,
	VCA_PTZT_CMD_WAITINGABSPOS
}
VCA_PTZT_CMD;

typedef struct _VCA_PTZT_AUTOTRACK_CMD
{
	VCA_PTZT_CMD	cmd;
	VCA_PTZT_DATA	ptzData;
}
VCA_PTZT_AUTOTRACK_CMD;


// BW: Pls update NUM_VCA_PTZCALIB_STATUS if you add an extra one here
typedef enum
{
	VCA_PTZCALIB_PANTILTSPEEDGAIN	= 0x00000001,
	VCA_PTZCALIB_PANTILTSPEEDMAX	= 0x00000002,
	VCA_PTZCALIB_PANTILTSPEEDMIN	= 0x00000004,
	VCA_PTZCALIB_ZOOMTIMEGAIN	= 0x00000008,
	VCA_PTZCALIB_ZOOMSPEED		= 0x00000010,
	VCA_PTZCALIB_ZOOMSIZEMIN	= 0x00000020,
	VCA_PTZCALIB_ZOOMSIZEMAX	= 0x00000040,
	VCA_PTZCALIB_ZOOMMAX		= 0x00000080,
	VCA_PTZCALIB_ZOOMABS		= 0x00000100,
	VCA_PTZCALIB_NOMOVEZONE		= 0x00000200,
	VCA_PTZCALIB_MOVEDELAY		= 0x00000400,
	VCA_PTZCALIB_CMDDELAY		= 0x00000800,
	VCA_PTZCALIB_SUPPORTABSPOS	= 0x00001000
}
VCA_PTZCALIB_STATUS;

typedef struct
{
	float fPantiltSpeedGain;		// gain factor to control speed of pan/tilt
	float fPantiltSpeedMax;			// max pan/tilt speed   (0 - 100)
	float fPantiltSpeedMin;			// min pan/titl speed 	(0 - 100)
	float fZoomTimeGain;			// gain factor to control how long to zoom for
	float fZoomSpeed;			// zooming speed (0 - 100)
	
	float fZoomSizeMin;			// min target size before zooming in (fraction of image width/height) (0-1)
	float fZoomSizeMax;			// max target size before zooming out (fraction of image width/height) (0-1)
	float fZoomMax;				// max absolute zoom value (10x or 20x or 36x)
	int nZoomAbs;				// using absolute zoom value on API?

	float fNoMoveZone;			// percentage of screen width/height in the centre where it wont do pan/tilt (0 - 100)

	int iMoveDelay;				// time delay in frames to allow for camera movement latency 
	int iCmdDelay;				// minimum time latency in frames for adjacent commands
	
	int fSupportsAbsPos;			// does the camera support absolute position commands
}
VCA_PTZT_AUTOTRACKER_PARAMS;

typedef struct
{
	double dPantiltSpeedGain;		// gain factor to control speed of pan/tilt
	double dPantiltSpeedMax;		// max pan/tilt speed   (0 - 100)
	double dPantiltSpeedMin;		// min pan/titl speed 	(0 - 100)
	double dZoomTimeGain;			// gain factor to control how long to zoom for
	double dZoomSpeed;			// zooming speed (0 - 100)
	
	double dZoomSizeMin;			// min target size before zooming in (percentage of image width/height) (0 - 100)
	double dZoomSizeMax;			// max target size before zooming out (percentage of image width/height) (0 - 100)
	double dZoomMax;			// max absolute zoom value (10x or 20x or 36x)
	int nZoomAbs;				// using absolute zoom value on API?

	double dNoMoveZone;			// percentage of screen width/height in the centre where it wont do pan/tilt (0 - 100)

	int iMoveDelay;				// time delay in frames to allow for camera movement latency 
	int iCmdDelay;				// minimum time latency in frames for adjacent commands
	
	int fSupportsAbsPos;			// does the camera support absolute position commands
}
VCA_PTZT_AUTOTRACKER_PARAMS_D;

typedef struct
{
	unsigned int	uiTotalCmds;
	unsigned char	szCmds[MAX_NUM_STARTUP_CMDS][MAX_LENGTH_STARTUP_CMD];		
}
VCA_PTZT_STARTUP_CMDS;

typedef struct
{
	char 	szFileName[MAX_LENGTH_PROFILE_FILENAME];
	char	szName[128];
	char 	szVersion[32];
	char 	szDescription[256];
	char	szProtocol[32];				// protocol file name
	int	nModelID;
	int	fFixed;
}
VCA_PTZT_PROFILE_INFO;

typedef struct
{
	VCA_PTZT_PROFILE_INFO		tProfileInfo;
	VCA_PTZT_AUTOTRACKER_PARAMS_D	tATParams;
	VCA_PTZT_STARTUP_CMDS		tStartupCmds;
}
VCA_PTZT_PROFILE;

typedef struct
{
	unsigned int		uiTotalProfiles;
	unsigned int		uiCurProfile;
	VCA_PTZT_PROFILE	tProfiles[MAX_NUM_PTZT_PROFILES];
}
VCA_PTZT_PROFILES;

typedef struct
{
	unsigned int		uiTotalProfiles;
	VCA_PTZT_PROFILE_INFO	tProfileInfo[MAX_NUM_PTZT_PROFILES];
}
VCA_PTZT_GLOBAL_INFO;

typedef struct
{
	unsigned int		fRunning;
	unsigned int		uiProgress;
	unsigned int		uiStatus;
	char 			szProfileName[MAX_LENGTH_PROFILE_FILENAME];
	char			szLogName[MAX_LENGTH_PROFILE_FILENAME];
}
VCA_PTZ_AUTOCALIB_INFO;

#endif // _vca_ptztrackingdef_h

