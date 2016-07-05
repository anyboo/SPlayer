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

/*! \file	vca_countingdef.h
 *  \brief	The VCA tracker include file for the counting definitions.
 */
#ifndef _vca_countingdef_h_
#define _vca_countingdef_h_

#include "vca_commondef.h"
#include "vca_eventdef.h"
#include "vca_zonedef.h"

/**************************************************************************
	COUNTING supports:

	Basic idea of counter:

	Counter is triggered by some predefined or custom defined events
	(each event is a combination of rules)

*************************************************************************/
#define MAX_NUM_COUNTERS		20
#define MAX_NUM_SUBCOUNTERS		20
#define MAX_TYPE_COUNTERS		5

// All Counter Verfication Macros
//#define	IsCounterIdValid(id )			((id >= 0) && ( id < MAX_NUM_COUNTERS))
static inline int IsCounterIdValid(int id) { return ((id >= 0) && ( id < MAX_NUM_COUNTERS)); }
#define	IsSubCounterIdValid(id )		((id >= 0) && ( id < MAX_NUM_SUBCOUNTERS))
#define IsCounterUsed(pCounter )		((pCounter)->ucUsed == 1)
#define IsSubCounterUsed(pSubCounter )		((pSubCounter)->ucUsed == 1)
#define IsSubCounterTicked(pSubCounter )	((pSubCounter)->ucTicked == 1)
#define IsCounterExported(pCounter)		((pCounter)->ucExported == 1)


typedef enum
{
	VCA_COUNTER_INCREMENT	= 0x0001,
	VCA_COUNTER_DECREMENT	= 0x0002,
	VCA_COUNTER_AVERAGE	= 0x0004,
	VCA_COUNTER_INSTANT	= 0x0008,
}
VCA_COUNTER_TYPE_E;

typedef struct
{
	unsigned short		usSubCounterId;
	unsigned short		usSubCounterType;	// VCA_COUNTER_TYPE_E
	unsigned short		usTrigId;		// EventId
	unsigned char		ucEnable;
// remaining part is only used by ActiveX
	unsigned char		ucUsed;
	unsigned char		ucIsNewCounter;
	unsigned char		ucTicked;
}
VCA_SUBCOUNTER_T;

typedef struct
{
	unsigned short		usCounterId;				// [0, MAX_NUM_COUNTERS-1]
	char			strCounterName[MAX_NUM_NAME];		// CounterName
	unsigned char		ucEnable;				// Enable counter
	unsigned int		uiColor;				// RGB color
	unsigned char		ucDisplay;				// Show counter?
	unsigned short		usNumSubCounters;			// [0, MAX_NUM_SUBCOUNTERS-1]
	VCA_POINT_T		pPoints[5];				// Region to display counter result
	VCA_SUBCOUNTER_T	pSubCounters[MAX_NUM_SUBCOUNTERS];
	unsigned char		ucUsed;
// remaining part is only used by ActiveX
	unsigned char		ucStatus;				// VCA_AREA_STATUS_E
	unsigned char		ucNewCounter;
	int			iCounterResult;				// Integrate counterresult into counter, which will be very useful for ActiveX
// UID and user data
	unsigned int		uiUID;					// UID
	char			strUserData[MAX_NUM_NAME];		// User data
}
VCA_COUNTER_T;

typedef struct
{
	unsigned int		uiTotalCounters;
	VCA_COUNTER_T		pCounters[MAX_NUM_COUNTERS];
}
VCA_COUNTERS_T;

/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/
typedef struct
{
	unsigned short		usSubCounterType;	// 16-bit
	unsigned short		usTrigId;		// 16-bit
}
VCA_SUBCOUNTER_DSP_T;

typedef struct
{
	unsigned short		usCounterId;				// 16-bit
	unsigned short		usNumSubCounters;			// 16-bit
	unsigned int		uiColor;				// rgb, 32-bit
	char			strCounterName[MAX_NUM_NAME];		// 32-bit
	unsigned short		usLeft;					// 16-bit
	unsigned short		usTop;					// 16-bit
	unsigned short		usWidth;				// 16-bit
	unsigned short		usHeight;				// 16-bit
	unsigned short		usColorIdx;				// 16-bit lookup into color table on DSP
	VCA_SUBCOUNTER_DSP_T	pSubCounters[MAX_NUM_SUBCOUNTERS];	// 32-bit * MAX_NUM_SUBCOUNTERS
}
VCA_COUNTER_DSP_T;

typedef struct
{
	unsigned int		uiTotalCounters;		// 32-bit
	VCA_COUNTER_DSP_T	pCounters[MAX_NUM_COUNTERS];
}
VCA_COUNTERS_DSP_T;

/****************************************************************************/
// Structure for saving counting result
/****************************************************************************/
typedef struct
{
	unsigned short		usCounterId;
	unsigned short		usPadding;
	char			strCounterName[MAX_NUM_NAME];	// 32-bytes
	int			iCounterResult;
}
VCA_COUNTER_OUTPUT_T;

#endif // _vca_countingdef_h_
