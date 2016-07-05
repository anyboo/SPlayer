#ifndef AUD3BOARDLIB_H
#define AUD3BOARDLIB_H

/*! \file Aud3BoardLib.h
Multi-board audio API header file.
*/

#if defined(AUDLIB_EXPORTS)
#	define DVRAUD_API  extern "C"
#else
#	define DVRAUD_API  extern "C" __declspec(dllimport) 
#endif

#define CALLING_API	__stdcall

#if defined(KERNEL_MODE_CODE)
	typedef unsigned long DWORD;
#endif

#if !defined(UDA_ULONG_DEFINED)
#	if !defined(_WIN64)
		typedef ULONG		UDA_ULONG;
#	else
		typedef ULONG_PTR	UDA_ULONG;
#	endif
#	define UDA_ULONG_DEFINED
#endif

#pragma pack( push, Aud3BoardLib_h )
#if !defined(_WIN64)
#pragma pack(1)
#else
#	pragma pack(16)
#endif

//______________________________________________________________________________________________
// Definition of System info

#define AUD_BOARD_MAXVP_V3				16
#define AUD_BOARD_MAXCHANNEL_V3			16
#define AUD_SYSTEM_MAXBOARD_V3			4

#define AUD_SIZE_PURE_VP_INFO_V3		4
#define AUD_SIZE_PURE_BOARD_INFO_V3		16
#define AUD_SIZE_PURE_SYSTEM_INFO_V3	64

#define AUD_SIZE_OF_VP_INFO_V3			(AUD_SIZE_PURE_VP_INFO_V3)
#define AUD_SIZE_OF_BOARD_INFO_V3		(AUD_SIZE_PURE_BOARD_INFO_V3+AUD_BOARD_MAXVP_V3*AUD_SIZE_OF_VP_INFO_V3)
#define AUD_SIZE_OF_SYSTEM_INFO_V3		(AUD_SIZE_PURE_SYSTEM_INFO_V3+AUD_SYSTEM_MAXBOARD_V3*AUD_SIZE_OF_BOARD_INFO_V3)

/*!
 * Shows VP information.
 */
typedef union {
	struct {
		UCHAR	VPID;		/*!< VP ID. */
		UCHAR	ADCType;	/*!< ADC type. */
	};
	UCHAR reserved[AUD_SIZE_OF_VP_INFO_V3];
} AUD_VP_INFO_V3;		// 4(pure) bytes

/*!
 * Shows Board information.
 */
typedef union {
	struct {
		ULONG	BoardHWId;		/*!< The model ID of the board. */
		UCHAR	NumOfVP;		/*!< The number of VP. (Informational) */
		UCHAR	NumOfChannels;	/*!< The number of channels supported by the board. */
		UCHAR	SlotNumber;		/*!< PCI slot number. This slot number can be different from the silk on the mother board.*/
		AUD_VP_INFO_V3 VPInfo[AUD_BOARD_MAXVP_V3];	/*!< VP information structure. */
	};
	UCHAR reserved[AUD_SIZE_OF_BOARD_INFO_V3];
} AUD_BOARD_INFO_V3;	// 16(pure) + 16(VP#)*4(VP total) = 80 bytes

/*!
 * Shows System information
 */
typedef union {
	struct {
		ULONG	SystemId;		/*!< Reserved. */
		ULONG	NumOfBoard;		/*!< The number of the board installed on the mother board. */
		ULONG	SysVersion;		/*!< Reserved. */
		ULONG	DllVersion;		/*!< Reserved. */
		AUD_BOARD_INFO_V3 BoardInfo[AUD_SYSTEM_MAXBOARD_V3];	/*!< Board information structure. */
	};
	UCHAR reserved[AUD_SIZE_OF_SYSTEM_INFO_V3];
} AUD_SYSTEM_INFO_V3;	// 64(pure) + 4(Brd#)*80(Brd total) = 384 bytes

//______________________________________________________________________________________________
// Definition of System info Descriptor

/*!
 * System information descriptor. This structure must be filled before getting system information.
 */
typedef struct {
	ULONG	InfoSize;		/*!< The structure size of \e AUD_SYSTEM_INFO in byte. */
	ULONG	InfoName;		/*!< Identifier indicating audio system information structure. Use AUD_SYSTEM_INFO_NAME. */
	ULONG	InfoVersion;	/*!< Version number of AUD_SYSTEM_INFO. */
	ULONG	reserved;		/*!< Reserved. */
} AUD_SYSTEM_INFO_DESC;

#if (USE_AUD_INFO_VERSION == 1) // For now both are same
	#define AUD_SYSTEM_MAXBOARD			AUD_SYSTEM_MAXBOARD_V3
	#define AUD_BOARD_MAXVP				AUD_BOARD_MAXVP_V3
	#define AUD_BOARD_MAXCHANNEL		AUD_BOARD_MAXCHANNEL_V3

	#define AUD_VP_INFO					AUD_VP_INFO_V3
	#define AUD_BOARD_INFO				AUD_BOARD_INFO_V3
	#define AUD_SYSTEM_INFO				AUD_SYSTEM_INFO_V3

	#define AUD_SYSTEM_INFO_NAME		0x43445541	// reversed 'AUDC'
	#define AUD_SYSTEM_INFO_VERSION		3
#else
	#define AUD_SYSTEM_MAXBOARD			AUD_SYSTEM_MAXBOARD_V3
	#define AUD_BOARD_MAXVP				AUD_BOARD_MAXVP_V3
	#define AUD_BOARD_MAXCHANNEL		AUD_BOARD_MAXCHANNEL_V3

	#define AUD_VP_INFO					AUD_VP_INFO_V3
	#define AUD_BOARD_INFO				AUD_BOARD_INFO_V3
	#define AUD_SYSTEM_INFO				AUD_SYSTEM_INFO_V3

//! Used at \e InfoName of the structure AUD_SYSTEM_INFO_DESC.
	#define AUD_SYSTEM_INFO_NAME		0x43445541	// reversed 'AUDC'
//! Used at \e InfoVersion of the structure AUD_SYSTEM_INFO_DESC. For example, if the version number is 3, the driver retreives the structure postfix with '_V3'.
	#define AUD_SYSTEM_INFO_VERSION		3
#endif

//______________________________________________________________________________________________
// Definition of ErrorCodeItem
#if !defined (CAP_OVR_AUD)											
//#if !defined (AUD_ONLY)

#ifndef ERRORCODEITEM_DEFINITION
#define ERRORCODEITEM_DEFINITION

#define MAX_ERRCMN_ERROR_CODE_MSG_LENGTH		128
#define MAX_ERRCMN_SESSION_LENGTH				64

/*!
 * The structure that describes error code occurred from the driver and the DLL.
 */
struct ErrorCodeItem {
	ULONG			ErrorCode;										/*!< Error code. */
	LARGE_INTEGER	TimeStamp;										/*!< The time that the error was occurred. */
	ULONG			Reserved;										/*!< Reserved. */
	char			AuxMsg[MAX_ERRCMN_ERROR_CODE_MSG_LENGTH + 1];	/*!< Readable string that describes error. */
	char			SessionName[MAX_ERRCMN_SESSION_LENGTH + 1];		/*!< This indicates what the session was when the error has occurred. */
};
#endif // ERRORCODEITEM_DEFINITION

#endif // CAP_OVR_AUD

//______________________________________________________________________________________________
// 

#define AUD_ACTIVATION_CODE_SIZE			16

/*!
 * ADC type
 */
enum ADC_TYPE {
	ADC_BT878_INTERNAL = 0,		/*!< BT878 internal audio function is used. */
	ADC_BT878_EXTERNAL_CS5333,	/*!< External CS5333 AD converter is used. */
	ADC_DEDICATED_BRD_CS5333,	/*!< 16-channel audio dedicated board. */
	ADC_NONE,					/*!< No audio function is available. */
	ADC_BT878_EXTERNAL_CS53L32	/*!< External CS53L32 AD converter is used. */
};

/*!
 * System state
 */
enum AUD_SYSTEM_STATE {
	AUD_STATE_NONE	=0,		/*!< Uninitialized state. */
	AUD_STATE_SETUP	=1,		/*!< Set-up state. */
	AUD_STATE_RUN	=2,		/*!< Running state. */
};

#if !defined(KERNEL_MODE_CODE)

/*!
 * Capture method
 */
enum {
	// method
	AUD_CAPTURE_METHOD_CALLBACK				=0,		/*!< call-back method. */
	AUD_CAPTURE_METHOD_QUERY				=2,		/*!< Not implemented. */

	// call-back type
	AUD_CAPTURE_CALLBACK_ONCAPTURE			=0x10,	/*!< Internal use. */
};

//! Normal sampling data. Refer SDK manual for the detailed information.
#define AUD_SAMP_ATTR_NORMAL		0x0000
//! Internal buffer was overflowed. Refer SDK manual for the detailed information.
#define AUD_SAMP_ATTR_BUF_OVERLOW	0x0001
//! PCI error has occurred. Refer SDK manual for the detailed information.
#define AUD_SAMP_ATTR_PCI_ERROR		0x0002

/*!
The structure for the sampled data.  Refer SDK manual for the detailed information.
*/
typedef struct {
	BYTE*			buf;
	ULONG			len;
	FILETIME		timestamp;
	ULONG			attr;			// [1..0]	00: normal sample
									//			01: buffer overflow
									//			10: PCI error (re-start)
	ULONG			uBoardNum;
	ULONG			uChannelNum;
	ULONG			bHasNextData;
	ULONG			reserved[8];
} AUDIO_SAMPLE_INFO;

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// Callback Prototype
/*!
Callback function prototype with 1 parameter.
\see Aud3SetOnCallBacks
\param A The first parameter. In general, this is the \e pContext of the \ref Aud3SetOnCallBacks. Type cast before use.
*/
typedef BOOL (CALLING_API *AUD_FUNC_ON_1DWORD)(UDA_ULONG A);
/*!
Callback function prototype with 3 parameters.
\see Aud3SetOnCallBacks
\param A The first paramter.
\param B The second parameter.
\param C The third parameter. In general, this is the \e pContext of the \ref Aud3SetOnCallBacks. Type cast before use.
\remarks
In case of sample data callback function:\n
The first parameter indicates channel number. Bit[7..4] is the board ID, bit[3..0] is the channel ID.\n
The second parameter points to AUDIO_SAMPLE_INFO.
*/
typedef BOOL (CALLING_API *AUD_FUNC_ON_3DWORD)(UDA_ULONG A, UDA_ULONG B, UDA_ULONG C);

//////////////////////////////////////////////////////////////////////////////////
// API

DVRAUD_API BOOL		CALLING_API Aud3Control(DWORD cmd, void* inD, DWORD inSize, void* outD, DWORD outSize, DWORD* rsBytes);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3CheckActivationCode(DWORD bdid, UCHAR* pActivationCode)
\brief		Aud3CheckActivationCode activates the specified board.
\see		Aud3Setup()
\param		bdid Zero-based board number.
\param		pActivationCode 16-byte activation code.
\remarks	\ref You should activate all the installed board, before call Aud3Setup.
			\ref Use model ID and its activation code from the information retreived by Aud3GetSystemInfoEx.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_NONE
*/
DVRAUD_API BOOL		CALLING_API Aud3CheckActivationCode(DWORD bdid, UCHAR* pActivationCode);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3Setup(void)
\brief		Aud3Setup set up the driver.
\remarks	\ref Aud3Setup does following works.
			-# Checks whether the device driver were installed successfully.
			-# Creates all the resources such as events, memory.
			-# Checks whether all the installed board were activated.
			-# Gets ready to operate.

			\ref Aud3Setup does critical works for the driver operation. You have to check its return value.
			DLL thread, which calls sample data callback function, is created at this time.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == INIT
\post		Driver state changes to the AUD_STATE_SETUP.
*/
DVRAUD_API BOOL		CALLING_API	Aud3Setup(void);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3Endup(void)
\brief		Aud3Endup closes the operation of the board.
\remarks	
			Aud3Endup frees allocated resources and makes the board to stop the operation.
			All API function calling is invalid after Aud3Endup except \ref Aud3CheckActivationCode, \ref Aud3Setup\n
			DLL thread, which calls sample data callback function, terminates.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		This function can be called at all driver state, even at AUD_STATE_NONE.
\post		Driver state changes to AUD_STATE_NONE.
*/
DVRAUD_API BOOL 	CALLING_API Aud3Endup(void);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3Run(void)
\remarks	
			The driver enters the state where the sampling is possible. You cannot change sampling attributes except gain after 
			\ref Aud3Run is called. Internal buffer is cleared. Sampling starts only after you call \ref Aud3StartRecording.
			At this state, DLL thread, which calls callback function, will call callback function immediately after the driver captures 
			audio samples.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_SETUP
\post		Driver state changes to AUD_STATE_RUN.
*/
DVRAUD_API BOOL 	CALLING_API Aud3Run(void);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3Stop(void)
\remarks	
			Sampling stops. DLL thread won't call callback any more. Be careful not to cause dead-lock. Consult SDK manual for the dead-lock. 
			If Aud3Stop returns successfully, DLL thread won't call callback function even though there remains sampled data in the
			internal buffer.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_RUN
\post		Driver state changes to AUD_STATE_SETUP.
*/
DVRAUD_API BOOL 	CALLING_API Aud3Stop(void);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3GetSystemState(AUD_SYSTEM_STATE* pState)
\param		pState Pointer to \ref AUD_SYSTEM_STATE structure.
\remarks	
			Aud3GetSystemState gets current Driver state.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		This function can be called at all driver state.
*/
DVRAUD_API BOOL 	CALLING_API Aud3GetSystemState(AUD_SYSTEM_STATE* pState);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3SetOnCallBacks(DWORD funcType, AUD_FUNC_ON_1DWORD fStart, AUD_FUNC_ON_3DWORD fCall, AUD_FUNC_ON_1DWORD fStop, void *pContext)
\brief		Aud3SetOnCallBacks registers callback function.
\param		funcType Specify \ref AUD_CAPTURE_CALLBACK_ONCAPTURE.
\param		fStart Specify function same as the prototype \ref AUD_FUNC_ON_1DWORD. 
			If you call \ref Aud3Run, DLL thread changes its state from suspended state to the state calling sampling data.
			The registerd function is called just before state change. Specify NULL, if you don't want to register.
\param		fCall Specify function same as the prototype \ref AUD_FUNC_ON_3DWORD. This is main callback function.
			DLL delivers sampled data through this callback function. Specify NULL, if you don't want to register.
\param		fStop Specify function same as the prototype \ref AUD_FUNC_ON_1DWORD.
			If you call \ref Aud3Stop, DLL thread changes its state from the state calling sampling data to suspended state.
			The registerd function is called just before state change. Specify NULL, if you don't want to register.
\param		pContext Pointer to a caller-supplied context area to be passed to the callback function when it is called.
			Context will be passed as the first parameter in case of \ref AUD_FUNC_ON_1DWORD and the third parameter in case of
			\ref AUD_FUNC_ON_3DWORD.
\remarks	

			This registers callback functions specified at \e funcType. Calling of callback functions is synchronized to the operation of
			DLL thread. Following is the pseudo code how the DLL thread calls callback functions.\n
			\n
			...\n
			fStart();\n
			while(running) {\n
			...\n
			fCall();\n
			...\n
			}\n
			fStop();\n
			...\n
			\n
			For example, COM library should be intialized thread basis. In this case, the APP calls \e CoInitialize at \e fStart, and
			\e CoUninitialize at \e fStop.

\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API	Aud3SetOnCallBacks(DWORD funcType, AUD_FUNC_ON_1DWORD fStart, AUD_FUNC_ON_3DWORD fCall, AUD_FUNC_ON_1DWORD fStop, void *pContext);

// *********************************************************************** 
// Capture Method API   		                                              }
// *********************************************************************** 
DVRAUD_API BOOL		CALLING_API	Aud3SetCaptureMethod(DWORD method);

//use AUDIO_SAMPLE_INFO* pInfo
DVRAUD_API BOOL		CALLING_API Aud3QueryWaitAudio(void* pInfo);
DVRAUD_API BOOL		CALLING_API Aud3ReleaseAudio(void* pInfo);

/*!
\fn DVRAUD_API BOOL		CALLING_API	Aud3SetCaptureEnable(DWORD bdid, DWORD ch, BOOL bStart)
\brief		Aud3SetCaptureEnable starts or stops sampling.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		bStart TRUE for start, FALSE for stop.
\remarks	
			This function starts or stops sampling. Stopping sampling does not delete the data in the internal buffer. Thus callback function
			can be called just after you stop sampling.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_RUN
*/
DVRAUD_API BOOL		CALLING_API	Aud3SetCaptureEnable(DWORD bdid, DWORD ch, BOOL bStart);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3SetGain(DWORD bdid, DWORD ch, int gain)
\brief		Aud3SetGain adjusts audio input gain.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		gain Gain value from 0 to 15. The default gain is 3.
\remarks	
			Consult SDK manual for the real gain value.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state >= AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3SetGain(DWORD bdid, DWORD ch, int gain);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3SetBitsPerSample(DWORD bdid, DWORD ch, int bits)
\brief		Aud3SetBitsPerSample sets bits per sample.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		bits Bits per sample. Use 8 for 8-bit, 16 for 16-bit.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3SetBitsPerSample(DWORD bdid, DWORD ch, int bits);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3SetSampleFreq(DWORD bdid, DWORD ch, int freq)
\brief		Aud3SetSampleFreq sets sampling frequency.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		freq Sampling frequency in Hz.
\remarks	
			Supported sampling frequency varies according to the ADC type used. Consult SDK manual for the usable sampling frequency.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3SetSampleFreq(DWORD bdid, DWORD ch, int freq);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3GetGain(DWORD bdid, DWORD ch, int* pGain)
\brief		Aud3GetGain reads current gain.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		pGain Pointer to the storage for the gain.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state >= AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3GetGain(DWORD bdid, DWORD ch, int* pGain);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3GetBitsPerSample(DWORD bdid, DWORD ch, int* pBits)
\brief		Aud3GetBitsPerSample reads current bits per sample.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		pBits Pointer to the storage for the bits per sample.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state >= AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3GetBitsPerSample(DWORD bdid, DWORD ch, int* pBits);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3GetSampleFreq(DWORD bdid, DWORD ch, int* pFreq)
\brief		Aud3GetSampleFreq reads current sampling frequency.
\param		bdid Zero-based board number.
\param		ch Zero-based channel number.
\param		pFreq Pointer to the storage for the sampling frequency.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state >= AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3GetSampleFreq(DWORD bdid, DWORD ch, int* pFreq);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3SetCallbackInterval(DWORD numCallbacksPerSecond)
\brief		Aud3SetCallbackInterval sets how many times the driver calls callback function in one second.
\param		numCallbacksPerSecond Sets how many times the driver calls callback function in one second. Specify among 2, 4, 8.
\remarks	It is applies system wide. You cannot set this to each channel. The bigger the value, the higher the system load increases.
			But it helps to synchronize video and audio.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		Driver state == AUD_STATE_SETUP
*/
DVRAUD_API BOOL		CALLING_API Aud3SetCallbackInterval(DWORD numCallbacksPerSecond);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3GetErrorCode(ErrorCodeItem* pEcode)
\brief		Aud3GetErrorCode retreives error information when the error occurs.
\param		pEcode Pointer to the storage of the \ref ErrorCodeItem structure.
\remarks	
			There can be more than one error information even though one API function is called.
			The driver buffers all the error information happened. \ref Aud3GetErrorCode gets one error information at a time.
			\e ErrorCode, \e ERRCMN_S_NO_MORE_ERROR means there is no more error in the error information buffer.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero.
\pre		This function can be called at all driver state.
*/
DVRAUD_API BOOL		CALLING_API Aud3GetErrorCode(ErrorCodeItem* pEcode);

/*!
\fn DVRAUD_API BOOL		CALLING_API Aud3GetSystemInfoEx(AUD_SYSTEM_INFO_DESC *pDesc, void *pInfo)
\brief		Aud3GetSystemInfoEx gets audio boards information.
\param		pDesc Pointer to the storage of the \ref AUD_SYSTEM_INFO_DESC structure. This structure should be filled before the function
			is called.
\param		pInfo Pointer to the storage of the \ref AUD_SYSTEM_INFO structure.
\remarks	
			Refer structures such as AUD_SYSTEM_INFO, AUD_BOARD_INFO, AUD_VP_INFO. We defines these structures according to the 
			version in the header file. For example, AUD_SYSTEM_INFO_V3 for AUD_SYSTEM_INFO, AUD_BOARD_INFO_V3 for AUD_BOARD_INFO,
			AUD_VP_INFO_V3 for AUD_VP_INFO.
\return		If the function succeeds, the return value is nonzero.\n
			If the function fails, the return value is zero. To get error information, call \ref Aud3GetErrorCode.
\pre		This function can be called at all driver state.
*/
DVRAUD_API BOOL		CALLING_API Aud3GetSystemInfoEx(AUD_SYSTEM_INFO_DESC *pDesc, void *pInfo);

DVRAUD_API BOOL		CALLING_API Aud3WriteUserEEPROM(DWORD bdid, DWORD btid, UCHAR *pData);

DVRAUD_API BOOL		CALLING_API Aud3ReadUserEEPROM(DWORD bdid, DWORD btid, UCHAR *pData);

#endif // !defined(KERNEL_MODE_CODE)

#pragma pack( pop, Aud3BoardLib_h )

#endif // AUD3BOARDLIB_H