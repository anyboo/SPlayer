#ifndef AUD5BOARDLIB_EX_H
#define AUD5BOARDLIB_EX_H

#include "Cmn5BoardLibEx.h"

#pragma pack( push, Aud5BaordLibEx_H )
#pragma pack(8)

extern "C" const IID IID_IAud5 ;

//IID for Aud5
// {2408BF04-FBA9-45f7-B6D8-26A64F733611}
DEFINE_GUID(IID_IAud5, 0x2408bf04, 0xfba9, 0x45f7, 0xb6, 0xd8, 0x26, 0xa6, 0x4f, 0x73, 0x36, 0x11);

typedef struct _AUD5_BOARD_INFO{
	ULONG		uBoardID;
	ULONG		uModelID;
	ULONG		uSlotNumber;

	ULONG		uMaxChannel;

	ULONG		uFrequencyType;
	ULONG		uGainType;
	
	ULONG		reserved[CMN5_MAX_RESERVED_BOARD_INFO - 6];
} AUD5_BOARD_INFO;

//enum QueryInfoCmd {

// 0x3000 ~ 0x3FFF : CMN5 QueryInfoCmd

// };

//enum AudioPropertyCmd {
#define	AUD5_APC_SAMPLING_FREQUENCY		0x01000001
#define	AUD5_APC_BITS_PER_SAMPLE		0x01000002
// HP4K
#define HP4K_APC_STREAMING				0x01080000
#define	AUD5_APC_ENABLE_CHANNEL			0x01080001
//};

//enum AudioAdjustCmd {
#define	AUD5_AAC_GAIN					0x01000001

// HP4K
#define	AUD5_AAC_ENABLE_CHANNEL			0x01180001
//};

//enum AudioFrequencyType {
#define	AUD5_AFT_FREQUENCY_GRP1		1	// BT878
#define	AUD5_AFT_FREQUENCY_GRP2		2	// CS5333
#define	AUD5_AFT_FREQUENCY_GRP3		3	// Audio card
#define	AUD5_AFT_FREQUENCY_GRP4		4	// NVS100
//};

//enum AudioGainType {
#define	AUD5_AGT_GAIN_TYPE1			1	// legacy default gain 3
#define	AUD5_AGT_GAIN_TYPE2			2	// 0-255 range default 128
#define	AUD5_AGT_GAIN_TYPE3			3	// 1-4 range
//};

//enum AudioDataAttr {
#define	AUD5_ADA_DATA				1
#define	AUD5_ADA_BUFFER_OVERFLOW	2
#define	AUD5_ADA_PCI_ERROR			3
//};

typedef struct _AUD5_DATA_INFO {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;			// Board ID[0..3]
	ULONG			uChannelNum;		// Channel ID of the Board
	ULONG			uHasNextData;
	UCHAR			*pDataBuffer;		// buffer address
	ULONG			uLen;
#if _MSC_VER >= 1300
	LARGE_INTEGER_CAP	TimeTag;			// KeQuerySystemTime()
#else
	LARGE_INTEGER	TimeTag;			// KeQuerySystemTime()
#endif
	ULONG			uDataAttr;
	
	//-- Aud5 extended
} AUD5_DATA_INFO;

typedef struct _AUD5_DATA_INFO_EX {
	_CMN5_DATA_INFO_DATA_EX_CMM_BODY;
	union {
		ULONG			reserved[32];
		struct {
			ULONG			uLen;
			ULONG			uSequenceNumber;	// 2009.11.23 jbyoon
		};
	};
} AUD5_DATA_INFO_EX;

#ifndef KERNEL_MODE_CODE

extern "C" {
//////////////////////////////////////////////////////////////////////////////////
// API

BOOL CMN5_API Aud5GetLastErrorCode(CMN5_ERROR_CODE_ITEM *pEcode);

BOOL CMN5_API Aud5QueryInfo(ULONG uQueryInfoCmd, ULONG uIn, void *pOut);

BOOL CMN5_API Aud5GetSystemInfo(CMN5_SYSTEM_INFO *pInfo);
BOOL CMN5_API Aud5GetBoardInfo(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, AUD5_BOARD_INFO *pInfo);

BOOL CMN5_API Aud5Activate(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]);

BOOL CMN5_API Aud5SetCaptureMethod(ULONG uFuncType, ULONG uMethod);
BOOL CMN5_API Aud5SetCallback(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext);
BOOL CMN5_API Aud5QueryData(void *pData, ULONG uTimeOut);
BOOL CMN5_API Aud5ReleaseData(const void *pData);
BOOL CMN5_API Aud5SetEventHandle(ULONG uType, HANDLE hHandle);
BOOL CMN5_API Aud5GetEventHandle(ULONG uType, HANDLE *pHandle);
BOOL CMN5_API Aud5GetEventData(ULONG uType, void *pData);

BOOL CMN5_API Aud5Setup(void);
BOOL CMN5_API Aud5Endup(void);
BOOL CMN5_API Aud5Run(void);
BOOL CMN5_API Aud5Stop(void);

BOOL CMN5_API Aud5ChannelEnable(ULONG uBdID, ULONG uCh, BOOL bEnable);

BOOL CMN5_API Aud5SetAudioProperty(ULONG uBdID, ULONG uCh, ULONG uAudioPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Aud5GetAudioProperty(ULONG uBdID, ULONG uCh, ULONG uAudioPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);
BOOL CMN5_API Aud5SetAudioAdjust(ULONG uBdID, ULONG uCh, ULONG uAudioAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Aud5GetAudioAdjust(ULONG uBdID, ULONG uCh, ULONG uAudioAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Aud5SetMultiAudioProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Aud5GetMultiAudioProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Aud5SetMultiAudioAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Aud5GetMultiAudioAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

BOOL CMN5_API Aud5SetNotificationPeriod(ULONG uBdID, ULONG uPeriod);
BOOL CMN5_API Aud5GetNotificationPeriod(ULONG uBdID, ULONG* puPeriod);

BOOL CMN5_API Aud5ReadUserEEPROM(ULONG uBdID, ULONG uVPID, UCHAR *pData);
BOOL CMN5_API Aud5WriteUserEEPROM(ULONG uBdID, ULONG uVPID,const UCHAR *pData);

// *********************************************************************** 
// Vendoer Specific IO API
// *********************************************************************** 
BOOL CMN5_API Aud5SetSpecialIOMode(ULONG uBdID, ULONG uData);
BOOL CMN5_API Aud5GetSpecialIOMode(ULONG uBdID, ULONG *puData);
BOOL CMN5_API Aud5WriteSpecialIOData(ULONG uBdID, ULONG uData);
BOOL CMN5_API Aud5ReadSpecialIOData(ULONG uBdID, ULONG *puData);
} // extern "C"

// Interface definition for IAud5
interface IAud5 : IUnknown
{
	STDMETHOD_(BOOL, Aud5GetLastErrorCode)(CMN5_ERROR_CODE_ITEM *pEcode) PURE;

	STDMETHOD_(BOOL, Aud5QueryInfo)(ULONG uQueryInfoCmd, ULONG uIn, void *pOut) PURE;

	STDMETHOD_(BOOL, Aud5GetSystemInfo)(CMN5_SYSTEM_INFO *pInfo) PURE;
	STDMETHOD_(BOOL, Aud5GetBoardInfo)(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, AUD5_BOARD_INFO *pInfo) PURE;

	STDMETHOD_(BOOL, Aud5Activate)(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]) PURE;

	STDMETHOD_(BOOL, Aud5SetCaptureMethod)(ULONG uFuncType, ULONG uMethod) PURE;
	STDMETHOD_(BOOL, Aud5SetCallback)(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext) PURE;
	STDMETHOD_(BOOL, Aud5QueryData)(void *pData, ULONG uTimeOut) PURE;
	STDMETHOD_(BOOL, Aud5ReleaseData)(const void *pData) PURE;
	STDMETHOD_(BOOL, Aud5SetEventHandle)(ULONG uType, HANDLE hHandle) PURE;
	STDMETHOD_(BOOL, Aud5GetEventHandle)(ULONG uType, HANDLE *pHandle) PURE;
	STDMETHOD_(BOOL, Aud5GetEventData)(ULONG uType, void *pData) PURE;

	STDMETHOD_(BOOL, Aud5Setup)(void) PURE;
	STDMETHOD_(BOOL, Aud5Endup)(void) PURE;
	STDMETHOD_(BOOL, Aud5Run)(void) PURE;
	STDMETHOD_(BOOL, Aud5Stop)(void) PURE;

	STDMETHOD_(BOOL, Aud5ChannelEnable)(ULONG uBdID, ULONG uCh, BOOL bEnable) PURE;

	STDMETHOD_(BOOL, Aud5SetAudioProperty)(ULONG uBdID, ULONG uCh, ULONG uAudioPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Aud5GetAudioProperty)(ULONG uBdID, ULONG uCh, ULONG uAudioPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;
	STDMETHOD_(BOOL, Aud5SetAudioAdjust)(ULONG uBdID, ULONG uCh, ULONG uAudioAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Aud5GetAudioAdjust)(ULONG uBdID, ULONG uCh, ULONG uAudioAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;

	STDMETHOD_(BOOL, Aud5SetMultiAudioProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Aud5GetMultiAudioProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Aud5SetMultiAudioAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Aud5GetMultiAudioAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;

	STDMETHOD_(BOOL, Aud5SetNotificationPeriod)(ULONG uBdID, ULONG uPeriod) PURE;
	STDMETHOD_(BOOL, Aud5GetNotificationPeriod)(ULONG uBdID, ULONG* puPeriod) PURE;

	STDMETHOD_(BOOL, Aud5ReadUserEEPROM)(ULONG uBdID, ULONG uVPID, UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Aud5WriteUserEEPROM)(ULONG uBdID, ULONG uVPID,const UCHAR *pData) PURE;

	// *********************************************************************** 
	// Vendoer Specific IO API
	// *********************************************************************** 
	STDMETHOD_(BOOL, Aud5SetSpecialIOMode)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Aud5GetSpecialIOMode)(ULONG uBdID, ULONG *puData) PURE;
	STDMETHOD_(BOOL, Aud5WriteSpecialIOData)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Aud5ReadSpecialIOData)(ULONG uBdID, ULONG *puData) PURE;
};

#endif

#pragma pack( pop, Aud5BaordLibEx_H )

#endif // Aud5BaordLib_EX_H