#ifndef  _PLAYERFACTORY_DLL_H_
#define  _PLAYERFACTORY_DLL_H_

#include "struct_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PLAYERFACTORY_EXPORTS
#define PLAYERFATORY_API __declspec(dllexport)
#else
#define PLAYERFATORY_API __declspec(dllimport)
typedef int BOOL;
typedef long LONG;
typedef unsigned long DWORD;
#define  CALLBACK1 __stdcall;
#endif


PLAYERFATORY_API BOOL InitPlayerFactory();
PLAYERFATORY_API BOOL ReleasePlayerFactory();

PLAYERFATORY_API BOOL  AnalyzeFileType(char *pFile, int *pfileType);//分析文件类型，pfileType为0表示通用，则调用通用的开源播放器，否则调用私有的
PLAYERFATORY_API BOOL IsSupportFastCut(int iFileType);//判断文件类型是否支持快速剪辑
PLAYERFATORY_API BOOL IsSupport22Renader(int iFileType);//判断文件类型是否支持设置YUV回调后，hwnd依然显示
PLAYERFATORY_API int  FindFFmpegFactoryID();//获取FFMPEG公开播放器ID

PLAYERFATORY_API BOOL GetPlayerInterface(LONG *nID);
PLAYERFATORY_API BOOL FreePlayerInterface(LONG nID);

PLAYERFATORY_API BOOL Player_OpenFile(LONG nID, char *pFile, HWND hwnd, int iFileType = 0);
//PLAYERFATORY_API BOOL Player_CloseFile(LONG nID);

PLAYERFATORY_API BOOL Player_Play(LONG nID);
PLAYERFATORY_API BOOL Player_Pause(LONG nID, DWORD nPause);
PLAYERFATORY_API BOOL Player_Stop(LONG nID);
PLAYERFATORY_API BOOL Player_Fast(LONG nID);
PLAYERFATORY_API BOOL Player_FastNoDelay(LONG nID);
PLAYERFATORY_API BOOL Player_Slow(LONG nID);
PLAYERFATORY_API BOOL Player_OneByOne(LONG nID);
PLAYERFATORY_API BOOL Player_OneByOneBack(LONG nID);
PLAYERFATORY_API BOOL Player_PlaySoundShare(LONG nID);
PLAYERFATORY_API BOOL Player_StopSoundShare(LONG nID);
PLAYERFATORY_API BOOL Player_SetVolume(LONG nID, DWORD nVolume);
PLAYERFATORY_API DWORD Player_GetVolume(LONG nID);

PLAYERFATORY_API BOOL   Player_SetPlayPos(LONG nID, float fRelativePos);
PLAYERFATORY_API float  Player_GetPlayPos(LONG nID);
PLAYERFATORY_API DWORD  Player_GetFileTime(LONG nID);
PLAYERFATORY_API DWORD  Player_GetPlayedTime(LONG nID);
PLAYERFATORY_API BOOL   Player_SetPlayedTimeEx(LONG nID, DWORD nTime);
PLAYERFATORY_API BOOL   Player_GetPictureSize(LONG nID, LONG *pWidth, LONG *pHeight);

PLAYERFATORY_API BOOL Player_CapturePic(LONG nID, char *pSaveFile, int iType);
PLAYERFATORY_API BOOL Player_SetFileEndCallback(LONG nID, void (__stdcall *FileEndCallback)(LONG nID, void *pUser), void *pUser);
PLAYERFATORY_API BOOL Player_SetDisPlayCallback(LONG nID, void(__stdcall *DisplayCallback)(DISPLAYCALLBCK_INFO *pstDisplayInfo), void *pUser);
PLAYERFATORY_API BOOL Player_GetSystemTime(LONG nID, unsigned long long* systemTime);

PLAYERFATORY_API BOOL Player_SetColor(LONG nID, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
PLAYERFATORY_API BOOL Player_GetColor(LONG nID, DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

PLAYERFATORY_API BOOL Player_FileCutStart(LONG nID, const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, BOOL bFast = 1);
PLAYERFATORY_API BOOL Player_FileCutClose(LONG nID);
PLAYERFATORY_API int Player_FileCutProcess(LONG nID);
PLAYERFATORY_API BOOL Player_FileCutPause(LONG nID, DWORD nPause);

PLAYERFATORY_API BOOL Player_FileConvertStart(LONG nID, const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTimeFileConvertStart, bool bConvert, T_ConverterParameters *pConvertPara);
PLAYERFATORY_API BOOL Player_FileConvertClose(LONG nID);
PLAYERFATORY_API int Player_FileConvertProcess(LONG nID);
PLAYERFATORY_API BOOL Player_FileConvertPause(LONG nID, DWORD nPause);

#ifdef __cplusplus
}
#endif






#endif