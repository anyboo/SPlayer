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

/*! \file	vca_trackerdef.h
 *  \brief	The VCA tracker include file for the tracker definitions.
 */
#ifndef _vca_trackerdef_h_
#define _vca_trackerdef_h_

#include "vca_commondef.h"

typedef enum
{
	fEVENTSBUF_INVALID	= 0x00000000,
	fEVENTSBUF_ALARMS	= 0x00000001,
	fEVENTSBUF_COUNTERS	= 0x00000002,
	fEVENTSBUF_PROCESSED	= 0x10000000,
}
VCA_EVENTSBUF_STATUS_E;

// VCA Tracker params
typedef struct
{
	unsigned int	uiMinObjAreaPix;		// The minimum tracked object size
	unsigned int	uiSecsToHoldOn;			// The number of seconds to hold on to an object before ditching it
	unsigned int	bAbObjEnabled;			// Enable/disable abandoned object detection
	unsigned int	bMovObjEnabled;			// Enable/disable moving/stationary object detection
	unsigned int 	bAdvTraEnabled;			// Enable/disable retail tracker
	unsigned int 	bCntLineEnabled;		// Enable/disable counting line
	unsigned int	bSmokeFireEnabled;		// Enable/disable smoke&fire
	unsigned int	uiDetectionPoint;		// Detection point of tracked objects: 0:default/1:centroid/2:midbottom
}
VCA_TRACKER_PARAMS_T;

typedef struct
{
	unsigned int	uiStatus;				// VCA_EVENTSBUF_STATUS_E
	unsigned int	uiNumAlarms;
	unsigned int	uiNumCounters;
	unsigned int	uiAlarmBufSize;
	unsigned int	uiCounterBufSize;
	unsigned char	pBuffer[1];
}
VCA_EVENTSBUF_T;

/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/

typedef VCA_TRACKER_PARAMS_T VCA_TRACKER_PARAMS_DSP_T;

#endif // _vca_trackerdef_h_
