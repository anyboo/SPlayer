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

/*! \file	vca_tamperdef.h
 *  \brief	The VCA tracker include file for the tamper detection definitions.
 */
#ifndef _vca_tamperdef_h_
#define _vca_tamperdef_h_


typedef struct 
{
	unsigned int	enabled;		// enable/disable
	unsigned int	alarmTimeout;		// time in frames that area must be fired before alarm is generated (1 - inf) 
	unsigned int	areaThreshold;		// percent of area fired to generate alarm (0 - 100)
	unsigned int	firingThreshold;	// firing threshold (percent difference) for each block (0 - 100)
	unsigned int 	lowLightEnabled; 	// enable/disable suppress tamper alarm when detect low light (lights on/off)

} VCA_TAMPER_CONFIG;


typedef enum
{
	VCA_SCENECHANGE_MODE_DISABLED = 0,
	VCA_SCENECHANGE_MODE_AUTOMATIC,
	VCA_SCENECHANGE_MODE_MANUAL

} VCA_SCENECHANGE_MODE;

typedef struct 
{
	VCA_SCENECHANGE_MODE	mode;
	VCA_TAMPER_CONFIG 	tamperConfig;		// config when using tamper (manual mode)
	unsigned long		DNColorEnabled;	// Enable/disable IR cut filter detection (color - grayscale transition)
	unsigned long		DNRimEnabled;	// Enable/disable IR cut filter detection (filter rim movement)

} VCA_SCENECHANGE_CONFIG;


#endif // _vca_tamperdef_h_

