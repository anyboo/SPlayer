// CVCAConfigure.cpp: implementation of the CVCAConfigure class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VCAConfigure.h"
#include "libvca/include/libvca.h"

#include "XMLUtils.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCAConfigure* CVCAConfigure::m_pInstance = NULL;

CVCAConfigure::CVCAConfigure()
{
	CoInitialize(NULL); 
	m_bLoad		= FALSE;
	m_EngCnt	= 0;

	m_EngineConfig	= new VCA5_APP_ENGINE_CONFIG[VCA5_MAX_NUM_ENGINE]; 
	memset(m_EngineConfig, 0, sizeof(VCA5_APP_ENGINE_CONFIG)*VCA5_MAX_NUM_ENGINE);

	m_EngCnt = 0xFFFFFFFF;
}

CVCAConfigure::~CVCAConfigure()
{
	delete [] m_EngineConfig;
	CoUninitialize();
}

// static
CVCAConfigure *CVCAConfigure::Instance()
{
	if( !m_pInstance )
	{
		m_pInstance = new CVCAConfigure();
	}

	return m_pInstance;
}


HRESULT CVCAConfigure::LoadZones(DWORD dwEngId, VCA5_APP_ZONES *pZones)
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	//---------------------------------------------------------------
	// Zones
	VCA_ZONES_T		tZones;

	if( LIBVCA_ERROR != libvca_retrieve_ZONEinfo( &tZones, dwEngId ) )
	{
		pZones->ulTotalZones	= tZones.uiTotalZones;

		int idx = 0;
		for( unsigned int i = 0; i < MAX_NUM_ZONES; i++ )
		{
			if( IsZoneUsed( &tZones.pZones[i] ) )
			{
				VCA5_APP_ZONE	*pZone = &(pZones->pZones[idx++]);
				pZone->usZoneId		= tZones.pZones[i].usZoneId;
				pZone->usZoneType	= tZones.pZones[i].usZoneType;
				pZone->usZoneStyle	= tZones.pZones[i].usZoneStyle;
				pZone->uiColor		= _byteswap_ulong( tZones.pZones[i].uiColor << 8 ); // RGB -> BGR
				strcpy( pZone->szZoneName, tZones.pZones[i].strZoneName );

				pZone->ulTotalPoints	= tZones.pZones[i].uiTotalPoints;
				for( unsigned int j = 0; j < tZones.pZones[i].uiTotalPoints; j++ )
				{
					pZone->pPoints[j].x	= tZones.pZones[i].pPoints[j].x;
					pZone->pPoints[j].y	= tZones.pZones[i].pPoints[j].y;
				}
			}
		}

		hr = S_OK;
	}

	return hr;
}


HRESULT CVCAConfigure::SaveZones(DWORD dwEngId, VCA5_APP_ZONES *pZones)
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	VCA_ZONES_T	tZones;
//	libvca_retrieve_ZONEinfo( &tZones, dwEngId );
	memset( &tZones, 0, sizeof( tZones ) );

	tZones.uiTotalZones	= pZones->ulTotalZones;

	for( unsigned int i = 0; i < pZones->ulTotalZones; i++ )
	{
		VCA5_APP_ZONE *pZone = &(pZones->pZones[i]);

		tZones.pZones[i].usZoneId		= pZone->usZoneId;
		tZones.pZones[i].usZoneType		= pZone->usZoneType;
		tZones.pZones[i].usZoneStyle	= pZone->usZoneStyle;
		tZones.pZones[i].uiColor		= _byteswap_ulong( pZone->uiColor ) >> 8;	// BGR -> RGB
		tZones.pZones[i].ucUsed			= 1;
		tZones.pZones[i].ucEnable		= 1;

		strncpy( tZones.pZones[i].strZoneName, pZone->szZoneName, MAX_NUM_NAME);
		tZones.pZones[i].strZoneName[MAX_NUM_NAME-1] = 0;

		tZones.pZones[i].uiTotalPoints	= pZone->ulTotalPoints;
		for( unsigned int j = 0; j < pZone->ulTotalPoints; j++ )
		{
			tZones.pZones[i].pPoints[j].x = pZone->pPoints[j].x;
			tZones.pZones[i].pPoints[j].y = pZone->pPoints[j].y;
		}
	}

	if( LIBVCA_ERROR != libvca_create_ZONEinfo( &tZones, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::LoadRules( DWORD dwEngId, VCA5_APP_RULES *pRules )
{
	HRESULT hr = E_FAIL;

	//--------------------------------------------------------------------
	// Rules
	VCA_EVENTS_T	tEvents;

	if( LIBVCA_ERROR != libvca_retrieve_EVENTinfo( &tEvents, dwEngId ) )
	{
		pRules->ulTotalRules	= tEvents.uiTotalEvents;

		int idx = 0;
		for( unsigned int i = 0; i < MAX_NUM_EVENTS; i++ )
		{
			if( IsEventUsed(&tEvents.pEvents[i]) )
			{
				VCA5_APP_RULE *pRule = &(pRules->pRules[idx++]);
				pRule->usRuleType	= tEvents.pEvents[i].pRules[0].eRuleType;
				pRule->ulRuleId		= tEvents.pEvents[i].iEventId;
				strcpy(pRule->szRuleName, tEvents.pEvents[i].strEventName);

				switch( pRule->usRuleType )
				{
					case VCA5_RULE_TYPE_PRESENCE:
					case VCA5_RULE_TYPE_ENTER:
					case VCA5_RULE_TYPE_EXIT:
					case VCA5_RULE_TYPE_APPEAR:
					case VCA5_RULE_TYPE_DISAPPEAR:
					{
						VCA_RULE_BASIC_T *pData = (VCA_RULE_BASIC_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId = pData->usZoneId;
					}
					break;

					case VCA5_RULE_TYPE_STOP:
					case VCA5_RULE_TYPE_DWELL:
					//case VCA5_RULE_TYPE_ABOBJ:
					case VCA5_RULE_TYPE_RMOBJ:
					case VCA5_RULE_TYPE_SMOKE:
					case VCA5_RULE_TYPE_FIRE:
					{
						VCA_RULE_TIMER_T *pData = (VCA_RULE_TIMER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId								= pData->usZoneId;
						pRule->tRuleDataEx.tTimer.ulTimeThreshold	= pData->uiTimeThreshold;
					}
					break;

					case VCA5_RULE_TYPE_DIRECTION:
					{
						VCA_RULE_DIRECTIONFILTERS_T *pData = (VCA_RULE_DIRECTIONFILTERS_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId		= pData->usZoneId;
						pRule->tRuleDataEx.tDirection.sStartAngle	= pData->pDirections->startangle;
						pRule->tRuleDataEx.tDirection.sFinishAngle	= pData->pDirections->finishangle;
					}
					break;

					case  VCA5_RULE_TYPE_SPEED:
					{
						VCA_RULE_SPEEDS_T	*pData = (VCA_RULE_SPEEDS_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId		= pData->usZoneId;
						pRule->tRuleDataEx.tSpeed.usSpeedLo		= (USHORT)pData->pSpeeds->fSpeedLo;
						pRule->tRuleDataEx.tSpeed.usSpeedUp		= (USHORT)pData->pSpeeds->fSpeedUp;
					}
					break;

					case VCA5_RULE_TYPE_TAILGATING:
					{
						VCA_RULE_TIMER_T *pData = (VCA_RULE_TIMER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId								= pData->usZoneId;
						pRule->tRuleDataEx.tTimer.ulTimeThreshold	= pData->uiTimeThreshold;
						pRule->tRuleDataEx.tTimer.usFlags			= pData->usFlags;
						pRule->tRuleDataEx.tTimer.usZoneId			= pData->usZoneId;
					}
					break;

					case VCA5_RULE_TYPE_LINECOUNTER_A:
					case VCA5_RULE_TYPE_LINECOUNTER_B:
					{
						VCA_RULE_LINECOUNTER_T	*pData = (VCA_RULE_LINECOUNTER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId										= pData->usZoneId;
						pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth	= pData->uiCalibrationWidth;
						pRule->tRuleDataEx.tLineCounter.ulShadowFilterEnabled = pData->uiShadowFilterEnabled;
						pRule->ucShadowFilterEnabled = pData->uiShadowFilterEnabled;
						if(pData->uiCalibrationWidth != 0) pRule->ucWidthCalibrationEnabled = TRUE;
						else pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth = 4000; //default 
					}
					break;

					case VCA5_RULE_TYPE_COLSIG:
					{
						VCA_RULE_COLSIG_T *pData = (VCA_RULE_COLSIG_T *)tEvents.pEvents[i].pRules[0].pRuleData;
						pRule->usZoneId								= pData->usZoneId;
						pRule->tRuleDataEx.tColorFilter.usColBin	= pData->usColBin;
						pRule->tRuleDataEx.tColorFilter.usThreshold	= pData->usThres;
					}
					break;

					default:
					{
						// Unknown rule type
						ASSERT( FALSE );
					}
					break;
				}

				// ** FRIG ALERT **//
				// Use internal strange rule id
				// BW: Note - this statment makes me unhappy :(, because the orginal design of the
				// internal data strcutures means that as soon as the number of rules changes,
				// your config files are not compatible from one version to the next
			//	pRule->ulRuleId = (pRule->usZoneId*VCA5_RULE_TYPE_NUM) + pRule->usRuleType;

				pRule->usTotalTrkObjs = tEvents.pEvents[i].pRules[0].iTotalTrkObjs;
				for( unsigned int j = 0; j < pRule->usTotalTrkObjs; j++ )
				{
					pRule->ucTrkObjs[j] = tEvents.pEvents[i].pRules[0].ucTrkObjs[j];
				}
			}
		}

		hr = S_OK;
	}

	return hr;
}


HRESULT CVCAConfigure::SaveRules( DWORD dwEngId, VCA5_APP_RULES *pRules )
{
	HRESULT hr = E_FAIL;

	VCA_EVENTS_T tEvents;

	memset( &tEvents, 0, sizeof( tEvents ) );

//	libvca_retrieve_EVENTinfo( &tEvents, dwEngId );

	tEvents.uiTotalEvents = pRules->ulTotalRules;
	for( unsigned int i = 0; i < pRules->ulTotalRules; i++ )
	{
		VCA5_APP_RULE *pRule = &(pRules->pRules[i]);
		tEvents.pEvents[i].iEventId				= pRule->ulRuleId;
		tEvents.pEvents[i].ucUsed				= 1;
		tEvents.pEvents[i].ucEnable				= 1;
		tEvents.pEvents[i].iNumRules			= 1;
		tEvents.pEvents[i].pLogics[0]			= VCA_LOGIC_AND;
		tEvents.pEvents[i].pRules[0].eRuleType	= (VCA_RULE_E)pRule->usRuleType;
		tEvents.pEvents[i].eType				= VCA_EVENTTYPE_PREDEFINED;

		strcpy(tEvents.pEvents[i].strEventName, pRule->szRuleName);
		int iZoneId = -1;

		switch( pRule->usRuleType )
		{
			case VCA5_RULE_TYPE_PRESENCE:
			case VCA5_RULE_TYPE_ENTER:
			case VCA5_RULE_TYPE_EXIT:
			case VCA5_RULE_TYPE_APPEAR:
			case VCA5_RULE_TYPE_DISAPPEAR:
			{
				VCA_RULE_BASIC_T	*pData = (VCA_RULE_BASIC_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId		= pRule->usZoneId;
				iZoneId				= pRule->usZoneId;
			}
			break;

			case VCA5_RULE_TYPE_STOP:
			case VCA5_RULE_TYPE_DWELL:
			//case VCA5_RULE_TYPE_ABOBJ:
			case VCA5_RULE_TYPE_RMOBJ:
			case VCA5_RULE_TYPE_SMOKE:
			case VCA5_RULE_TYPE_FIRE:
			{
				VCA_RULE_TIMER_T *pData = (VCA_RULE_TIMER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId = pRule->usZoneId;
				pData->uiTimeThreshold = pRule->tRuleDataEx.tTimer.ulTimeThreshold;

				iZoneId				= pRule->usZoneId;
			}
			break;

			case VCA5_RULE_TYPE_DIRECTION:
			{
				VCA_RULE_DIRECTIONFILTERS_T *pData = (VCA_RULE_DIRECTIONFILTERS_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId = pRule->usZoneId;
				pData->uiTotalFilters			= 1;
				pData->pDirections->startangle	= pRule->tRuleDataEx.tDirection.sStartAngle;
				pData->pDirections->finishangle = pRule->tRuleDataEx.tDirection.sFinishAngle;

				iZoneId				= pRule->usZoneId;
			}
			break;

			case  VCA5_RULE_TYPE_SPEED:
			{
				VCA_RULE_SPEEDS_T	*pData = (VCA_RULE_SPEEDS_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId = pRule->usZoneId;
				pData->uiTotalFilters		= 1;
				pData->pSpeeds->fSpeedLo	= pRule->tRuleDataEx.tSpeed.usSpeedLo;
				pData->pSpeeds->fSpeedUp	= pRule->tRuleDataEx.tSpeed.usSpeedUp;

				iZoneId				= pRule->usZoneId;
			}
			break;

			case  VCA5_RULE_TYPE_TAILGATING:
			{
				VCA_RULE_TIMER_T *pData = (VCA_RULE_TIMER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId			= pRule->usZoneId;
				pData->uiTimeThreshold	= pRule->tRuleDataEx.tTimer.ulTimeThreshold;
				pData->usFlags			= pRule->tRuleDataEx.tTimer.usFlags;

				iZoneId				= pRule->usZoneId;
			}
			break;

			case VCA5_RULE_TYPE_LINECOUNTER_A:
			case VCA5_RULE_TYPE_LINECOUNTER_B:
			{
				tEvents.pEvents[i].pRules[0].uiRuleDataSize	= sizeof(VCA_RULE_LINECOUNTER_T);
				VCA_RULE_LINECOUNTER_T *pData = (VCA_RULE_LINECOUNTER_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId				= pRule->usZoneId;
				pData->usZoneExt			= 0;
				if(pRule->ucWidthCalibrationEnabled)
					pData->uiCalibrationWidth	= pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth;
				else
					pData->uiCalibrationWidth	= 0;
				pData->uiShadowFilterEnabled= pRule->ucShadowFilterEnabled;
				iZoneId				= pRule->usZoneId;
			}
			break;

			case VCA5_RULE_TYPE_COLSIG:{
				VCA_RULE_COLSIG_T *pData = (VCA_RULE_COLSIG_T *)tEvents.pEvents[i].pRules[0].pRuleData;
				pData->usZoneId			= pRule->usZoneId;
				pData->usZoneExt		= 0;
				pData->uiTotalFilters	= 1;
				pData->usColBin			= pRule->tRuleDataEx.tColorFilter.usColBin;
				pData->usThres			= pRule->tRuleDataEx.tColorFilter.usThreshold;

				iZoneId				= pRule->usZoneId;
			}
			break;

			default:
			{
				// Unknown rule type
				ASSERT( FALSE );
			}
			break;

		}

		if( 0 == strlen( tEvents.pEvents[i].strEventName ) )
		{
			// Rule doesn't have a name - add one now
			const char *pszRuleName = "Unknown";
			if( pRule->usRuleType > 0 && pRule->usRuleType < (sizeof( VCA_RULE_STR ) / sizeof( VCA_RULE_STR[0])) )
			{
				pszRuleName = VCA_RULE_STR[pRule->usRuleType];
			}

			sprintf( tEvents.pEvents[i].strEventName, "Zone %d-%s", iZoneId, pszRuleName );
		}

		tEvents.pEvents[i].pRules[0].iTotalTrkObjs = pRule->usTotalTrkObjs;
		for( unsigned int j = 0; j < pRule->usTotalTrkObjs; j++ )
		{
			tEvents.pEvents[i].pRules[0].ucTrkObjs[j] = pRule->ucTrkObjs[j];
		}
	}

	if( LIBVCA_ERROR != libvca_create_EVENTinfo( &tEvents, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}


HRESULT CVCAConfigure::LoadCounters( DWORD dwEngId, VCA5_APP_COUNTERS *pCounters )
{
	USES_CONVERSION;
	HRESULT hr = E_FAIL;
	//-----------------------------------------------------
	// Counters
	VCA_COUNTERS_T tCounters;

	if( LIBVCA_ERROR != libvca_retrieve_COUNTERinfo( &tCounters, dwEngId ) )
	{
		pCounters->ulTotalCounters = tCounters.uiTotalCounters;

		int idx = 0;
		for( unsigned int i = 0; i < MAX_NUM_COUNTERS; i++ )
		{
			if( IsCounterUsed( &tCounters.pCounters[i] ) )
			{
				VCA5_APP_COUNTER *pCtr = &(pCounters->pCounters[idx++]);
				pCtr->usCounterId		= tCounters.pCounters[i].usCounterId;
				pCtr->uiColor			= tCounters.pCounters[i].uiColor;
				pCtr->usNumSubCounters	= tCounters.pCounters[i].usNumSubCounters;
				strcpy( pCtr->szCounterName, tCounters.pCounters[i].strCounterName );
				pCtr->iCounterResult	= tCounters.pCounters[i].iCounterResult;

				for( int j = 0; j < pCtr->usNumSubCounters; j++ )
				{
					VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);
					pSc->usSubCounterType	= tCounters.pCounters[i].pSubCounters[j].usSubCounterType;
					pSc->usTrigId			= tCounters.pCounters[i].pSubCounters[j].usTrigId;

					pCtr->uiSubCounterStatuses[j] = VCA5_APP_SUBCOUNTER_STATUS_TICKED|VCA5_APP_SUBCOUNTER_STATUS_USED;
				}

				pCtr->ulTotalPoints	= 5;
				for( int j = 0; j < 5; j++ )
				{
					pCtr->pPoints[j].x	= tCounters.pCounters[i].pPoints[j].x;
					pCtr->pPoints[j].y	= tCounters.pCounters[i].pPoints[j].y;
				}
			}
		}

		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::SaveCounters( DWORD dwEngId, VCA5_APP_COUNTERS *pCounters )
{
	USES_CONVERSION;
	HRESULT hr = E_FAIL;

	VCA_COUNTERS_T tCounters;
	
	memset( &tCounters, 0, sizeof( tCounters ) );
	//libvca_retrieve_COUNTERinfo( &tCounters, dwEngId );

	tCounters.uiTotalCounters = pCounters->ulTotalCounters;

	for( unsigned int i = 0; i < tCounters.uiTotalCounters; i++ )
	{
		VCA5_APP_COUNTER *pCtr = &(pCounters->pCounters[i]);

		tCounters.pCounters[i].usCounterId		= pCtr->usCounterId;
		tCounters.pCounters[i].uiColor			= pCtr->uiColor;
		tCounters.pCounters[i].usNumSubCounters	= pCtr->usNumSubCounters;
		tCounters.pCounters[i].ucUsed			= 1;
		tCounters.pCounters[i].ucEnable			= 1;
		strncpy( tCounters.pCounters[i].strCounterName, pCtr->szCounterName, MAX_NUM_NAME);
		tCounters.pCounters[i].strCounterName[MAX_NUM_NAME-1] = 0;

		tCounters.pCounters[i].iCounterResult	= pCtr->iCounterResult;

		for( int j = 0; j < pCtr->usNumSubCounters; j++ )
		{
			VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);
			tCounters.pCounters[i].pSubCounters[j].usSubCounterType	= pSc->usSubCounterType;
			tCounters.pCounters[i].pSubCounters[j].usTrigId			= pSc->usTrigId;
			tCounters.pCounters[i].pSubCounters[j].ucUsed			= 1;
			tCounters.pCounters[i].pSubCounters[j].ucEnable			= 1;
			tCounters.pCounters[i].pSubCounters[j].usSubCounterId	= j;


			// TBD - status
		}

		pCtr->ulTotalPoints	= 5;
		for( int j = 0; j < 5; j++ )
		{
			tCounters.pCounters[i].pPoints[j].x = pCtr->pPoints[j].x;
			tCounters.pCounters[i].pPoints[j].y = pCtr->pPoints[j].y;
		}
	}

	if( LIBVCA_ERROR != libvca_create_COUNTERinfo( &tCounters, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::LoadCalibration( DWORD dwEngId, VCA5_APP_CALIB_INFO *pCalibInfo )
{
	HRESULT hr = E_FAIL;

	VCA_CALIB_ALL_INFO tInfo;
	
	if( LIBVCA_ERROR != libvca_retrieve_CALIBinfo( &tInfo, dwEngId ) )
	{
		pCalibInfo->calibrationStatus	= tInfo.calibrationStatus;
		pCalibInfo->fFOV				= (float)tInfo.info.fFOV;
		pCalibInfo->fHeight				= (float)tInfo.info.fHeight;
		pCalibInfo->fRoll				= (float)tInfo.info.fRoll;
		pCalibInfo->fTilt				= (float)tInfo.info.fTilt;
		pCalibInfo->fPan				= (float)tInfo.info.fPan;

		hr = S_OK;
	}

	VCA_CHANNEL_INFO_T tChanInfo;
	if( LIBVCA_ERROR != libvca_retrieve_CHANNELinfo( &tChanInfo, dwEngId ) )
	{
		if( MEAUNIT_METRIC == tChanInfo.nMeaUnits )
		{
			pCalibInfo->ulHeightUnits	= VCA5_HEIGHT_UNITS_METERS;
			pCalibInfo->ulSpeedUnits	= VCA5_SPEED_UNITS_KPH;
		}
		else
		{
			pCalibInfo->ulHeightUnits	= VCA5_HEIGHT_UNITS_FEET;
			pCalibInfo->ulSpeedUnits	= VCA5_SPEED_UNITS_MPH;
		}
	}

	return S_OK;
}

HRESULT CVCAConfigure::SaveCalibration( DWORD dwEngId, VCA5_APP_CALIB_INFO *pCalibInfo )
{
	HRESULT hr = E_FAIL;
	VCA_CALIB_ALL_INFO tInfo;
	VCA_CHANNEL_INFO_T tChanInfo;

	libvca_retrieve_CALIBinfo( &tInfo, dwEngId );
	libvca_retrieve_CHANNELinfo( &tChanInfo, dwEngId );

	tInfo.calibrationStatus		= pCalibInfo->calibrationStatus;
	tInfo.info.fFOV				= pCalibInfo->fFOV;
	tInfo.info.fHeight			= pCalibInfo->fHeight;
	tInfo.info.fRoll			= pCalibInfo->fRoll;
	tInfo.info.fTilt			= pCalibInfo->fTilt;
	tInfo.info.fPan				= pCalibInfo->fPan;


	if( VCA5_HEIGHT_UNITS_METERS == pCalibInfo->ulHeightUnits )
	{
		tChanInfo.nMeaUnits = MEAUNIT_METRIC;
	}
	else
	{
		tChanInfo.nMeaUnits = MEAUNIT_IMPERIAL;
	}

	libvca_create_CHANNELinfo( &tChanInfo, dwEngId );
	if( libvca_create_CALIBinfo( &tInfo, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::LoadObjects( DWORD dwEngId, VCA5_APP_CLSOBJECTS *pObjs )
{
	HRESULT hr = E_FAIL;
	VCA_CLSGRPS_T tGrps;
	USES_CONVERSION;

	if( LIBVCA_ERROR != libvca_retrieve_CLSGRPinfo( &tGrps, dwEngId ) )
	{
		VCA_CLSOBJECTS_T *pClsObjs = &(tGrps.pClsgrps[0].tClsobjects);

		pObjs->ulTotalClsObjects = pClsObjs->uiTotalClsobjects;
		for( unsigned int i = 0; i < pObjs->ulTotalClsObjects; i++ )
		{
			VCA5_APP_CLSOBJECT *pObj = &(pObjs->pClsObjects[i]);
			pObj->sClsObjectId		= pClsObjs->pClsobjects[i].sClsobjectId;
			pObj->bEnable			= pClsObjs->pClsobjects[i].ucEnable;
			strcpy( pObj->szClsobjectName, pClsObjs->pClsobjects[i].strClsobjectName );

			pObj->tAreaSetting.usAreaLo	= (USHORT)pClsObjs->pClsobjects[i].tAreaSettings.pAreas[0].fAreaLo;
			pObj->tAreaSetting.usAreaUp	= (USHORT)pClsObjs->pClsobjects[i].tAreaSettings.pAreas[0].fAreaUp;

			pObj->tSpeedSetting.usSpeedLo	= (USHORT)pClsObjs->pClsobjects[i].tSpeedSettings.pSpeeds[0].fSpeedLo;
			pObj->tSpeedSetting.usSpeedUp	= (USHORT)pClsObjs->pClsobjects[i].tSpeedSettings.pSpeeds[0].fSpeedUp;

		}

		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::SaveObjects( DWORD dwEngId, VCA5_APP_CLSOBJECTS *pObjs )
{
	HRESULT hr = E_FAIL;
	VCA_CLSGRPS_T tGrps;
	USES_CONVERSION;

	//libvca_retrieve_CLSGRPinfo( &tGrps, dwEngId );

	memset( &tGrps, 0, sizeof( tGrps ) );
	VCA_CLSOBJECTS_T *pClsObjs = &(tGrps.pClsgrps[0].tClsobjects);

	pClsObjs->uiTotalClsobjects = pObjs->ulTotalClsObjects;
	for( unsigned int i = 0; i < pObjs->ulTotalClsObjects; i++ )
	{

		VCA5_APP_CLSOBJECT *pObj = &(pObjs->pClsObjects[i]);

		pClsObjs->pClsobjects[i].sClsobjectId		= pObj->sClsObjectId;
		pClsObjs->pClsobjects[i].ucEnable			= pObj->bEnable;
		strcpy( pClsObjs->pClsobjects[i].strClsobjectName, pObj->szClsobjectName );
		pClsObjs->pClsobjects[i].sPosition			= (short)i;

		pClsObjs->pClsobjects[i].tAreaSettings.pAreas[0].fAreaLo	= pObj->tAreaSetting.usAreaLo;
		pClsObjs->pClsobjects[i].tAreaSettings.pAreas[0].fAreaUp	= pObj->tAreaSetting.usAreaUp;
		pClsObjs->pClsobjects[i].tAreaSettings.uiTotalFilters		= 1;

		pClsObjs->pClsobjects[i].tSpeedSettings.pSpeeds[0].fSpeedLo	= pObj->tSpeedSetting.usSpeedLo;
		pClsObjs->pClsobjects[i].tSpeedSettings.pSpeeds[0].fSpeedUp	= pObj->tSpeedSetting.usSpeedUp;
		pClsObjs->pClsobjects[i].tSpeedSettings.uiTotalFilters		= 1;


		pClsObjs->pClsobjects[i].ucUsed		= 1;
	}

	tGrps.uiTotalClsgrps			= 1;
	tGrps.pClsgrps[0].ucUsed		= 1;
	tGrps.pClsgrps[0].usClsgrpId	= 0;

	if( LIBVCA_ERROR != libvca_create_CLSGRPinfo( &tGrps, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::LoadTamper( DWORD dwEngId, VCA5_TAMPER_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_TAMPER_CONFIG tConfig;

	if( LIBVCA_ERROR != libvca_retrieve_TAMPERinfo( &tConfig, dwEngId ) )
	{
		pInfo->ulEnabled			= tConfig.enabled;
		pInfo->ulAlarmTimeout		= tConfig.alarmTimeout;
		pInfo->ulAreaThreshold		= tConfig.areaThreshold;
		pInfo->ulFiringThreshold	= tConfig.firingThreshold;
		pInfo->ulLowLightEnabled	= tConfig.lowLightEnabled;

		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::SaveTamper( DWORD dwEngId, VCA5_TAMPER_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_TAMPER_CONFIG tConfig;

	libvca_retrieve_TAMPERinfo( &tConfig, dwEngId );

	tConfig.enabled				= pInfo->ulEnabled;
	tConfig.alarmTimeout		= pInfo->ulAlarmTimeout;
	tConfig.areaThreshold		= pInfo->ulAreaThreshold;
	tConfig.firingThreshold		= pInfo->ulFiringThreshold;
	tConfig.lowLightEnabled		= pInfo->ulLowLightEnabled;

	if( LIBVCA_ERROR != libvca_create_TAMPERinfo( &tConfig, dwEngId ) )
	{
		hr = S_OK;
	}
	
	return hr;
}


HRESULT CVCAConfigure::LoadSceneChange( DWORD dwEngId, VCA5_SCENECHANGE_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_SCENECHANGE_CONFIG tConfig;

	if( LIBVCA_ERROR != libvca_retrieve_SCENECHANGEinfo( &tConfig, dwEngId ) )
	{
		pInfo->ulMode				= tConfig.mode;
		pInfo->tTamper.ulEnabled			= tConfig.tamperConfig.enabled;
		pInfo->tTamper.ulAlarmTimeout		= tConfig.tamperConfig.alarmTimeout;
		pInfo->tTamper.ulAreaThreshold		= tConfig.tamperConfig.areaThreshold;
		pInfo->tTamper.ulFiringThreshold	= tConfig.tamperConfig.firingThreshold;
		pInfo->tTamper.ulLowLightEnabled	= tConfig.tamperConfig.lowLightEnabled;
		pInfo->ulDNColorEnabled				= tConfig.DNColorEnabled;
		pInfo->ulDNRimEnabled				= tConfig.DNRimEnabled;

		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::SaveSceneChange( DWORD dwEngId, VCA5_SCENECHANGE_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_SCENECHANGE_CONFIG tConfig;

	libvca_retrieve_SCENECHANGEinfo( &tConfig, dwEngId );

	tConfig.mode				= (VCA_SCENECHANGE_MODE)pInfo->ulMode;
	tConfig.tamperConfig.enabled			= pInfo->tTamper.ulEnabled;
	tConfig.tamperConfig.alarmTimeout		= pInfo->tTamper.ulAlarmTimeout;
	tConfig.tamperConfig.areaThreshold		= pInfo->tTamper.ulAreaThreshold;
	tConfig.tamperConfig.firingThreshold	= pInfo->tTamper.ulFiringThreshold;
	tConfig.tamperConfig.lowLightEnabled	= pInfo->tTamper.ulLowLightEnabled;
	tConfig.DNColorEnabled					= pInfo->ulDNColorEnabled;
	tConfig.DNRimEnabled					= pInfo->ulDNRimEnabled;

	if( LIBVCA_ERROR != libvca_create_SCENECHANGEinfo( &tConfig, dwEngId ) )
	{
		hr = S_OK;
	}
	
	return hr;
}



HRESULT CVCAConfigure::LoadAdvanced( DWORD dwEngId, VCA5_APP_ADVANCED_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_CHANNEL_INFO_T	tChanInfo;

	if( LIBVCA_ERROR != libvca_retrieve_CHANNELinfo( &tChanInfo, dwEngId ) )
	{
		pInfo->bEnableStab	= tChanInfo.nStabilizer;
		pInfo->ulRetrigTime	= tChanInfo.nRetrigTime;

		pInfo->TrackerParams.ulMinObjAreaPix	= tChanInfo.nMinObjAreaPix;
		pInfo->TrackerParams.ulSecsToHoldOn		= tChanInfo.nStatHoldonTime;
		pInfo->TrackerParams.bSmokeFireEnabled	= tChanInfo.nSmokeFireEnabled;
		pInfo->TrackerParams.bAbObjEnabled		= tChanInfo.nAbObjEnabled;
		pInfo->TrackerParams.bAdvTraEnabled		= tChanInfo.nAdvTraEnabled;
		pInfo->TrackerParams.bCntLineEnabled	= tChanInfo.nCntLineEnabled;
		pInfo->TrackerParams.bMovObjEnabled		= tChanInfo.nMovObjEnabled;
		pInfo->TrackerParams.ulDetectionPoint	= tChanInfo.nDetectionPoint;

		hr = S_OK;
	}

	return hr;
}

HRESULT CVCAConfigure::SaveAdvanced( DWORD dwEngId, VCA5_APP_ADVANCED_INFO *pInfo )
{
	HRESULT hr = E_FAIL;
	VCA_CHANNEL_INFO_T tChanInfo;

	libvca_retrieve_CHANNELinfo( &tChanInfo, dwEngId );

	tChanInfo.nStabilizer = pInfo->bEnableStab;
	tChanInfo.nRetrigTime = pInfo->ulRetrigTime;

	tChanInfo.nMinObjAreaPix	= pInfo->TrackerParams.ulMinObjAreaPix;
	tChanInfo.nStatHoldonTime	= pInfo->TrackerParams.ulSecsToHoldOn;
	tChanInfo.nSmokeFireEnabled	= pInfo->TrackerParams.bSmokeFireEnabled;
	tChanInfo.nAbObjEnabled		= pInfo->TrackerParams.bAbObjEnabled;
	tChanInfo.nAdvTraEnabled	= pInfo->TrackerParams.bAdvTraEnabled;
	tChanInfo.nCntLineEnabled	= pInfo->TrackerParams.bCntLineEnabled;
	tChanInfo.nMovObjEnabled	= pInfo->TrackerParams.bMovObjEnabled;
	tChanInfo.nDetectionPoint	= pInfo->TrackerParams.ulDetectionPoint;

	if( libvca_create_CHANNELinfo( &tChanInfo, dwEngId ) )
	{
		hr = S_OK;
	}

	return hr;
}


HRESULT CVCAConfigure::LoadFunction( DWORD dwEngId, ULONG *pFunction)
{
	HRESULT hr = E_FAIL;
	VCA_LICENSE_INFO_T tLicInfo;
	VCA_CHANNEL_INFO_T tChanInfo;
	ULONG ulFunction = 0;

	libvca_retrieve_CHANNELinfo( &tChanInfo, dwEngId );

	for( int i = 0; i < tChanInfo.nNumLicenseIds; i++ )
	{
		libvca_retrieve_LICinfo( &tLicInfo, tChanInfo.nLicenseIds[i] );

		if( tLicInfo.fIsValid )
		{
			// Extract the function if VCAsys
			VCA_FEATURE_SUPPORT_T *pFs = &(tLicInfo.tProperties.tFeatureSupport);

			if( pFs->eFeatureType == VCA_FEATURE_DEF_VCASYS )
			{
				VCA_SYS_FEATURES_T *pFeat = (VCA_SYS_FEATURES_T *)&(pFs->tFeatures);

				if( pFeat->fPresence )		ulFunction |= VCA5_FEATURE_PRESENCE;
				if( pFeat->fEnter )			ulFunction |= VCA5_FEATURE_ENTER;
				if( pFeat->fExit )			ulFunction |= VCA5_FEATURE_EXIT;
				if( pFeat->fAppear )		ulFunction |= VCA5_FEATURE_APPEAR;
				if( pFeat->fDisappear )		ulFunction |= VCA5_FEATURE_DISAPPEAR;
				if( pFeat->fStopped )		ulFunction |= VCA5_FEATURE_STOPPED;
				if( pFeat->fDwell )			ulFunction |= VCA5_FEATURE_DWELL;
				if( pFeat->fDirection )		ulFunction |= VCA5_FEATURE_DIRECTION;
				if( pFeat->fSpeed )			ulFunction |= VCA5_FEATURE_SPEED;
				if( pFeat->fAbObj)			ulFunction |= VCA5_FEATURE_ABOBJ;
				if( pFeat->fTailgating)		ulFunction |= VCA5_FEATURE_TAILGATING;
				if( pFeat->fCountingLine)	ulFunction |= VCA5_FEATURE_LINECOUNTER;
				if( pFeat->fPeopleTracking)	ulFunction |= VCA5_FEATURE_PEOPLETRACKING;
				if( pFeat->fSmoke)			ulFunction |= VCA5_FEATURE_SMOKE;
				if( pFeat->fFire)			ulFunction |= VCA5_FEATURE_FIRE;

				if( pFeat->fCounting )		ulFunction |= VCA5_FEATURE_COUNTING;
				if( pFeat->fCalibration )	ulFunction |= VCA5_FEATURE_CALIBRATION;
				if( pFeat->fMetadata )		ulFunction |= VCA5_FEATURE_METADATA;
				
			}
		}
	}

	// Sanity check - make sure we always have at least VCA presence
	ulFunction |= VCA5_FEATURE_PRESENCE | VCA5_FEATURE_COUNTING;
	*pFunction = ulFunction;

	return S_OK;
}

HRESULT CVCAConfigure::SquishIds( VCA5_APP_ENGINE_CONFIG *pCfg )
{
	// Go through and serialize the rule and counter ids so they fit within a smaller range
	// This is necessary for compatibility with NVC version

	unsigned int *pOldIds = new unsigned int[pCfg->Rules.ulTotalRules];

	for( unsigned int i = 0; i < pCfg->Rules.ulTotalRules; i++ )
	{
		VCA5_APP_RULE *pRule = &(pCfg->Rules.pRules[i]);

		pOldIds[i] = pRule->ulRuleId;

		// Assign a new one
		pRule->ulRuleId = i;
	}

	// Now go through and re-assign counter ids
	for( unsigned int i = 0; i < pCfg->Counters.ulTotalCounters; i++ )
	{
		VCA5_APP_COUNTER *pCtr = &(pCfg->Counters.pCounters[i]);

		// Examine subcounters
		for( unsigned int j = 0; j < pCtr->usNumSubCounters; j++ )
		{
			VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);

			for( unsigned int k = 0; k < pCfg->Rules.ulTotalRules; k++ )
			{
				if( pSc->usTrigId == pOldIds[k] )
				{
					pSc->usTrigId = k;
					break;
				}
			}
			
		}
	}

	delete [] pOldIds;

	return S_OK;
}

HRESULT CVCAConfigure::UnSquishIds( VCA5_APP_ENGINE_CONFIG *pCfg )
{
	// Unserialize the ids to the bizarre internal format used by DemoVCA
	struct sMapping
	{
		unsigned int uiOld;
		unsigned int uiNew;
	};

	sMapping *pIds = new sMapping[ pCfg->Rules.ulTotalRules ];

	for( unsigned int i = 0; i < pCfg->Rules.ulTotalRules; i++ )
	{
		VCA5_APP_RULE *pRule = &(pCfg->Rules.pRules[i]);

		pIds[i].uiOld = pRule->ulRuleId;

		pRule->ulRuleId = (pRule->usZoneId * VCA5_RULE_TYPE_NUM) + pRule->usRuleType;

		pIds[i].uiNew = pRule->ulRuleId;
	}

	// Update counter sources
	for( unsigned int i = 0; i < pCfg->Counters.ulTotalCounters; i++ )
	{
		VCA5_APP_COUNTER *pCtr = &(pCfg->Counters.pCounters[i]);

		for( unsigned int j = 0; j < pCtr->usNumSubCounters; j++ )
		{
			VCA5_SUBCOUNTER *pSc = &(pCtr->pSubCounters[j]);

			for( unsigned int k = 0; k < pCfg->Rules.ulTotalRules; k++ )
			{
				if( pIds[k].uiOld == pSc->usTrigId )
				{
					pSc->usTrigId = pIds[k].uiNew;
				}
			}
		}
	}

	delete [] pIds;

	return S_OK;
}

HRESULT CVCAConfigure::DoLoad( DWORD dwEngId )
{
	// Now assign all libvca structs to internal jobbies
	VCA5_APP_ENGINE_CONFIG *pCfg = &(m_EngineConfig[dwEngId]);

	LoadZones( dwEngId, &(pCfg->Zones) );
	LoadRules( dwEngId, &(pCfg->Rules) );
	LoadCounters( dwEngId, &(pCfg->Counters) );
	LoadCalibration( dwEngId, &(pCfg->CalibInfo) );
	LoadObjects( dwEngId, &(pCfg->ClsObjects) );
	LoadTamper( dwEngId, &(pCfg->TamperInfo) );
	LoadSceneChange( dwEngId, &(pCfg->SceneChangeInfo) );
	LoadAdvanced( dwEngId, &(pCfg->AdvInfo) );
	LoadFunction( dwEngId, &(pCfg->ulFunction) );

//	UnSquishIds( pCfg );


	return S_OK;
}

HRESULT CVCAConfigure::LoadEngine( DWORD dwEngId, LPTSTR lpszFilename )
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	do
	{
		// Reset this engine
		if( LIBVCA_ERROR == libvca_reset_ALLdefaults( dwEngId ) )
		{
			break;
		}

		// Load the file
		libvca_loadCfgFromXMLFile( T2A(lpszFilename), dwEngId, VCA_CFGFLAG_ALL );

		// Carry on, even if file was invalid - to get the default values
		hr = DoLoad( dwEngId );

	}
	while( 0 );

	return hr;
}

HRESULT CVCAConfigure::LoadEngine( DWORD dwEngId, char *pszBuf, unsigned int uiBufLen )
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	do
	{
		// Reset this engine
		if( LIBVCA_ERROR == libvca_reset_ALLdefaults( dwEngId ) )
		{
			break;
		}

		// Load the file
		libvca_loadCfgFromXMLBuffer(pszBuf, uiBufLen, dwEngId, VCA_CFGFLAG_ALL );

		// Carry on, even if file was invalid - to get the default values
		hr = DoLoad( dwEngId );

	}
	while( 0 );

	return hr;
}

HRESULT CVCAConfigure::DoSave( DWORD dwEngId )
{
	VCA5_APP_ENGINE_CONFIG *pCfg = &(m_EngineConfig[dwEngId]);

//	SquishIds( pCfg );

	SaveZones( dwEngId, &(pCfg->Zones) );
	SaveRules( dwEngId, &(pCfg->Rules) );
	SaveCounters( dwEngId, &(pCfg->Counters) );
	SaveCalibration( dwEngId, &(pCfg->CalibInfo) );
	SaveObjects( dwEngId, &(pCfg->ClsObjects) );
	SaveTamper( dwEngId, &(pCfg->TamperInfo) );
	SaveSceneChange( dwEngId, &(pCfg->SceneChangeInfo) );
	SaveAdvanced( dwEngId, &(pCfg->AdvInfo) );

	return S_OK;
}

HRESULT CVCAConfigure::SaveEngine(DWORD dwEngId, LPTSTR lpszFilename)
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	do
	{
		DoSave( dwEngId );


		libvca_saveCfgtoXMLFile( T2A(lpszFilename), dwEngId, VCA_CFGFLAG_ALL );
	}
	while( 0 );

	return S_OK;
}

HRESULT CVCAConfigure::SaveEngine(DWORD dwEngId, char *pszBuf, unsigned int &uiBufLen )
{
	HRESULT hr = E_FAIL;
	USES_CONVERSION;

	do
	{
		DoSave( dwEngId );

		if( 0 == libvca_saveCfgtoXMLBuffer( pszBuf, &uiBufLen, dwEngId, VCA_CFGFLAG_ALL ) )
		{
			hr = S_OK;
		}
	}
	while( 0 );

	return hr;
}

