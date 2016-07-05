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

/*! \file	vca_calibdef.h
 *  \brief	The VCA tracker include file for the calibration definitions.
 */
#ifndef _VCA_CALIBDEF_H_
#define _VCA_CALIBDEF_H_

#define MAX_NUM_MARKERS 3

#define CALIB_STATUS_CALIBRATED_OVERHEAD		4
#define CALIB_STATUS_CALIBRATED_SIDEON			3
#define	CALIB_STATUS_CALIBRATED				2

#define CALIB_STATUS_NUM_ERR_COND 5

#define CALIB_STATUS_UNCALIBRATED			0
#define CALIB_STATUS_INVALIDSETUP			-1
#define CALIB_STATUS_FAILED_TOOFEW_MARKERS		-2
#define CALIB_STATUS_FAILED_TILT_OUTOFRANGE		-3
#define CALIB_STATUS_FAILED_HEIGHT_OUTOFRANGE		-4
#define CALIB_STATUS_FAILED_FOV_OUTOFRANGE		-5

#define	CALIB_STATUS_CALIBRATED_STR			"Calibrated"
#define CALIB_STATUS_CALIBRATED_SIDEON_STR		"Calibrated side-on"
#define CALIB_STATUS_CALIBRATED_OVERHEAD_STR		"Calibrated overhead"
#define CALIB_STATUS_UNCALIBRATED_STR			"Uncalibrated"
#define CALIB_STATUS_INVALIDSETUP_STR			"Calibration Failed: Invalid setup"
#define CALIB_STATUS_FAILED_TOOFEW_MARKERS_STR		"Calibration Failed: Insufficient calibration markers"
#define CALIB_STATUS_FAILED_TILT_OUTOFRANGE_STR		"Calibration Failed: Tilt angle out of range"
#define CALIB_STATUS_FAILED_HEIGHT_OUTOFRANGE_STR	"Calibration Failed: Height out of range. Must be positive"
#define CALIB_STATUS_FAILED_FOV_OUTOFRANGE_STR		"Calibration Failed: Field of view out of range"


#define CALIB_MODE_SIDEON			0x00
#define CALIB_MODE_OVERHEAD			0x01
#define CALIB_MODE_UNCALIBRATED			0x02
#define CALIB_MODE_AUTOSELECT			0x03


#define CALIB_NAN 0xFFFF

typedef struct
{
	int posX, posY;
	int height;

}VCA_CALIB_HEIGHT_MARKER;

typedef enum
{
	HEIGHT_UNITS_METERS,
	HEIGHT_UNITS_FEET

}VCA_CALIB_HEIGHT_UNITS;

typedef enum
{
	SPEED_UNITS_KPH,
	SPEED_UNITS_MPH

}VCA_CALIB_SPEED_UNITS;

typedef struct
{
	double	fHeight;
	double	fTilt;
	double	fRoll;
	double	fFOV;
	double	fPan;

}VCA_CALIB_INFO_T_D;

// NB don't change the order of the following - for PC VCA
typedef struct
{
	float			fHeight;
	float			fTilt;
	float			fRoll;
	float			fFOV;
	int			calibrationStatus;
	VCA_CALIB_HEIGHT_UNITS	heightUnits;
	VCA_CALIB_SPEED_UNITS	speedUnits;

}VCA_CALIB_DSP_INFO;

typedef struct
{
	int			calibrationStatus;
	char			calibrationStatusStr [64];
	VCA_CALIB_INFO_T_D	info;

}VCA_CALIB_ALL_INFO;

#endif // _VCA_CALIBDEF_H_
