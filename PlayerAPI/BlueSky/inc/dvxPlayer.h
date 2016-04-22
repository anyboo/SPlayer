/** $Id$ $DateTime$
 *  @file  bsrPlayer.h
 *  @brief bsrPlayer模块的导出头文件
 *  @version 0.0.1
 *  @since 0.0.1
 *  @author SongZhanjun<songzhj@bstar.com.cn>
 *  @date 2009-07-02    Created it
 */
/******************************************************************************
*@note
    Copyright 2007, BeiJing Bluestar Corporation, Limited
                        ALL RIGHTS RESERVED
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products
to use or abstract this computer program for the sole purpose of implementing
a product based on BeiJing Bluestar, Inc. products. No other rights to
reproduce, use, or disseminate this computer program,whether in part or  in
whole, are granted. BeiJing Bluestar, Inc. makes no representation or
warranties with respect to the performance of this computer program, and
specifically disclaims any responsibility for any damages, special or
consequential, connected with the use of this program.
For details, see http://www.bstar.com.cn/
******************************************************************************/

#ifndef __BSR_PLAYER_H__
#define __BSR_PLAYER_H__


#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef BSRPLAYER_EXPORTS
#define DVXPLAYER_API EXTERNC __declspec(dllexport)	
#else	// else BSRPLAYER_EXPORTS
#define DVXPLAYER_API EXTERNC __declspec(dllimport)
#endif	// end of BSRPLAYER_EXPORTS

/// 播放器句柄
typedef void* PlayerHandle;

/// 叠加信息句柄
typedef void* InfoHandle;

/// 播放对象
enum
{
	PLAYER_SOURCE_REAL		= 0,
	PLAYER_SOURCE_RECORD	= 1,
	PLAYER_SOURCE_FILE		= 2,
  PLAYER_SOURCE_PICTURE_EVENT		= 3,
  PLAYER_SOURCE_PICTURE_CLIP		= 4
};

/// 播放模式, 按位使用
enum 
{
	PLAYER_MODE_NORMAL		= 0x00,
	PLAYER_MODE_PERFRAME	= 0x01,
	PLAYER_MODE_IFRAME		= 0x02,
};

/// 返回值定义
enum
{
	PLAYER_ERR_NOIMPL		= -1,
	PLAYER_OK						= 0,

	PLAYER_ERR_BASE			= 0x00030000,
	PLAYER_ERR_PARA			= (PLAYER_ERR_BASE + 1),
	PLAYER_ERR_SYSTEM		= (PLAYER_ERR_BASE + 2),
	PLAYER_ERR_INVALID_HANDLE	=	(PLAYER_ERR_BASE + 3),
	PLAYER_ERR_FAILED		= (PLAYER_ERR_BASE + 4),
	PLAYER_ERR_FRAMEHDR	= (PLAYER_ERR_BASE + 5),
	PLAYER_ERR_FRAMEDATA =(PLAYER_ERR_BASE + 5),
	PLAYER_ERR_NO_SPACE =	(PLAYER_ERR_BASE + 6),
	PLAYER_ERR_NOTPLAY	=	(PLAYER_ERR_BASE + 7),
};

enum
{
  DECDATA_VIDEO = 1, 
  DECDATA_AUDIO,
};

struct dvxDecFrameInfo
{
  unsigned int sequence;				//帧序号,各通道独立,音视频独立
  unsigned int type;					  //数据类型:1视频,2音频
  unsigned int frameRate;			//编码时产生的图像帧率(若为音频此参数无意义)
  unsigned int timestamp;				//时标信息,单位秒;
  unsigned int tick;          //时间信息，单位为us


  unsigned int audioLen;   //音频长度
  unsigned char *pAudio;   ////音频指针
  unsigned int  frameType;  //码率类型

  //视频帧信息
  unsigned int videoType;		//是否I帧(对解码后数据无意义，恒为零，暂不用)
  unsigned int width;				//画面宽,单位像素(若为音频此参数无意义)
  unsigned int height;			//画面高,单位像素(若为音频此参数无意义)

  unsigned int strideY;      //视频亮度跨度
  unsigned int strideU;      //视频色度跨度
  unsigned int strideV;      //

  unsigned char* pY;      //视频Y分量
  unsigned char* pU;      //视频U分量
  unsigned char* pV;      //视频V分量

};


//
typedef void (CALLBACK *DecDataRoutine)( dvxDecFrameInfo *pInfo, void* pUser );

DVXPLAYER_API int dvxPlayerCreate( PlayerHandle* phPlayer, int nPlaySource, int nBufferFrameScale );
DVXPLAYER_API int dvxPlayerDestory( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerInit( PlayerHandle hPlayer, HWND hPlayeWnd, int sizeVideoFifo, 
																unsigned int nChannels, unsigned int nSamplesPerSec, unsigned int nBitsPerSample, 
																void (*pfNotifyRoutine)( void*, int ), void* pRoutinePara );
DVXPLAYER_API int dvxPlayerReset( PlayerHandle hPlayer, bool bIsForward );
DVXPLAYER_API int dvxPlayerGetPosition( PlayerHandle hPlayer, __int64* ms );
DVXPLAYER_API int dvxPlayerInputFrame( PlayerHandle hPlayer, void* pBuff, int nSize );
DVXPLAYER_API int dvxPlayerPlay( PlayerHandle hPlayer, int nPlayMode );
DVXPLAYER_API int dvxGetPlayerCurPos( PlayerHandle hPlayer, unsigned int *nPlayerCurPos );
DVXPLAYER_API int dvxPlayerPause( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerStop( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerImageEnhance( PlayerHandle hPlayer, bool bImageEnhance );
DVXPLAYER_API int dvxPlayerSetRate( PlayerHandle hPlayer, int nRate, int nScale );
DVXPLAYER_API int dvxPlayerGetRate( PlayerHandle hPlayer, int* pnRate, int* pnScale );
DVXPLAYER_API int dvxPlayerSound( PlayerHandle hPlayer );
DVXPLAYER_API int dvxPlayerSetVolume( PlayerHandle hPlayer, int nVolume );
DVXPLAYER_API int dvxPlayerGetVolume( PlayerHandle hPlayer, int *pnVolume );
DVXPLAYER_API int dvxPlayerGetFrameNum( PlayerHandle hPlayer, int* pnNum );
DVXPLAYER_API int dvxPlayerSetVideoPlayMode( PlayerHandle hPlayer, int nFifoMode );

// 与Render相关的功能
DVXPLAYER_API void dvxPlayerSetDrawCallback( PlayerHandle hPlayer, 
  void (*pfDrawRoutine)( void*, HDC hDC, unsigned int nWidth, unsigned int nHeight ), void* pDrawPara );
DVXPLAYER_API void dvxPlayerSetDrawCallbackEx( PlayerHandle hPlayer, 
  void (*pfDrawRoutine)( void*, HDC hDC, unsigned int nWndWidth, unsigned int nWndHeight, unsigned int nVideoWidth, unsigned int nVideoHeight ), void* pDrawPara );
DVXPLAYER_API int dvxPlayerSavePicture( PlayerHandle hPlayer, const char* szFileName);
DVXPLAYER_API int dvxPlayerSavePictureEx( PlayerHandle hPlayer, const char* szFileName, int nImgType=0/*0 bitmap,1 jpg*/ );
DVXPLAYER_API int dvxPlayerRefresh( PlayerHandle hPlayer );
DVXPLAYER_API void dvxPlayerShowDateTime( PlayerHandle hPlayer, bool bShow );
DVXPLAYER_API void dvxPlayerSetDateTimeColor( PlayerHandle hPlayer, unsigned int rgbColor );
DVXPLAYER_API void dvxPlayerSetDateTimeCoordinate( PlayerHandle hPlayer, unsigned int nX, unsigned int nY );
DVXPLAYER_API int dvxPlayerAddExtraInfo( PlayerHandle hPlayer, InfoHandle* phInfo, const char* szText,
																				unsigned int rgbColor, unsigned int nX, unsigned int nY );
DVXPLAYER_API void dvxPlayerDelExtraInfo( PlayerHandle hPlayer, InfoHandle hInfo );
DVXPLAYER_API void dvxPlayerSetExtraInfoColor( PlayerHandle hPlayer, InfoHandle hInfo, unsigned int rgbColor );
DVXPLAYER_API void dvxPlayerSetExtraInfoCoordinate( PlayerHandle hPlayer, InfoHandle hInfo, unsigned int nX, unsigned int nY );
DVXPLAYER_API void dvxPlayerShowIAInfo( PlayerHandle hPlayer, BOOL bShow ); //设置是否显示智能信息
DVXPLAYER_API BOOL dvxPlayerIAInfoShow( PlayerHandle hPlayer );             //是否显示智能信息
DVXPLAYER_API bool dvxPlayerHasAiInfo(PlayerHandle hPlayer);
DVXPLAYER_API void dvxSetPlayerShowAiLang(PlayerHandle hPlayer, int nLanguage);//设置智能叠加信息的显示语言

DVXPLAYER_API void dvxPlayerSetDecCallback( PlayerHandle hPlayer, DecDataRoutine pCallback, void* pUser );
DVXPLAYER_API bool dvxPlayerSetTrapwirePoint(PlayerHandle hPlayer, char *pPoint);

//到指定的时间开始播放，只对远程回放和本地文件播放有效
DVXPLAYER_API bool dvxPlayerWaitForTime( PlayerHandle hPlayer, __int64 ms, int timeout );
DVXPLAYER_API int dvxPlayerSetFluence( PlayerHandle hPlayer,int frameChacNum );

//设置鸟瞰
DVXPLAYER_API BOOL dvxSetAirScapeRect( PlayerHandle hPlayer, RECT miniRect, RECT selRect, bool bOn );

//增加选择声道函数 0 = 左声道， 1 = 右声道
DVXPLAYER_API int  dvxPlayerSelectAudio( PlayerHandle hPlayer, unsigned char audioChnnl );

//获取当前所选声音通道
DVXPLAYER_API int dvxPlayerGetCurAudioChnnl( PlayerHandle hPlayer, unsigned char &audioChnnl );

//判断是否支持多声道
DVXPLAYER_API bool dvxPlayerSupportMultiAudio( PlayerHandle hPlayer );

//增加获取图像宽、高
DVXPLAYER_API bool dvxPlayerGetResolution( PlayerHandle hPlayer, int *pWidth, int *pHeight );

#endif	// end of __BSR_PLAYER_H__

