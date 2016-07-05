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

/*! \file	vca_generaldef.h
 *  \brief	The VCA tracker include file for the general definitions.
 */
#ifndef _vca_generaldef_h_
#define _vca_generaldef_h_

#include "vca_commondef.h"
#include "vca_featuredef.h"
#include "vca_meta_version.h"
#include "vca_cfg_version.h"

#define MAX_NUM_LICENSES 10

// VCA Channel Info Default values
#define DEFAULT_VCA_RESOLUTION		"D1"
#define DEFAULT_VCA_MAXFRAMERATE	31	// Hardcoded here, which process every frame for both NTSC and PAL
#define DEFAULT_MAX_WIDTH		640
#define DEFAULT_MAX_HEIGHT		480

// VCA License default value
#define DEFAULT_VCA_LIC_STR		"No VCA licenses found"
#define CONTACTING_CODEC_LIC_STR	"Trying to contact VCA codec to validate license..."
#define NEW_VCA_LIC_STR			"Activation code entered, currently being analysed. Please check back later."
//#define VCA_LICENSE_INFO_STR_LEN		1024

#define VCA_CODEC_TRACKER_STR		"track"
#define VCA_CODEC_ABOBJ_STR		"abobj"

// VCA XML schema default values
#define DEFAULT_XML_META_VERSION	META_SCHEMA_VERSION
#define DEFAULT_XML_META_SCHEMA		"vca_meta_schema.xsd"
#define DEFAULT_XML_CFG_VERSION		CFG_SCHEMA_VERSION
#define DEFAULT_XML_CFG_SCHEMA		"vca_cfg_schema.xsd"

// VCA Info
typedef struct tVCAINFO
{
	int	nChannels;			// Number of Video-input Channels
	char	szTrackerSVNVersion[32];	// VCA Tracker SVN version, for VCA debugging and bug fix only;
}
VCA_INFO_T;

typedef struct
{
	char	szMetaVersion[32];		// XML version for vca metadata
	char	szMetaSchema[256];		// XML schema file for metadata
	char	szCfgVersion[32];		// XML for vca configurations 
	char	szCfgSchema[256];		// XML schema file for vca configurations
}
VCA_XML_INFO_T;

// Local definitions for licensing
typedef struct
{
	int		fInUse;
	int		fIsNew;
	int		fIsValid;

	VCA_LICENSE_PROPERTIES_T	tProperties;
	
	/* The channels this license is assigned to
	 * This is a bit packed field:
	 *	0x01	Channel 0
	 *	0x02	Channel 1
	 *	0x04	Channel 2
	 *	 etc	 etc
	 */
	unsigned int	uiChannelAssignment;
	unsigned int	uiPartCode;
}
VCA_LICENSE_INFO_T;

// VCA Channel Info
typedef struct tVCACHANNELINFO
{
	int	nEnable;			// VCA Tracker on/off
	int	nStabilizer;			// Tracker integrated stabilizer on/off
	char	szResolution[32];		// "4CIF" "D1"
	int	nMaxFPS;			// MaxFPS
	int	nMetaFmt;			// Metadata format
	int	nRetrigTime;			// Alarm retrig time (sec)
	int	nMeaUnits;			// MEAUNIT_METRIC or MEAUNIT_IMPERIAL
	int	nClsgrp;			// The current classification group
	int	nNumLicenseIds;
	int	nStatHoldonTime;		// Length of time to hold on to stationary objects
	int	nAbObjEnabled;			// Abandoned object on/off 
	int	nMovObjEnabled;			// Moving/Stationary tracking on/off
	int	nMinObjAreaPix;			// The minimum tracked object size in pixels
	int	nProcessingWidth;		// The processing width for VCA codecs
	int	nProcessingHeight;		// The processing height for VCA codecs
	int	nLicenseIds[10];
	unsigned int	uiAlarmDwell;		// alarmobject display dwell in milli-second, will be used for both ActiveX and BiA
	unsigned int	uiFadeTime;		// object fadeout display dwell in milli-second, will be used for both ActiveX and BiA
	int	nAdvTraEnabled;
	int	nCntLineEnabled;		// Counting line on/off
	int	nSmokeFireEnabled;		// Smoke&fire on/off
	int	nDetectionPoint;		// Detection point of tracked objects: 0:default/1:centroid/2:midbottom
}
VCA_CHANNEL_INFO_T;

#endif // _vca_generaldef_h_
