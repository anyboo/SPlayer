#include "StdAfx.h"

#include <atlbase.h>
#include <atlstr.h>

#include "VCAMetaTagDefs.h"
#include "VCAMetaParserMSXML.h"

#define LCID_ENGLISH MAKELCID(MAKELANGID(0x09, 0x01), SORT_DEFAULT)


CVCAMetaParserMSXML::CVCAMetaParserMSXML() 
{
	CoInitialize(NULL);
	
	// Set up the DOM
	HRESULT hr;
	hr = m_pDOMDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if(S_OK != hr)
	{
		MessageBox(NULL, _T("Can not Create XMLDOMDocument Install MSXML6.0"), _T("ERROR") ,MB_OK | MB_ICONERROR);

	}

//	FILE *pFile;
//	pFile = fopen( "C:\\vtmeta.txt", "w" );
//	fclose( pFile );
}

CVCAMetaParserMSXML::~CVCAMetaParserMSXML()
{
	CoUninitialize();
}


BOOL CVCAMetaParserMSXML::ParseMetaData( unsigned char *pMetadata, int nLength )
{
	CComBSTR sz( (char *)pMetadata );
	VARIANT_BOOL vb;
	USES_CONVERSION;

	pMetadata[nLength] = '\0';

	BSTR bstr = SysAllocString( A2W( (char *)pMetadata ) );

//	sz = CComBSTR("<?xml version=\"1.0\"?>\n<vca>\n <vca_hdr>\n  <frame_id>10</frame_id>\n </vca_hdr>\n</vca>\n");

//	TRACE( (char *)pMetadata );

	vb = m_pDOMDoc->loadXML( bstr);

//	FILE *pFile;
//	pFile = fopen( "C:\\vtmeta.txt", "a" );
//	fwrite( pMetadata, strlen( (char *)pMetadata ), 1, pFile );
//	fclose( pFile );

	if( VARIANT_TRUE == vb )
	{
		// Find the header
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
	}
	else
	{
		// Loading the doc failed, find out why...
		MSXML2::IXMLDOMParseErrorPtr pErr = m_pDOMDoc->parseError;

		CString strLine, sResult;
		strLine.Format(_T(" ( line %u, column %u )"), pErr->Getline(), pErr->Getlinepos());
		// Return validation results in message to the user.
		if (pErr->errorCode != S_OK)
		{
			sResult = CString("Validation failed on ") +
			 CString ("\n=====================") +
			 CString("\nReason: ") + CString( (char*)(pErr->Getreason())) +
			 CString("\nSource: ") + CString( (char*)(pErr->GetsrcText())) +
			 strLine + CString("\n");

			ATLTRACE(sResult);

//			AfxMessageBox( (char *)pMetadata );


		}
	}

//	memset( pMetadata, NULL, 2048 );
	SysFreeString( bstr );

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseHeader( )
{
	CComBSTR						bsTag;
	MSXML2::IXMLDOMNodePtr			pNode, pSubNode;

	// Find the header node
	bsTag = CComBSTR(_XML_VCA) += CComBSTR("/") += CComBSTR( _XML_VCA_HDR );
	
	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );

	if( pNode )
	{
		// Find the status header
		bsTag = _XML_VCA_STATUS;
		pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
		VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&m_vcaHdr.ulVCAStatus );

		bsTag = _XML_FRAME_ID;
		pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
		VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&m_vcaHdr.ulFrameId );
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseStab()
{
	// Find all the objects
	CComBSTR					bsTag;

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode, pSubSubNode, pSubSubSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList, pSubNodeList;

	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( _XML_STAB );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );

	if( pNode )
	{
		bsTag = CComBSTR( _XML_SHIFT );

		pNode = pNode->selectSingleNode( bsTag.operator BSTR() );

		if( pNode )
		{
			bsTag = CComBSTR( _XML_X );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );

			if( pSubNode )
			{
				VarI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (long *)&m_vcaStab.iShiftX );
			}

			bsTag = CComBSTR( _XML_Y );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR () );

			if( pSubNode )
			{
				VarI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (long *)&m_vcaStab.iShiftY );
			}
		}
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseObjects( )
{
	// Find all the objects
	CComBSTR					bsTag;
	

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode, pSubSubNode, pSubSubSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList, pSubNodeList;

	// See if we have any objects this time round
	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( _XML_OBJECTS );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );
// 	ATLTRACE( "====================================================== \n" );

	if( pNode )
	{
		// There are some in this packet
		// Wipe out all exisiting
		m_vcaObjects.ulTotalObject = 0;

		// Select all objects
		bsTag = CComBSTR( _XML_OBJECT );

		pNodeList = pNode->selectNodes( bsTag.operator BSTR() );

		// Iterate through all the nodes
		pNode = pNodeList->nextNode();

		while( pNode )
		{
			VCA5_PACKET_OBJECT	vcaObj;
			memset( &vcaObj, 0, sizeof( vcaObj ) );

			// Id
			bsTag = CComBSTR( _XML_ID );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaObj.ulId );
			}

			// Calibrated height and speed
			bsTag = CComBSTR( _XML_CH );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{				
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaObj.ulCalibHeight);
			}

			bsTag = CComBSTR( _XML_CS );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaObj.ulCalibSpeed);
			}

			bsTag = CComBSTR( _XML_CA );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaObj.ulCalibArea);
			}

			bsTag = CComBSTR( _XML_CLS );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (long *)&vcaObj.iClassificationId);
			}
			else
			{
				vcaObj.iClassificationId = -2;  // unknown	
			}

			bsTag = CComBSTR( _XML_COLSIG );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				std::string tempColSigs = pSubNode->text;
				for(int n=0; n<VCA5_APP_PALETTE_SIZE; n++)
				{
					vcaObj.colSigs[n] = (UCHAR)strtoul( tempColSigs.substr(n*2,2).c_str(), NULL, 16 );
				}

// 				ATLTRACE( "%s: %d %d %d %d %d %d %d %d %d %d \n",  tempColSigs.c_str(), 
// 					vcaObj.colSigs[0], vcaObj.colSigs[1], vcaObj.colSigs[2], vcaObj.colSigs[3], vcaObj.colSigs[4],
// 					vcaObj.colSigs[5], vcaObj.colSigs[6], vcaObj.colSigs[7], vcaObj.colSigs[8], vcaObj.colSigs[9] );
			}
			else
			{
				for(int n=0; n<VCA5_APP_PALETTE_SIZE; n++)
				{
					vcaObj.colSigs[n] = 0;
				}
			}

			// Bounding box
			ParseBBox( pNode, &vcaObj.bBox );


			// Trail
			bsTag = CComBSTR( _XML_TRAIL );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );

			if( pSubNode )
			{
				bsTag = CComBSTR( _XML_PT );
				pSubNodeList = pSubNode->selectNodes( bsTag.operator BSTR() );

				pSubSubNode = pSubNodeList->nextNode();
				vcaObj.trail.usNumTrailPoints = 0;
				while( pSubSubNode )
				{
					VCA5_POINT	vcaPoint;
					memset( &vcaPoint, 0, sizeof( vcaPoint ) );

					bsTag = CComBSTR( _XML_X );
					pSubSubSubNode = pSubSubNode->selectSingleNode( bsTag.operator BSTR() );
					if( pSubSubSubNode )
					{
						VarUI2FromStr( pSubSubSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&vcaPoint.x );
					}

					bsTag = CComBSTR( _XML_Y );
					pSubSubSubNode = pSubSubNode->selectSingleNode( bsTag.operator BSTR() );
					if( pSubSubSubNode )
					{
						VarUI2FromStr( pSubSubSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&vcaPoint.y);
					}

					// Ph it (like Salt 'n' Pepa)
					vcaObj.trail.trailPoints[vcaObj.trail.usNumTrailPoints] = vcaPoint;
					vcaObj.trail.usNumTrailPoints++;	

					pSubSubNode = pSubNodeList->nextNode();
				}
			}

			// Flags
			bsTag = CComBSTR( _XML_FLAGS );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaObj.ulFlags );
			}


			m_vcaObjects.Objects[m_vcaObjects.ulTotalObject]  = vcaObj;
			//m_vcaObjects.ph_back( vcaObj );
			m_vcaObjects.ulTotalObject++;

			pNode = pNodeList->nextNode();
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseEvents()
{
	
	// Find all the events
	USES_CONVERSION;
	CComBSTR					bsTag;

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode, pSubSubNode, pSubSubSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList, pSubNodeList;


	// See if we have any objects this time round
	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( _XML_EVENTS );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );
	
	if( pNode )
	{
		// There are some event specified in this packet... take a look...
		m_vcaEvents.ulTotalEvents = 0;

		// Select all events
		bsTag = CComBSTR( _XML_EVENT );

		pNodeList = pNode->selectNodes( bsTag.operator BSTR() );

		// Iterate through all the nodes
		pNode = pNodeList->nextNode();

		while( pNode )
		{
			VCA5_PACKET_EVENT vcaEvent;
			memset( &vcaEvent, 0, sizeof( VCA5_PACKET_EVENT ) );

			// <type>
			bsTag = CComBSTR( _XML_TYPE );

			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				if( 0 == strcmp( pSubNode->text, "zone")) vcaEvent.ulEventType = VCA5_EVENT_TYPE_ZONE;
				else if( 0 == strcmp( pSubNode->text, "tamper")) vcaEvent.ulEventType = VCA5_EVENT_TYPE_TAMPER;
			}

			switch (vcaEvent.ulEventType)
			{
			case VCA5_EVENT_TYPE_TAMPER:
				m_uTamperAlarm = 300;

				strcpy( vcaEvent.szRuleName, "---" );
				strcpy( vcaEvent.szZoneName, "---" );
				break;

				// Deliberate fall-thru (durchfall :)
			case VCA5_EVENT_TYPE_ZONE:

				// <id>
				bsTag = CComBSTR( _XML_ID );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.ulId );
				}

				// <rule_name>
				// TBD
				
				// <rule_id>
				bsTag = CComBSTR( _XML_RULE_ID );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.ulRuleId );
				}

				// <rule_name>

				bsTag = CComBSTR( _XML_RULE_NAME );
				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					strcpy( vcaEvent.szRuleName, CT2A(pSubNode->text , CP_UTF8) );
				}
				else
				{
					strcpy( vcaEvent.szRuleName, "---" );
				}

				// <obj_id> (multiple)
				bsTag = CComBSTR( _XML_OBJ_ID );
				pSubNodeList = pNode->selectNodes( bsTag.operator BSTR() );
				if( pSubNodeList )
				{
					pSubNode = pSubNodeList->nextNode();
					if( pSubNode )
						VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.ulObjId );
				}

				// <zone_id> (multiple)
				bsTag = CComBSTR( _XML_ZONE_ID );

				pSubNodeList = pNode->selectNodes( bsTag.operator BSTR() );
				if( pSubNodeList )
				{
					pSubNode = pSubNodeList->nextNode();

					while( pSubNode )
					{
						unsigned int uiZoneId;
						VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&uiZoneId );

						vcaEvent.ulZoneId = uiZoneId;

						break;

						pSubNode = pSubNodeList->nextNode();
					}
				}

				// <zone_name> (multiple)
				bsTag = CComBSTR( _XML_ZONE_NAME );

				pSubNodeList = pNode->selectNodes( bsTag.operator BSTR() );
				if( pSubNodeList )
				{
					pSubNode = pSubNodeList->nextNode();
					while( pSubNode )
					{
						std::string szZoneName;
						szZoneName  = pSubNode->text;
						strcpy( vcaEvent.szZoneName, szZoneName.c_str());

						// Assume only 1
						break;
						pSubNode = pSubNodeList->nextNode();
					}
				}
/*
				// <obj_id> (multiple)
				bsTag = CComBSTR( _XML_OBJ_ID );

				pSubNodeList = pNode->selectNodes( bsTag.operator BSTR() );
				if( pSubNodeList )
				{
					pSubNode = pSubNodeList->nextNode();

					while( pSubNode )
					{
						unsigned int uiObjId;

						VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&uiObjId );

						vcaEvent.vObjIds.push_back( uiObjId );

						pSubNode = pSubNodeList->nextNode();
					}
				}

				// <obj_cls_name> (multiple)
				bsTag = CComBSTR( _XML_OBJ_CLS_NAME );

				pSubNodeList = pNode->selectNodes( bsTag.operator BSTR() );
				if( pSubNodeList )
				{
					pSubNode = pSubNodeList->nextNode();
					while( pSubNode )
					{
						std::string szObjClsName;
						szObjClsName  = pSubNode->text;
						vcaEvent.vObjClsNames.push_back( szObjClsName );
						pSubNode = pSubNodeList->nextNode();
					}
				}
*/
				// <status>
				bsTag = CComBSTR( _XML_STATUS );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.ulStatus );
				}

				// <start_time>
				bsTag = CComBSTR( _XML_START_TIME ) += CComBSTR("/") += CComBSTR( _XML_S );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					//time_t tTime;
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.tStartTime.ulSec  );
					//vcaEvent.tStartTime = CTime( tTime );
				}

				bsTag = CComBSTR( _XML_START_TIME ) += CComBSTR("/") += CComBSTR( _XML_MS );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					//time_t tTime;
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.tStartTime.ulMSec );
					//vcaEvent.tStartTime = CTime( tTime );
				}

				// <end_time>
				bsTag = CComBSTR( _XML_END_TIME ) += CComBSTR("/") += CComBSTR( _XML_S );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					//time_t tTime;
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.tStopTime.ulSec  );
					//vcaEvent.tStopTime = CTime( tTime );
				}

				bsTag = CComBSTR( _XML_END_TIME ) += CComBSTR("/") += CComBSTR( _XML_MS );

				pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
				if( pSubNode )
				{
					//time_t tTime;
					VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaEvent.tStopTime.ulMSec  );
					//vcaEvent.tStopTime = CTime( tTime );
				}

				// Bounding box
				ParseBBox( pNode, &vcaEvent.bBox );

				m_vcaEvents.Events[m_vcaEvents.ulTotalEvents] =  vcaEvent;
				m_vcaEvents.ulTotalEvents++;

			break;

			default: break;
			
			}

			pNode = pNodeList->nextNode();
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseCounts()
{
	// Find all the events
	CComBSTR					bsTag;

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode, pSubSubNode, pSubSubSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList, pSubNodeList;

	// See if we have any objects this time round
	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( _XML_COUNTS );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );

	if( pNode )
	{
		// There are some event specified in this packet... take a look...
		//m_vcaCounts.clear();
		m_vcaCounts.ulTotalCounter = 0;

		// Select all events
		bsTag = CComBSTR( _XML_COUNT );

		pNodeList = pNode->selectNodes( bsTag.operator BSTR() );

		pNode = pNodeList->nextNode();

		while( pNode )
		{
			VCA5_PACKET_COUNT vcaCount;
			memset( &vcaCount, 0, sizeof( vcaCount ) );

			bsTag = CComBSTR( _XML_ID );

			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&vcaCount.ulId );
			}

			bsTag = CComBSTR( _XML_VAL );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (long *)&vcaCount.iVal );
			}

			pNode = pNodeList->nextNode();
			//m_vcaCounts.ph_back( vcaCount );
			m_vcaCounts.Counters[m_vcaCounts.ulTotalCounter] = vcaCount;
			m_vcaCounts.ulTotalCounter++;
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CVCAMetaParserMSXML::ParseBBox( MSXML2::IXMLDOMNodePtr pNode, VCA5_RECT *pBbox )
{
	MSXML2::IXMLDOMNodePtr pSubNode;
	CComBSTR bsTag;

	// Bounding Box
	bsTag = CComBSTR( _XML_BB ) += CComBSTR("/") += CComBSTR( _XML_X );
	pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
	if( pSubNode )
	{
		VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&pBbox->x );
	}

	bsTag = CComBSTR( _XML_BB ) += CComBSTR("/") += CComBSTR( _XML_Y );
	pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
	if( pSubNode )
	{
		VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&pBbox->y );
	}

	bsTag = CComBSTR( _XML_BB ) += CComBSTR("/") += CComBSTR( _XML_W );
	pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
	if( pSubNode )
	{
		VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&pBbox->w );
	}

	bsTag = CComBSTR( _XML_BB ) += CComBSTR("/") += CComBSTR( _XML_H );
	pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
	if( pSubNode )
	{
		VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&pBbox->h);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
BOOL CVCAMetaParserMSXML::ParseCE( )
{

	CComBSTR bsTag;

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode, pSubSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList;

		// See if we have any objects this time round
	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( _XML_COUNTEVENTS );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );

	if( pNode )
	{
		// There are some event specified in this packet... take a look...
		memset(&m_vcaCountLineEvents, 0, sizeof(m_vcaCountLineEvents));
		// Select all events
		bsTag = CComBSTR( _XML_CE );

		pNodeList = pNode->selectNodes( bsTag.operator BSTR() );

		// Iterate through all the nodes
		pNode = pNodeList->nextNode();

		while( pNode )
		{
			VCA5_COUNTLINE_EVENT clEvent;
			memset( &clEvent, 0, sizeof( VCA5_COUNTLINE_EVENT ) );


			// pClEvent 
			bsTag = CComBSTR( _XML_ID );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&clEvent.uiId );
			}


			bsTag = CComBSTR( _XML_P );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&clEvent.usPos );
			}

			bsTag = CComBSTR( _XML_W );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&clEvent.usWidth );
			}
			
			bsTag = CComBSTR( _XML_N );
			pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
			if( pSubNode )
			{
				VarUI2FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned short *)&clEvent.usNum );
			}

			m_vcaCountLineEvents.CountLineEvents[m_vcaCountLineEvents.ulTotalCountLineEvents] = clEvent;
			m_vcaCountLineEvents.ulTotalCountLineEvents++;

			pNode = pNodeList->nextNode();
		}
	}

	return TRUE;
}



//---------------------------------------------------------------------------



BOOL CVCAMetaParserMSXML::ParseBlobsImp(char* szTag, VCA5_PACKET_BLOBMAP *pBLOBMAP)
{
	CComBSTR					bsTag;

	MSXML2::IXMLDOMNodePtr		pNode, pSubNode;	// Great names eh ;~)
	MSXML2::IXMLDOMNodeListPtr	pNodeList, pSubNodeList;
	unsigned int				uiWidth = 0, uiHeight = 0;


	// See if we have any objects this time round
	bsTag = CComBSTR( _XML_VCA ) += CComBSTR("/") += CComBSTR( szTag );

	pNode = m_pDOMDoc->selectSingleNode( bsTag.operator BSTR() );

	if( pNode )
	{
		// Tampermap is there
		bsTag = CComBSTR( _XML_W );
		pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
		if( pSubNode )
		{
			VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&uiWidth );
		}

		bsTag = CComBSTR( _XML_H );
		pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
		if( pSubNode )
		{
			VarUI4FromStr( pSubNode->text, LCID_ENGLISH, LOCALE_NOUSEROVERRIDE, (unsigned long *)&uiHeight );
		}

		if( uiWidth != pBLOBMAP->ulWidth || uiHeight != pBLOBMAP->ulHeight )
		{
			if (pBLOBMAP->pBlobMap)
				delete [] pBLOBMAP->pBlobMap;

			pBLOBMAP->pBlobMap	= new unsigned char[ uiWidth * (uiHeight+1) ];
			pBLOBMAP->ulWidth	= uiWidth;
			pBLOBMAP->ulHeight	= uiHeight;
		}

		// Now look for the data
		bsTag = CComBSTR( _XML_DATA );
		pSubNode = pNode->selectSingleNode( bsTag.operator BSTR() );
		if( pSubNode )
		{
			unsigned char *pBlobMap = pBLOBMAP->pBlobMap;

			// Demunge the data
			CString s( (LPCWSTR) pSubNode->text );
			TCHAR *pS = s.GetBuffer(0);
			int len = s.GetLength();

			for( int i = 0; i < s.GetLength(); i++ )
			{
				char cc = (char)pS[i];
				unsigned char ucBits = m_base64[pS[i]];

				*pBlobMap++	= ucBits & 0x01 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x02 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x04 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x08 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x10 ? 1 : 0;
				*pBlobMap++	= ucBits & 0x20 ? 1 : 0;
			}
			
		}

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

//---------------------------------------------------------------------------

/*
CTime CVCAMetaParserMSXML::MakeCTime( BSTR bszTime )
{
	CString sTime( bszTime );

	int nYear, nMonth, nDay, nHour, nMin, nSec;

//	time_t tt = atoi( sTime );

//	struct tm *pT;
//	pT = gmtime( &tt );


	sscanf( sTime.GetBuffer(0), "%04d-%02d-%02dT%02d:%02d:%02d", &nYear,
				&nMonth, &nDay, &nHour, &nMin, &nSec );

	return CTime( nYear+70, nMonth, nDay, nHour, nMin, nSec );

}
*/