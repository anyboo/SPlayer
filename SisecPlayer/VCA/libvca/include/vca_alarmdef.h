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

/*! \file	vca_alarmdef.h
 *  \brief	The VCA tracker include file for the alarm definitions.
 */
#ifndef _vca_alarmdef_h_
#define _vca_alarmdef_h_

#include "vca_ruledef.h"

#define MAX_NUM_ALARMOBJECTS	200

#define MAX_NUM_EVENTTIMERS	4

#define MAX_ALARM_ZONES		5

#define TARGETID_INVALID	30000

/*
typedef enum
{
	VCA_ALARM_INTRUSION = 0x00000001,
	VCA_ALARM_STATIONARYOBJECT,
	VCA_ALARM_REMOVEDOBJECT,
	VCA_ALARM_ILLEGALPARKING
}
VCA_ALARM_TYPE_E;
*/

typedef enum
{
	VCA_ALARM_TYPE_ZONE = 1,
	VCA_ALARM_TYPE_TAMPER,
	VCA_ALARM_TYPE_STATS,
	VCA_ALARM_TYPE_AUTOTRACK,
	VCA_ALARM_TYPE_SMOKE,
	VCA_ALARM_TYPE_FIRE
}
VCA_ALARM_TYPE_E;

typedef enum
{

	VCA_ALARM_OBJECT_AVAILABLE = 0x00000001,
	VCA_ALARM_OBJECT_ACTIVE,
	VCA_ALARM_OBJECT_PREACTIVE,		// for applications which will use timers
//	VCA_ALARM_OBJECT_ALARMDWELL, 
	VCA_ALARM_OBJECT_INACTIVE,
	VCA_ALARM_OBJECT_FINISHED

}
VCA_OBJECT_STATUS_E;

typedef enum
{
	VCA_ALARM_START = 0x00000001,
	VCA_ALARM_UPDATE,
	VCA_ALARM_FINISH,
	VCA_ALARM_INSTANTANEOUS,
	VCA_ALARM_INVALID
}
VCA_ALARM_STATUS_E;

typedef enum
{
	VCA_EVENTTIMER_PRESENCE	= 0x00000001,
	VCA_EVENTTIMER_ENTER,
	VCA_EVENTTIMER_EXIT,
	VCA_EVENTTIMER_APPEAR,
	VCA_EVENTTIMER_DISAPPEAR,
	VCA_EVENTTIMER_STOP,
	VCA_EVENTTIMER_LOITERING,
	VCA_EVENTTIMER_DIRECTION,
	VCA_EVENTTIMER_STATIONARY,
	VCA_EVENTTIMER_SPEED,
	VCA_EVENTTIMER_AREA,
	VCA_EVENTTIMER_TAILGATING,
	VCA_EVENTTIMER_OTHERS,
	VCA_EVENTTIMER_AVAILABLE

}VCA_EVENTTIMER_E;

typedef struct
{
	VCA_EVENTTIMER_E	eType;
	long long		iTimerStart;
	long			iUpperThreshhold;
}
VCA_EVENTTIMER_T;

typedef struct
{
	unsigned short		usX;
	unsigned short		usY;
	unsigned short		usW;
	unsigned short		usH;
}
VCA_ALARM_POSITION_T;

typedef struct
{
	unsigned int	uiSec;
	unsigned int 	uiMSec;// uiSec refers to how many seconds since 1970-1-1, uiMSec refers to the milliseconds
}
VCA_TIME_STRUCT;

// Internal structure for alarm object
typedef struct
{
	VCA_OBJECT_STATUS_E	eStatus;
	VCA_OBJECT_STATUS_E	eStatusOld;
	unsigned int		uiEventTimerNum;
	VCA_EVENTTIMER_T	tEventTimer[MAX_NUM_EVENTTIMERS];
//	unsigned char		ucTrackerId
}
VCA_ALARM_OBJECT_INTERNAL;

// Export structure for sending back alarms through http/rtsp
typedef struct
{
	unsigned int		uiId;
	unsigned short		eAlarmStatus;
	unsigned short		usSeverity;		// reserved, not implemented yet
 	VCA_TIME_STRUCT		tStartTime;
	VCA_TIME_STRUCT		tEndTime;
	VCA_ALARM_POSITION_T	tStartPosition;
	VCA_ALARM_POSITION_T	tEndPosition;
	unsigned short		usZoneNum;
	unsigned short		usZoneId[MAX_ALARM_ZONES];
	unsigned short		usTargetId;
	unsigned short		usRealTargetId;
	short			sClsobjectId;
	unsigned short		usEventId;
	unsigned short		usFlags;		// reserved, not implemented yet
	unsigned short		eAlarmType;
}
VCA_ALARM_OBJECT_EXTERNAL;

typedef struct
{
	
	VCA_ALARM_OBJECT_INTERNAL	tAlarmInternal;
	VCA_ALARM_OBJECT_EXTERNAL	tAlarmExternal;
}
VCA_ALARM_OBJECT;


#endif // _vca_alarmdef_h_
