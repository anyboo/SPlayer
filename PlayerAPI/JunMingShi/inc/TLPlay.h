#ifndef _TL_PLAY_H_
#define _TL_PLAY_H_

#include <windows.h>

//错误码
#define TLPLAY_NO_ERROR				0		//无错误
#define TLPLAY_PARAM_INVALID		1		//错误的参数
#define TLPLAY_MODE_ERR				2		//模式设置错误
#define TLPLAY_FILE_OPENERR			3		//文件打开失败
#define TLPLAY_FILE_NOVIDEO			4		//文件没有视频
#define TLPLAY_NOT_OPENED			5		//未打开
#define TLPLAY_NOT_PLAYING			6		//未开始play
#define TLPLAY_DDRAW_INITERR		7		//DDRAW对象初始化失败
#define TLPLAY_VIDEODEC_INITERR		8		//视频解码器初始化失败
#define TLPLAY_OPENSOUND_ERR		9		//打开音频失败
#define TLPLAY_DECVIDEO_ERR			10		//解码视频失败
#define TLPLAY_DECAUDIO_ERR			11		//解码音频失败
#define TLPLAY_FILE_PLAYOVER		12		//文件已结束
#define TLPLAY_CREATEDIR_ERR		13		//创建路径失败
#define TLPLAY_CREATEFILE_ERR		14		//创建文件失败
#define TLPLAY_NO_ENOUGH_POOL		15		//缓冲剩余空间不足
#define TLPLAY_OUTOFMEMORY			16		//系统空间不足
#pragma pack( push, 4 )
typedef struct
{
    BYTE     m_byMediaType; //媒体类型
    BYTE	 reserved[8];		//保留字段
    DWORD    m_dwDataSize;  //实际编码数据大小
    BYTE     m_byFrameRate; //发送帧率,用于接收端
	DWORD    m_dwFrameID;   //帧标识，用于接收端
	DWORD    m_dwTimeStamp; //时间戳, 用于接收端
    union
    {
        struct{
			BOOL		m_bKeyFrame;    //频帧类型(I or P)
			WORD		m_wVideoWidth;  //视频帧宽
			WORD		m_wVideoHeight; //视频帧宽
			DWORD       m_wBitRate;
		}m_tVideoParam;
        BYTE    m_byAudioMode;//音频模式
    };
}PLAYFRAMEHDR,*LPPLAYFRAMEHDR;

#define  T_AUDIO_8_8K	1 //8K8bit 音频		
#define  T_AUDIO_16_8K	2 //8K16bit 音频	
#define  T_VIDEO_YV12	11 //YV12 图像	

typedef struct{ 
    long nWidth;  //画面宽，单位像素。如果是音频数据则为0； 
    long nHeight;   //画面高。如果是音频数据则为0； 
    long nStamp;  //时标信息，单位毫秒。 
    long nType;  //数据类型，T_AUDIO_8_8K，T_AUDIO_16_8K，T_VIDEO_YV12，详见宏定义说明。 
    long nFrameRate;//编码时产生的图像帧率。 
}DECFRAME_INFO; 
#pragma pack( pop )

#ifdef __cplusplus
extern "C"{
#endif

BOOL __stdcall TLPlay_GetPort(LONG* pnPort);
//获得新的播放句柄

BOOL __stdcall TLPlay_FreePort(LONG nPort);
//释放播放句柄

DWORD __stdcall TLPlay_GetLastError(LONG nPort);
//获得错误码


DWORD __stdcall TLPlay_GetSdkVersion(); 
// 说明：得到当前播放器sdk 的版本号和build 号。如果只是修改bug，我们只升级build 号。 
// 返回值：高 16 位表示当前的build 号。9~16 位表示主版本号，1~8 位表示次版本号。如：返回值0x08310001 
// 表示：build 号是0831，版本号 0.1。 

DWORD __stdcall TLPlay_GetDDrawDeviceTotalNums(); 
// 说明：获得系统中与windows 桌面绑定的总的显示设备数目（这里主要是指显卡）； 
// 返回值：如果返回 0，则表示系统中只有主显示设备。如果返回 1，则表示系统中安装了多块显卡， 
// 但只有一块显卡与Windows 桌面绑定。返回其他值，则表示系统中与桌面绑定的显卡数目。在多显卡 
//     的系统中可以通过设置显示属性，而指定任意一块显卡作为主显示设备。 

BOOL __stdcall TLPlay_SetDDrawDevice(LONG nPort, DWORD nDeviceNum); 
// 说明：设置播放窗口使用的显卡。 
// 注意：该窗口必须在该显卡所对应的监视器上才能显示播放画面。 
// 输入参数：nDeviceNum 显示设备的设备号，如果是0，则表示使用主显示设备。 


enum TLPLAYMODE{TL_PLAY_REALTIME_STREAM, TL_PLAY_POOL_STREAM, TL_PLAY_FILE};
BOOL __stdcall TLPlay_SetPlayMode(LONG nPort, enum TLPLAYMODE mode);
//设置播放模式，在Open之前调用
//TL_PLAY_REALTIME_STREAM 实时码流方式，一次输入一帧数据。 立即解码
//TL_PLAY_POOL_STREAM 缓冲流方式，不立即解码，通过缓冲拿数据
//TL_PLAY_FILE 文件方式，一次播放一个文件

BOOL __stdcall TLPlay_OpenFile(LONG nPort, const char* sFileName);
//打开文件 需先设置TL_PLAY_FILE方式


#define POOL_BUF_MAX	1024*100000
#define POOL_BUF_MIN	1024*65

BOOL __stdcall TLPlay_OpenStream(LONG nPort, DWORD dwPoolsize);
//打开码流 需先设置TL_PLAY_REALTIME_STREAM 或者 TL_PLAY_POOL_STREAM方式
// dwPoolsize 取值范围 POOL_BUF_MIN ～ POOL_BUF_MAX
//对于TL_PLAY_REALTIME_STREAM 模式，dwPoolsize参数忽略

BOOL __stdcall TLPlay_Close(LONG nPort);
//关闭

typedef void (CALLBACK*  pDecCBFun)(long nPort,unsigned char* pBuf,long nSize,DECFRAME_INFO* pFrameInfo,unsigned int dwContext);

BOOL __stdcall TLPlay_SetDecCallBack(LONG nPort, pDecCBFun DecCBFun, LONG nUser); 
// 说明：设置回调函数，获得解码数据, 在TLPlay_Play之前调用， TLPlay_Stop时会将回调清空
// 输入参数：DecCBFun 回调函数指针，不能为NULL; 
// 回调函数中的参数说明： 
// nPort 播放器通道号。 
// pBuf 解码后的音视频数据。 
// nSize 解码后的音视频数据pBuf 的长度； 
// pFrameInfo 图像和声音信息。
// nUser 用户上下文 



BOOL __stdcall TLPlay_Play(LONG nPort, HWND hWnd); 
//播放开始，播放视频画面大小将根据hWnd 窗口调整，要全屏显示，只要把hWnd 窗口放大到全屏。 
//如果已经播放，只是改变当前播放速度为正常速度。 
//输入参数：hWnd    播放视频的窗口句柄。

BOOL __stdcall TLPlay_Stop(LONG nPort);
//停止播放


BOOL __stdcall TLPlay_PlaySound(LONG nPort);
//打开音频播放
//说明：打开声音；同一时刻只能有一路声音。如果现在已经有声音打开，则自动关闭原来已经打开的声音。 
//注意：默认情况下声音是关闭的！

BOOL __stdcall TLPlay_StopSound();
//停止音频播放

BOOL __stdcall TLPlay_SetVolume(LONG nPort, WORD nVolume);
//设置音量  取值0-0xffff

WORD __stdcall TLPlay_GetVolume(LONG nPort);
//获得音量  取值0-0xffff


BOOL __stdcall TLPlay_GetBMP(LONG nPort, char* sBMPFileName); 
// 说明:  抓取bmp  图像 
// 参数:  nPort:  通道号，sBMPFileName 存放的BMP文件名,带路径。如果不带路径，缺省在c:\\SnapShotFile下

BOOL __stdcall TLPlay_GetJPG(LONG nPort, char* sJPGFileName, BYTE quality); 
// 说明:  抓取jpeg  图像 
// 参数:  nPort: 通道号，sJPGFileName 存放的JPG文件名,带路径。如果不带路径，缺省在c:\\SnapShotFile下
// quality图像质量 0-100 
typedef void(CALLBACK *pCBDrawFun)(long  nPort, HDC hDc, LONG nUser);
BOOL __stdcall TLPlay_RigisterDrawFun(LONG  nPort, pCBDrawFun drawfun, LONG nUser); 
// 说明：注册一个回调函数，获得当前表面的device context, 你可以在这个DC 上画图（或写字）
//输入参数：DrawFun    回调函数句柄。 
//          nUser 用户数据； 
// 回调函数说明：void CALLBACK DrawFun(long nPort,HDC hDc,LONG nUser)； 
// 		 nPort 通道号； 
// 		 hDc 设备DC。 
//       nUser  用户数据，就是上面输入的用户数据； 


BOOL __stdcall TLPlay_SetColor(LONG nPort, int nBrightness, int nContrast, int nSaturation, int nHue); 
// 说明：设置图象的视频参数，即时起作用； 
// 输入参数： 
// nBrightness: 亮度，默认 64； 范围0-128； 
// nContrast: 对比度，默认 64； 范围0-128； 
// nSaturation: 饱和度，默认 64； 范围0-128； 
// nHue:      色调，默认 64； 范围0-128； 
// 注意： 如果全部为默认值将不进行颜色调节； 
		   
BOOL __stdcall TLPlay_GetColor(LONG nPort, int *pBrightness, int *pContrast, int *pSaturation, int *pHue); 
// 说明：相应的获得颜色值，参数同上 

//////////////////////////////////////////////////////////////////////////
//只适用于TL_PLAY_POOL_STREAM模式接口
//////////////////////////////////////////////////////////////////////////
DWORD __stdcall TLPlay_GetSourceBufferRemain(LONG nPort);
//说明：获得流播放模式下源缓冲剩余数据； 
//返回值：当前源缓冲的大小（BYTE ）。

BOOL __stdcall TLPlay_ResetSourceBuffer(LONG nPort);
//说明：清除流播放模式下源缓冲区剩余数据 

DWORD __stdcall TLPlay_GetPlayedTime(LONG nPort); 
//说明：得到相对于第一帧的当前播放的时间戳，单位毫秒

BOOL __stdcall TLPlay_GetPictureSize(LONG nPort,LONG *pWidth,LONG *pHeight); 
//说明：获得码流中原始图像的大小

//////////////////////////////////////////////////////////////////////////
// 适用TL_PLAY_FILE 或者 TL_PLAY_POOL_STREAM方式 接口
//////////////////////////////////////////////////////////////////////////

BOOL __stdcall TLPlay_Pause(LONG nPort, BOOL bPause);
//说明：播放暂停/恢复： 
//输入参数：bPause=TRUE 暂停；否则恢复； 

BOOL __stdcall TLPlay_Fast(LONG nPort);
// 说明：快速播放，每次调用将使当前播放速度加快一倍，16倍上限；要恢复正常播放调用 
// TLPlay_Play(),从当前位置开始正常播放； 

BOOL __stdcall TLPlay_Slow(LONG nPort);
// 说明：慢速播放，每次调用将使当前播放速度慢一倍；1/16倍上限；要恢复正常播放调用 TLPlay_Play() 

BOOL __stdcall TLPlay_OneByOne(LONG nPort);
// 说明：单帧播放。要恢复正常播放调用TLPlay_Pause(nPort, FALSE); 


//////////////////////////////////////////////////////////////////////////
// 只适用文件模式 (TL_PLAY_FILE) 接口
//////////////////////////////////////////////////////////////////////////

DWORD __stdcall TLPlay_GetFileTime(LONG nPort); 
//说明：得到文件总的时间长度，单位毫秒，不支持对文件的数据追加； 

DWORD __stdcall TLPlay_GetFileTotalFrames(LONG nPort); 
// 说明：得到文件中的总帧数。 
// 返回值：文件中的总帧数。  

DWORD  __stdcall TLPlay_GetPlayedFrames(LONG nPort); 
//说明：得到已经解码的视频帧数； 

BOOL __stdcall TLPlay_SetPlayPos(LONG nPort,float fRelativePos);
// 设置播放位置百分比
// 输入参数：fRelativePos  范围0-100%；

float __stdcall TLPlay_GetPlayPos(LONG nPort);
// 获取播放位置百分比
// 返回值：范围0-100% 

BOOL __stdcall TLPlay_SetFileEndMsg(LONG nPort,HWND hWnd,UINT nMsg); 
// 说明：设置文件结束时要发送的消息； 
// 输入参数：hWnd    消息发送的窗口。 
// nMsg 用户自定义的输入的消息；当播放到文件结束时用户从hWnd 窗口过程中收到这个消息。 
// 此消息函数中的wParam 参数返回nPort 的值。 

//////////////////////////////////////////////////////////////////////////
// 适用码流模式 (TL_PLAY_REALTIME_STREAM 或者 TL_PLAY_POOL_STREAM方式) 接口
//////////////////////////////////////////////////////////////////////////

BOOL __stdcall TLPlay_InputVideoData(LONG nPort, PBYTE/*PPLAYFRAMEHDR*/ pBuf,DWORD nSize);
//输入视频编码数据，一次一帧。 pBuf为带PLAYFRAMEHDR头的编码数据，nSize =(sizeof(PLAYFRAMEHDR)+((LPPLAYFRAMEHDR)pBuf)->m_dwDataSize)

BOOL __stdcall TLPlay_InputAudioData(LONG nPort, PBYTE/*PPLAYFRAMEHDR*/ pBuf,DWORD nSize);
//输入音频编码数据，一次一帧。 pBuf为带PLAYFRAMEHDR头的编码数据，nSize =(sizeof(PLAYFRAMEHDR)+((LPPLAYFRAMEHDR)pBuf)->m_dwDataSize)
enum AUDIOTYPE{AT_ERR, AT_ADPB_8k16, AT_GRAW_8K8, AT_GRAW_8K16, AT_G711_8K8};

BOOL __stdcall EncAudioFrame(AUDIOTYPE type, unsigned char *pInputBuf, unsigned char *pOutputBuf, int souraudioframelen);//maxframelen为音频源长度

#ifdef __cplusplus
}
#endif

#endif


