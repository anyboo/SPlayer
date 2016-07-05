#include "StdAfx.h"
#include "VCAMetaParserIXML.h"

#include "VCAMetaTagDefs.h"

CVCAMetaParserIXML::CVCAMetaParserIXML() 
{
	m_pTmpBuf4BlobMap = NULL;
	m_pDOMDoc = NULL;
}

CVCAMetaParserIXML::~CVCAMetaParserIXML()
{
	if(m_pTmpBuf4BlobMap)
		delete [] m_pTmpBuf4BlobMap;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseMetaData( unsigned char *pMetadata, int nLength )
{
	pMetadata[nLength] = '\0';
	m_pDOMDoc = ixmlParseBuffer((char*)pMetadata);
//	puts(ixmlPrintDocument(m_pDOMDoc));
	if(m_pDOMDoc) {
		ParseHeader();
		ParseStab();
		ParseObjects();
		ParseEvents();
		ParseCounts();
		
		ParseBlobsImp(_XML_TAMPERMAP, &m_TamperInfo);
		ParseBlobsImp(_XML_SCENECHANGEMAP, &m_SceneChangeInfo);
		ParseBlobsImp(_XML_BLOBMAP, &m_BlobMap);
		ParseBlobsImp(_XML_STATBLOBMAP, &m_StationaryMap);
		ParseBlobsImp(_XML_SMOKEMAP,&m_SmokeMap);
		ParseBlobsImp(_XML_FIREMAP,&m_FireMap);

		ParseCE();

		ixmlDocument_free(m_pDOMDoc);
	} else {
		printf("!!! Parsing Error\n");
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

int GetNodeValueIntFromNodelist(IXML_NodeList *pNodeList, char *pszName)
{
	assert( pNodeList != NULL && pszName != NULL );

	int nVal = -1;
	IXML_NodeList	*pTmpNodeList = NULL;
	IXML_Node		*pNode = pNodeList->nodeItem;
//	puts(ixmlPrintNode(node));

	pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pNode, pszName);
	if(pTmpNodeList) {
		IXML_Node *n = pTmpNodeList->nodeItem;
		IXML_Node *child = ixmlNode_getFirstChild( n );
	//	printf("%s = %d\n", n->nodeName, atoi(ixmlNode_getNodeValue(child)));
		if( !child ) nVal = -1;
		else nVal = atoi(ixmlNode_getNodeValue(child));
		ixmlNodeList_free(pTmpNodeList);
	}
	return nVal;
}


int GetNodeValueInt(IXML_Node *pNode, char *pszName)
{
	assert( pNode != NULL && pszName != NULL );

	int nVal = -1;
	IXML_NodeList	*pTmpNodeList = NULL;
	pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pNode, pszName);
	if(pTmpNodeList) {
		IXML_Node *n = pTmpNodeList->nodeItem;
		IXML_Node *child = ixmlNode_getFirstChild( n );
	//	printf("%s = %d\n", n->nodeName, atoi(ixmlNode_getNodeValue(child)));
		if( !child ) nVal = -1;
		else nVal = atoi(ixmlNode_getNodeValue(child));
		ixmlNodeList_free(pTmpNodeList);
	}
	return nVal;
}


BOOL GetNodeValueString(IXML_Node *pNode, char *pszName, char *pszOutStr)
{
	assert( pNode != NULL && pszName != NULL );

	BOOL ret = FALSE;
	IXML_NodeList	*pTmpNodeList = NULL;
	pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pNode, pszName);
	if(pTmpNodeList) {
		IXML_Node *n = pTmpNodeList->nodeItem;
		IXML_Node *child = ixmlNode_getFirstChild( n );
		if( !child ) {
			ret = FALSE;
		} else {
			strcpy(pszOutStr, ixmlNode_getNodeValue(child));
			ret = TRUE;
		}
		ixmlNodeList_free(pTmpNodeList);
	}
	return ret;
}

BOOL GetMultipleNodeValueInt(IXML_Node *pNode, char *pszName, int *arrOutint)
{
	BOOL ret = FALSE;
	IXML_NodeList *pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pNode, pszName);
	if( pTmpNodeList ) {
		
		IXML_Node *pTmpNode = pTmpNodeList->nodeItem;
		IXML_Node *child  = ixmlNode_getFirstChild( pTmpNode );
		char *pszNodeValue = ixmlNode_getNodeValue( child );

		switch ( ixmlNode_getNodeType( child ) ) {

		case eTEXT_NODE:
			arrOutint[0] = atoi(pszNodeValue);
			ret = TRUE;
			break;

		case eELEMENT_NODE:
		//	while() {
		//	}
		//	break;

		default:
			break;
		}

		ixmlNodeList_free(pTmpNodeList);
	}

	return ret;
}

BOOL GetMultipleNodeValueString(IXML_Node *pNode, char *pszName, char (*arrOutStr)[VCA5_APP_MAX_STR_LEN])
{
	BOOL ret = FALSE;
	IXML_NodeList *pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pNode, pszName);
	if( pTmpNodeList ) {

		IXML_Node *pTmpNode = pTmpNodeList->nodeItem;
		IXML_Node *child  = ixmlNode_getFirstChild( pTmpNode );
		char *pszNodeValue = ixmlNode_getNodeValue( child );

		switch ( ixmlNode_getNodeType( child ) ) {

		case eTEXT_NODE:
			strcpy(arrOutStr[0], pszNodeValue);
			ret = TRUE;
			break;

		case eELEMENT_NODE:
		//	while() {
		//	}
		//	break;

		default:
			break;
		}

		ixmlNodeList_free(pTmpNodeList);
	}

	return ret;
}


BOOL CVCAMetaParserIXML::ParseHeader( )
{
	IXML_NodeList *pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_VCA_HDR);

	if( pNodeList ) {
		m_vcaHdr.ulVCAStatus	= (ULONG)GetNodeValueIntFromNodelist(pNodeList, _XML_VCA_STATUS);
		m_vcaHdr.ulFrameId		= (ULONG)GetNodeValueIntFromNodelist(pNodeList, _XML_FRAME_ID);
		ixmlNodeList_free(pNodeList);
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseStab()
{
	IXML_NodeList *pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_STAB);
	if( pNodeList ) {
		IXML_Node *pNode = pNodeList->nodeItem;
		IXML_NodeList *pShiftNodeList = ixmlElement_getElementsByTagName ((IXML_Element*)pNode, _XML_SHIFT);
		
		if( pShiftNodeList ) {
			m_vcaStab.iShiftX	= (ULONG)GetNodeValueIntFromNodelist(pNodeList, _XML_X);
			m_vcaStab.iShiftY	= (ULONG)GetNodeValueIntFromNodelist(pNodeList, _XML_Y);
			ixmlNodeList_free(pShiftNodeList);
		}
		ixmlNodeList_free(pNodeList);
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseObjects( )
{
	IXML_NodeList *pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_OBJECTS);
	if( pNodeList ) {
		IXML_Node *pObjectsNode, *pObjectNode;
		IXML_NodeList *pObjectNodeList;

		m_vcaObjects.ulTotalObject = 0;

		pObjectsNode = pNodeList->nodeItem;

	//	TRACE(ixmlPrintNode(pObjectsNode));
		//pObjectNode = ixmlNode_getFirstChild(pObjectsNode);
		pObjectNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pObjectsNode, _XML_OBJECT);
#if 0
		if(pObjectNodeList) {
			pObjectNode = pObjectNodeList->nodeItem;
			while(pObjectNode) {
				TRACE("=====================================\n");
				TRACE(ixmlPrintNode(pObjectNode));
				pObjectNode = pObjectNode->nextSibling;
				//pObjectNode = pObjectNodeList->next;
			}
		}
#endif
		VCA5_PACKET_OBJECT	vcaObj;
		ULONG ulVal = 0;
		int iVal = 0;
		if(pObjectNodeList) {
			pObjectNode = pObjectNodeList->nodeItem;

			while(pObjectNode) {
				memset( &vcaObj, 0, sizeof( vcaObj ) );

				ulVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_ID);
				if(ulVal != -1) vcaObj.ulId = ulVal;

				ulVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_CH);
				if(ulVal != -1) vcaObj.ulCalibHeight = ulVal;

				ulVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_CS);
				if(ulVal != -1) vcaObj.ulCalibSpeed = ulVal;

				ulVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_CA);
				if(ulVal != -1) vcaObj.ulCalibArea = ulVal;

				iVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_CLS);
				if(ulVal != -1) vcaObj.iClassificationId = iVal;
				else vcaObj.iClassificationId = -2;  // unknown
				
				CHAR colSigs[VCA5_APP_PALETTE_SIZE * 2] = { 0, };
				ulVal = (ULONG)GetNodeValueString(pObjectNode, _XML_COLSIG, colSigs);
				if(ulVal != FALSE) memcpy(vcaObj.colSigs, colSigs, sizeof( vcaObj.colSigs ) );

				// Bounding box
				IXML_NodeList *pBBOXNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pObjectNode, _XML_BB);
				if(pBBOXNodeList) {
					ParseBBox( pBBOXNodeList->nodeItem, &vcaObj.bBox );
					ixmlNodeList_free(pBBOXNodeList);
				}

				// Trail
				IXML_NodeList *pTrailNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pObjectNode, _XML_TRAIL);
				if( pTrailNodeList ) {
					ParseTrail( pTrailNodeList, &vcaObj.trail );
					ixmlNodeList_free(pTrailNodeList);
				}

				// Flags
				ulVal = (ULONG)GetNodeValueInt(pObjectNode, _XML_FLAGS);
				if(ulVal != -1) vcaObj.ulFlags = ulVal;

				m_vcaObjects.Objects[m_vcaObjects.ulTotalObject]  = vcaObj;
				m_vcaObjects.ulTotalObject++;

				pObjectNode = pObjectNode->nextSibling;
			}
			ixmlNodeList_free(pObjectNodeList);
		//	TRACE("# Objects [%2d]\n", m_vcaObjects.ulTotalObject);
		}

		ixmlNodeList_free(pNodeList);
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseEvents()
{
	IXML_NodeList *pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_EVENTS);
	if( pNodeList ) {
		IXML_Node *pEventsNode, *pEventNode;
		IXML_NodeList *pEventNodeList, *pTmpNodeList;

		m_vcaEvents.ulTotalEvents = 0;

		pEventsNode = pNodeList->nodeItem;
		pEventNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pEventsNode, _XML_EVENT);
		if(pEventNodeList) {
			
			VCA5_PACKET_EVENT vcaEvent;
			char szStr[VCA5_APP_MAX_STR_LEN] = {0,};
			ULONG ulVal = 0;
			BOOL ret = FALSE;

			pEventNode = pEventNodeList->nodeItem;
			while(pEventNode) {
			//	TRACE("=====================================\n");
			//	TRACE(ixmlPrintNode(pEventNode));
			//	TRACE("\n\n");

				memset( &vcaEvent, 0, sizeof( VCA5_PACKET_EVENT ) );
				
				ret = GetNodeValueString(pEventNode, _XML_TYPE, szStr);
				if(ret) {
					if( 0 == strcmp( szStr, "zone")) vcaEvent.ulEventType = VCA5_EVENT_TYPE_ZONE;
					else if( 0 == strcmp( szStr, "tamper")) vcaEvent.ulEventType = VCA5_EVENT_TYPE_TAMPER;
					else assert(0);
				}

				switch (vcaEvent.ulEventType)
				{
				case VCA5_EVENT_TYPE_TAMPER:
					m_uTamperAlarm = 300;

					strcpy( vcaEvent.szRuleName, "---" );
					strcpy( vcaEvent.szZoneName, "---" );
					break;

				case VCA5_EVENT_TYPE_ZONE:
					int arIntValues[10];
					char arStrValues[10][VCA5_APP_MAX_STR_LEN];

					// <id>
					ulVal = (ULONG)GetNodeValueInt(pEventNode, _XML_ID);
					if(ulVal != -1) vcaEvent.ulId = ulVal;

					// <rule_name>
					// TBD

					// <rule_id>
					ulVal = (ULONG)GetNodeValueInt(pEventNode, _XML_RULE_ID);
					if(ulVal != -1) vcaEvent.ulRuleId = ulVal;

					// <rule_name>
					ret = GetNodeValueString(pEventNode, _XML_RULE_NAME, szStr);
					if(ret) {
						strcpy( vcaEvent.szRuleName, szStr );
					} else {
						strcpy( vcaEvent.szRuleName, "---" );
					}

					// <obj_id> (multiple)
					ret = GetMultipleNodeValueInt(pEventNode, _XML_OBJ_ID, arIntValues);
					if(ret) vcaEvent.ulObjId = (ULONG)arIntValues[0];

					// <zone_id> (multiple)
					ret = GetMultipleNodeValueInt(pEventNode, _XML_ZONE_ID, arIntValues);
					if(ret) vcaEvent.ulZoneId = (ULONG)arIntValues[0];

					// <zone_name> (multiple)
					ret = GetMultipleNodeValueString(pEventNode, _XML_ZONE_NAME, arStrValues);
					if(ret) strcpy( vcaEvent.szZoneName, arStrValues[0]);

					// <status>
					ulVal = (ULONG)GetNodeValueInt(pEventNode, _XML_STATUS);
					if(ulVal != -1) vcaEvent.ulStatus = ulVal;

					// <start_time>
					pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pEventsNode, _XML_START_TIME);
					if(pTmpNodeList) {
						vcaEvent.tStartTime.ulSec = (ULONG)GetNodeValueIntFromNodelist(pTmpNodeList, _XML_S);
						vcaEvent.tStartTime.ulMSec = (ULONG)GetNodeValueIntFromNodelist(pTmpNodeList, _XML_MS);
						ixmlNodeList_free(pTmpNodeList);
					}

					// <end_time>
					pTmpNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pEventsNode, _XML_END_TIME);
					if(pTmpNodeList) {
						vcaEvent.tStopTime.ulSec = (ULONG)GetNodeValueIntFromNodelist(pTmpNodeList, _XML_S);
						vcaEvent.tStopTime.ulMSec = (ULONG)GetNodeValueIntFromNodelist(pTmpNodeList, _XML_MS);
						ixmlNodeList_free(pTmpNodeList);
					}

					// Bounding box
					ParseBBox( pEventNode, &vcaEvent.bBox );

					m_vcaEvents.Events[m_vcaEvents.ulTotalEvents] =  vcaEvent;
					m_vcaEvents.ulTotalEvents++;
				break;

				default: break;
				}

				pEventNode = pEventNode->nextSibling;
			}
			ixmlNodeList_free(pEventNodeList);
		//	TRACE("# Objects [%2d]\n", m_vcaObjects.ulTotalObject);
		}

		ixmlNodeList_free(pNodeList);
	}


	
	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseCounts()
{
	IXML_NodeList *pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_COUNTS);
	if( pNodeList ) {
		// There are some event specified in this packet... take a look...
		m_vcaCounts.ulTotalCounter = 0;

		IXML_Node *pCountsNode, *pCountNode;
		IXML_NodeList *pCountNodeList;
		pCountsNode = pNodeList->nodeItem;
		pCountNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pCountsNode, _XML_COUNT);
		if(pCountNodeList) {
			pCountNode = pCountNodeList->nodeItem;
			while(pCountNode) {
				VCA5_PACKET_COUNT vcaCount;
				memset( &vcaCount, 0, sizeof( vcaCount ) );
				
				ULONG ulVal = 0;
				int iVal = 0;
				ulVal = (ULONG)GetNodeValueInt(pCountNode, _XML_ID);
				if(ulVal != -1) vcaCount.ulId = ulVal;

				iVal = GetNodeValueInt(pCountNode, _XML_VAL);
				if(iVal != -1) vcaCount.iVal = iVal;

				m_vcaCounts.Counters[m_vcaCounts.ulTotalCounter] = vcaCount;
				m_vcaCounts.ulTotalCounter++;

				pCountNode = pCountNode->nextSibling;
			}
			ixmlNodeList_free(pCountNodeList);
		}
		ixmlNodeList_free(pNodeList);
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseBBox( IXML_Node *pBBOXNode, VCA5_RECT *pBbox )
{
	pBbox->x	= (USHORT)GetNodeValueInt(pBBOXNode, _XML_X);
	pBbox->y	= (USHORT)GetNodeValueInt(pBBOXNode, _XML_Y);
	pBbox->w	= (USHORT)GetNodeValueInt(pBBOXNode, _XML_W);
	pBbox->h	= (USHORT)GetNodeValueInt(pBBOXNode, _XML_H);

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseTrail( IXML_NodeList *pTrailNodeList, VCA5_TRAIL *pTrail )
{
	USHORT usVal = 0;
	IXML_Node *pTrailNode = pTrailNodeList->nodeItem;
	IXML_NodeList *pPtNodeList = ixmlElement_getElementsByTagName((IXML_Element*)pTrailNode, _XML_PT);
	if( pPtNodeList ) {
		IXML_Node *pPtNode = pPtNodeList->nodeItem;
		pTrail->usNumTrailPoints = 0;
		while(pPtNode) {
			VCA5_POINT	vcaPoint;
			memset( &vcaPoint, 0, sizeof( vcaPoint ) );

			usVal = (USHORT)GetNodeValueInt(pPtNode, _XML_X);
			if(usVal != -1) vcaPoint.x = usVal;

			usVal = (USHORT)GetNodeValueInt(pPtNode, _XML_Y);
			if(usVal != -1) vcaPoint.y = usVal;

			pTrail->trailPoints[pTrail->usNumTrailPoints] = vcaPoint;
			pTrail->usNumTrailPoints++;	

			pPtNode = pPtNode->nextSibling;
		}
		ixmlNodeList_free(pPtNodeList);
	}

	return TRUE;
}


//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseCE( )
{

	IXML_NodeList *pNodeList = NULL, *pCENodeList = NULL;
	IXML_Node *pCENode = NULL, *pNode = NULL;
	ULONG ulVal = 0;
	USHORT usVal = 0;
	
	pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, _XML_COUNTEVENTS);
	if( pNodeList ) {
		pCENode = pNodeList->nodeItem;

		// There are some event specified in this packet... take a look...
		memset(&m_vcaCountLineEvents, 0, sizeof(m_vcaCountLineEvents));

		pCENodeList = ixmlElement_getElementsByTagName((IXML_Element*)pCENode, _XML_CE);
		if( pCENodeList ) {

			pNode = pCENodeList->nodeItem;
			while(pNode) {
				VCA5_COUNTLINE_EVENT clEvent;
				memset( &clEvent, 0, sizeof( VCA5_COUNTLINE_EVENT ) );

				ulVal = (ULONG)GetNodeValueInt(pNode, _XML_ID);
				if(ulVal != -1) clEvent.uiId = ulVal;

				usVal = (USHORT)GetNodeValueInt(pNode, _XML_P);
				if(usVal != -1) clEvent.usPos = usVal;

				usVal = (USHORT)GetNodeValueInt(pNode, _XML_W);
				if(usVal != -1) clEvent.usWidth = usVal;

				usVal = (USHORT)GetNodeValueInt(pNode, _XML_N);
				if(usVal != -1) clEvent.usNum = usVal;

				m_vcaCountLineEvents.CountLineEvents[m_vcaCountLineEvents.ulTotalCountLineEvents] = clEvent;
				m_vcaCountLineEvents.ulTotalCountLineEvents++;

				pNode = pNode->nextSibling;
			}
			ixmlNodeList_free(pCENodeList);
		}

		ixmlNodeList_free(pNodeList);
	}


	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserIXML::ParseBlobsImp(char* szTag, VCA5_PACKET_BLOBMAP *pBLOBMAP )
{
	ULONG ulWidth = 0, ulHeight = 0;
	ULONG ulVal = 0;
	BOOL ret = FALSE;
	char pszStr = NULL;

	IXML_NodeList *pNodeList = NULL;
	IXML_Node *pBlobmapNode = NULL;

	// See if we have any objects this time round
	pNodeList = ixmlDocument_getElementsByTagName(m_pDOMDoc, szTag);
	if( pNodeList ) {
		// Blobmap is there
		pBlobmapNode = pNodeList->nodeItem;

		ulVal = (ULONG)GetNodeValueInt(pBlobmapNode, _XML_W);
		if(ulVal != -1) ulWidth = ulVal;

		ulVal = (ULONG)GetNodeValueInt(pBlobmapNode, _XML_H);
		if(ulVal != -1) ulHeight = ulVal;

		if( ulWidth != pBLOBMAP->ulWidth ||
			ulHeight != pBLOBMAP->ulHeight )
		{
			delete [] pBLOBMAP->pBlobMap;
			delete [] m_pTmpBuf4BlobMap;

			pBLOBMAP->pBlobMap	= new unsigned char[ ulWidth * (ulHeight +1) ];
			pBLOBMAP->ulWidth	= ulWidth;
			pBLOBMAP->ulHeight	= ulHeight;
			m_pTmpBuf4BlobMap	= new unsigned char[ ulWidth * ulHeight ];
		}

		// Now look for the data
		ret = GetNodeValueString(pBlobmapNode, _XML_DATA, (char*)m_pTmpBuf4BlobMap);
		if(ret) {
			unsigned char *pBlobMap = pBLOBMAP->pBlobMap;

			int len = (int)strlen((char*)m_pTmpBuf4BlobMap);
			for( int i = 0; i < len; i++ )
			{
				char cc = (char)m_pTmpBuf4BlobMap[i];
				unsigned char ucBits = m_base64[m_pTmpBuf4BlobMap[i]];

				*pBlobMap++	= ucBits & 0x01 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x02 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x04 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x08 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x10 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x20 ? 1 : 0;
			}
		}
		ixmlNodeList_free(pNodeList);

		pBLOBMAP->ulMapAge = 0;

	}else{
		pBLOBMAP->ulMapAge++;
	
		// we shoud get map update every 2nd frame
		// if map is 2 frames old so clean it up or else we get old map hanging around
		if ( pBLOBMAP->ulMapAge == 2 && pBLOBMAP->pBlobMap )
		{
			memset( pBLOBMAP->pBlobMap, 0, pBLOBMAP->ulWidth * pBLOBMAP->ulHeight );
		}
	}

	return TRUE;
}

