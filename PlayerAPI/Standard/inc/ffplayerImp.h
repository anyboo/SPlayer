#ifndef  _FFPLAYERY_DLL_H_
#define  _FFPLAYER_DLL_H_

#include "../../../PlayerFactory/struct_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef FFPLAYER_EXPORTS
#define FFPLAYER_API __declspec(dllexport)
#else
#define FFPLAYER_API __declspec(dllimport)
#endif


FFPLAYER_API BOOL FFPlayer_Init();
FFPLAYER_API BOOL  FFPlayer_Release();

FFPLAYER_API BOOL FFPlayer_GetPort(LONG *nID);
FFPLAYER_API BOOL FFPlayer_FreePort(LONG nID);

FFPLAYER_API BOOL FFPlayer_OpenFile(LONG nID, char *pFile);

FFPLAYER_API BOOL FFPlayer_Play(LONG nID,HWND hwnd);
FFPLAYER_API BOOL FFPlayer_Pause(LONG nID, DWORD nPause);
FFPLAYER_API BOOL FFPlayer_Stop(LONG nID);
FFPLAYER_API BOOL FFPlayer_Fast(LONG nID);
FFPLAYER_API BOOL FFPlayer_FastNoDelay(LONG nID);
FFPLAYER_API BOOL FFPlayer_Slow(LONG nID);
FFPLAYER_API BOOL FFPlayer_OneByOne(LONG nID);
FFPLAYER_API BOOL FFPlayer_OneByOneBack(LONG nID);
FFPLAYER_API BOOL FFPlayer_PlaySoundShare(LONG nID);
FFPLAYER_API BOOL FFPlayer_StopSoundShare(LONG nID);
FFPLAYER_API BOOL FFPlayer_SetVolume(LONG nID, DWORD nVolume);
FFPLAYER_API DWORD FFPlayer_GetVolume(LONG nID);

FFPLAYER_API BOOL   FFPlayer_SetPlayPos(LONG nID, float fRelativePos);
FFPLAYER_API float  FFPlayer_GetPlayPos(LONG nID);
FFPLAYER_API DWORD FFPlayer_GetFileTime(LONG nID);
FFPLAYER_API DWORD  FFPlayer_GetPlayedTime(LONG nID);
FFPLAYER_API BOOL  FFPlayer_SetPlayedTime(LONG nID, DWORD nTime);
FFPLAYER_API BOOL  FFPlayer_GetPictureSize(LONG nID, LONG *pWidth, LONG *pHeight);

FFPLAYER_API BOOL FFPlayer_CapturePic(LONG nID, char *pSaveFile, int iType);
FFPLAYER_API BOOL FFPlayer_SetFileEndCallback(LONG nID, void(CALLBACK*FileEndCallback)(LONG nID, void *pUser), void *pUser);
FFPLAYER_API BOOL FFPlayer_SetDisplayCallback(LONG nID, void(CALLBACK*DisplayCallback)(DISPLAYCALLBCK_INFO*pInfo), void *pUser);


FFPLAYER_API BOOL FFPlayer_FileCutStart(LONG nID, const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime);
FFPLAYER_API BOOL FFPlayer_FileCutClose(LONG nID);
FFPLAYER_API int FFPlayer_FileCutProcess(LONG nID);
FFPLAYER_API int FFPlayer_FileCutPause(LONG nID);

FFPLAYER_API BOOL FFPlayer_FileConvertStart(LONG nID,const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara);
FFPLAYER_API BOOL FFPlayer_FileConvertClose(LONG nID);
FFPLAYER_API int FFPlayer_FileConvertProcess(LONG nID);
FFPLAYER_API BOOL FFPlayer_FileConvertPause(LONG nID,DWORD nPause);

#ifdef __cplusplus
}
#endif


#endif