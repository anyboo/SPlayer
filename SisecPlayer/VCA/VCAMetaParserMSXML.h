#ifndef _VCA_META_PARSER_MSXML_H_
#define _VCA_META_PARSER_MSXML_H_

#include "VCAMetaLib.h"
#import "msxml3.dll" 
#include <vector>

class CVCAMetaParserMSXML :public CVCAMetaLib
{
public:
	CVCAMetaParserMSXML();
	virtual ~CVCAMetaParserMSXML();
	BOOL ParseMetaData( unsigned char *pMetadata, int nLength );
	BOOL ParseBlobsImp(char* szTag, VCA5_PACKET_BLOBMAP *pBLOBMAP);

private:
	
	BOOL ParseHeader();
	BOOL ParseObjects();
	BOOL ParseEvents();
	BOOL ParseCounts();
	BOOL ParseStab();
	BOOL ParseCE();
	BOOL ParseBBox( MSXML2::IXMLDOMNodePtr pNode, VCA5_RECT *pBbox );
	
	MSXML2::IXMLDOMDocument2Ptr	m_pDOMDoc;
};

#endif	// _VCA_META_PARSER_MSXML_H_