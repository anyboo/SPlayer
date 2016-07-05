#ifndef __libvca_H__
#define __libvca_H__

/* HISTORY
 *
 * 	[V1.00.00]	2008-11-11	ben
 *					Added history header
 *					Made funcs extern "C" for cpp building
 *--------------------------------------------------------------------------------------------
 *	[V1.00.01] 	2008-11-20 	bincheng
 *		[FIX] 	zonename&countername empty crash
 *--------------------------------------------------------------------------------------------
 *	[V1.00.02]	2008-11-27	bincheng
 *		[ADD]	enabled/disable stabilizer inside tracker	
 *--------------------------------------------------------------------------------------------
 *	[V1.00.03]	2009-01-30	bincheng
 *		[FIX]	all info files changed according to the common files
 *--------------------------------------------------------------------------------------------
 *	[V1.01.00] 	2009-02-06	etienne
 *		[ADD]   calibration functions
 *--------------------------------------------------------------------------------------------
 *	[V1.01.01]	2009-02-11	bincheng
 *		[FIX]	counterid invalid when retrieving counterinfo
 *--------------------------------------------------------------------------------------------
 *	[V1.02.00] 	2009-02-16	etienne
 *		[ADD]   more calibration properties
 *--------------------------------------------------------------------------------------------
 *	[V1.02.01] 	2009-02-16	etienne
 *		[ADD]   calibration units
 *--------------------------------------------------------------------------------------------
 *	[V1.03.00]	2009-02-20	bincheng
 *		[ADD]	object classifier units
 *--------------------------------------------------------------------------------------------
 *	[V1.03.01]	2009-02-28	bincheng
 *		[CHG]	VCA_OBJECT_T to VCA_CLSOBJECT_T to avoid naming conflicts
 *--------------------------------------------------------------------------------------------
 *	[V1.03.02]	2009-03-04	bincheng
 *		[CHG]	libvca_get_objectssi_filename to libvca_get_clsobjectssi_filename
 *--------------------------------------------------------------------------------------------
 *	[V1.03.03]	2009-03-06	bincheng
 *		[ADD]	minalarmtime to vca info file
 *--------------------------------------------------------------------------------------------
 *	[V1.03.04]	2009-03-10	bincheng
 *		[CHG]	usClsObjectId to sClsObject
 *--------------------------------------------------------------------------------------------
 *	[V1.03.05]	2009-03-16	bincheng
 *		[ADD]	VCA_INFO_T for all VCA info
 *		[ADD]	VCA_CHANNEL_INFO_T for each video-in channel, multi-channel support
 *		[CHG]	removed dependency between VCA and stream, VCA only depends on the channel
 *--------------------------------------------------------------------------------------------
 *	[V1.03.06]	2009-03-17	jinohan
 *		[ADD]	initialize '0' code to VCA_CLSOBJECTS_T tInfo; at libvca_setup_CLSOBJECTinfo(...);
 *--------------------------------------------------------------------------------------------
 *	[V1.03.07]	2009-03-18	bincheng
 *		[CHG]	init 0 for all set_default functions
 *--------------------------------------------------------------------------------------------
 *	[V1.04.00]	2009-03-24	bincheng
 *		[ADD]	szTrackerSVNVersion inside VCA info for debugging and bug fix
 *--------------------------------------------------------------------------------------------
 *	[V1.04.01]	2009-03-26	bincheng
 *		[ADD]	saving counter result inside info file
 *--------------------------------------------------------------------------------------------
 *	[V1.05.00]	2009-03-30	bincheng
 *		[CHG]	move meaunits from VCA info to CHANNEL info
 *		[ADD]	classification group info (VCA_CLSGRPS_T)
 *--------------------------------------------------------------------------------------------
 *	[V1.06.00]	2009-04-03	bincheng
 *		[CHG]	change usAreaLo to fAreaLo, usAreaUp to fAreaUp, usSpeedLo to fSpeedLo, usSpeedUp to fSpeedUp
 *--------------------------------------------------------------------------------------------
 *	[V1.07.00]	2009-04-06	bincheng
 *		[ADD]	VCA supported features inside vcalicense info
 *		[ADD]	nLicenseId for each channel, considering the multiple licenses in future
 *		[ADD]	default classification group for overhead calibration mode
 *--------------------------------------------------------------------------------------------
 *	[V1.07.01]	2009-04-09	bincheng
 *		[ADD]	fCalibration inside VCA license SSI files
 *--------------------------------------------------------------------------------------------
 *	[V1.08.00]	2009-04-17	benwhite
 *		[FIX]	Added check for emtpy string in license properties
 *--------------------------------------------------------------------------------------------
 *	[V1.09.00]	2009-06-05	etienne
 *		[ADD]	Added tamper detection functions
 *--------------------------------------------------------------------------------------------
 *	[V1.10.00]	2009-07-02	bincheng
 *		[ADD]	Added burnt-in annotation functions
 *--------------------------------------------------------------------------------------------
 *	[V1.10.01]	2009-07-28	bincheng
 *		[ADD]	BiA name
 *--------------------------------------------------------------------------------------------
 *	[V1.11.00]	2009-09-22	etienne
 *		[ADD]	Abobj info
 *		[ADD]	Stat_holon_time to channel info
 *--------------------------------------------------------------------------------------------
 *	[V1.12.00]	2009-09-24	bincheng
 *		[FIX]	using mutex when retrieving Tamper/Abobj info
 *		[FIX]	BiA info recreation bug
 *--------------------------------------------------------------------------------------------
 *	[V1.13.00]	2010-12-05	bincheng
 *		[ADD]	minobjarea, processingwidth and processingheight for VCA Channel info
 *		[ADD]	Group of People classification to default classification group
 *		[ADD]	mutliwireid and rulelogic if TMP_ADVANCED_RULE is defined (DEFAULT OFF!)
 *--------------------------------------------------------------------------------------------
 *	[V1.13.00]	2010-01-14	neil
 *		[CHG]	License info functions now take a product-type parameter 
 *		[ADD]	STAB info functions now keep a general stabssi file up to date
 *--------------------------------------------------------------------------------------------
 *	[V1.14.00]	2010-01-28	bincheng
 *		[CHG]	split libvca.c to several different files: zone_cfg, event_cfg ...
 *		[ADD]	load/save vca configurations from/to XML file, added libixml
 *		[CHG]	move alarmdwell and fadetime from BiA to Channel
 *--------------------------------------------------------------------------------------------
 *	[V1.15.00]	2010-01-28	benwhite
 *		[CHG]	changed VCAFILE_XMLCFG_INFO to /tmp instead of /opt/www
 *--------------------------------------------------------------------------------------------
 *	[V1.15.01]	2010-02-26	bincheng
 *		[ADD]	position to classification_cfg
 *--------------------------------------------------------------------------------------------
 *	[V1.15.02]	2010-03-03	bincheng
 *		[ADD]	classification position to xml_cfg
 *		[FIX]	currently classification group is hardcoded to be 0
 *		[FIX]	some zone/counter/event XML settings not loaded properly
 *--------------------------------------------------------------------------------------------
 *	[V1.15.03]	2010-04-08	bincheng
 *		[FIX]	added backward compatibility for classification position, user won't lose all the classification settings when upgrading to the latest
 *--------------------------------------------------------------------------------------------
 *	[V1.16.00]	2010-06-14	bincheng
 *		[CHG]	split counter info to 2 files, general info and result info (result info will be saved more frequently than general info)
 *		[ADD]	restore defaults for all vca settings
 *--------------------------------------------------------------------------------------------
 *	[V1.17.00]	2010-06-17	bincheng
 *		[ADD]	XML version/schema info for both metadata and configurations
 *--------------------------------------------------------------------------------------------
 *	[V1.17.01]	2010-06-23	bincheng
 *		[ADD]	object/counter/sysmsg text size for BiA
 *		[ADD]	drawing objects in 2-color or multiple color
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.17.02]	2010-06-14	benwhite
 *		[ADD]	added vca product code -> license slot lookup
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.00]	2010-06-30	benwhite
 *		[ADD]	added support for running on Windows for PC VCA
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.01]	2010-06-30	mattclarkson
 *		[FIX]	removed commas in sprintf() to fix compilation error
 *--------------------------------------------------------------------------------------------
 *	[V1.18.02]	2010-07-21	bincheng
 *		[FIX]	QA-679, subcounter new settings sometimes are not applied properly
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.03]	2010-07-23	benwhite
 *		[ADD]	Function to reset all to defaults
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.04]	2010-07-28	benwhite
 *		[ADD]	Save/load license info
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.05]	2010-08-25	bincheng
 *		[CHG]	moved vca schema files to /opt/bin/
 *
 *--------------------------------------------------------------------------------------------
 *	[V1.18.06]	2010-09-10	bincheng
 *		[ADD]	PTZ-tracking info
 *		[ADD]	UID for zone/rule/counter
 *--------------------------------------------------------------------------------------------
 *	[V1.19.00]	2010-10-13	benwhite
 *		[CHG]	Pass parameters by reference instead of value = quicker + no stack overflows
 *--------------------------------------------------------------------------------------------
 *	[V1.20.0]	2011-06-27	jbyoon
 *		[ADD]	function for countdog
 *--------------------------------------------------------------------------------------------
 *	[V1.20.1]	2012-02-10	Conrad Kim
 *		[CHG]	libvca_get_infopath() for PCVCAsysWin.
 *              multiple instances which are linked with libvca can keep their own vca_xx.info files.
*/

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <assert.h>
#pragma comment(lib, "Psapi.lib")
#define __attribute__(exp)
#else
#include <nvccommon.h>
#include "davinci_common.h"	// _DEFAULT_SERVER_INFOPATH, _DEFAULT_WEBSERVER_PATH
#include "typedef.h"
#endif // _WIN32

#ifdef PCLINUX
#define	_TEMP_DIR_PCLINUX	"/tmp/"
#endif

#include <stdio.h>
#include <string.h>


// all definitions about zones, rules, events
#include "vca_commondef.h"
#include "vca_generaldef.h"	// VCA_INFO_T, VCA_LICENSE_INFO_T, VCA_CHANNEL_INFO_T, VCA_XML_INFO_T
#include "vca_zonedef.h"	// VCA_ZONES_T
#include "vca_ruledef.h"	// VCA_RULES_T
#include "vca_eventdef.h"	// VCA_EVENTS_T
#include "vca_countingdef.h"	// VCA_COUNTERS_T
#include "vca_eventmacros.h"	// DESERIALIZE_EVENT, SERIALIZE_EVENT
#include "vca_stabdef.h"
#include "vca_calibdef.h"	// VCA_CALIB_ALL_INFO
#include "vca_clsobjectdef.h"	// VCA_CLSOBJECTS_T, VCA_CLSOBJECT_T
#include "vca_tamperdef.h"
#include "vca_biadef.h"		// VCA_BIAS_T, VCA_BIA_T
#include "vca_ptztrackingdef.h"
#include "vca_countdogdef.h"	// VCA_COUNTDOG_INFO_T

/***************************************************************************/
/*               _    _ ______ _      _      ____  _                       */
/*              | |  | |  ____| |    | |    / __ \| |                      */
/*              | |__| | |__  | |    | |   | |  | | |                      */
/*              |  __  |  __| | |    | |   | |  | | |                      */
/*              | |  | | |____| |____| |___| |__| |_|                      */
/*              |_|  |_|______|______|______\____/(_)                      */
/*                                                                         */
/*  If you are adding functionality to this file, please don't forget to   */
/*  to add your _setup_XXX function to libvca_resetALLdefaults, at the     */
/*  bottom of the file.                                                    */
/*                               Fanks  :)                                 */
/*                                                                         */
/***************************************************************************/

 
#define LIBVCA_MAJERVER		1
#define LIBVCA_MINORVER		20
#define LIBVCA_BUILDVER		00
#define	LIBVCA_VERSION		"1.20.01"

#define LIBVCA_ERROR		-1

#define _DEFAULTIFFAILED_	// create default info file if failed to open

#ifdef _DM6467_ARCHITECTURE
#define MAX_NUM_CHANNELS 4
#else
#define MAX_NUM_CHANNELS 1
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static inline int libvca_getversion( char* _pszver )
{
	strcpy( _pszver, LIBVCA_VERSION );
	return ((LIBVCA_MAJERVER << 16) | (LIBVCA_MINORVER << 8) | (LIBVCA_BUILDVER << 0));
}

typedef enum
{
	VCA_CFGFLAG_CHANNEL		= 0x00000001,
	VCA_CFGFLAG_ZONE		= 0x00000002,
	VCA_CFGFLAG_EVENT		= 0x00000004,
	VCA_CFGFLAG_COUNTER		= 0x00000008,
	VCA_CFGFLAG_CLASSIFICATION	= 0x00000010,
	VCA_CFGFLAG_CALIBRATION		= 0x00000020,
	VCA_CFGFLAG_BIA			= 0x00000040,
	VCA_CFGFLAG_TAMPER		= 0x00000080,
	VCA_CFGFLAG_LICENSE		= 0x00000100,
	VCA_CFGFLAG_PTZTRACKING		= 0x00000200,
	VCA_CFGFLAG_SCENECHANGE		= 0x00000400,
	VCA_CFGFLAG_COUNTDOG		= 0x00000800,
	VCA_CFGFLAG_ALL			= 0xFFFFFFFF
}
VCA_CFGFLAGS_E;

typedef enum
{
	VCA_LIC_TRACKER,
	VCA_LIC_STAB,
	VCA_LIC_INVALID
}
VCA_LIC_E;

// VCA
#define	VCAFILE_VCA_INFO_PREFIX		"vca"
#define	VCAFILE_VCA_INFO_SUFFIX		".info"
#define	VCAFILE_VCA_SSI_PREFIX		"ssi/vca"
#define	VCAFILE_VCA_SSI_SUFFIX		"_SettingServerForm"

// VCA Licensing
#define VCAFILE_LIC_INFO_PREFIX		"vca_lc"
#define VCAFILE_LIC_INFO_SUFFIX		".info"
#define VCAFILE_LIC_SSI_PREFIX		"ssi/vca_lc"
#define VCAFILE_LIC_SSI_SUFFIX		"_SettingServerForm"

// HwGuid
#define VCAFILE_HWGUID_SSI_PREFIX	"ssi/vcaHwGuid"
#define VCAFILE_HWGUID_SSI_SUFFIX	"_SettingServerForm"

// VCA Channel info
#define	VCAFILE_CHANNEL_INFO_PREFIX	"vca_ch"
#define	VCAFILE_CHANNEL_INFO_SUFFIX	".info"
#define	VCAFILE_CHANNEL_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_CHANNEL_SSI_SUFFIX	"_SettingServerForm"

// VCA Zone Info
#define	VCAFILE_ZONE_INFO_PREFIX	"vca_ch"
#define	VCAFILE_ZONE_INFO_SUFFIX	"_zone.info"
#define	VCAFILE_ZONE_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_ZONE_SSI_SUFFIX		"_zone_SettingServerForm"

// VCA Event info
#define	VCAFILE_EVENT_INFO_PREFIX	"vca_ch"
#define	VCAFILE_EVENT_INFO_SUFFIX	"_event.info"
#define	VCAFILE_EVENT_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_EVENT_SSI_SUFFIX	"_event_SettingServerForm"

// VCA Counter info
#define	VCAFILE_COUNTER_INFO_PREFIX		"vca_ch"		// Old info file, will be split into 2 seperate files
#define	VCAFILE_COUNTER_INFO_SUFFIX		"_counter.info"					// Old info file, will be split into 2 seperate files
#define	VCAFILE_COUNTERGEN_INFO_PREFIX	"vca_ch"
#define	VCAFILE_COUNTERGEN_INFO_SUFFIX	"_countergen.info"
#define	VCAFILE_COUNTERRES_INFO_PREFIX	"vca_ch"
#define	VCAFILE_COUNTERRES_INFO_SUFFIX	"_counterres.info"
#define	VCAFILE_COUNTER_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_COUNTER_SSI_SUFFIX		"_counter_SettingServerForm"

// VCA Stabilizer info
#define	VCAFILE_STAB_INFO_PREFIX	"vca_ch"
#define	VCAFILE_STAB_INFO_SUFFIX	"_stab.info"
#define	VCAFILE_STAB_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_STAB_SSI_SUFFIX		"_stab_SettingServerForm"

// VCA Calibration info
#define	VCAFILE_CALIB_INFO_PREFIX	"vca_ch"
#define	VCAFILE_CALIB_INFO_SUFFIX	"_calib.info"
#define	VCAFILE_CALIB_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_CALIB_SSI_SUFFIX	"_calib_SettingServerForm"

// VCA Object classification group info
#define	VCAFILE_CLSGRP_INFO_PREFIX	"vca_ch"
#define VCAFILE_CLSGRP_INFO_SUFFIX	"_clsgrp.info"
#define	VCAFILE_CLSGRP_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_CLSGRP_SSI_SUFFIX	"_clsgrp_SettingServerForm"

// VCA Tamper detection info
#define	VCAFILE_TAMPER_INFO_PREFIX	"vca_ch"
#define VCAFILE_TAMPER_INFO_SUFFIX	"_tamper.info"
#define	VCAFILE_TAMPER_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_TAMPER_SSI_SUFFIX	"_tamper_SettingServerForm"

// VCA Scene change detection info
#define	VCAFILE_SCENECHANGE_INFO_PREFIX	"vca_ch"
#define VCAFILE_SCENECHANGE_INFO_SUFFIX	"_scenechange.info"
#define	VCAFILE_SCENECHANGE_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_SCENECHANGE_SSI_SUFFIX	"_scenechange_SettingServerForm"

// VCA Burnt-in annotation info
#define	VCAFILE_BIA_INFO_PREFIX		"vca_ch"
#define VCAFILE_BIA_INFO_SUFFIX		"_bia.info"
#define	VCAFILE_BIA_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_BIA_SSI_SUFFIX		"_bia_SettingServerForm"

// VCA Abandoned object info
#define	VCAFILE_ABOBJ_INFO_PREFIX	"vca_ch"
#define VCAFILE_ABOBJ_INFO_SUFFIX	"_abobj.info"
#define	VCAFILE_ABOBJ_SSI_PREFIX	"ssi/vca_ch"
#define	VCAFILE_ABOBJ_SSI_SUFFIX	"_abobj_SettingServerForm"

// VCA XML Configurations
#define VCAFILE_XMLCFG_INFO		"/tmp/vca_config.xml"
#define VCAFILE_XMLCFG_SSI		"ssi/vca_xmlconfig_SettingServerForm"

#define VCAFILE_XML_META_SCHEMA		_DEFAULT_SERVER_BINPATH	 DEFAULT_XML_META_SCHEMA	
#define VCAFILE_XML_CFG_SCHEMA		_DEFAULT_SERVER_BINPATH	 DEFAULT_XML_CFG_SCHEMA

#define	VCAFILE_XML_INFO_PREFIX		"vcaxml"
#define	VCAFILE_XML_INFO_SUFFIX		".info"
#define	VCAFILE_XML_SSI_PREFIX		"ssi/vcaxml"
#define	VCAFILE_XML_SSI_SUFFIX		"_SettingServerForm"

// VCA PTZ tracking info
#define	VCAFILE_PTZT_INFO_PREFIX	"vca_ch"
#define VCAFILE_PTZT_INFO_SUFFIX	"_ptztracking.info"
#define	VCAFILE_PTZT_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_PTZT_SSI_SUFFIX		"_ptztracking_SettingServerForm"

// VCA PTZ global info
#define	VCAFILE_PTZG_INFO_PREFIX	"vca"
#define VCAFILE_PTZG_INFO_SUFFIX	"_ptzglobal.info"
#define	VCAFILE_PTZG_SSI_PREFIX		"ssi/vca"
#define	VCAFILE_PTZG_SSI_SUFFIX		"_ptzglobal_SettingServerForm"

// VCA PTZ profile
#define VCAFILE_PTZPROFILE_DIR		"ptzprofiles/"
#define VCAFILE_PTZPROFILE_EXT		"*.ptzp"

// VCA PTZ auto-calibration info
#define VCAFILE_PTZC_INFO_PREFIX	"vca"
#define VCAFILE_PTZC_INFO_SUFFIX	"_ptz_autocalibration.info"
#define	VCAFILE_PTZC_SSI_PREFIX		"ssi/vca"
#define	VCAFILE_PTZC_SSI_SUFFIX		"_ptz_autocalibration_SettingServerForm"

#define VCAFILE_PTZC_LOGFILE_DIR	VCAFILE_PTZPROFILE_DIR
#define VCAFILE_PTZC_LOGFILE_EXT	"*.ptzl"

// VCA CountDog info
#define	VCAFILE_COUNTDOG_INFO_PREFIX	"vca_ch"
#define VCAFILE_COUNTDOG_INFO_SUFFIX	"_countdog.info"
#define	VCAFILE_COUNTDOG_SSI_PREFIX		"ssi/vca_ch"
#define	VCAFILE_COUNTDOG_SSI_SUFFIX		"_countdog_SettingServerForm"

// UID
#define	VCAFILE_UID_INFO_PREFIX		"vca_uid"
#define	VCAFILE_UID_INFO_SUFFIX		".info"

#define	VCA_INFO_PATH			"\\UDPTech\\libvca"
#define	VCA_INFO_REGKEY			"Software"VCA_INFO_PATH
#define	VCA_DEFAULT_INFO_PATH		".\\"

/****************************** All function defintions ************************************/

//---------------------------------------------------------------
// These functions retrieve the base path for the settings files.
// it needs to be this way because this library is also used in PCVCA.

#if defined(_WIN32)

static void CreateDir(char* szPath)
{
	char DirName[MAX_PATH];
	char* p = szPath;
	char* q = DirName; 
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
				CreateDirectoryA(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectoryA(DirName, NULL);
}

static void EmptyDirectory(char* szPath, int pathBufSize)
{
	char fileFound[MAX_PATH];
	WIN32_FIND_DATAA info;
	HANDLE hp; 
	sprintf(fileFound, "%s\\*.*", szPath);
	hp = FindFirstFileA(fileFound, &info);
	do
	{
		if (!((strcmp(info.cFileName, ".")==0)||
			(strcmp(info.cFileName, "..")==0)))
		{
			if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==
				FILE_ATTRIBUTE_DIRECTORY)
			{
				char szTemp[MAX_PATH];
				sprintf(szTemp, "%s\\%s", szPath, info.cFileName);
				EmptyDirectory(szTemp, MAX_PATH);
				RemoveDirectoryA(szTemp);
			}
			else
			{
				sprintf(fileFound,"%s\\%s", szPath, info.cFileName);
				BOOL retVal = DeleteFileA(fileFound);
			}
		}

	}while(FindNextFileA(hp, &info));
	RemoveDirectoryA(szPath);
	FindClose(hp);
}

static inline BOOL IsProcessRunning(int pid)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	BOOL bRet = EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
	assert(bRet);

	cProcesses = cbNeeded / sizeof(DWORD);

	for (DWORD i = 0; i < cProcesses; i++)
		if(aProcesses[i] == pid) return TRUE;

	return FALSE;
}

static void EnumAndCleanUnusedSubKeys(HKEY RootKey, char* subKey) 
{
	HKEY hKey;
	DWORD cSubKeys;        //Used to store the number of Subkeys
	DWORD maxSubkeyLen;    //Longest Subkey name length
	DWORD cValues = 0;        //Used to store the number of Subkeys
	DWORD maxValueLen;    //Longest Subkey name length

	RegOpenKeyExA(RootKey, subKey, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey);

	RegQueryInfoKey(hKey,            // key handle
		NULL,            // buffer for class name
		NULL,            // size of class string
		NULL,            // reserved
		&cSubKeys,        // number of subkeys
		&maxSubkeyLen,    // longest subkey length
		NULL,            // longest class string 
		&cValues,        // number of values for this key 
		&maxValueLen,    // longest value name 
		NULL,            // longest value data 
		NULL,            // security descriptor 
		NULL);            // last write time

	if(cValues > 0)
	{
		DWORD dwIndex = 0;

		for (DWORD i = 0; i < cValues; i++)
		{
			char valueName[MAX_PATH] = {0};
			DWORD valNameLen = sizeof(valueName);
			DWORD dataType;
			char data[MAX_PATH] = {0};
			DWORD dataSize = sizeof(data);

			RegEnumValueA(hKey, dwIndex, valueName, &valNameLen, NULL, &dataType, (BYTE*)&data,	&dataSize);

			if( REG_SZ == dataType ) {;}
			else if( REG_DWORD == dataType ) {;} // This can't happen;

			int pid = atoi(valueName);
			if( FALSE == IsProcessRunning(pid) )
			{
				// Delete Path
				DWORD attribs = GetFileAttributesA(data); 
				if (attribs != INVALID_FILE_ATTRIBUTES) EmptyDirectory(data, MAX_PATH);

				// Delete RegKey
				RegDeleteValueA(hKey, valueName);
			}


			dwIndex++;
		}
	}

	if(hKey) RegCloseKey(hKey); 
}

static inline int libvca_clean_infopathes()
{
	EnumAndCleanUnusedSubKeys(HKEY_CURRENT_USER, VCA_INFO_REGKEY);

	return 0;
}

#endif // _WIN32

static inline char* libvca_get_infopath()
{
#if defined(_WIN32)
	static char szBuf[1024];
	DWORD dwDisposition;

	// Create and Open VCA Reg Key
	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyExA( HKEY_CURRENT_USER, VCA_INFO_REGKEY, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey );
	if( lResult != ERROR_SUCCESS )
	{
		if( lResult == ERROR_FILE_NOT_FOUND )
		{
			LONG lResult = RegCreateKeyExA( HKEY_CURRENT_USER, VCA_INFO_REGKEY, NULL, NULL, REG_OPTION_NON_VOLATILE, 
				KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition );
			if( ERROR_SUCCESS != lResult ) return VCA_DEFAULT_INFO_PATH;
		}
	}
	
	// Create Temp Path
	if( !GetTempPathA( sizeof( szBuf ), szBuf ) ) return VCA_DEFAULT_INFO_PATH;
	sprintf(szBuf, "%s%s\\", szBuf, VCA_INFO_PATH);
	CreateDir(szBuf);

	int pid = GetCurrentProcessId();
	char szRegKey[MAX_PATH];
	static char szRegValue[MAX_PATH];
	DWORD dwType;
	DWORD dwLen = MAX_PATH;
	_itoa(pid, szRegKey, 10);
	DWORD retVal = RegQueryValueExA( hKey, szRegKey, NULL, &dwType, (BYTE *)szRegValue, &dwLen );
	if( ERROR_FILE_NOT_FOUND == retVal )
	{
		sprintf(szRegValue, "%s%d\\", szBuf, pid);
		RegSetValueExA( hKey, szRegKey, NULL, REG_SZ, (BYTE *)szRegValue, (DWORD)strlen(szRegValue)+1 );
		CreateDir(szRegValue);
	}

	RegCloseKey( hKey );

	return szRegValue;
	
#elif defined(PCLINUX)
	return _TEMP_DIR_PCLINUX;
#else
	return _DEFAULT_SERVER_INFOPATH;
#endif
}

static inline char* libvca_get_webpath()
{
#if defined(_WIN32)
	static char szBuf[1024];
	int iLen = 0;
	if( iLen = GetTempPathA( sizeof( szBuf ), szBuf ) )
	{
		sprintf( szBuf + iLen, "web_" );
		return szBuf;
	}
	else
	{
		// All failed, just use C:\ as a last resort
		return "C:\\";
	}
#elif defined(PCLINUX)
	static char szBuf[1024];
	int iLen = strlen(_TEMP_DIR_PCLINUX);
	sprintf( szBuf + iLen, "web_" );
	return szBuf;
#else
	return _DEFAULT_WEBSERVER_PATH;
#endif
}

static inline char* libvca_get_binpath()
{
#if defined(_WIN32)
	static char szBuf[1024];
	int iLen = 0;
	if( iLen = GetTempPathA( sizeof( szBuf ), szBuf ) )
	{
		sprintf( szBuf + iLen, "bin_" );
		return szBuf;
	}
	else
	{
		// All failed, just use C:\ as a last resort
		return "C:\\";
	}
#elif defined(PCLINUX)
	static char szBuf[1024];
	int iLen = strlen(_TEMP_DIR_PCLINUX);
	sprintf( szBuf + iLen, "bin_" );
	return szBuf;
#else
	return _DEFAULT_SERVER_BINPATH;
#endif
}


//----------------------------------------------------------------

// VCA Info
static inline char* libvca_get_vcainfo_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_VCA_INFO_PREFIX VCAFILE_VCA_INFO_SUFFIX, libvca_get_infopath() );
	return filename;
};
static inline char* libvca_get_vcassi_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_VCA_SSI_PREFIX VCAFILE_VCA_SSI_SUFFIX, libvca_get_webpath() );
	return filename;
};
int libvca_setup_VCAinfo( int _nParam );
int libvca_create_VCAinfo( IN VCA_INFO_T *_ptinfo );
int libvca_retrieve_VCAinfo( OUT VCA_INFO_T *_ptnfo );
int libvca_check_VCAinfo( );

// HW guid bits
static inline char *libvca_gethwguidssi_filename()
{
	static char filename[256];
	memset( filename, 0, 256 );
	sprintf( filename, "%s" VCAFILE_HWGUID_SSI_PREFIX VCAFILE_HWGUID_SSI_SUFFIX, libvca_get_webpath() );
	return filename;
};

// LICENSING
static inline char *libvca_getlicinfo_filename(int _nslot)
{
	static char filename[256];
	memset( filename, 0, 256 );
	sprintf( filename, "%s" VCAFILE_LIC_INFO_PREFIX "%d" VCAFILE_LIC_INFO_SUFFIX, libvca_get_infopath(), _nslot );

	return filename;
};
static inline char *libvca_getlicssi_filename(int _nslot)
{
	static char filename[256];
	memset( filename, 0, 256 );
	sprintf( filename, "%s" VCAFILE_LIC_SSI_PREFIX "%d" VCAFILE_LIC_SSI_SUFFIX, libvca_get_webpath(), _nslot);

	return filename;
};
int libvca_setup_LICinfo(int _nParam, int _nslot);
int libvca_create_LICinfo( IN VCA_LICENSE_INFO_T *_ptinfo, int _nslot );
int libvca_retrieve_LICinfo( OUT VCA_LICENSE_INFO_T *_ptinfo, int _nslot );
int libvca_check_LICinfo(int _nslot);
unsigned int libvca_getLicenseChannels( IN VCA_LICENSE_INFO_T *_ptinfo );
int libvca_getLICFeatures( IN int _nchannelno, OUT VCA_SYS_FEATURES_T* pSYSFeatures, OUT VCA_STAB_FEATURES_T* pSTABFeatures );
VCA_LIC_E libvca_getLICtype( IN int _nslot, IN int _nchannelno );

// VCA Channel
static inline char* libvca_get_channelinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CHANNEL_INFO_PREFIX "%d" VCAFILE_CHANNEL_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_channelssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CHANNEL_SSI_PREFIX "%d" VCAFILE_CHANNEL_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_CHANNELinfo( int _nParam, int _nchannelno );	// 1: create default values for all channel info, 2: create default values for enable/disbale, 2: create default values for advanced
int libvca_create_CHANNELinfo( IN VCA_CHANNEL_INFO_T *_ptinfo, int _nchannelno );
int libvca_retrieve_CHANNELinfo( OUT VCA_CHANNEL_INFO_T *_ptinfo, int _nchannelno );
int libvca_check_CHANNELinfo( int _nchannelno );

// Zones
static inline char* libvca_get_zoneinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_ZONE_INFO_PREFIX "%d" VCAFILE_ZONE_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_zonessi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_ZONE_SSI_PREFIX "%d" VCAFILE_ZONE_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_ZONEinfo( int _nParam, int _nchannelno );
int libvca_create_ZONEinfo( IN VCA_ZONES_T *_ptinfo, int _nchannelno );
int libvca_retrieve_ZONEinfo( OUT VCA_ZONES_T *_ptinfo, int _nchannelno );
int libvca_check_ZONEinfo( int _nchannelno );

// Events
static inline char* libvca_get_eventinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_EVENT_INFO_PREFIX "%d" VCAFILE_EVENT_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_eventssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_EVENT_SSI_PREFIX "%d" VCAFILE_EVENT_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_EVENTinfo( int _nParam, int _nchannelno );
int libvca_create_EVENTinfo( IN VCA_EVENTS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_EVENTinfo( OUT VCA_EVENTS_T *_ptinfo, int _nchannelno );
int libvca_check_EVENTinfo( int _nchannelno );


// Counters
static inline char* libvca_get_counterinfo_filename( int _nchannelno )	// Old counter info file name
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTER_INFO_PREFIX "%d" VCAFILE_COUNTER_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_countergeninfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTERGEN_INFO_PREFIX "%d" VCAFILE_COUNTERGEN_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_counterresinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTERRES_INFO_PREFIX "%d" VCAFILE_COUNTERRES_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_counterssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTER_SSI_PREFIX "%d" VCAFILE_COUNTER_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};




// both counter general and result info files
int libvca_setup_COUNTERinfo( int _nParam, int _nchannelno );
int libvca_create_COUNTERinfo( IN VCA_COUNTERS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_COUNTERinfo( OUT VCA_COUNTERS_T *_ptinfo, int _nchannelno );
// only counter general info file
int libvca_setup_COUNTERGENinfo( int _nParam, int _nchannelno );
int libvca_create_COUNTERGENinfo( IN VCA_COUNTERS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_COUNTERGENinfo( OUT VCA_COUNTERS_T *_ptinfo, int _nchannelno );
int libvca_check_COUNTERGENinfo( int _nchannelno );
// only counter result info file
int libvca_setup_COUNTERRESinfo( int _nParam, int _nchannelno );
int libvca_create_COUNTERRESinfo( IN VCA_COUNTERS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_COUNTERRESinfo( OUT VCA_COUNTERS_T *_ptinfo, int _nchannelno );
int libvca_check_COUNTERRESinfo( int _nchannelno );

// STAB
static inline char* libvca_get_stabinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_STAB_INFO_PREFIX "%d" VCAFILE_STAB_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_basic_stabssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_STAB_SSI_PREFIX "%d_basic" VCAFILE_STAB_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_advanced_stabssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_STAB_SSI_PREFIX "%d_advanced" VCAFILE_STAB_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_stabssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_STAB_SSI_PREFIX "%d" VCAFILE_STAB_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_STABinfo( int _nParam, int _nchannelno, int defstype );
int libvca_create_STABinfo( STAB_UIOptions *pCfg, int _nchannelno );
int libvca_retrieve_STABinfo( STAB_UIOptions *pCfg, int _nchannelno );
int libvca_check_STABinfo( int _nchannelno );

/// Calibration
static inline char* libvca_get_calibinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CALIB_INFO_PREFIX "%d" VCAFILE_CALIB_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_calibssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CALIB_SSI_PREFIX "%d" VCAFILE_CALIB_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};

int libvca_setup_CALIBinfo( int _nParam, int _nchannelno );
int libvca_create_CALIBinfo( IN VCA_CALIB_ALL_INFO *tInfo, int _nchannelno );
int libvca_retrieve_CALIBinfo( OUT VCA_CALIB_ALL_INFO *tInfo, int _nchannelno );
int libvca_check_CALIBinfo( int _nchannelno );

// Object classification groups
static inline char* libvca_get_clsgrpinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CLSGRP_INFO_PREFIX "%d" VCAFILE_CLSGRP_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_clsgrpssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_CLSGRP_SSI_PREFIX "%d" VCAFILE_CLSGRP_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_CLSGRPinfo( int _nParam, int _nchannelno );
int libvca_create_CLSGRPinfo( IN VCA_CLSGRPS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_CLSGRPinfo( OUT VCA_CLSGRPS_T *_ptinfo, int _nchannelno );
int libvca_check_CLSGRPinfo( int _nchannelno );

// Tamper detection
static inline char* libvca_get_tamperinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_TAMPER_INFO_PREFIX "%d" VCAFILE_TAMPER_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_tamperssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_TAMPER_SSI_PREFIX "%d" VCAFILE_TAMPER_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_TAMPERinfo( int _nParam, int _nchannelno );
int libvca_create_TAMPERinfo( IN VCA_TAMPER_CONFIG *_ptinfo, int _nchannelno );
int libvca_retrieve_TAMPERinfo( OUT VCA_TAMPER_CONFIG *_ptinfo, int _nchannelno );
int libvca_check_TAMPERinfo( int _nchannelno );

// Scene change detection
static inline char* libvca_get_scenechangeinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_SCENECHANGE_INFO_PREFIX "%d" VCAFILE_SCENECHANGE_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_scenechangessi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_SCENECHANGE_SSI_PREFIX "%d" VCAFILE_SCENECHANGE_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_SCENECHANGEinfo( int _nParam, int _nchannelno );
int libvca_create_SCENECHANGEinfo( IN VCA_SCENECHANGE_CONFIG *_ptinfo, int _nchannelno );
int libvca_retrieve_SCENECHANGEinfo( OUT VCA_SCENECHANGE_CONFIG *_ptinfo, int _nchannelno );
int libvca_check_SCENECHANGEinfo( int _nchannelno );

// Burnt-in annotation
static inline char* libvca_get_biainfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_BIA_INFO_PREFIX "%d" VCAFILE_BIA_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_biassi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_BIA_SSI_PREFIX "%d" VCAFILE_BIA_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_BIAinfo( int _nParam, int _nchannelno );
int libvca_create_BIAinfo( IN VCA_BIAS_T *_ptinfo, int _nchannelno );
int libvca_retrieve_BIAinfo( OUT VCA_BIAS_T *_ptinfo, int _nchannelno );
int libvca_check_BIAinfo( int _nchannelno );

// XML Configuration 
unsigned int libvca_getFlagsFromXMLFile( char *pszFileName );
int libvca_loadCfgFromXMLFile( char *pszFileName, int nchannelno, unsigned int uiFlags );
int libvca_saveCfgtoXMLFile( char *pszFileName, int nchannelno, unsigned int uiFlags );

unsigned int libvca_getFlagsFromXMLBuffer( char *pszXMLBuffer, unsigned int uiBufLen );
int libvca_loadCfgFromXMLBuffer( char *pszXMLBuffer, unsigned int uiBufLen, int nchannelno, unsigned int uiFlags );
int libvca_saveCfgtoXMLBuffer( char *pszXMLBuffer, unsigned int *puiBufLen, int nchannelno, unsigned int uiFlags );

/*
// This function is obsolete, because there is no fixed slot for license any more, please use libvcasecurity_getLicenseSlot
// License slot lookup
unsigned int libvca_getLicenseSlot( unsigned short usPartCode );
*/

static inline char* libvca_get_xmlinfo_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_XML_INFO_PREFIX VCAFILE_XML_INFO_SUFFIX, libvca_get_infopath() );
	return filename;
};

static inline char* libvca_get_xmlssi_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_XML_SSI_PREFIX VCAFILE_XML_SSI_SUFFIX, libvca_get_webpath() );
	return filename;
};
int libvca_setup_XMLinfo( int _nParam );
int libvca_create_XMLinfo( IN VCA_XML_INFO_T *_ptinfo );
int libvca_retrieve_XMLinfo( OUT VCA_XML_INFO_T *_ptnfo );
int libvca_check_XMLinfo( );

// PTZ tracking configuration
static inline char* libvca_get_ptztinfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZT_INFO_PREFIX "%d" VCAFILE_PTZT_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_ptztssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZT_SSI_PREFIX "%d" VCAFILE_PTZT_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_PTZTinfo( int _nParam, int _nchannelno );
int libvca_create_PTZTinfo( IN VCA_PTZT_T *_ptinfo, int _nchannelno );
int libvca_retrieve_PTZTinfo( OUT VCA_PTZT_T *_ptinfo, int _nchannelno );
int libvca_check_PTZTinfo( int _nchannelno );

// PTZ global info
static inline char* libvca_get_ptzginfo_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZG_INFO_PREFIX VCAFILE_PTZG_INFO_SUFFIX, libvca_get_infopath() );
	return filename;
};
static inline char* libvca_get_ptztgssi_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZG_SSI_PREFIX VCAFILE_PTZG_SSI_SUFFIX, libvca_get_webpath() );
	return filename;
};
int libvca_setup_PTZGinfo( );
int libvca_create_PTZGinfo( IN VCA_PTZT_GLOBAL_INFO *_ptinfo );
int libvca_retrieve_PTZGinfo( OUT VCA_PTZT_GLOBAL_INFO *_ptinfo );
int libvca_check_PTZGinfo( );

// PTZ profile info
int libvca_retrieve_PTZPinfo( OUT VCA_PTZT_PROFILE *_ptinfo );
int libvca_create_PTZPinfo( IN VCA_PTZT_PROFILE *_ptinfo );
int libvca_setup_PTZPinfo( char *pszFileName );
int libvca_remove_PTZPinfo( char *pszFileName );
int libvca_check_autoPTZprofile( int _nchannelno );

static inline char* libvca_get_ptzprofile_ext( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, VCAFILE_PTZPROFILE_EXT );
	return filename;
};
static inline char* libvca_get_ptzprofile_original_path( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZPROFILE_DIR, libvca_get_binpath() );
	return filename;
};
static inline char* libvca_get_ptzprofile_path( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZPROFILE_DIR, libvca_get_infopath() );
	return filename;
};

// PTZ auto-calibration
static inline char* libvca_get_ptzcinfo_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZC_INFO_PREFIX VCAFILE_PTZC_INFO_SUFFIX, libvca_get_infopath() );
	return filename;
};
static inline char* libvca_get_ptzcssi_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_PTZC_SSI_PREFIX VCAFILE_PTZC_SSI_SUFFIX, libvca_get_webpath() );
	return filename;
};
int libvca_setup_PTZCinfo( int _nParam );
int libvca_create_PTZCinfo( IN VCA_PTZ_AUTOCALIB_INFO *_ptinfo );
int libvca_retrieve_PTZCinfo( OUT VCA_PTZ_AUTOCALIB_INFO *_ptinfo );
int libvca_check_PTZCinfo( );


// UID
static inline char* libvca_get_uidinfo_filename( )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_UID_INFO_PREFIX VCAFILE_UID_INFO_SUFFIX, libvca_get_infopath() );
	return filename;
};
int libvca_setup_UID( int _nParam );
int libvca_set_UID( IN unsigned int uiUID );
int libvca_get_UID( OUT unsigned int *puiUID );
unsigned int libvca_increment_UID( );

// CountDog configuration
static inline char* libvca_get_CountDoginfo_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTDOG_INFO_PREFIX "%d" VCAFILE_COUNTDOG_INFO_SUFFIX, libvca_get_infopath(), _nchannelno);
	return filename;
};
static inline char* libvca_get_CountDogssi_filename( int _nchannelno )
{
	static char filename[256];
	memset(filename, 0, 256);
	sprintf(filename, "%s" VCAFILE_COUNTDOG_SSI_PREFIX "%d" VCAFILE_COUNTDOG_SSI_SUFFIX, libvca_get_webpath(), _nchannelno);
	return filename;
};
int libvca_setup_CountDoginfo( int _nParam, int _nchannelno );
int libvca_create_CountDoginfo( IN VCA_COUNTDOG_INFO_T *_ptinfo, int _nchannelno );
int libvca_retrieve_CountDoginfo( OUT VCA_COUNTDOG_INFO_T *_ptinfo, int _nchannelno );
int libvca_check_CountDoginfo( int _nchannelno );
char *libvca_get_CountDogStoragePath(int iStorage);

//----------------------------------------------------------------

// Reset ALL to defaults
__attribute__((unused))
static int libvca_reset_ALLdefaults( int _nchannelno )
{
	int i;
	libvca_setup_UID( 1 );
	libvca_setup_BIAinfo( 1, _nchannelno );
	libvca_setup_CALIBinfo( 1, _nchannelno );
	libvca_setup_CHANNELinfo( 1, _nchannelno );
	libvca_setup_CLSGRPinfo( 1, _nchannelno );
	libvca_setup_COUNTERGENinfo( 1, _nchannelno );
	libvca_setup_COUNTERinfo( 1, _nchannelno );
	libvca_setup_COUNTERRESinfo( 1, _nchannelno );
	libvca_setup_EVENTinfo( 1, _nchannelno );
	libvca_setup_STABinfo( 1, _nchannelno, 2 );
	libvca_setup_TAMPERinfo( 1, _nchannelno );
	libvca_setup_SCENECHANGEinfo( 1, _nchannelno );
	libvca_setup_VCAinfo( 1 );
	libvca_setup_XMLinfo( 1 );
	libvca_setup_ZONEinfo( 1, _nchannelno );
	libvca_setup_PTZTinfo( 1, _nchannelno );
	libvca_setup_CountDoginfo( 1, _nchannelno );
	
	for( i = 0; i < MAX_NUM_LICENSES; i++ )
	{
		libvca_setup_LICinfo( 1, i );
	}

	return 0;
}


#ifdef __cplusplus
};
#endif // __cplusplus

#endif	// __libvca_H__

