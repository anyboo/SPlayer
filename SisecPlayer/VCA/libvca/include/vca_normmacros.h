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

/*! \file	vca_normmacros.h
 *  \brief	The VCA tracker include file for the normalisation macros.
 */
#ifndef _vca_normmacros_h_
#define _vca_normmacros_h_

#define NORMALISATOR	65535	// Everything is normalized to 16bits (2Bytes) in the new tracker

#define GETPERCENT( percent ) percent * NORMALISATOR / 100

#define PERCENTASSERT( value) \
	value = ( value > NORMALISATOR ) ? NORMALISATOR: value; \
	value = ( value < 0 ) ? 0: value;

#ifndef PIXELTOPERCENT
#define PIXELTOPERCENT( normalizeValue, pixelValue, maxValue ) \
	normalizeValue = (unsigned short) ( pixelValue * NORMALISATOR / ( maxValue - 1 ) ); \
	PERCENTASSERT( normalizeValue );
#endif 

#ifndef PERCENTTOPIXEL
#define PERCENTTOPIXEL( pixelValue, normalizeValue, maxValue ) \
	pixelValue = ( maxValue - 1 ) * normalizeValue / NORMALISATOR;
#endif

#endif


