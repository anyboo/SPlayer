#ifndef NET5BOARDLIB_EX_H
#define NET5BOARDLIB_EX_H

#include "Cmn5BoardLibEx.h"

#pragma pack( push, Net5BoardLibEx_H )
#pragma pack(8)

extern "C" const IID IID_INet5 ;

//IID for Net5
// {A0C718FE-A5CD-41bf-AF74-903F05A7C131}
DEFINE_GUID( IID_INet5, 0xa0c718fe, 0xa5cd, 0x41bf, 0xaf, 0x74, 0x90, 0x3f, 0x5, 0xa7, 0xc1, 0x31 );

#define	NET5_MAX_ID_LENGTH			50
#define	NET5_MAX_PW_LENGTH			50
#define	NET5_MAX_USER_LIST			10
#define	NET5_IP_ADDR_LENGTH			20
#define	NET5_NUM_OF_DNS				2
#define	NET5_MAX_DOMAIN_NAME_LENGTH		128

//enum UserGroup {
#define	NET5_UG_ADMINISTRATOR		1
#define	NET5_UG_USER				2
#define	NET5_UG_GUEST				3
//};

//IP Address Config
#define NET5_IAC_STATIC		1
#define NET5_IAC_DHCP		2

typedef struct _NET5_ADDR_INFO {
	char	Address[NET5_IP_ADDR_LENGTH];
	char	SubnetMask[NET5_IP_ADDR_LENGTH];
	char	Gateway[NET5_IP_ADDR_LENGTH];
	char	DNS[NET5_NUM_OF_DNS][NET5_IP_ADDR_LENGTH];
} NET5_ADDR_INFO;

#define NET5_MAX_RESERVED_DIR_SERVER_INFO	64

typedef union {
	struct {
		BOOL	bEnable;
		ULONG	uPort;
		char	szIP[NET5_IP_ADDR_LENGTH];
		char	szID[NET5_MAX_ID_LENGTH];
		char	szPassword[NET5_MAX_PW_LENGTH];
	};
	ULONG	reserved[NET5_MAX_RESERVED_DIR_SERVER_INFO];
} NET5_DIR_SERVER_INFO;

//enum NetDataType {
#define	NET5_NDT_CONNECTION				1
#define	NET5_NDT_CHANGE_PROPERTY		2
#define	NET5_NDT_RECV_SERIAL_DATA		3
#define	NET5_NDT_UNSTABLE_CONNECTION	4
//};

typedef struct _NET5_EVENT_DATA {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;

	ULONG			uNetDataType;
} NET5_EVENT_DATA;

#define NET5_MI_UPDATE_NOT_REQUIRED		1
#define NET5_MI_REQUIRE_SERVER_UPDATE	2
#define NET5_MI_REQUIRE_CLIENT_UPDATE	3
#define NET5_MAX_RESERVED_VERSION_INFO	16

typedef union {
	struct {
		ULONG uCurServerVer;			// Current server version
		ULONG uMinRequiredClientVer;	// Minimum required client version
		ULONG uCurClientVer;			// Current client version
		ULONG uMinRequiredServerVer;	// Minimum required server version
		ULONG uMatchInfo;				// Version match information
	};
	ULONG reserved[NET5_MAX_RESERVED_VERSION_INFO];
} NET5_VERSION_INFO;


// enum AudioTxLock {
#define NET5_ATL_LOCK				1
#define NET5_ATL_FORCED_LOCK		2
#define NET5_ATL_UNLOCK				3
#define NET5_ATL_GET_STATUS			4
//};

#define NET5_MAX_RESERVED_AUTOMATION_INFO	16

// enum EventAutomationType
#define NET5_EAT_DDNS				1
//};

// enum DDNS service type {
#define	NET5_DDNS_SERVICE_TYPE_DYNDNS		1
//};

// enum EventAutomationCommand {
#define NET5_EAC_ADD			1
#define NET5_EAC_DELETE			2
#define NET5_EAC_MODIFY			3
//};

#define _NET5_EVENT_AUTOMATION_INFO_HEADER_BODY				\
	union {													\
		ULONG allHdr[NET5_MAX_RESERVED_AUTOMATION_INFO];	\
		struct {											\
			GUID	uID;									\
			ULONG	uAutomationType;						\
			ULONG	uStructSize;							\
			ULONG	uCommand;								\
		};													\
	}														\

typedef struct _NET5_EVENT_AUTOMATION_INFO_HEADER {
	_NET5_EVENT_AUTOMATION_INFO_HEADER_BODY;
} NET5_EVENT_AUTOMATION_INFO_HEADER;

typedef struct _NET5_EVENT_AUTOMATION_INFO_DDNS {
	_NET5_EVENT_AUTOMATION_INFO_HEADER_BODY;
	union {
		ULONG all[NET5_MAX_RESERVED_AUTOMATION_INFO*8];
		struct {
			ULONG	uDdnsServiceType;
			char	DdnsServerAddr[NET5_MAX_DOMAIN_NAME_LENGTH];
			char	DdnsUserName[NET5_MAX_ID_LENGTH];
			char	DdnsPassword[NET5_MAX_PW_LENGTH];
			char	DomainName[NET5_MAX_DOMAIN_NAME_LENGTH];
			ULONG	uUpdateIntervalMin;
		};
	};
} NET5_EVENT_AUTOMATION_INFO_DDNS;

#define	NET5_MAX_RESERVED_MULTICAST_INFO	64

typedef union {
	struct {
		char Address[NET5_MAX_DOMAIN_NAME_LENGTH];
		ULONG uPort;
		ULONG uTTL;
	};
	ULONG reserved[NET5_MAX_RESERVED_MULTICAST_INFO];
} NET5_MULTICAST_INFO;

#define	NET5_MAX_RESERVED_STATISTICS_INFO	16

typedef union {
	struct {
		ULONG	uLostVideoFrame;
		ULONG	uLostAudioPacket;
		ULONG	uLostEventPacket;
	};
	ULONG	reserved[NET5_MAX_RESERVED_STATISTICS_INFO];
} NET5_STATISTICS_INFO;

// Encryption Media type
#define NET5_EMT_VIDEO			1
#define NET5_EMT_AUDIO			2

//enum NetCommand {
// Admin Level
	// User Management
#define	NET5_NC_ADD_USER_ID					1001	// LPCSTR sUserId,		LPCSTR sUserPw,		ULONG *puIdInfo
#define	NET5_NC_DEL_USER_ID					1002	// LPCSTR sUserId
#define	NET5_NC_GET_USER_PW					1003	// LPCSTR sUserId,		[out]LPSTR uUserPw
#define	NET5_NC_GET_ALL_USER_LIST			1004	// [out]LPSTR *pUserList
#define	NET5_NC_GET_CONNECTION_NUM			1005	// [out]ULONG *pCurrentUserNum
#define	NET5_NC_GET_CONNECTION_LIST			1006	// [out]LPSTR *pCurrentUserList
// renamed (Use above names instead.)
#define	NET5_NC_GET_CURRENT_USER_NUM		NET5_NC_GET_CONNECTION_NUM
#define	NET5_NC_GET_CURRENT_USER_LIST		NET5_NC_GET_CONNECTION_LIST
#define	NET5_NC_GET_ALL_USER_NUM			1007	// [out]ULONG *pAllUserNum
#define	NET5_NC_CHANGE_USER_ID_DATA			1008	// LPCSTR pUserId,		ULONG *puIdData

	// System Management
#define	NET5_NC_SET_ADDR					1101	// NET5_ADDR_INFO DataInfo
#define	NET5_NC_SET_PORT					1102	// ULONG *puWebServerPort,ULONG* puStreamServerPort
#define	NET5_NC_SET_SYSTEM_NAME				1103	// LPCSTR sSystemName
#define	NET5_NC_SET_SYSTEM_TIME				1104	// SYSTEMTIME SystemTime
#define	NET5_NC_FIRMWARE_UPDATE				1105	// LPCSTR sFilePath
#define	NET5_NC_RESTART						1106	// void
#define NET5_NC_SET_SNTP_ADDRESS			1107	// LPCSTR pAddress1, LPCSTR pAddress2, LPCSTR pAddress3
#define NET5_NC_FACTORY_DEFAULT				1108	// ULONG *puLevel (1: Hard, 2: Soft)
#define NET5_NC_SET_DHCP					1109	// ULONG *puEnable
#define NET5_NC_SET_IP_ADDR_CONFIG			1110	// ULONG *puIPAddrConf
#define NET5_NC_SET_DIR_SERVER_INFO			1111	// NET5_DIR_SERVER_INFO *pInfo

// User Level
	// ID Info
#define	NET5_NC_CHANGE_PW					2101	// LPCSTR sUserId,		LPCSTR sNewPw
#define	NET5_NC_GET_ID_DATA					2102	// LPCSTR sUserId,		ULONG *puIdData

	// System Info
#define	NET5_NC_GET_SERVER_ADDR				2201	// NET5_ADDR_INFO *pAddrInfo
#define	NET5_NC_GET_SYSTEM_NAME				2202	// LPSTR SystemName
#define	NET5_NC_GET_SYSTEM_TIME				2203	// SYSTEMTIME *pSystemTime
#define	NET5_NC_IS_CONNECTION				2204	// BOOL *pbOnOff
#define	NET5_NC_GET_PORT					2205	// [out]ULONG *puWebServerPort, [out]ULONG *puStreamServerPort
#define	NET5_NC_SEND_SERIAL_DATA			2206	// ULONG *puPortNumber,	BYTE *pData,		ULONG *puDataSize
#define	NET5_NC_RECV_SERIAL_DATA			2207	// ULONG *puPortNumber,	BYTE *pData,		ULONG *puMaxSize, ULONG *puDataSize
#define	NET5_NC_SET_COMM_STATE				2208	// ULONG *puPortNum,	DCB *pDCB
#define	NET5_NC_GET_COMM_STATE				2209	// ULONG *puPortNum,	[out]DCB *pDCB
#define	NET5_NC_GET_VERSION					2210	// const CMN5_BOARD_INFO_DESC *pDesc, NET5_VERSION_INFO *pInfo
#define	NET5_NC_GET_LOG						2211	// CMN5_ERROR_CODE_ITEM *pLogInfo
#define NET5_NC_GET_SNTP_ADDRESS			2212	// LPCSTR pAddress1, LPCSTR pAddress2, LPCSTR pAddress3
#define NET5_NC_GET_DHCP					2213	// ULONG *puEnable
#define NET5_NC_GET_IP_ADDR_CONFIG			2214	// ULONG *puIPAddrConf
#define NET5_NC_GET_DIR_SERVER_INFO			2215	// NET5_DIR_SERVER_INFO *pInfo
#define NET5_NC_GET_STATISTICS				2216	// NET5_STATISTICS_INFO *pInfo

	// Resource Management
#define NET5_NC_AUDIO_TX_LOCK				2301	// ULONG *puSubCmd, ULONG *puChannel, ULONG *puStatus

	// Automation
#define NET5_NC_SET_EVENT_AUTOMATION		2401	// ULONG *puNumInfo, NET5_EVENT_AUTOMATION_INFO_HEADER *pInfos
#define NET5_NC_QUERY_EVENT_AUTOMATION_SIZE	2402	// ULONG *puSize, ULONG *puNum
#define NET5_NC_GET_ALL_EVENT_AUTOMATION	2403	// ULONG *puSize, NET5_EVENT_AUTOMATION_INFO_HEADER *pInfos

	// File management
#define NET5_NC_WRITE_USER_FILE_DATA		2501	// LPCTSTR pFileName, VOID *pData, ULONG *puDataSize, BOOL *bAppend
#define NET5_NC_GET_USER_FILE_DATA_SIZE		2502	// LPCTSTR pFileName, [out]ULONG *puDataSize
#define NET5_NC_READ_USER_FILE_DATA			2503	// LPCTSTR pFileName, VOID *pData, ULONG *puDataSize

	// Multicast
#define NET5_NC_GET_MULTICAST_INFO			2601	// MULTICAST_INFO *pInfo
#define NET5_NC_SET_MULTICAST_INFO			2602	// MULTICAST_INFO *pInfo
#define NET5_NC_ENABLE_MULTICAST			2603	// BOOL *pbEnable

// Extension commands
	// RTSP
#define NET5_NC_SEND_RTSP_REQUEST			3001	// LPCTSTR pRequest
#define NET5_NC_RECV_RTSP_RESPONSE			3002	// LPCSTR pResponse

	// CGI
#define NET5_NC_SEND_CGI_REQUEST			3101	// LPCTSTR pRequest
#define NET5_NC_RECV_CGI_RESPONSE			3102	// LPCSTR pResponse

	// Encryption
#define NET5_NC_SET_ENCRYPTION_KEY			3201	// ULONG uMediaType, UCHAR *pKey
//};

//enum ServerType {
#define	NET5_ST_VIDEO_SERVER			1
#define	NET5_ST_DIRECTORY_SERVER		2
#define	NET5_ST_MULTICAST				3
//};

// enum StreamingType {
#define NET5_ST_RTP_UDP					1
#define NET5_ST_RTP_TCP					2
#define NET5_ST_HTTP_TUNNELING			3
#define NET5_ST_MULTICAST_UDP			4
//};

extern "C"
{
//////////////////////////////////////////////////////////////////////////////////
// APIs
BOOL CMN5_API exNet5Connect(ULONG *puID, LPCSTR sAddress, LPCSTR sUserID, LPCSTR sUserPW, ULONG uServerType, ULONG *puIdData, ULONG uTimeOut);
BOOL CMN5_API exNet5Disconnect(ULONG uID);

BOOL CMN5_API exNet5Command(ULONG uID, ULONG uVideoServerCmd, ULONG *puParam1, ULONG *puParam2, ULONG *puParam3, ULONG *puParam4);

BOOL CMN5_API exNet5SetApiTimeOut(ULONG uID, ULONG uTimeOut);
BOOL CMN5_API exNet5GetApiTimeOut(ULONG uID, ULONG *puTimeOut);

BOOL CMN5_API exNet5SetEventHandle(ULONG uID, ULONG uType, HANDLE hHandle);
BOOL CMN5_API exNet5GetEventHandle(ULONG uID, ULONG uType, HANDLE *pHandle);
BOOL CMN5_API exNet5GetEventData(ULONG uID, ULONG uType, void *pData);

BOOL CMN5_API exNet5GetLastErrorCode(ULONG uID, CMN5_ERROR_CODE_ITEM *pEcode);

BOOL CMN5_API exNet5ConnectEx(ULONG *puID, LPCSTR pszAddress, ULONG uStreamingType, ULONG uTimeOut);
BOOL CMN5_API exNet5Login(ULONG uID, LPCSTR pszUserID, LPCSTR pszUserPW);
BOOL CMN5_API exNet5Logout(ULONG uID);

// Cod5 export APIs for NVE series
BOOL CMN5_API exCod5GetLastErrorCode(ULONG uID, CMN5_ERROR_CODE_ITEM *pEcode);
BOOL CMN5_API exCod5QueryInfo(ULONG uID, ULONG uQueryInfoCmd, ULONG uIn, void *pOut);

BOOL CMN5_API exCod5Setup(ULONG uID);
BOOL CMN5_API exCod5Endup(ULONG uID);
BOOL CMN5_API exCod5Run(ULONG uID);
BOOL CMN5_API exCod5Stop(ULONG uID);

BOOL CMN5_API exCod5ReleaseData(ULONG uID, const void *pData);
BOOL CMN5_API exCod5SetEventHandle(ULONG uID, ULONG uType, HANDLE hHandle);
BOOL CMN5_API exCod5GetEventHandle(ULONG uID, ULONG uType, HANDLE *pHandle);
BOOL CMN5_API exCod5GetEventData(ULONG uID, ULONG uType, void *pData);
BOOL CMN5_API exCod5SetEventData(ULONG uID, ULONG uType, void *pData);
} // extern "C"

// Interface definition for INet5
interface INet5 : IUnknown
{
	STDMETHOD_(BOOL, Net5Connect)(LPCSTR sAddress, LPCSTR sUserID, LPCSTR sUserPW, ULONG uServerType, ULONG *puIdData, ULONG uTimeOut) PURE;
	STDMETHOD_(void, Net5Disconnect)(void) PURE;

	STDMETHOD_(BOOL, Net5Command)(ULONG uVideoServerCmd, ULONG *puParam1, ULONG *puParam2, ULONG *puParam3, ULONG *puParam4) PURE;

	STDMETHOD_(BOOL, Net5SetApiTimeOut)(ULONG uTimeOut) PURE;
	STDMETHOD_(BOOL, Net5GetApiTimeOut)(ULONG *uTimeOut) PURE;

	STDMETHOD_(BOOL, Net5SetEventHandle)(ULONG uType, HANDLE hHandle) PURE;
	STDMETHOD_(BOOL, Net5GetEventHandle)(ULONG uType, HANDLE *pHandle) PURE;
	STDMETHOD_(BOOL, Net5GetEventData)(ULONG uType, void *pData) PURE;

	STDMETHOD_(BOOL, Net5GetLastErrorCode)(CMN5_ERROR_CODE_ITEM *pEcode) PURE;

	STDMETHOD_(BOOL, Net5ConnectEx)(LPCSTR pszAddress, ULONG uStreamingType, ULONG uTimeOut) PURE;
	STDMETHOD_(BOOL, Net5Login)(LPCSTR pszUserID, LPCSTR pszUserPW) PURE;
	STDMETHOD_(BOOL, Net5Logout)() PURE;
};

#pragma pack( pop, Net5BoardLibEx_H )


#endif
