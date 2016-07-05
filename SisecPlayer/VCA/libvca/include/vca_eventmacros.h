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

/*! \file	vca_eventmacros.h
 *  \brief	The VCA tracker include file for the event macro definitions.
 */
#ifndef _vca_eventmacros_h_
#define _vca_eventmacros_h_

#include "vca_normmacros.h"

//#define ALLOC_RULE_DATA( pData, vca_struct ) pData = (char *) malloc( sizeof (vca_struct) )
											
//#define FREE_RULE_DATA( pData ) free( pData )

#define ADDRULETOEVENT( Events, eventId, ruleId, vca_struct_value, vca_structtype, vca_ruletype ) \
	Events->pEvents[eventId].pRules[ruleId].eRuleType = vca_ruletype; \
	Events->pEvents[eventId].pRules[ruleId].iTotalTrkObjs = 0; \
	Events->pEvents[eventId].pRules[ruleId].uiRuleDataSize = sizeof( vca_structtype ); \
	Events->pEvents[eventId].pRules[ruleId].ucUsed = 1; \
	memcpy( Events->pEvents[eventId].pRules[ruleId].pRuleData, vca_struct_value, sizeof( vca_structtype ));\
	Events->pEvents[eventId].iNumRules++; 

 #define REMOVERULEFROMEVENT( Events, eventId, ruleId ) \
	Events->pEvents[eventId].pRules[ruleId].ucUsed = 0; \
	Events->pEvents[eventId].pRules[ruleId].uiRuleDataSize = 0; \
	Events->pEvents[eventId].iNumRules--; \
	REMOVEEMPTYEVENTS( Events, eventId );	

#define REMOVEEMPTYEVENTS( Events, eventId ) \
	if (Events->pEvents[eventId].iNumRules == 0) \
	{ \
		Events->pEvents[eventId].ucUsed = 0; \
		Events->uiTotalEvents--; \
	} 

#define GETFIELD( vca_struct, pData, field) (((vca_struct *)(pData))->##field)

// Serialize marcros
#define SERIALIZE_OBJCLS( iTotalTrkObjs, pObjcls, str ) \
	sprintf( str, "%d,", iTotalTrkObjs ); \
	str += strlen(str); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) iTotalTrkObjs; tIdx2++ ) \
	{ \
		sprintf( str, "%d,", pObjcls[tIdx2] ); \
		str += strlen(str); \
	} 

#define SERIALIZE_AREA( pData, str ) \
	VCA_RULE_AREAS_T *pAFs = (VCA_RULE_AREAS_T *)pData; \
	sprintf( str, "%d,%d,%d,", pAFs->usZoneId, pAFs->usZoneExt, pAFs->uiTotalFilters ); \
	str += strlen(str); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pAFs->uiTotalFilters; tIdx2++ ) \
	{ \
		sprintf( str, "%f:%f,", pAFs->pAreas[tIdx2].fAreaLo, pAFs->pAreas[tIdx2].fAreaUp ); \
		str += strlen(str); \
	} 

#define SERIALIZE_COLSIG( pData, str ) \
	VCA_RULE_COLSIG_T *pCSF = (VCA_RULE_COLSIG_T *)pData; \
	sprintf( str, "%d,%d,%d,", pCSF->usZoneId, pCSF->usZoneExt, pCSF->uiTotalFilters ); \
	str += strlen(str); \
	sprintf( str, "%d:%d,", pCSF->usColBin, pCSF->usThres ); \
	str += strlen(str); 
	
#define SERIALIZE_SPEED( pData, str ) \
	VCA_RULE_SPEEDS_T *pSFs = (VCA_RULE_SPEEDS_T *)pData; \
	sprintf( str, "%d,%d,%d,", pSFs->usZoneId, pSFs->usZoneExt, pSFs->uiTotalFilters ); \
	str += strlen(str); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pSFs->uiTotalFilters; tIdx2++ ) \
	{ \
		sprintf( str, "%f:%f,", pSFs->pSpeeds[tIdx2].fSpeedLo, pSFs->pSpeeds[tIdx2].fSpeedUp ); \
		str += strlen(str); \
	} 

#define SERIALIZE_DIRECTIONFILTER( pData, str ) \
	VCA_RULE_DIRECTIONFILTERS_T *pDFs = (VCA_RULE_DIRECTIONFILTERS_T *)pData; \
	sprintf( str, "%d,%d,%d,", pDFs->usZoneId, pDFs->usZoneExt, pDFs->uiTotalFilters ); \
	str += strlen(str); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pDFs->uiTotalFilters; tIdx2++ ) \
	{ \
		sprintf( str, "%d:%d,", pDFs->pDirections[tIdx2].startangle, pDFs->pDirections[tIdx2].finishangle); \
		str += strlen(str); \
	} 

#define SERIALIZE_TIMER( pData, str ) \
	VCA_RULE_TIMER_T *pTimer = (VCA_RULE_TIMER_T *)pData; \
	sprintf( str, "%d,%d,%d,", pTimer->usZoneId, pTimer->usFlags, pTimer->uiTimeThreshold); \
	str += strlen(str);

#define SERIALIZE_LINECOUNTER( pData, str ) \
	VCA_RULE_LINECOUNTER_T *pLineCounter = (VCA_RULE_LINECOUNTER_T *)pData; \
	sprintf( str, "%d,%d,%d,%d,", pLineCounter->usZoneId, pLineCounter->usZoneExt, pLineCounter->uiCalibrationWidth, pLineCounter->uiShadowFilterEnabled); \
	str += strlen(str);

#define SERIALIZE_BASIC( pData, str )\
	VCA_RULE_BASIC_T *pDT = (VCA_RULE_BASIC_T *)pData; \
	sprintf( str, "%d,%d,", pDT->usZoneId, pDT->usZoneExt ); \
	str += strlen(str);

#define SERIALIZE_EVENT( tEvent, str ) \
	int tIdx; \
	sprintf( str, "%d,%d,%d,%d,", tEvent.iEventId, tEvent.eType, tEvent.iNumRules, tEvent.ucEnable ); \
	str += strlen(str); \
	if ( strlen( tEvent.strEventName ) > 0 ) \
		strcat( str, tEvent.strEventName ); \
	else \
		strcat( str, VCA_EMPTY_SETTINGS ); \
	strcat(str, "\1"); \
	str += strlen(str); \
	for ( tIdx = 0; tIdx < tEvent.iNumRules; tIdx++ ) \
	{ \
		sprintf( str, "%d,", tEvent.pRules[tIdx].eRuleType ); \
		str += strlen(str); \
		SERIALIZE_OBJCLS( tEvent.pRules[tIdx].iTotalTrkObjs, tEvent.pRules[tIdx].ucTrkObjs, str ); \
		switch ( tEvent.pRules[tIdx].eRuleType ) \
		{ \
			case VCA_RULE_AREA: \
			case VCA_RULE_BASIC_AREA: \
			{ \
				SERIALIZE_AREA( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_SPEED: \
			case VCA_RULE_BASIC_SPEED: \
			{ \
				SERIALIZE_SPEED( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_COLSIG: \
			case VCA_RULE_BASIC_COLSIG: \
			{ \
				SERIALIZE_COLSIG( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_DIRECTIONFILTER: \
			case VCA_RULE_BASIC_DIRECTION: \
			{ \
				SERIALIZE_DIRECTIONFILTER( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_SMOKE: \
			case VCA_RULE_FIRE: \
			case VCA_RULE_TAILGATING: \
			case VCA_RULE_ABOBJ: \
			case VCA_RULE_RMOBJ: \
			case VCA_RULE_ABRMOBJ: \
			case VCA_RULE_STOP: \
			case VCA_RULE_LOITERING: \
			case VCA_RULE_BASIC_TIMER: \
			{ \
				SERIALIZE_TIMER( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_LINECOUNTER_A: \
			case VCA_RULE_LINECOUNTER_B: \
			{ \
				SERIALIZE_LINECOUNTER( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			case VCA_RULE_PRESENCE: \
			case VCA_RULE_ENTER: \
			case VCA_RULE_EXIT: \
			case VCA_RULE_APPEAR: \
			case VCA_RULE_DISAPPEAR: \
			case VCA_RULE_BASIC_PRESENCE:\
			case VCA_RULE_BASIC_DISPRESENCE: \
			case VCA_RULE_BASIC_ENTER: \
			case VCA_RULE_BASIC_EXIT: \
			case VCA_RULE_BASIC_APPEAR: \
			case VCA_RULE_BASIC_DISAPPEAR: \
			case VCA_RULE_BASIC_STATIONARY: \
			{ \
				SERIALIZE_BASIC( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
			default: \
			{ \
				SERIALIZE_BASIC( tEvent.pRules[tIdx].pRuleData, str ); \
				break; \
			} \
		} \
		sprintf( str, "%d,", tEvent.pLogics[tIdx]); \
		str += strlen(str);\
	}\
	sprintf( str, "%d,", tEvent.uiUID ); \
	str += strlen(str); \
	if ( strlen( tEvent.strUserData ) > 0 ) \
		strcat( str, tEvent.strUserData ); \
	else \
		strcat( str, VCA_EMPTY_SETTINGS );

// Deserialize macros
#define DESERIALIZE_INT( data, pStr ) \
	{ \
	char token[3]=":,"; \
	char szTmp[800]; \
	char *szResult; \
	szResult = strtok( pStr, token ); \
	if ( szResult ) { \
		strcpy(szTmp, ""); \
		strncpy( szTmp, szResult, 20); \
		data =  atoi( szTmp ); } \
	else { \
		data = 0; } \
	}

#define DESERIALIZE_FLOAT( data, pStr ) \
	{ \
	char token[3]=":,"; \
	char szTmp[800]; \
	char *szResult; \
	szResult = strtok( pStr, token ); \
	strcpy(szTmp, ""); \
	strncpy( szTmp, szResult, 20); \
	data =  atof( szTmp ); \
	}

#define DESERIALIZE_STRING( data, pStr, string_mode ) \
	{ \
	char *szResult; \
	if (string_mode  != NULL) \
		szResult = strtok( pStr, "\1" ); \
	else \
		szResult = strtok( pStr, ":," ); \
	strcpy( data, "" ); \
	if ( szResult && ( strcmp( szResult, VCA_EMPTY_SETTINGS ) != 0 ) ) strncpy( data, szResult, sizeof(data) ); \
	}


#define DESERIALIZE_AREA( pData ) \
	VCA_RULE_AREAS_T *pAFs = (VCA_RULE_AREAS_T *)pData; \
	DESERIALIZE_INT( pAFs->usZoneId, NULL ); \
	DESERIALIZE_INT( pAFs->usZoneExt, NULL ); \
	DESERIALIZE_INT( pAFs->uiTotalFilters, NULL ); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pAFs->uiTotalFilters; tIdx2++) \
	{ \
		DESERIALIZE_FLOAT( pAFs->pAreas[tIdx2].fAreaLo, NULL ); \
		DESERIALIZE_FLOAT( pAFs->pAreas[tIdx2].fAreaUp, NULL ); \
	}

#define DESERIALIZE_COLSIG( pData ) \
	VCA_RULE_COLSIG_T *pCSF = (VCA_RULE_COLSIG_T *)pData; \
	DESERIALIZE_INT( pCSF->usZoneId, NULL ); \
	DESERIALIZE_INT( pCSF->usZoneExt, NULL ); \
	DESERIALIZE_INT( pCSF->uiTotalFilters, NULL ); \
	DESERIALIZE_INT( pCSF->usColBin, NULL ); \
	DESERIALIZE_INT( pCSF->usThres, NULL ); 

#define DESERIALIZE_SPEED( pData ) \
	VCA_RULE_SPEEDS_T *pSFs = (VCA_RULE_SPEEDS_T *)pData; \
	DESERIALIZE_INT( pSFs->usZoneId, NULL ); \
	DESERIALIZE_INT( pSFs->usZoneExt, NULL ); \
	DESERIALIZE_INT( pSFs->uiTotalFilters, NULL ); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pSFs->uiTotalFilters; tIdx2++) \
	{ \
		DESERIALIZE_FLOAT( pSFs->pSpeeds[tIdx2].fSpeedLo, NULL ); \
		DESERIALIZE_FLOAT( pSFs->pSpeeds[tIdx2].fSpeedUp, NULL ); \
	}

#define DESERIALIZE_DIRECTIONFILTER( pData ) \
	VCA_RULE_DIRECTIONFILTERS_T *pDFs = (VCA_RULE_DIRECTIONFILTERS_T *)pData; \
	DESERIALIZE_INT( pDFs->usZoneId, NULL ); \
	DESERIALIZE_INT( pDFs->usZoneExt, NULL ); \
	DESERIALIZE_INT( pDFs->uiTotalFilters, NULL ); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) pDFs->uiTotalFilters; tIdx2++) \
	{ \
		DESERIALIZE_INT( pDFs->pDirections[tIdx2].startangle, NULL ); \
		DESERIALIZE_INT( pDFs->pDirections[tIdx2].finishangle, NULL ); \
	}

#define DESERIALIZE_TIMER( pData ) \
	VCA_RULE_TIMER_T *pTimer = (VCA_RULE_TIMER_T *)pData; \
	DESERIALIZE_INT( pTimer->usZoneId, NULL ); \
	DESERIALIZE_INT( pTimer->usFlags, NULL ); \
	DESERIALIZE_INT( pTimer->uiTimeThreshold, NULL );

#define DESERIALIZE_BASIC( pData )\
	VCA_RULE_BASIC_T *pDT = (VCA_RULE_BASIC_T *)pData; \
	DESERIALIZE_INT( pDT->usZoneId, NULL ); \
	DESERIALIZE_INT( pDT->usZoneExt, NULL );

#define DESERIALIZE_OBJCLS( iTotalTrkObjs, pObjcls ) \
	DESERIALIZE_INT( iTotalTrkObjs, NULL ); \
	int tIdx2; \
	for ( tIdx2 = 0; tIdx2 < (int) iTotalTrkObjs; tIdx2++ ) \
	{ \
		DESERIALIZE_INT( pObjcls[tIdx2], NULL ); \
	} 

#define DESERIALIZE_LINECOUNTER( pData ) \
	VCA_RULE_LINECOUNTER_T *pLineCounter = (VCA_RULE_LINECOUNTER_T *)pData; \
	DESERIALIZE_INT( pLineCounter->usZoneId, NULL ); \
	DESERIALIZE_INT( pLineCounter->usZoneExt, NULL ); \
	DESERIALIZE_INT( pLineCounter->uiCalibrationWidth, NULL ); \
	DESERIALIZE_INT( pLineCounter->uiShadowFilterEnabled, NULL ); 

#define DESERIALIZE_EVENT( tEvent, str ) \
	int tIdx; \
	int iType; \
	char* string_mode = strstr(str, "\1"); \
	DESERIALIZE_INT( tEvent.iEventId, str  ); \
	DESERIALIZE_INT( iType, NULL  ); \
	tEvent.eType = ( VCA_EVENTTYPE_T ) iType; \
	DESERIALIZE_INT( tEvent.iNumRules, NULL ); \
	DESERIALIZE_INT( tEvent.ucEnable, NULL ); \
	DESERIALIZE_STRING( tEvent.strEventName, NULL, string_mode ); \
	tEvent.ucUsed = 1; \
	for ( tIdx = 0; tIdx < tEvent.iNumRules; tIdx++ ) \
	{ \
		DESERIALIZE_INT( iType, NULL ); \
		tEvent.pRules[tIdx].eRuleType = (VCA_RULE_E) iType; \
		DESERIALIZE_OBJCLS( tEvent.pRules[tIdx].iTotalTrkObjs, tEvent.pRules[tIdx].ucTrkObjs ); \
		tEvent.pRules[tIdx].ucUsed = 1; \
		switch ( tEvent.pRules[tIdx].eRuleType ) \
		{ \
			case VCA_RULE_AREA: \
			case VCA_RULE_BASIC_AREA: \
			{ \
				DESERIALIZE_AREA( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_AREAS_T ); \
				break; \
			} \
			case VCA_RULE_SPEED: \
			case VCA_RULE_BASIC_SPEED: \
			{ \
				DESERIALIZE_SPEED( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_SPEEDS_T ); \
				break; \
			} \
			case VCA_RULE_COLSIG: \
			case VCA_RULE_BASIC_COLSIG: \
			{ \
				DESERIALIZE_COLSIG( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_COLSIG_T ); \
				break; \
			} \
			case VCA_RULE_DIRECTIONFILTER: \
			case VCA_RULE_BASIC_DIRECTION: \
			{ \
				DESERIALIZE_DIRECTIONFILTER( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_DIRECTIONFILTERS_T ); \
				break; \
			} \
			case VCA_RULE_SMOKE: \
			case VCA_RULE_FIRE: \
			case VCA_RULE_TAILGATING: \
			case VCA_RULE_ABOBJ: \
			case VCA_RULE_RMOBJ: \
			case VCA_RULE_ABRMOBJ: \
			case VCA_RULE_STOP: \
			case VCA_RULE_LOITERING: \
			case VCA_RULE_BASIC_TIMER: \
			{ \
				DESERIALIZE_TIMER( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_TIMER_T ); \
				break; \
			} \
			case VCA_RULE_LINECOUNTER_A: \
			case VCA_RULE_LINECOUNTER_B: \
			{ \
				DESERIALIZE_LINECOUNTER( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_LINECOUNTER_T ); \
				break; \
			} \
			case VCA_RULE_PRESENCE: \
			case VCA_RULE_ENTER: \
			case VCA_RULE_EXIT: \
			case VCA_RULE_APPEAR: \
			case VCA_RULE_DISAPPEAR: \
			case VCA_RULE_BASIC_PRESENCE:\
			case VCA_RULE_BASIC_DISPRESENCE: \
			case VCA_RULE_BASIC_ENTER: \
			case VCA_RULE_BASIC_EXIT: \
			case VCA_RULE_BASIC_APPEAR: \
			case VCA_RULE_BASIC_DISAPPEAR: \
			case VCA_RULE_BASIC_STATIONARY: \
			{ \
				DESERIALIZE_BASIC( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_BASIC_T ); \
				break; \
			} \
			default: \
			{ \
				DESERIALIZE_BASIC( tEvent.pRules[tIdx].pRuleData ); \
				tEvent.pRules[tIdx].uiRuleDataSize = sizeof( VCA_RULE_BASIC_T ); \
				break; \
			} \
		} \
		DESERIALIZE_INT( iType, NULL ); \
		tEvent.pLogics[tIdx] = (VCA_LOGIC_T) iType; \
	} \
	DESERIALIZE_INT( tEvent.uiUID, NULL ); \
	DESERIALIZE_STRING( tEvent.strUserData, NULL, string_mode );
	

//#define GETZONEIDFROMRULE( eventId, ruleId, vca_ruletype) ((vca_ruletype##_T *) m_pEvents->pEvents[eventId].pRules[ruleId].pRuleData)->usZoneId


#endif // _vca_eventmacros_h_
