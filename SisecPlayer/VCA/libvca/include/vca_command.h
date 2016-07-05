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

/*! \file	vca_command.h
 *  \brief	The VCA tracker include file for the command definitions for ActiveX VCA library (axvcalib).
 */

// SetParamVCA commands
VCA_CMD_ADD( VCAEnableMetadata ),
VCA_CMD_ADD( VCAEnableConfig ),
VCA_CMD_ADD( VCASetVideoSize ),

VCA_CMD_ADD( VCASetDataMgrPtr ),
VCA_CMD_ADD( VCASetLogAxPtr ),
VCA_CMD_ADD( VCAApplyAll ),

VCA_CMD_ADD( VCASelectMode ),
VCA_CMD_ADD( VCASetCalibParams ),
VCA_CMD_ADD( VCAPauseVideo ),
VCA_CMD_ADD( VCASetCalibUncalibrated ),
VCA_CMD_ADD( VCAApplyCalibParams),

VCA_CMD_ADD( VCAGetAxVersion),
VCA_CMD_ADD( VCAResetAll),

// GetParamVCA commands
VCA_CMD_ADD( VCAGetCalibrationStatus ),
VCA_CMD_ADD( VCACmdNum )	// Last one must be this

