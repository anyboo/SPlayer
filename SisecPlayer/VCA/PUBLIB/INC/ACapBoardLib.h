#ifndef DVRAUDDLL_H
#define DVRAUDDLL_H

#include "ErrCmnError.h"

#define DVRAUD_API __declspec(dllimport)

#if defined(STD_CALL)
#define CALL_MTD	__stdcall
#else
#define CALL_MTD
#endif

/*-----------------------------------------------------------
	Input gain table
  -----------------------------------------------------------
value	Input GAIN		dB		Nominal Input Vrms		Vp-p
0		0.500			-6.02	1.000					2.828
1		0.667			-3.52	0.750					2.121
2		0.833			-1.58	0.600					1.697
3		1.000			0.00	0.500					1.414
4		1.167			1.34	0.429					1.212
5		1.333			2.50	0.375					1.061
6		1.500			3.52	0.333					0.943
7		1.667			4.44	0.300					0.849
8		1.833			5.26	0.273					0.771
9		2.000			6.02	0.250					0.707
10		2.167			6.72	0.231					0.653
11		2.333			7.36	0.214					0.606
12		2.500			7.96	0.200					0.566
13		2.667			8.52	0.188					0.530
14		2.833			9.05	0.176					0.499
15		3.000			9.54	0.167					0.471
*/

// Bits per sample: 8 or 16-bit

// Sampling frequencies supported
// 44.8 KHz, 32 KHz, 22.4 KHz, 16 KHz, 11.2 KHz, 8 KHz, 5.6 KHz, 4 KHz
// specify sampling frequency in Hz.


#define AUD_SAMP_ATTR_NORMAL		0x0000
#define AUD_SAMP_ATTR_BUF_OVERLOW	0x0001
#define AUD_SAMP_ATTR_PCI_ERROR		0x0002
typedef struct {
	BYTE*			buf;
	ULONG			len;
	FILETIME		timestamp;
	ULONG			attr;			// [1..0]	00: normal sample
									//			01: buffer overflow
									//			10: PCI error (re-start)
	ULONG			reserved[11];
} AUDIO_SAMPLE_INFO;


//______________________________________________________________________________________________
// Definition of System info
//______________________________________________________________________________________________
#define AUD_SYSTEM_MAXBOARD				4
#define AUD_BOARD_MAXAP					4
#define AUD_BOARD_MAXCHANNEL			16

#define AUD_SIZE_PURE_AP_INFO			16
#define AUD_SIZE_PURE_BOARD_INFO		32
#define AUD_SIZE_PURE_SYSTEM_INFO		32

#define AUD_SIZE_OF_AP_INFO				(AUD_SIZE_PURE_AP_INFO)
#define AUD_SIZE_OF_BOARD_INFO			(AUD_SIZE_PURE_BOARD_INFO+AUD_BOARD_MAXAP*AUD_SIZE_OF_AP_INFO)
#define AUD_SIZE_OF_SYSTEM_INFO			(AUD_SIZE_PURE_SYSTEM_INFO+AUD_SYSTEM_MAXBOARD*AUD_SIZE_OF_BOARD_INFO)

typedef union {
	struct {
		UCHAR	APID;
		UCHAR	ADCType;
	};
	UCHAR reserved[AUD_SIZE_OF_AP_INFO];
} AUD_AP_INFO;

typedef union {
	struct {
		ULONG	BoardHWId;		// oemid
		UCHAR	NumOfAP;
		UCHAR	NumOfChannels;
		AUD_AP_INFO APInfo[AUD_BOARD_MAXAP];
	};
	UCHAR reserved[AUD_SIZE_OF_BOARD_INFO];
} AUD_BOARD_INFO;

typedef union {
	struct {
		ULONG	SystemId;		// ?
		UCHAR	NumOfBoard;
		AUD_BOARD_INFO BoardInfo[AUD_SYSTEM_MAXBOARD];
	};
	UCHAR reserved[AUD_SIZE_OF_SYSTEM_INFO];
} AUD_SYSTEM_INFO;
//______________________________________________________________________________________________
// End of definition
//______________________________________________________________________________________________

enum ADC_TYPE {ADC_BT878_INTERNAL = 0, ADC_BT878_EXTERNAL_CS5333, ADC_DEDICATED_BRD_CS5333, ADC_NONE};

// Call Back Function
typedef BOOL (CALL_MTD *FUNC_ON_AUDIO_CAPTURE)(DWORD channel, AUDIO_SAMPLE_INFO* info, DWORD reserved1);
typedef BOOL (CALL_MTD *FUNC_ON_AUDIO_OVERFLOW)(DWORD status, DWORD reserved1, DWORD reserved2);

//////////////////////////////////////////////////////////////////////////////////
// Device Driver API
DVRAUD_API BOOL 	CALL_MTD AudSetup(UCHAR* serialNumber);
DVRAUD_API BOOL 	CALL_MTD AudEndup(void);
DVRAUD_API BOOL 	CALL_MTD AudRun(void);
DVRAUD_API BOOL 	CALL_MTD AudStop(void);
DVRAUD_API BOOL 	CALL_MTD AudStartRecording(DWORD channel);
DVRAUD_API BOOL 	CALL_MTD AudStopRecording(DWORD channel);
DVRAUD_API void 	CALL_MTD AudSetFuncOnCaptureAudio(FUNC_ON_AUDIO_CAPTURE func);
DVRAUD_API void		CALL_MTD AudSetFuncOnAudioOverflow(FUNC_ON_AUDIO_OVERFLOW func);
DVRAUD_API BOOL		CALL_MTD AudSetGain(DWORD channel, int gain);
DVRAUD_API BOOL		CALL_MTD AudSetBitsPerSample(DWORD channel, int bits);
DVRAUD_API BOOL		CALL_MTD AudSetSampleRate(DWORD channel, int rate);
DVRAUD_API BOOL		CALL_MTD AudSetCallbackInterval(DWORD numCallbacksPerSecond);
DVRAUD_API BOOL		CALL_MTD AudEnableReportOverflow(DWORD channel, BOOL bEnable);

DVRAUD_API BOOL		CALL_MTD AudIsDeviceInstalled();
DVRAUD_API BOOL		CALL_MTD AudGetDeviceOemId(ULONG *id);
DVRAUD_API BOOL		CALL_MTD AudShowErrorCode();
DVRAUD_API BOOL		CALL_MTD AudGetErrorCode(ErrorCodeItem* item);
//DVRAUD_API BOOL		CALL_MTD AudGetDeviceCaps(AUD_DEVICE_CAPS* caps);
DVRAUD_API BOOL		CALL_MTD AudGetSystemInfo(AUD_SYSTEM_INFO* pSysInfo);

#endif