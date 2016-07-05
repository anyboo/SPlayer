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

/*! \file	vca_biadef.h
 *  \brief	The VCA tracker include file for the burnt-in annotation definitions.
 */
#ifndef _vca_biadef_h_
#define _vca_biadef_h_

#include "vca_commondef.h"

#define MAX_BIA_STREAMS			4

#define MAX_BIA_COLORS			7

#define BIA_RGB_RED			RGB2(224,64,64)
#define BIA_RGB_GREEN			RGB2(0,255,0)
#define BIA_RGB_BLUE			RGB2(0,0,255)
#define BIA_RGB_YELLOW			RGB2(255,255,0)
#define BIA_RGB_FUCHSIA			RGB2(255,0,255)
#define BIA_RGB_AQUA			RGB2(0,255,255)
#define BIA_RGB_ORANGE			RGB2(255,165,0)
#define BIA_RGB_WHITE			RGB2(255,255,255)
#define BIA_RGB_BLACK			RGB2(0,0,0)

#define BIA_YUYV_RED			0x70C67068
#define BIA_YUYV_GREEN			0x90229036
#define BIA_YUYV_BLUE			0x296E29F0
#define BIA_YUYV_YELLOW			0xD292D210
#define BIA_YUYV_FUCHSIA		0x54C754B8 //0x6BDE6BCA
#define BIA_YUYV_AQUA			0xA910A9A6
#define BIA_YUYV_ORANGE			0xA5B3A52A
#define BIA_YUYV_WHITE			0xEB80EB80
#define BIA_YUYV_BLACK			0x10801080

typedef enum
{
	BIA_FONT_SMALL		= 0,
	BIA_FONT_MEDIUM,
	BIA_FONT_LARGE,
	BIA_NUM_FONTS
}
BIA_FONT_SIZE;

// VCA burnt-in annotation (BIA) info default values
#define DEFAULT_DRAWING_FLAGS		VCA_DISPLAY_DEFAULT
#define DEFAULT_COLOR_ALARM		RGB2(224,64,64) 	// RGB2(255,0,0)	// red
#define DEFAULT_COLOR_NONALARM		RGB2(255,255,0)		// yellow
#define DEFAULT_TXTSZ_OBJECT		BIA_FONT_SMALL		// small text for object
#define DEFAULT_TXTSZ_COUNTER		BIA_FONT_SMALL		// small text for counter
#define DEFAULT_TXTSZ_SYSTEM		BIA_FONT_MEDIUM		// medium text for system message
#define DEFAULT_NAME_ST0		"first stream"		// default name for BiA 0
#define DEFAULT_NAME_ST1		"second stream"		// default name for BiA 1
#define DEFAULT_NAME_ST2		"snapshot"		// default name for BiA 2
#define DEFAULT_NAME_ST3		"analogue output"	// default name for BiA 3


// VCA Burnt-in annotations
typedef struct
{
	char		strBIAName[MAX_NUM_NAME];// burnt-in-annotation name
	int		fEnable;		// enable/disable
	unsigned int	uiDrawingFlags;		// drawing flags
	unsigned int	uiColorAlarm;		// alpha+RGB
	unsigned int	uiColorNonAlarm;	// alpha+RGB
	unsigned int	uiTxtSzObject;		// object text size: 0 - small(default), 1 - medium, 2 - large
	unsigned int	uiTxtSzCounter;		// counter text size: 0 - small(default), 1 - medium, 2 - large
	unsigned int	uiTxtSzSystem;		// system message text size: 0 - small(default), 1 - medium, 2 - large
}
VCA_BIA_T;

typedef struct
{
	int		iTotalStreams;		// Number of streams
	int		fEnable;		// Enable/disbale
	VCA_BIA_T	pBIAs[MAX_BIA_STREAMS];
}
VCA_BIAS_T;

/****************************************************************************/
// Important! All the dsp side structure should be 32-bit alignment for XDM
/****************************************************************************/
typedef struct
{
	unsigned short	fEnable;		// Enable
	unsigned int	uiDrawingFlags;		// drawing flags
	unsigned int	uiColorAlarm;		// alpha+RGB
	unsigned int	uiColorNonAlarm;	// alpha+RGB
	unsigned int	uiTxtSzObject;		// object text size: 0 - small(default), 1 - medium, 2 - large
	unsigned int	uiTxtSzCounter;		// counter text size: 0 - small(default), 1 - medium, 2 - large
	unsigned int	uiTxtSzSystem;		// system message text size: 0 - small(default), 1 - medium, 2 - large
}
VCA_BIA_DSP_T;

typedef struct
{
	int		iTotalStreams;		// Number of streams
	int		fEnable;		// Enable/disbale
	unsigned int	uiAlarmDwell;		// milli-second, comes from VCA_CHANNEL_INFO_T
	unsigned int	uiFadeTime;		// milli-second, comes from VCA_CHANNEL_INFO_T
	VCA_BIA_DSP_T	pBIAs[MAX_BIA_STREAMS];
}
VCA_BIAS_DSP_T;

#define PICKCLOSECOLORIDX( idx, color ) \
{ \
	unsigned char r = color >> 16; \
	unsigned char g = color >> 8; \
	unsigned char b = color; \
	idx = 0; \
	if ( ( r >= 192 ) && ( g <= 64 ) && ( b <= 64 ) ) \
		idx = 0; \
	else if ( ( r <= 64 ) && ( g >= 192 ) && ( b <= 64 ) ) \
		idx = 1; \
	else if ( ( r <= 64 ) && ( g <= 64 ) && ( b >=192 ) ) \
		idx = 2; \
	else if ( ( r >= 192 ) && ( g >=192 ) && ( b <= 64 ) ) \
		idx = 3; \
	else if ( ( r >= 192 ) && ( g <= 64 ) && ( b >= 192 ) ) \
		idx = 4; \
	else if ( ( r <= 64 ) && ( g >= 192 ) && ( b >= 192 ) ) \
		idx = 5; \
	else if ( ( r >= 192 ) && ( g <= 192 ) && ( b <= 64 ) ) \
		idx = 6; \
}

#endif // _vca_biadef_h_
