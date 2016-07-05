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

/*! \file	vca_drawingdef.h
 *  \brief	The VCA tracker include file for the drawing definitions.
 */
#ifndef _vca_drawingdef_h_
#define _vca_drawingdef_h_

#define VD_MAX_RECORDS_SIZE		(32*1024)
#define VD_MAX_RECORDS_RECT_SIZE	(2*1024)
#define VD_MAX_RECORDS_LINE_SIZE	(22*1024)
#define VD_MAX_RECORDS_TEXT_SIZE	(8*1024)

#define VD_FONTBUFSIZE			(18*1024)
#define VD_FONTBUFFER_SZ		VD_FONTBUFSIZE

#define VD_BITBUFSIZE			(4*1024)	// 180*144/8=3240

#define VD_YUYV_RED			0x52F0525A
#define VD_YUYV_YELLOW			0xD292D210
#define VD_YUYV_BLUE			0x296E29F0
#define VD_YUYV_BLACK			0x10801080
#define VD_YUYV_WHITE			0xEB80EB80

#define VD_ALPHA_FULL			0xFF

#define VD_MAX_STRING_SIZE		48

typedef unsigned int	VD_COLOR;
typedef unsigned char	VD_ALPHA;

typedef enum
{
	VD_TYPE_NULL		= 0x0000,
	VD_TYPE_TEXT,
	VD_TYPE_RECTANGLE,
	VD_TYPE_LINE,
	VD_TYPE_BLOBMAP,
	VD_TYPE_BITMAP
}VD_TYPE_E;


typedef struct 
{
	unsigned short		type;			// specify 1(text)
	unsigned short		size;			// size of data in byte(include text data)
}VD_BASE_TYPE_T;

// follows font bitmap data size .. requires 32bit align
typedef struct 
{
	unsigned short 		size;
	unsigned char  		width;
	unsigned char  		height;
	unsigned char  		ascent;
	unsigned char  		decent;
	unsigned char 		startchar;
	unsigned char  		endchar;
	unsigned char  		bMasked;
	unsigned char  		widths[1];
}VD_FONTDATA_T;

#define TEXT_OPT_MAGNIFY		(0x01 << 0)
#define TEXT_OPT_BACKGROUND		(0x01 << 1)

typedef struct
{
	unsigned short		type;			// specify VD_TYPE_TEXT
	unsigned short		size;			// size of data in byte (include text data)
	VD_COLOR		font_color;		// argb 4bytes
	VD_COLOR		fontoutline_color;	// argb 4bytes
	VD_COLOR		background_color;	// argb 4bytes
	unsigned short		nposition_x;		// pixel
	unsigned short		nposition_y;		// pixel
	unsigned int		opt;
	unsigned short		cblength;		// number of characters
	unsigned char		cbstring[VD_MAX_STRING_SIZE];		// array of character offset at font table
}VD_TEXT_T;

typedef struct
{
	unsigned short		type;			// specify VD_TYPE_RECTANGLE
	unsigned short		size;			// sizoef this structure
	VD_COLOR		color;			// argb 4bytes
	unsigned short		nposition_x;		// pixel
	unsigned short		nposition_y;		// pixel
	unsigned short		nwidth;			// pixel
	unsigned short		nheight;		// pixel
	unsigned short		pattern;
}VD_RECTANGLE_T;

typedef struct
{
	unsigned short		type;			// specify VD_TYPE_LINE
	unsigned short		size;			// sizoef this structure
	VD_COLOR		color;			// argb 4bytes
	unsigned short		nsposition_x;		// pixel
	unsigned short		nsposition_y;		// pixel
	unsigned short		neposition_x;		// pixel
	unsigned short		neposition_y;		// pixel
	unsigned short		nlinewidth;		// pixel
}VD_LINE_T;

typedef struct
{
	unsigned short		type;			// specify VD_TYPE_BTMAP
	unsigned short		size;			// sizoef this structure
}VD_BITMAP_T;

typedef struct
{
	unsigned short		type;			// specify VD_TYPE_BTMAP
	unsigned short		size;			// sizoef this structure
	unsigned char		blobmap[VD_BITBUFSIZE];
}VD_BLOBMAP_T;

typedef struct 
{
	unsigned int		cbtype;			// reserved..
	unsigned int		size;
	unsigned int		ncount_drawing_record;
	unsigned int		nrecord_size;
	unsigned int		pdraw_record[1];	// cast (VD_BASE_TYPE_T*) and inspect type and recast each type.
}
VD_RECORDS_T;

#endif	// _vca_drawingdef_h_
