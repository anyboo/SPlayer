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

/*! \file	vca_cdogdef.h
 *  \brief	The VCA countdog include file for the countdog definitions.
 */
#ifndef _vca_cdogdef_h_
#define _vca_cdogdef_h_

#include "vca_commondef.h"


// VCA Tracker params
typedef struct _tagDBTable
{
	int bEnable;
	int nSampling;			// update interval
	int	nRollCount;
}VCA_COUNTDOG_TABLE_T;

#define VCA_COUNTDOG_MAX_DBTABLE	(4)
#define VCA_COUNTDOG_MAX_NAME		(64)
typedef struct _tagDBSetting
{
	int bEnable;		// 0:disable, 1:enable
	int	iStorage;		// 0:flash 1:USB 2:SD
	int	iRealStorage;	// 0:flash 1:USB 2:SD
	int	nMaxSize;		// maximum size of storage
	VCA_COUNTDOG_TABLE_T	aTable[VCA_COUNTDOG_MAX_DBTABLE];
}VCA_COUNTDOG_DB_T;

typedef struct _tagTCPQXML
{
	char 			szIP[VCA_COUNTDOG_MAX_NAME];
	unsigned int 	uPort;
	unsigned int 	uPeriod;			// report interval
	unsigned int	uHeartBeat;
}VCA_COUNTDOG_TCPQXML_T;

typedef struct _tagPushSetting
{
	int 		bEnable;			// 0:disable, 1:enable
	char		szProtocol[VCA_COUNTDOG_MAX_NAME];		// TCP/QXML, 
	VCA_COUNTDOG_TCPQXML_T	tQXML;
}VCA_COUNTDOG_PUSH_T;

typedef struct _tagVCA_COUNTDOG_INFO_T
{
	VCA_COUNTDOG_DB_T tDB;
	VCA_COUNTDOG_PUSH_T tPush;
}VCA_COUNTDOG_INFO_T;


/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/

#endif // _vca_cdogdef_h_
