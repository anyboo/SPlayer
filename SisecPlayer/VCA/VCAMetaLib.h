#ifndef _IVCA_META_LIB_H_
#define _IVCA_META_LIB_H_
#include <windows.h>
#include "VCA5CoreLib.h"

#define VCA5_APP_MAX_NUM_OBJECT				100
#define VCA5_APP_MAX_NUM_EVENT				50
#define VCA5_APP_MAX_STR_LEN				256
#define VCA5_APP_MAX_NUM_COUNTER_OUT		20
#define VCA5_APP_MAX_NUM_COUNTLINE_EVENT	20	
#define VCA5_APP_PALETTE_SIZE 				10

enum VCA5_PACKET_STATUS_E{
	VCA5_PACKET_STATUS_OK,
	VCA5_PACKET_STATUS_UNCONFIGURED,
	VCA5_PACKET_STATUS_UNSUPPORTED_INPUT_FMT,
	VCA5_PACKET_STATUS_UNLICENSED,
	VCA5_PACKET_STATUS_LEARNING_SCENE,
	VCA5_PACKET_STATUS_INHIBITED,
	VCA5_PACKET_STATUS_LICENSE_EXPIRED,
	VCA5_PACKET_STATUS_ERROR
};

enum VCA5_EVENT_STATUS_E{
	VCA5_EVENT_STATUS_START = 0x00000001,
	VCA5_EVENT_STATUS_UPDATE,
	VCA5_EVENT_STATUS_END,
	VCA5_EVENT_STATUS_INSTANT,
	VCA5_EVENT_INVALID
};

enum VCA5_EVENT_TYPE_E{
	VCA5_EVENT_TYPE_ZONE = 1,
	VCA5_EVENT_TYPE_TAMPER,
};

typedef struct _VCA5_PACKET_HEADER{
	ULONG		ulVCAStatus;
	ULONG		ulFrameId;
}VCA5_PACKET_HEADER;

typedef struct _VCA5_TRAIL{
	USHORT		usNumTrailPoints;
	VCA5_POINT	trailPoints[VCA5_MAX_NUM_TRAIL_POINTS];
}VCA5_TRAIL;

typedef struct _VCA5_PACKET_OBJECT{
	ULONG		ulId;
	ULONG		ulCalibHeight;
	ULONG		ulCalibSpeed;
	ULONG		ulCalibArea;
	int			iClassificationId;
	VCA5_RECT	bBox;
	VCA5_TRAIL	trail;
	ULONG		ulFlags;
	UCHAR		colSigs[VCA5_APP_PALETTE_SIZE];
}VCA5_PACKET_OBJECT;

typedef struct _VCA5_PACKET_EVENT{
	ULONG		ulId;
	ULONG		ulEventType;
	ULONG		ulRuleId;
	ULONG		ulZoneId;
	ULONG		ulObjId;
	ULONG		ulStatus;
	char		szRuleName[VCA5_APP_MAX_STR_LEN];
	char		szZoneName[VCA5_APP_MAX_STR_LEN];
	VCA5_TIME_STRUCT	tStartTime;
	VCA5_TIME_STRUCT	tStopTime;
	VCA5_RECT	bBox;
}VCA5_PACKET_EVENT;

typedef struct _VCA5_PACKET_COUNT{
	ULONG	ulId;
	int		iVal;
}VCA5_PACKET_COUNT;

typedef struct _VCA5_PACKET_BLOBMAP{

	ULONG	ulWidth;
	ULONG	ulHeight;
	ULONG	ulMapAge;
	BYTE	*pBlobMap;
}VCA5_PACKET_BLOBMAP;

// struct for counting line events
typedef struct _VCA5_COUNTLINE_EVENT{
	ULONG	uiId;
	USHORT	usPos;
	USHORT	usWidth;
	USHORT	usNum;
} VCA5_COUNTLINE_EVENT;	


typedef struct{
	ULONG				ulTotalObject;
	VCA5_PACKET_OBJECT	Objects[VCA5_APP_MAX_NUM_OBJECT];
}VCA5_PACKET_OBJECTS;

typedef struct{
	ULONG				ulTotalEvents;
	VCA5_PACKET_EVENT	Events[VCA5_APP_MAX_NUM_EVENT];
}VCA5_PACKET_EVENTS;

typedef struct{
	ULONG				ulTotalCounter;
	VCA5_PACKET_COUNT	Counters[VCA5_APP_MAX_NUM_COUNTER_OUT];
}VCA5_PACKET_COUNTS;

typedef struct{
	ULONG					ulTotalCountLineEvents;
	VCA5_COUNTLINE_EVENT	CountLineEvents[VCA5_APP_MAX_NUM_COUNTLINE_EVENT];
}VCA5_COUNTLINE_EVENTS;


typedef struct{
	int		iShiftX;
	int		iShiftY;
}VCA5_PACKET_STAB;

class CVCAMetaLib
{
public:
	typedef enum { MSXML_PARSER, IXML_PARSER } PARSER_TYPE;

	CVCAMetaLib();
	virtual ~CVCAMetaLib();

	virtual BOOL ParseMetaData( unsigned char *pMetadata, int nLength ) = 0;
	virtual BOOL ParseBlobsImp(char* szTag, VCA5_PACKET_BLOBMAP *pBLOBMAP) = 0;

	VCA5_PACKET_BLOBMAP*	GetBlobMap()	{return &m_BlobMap;};
	VCA5_PACKET_BLOBMAP*	GetSmokeMap()	{return &m_SmokeMap;};
	VCA5_PACKET_BLOBMAP*	GetFireMap()	{return &m_FireMap;};
	VCA5_PACKET_HEADER*		GetHeader()		{return &m_vcaHdr; }
	VCA5_PACKET_OBJECTS*	GetObjects()	{return &m_vcaObjects; }
	VCA5_PACKET_EVENTS*		GetEvents()		{return &m_vcaEvents; }
	VCA5_PACKET_COUNTS*		GetCounts()		{return &m_vcaCounts; }
	VCA5_PACKET_BLOBMAP*	GetTamperInfo() {return &m_TamperInfo;}
	VCA5_PACKET_BLOBMAP*	GetSceneChangeInfo() {return &m_SceneChangeInfo;}

	VCA5_PACKET_BLOBMAP*	GetStationaryBlobMap()	{return &m_StationaryMap;};
	ULONG*					GetTamperAlarm(){return &m_uTamperAlarm;} 
	VCA5_PACKET_STAB*		GetStab()		{return &m_vcaStab; }
	VCA5_COUNTLINE_EVENTS*	GetCountLineEvents() { return &m_vcaCountLineEvents; };
	void					ClearCountLineEvents() {memset(&m_vcaCountLineEvents, 0, sizeof(m_vcaCountLineEvents));}


protected:

	VCA5_PACKET_HEADER		m_vcaHdr;
	VCA5_PACKET_BLOBMAP		m_BlobMap;
	VCA5_PACKET_BLOBMAP		m_StationaryMap;
	VCA5_PACKET_BLOBMAP		m_TamperInfo;
	VCA5_PACKET_BLOBMAP		m_SceneChangeInfo;
	VCA5_PACKET_BLOBMAP		m_SmokeMap;
	VCA5_PACKET_BLOBMAP		m_FireMap;
	
	VCA5_PACKET_OBJECTS		m_vcaObjects;
	VCA5_PACKET_EVENTS		m_vcaEvents;
	VCA5_PACKET_COUNTS		m_vcaCounts;
	
	VCA5_PACKET_STAB		m_vcaStab;
	VCA5_COUNTLINE_EVENTS	m_vcaCountLineEvents;

	ULONG					m_uTamperAlarm;
	unsigned char			m_base64[256];
};

CVCAMetaLib* CreateVCAMetaLib(CVCAMetaLib::PARSER_TYPE type);

#endif	// _VCA_METADATA_LIB_H_