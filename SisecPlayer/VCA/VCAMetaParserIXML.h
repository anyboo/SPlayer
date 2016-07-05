#ifndef _VCA_META_PARSER_IXML_H_
#define _VCA_META_PARSER_IXML_H_

#include "VCAMetaLib.h"
#include "ixml.h"

class CVCAMetaParserIXML :public CVCAMetaLib
{
public:
	CVCAMetaParserIXML();
	virtual ~CVCAMetaParserIXML();
	BOOL ParseMetaData( unsigned char *pMetadata, int nLength );
	BOOL ParseBlobsImp(char* szTag, VCA5_PACKET_BLOBMAP *pBLOBMAP);

private:
	
	BOOL ParseHeader();
	BOOL ParseObjects();
	BOOL ParseEvents();
	BOOL ParseCounts();
	BOOL ParseBlobs();
	BOOL ParseStab();
	BOOL ParseTamperDetection();
	BOOL ParseCE();
	BOOL ParseBBox( IXML_Node *pBBOXNode, VCA5_RECT *pBbox );
	BOOL ParseTrail( IXML_NodeList *pTrailNodeList, VCA5_TRAIL *pTrail );
	
	IXML_Document	*m_pDOMDoc;
	BYTE			*m_pTmpBuf4BlobMap;
};

#endif	// _VCA_META_PARSER_IXML_H_