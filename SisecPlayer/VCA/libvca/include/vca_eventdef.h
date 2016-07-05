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

/*! \file	vca_eventdef.h
 *  \brief	The VCA tracker include file for the event definitions.
 */
#ifndef _vca_event_h_
#define _vca_event_h_

#include "vca_ruledef.h"
#include "vca_commondef.h"

#if defined(_WIN32) || defined(PCLINUX)
#define MAX_NUM_EVENTS		(40*(VCA_RULE_BASIC_PRESENCE-1))
#else
#define MAX_NUM_EVENTS		60
#endif // defined(_WIN32) || defined(PCLINUX)

#define MAX_NUM_STAT_EVENTS	40

#define MAX_NUM_RULESPEREVENT	4

// All Event Verfication Macros
#ifdef _C64P
#define	IsEventIdValid(id)		(( id >= 0) && (id < (MAX_NUM_EVENTS+MAX_NUM_STAT_EVENTS)))
#define	IsEventIdNormal(id)		(( id >= 0) && (id < MAX_NUM_EVENTS))
#else
static inline int IsEventIdValid(int id) { return ((id >= 0) && (id < (MAX_NUM_EVENTS))); }
//#define	IsEventIdValid(id)		(( id >= 0) && (id < (MAX_NUM_EVENTS)))
#endif
#define IsEventUsed(pEvent)		((pEvent)->ucUsed == 1)
#define IsEventExported(pEvent)		((pEvent)->ucExported == 1)
#define IsEventTicked(pEvent)		((pEvent)->ucTicked)

typedef enum
{
	VCA_LOGIC_AND			= 0x0001,
	VCA_LOGIC_OR,
	VCA_LOGIC_NOR,
	VCA_LOGIC_NOT,
	VCA_LOGIC_NAND,
	VCA_LOGIC_NONE
}
VCA_LOGIC_T;

typedef enum
{
	VCA_EVENTTYPE_PREDEFINED = 0x0001,
	VCA_EVENTTYPE_CUSTOM
}
VCA_EVENTTYPE_T;

typedef struct
{
	int			iEventId;			// if EventId < 0, this event not set up yet
	char			strEventName[MAX_NUM_NAME];
	VCA_EVENTTYPE_T		eType;
	unsigned char		ucEnable;
	int			iNumRules;
	VCA_RULE_T		pRules[MAX_NUM_RULESPEREVENT];
	VCA_LOGIC_T		pLogics[MAX_NUM_RULESPEREVENT];
// remaining part is only used by ActiveX
	unsigned char		ucUsed;
	unsigned char		ucTicked;
	unsigned char		ucExpanded;
// UID and user data
	unsigned int		uiUID;				// UID
	char			strUserData[MAX_NUM_NAME];	// User data
}
VCA_EVENT_T;

typedef struct
{
	unsigned int		uiTotalEvents;
	VCA_EVENT_T		pEvents[MAX_NUM_EVENTS+MAX_NUM_STAT_EVENTS];
}
VCA_EVENTS_T;


/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/
typedef struct
{
	unsigned short	usEventId;						// 16-bit
	unsigned short	usNumRules;						// 16-bit
	char		strEventName[MAX_NUM_NAME];				// 8-bit * 32
	int		iType;							// 32-bit
	unsigned short	pLogics[MAX_NUM_RULESPEREVENT];				// pLogic[0] is set to LOGIC_AND as default
	VCA_RULE_DSP_T	pRules[MAX_NUM_RULESPEREVENT];
}
VCA_EVENT_DSP_T;

typedef struct
{
	int		uiTotalEvents;						// 32-bit
	VCA_EVENT_DSP_T	pEvents[MAX_NUM_EVENTS+MAX_NUM_STAT_EVENTS];
}
VCA_EVENTS_DSP_T;

#endif // _vca_event_h_
