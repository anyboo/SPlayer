/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴力维软件有限公司。
*
* 文件名称：ZteScreen.h
* 文件标识： 
* 内容摘要： 8000解码相关接口定义
* 其它说明： 
* 当前版本： 
* 完成日期： 
*
* 修改记录1：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef _PLAY_SDK_H
#define _PLAY_SDK_H

#if defined( PLAY_SDK_EXPORTS)
	#define PLAY_API  extern "C"__declspec(dllexport) 
#else 
	#define PLAY_API  extern "C" __declspec(dllimport) 
#endif

enum KEY_FRAME_FIND_TYPE
{
	BY_FRAMENUM,	//按当前帧序号查找
	BY_FRAMTIME,	//按当前帧时间戳(相对时间)查找
};

typedef struct
{
	int	iFilePos;		//该帧在文件中的起始位
	int	iFrameNum;		//该帧帧序号
	__int64	iFrameTime;	//该帧时间撮(相对时间)
}FRAME_POS,*PFRAME_POS;

PLAY_API int Play_GetPort(int *nPort);
PLAY_API int Play_FreePort(int nPort);


PLAY_API int Play_OpenFile(int nPort,int hwnd,char *sFileName);
PLAY_API int Play_CloseFile(int nPort);
PLAY_API int Play_Play(int nPort);
PLAY_API int Play_Stop(int nPort);
PLAY_API int Play_Pause(int nPort);
PLAY_API int Play_Speed(int nPort,int nSpeed); //{-4,-2,1,2,4,8}
PLAY_API int Play_StopSound(int nPort);
PLAY_API int Play_PlaySound(int nPort);
PLAY_API unsigned int Play_GetFileTime(int nPort); //单位秒
PLAY_API unsigned int Play_GetPlayedTime(int nPort); //单位秒
PLAY_API unsigned int Play_GetPlayedPos(int nPort); //单位1-100%
PLAY_API int Play_SetPlayedTime(int nPort,unsigned int lSecond); //单位秒
PLAY_API int Play_SetPlayedPos(int nPort,unsigned int lpos); //单位1-100%
PLAY_API int Play_CapturePicture(int nPort,char *fileName);



//视频YUV数据
typedef void (* Play_VideoYUVCallBack)(int nPort, const char* pYUVBuf, int nLen, long nWidth,long nHeight,unsigned __int64 nStamp,long nType,long nReceived);

PLAY_API int Play_OpenStream(int nPort,int hwnd,char *pFileHeadBuf,unsigned long nSize);
PLAY_API int Play_InputData(int nPort,char * pBuf,unsigned long nSize);
/*	保存当前流/文件到本地ps、avi视频文件
	char *fileName(要保存的文件路径+文件名 如E:\Video.ps)
    int fileFmt(文件类型，0: PS流，1: AVI，）
    int mux（0: 混合，1: 只存视频，2: 只存音频）
    int videoType（默认 - 1 后面再扩展，转MPEG4或H265）
    int audioType（默认 - 1 后面再扩展，转PCM或AAC)
*/
PLAY_API int Play_startSaveFile(int nPort, char *fileName, int fileFmt, int mux, int videoType, int audioType);
//停止保存
PLAY_API void Play_stopSaveFile(int nPort);
PLAY_API int Play_CloseStream(int nPort);
PLAY_API int Play_SetYUVCallBack(int nPort, Play_VideoYUVCallBack fnHandler,long dwYUVCallbackParam);


//获取当前视频的长高 要求用户定位的区域要以视频的长高比例为准
PLAY_API int Play_GetVideoInfo(int nPort,int *pwidth,int *pheight);
// 设置右下角原始图像相对于窗口的显示比例,dRation(有效取值范围为0.25～0.5)
PLAY_API int Play_SetSrcImageRation(int nPort,const double dRation);
//使能定位
PLAY_API int Play_SetThreeDFlag(int nPort,bool bZoom);
//定位区域设置
PLAY_API int Play_SetThreeDRect(int nPort,int dstx,int dsty,int dstw,int dsth);

//要使用下列帧操作、时间操作接口，必须调用 openFileEx 打开文件
PLAY_API int Play_openFileEx(int nPort, int hWnd, const char * sFileName);
//设置当前播放时间点，精确到毫秒，相对时间
PLAY_API int Play_setPlayTimeEx(int nPort, __int64 iMilliSecond);
//获取当前播放时间点，精确到毫秒，相对时间
PLAY_API int Play_getPlayTimeEx(int nPort, __int64 *iMilliSecond);
//设置当前播放帧序号
PLAY_API int Play_setCurrentFrameNum(int nPort, int iFrameIndex);
//获取当前播放帧序号
PLAY_API int Play_getCurrentFrameNum(int nPort, int *iFrameIndex);
//获取文件总帧数
PLAY_API int Play_getFileTotalFrames(int nPort, int *iFrames);
//获取当前播放位置前第一个关键帧
PLAY_API int Play_getPreKeyFramePos(int nPort, __int64 iValue, KEY_FRAME_FIND_TYPE iType, PFRAME_POS pFramePos);
//获取当前播放位置后第一个关键帧
PLAY_API int Play_getNextKeyFramePos(int nPort, __int64 iValue, KEY_FRAME_FIND_TYPE iType, PFRAME_POS pFramePos);

//设置单帧播放
PLAY_API void Play_SetStepPlay(int nPort);
//文件播放结束回调接口
typedef void (*FileEndCallBack)(long lParam);
PLAY_API void Play_SetFileEndCallBack(int nPort,FileEndCallBack callback,long lPara);
//获取帧率
PLAY_API void Play_GetFrameRate(int nPort, long *lFrameRate);


#endif
