#include "StdAfx.h"
#include "VCAMetaLib.h"
#include "VCAMetaParserMSXML.h"
#include "VCAMetaParserIXML.h"


CVCAMetaLib::CVCAMetaLib()
{
	memset( &m_vcaHdr, 0, sizeof( m_vcaHdr ) );
	memset( &m_BlobMap, 0, sizeof( m_BlobMap ) );
	memset( &m_SmokeMap, 0, sizeof( m_SmokeMap ) );
	memset( &m_FireMap, 0, sizeof( m_FireMap ) );
	memset( &m_vcaObjects, 0, sizeof( m_vcaObjects) );
	memset( &m_vcaEvents, 0, sizeof( m_vcaEvents ) );
	memset( &m_vcaCounts, 0, sizeof( m_vcaCounts ) );
	memset( &m_TamperInfo, 0, sizeof( m_TamperInfo ) );
	memset( &m_SceneChangeInfo, 0, sizeof( m_SceneChangeInfo ) );
	memset( &m_vcaStab, 0, sizeof( m_vcaStab ) );
	memset( &m_vcaCountLineEvents, 0, sizeof( m_vcaCountLineEvents ) );
	memset( &m_StationaryMap, 0, sizeof( m_StationaryMap ) );

	m_uTamperAlarm = 0;
	int val;
	char c;


	// Build the base64 table
	memset( m_base64, 0, 256 );
	val = 0;
	for( c = 'A'; c <= 'Z'; c++ )
	{
		m_base64[c] = val++;
	}
	for( c = 'a'; c <= 'z'; c++ )
	{
		m_base64[c] = val++;
	}
	for( c = '0'; c <= '9'; c++ )
	{
		m_base64[c] = val++;
	}
	m_base64['+'] = val++;
	m_base64['/'] = val++;
}


CVCAMetaLib::~CVCAMetaLib() 
{	
	if(m_BlobMap.pBlobMap)
		delete []m_BlobMap.pBlobMap;
	if (m_TamperInfo.pBlobMap)
		delete [] m_TamperInfo.pBlobMap;
	if (m_StationaryMap.pBlobMap)
		delete [] m_StationaryMap.pBlobMap;
	if(m_SmokeMap.pBlobMap)
		delete []m_SmokeMap.pBlobMap;
	if(m_FireMap.pBlobMap)
		delete []m_FireMap.pBlobMap;

}


CVCAMetaLib* CreateVCAMetaLib(CVCAMetaLib::PARSER_TYPE type)
{
	CVCAMetaLib*pVCAMeta = NULL;
	if(CVCAMetaLib::MSXML_PARSER == type) {
		pVCAMeta = new CVCAMetaParserMSXML;
	} else if(CVCAMetaLib::IXML_PARSER == type) {
		pVCAMeta = new CVCAMetaParserIXML;
	}
	return pVCAMeta;
}

