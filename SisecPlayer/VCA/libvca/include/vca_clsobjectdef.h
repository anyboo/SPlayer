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

/*! \file	vca_clsobjectdef.h
 *  \brief	The VCA tracker include file for the object classification definitions.
 */
#ifndef _vca_clsobjectdef_h_
#define _vca_clsobjectdef_h_

#include "vca_commondef.h"
#include "vca_ruledef.h"

#define MAX_NUM_CLSGRPS			5
#define MAX_NUM_CLSOBJECTS		20
#define CLSOBJECT_UNKNOWN		-2	/* Not yet been classified (still figuring it out) */
#define CLSOBJECT_UNCLASSIFIED		-1	/* Does not fit into any classification class */
#define	TRKOBJECT_UNKNOWN		62
#define TRKOBJECT_UNCLASSIFIED		63

#define VCA_CLSGRP_SIDEON		0
#define VCA_CLSGRP_OVERHEAD		1

#define CLSOBJECT_UNKNOWN_NAME		"Unknown"
#define CLSOBJECT_UNCLASSIFIED_NAME	"Unclassified"
#define CLSOBJECT_NULL_NAME		"---"

// All Clsobject Verfication Macros
#define IsClsgrpIdValid( id )		( ( id >= 0 ) && ( id < MAX_NUM_CLSGRPS ) )
#define IsClsgrpUsed( pClsgrp )		( pClsgrp->ucUsed == 1 )
#define IsClsobjectIdValid( id ) 	( ( id >= CLSOBJECT_UNCLASSIFIED ) && ( id < MAX_NUM_CLSOBJECTS+1 ) )
#define IsClsobjectUsed( pClsobject ) 	( (pClsobject)->ucUsed == 1 )

typedef struct tCLSOBJECT
{
	short			sClsobjectId;			// [-2, MAX_NUM_CLSOBJECTS-1]
	char			strClsobjectName[MAX_NUM_NAME];	// Clsobject name
	unsigned char		ucEnable;			// Clsobject enabled?
	VCA_RULE_AREAS_T	tAreaSettings;			// Area definition 
	VCA_RULE_SPEEDS_T	tSpeedSettings;			// Speed definition
	unsigned char		ucUsed;
	short			sPosition;			// Position
}
VCA_CLSOBJECT_T;

typedef struct tCLSOBJECTS
{
	unsigned int		uiTotalClsobjects;
	VCA_CLSOBJECT_T		pClsobjects[MAX_NUM_CLSOBJECTS+2];	// the last one is for all other objects
}
VCA_CLSOBJECTS_T;

typedef struct tCLSGRP
{
	unsigned short		usClsgrpId;			// [0, MAX_NUM_CLSGRPS-1]
	char			strClsgrpName[MAX_NUM_NAME];	// classifcation group name
	VCA_CLSOBJECTS_T	tClsobjects;			// detailed object classifiers for this group
	unsigned char		ucUsed;
}
VCA_CLSGRP_T;

typedef struct tCLSGRPS
{
	unsigned int		uiTotalClsgrps;
	VCA_CLSGRP_T		pClsgrps[MAX_NUM_CLSGRPS];
}
VCA_CLSGRPS_T;

/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/
typedef struct
{
	short			sClsobjectId;			// 32-bit
	char			strClsobjectName[MAX_NUM_NAME];	// Clsobject name
	VCA_RULE_AREAS_DSP_T	tAreaSettings;			
	VCA_RULE_SPEEDS_DSP_T	tSpeedSettings;			
}
VCA_CLSOBJECT_DSP_T;

typedef struct
{
	unsigned int			uiTotalClsobjects;	// 32-bit
	VCA_CLSOBJECT_DSP_T		pClsobjects[MAX_NUM_CLSOBJECTS+2];	// the last one is for all other objects
}
VCA_CLSOBJECTS_DSP_T;

#endif // _vca_clsobjectdef_h_
