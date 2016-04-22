////////////////////////////////////////////////////////////////////////////////
// 版权所有，2009-2012，北京汉邦高科数字技术有限公司。
////////////////////////////////////////////////////////////////////////////////
// 文件名： HBPlaySDK.h
// 作者：   HBGK
// 版本：   3.4
// 日期：   2012-08-09
// 描述：   汉邦高科解码库C/C++头文件
////////////////////////////////////////////////////////////////////////////////
#ifndef HBPLAYSDK_H
#define HBPLAYSDK_H

#ifdef __cplusplus
extern "C" {
#endif


//DLL导出
#define HBDLLAPI                    __declspec(dllexport)


////////////////////////////////////////////////////////////////////////////////
//定义HB_PLAY2 API
////////////////////////////////////////////////////////////////////////////////


//媒体播放对象
DECLARE_HANDLE(HHBPLAY2);

//错误代码
//可以使用SUCCESS和FAILED宏判断错误代码。
#define FACILITY_HBPLAY2            0xAB5
#define HBPLAY2_ERROR(code)         MAKE_HRESULT(1, FACILITY_HBPLAY2, code)

#define HBPLAY2_OK                  S_OK                    //成功
#define HBPLAY2_ERR_GENERIC         HBPLAY2_ERROR(0x1)      //一般错误
#define HBPLAY2_ERR_NOT_SUPPORTED   HBPLAY2_ERROR(0x2)      //不支持
#define HBPLAY2_ERR_INVALID_PARAMETER HBPLAY2_ERROR(0x10)   //无效参数
#define HBPLAY2_ERR_INVALID_HANDLE  HBPLAY2_ERROR(0x11)     //无效的句柄
#define HBPLAY2_ERR_INVALID_POINTER HBPLAY2_ERROR(0x12)     //无效的指针
#define HBPLAY2_ERR_INVALID_SIZE    HBPLAY2_ERROR(0x13)     //无效的结构体长度
#define HBPLAY2_ERR_INVALID_PIXEL_FORMAT HBPLAY2_ERROR(0x14)//无效的像素格式
#define HBPLAY2_ERR_BUFFER_TOO_SMALL HBPLAY2_ERROR(0x20)    //缓冲区太小
#define HBPLAY2_ERR_CANNOT_OPEN_FILE HBPLAY2_ERROR(0x21)    //不能打开文件
#define HBPLAY2_ERR_OUT_OF_MEMORY   HBPLAY2_ERROR(0x22)     //内存不足
#define HBPLAY2_ERR_BUFFER_EMPTY    HBPLAY2_ERROR(0X23)     //数据缓冲区空
#define HBPLAY2_ERR_BUFFER_FULL     HBPLAY2_ERROR(0X24)     //数据缓冲区满
#define HBPLAY2_ERR_BUSY            HBPLAY2_ERROR(0x25)     //忙，稍后再试
#define HBPLAY2_ERR_FILE_INDEX_INCOMPLETE HBPLAY2_ERROR(0x26)//文件索引尚未完成
#define HBPLAY2_ERR_NO_KEY_FRAME    HBPLAY2_ERROR(0x27)     //没有关键帧
#define HBPLAY2_ERR_DISK_FULL       HBPLAY2_ERROR(0x28)     //磁盘满

//打开文件
HBDLLAPI HRESULT WINAPI HB_PLAY2_OpenFileA( OUT HHBPLAY2* phPlay, IN LPCSTR lpszFileName );
HBDLLAPI HRESULT WINAPI HB_PLAY2_OpenFileW( OUT HHBPLAY2* phPlay, IN LPCWSTR lpszFileName );
#ifdef _UNICODE
#define HB_PLAY2_OpenFile HB_PLAY2_OpenFileW
#else
#define HB_PLAY2_OpenFile HB_PLAY2_OpenFileA
#endif

//文件头长度范围
#define HBPLAY2_HEADER_LENGTH_MIN      64           //文件头长度最小值
#define HBPLAY2_HEADER_LENGTH_MAX      (256*1024)   //文件头长度最大值

//打开流
HBDLLAPI HRESULT WINAPI HB_PLAY2_OpenStream( OUT HHBPLAY2* phPlay, IN LPCVOID pFileHeader, 
    IN DWORD dwHeaderLength );

//关闭媒体播放对象
HBDLLAPI HRESULT WINAPI HB_PLAY2_Close( IN HHBPLAY2 hPlay );

//设置/获取播放使能标志
//这些标志位不能完全独立设置。它们之间的依赖关系，参考标志位的注释。

//使能码流分析标志。
//该功能始终使能，不能被禁止。
#define HBPLAY2_ENABLE_STREAM_PARSE             0x00000000
#define HBPLAY2_ENABLE_NONE                     HBPLAY2_ENABLE_STREAM_PARSE

//使能视频解码器标志。
//使能该标志才能进行视频解码。
//禁止该标志时，同时也会禁止HBPLAY2_ENABLE_MULTITHREADING_VIDEO_CODEC、
//HBPLAY2_ENABLE_HARDWARE_VIDEO_CODEC、HBPLAY2_ENABLE_VIDEO_QUALITY_PRIORITY、
//HBPLAY2_ENABLE_VERIFY_CONTINUOUS_VIDEO和HBPLAY2_ENABLE_DISPLAY标志。
#define HBPLAY2_ENABLE_VIDEO_CODEC              0x00000001

//使能多线程视频解码标志。
//多线程解码能够充分利用多核CPU的资源，加快解码速度，适用于较少路视频同时解码。
//禁止该标志时，一个媒体播放对象只会使用一个线程进行视频解码，适用于较多路
//视频，例如大于16路，同时解码。
#define HBPLAY2_ENABLE_MULTITHREADING_VIDEO_CODEC 0x00000002

//使能硬件视频解码器标志。
//使能该标志时，可以自动探测可用的硬件解码器并优先使用硬件解码器。硬件解码能够
//充分利用显卡的硬件资源，节省CPU资源。当前支持的硬件解码器有：NVIDIA CUDA。
//1. NVIDIA CUDA。
//  使用NVIDIA CUDA解码器，必须同时满足以下条件：
//  [1] 视频编码算法是H.264 Baseline, Main, High Profile Level 4.1，
//      图像尺寸不超过1080P，码流大小不超过45mbps。
//  [2] 存在NVIDIA G8x, G9x, MCP79, MCP89, G98, GT2xx, GF1xx或更高版本GPU，GPU的
//      计算能力不低于v1.1，显示内存不少于128MB。
//  [3] 显卡驱动程序版本不低于v286.19，CUDA驱动程序的版本不低于v4.1。
#define HBPLAY2_ENABLE_HARDWARE_VIDEO_CODEC     0x00000004

//使能视频图像质量优先标志。
//使能该标志时，在视频解码时会优先保证图像的质量，但可能会导致较高的
//CPU使用率。
//若禁止该标志，在视频解码时会优先保证视频的流畅性，尽量维持较低的CPU使用率，
//但可能会降低图像的质量。
#define HBPLAY2_ENABLE_VIDEO_QUALITY_PRIORITY   0x00000008

//使能校验连续视频标志。
//使能该标志时，能够检查视频编码数据的连续性。当发现不连续的视频帧时，暂停解码，
//一直等到下一个关键帧再恢复解码。使能该标志，可以避免由于丢帧导致的视频图像的
//马赛克现象，但可能导致视频短暂停顿。
#define HBPLAY2_ENABLE_VERIFY_CONTINUOUS_VIDEO  0x00000010

//使能视频显示标志。
//使能该标志才能显示视频图像。
//只使能该标志而没有使能具体的图像显示标志时，会自动选择默认的图像显示方式。
//禁止该标志，同时也会禁止HBPLAY2_ENABLE_DRAWDIB、HBPLAY2_ENABLE_DIRECTDRAW_7和
//HBPLAY2_ENABLE_DIRECT3D_9标志。
#define HBPLAY2_ENABLE_DISPLAY                  0x00000100

//使能立即更新图像标志。
//该标志提供了不受显示器限制的图像刷新率，但可能导致图像的“撕裂”现象。
//禁止该标志时，视频图像显示帧率不能超过显示器的刷新率，会略微提高CPU使用率，
//适用于显示高质量的视频图像。
//该标志只对Direct3D 9的显示方式有效。
#define HBPLAY2_ENABLE_PRESENT_IMMEDIATE        0x00000200

//使能Direct3D 9显示图像。
//Windows Vista/2008/7及以后版本，默认使用Direct3D 9来显示视频。
//禁止该标志，同时也会禁止HBPLAY2_ENABLE_PRESENT_IMMEDIATE标志。
#define HBPLAY2_ENABLE_DIRECT3D_9               0x00000400

//使能DirectDraw 7显示图像。
//DirectDraw 7的显示效率较高，对显卡的要求较低，但不能避免图像的“撕裂”现象。
//Windows 2000/XP/2003，默认使用DirectDraw 7来显示视频。
#define HBPLAY2_ENABLE_DIRECTDRAW_7             0x00000800

//使能直接显示与设备无关位图。
//该标志提供很好的兼容性，适合在远程桌面连接，或没有安装显卡驱动时使用。
//但显示效率低，图像质量低。
#define HBPLAY2_ENABLE_DRAWDIB                  0x00010000

//使能音频解码器标志。
//使能该标志才能进行音频解码。
//禁止该标志，同时也会禁止HBPLAY2_ENABLE_SOUND标志。
#define HBPLAY2_ENABLE_AUDIO_CODEC              0x00001000

//使能音频播放标志。
//使能该标志才能进行音频播放。
//禁止该标志，同时也会禁止HBPLAY2_ENABLE_SOUND_PRIORITY标志。
#define HBPLAY2_ENABLE_SOUND                    0x00002000

//使能音频播放优先标志。
//一般情况下，以视频的时间来控制播放的速度。使能该标志后，优先使用音频的时间
//来控制播放的速度。
//该标志一般用于播放纯音频流。
#define HBPLAY2_ENABLE_SOUND_PRIORITY           0x00004000

//使能HB_PLAY2_GetDecodeFrame函数标志。
//使能该标志，同时会禁止HBPLAY2_ENABLE_DISPLAY和HBPLAY2_ENABLE_SOUND标志，并使能
//HBPLAY2_ENABLE_VIDEO_QUALITY_PRIORITY标志。
//只能通过HB_PLAY2_GetDecodeFrame函数获取解码后的音视频数据，并且不能执行任何
//播放动作。
//建议高级用户才使用该标志位。
//更多详细信息，参考HB_PLAY2_GetDecodeFrame函数。
#define HBPLAY2_ENABLE_GET_DECODE_FRAME         0x10000000

//使能默认设置。
#define HBPLAY2_ENABLE_DEFAULT      (HBPLAY2_ENABLE_STREAM_PARSE \
                                    | HBPLAY2_ENABLE_VIDEO_CODEC \
                                    | HBPLAY2_ENABLE_MULTITHREADING_VIDEO_CODEC \
                                    | HBPLAY2_ENABLE_VERIFY_CONTINUOUS_VIDEO \
                                    | HBPLAY2_ENABLE_DISPLAY \
                                    | HBPLAY2_ENABLE_AUDIO_CODEC \
                                    | HBPLAY2_ENABLE_SOUND)

HBDLLAPI HRESULT WINAPI HB_PLAY2_SetEnableFlag( IN HHBPLAY2 hPlay, IN DWORD dwEnableFlag );
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetEnableFlag( IN HHBPLAY2 hPlay, OUT DWORD* pdwEnableFlag );

//播放动作
HBDLLAPI HRESULT WINAPI HB_PLAY2_Stop( IN HHBPLAY2 hPlay );
HBDLLAPI HRESULT WINAPI HB_PLAY2_Pause( IN HHBPLAY2 hPlay );
HBDLLAPI HRESULT WINAPI HB_PLAY2_Play( IN HHBPLAY2 hPlay );
HBDLLAPI HRESULT WINAPI HB_PLAY2_PlayBackward( IN HHBPLAY2 hPlay );

//播放速度
#define HBPLAY2_SPEED_MIN           float(1.0/64.0) //最小播放速度，1/64X
#define HBPLAY2_SPEED_MAX           float(64.0)     //最大播放速度，64X
#define HBPLAY2_SPEED_1X            float(1.0)      //正常播放速度，1X
HBDLLAPI HRESULT WINAPI HB_PLAY2_SetSpeed( IN HHBPLAY2 hPlay, IN float fSpeed );
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetSpeed( IN HHBPLAY2 hPlay, OUT float* pfSpeed );

//音量
#define HBPLAY2_VOLUME_MIN          0x0             //音量最小值（静音）
#define HBPLAY2_VOLUME_MAX          0xFFFF          //音量最大值
HBDLLAPI HRESULT WINAPI HB_PLAY2_SetVolume( IN HHBPLAY2 hPlay, IN DWORD dwVolume );
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetVolume( IN HHBPLAY2 hPlay, OUT DWORD* pdwVolume );


//视频颜色
#define HBPLAY2_VIDEO_COLOR_MIN     0               //视频颜色最小值
#define HBPLAY2_VIDEO_COLOR_MAX     127             //视频颜色最大值
#define HBPLAY2_VIDEO_COLOR_DEFAULT 64              //视频颜色默认值
typedef struct HBPLAY2_COLOR_SPACE
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_COLOR_SPACE)。
    DWORD                           dwSize;

    //亮度，取值范围[HBPLAY2_VIDEO_COLOR_MIN, HBPLAY2_VIDEO_COLOR_MAX]，
    //默认值HBPLAY2_VIDEO_COLOR_DEFAULT。
    DWORD                           dwBrightness;

    //对比度，取值范围[HBPLAY2_VIDEO_COLOR_MIN, HBPLAY2_VIDEO_COLOR_MAX]，
    //默认值HBPLAY2_VIDEO_COLOR_DEFAULT。
    DWORD                           dwContrast;

    //饱和度，取值范围[HBPLAY2_VIDEO_COLOR_MIN, HBPLAY2_VIDEO_COLOR_MAX]，
    //默认值HBPLAY2_VIDEO_COLOR_DEFAULT。
    DWORD                           dwSaturation;

    //色调，取值范围[HBPLAY2_VIDEO_COLOR_MIN, HBPLAY2_VIDEO_COLOR_MAX]，
    //默认值HBPLAY2_VIDEO_COLOR_DEFAULT。
    DWORD                           dwHue;

    //保留。
    DWORD                           dwReserved;

} HBPLAY2_COLOR_SPACE, *PHBPLAY2_COLOR_SPACE;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_SetVideoColor( IN HHBPLAY2 hPlay, IN const HBPLAY2_COLOR_SPACE* pColorSpace );
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetVideoColor( IN HHBPLAY2 hPlay, OUT PHBPLAY2_COLOR_SPACE pColorSpace );

//CPU使用率
#define HBPLAY2_CPU_LIMIT_MIN       50              //最小CPU使用率限制，50%
#define HBPLAY2_CPU_LIMIT_MAX       100             //最大CPU使用率限制，100%
HBDLLAPI HRESULT WINAPI HB_PLAY2_SetCpuLimit( IN HHBPLAY2 hPlay, IN int nLimit );
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetCpuLimit( IN HHBPLAY2 hPlay, OUT int* pnLimit );

//播放模式
enum HBPLAY2_MODE
{
    HBPLAY2_MODE_UNKNOWN = 0,                       //未知的模式
    HBPLAY2_MODE_FILE,                              //文件模式
    HBPLAY2_MODE_STREAM,                            //流模式
    HBPLAY2_MODE_COUNT,                             //播放模式数量
};
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetMode( IN HHBPLAY2 hPlay, OUT HBPLAY2_MODE* pMode );

//播放状态
enum HBPLAY2_STATE
{
    //停止。
    HBPLAY2_STATE_STOPPED = 0,

    //暂停。
    HBPLAY2_STATE_PAUSED,

    //播放中。
    //由HB_PLAY2_IsPlayBackward表明现在的播放方向。
    HBPLAY2_STATE_PLAYING,

    //保留。
    HBPLAY2_STATE_REQUIRE_TO_PAUSE,

    //保留。
    HBPLAY2_STATE_REQUIRE_TO_SEEK,

    //保留。
    HBPLAY2_STATE_SEEKING,

    //播放状态总数。
    HBPLAY2_STATE_COUNT,
};
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetState( IN HHBPLAY2 hPlay, OUT HBPLAY2_STATE* pState );

//播放方向
HBDLLAPI HRESULT WINAPI HB_PLAY2_IsPlayBackward( IN HHBPLAY2 hPlay, OUT BOOL* pbBackward );

//文件索引完成
HBDLLAPI HRESULT WINAPI HB_PLAY2_IsFileIndexCompleted( IN HHBPLAY2 hPlay, OUT BOOL* pbIndexCompleted );

//文件播放结束
HBDLLAPI HRESULT WINAPI HB_PLAY2_IsFileEnded( IN HHBPLAY2 hPlay, OUT BOOL* pbFileEnded );

//视频图像尺寸
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetPictureSize( IN HHBPLAY2 hPlay, OUT int* pnWidth, OUT int* pnHeight );

//缓冲的音视频帧数
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetBufferedFrameCount( IN HHBPLAY2 hPlay, OUT DWORD* pdwBufferedFrameCount );

//文件总长度
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetFileSize( IN HHBPLAY2 hPlay, OUT LONGLONG* pllFileSize );

//播放数据长度
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetPlayDataSize( IN HHBPLAY2 hPlay, OUT LONGLONG* pllPlayDataSize );

//文件总时长
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetFileDuration( IN HHBPLAY2 hPlay, OUT DWORD* pdwFileDuration );

//播放的时长
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetPlayDuration( IN HHBPLAY2 hPlay, OUT DWORD* pdwPlayDuration );

//文件总视频帧数
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetFileVideoFrameCount( IN HHBPLAY2 hPlay, OUT DWORD* pdwFileVideoFrameCount );

//播放的视频帧序号
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetPlayVideoFrameIndex( IN HHBPLAY2 hPlay, OUT DWORD* pdwPlayVideoFrameIndex );

//视频关键帧距离间隔
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetKeyFrameDistance( IN HHBPLAY2 hPlay, OUT DWORD* pdwKeyFrameDistance );

//视频关键帧时间间隔
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetKeyFrameInterval( IN HHBPLAY2 hPlay, OUT DWORD* pdwKeyFrameInterval );

//视频帧率
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetVideoFrameRate( IN HHBPLAY2 hPlay, OUT float* pfVideoFrameRate );

//视频码流比特率
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetVideoBitrate( IN HHBPLAY2 hPlay, OUT float* pfVideoBitrate );

//音频帧率
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetAudioFrameRate( IN HHBPLAY2 hPlay, OUT float* pfAudioFrameRate );

//音频码流比特率
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetAudioBitrate( IN HHBPLAY2 hPlay, OUT float* pfAudioBiterate );

//码流分析器特性
enum HBPLAY2_DEMUX_STREAM_VERSION
{
    HBPLAY2_DEMUX_STREAM_UNKNOWN = 0,               //未知流类型
    HBPLAY2_DEMUX_STREAM_15      = 1,               //早期码流，15码流
    HBPLAY2_DEMUX_STREAM_6000    = 2,               //早期码流，6000码流
    HBPLAY2_DEMUX_STREAM_V10     = 10,              //第一代码流
    HBPLAY2_DEMUX_STREAM_V20     = 20,              //第二代码流
    HBPLAY2_DEMUX_STREAM_V30     = 30,              //第三代码流
};
typedef struct HBPLAY2_DEMUX_PROPERTY
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_DEMUX_PROPERTY)。
    DWORD                           dwSize;

    //码流版本。
    HBPLAY2_DEMUX_STREAM_VERSION    StreamVersion;

    //保留。
    DWORD                           dwReserved[2];

    //文件头。
    //注意： 绝大多数码流都有文件头，但HBPLAY2_DEMUX_STREAM_6000码流没有文件头。
    char                            szFileHeader[64];

} HBPLAY2_DEMUX_PROPERTY, *PHBPLAY2_DEMUX_PROPERTY;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetDemuxProperty( IN HHBPLAY2 hPlay, OUT PHBPLAY2_DEMUX_PROPERTY pDemuxProperty );

//码流分析器性能
typedef struct HBPLAY2_DEMUX_STATUS
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_DEMUX_STATUS)。
    DWORD                           dwSize;

    //音频帧数。
    DWORD                           dwAudioFrameCount;

    //视频I帧数。
    DWORD                           dwVideoIFrameCount;

    //视频P帧数。
    DWORD                           dwVideoPFrameCount;

    //视频B帧数。
    DWORD                           dwVideoBFrameCount;

    //视频E帧数。
    DWORD                           dwVideoEFrameCount;

    //码流总时间，以毫秒（ms）为单位。
    DWORD                           dwStreamDuration;

    //保留。
    DWORD                           dwReserved0;

    //分析码流总时间，以微秒（us）为单位。
    LONGLONG                        llParseDuration;

    //保留。
    DWORD                           dwReserved1[2];

} HBPLAY2_DEMUX_STATUS, *PHBPLAY2_DEMUX_STATUS;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetDemuxStatus( IN HHBPLAY2 hPlay, OUT PHBPLAY2_DEMUX_STATUS pDemuxStatus );

//视频解码器性能
enum HBPLAY2_VDEC_CODEC_TYPE
{
    HBPLAY2_VDEC_CODEC_UNKNOWN = 0,                 //未知解码器类型
    HBPLAY2_VDEC_CODEC_MPEG4,                       //MPEG4解码器
    HBPLAY2_VDEC_CODEC_MPEG4_ISO,                   //MPEG4 ISO解码器
    HBPLAY2_VDEC_CODEC_FFMPEG,                      //FFMPEG H.264解码器
    HBPLAY2_VDEC_CODEC_HISILICON,                   //Hisilicon H.264解码器
    HBPLAY2_VDEC_CODEC_NVIDIA_CUDA,                 //NVIDIA CUDA H.264解码器
    HBPLAY2_VDEC_CODEC_JPEG,                        //JPEG解码器
    HBPLAY2_VDEC_CODEC_AMD_OVD,                     //AMD_OVD H.264解码器
    HBPLAY2_VDEC_CODEC_COUNT,                       //视频解码器数量
};
typedef struct HBPLAY2_VDEC_STATUS
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_VDEC_STATUS)。
    DWORD                           dwSize;

    //视频解码的总帧数。
    DWORD                           dwFrameCount;

    //视频解码的总时间，以微秒（us）为单位。
    LONGLONG                        llDecodeDuration;

    //实时解码速度，以帧/秒为单位。
    DWORD                           dwRealtimeSpeed;

    //解码线程数量。
    //若dwThreadCount == 0，表示当前没有使用CPU解码，而是使用GPU解码。
    DWORD                           dwThreadCount;

    //视频解码器类型。
    HBPLAY2_VDEC_CODEC_TYPE         CodecType;

    //保留。
    DWORD                           dwReserved;

} HBPLAY2_VDEC_STATUS, *PHBPLAY2_VDEC_STATUS;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetVdecStatus( IN HHBPLAY2 hPlay, OUT PHBPLAY2_VDEC_STATUS pVdecStatus );

//视频显示性能
enum HBPLAY2_DISPLAY_TYPE
{
    HBPLAY2_DISPLAY_NONE         = 0,               //未使用特定的显示方式
    HBPLAY2_DISPLAY_DRAWDIB      = 1,               //直接显示与设备无关位图
    HBPLAY2_DISPLAY_DIRECTDRAW_7 = 2,               //通过DirectDraw 7显示图像
    HBPLAY2_DISPLAY_DIRECT_3D_9  = 4,               //通过Direct3D 9显示图像
    HBPLAY2_DISPLAY_DIRECT_3D_11 = 8,               //通过Direct3D 11显示图像
};
typedef struct HBPLAY2_DISPLAY_STATUS
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_DISPLAY_STATUS)。
    DWORD                           dwSize;

    //当前的显示方式
    HBPLAY2_DISPLAY_TYPE            DisplayType;

    //显示的帧数。
    DWORD                           dwFrameCount;

    //抓图的帧数。
    DWORD                           dwSnapshotCount;

    //计划显示总时间，以微秒（us）为单位。
    LONGLONG                        llPlannedDuration;

    //实际显示总时间，以微秒（us）为单位。
    LONGLONG                        llActualDuration;

    //抓图总时间，以微秒（us）为单位。
    LONGLONG                        llSnapshotDuration;

    //保留。
    DWORD                           dwReserved[2];

} HBPLAY2_DISPLAY_STATUS, *PHBPLAY2_DISPLAY_STATUS;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetDisplayStatus( IN HHBPLAY2 hPlay, OUT PHBPLAY2_DISPLAY_STATUS pDisplayStatus );

//音频解码器性能
enum HBPLAY2_ADEC_CODEC_TYPE
{
    HBPLAY2_ADEC_CODEC_UNKNOWN = 0,                 //未知解码器类型
    HBPLAY2_ADEC_CODEC_PCM_16_BIT,                  //未压缩的16位PCM
    HBPLAY2_ADEC_CODEC_PCM_LINEAR_8_BIT,            //8位线性PCM，需要扩展到16位
    HBPLAY2_ADEC_CODEC_G711_ALAW,                   //G.711 A律
    HBPLAY2_ADEC_CODEC_G722,                        //G.722
    HBPLAY2_ADEC_CODEC_G726,                        //Hisilicon G.726 16kbps
    HBPLAY2_ADEC_CODEC_COUNT,                       //音频解码器数量
};
typedef struct HBPLAY2_ADEC_STATUS
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_ADEC_STATUS)。
    DWORD                           dwSize;

    //音频解码的总帧数。
    DWORD                           dwFrameCount;

    //音频解码的总时间，以微秒（us）为单位。
    LONGLONG                        llDecodeDuration;

    //音频解码器类型。
    HBPLAY2_ADEC_CODEC_TYPE         CodecType;

    //保留。
    DWORD                           dwReserved;

} HBPLAY2_ADEC_STATUS, *PHBPLAY2_ADEC_STATUS;  
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetAdecStatus( IN HHBPLAY2 hPlay, OUT PHBPLAY2_ADEC_STATUS pAdecStatus );

//音频播放性能
typedef struct HBPLAY2_SOUND_STATUS
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员不小于sizeof(HBPLAY2_SOUND_STATUS)。
    DWORD                           dwSize;

    //播放标志。
    //标志为FLASE时，停止播放声音。否则，正在播放声音。
    BOOL                            bPlaying;

    //缓冲数据的长度，以字节为单位。
    DWORD                           dwBufferedDataLength;

    //缓冲数据的时间，以毫秒为单位。
    DWORD                           dwBufferedTime;

    //保留。
    DWORD                           dwReserved[2];

} HBPLAY2_SOUND_STATUS, *PHBPLAY2_SOUND_STATUS; 
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetSoundStatus( IN HHBPLAY2 hPlay, OUT PHBPLAY2_SOUND_STATUS pSoundStatus );

//增加或修改视口
typedef struct HBPLAY2_VIEWPORT
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_VIEWPORT)。
    DWORD                           dwSize;

    //视口标识符。
    //由调用者指定的唯一标志，用于区分多个视口。
    DWORD                           dwID;

    //显示窗口句柄。
    //必须是有效的窗口句柄。
    HWND                            hDestWnd;

    //保留。
    DWORD                           dwReserved[3];

    //原始数据矩形，该矩形不能大于原始图像的尺寸。
    //当矩形为空时（即通过IsRectEmpty函数判断返回值是TRUE），显示整个原始图像。
    //当矩形为非空时，显示指定部分的图像。一般用于局部放大显示。
    RECT                            rcSrc;

    //窗口中显示位置。
    //当矩形为空时（即通过IsRectEmpty函数判断返回值是TRUE），图像显示在窗口的整个客户区。
    //当矩形为非空时，图像显示在窗口中指定的位置。一般用于在一个窗口上显示多个图像。
    RECT                            rcDest;

} HBPLAY2_VIEWPORT, *PHBPLAY2_VIEWPORT;   
HBDLLAPI HRESULT WINAPI HB_PLAY2_AddViewport( IN HHBPLAY2 hPlay, IN const HBPLAY2_VIEWPORT* pViewport );

//移除视口
HBDLLAPI HRESULT WINAPI HB_PLAY2_RemoveViewport( IN HHBPLAY2 hPlay, IN DWORD dwViewportID );

//获取视口
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetViewport( IN HHBPLAY2 hPlay, 
    OUT OPTIONAL PHBPLAY2_VIEWPORT pViewportArray, IN OUT DWORD* pViewportCount );

//抓图
typedef struct HBPLAY2_SNAP_PICTURE
{
    //结构体长度，以字节（byte）为单位。
    //调用者必须设置该成员等于sizeof(HBPLAY2_SNAP_PICTURE)。
    DWORD                           dwSize;

    //DIB数据缓冲区长度，以字节为单位。
    DWORD                           dwBitsLength;

    //DIB数据缓冲区指针。
    //根据图像的尺寸，先分配足够大的内存。
    PVOID                           pBits;

    //保留。
    DWORD                           dwReserved;

    //返回DIB尺寸和颜色信息。
    BITMAPINFOHEADER                bmiHeader;

} HBPLAY2_SNAP_PICTURE, *PHBPLAY2_SNAP_PICTURE;
#define HBPLAY2_JPEG_QUALITY_MIN    1               //JPEG图像质量最小值
#define HBPLAY2_JPEG_QUALITY_MAX    100             //JPEG图像质量最大值
HBDLLAPI HRESULT WINAPI HB_PLAY2_SnapshotToMemory( IN HHBPLAY2 hPlay, IN OUT PHBPLAY2_SNAP_PICTURE pSnapPicture );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SnapshotToBmpFileA( IN HHBPLAY2 hPlay, IN LPCSTR lpszFileName );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SnapshotToBmpFileW( IN HHBPLAY2 hPlay, IN LPCWSTR lpszFileName );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SnapshotToJpegFileA( IN HHBPLAY2 hPlay, IN LPCSTR lpszFileName, IN int nQuality );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SnapshotToJpegFileW( IN HHBPLAY2 hPlay, IN LPCWSTR lpszFileName, IN int nQuality );
#ifdef _UNICODE
#define HB_PLAY2_SnapshotToBmpFile HB_PLAY2_SnapshotToBmpFileW
#define HB_PLAY2_SnapshotToJpegFile HB_PLAY2_SnapshotToJpegFileW
#else
#define HB_PLAY2_SnapshotToBmpFile HB_PLAY2_SnapshotToBmpFileA
#define HB_PLAY2_SnapshotToJpegFile HB_PLAY2_SnapshotToJpegFileA
#endif

//输入流数据
HBDLLAPI HRESULT WINAPI HB_PLAY2_InputData( IN HHBPLAY2 hPlay, IN LPCVOID pBuffer, IN DWORD dwBufferLength );

//像素格式
//在 http://www.fourcc.org/yuv.php 查看FOURCC定义和YUV的内存排列。
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                  \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#define HBPLAY2_PIXEL_UNCHANGE      0                               //不改变像素格式
#define HBPLAY2_PIXEL_RGB24         2                               //打包24位，RGBRGB...
#define HBPLAY2_PIXEL_BGR24         3                               //打包24位（最常用的RGB格式），BGRBGR...
#define HBPLAY2_PIXEL_UYVY          MAKEFOURCC('U', 'Y', 'V', 'Y')  //打包16位，同Y422、UYNV、HDYC
#define HBPLAY2_PIXEL_YUY2          MAKEFOURCC('Y', 'U', 'Y', '2')  //打包16位，同YUNV、V422、YUYV
#define HBPLAY2_PIXEL_YV12          MAKEFOURCC('Y', 'V', '1', '2')  //平面12位
#define HBPLAY2_PIXEL_I420          MAKEFOURCC('I', '4', '2', '0')  //平面12位，同IYUV
#define HBPLAY2_PIXEL_NV12          MAKEFOURCC('N', 'V', '1', '2')  //平面12位
#define HBPLAY2_PIXEL_NV21          MAKEFOURCC('N', 'V', '2', '1')  //平面12位

//帧类型
enum HBPLAY2_FRAME_TYPE
{
    HBPLAY2_FRAME_UNKNOWN,                          //未知帧类型
    HBPLAY2_FRAME_AUDIO,                            //音频帧
    HBPLAY2_FRAME_VIDEO_I,                          //视频I帧
    HBPLAY2_FRAME_VIDEO_P,                          //视频P帧
    HBPLAY2_FRAME_VIDEO_B,                          //视频B帧
    HBPLAY2_FRAME_VIDEO_E,                          //视频E帧
    HBPLAY2_FRAME_COUNT,                            //帧类型数量
};

//判断帧类型
#define HB_PLAY2_IsAudioFrame(FrameType) (HBPLAY2_FRAME_AUDIO == (FrameType))
#define HB_PLAY2_IsVideoFrame(FrameType) ((FrameType) >= HBPLAY2_FRAME_VIDEO_I\
                                    && (FrameType) <= HBPLAY2_FRAME_VIDEO_E)

//帧信息
typedef struct HBPLAY2_FRAME
{
    //保留
    DWORD                           dwReserved0;

    //保留
    DWORD                           dwReserved1[2];

    //帧类型
    //帧类型为HBPLAY2_FRAME_AUDIO时，u.Audio有效。
    //帧类型为HBPLAY2_FRAME_VIDEO_*时，u.Video有效。
    HBPLAY2_FRAME_TYPE              FrameType;

    union FRAME
    {
        struct FRAME_AUDIO
        {
            //帧头长度，以字节为单位。
            DWORD                   dwHeaderSize;

            //帧头地址。
            PVOID                   pHeader;

            //编码数据长度，以字节为单位。
            DWORD                   dwDataSize;

            //编码数据地址。
            PVOID                   pData;

            //音频通道数。
            WORD                    wChannels;

            //采样精度。
            WORD                    wBitsPerSample; 

            //采样率，以赫兹（Hz）为单位。
            DWORD                   dwSampleRate; 

            //解码缓冲区。
            PVOID                   pBuffer;

            //解码缓冲区的长度，以字节为单位。
            //输入时，说明缓冲区的总长度。
            //输出时，说明缓冲区的实际使用长度。
            DWORD                   dwBufferLength;

            //保留。
            DWORD                   dwReserved[4];

        } Audio;

        struct FRAME_VIDEO
        {
            //帧头长度，以字节为单位。
            DWORD                   dwHeaderSize;

            //帧头地址。
            PVOID                   pHeader;

            //编码数据长度，以字节为单位。
            DWORD                   dwDataSize;

            //编码数据地址。
            PVOID                   pData;

            //图像宽度，以像素为单位。
            int                     nWidth;

            //图像高度，以像素为单位。
            int                     nHeight;

            //距文件起始地址的偏移量，用于文件索引。
            LONGLONG                llOffset;

            //视频帧绝对时间。
            SYSTEMTIME              TimeStamp;

            //帧序号。
            //帧序号是逐渐递增的，用于判断视频帧的连续性。
            DWORD                   dwIndex;

            //视频帧相对时间，以毫秒为单位。
            //相邻两帧之间的差值，就是这两帧之间的时间间隔。
            DWORD                   dwTickCount;

            //实际输出的像素格式，由视频解码器设置该格式（HBPLAY2_PIXEL_*宏）。
            DWORD                   dwPixelFormat;

            //解码缓冲区。
            PVOID                   pBuffer;

            //解码缓冲区的长度，以字节为单位。
            DWORD                   dwBufferLength;

            //保留。
            DWORD                   dwReserved[3];

        } Video;
    } u;

} HBPLAY2_FRAME, *PHBPLAY2_FRAME;

//获取/释放解码数据
HBDLLAPI HRESULT WINAPI HB_PLAY2_GetDecodeFrame( IN HHBPLAY2 hPlay, IN DWORD dwPixelFormat,
    OUT PHBPLAY2_FRAME pFrame );
HBDLLAPI HRESULT WINAPI HB_PLAY2_FreeDecodeFrame( IN HHBPLAY2 hPlay, IN OUT PHBPLAY2_FRAME pFrame );

//设置文件的播放位置
HBDLLAPI HRESULT WINAPI HB_PLAY2_SeekByRatio( IN HHBPLAY2 hPlay, IN float fRatio );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SeekByTime( IN HHBPLAY2 hPlay, IN DWORD dwTime );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SeekByIndex( IN HHBPLAY2 hPlay, IN DWORD dwIndex );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SeekNextIndex( IN HHBPLAY2 hPlay );
HBDLLAPI HRESULT WINAPI HB_PLAY2_SeekPreviousIndex( IN HHBPLAY2 hPlay );

//注册文件索引完成回调函数
typedef void (CALLBACK* PHB_PLAY2_FILE_INDEX_COMPLETED_PROC)(
    IN HHBPLAY2 hPlay, IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterFileIndexCompletedCallback( IN HHBPLAY2 hPlay, 
    IN PHB_PLAY2_FILE_INDEX_COMPLETED_PROC pfnCallback, IN PVOID pContext );

//注册文件结束回调函数
typedef void (CALLBACK* PHB_PLAY2_FILE_ENDED_PROC)(
    IN HHBPLAY2 hPlay, IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterFileEndedCallback( IN HHBPLAY2 hPlay, 
    IN PHB_PLAY2_FILE_ENDED_PROC pfnCallback, IN PVOID pContext );

//注册缓冲区接近空回调函数
#define HBPLAY2_BUFFER_EMPTY_THRESHOLD_MIN  0
#define HBPLAY2_BUFFER_EMPTY_THRESHOLD_MAX  150
typedef void (CALLBACK* PHB_PLAY2_BUFFER_ALMOST_EMPTY_PROC)( 
    IN HHBPLAY2 hPlay, IN DWORD dwBufferedFrameCount, IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterBufferAlmostEmptyCallback( IN HHBPLAY2 hPlay, 
    IN DWORD dwThreshold, IN PHB_PLAY2_BUFFER_ALMOST_EMPTY_PROC pfnCallback, 
    IN PVOID pContext );

//注册码流分析回调函数
typedef void (CALLBACK* PHB_PLAY2_STREAM_PARSE_PROC)( 
    IN HHBPLAY2 hPlay, IN const HBPLAY2_FRAME* pFrame, IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterStreamParseCallback( IN HHBPLAY2 hPlay, 
    IN PHB_PLAY2_STREAM_PARSE_PROC pfnCallback, IN PVOID pContext );

//注册解码回调函数
typedef void (CALLBACK* PHB_PLAY2_DECODE_PROC)( 
    IN HHBPLAY2 hPlay, IN const HBPLAY2_FRAME* pFrame, IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterDecodeCallback( IN HHBPLAY2 hPlay, 
    IN DWORD dwPixelFormat, IN PHB_PLAY2_DECODE_PROC pfnCallback, IN PVOID pContext );

//注册DC绘图回调函数
typedef void (CALLBACK* PHB_PLAY2_DC_RENDER_PROC)( 
    IN HHBPLAY2 hPlay, IN HDC hDC, IN const HBPLAY2_VIEWPORT* pViewport,
    IN PVOID pContext );
HBDLLAPI HRESULT WINAPI HB_PLAY2_RegisterDcRenderCallback( IN HHBPLAY2 hPlay, 
    IN PHB_PLAY2_DC_RENDER_PROC pfnCallback, IN PVOID pContext );

//合并录像文件
HBDLLAPI HRESULT WINAPI HB_PLAY2_MergeFileA( IN LPCSTR lpszDestFile,
    IN LPCSTR* lpSrcFileArray, IN DWORD dwSrcFileCount );
HBDLLAPI HRESULT WINAPI HB_PLAY2_MergeFileW( IN LPCWSTR lpszDestFile,
    IN LPCWSTR* lpSrcFileArray, IN DWORD dwSrcFileCount );
#ifdef _UNICODE
#define HB_PLAY2_MergeFile HB_PLAY2_MergeFileW
#else
#define HB_PLAY2_MergeFile HB_PLAY2_MergeFileA
#endif


////////////////////////////////////////////////////////////////////////////////
//结束HB_PLAY2 API
////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////
//Define HB_PLAY API
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Macro definition.
////////////////////////////////////////////////////////////////////////////////


// the message sent when the index has been created
#define MSG_INDEX_OK            (WM_USER+154)

// the maximum number of regions for display in a channel.
#define MAX_DISPLAY_WND         4 

// frame type
#define UNKOWN_FRAME_TYPE       0x0000      // unknown frame type
#define VIDEO_FRAME_I           0x2000      // video I-frame
#define VIDEO_FRAME_P           0x2001      // video P-frame
#define VIDEO_FRAME_B           0x2002      // video B-frame
#define VIDEO_FRAME_E           0x2003      // video E-frame
#define AUDIO_FRAME_ALAW        0x1000      // audio A-law
#define AUDIO_FRAME_G722        0x1001      // audio G722
#define AUDIO_FRAME_G726        0x1002      // audio G726
#define AUDIO_FRAME_PCM8_16     0x1003      // audio compressed 8-bit linear PCM

// the decoding formats for the output
#define OUT_FMT_YUV_420         0x00000601  // YUV420, equal to OUT_FMT_I420
#define OUT_FMT_YUV_422         0x00000102  // YUV422, equal to OUT_FMT_YUY2
#define OUT_FMT_AUDIO           0x00000602  // audio, PCM 8000Hz, 16-bit sampling, 1 channel

// pixel format
// see http://www.fourcc.org/yuv.php for more information
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                  \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#define OUT_FMT_UYVY             MAKEFOURCC('U', 'Y', 'V', 'Y')  // packet 16-bit YUV, equal to Y422, UYNV, HDYC
#define OUT_FMT_YUY2             MAKEFOURCC('Y', 'U', 'Y', '2')  // packet 16-bit YUV, equal to YUNV, V422, YUYV
#define OUT_FMT_YV12             MAKEFOURCC('Y', 'V', '1', '2')  // planner 12-bit YUV
#define OUT_FMT_I420             MAKEFOURCC('I', '4', '2', '0')  // planner 12-bit YUV, equal to IYUV
#define OUT_FMT_NV12             MAKEFOURCC('N', 'V', '1', '2')  // planner 12-bit YUV
#define OUT_FMT_NV21             MAKEFOURCC('N', 'V', '2', '1')  // planner 12-bit YUV
#define OUT_FMT_RGB24            2                               // packet 24-bit, RGBRGB...
#define OUT_FMT_BGR24            3                               // packet 24-bit (the most common RGB format), BGRBGR...


////////////////////////////////////////////////////////////////////////////////
// Structure definition.
////////////////////////////////////////////////////////////////////////////////

// The data type of input.
enum HB_PLAY_MODE
{
    HB_FILE_MODE,                       // Read data from a file.
    HB_STREAM_REALTIME_MODE,            // Receive real-time data.
    HB_STREAM_FILE_MODE                 // Input data from a file as stream.
};

// The type of hardware accelerated decoder.
enum HB_HD_TYPE
{
    HB_HD_NOT_SUPPORTED=1,
    HB_HD_AUTO,            // Determines the decoder according to graphics type.
    HB_HD_NVIDIA_CUDA,     // NVIDIA graphics solution.
    HB_HD_MS_DXVA          // Microsoft solution.
};

// detailed information of the frame
typedef struct _HB_FRAME
{
    DWORD           dwFrameNumber;      // frame number, incremental

    WORD            wFrameType;         // frame type
    BYTE            cbAlgorithm;        // algorithm
    BYTE            cbVersion;          // version

    WORD            wWidth;             // picture width in pixels
    WORD            wHeight;            // picture height in pixels

    DWORD           dwDTS;              // the time stamp of the frame, in milliseconds
    SYSTEMTIME      ETS;                // the system time when encoded

    DWORD           dwEncryptFlag;      // encrypt flag
    DWORD           dwEncryptKey;       // encrypt key
    DWORD           dwEncryptText;      // encrypt text

    WORD            wOsdFlag;           // OSD flag, valid only for video frame
    WORD            wOsdLines;          // Represents the OSD lines if the wOsdFlag 
                                        // parameter is not null, otherwise null. 
    LPVOID          pOsd;               // the long pointer to the OSD information
    DWORD           dwReserved;         // reserved for  alignment 

    ULARGE_INTEGER  ulOffset;           // the offset of bytes from the beginning of file
                                        // to the current position

    DWORD           dwHeaderSize;       // the size of the frame header 
    PVOID           pHeader;            // the pointer to the frame header

    DWORD           dwDataSize;         // the size of the frame data
    PVOID           pData;              // the pointer to the frame data

    DWORD           Reserved[8];        // reserved
}HB_FRAME, *PHB_FRAME;

// brief information of the frame
typedef struct	_HB_FRAME_INFO
{
    long            nWidth;             // picture width in pixels, 0 if audio
    long            nHeight;            // picture height in pixels, 0 if audio
    long            nStamp;             // the time stamp of the frame in milliseconds
    long            nType;              // the decoding format for the output
    long            nFrameRate;         // frame rate
}HB_FRAME_INFO, *PHB_FRAME_INFO;

// time information of the frame
typedef	struct _HB_VIDEO_TIME
{
    short           year;               // year
    short           month;              // month
    short           day;                // day
    short           hour;               // hour
    short           minute;	            // minute
    short           second;             // second
    short           milli;              // millisecond
    short           week;               // week
}HB_VIDEO_TIME, *PHB_VIDEO_TIME;

// position information of the frame
typedef struct _HB_FRAME_POS
{
    // the offset of bytes from the beginning of file to the current position
    ULARGE_INTEGER  nFilePos;
    long            nFrameNum;          // frame number
    long            nFrameTime;         // the time stamp of the frame in milliseconds
}HB_FRAME_POS, *PHB_FRAME_POS;

// single frame information
typedef struct _HB_FRAME_INFO_EX
{
    DWORD           dwFrameType;        // frame type, OUT_FMT_AUDIO or pixel format
    DWORD           dwFrameSize;        // the frame size, in bytes
    PVOID           pFrame;             // the pointer to the frame buffer

    // Only for video frame.
    DWORD           dwWidth;            // picture width in pixels
    DWORD           dwHeight;           // picture height in pixels
    DWORD           dwDTS;              // the time stamp of the frame in milliseconds

    // Only for audio frame.
    DWORD           dwSampleRate;       // audio sampling rate
    DWORD           dwSampleBit;        // audio sampling bits

    DWORD           dwReserved[4];      // reserved
}HB_FRAME_INFO_EX, *PHB_FRAME_INFO_EX;

////////////////////////////////////////////////////////////////////////////////
// Callback functions definition.
////////////////////////////////////////////////////////////////////////////////

// For HB_PLAY_SetParseCallBackEx.
typedef void (CALLBACK* LPSRCDATAPARSECBFUNEX)(long nChl, const HB_FRAME* pFrame, LPVOID pContext);

// For HB_PLAY_SetParseCallBack.
typedef void (CALLBACK* LPSRCDATAPARSECBFUN)(long nChl, char* SrcDataBuf,
            long nFrameType, long nReserved1, long nReserved2, HB_VIDEO_TIME ets);

// For HB_PLAY_SetDecCallBackEx.
typedef void (CALLBACK* LPDECCBFUN)(long nChl, char* pBuf, long nSize,
            HB_FRAME_INFO* pFrameInfo, LPVOID pContext, HB_VIDEO_TIME* pVideoTime);

// For HB_PLAY_RegisterDrawDCFun
typedef void (CALLBACK* LPDRAWDCFUN)(long nChl, HDC hDC, long nUserData);

// For HB_PLAY_SetSnapShotCallBack
typedef void (CALLBACK* LPSNAPSHOTCBFUN)(long nChl, DWORD nSize, char* pBuf,
            long nWidth, long hHeight, long nType);

// For HB_PLAY_SetIndexInfoCallBack
typedef void (CALLBACK* LPINDEXCBFUN)(long nChl, long nUserData);

// For HB_PLAY_SetSourceBufferCB.
typedef void (CALLBACK* LPSOURCEBUFCALLBACK)(long nChl, DWORD dwBufSize,
            void* pUser, void* pResvered);

// For HB_PLAY_SetDisplayCallBack.
typedef void (CALLBACK* LPDISPLAYCBFUN)(long nChl, char* pBuf, long nSize, long nWidth, 
    long hHeight, long nStamp, long nType, long nReserved);


////////////////////////////////////////////////////////////////////////////////
// Functions definition.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OpenFile.
// Description:
//      Opens a Hangban video file.
// Parameters:
//      [in] nChl - Channel number.
//      [in] sFile - File name.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Must call this function before operating a channel. When the channel is
//      no longer needed, close it by HB_PLAY_CloseFile.
//      There are two versions of the function, ANSI and Unicode:
//      HB_PLAY_OpenFileA, ANSI version.
//      HB_PLAY_OpenFileW, Unicode version.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_OpenFile(long nChl, LPCSTR sFile);
HBDLLAPI BOOL __stdcall HB_PLAY_OpenFileA(long nChl, LPCSTR sFile);
HBDLLAPI BOOL __stdcall HB_PLAY_OpenFileW(long nChl, LPCWSTR sFile);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OpenFileEx.
// Description:
//      Opens a Hangban video file.
// Parameters:
//      [in] sFile - File name.
// Return:
//      If the channel is opened successfully, this function returns a valid channel
//      number, which can also be considered as channel handle; otherwise returns 0.
// Remarks:
//      Must call this function before operating a channel. When the channel is
//      no longer needed, close it by HB_PLAY_CloseFile.
//      There are two versions of the function, ANSI and Unicode:
//      HB_PLAY_OpenFileExA, ANSI version.
//      HB_PLAY_OpenFileExW, Unicode version.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI long __stdcall HB_PLAY_OpenFileEx(LPCSTR sFile);
HBDLLAPI long __stdcall HB_PLAY_OpenFileExA(LPCSTR sFile);
HBDLLAPI long __stdcall HB_PLAY_OpenFileExW(LPCWSTR sFile);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_CloseFile.
// Description:
//      Closes the channel, which was opened by HB_PLAY_OpenFile or HB_PLAY_OpenFileEx.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_CloseFile(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function:
//      HB_PLAY_Play.
// Description:
//      Start to play a file or stream.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] hWnd - Handle to the window for displaying video.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_Play(long nChl, HWND hWnd);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_Pause.
// Description:
//      Pauses the play of the file.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_Pause(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_Stop.
// Description:
//      Stops the play of the file.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Valid when the channel is opened by HB_PLAY_OpenFile or HB_PLAY_OpenFileEx.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_Stop(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_Fast.
// Description:
//      Speeds up the play rate.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      For each call, the play rate speeds up to the twice of the original rate.
//      Maintains the maximum rate if it has already speeded up to the 16 times
//      of the normal rate.
//      Call HB_PLAY_Play to resume the normal rate from current frame.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_Fast(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_Slow.
// Description:
//      Slows down the play rate.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      For each call, the play rate slows down to the half of the original rate.
//      Maintains the minimum rate if it has already slowed down to 1/16 of the normal rate.
//      Call HB_PLAY_Play to resume the normal rate from current frame.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_Slow(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OneByOne.
// Description:
//      Plays one single frame forward.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      For each call, plays one frame forward.
//      HB_PLAY_PlayBySingleFrame is the same as HB_PLAY_OneByOne.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_OneByOne(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_PlayBySingleFrame(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OneByOneBack.
// Description:
//      Plays one single frame backwards.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      For each call, plays one frame backwards.
//      Valid when the index has been created.
//      HB_PLAY_PlayBySingleFrameBack is the same as HB_PLAY_OneByOneBack.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_OneByOneBack(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_PlayBySingleFrameBack(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_PlayBack.
// Description:
//      Plays backwards continuously from current frame.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_PlayBack(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetFileTime.
// Description:
//      Gets the total time for playing the opened file.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The total time for playing the opened file in seconds.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetFileTime(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetFileTimeMilliSec.
// Description:
//      Gets the total time for playing the opened file.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The total time for playing the opened file in milliseconds.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetFileTimeMilliSec(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetFileFrames.
// Description:
//      Gets the total frames of the opened file.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The total frames of the opened file.
// Remarks:
//      Valid when the index has been created.
//      HB_PLAY_GetFileTotalFrames is the same as HB_PLAY_GetFileFrames.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetFileFrames(long nChl);
HBDLLAPI DWORD __stdcall HB_PLAY_GetFileTotalFrames(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetPlayedFrames.
// Description:
//      Gets the frames already decoded.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The frames already decoded.
// Remarks:
//      HB_PLAY_GetPlayedFrameNum is the same as HB_PLAY_GetPlayedFrames.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI long __stdcall HB_PLAY_GetPlayedFrames(long nChl);
HBDLLAPI long __stdcall HB_PLAY_GetPlayedFrameNum(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetCurrentFrameRate.
// Description:
//      Gets the current frame rate.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The current frame rate.
// Remarks:
//      HB_PLAY_GetCurrFrameRate is the same as HB_PLAY_GetCurrentFrameRate.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetCurrentFrameRate(long nChl);
HBDLLAPI DWORD __stdcall HB_PLAY_GetCurrFrameRate(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetPlaySpeed.
// Description:
//      Sets play rate for speeding up or slowing down.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] dwSpeed - Effective values: 2, 4, 8 and 16.
//      [in] bSlow - If TRUE, slow down; otherwise speed up.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Call HB_PLAY_Play to play normally.
//      If bSlow is FALSE, the play rate equals the value of dwSpeed.
//      If bSlow is TRUE, the play rate equals the reciprocal of the value of dwSpeed.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlaySpeed(long nChl, long dwSpeed, BOOL bSlow);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetCurrentFrameNum.
// Description:
//      Gets current frame number.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Current frame number.
// Remarks:
//      HB_PLAY_GetCurrFrameNum is the same as HB_PLAY_GetCurrentFrameNum.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetCurrentFrameNum(long nChl);
HBDLLAPI long __stdcall HB_PLAY_GetCurrFrameNum(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetCurrentFrameNum.
// Description:
//      Sets current frame number.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] dwFrameNum - Current frame number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetCurrentFrameNum(long nChl, DWORD dwFrameNum);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetPlayedTimeEx.
// Description:
//      Gets current playing time.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      HB_PLAY_GetPlayTime is the same as HB_PLAY_GetPlayedTimeEx.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI DWORD __stdcall HB_PLAY_GetPlayedTimeEx(long nChl);
HBDLLAPI DWORD __stdcall HB_PLAY_GetPlayTime(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetPlayedTimeEx.
// Description:
//      Sets current playing time.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] dwTime - Current playing time in milliseconds.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Both HB_PLAY_GetCurrentFrameNum and HB_PLAY_SetPlayTime are the same
//      as HB_PLAY_SetPlayedTimeEx.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlayedTimeEx(long nChl, DWORD dwTime);
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlayedTime(long nChl, DWORD dwTime);
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlayTime(long nChl, DWORD dwTime);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetPlayPos.
// Description:
//      Gets play position.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Play position in percentage.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI float __stdcall HB_PLAY_GetPlayPos(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetPlayPos.
// Description:
//      Sets play position.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] dPos - Play position in percentage.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Valid when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlayPos(long nChl, float dPos);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetFileEndMsg.
// Description:
//      Sets the window specified by hWnd to receive the message specified by nMsg
//      when the file is played to the end.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] hWnd - The handle of the window.
//      [in] nMsg - The message ID.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetFileEndMsg(long nChl, HWND hWnd, UINT nMsg);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OpenSound.
// Description:
//      Opens sound.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      HB_PLAY_PlaySound is the same as HB_PLAY_OpenSound.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_OpenSound(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_PlaySound(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_CloseSound.
// Description:
//      Closes sound.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      If the sound has been opened by HB_PLAY_OpenSound or HB_PLAY_PlaySound,
//      call this function to stop playing audio. 
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_CloseSound(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetVolume.
// Description:
//      Gets the volume of the sound.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The volume of the sound.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI WORD __stdcall HB_PLAY_GetVolume(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetVolume.
// Description:
//      Sets the volume of the sound.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] nVolume - The volume of the sound, ranging from 0 to 65535.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetVolume(long nChl, WORD nVolume);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetPictureSize.
// Description:
//      Gets the original size of the picture.
// Parameters: 
//      [in] nChl - Channel number.
//      [out] pWidth - The pointer to the original picture width .
//      [out] pHeight - The pointer to the original picture height.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Couldn't get correct value until a frame is decoded successfully.
//      If the picture size changes, call this function to get the new value.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_GetPictureSize(long nChl, long* pWidth, long* pHeight);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_RegisterDrawDCFun.
// Description: 
//      Sets callback function.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] DrawDCFun - Callback function pointer.
//           void CALLBACK DrawDCFun(long nChl, HDC hDC, long nUserData);
//              - [in] nChl - Channel number.
//              - [in] hDC - The handle of DC.
//              - [in] nUserData - User data.
//      [in] nUserData - User data.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      This function would be called when the index has been created.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_RegisterDrawDCFun(long nChl, LPDRAWDCFUN DrawDCFun, long nUserData);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetDecCallBackEx.
// Description:
//      Sets callback function after decoding.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] DecCBFun - The callback function pointer.
//           void CALLBACK DecCBFun(long nChl, char* pBuf, long nSize, 
//              FRAME_INFO* pFrameInfo, long nReserved1, long nReserved2);
//              - [in] nChl - Channel number.
//              - [in] pBuf - The pointer to the output YUV data.
//              - [in] nSize - The size of the output YUV data.
//              - [in] pFrameInfo - The pointer to the frame information.
//              - [in] pContext - Pointer to a user defined variable.
//              - [in] nReserved - Reserved.
//      [in] pContext - Pointer to a user defined variable.
//      [in] nReserved - Reserved.
//      [in] nOutFormat - The formats of output YUV data. For format details, refer
//           to the decoding formats for the output in Macro definition.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetDecCallBackEx(long nChl, LPDECCBFUN DecCBFun,
    LPVOID pContext, long nReserved, long nOutFormat);

HBDLLAPI BOOL __stdcall HB_PLAY_SetDecCallBack(long nChl, LPDECCBFUN DecCBFun,long nOutFormat);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetParseCallBackEx.
// Description:
//      Sets callback function.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] pfnParse - Callback function pointer.
//           void CALLBACK SrcDataParseCBFun(long nChl, const HB_FRAME* pFrame, LPVOID pContext);
//              - [in] nChl - Channel number.
//              - [in] pFrame - The pointer to the original data of a frame.
//              - [in] pContext - Reserved.
//      [in] pContext - Reserved.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetParseCallBackEx(long nChl,
    LPSRCDATAPARSECBFUNEX pfnParse, LPVOID pContext );


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetParseOnly.
// Description:
//      Sets whether to decode or not.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] bParseOnly - Decode or not.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      If bParseOnly is TRUE, the SDK does not execute decoding.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetParseOnly(long nChl, BOOL bParseOnly);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetPlayOption.
// Description:
//      Sets play options, including displaying or not, and playing audio or not.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] bEnableDisplay - Display or not.
//      [in] bEnableSound - Play audio or not.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetPlayOption(long nChl, BOOL bEnableDisplay, BOOL bEnableSound);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetDisplayRegion.
// Description:
//      Sets display region.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] nRegionNum - Display region number from 1 to (MAX_DISPLAY_WND-1).
//      [in] pSrcRect - The pointer to the original picture region.
//      [in] hDestWnd - Handle of the destination window.
//      [in] bEnable - Enable or disable the display region.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetDisplayRegion(long nChl, DWORD nRegionNum,
    RECT* pSrcRect, HWND hDestWnd, BOOL bEnable);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OpenStream.
// Description:
//      Opens stream.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] pFileHeadBuf - The pointer to the header buffer of the stream data.
//      [in] dwSize - The size of the header buffer of the stream data.
//      [in] dwReserved - Reserved.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      When the channel is no longer needed, close it by HB_PLAY_CloseStream.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_OpenStream(long nChl, char* pFileHeadBuf, DWORD dwSize, DWORD dwReserved);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_OpenStreamEx.
// Description:
//      Searches for an unoccupied channel to open stream.
// Parameters: 
//      [in] pFileHeadBuf - The pointer to the header buffer of the stream data.
//      [in] dwSize - The size of the header buffer of the stream data.
//      [in] dwReserved - Reserved.
// Return:
//      If the channel is opened successfully, this function returns a valid channel
//      number, which can also be considered as channel handle; otherwise returns 0.
// Remarks:
//      When the channel is no longer needed, close it by HB_PLAY_CloseStream.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI long __stdcall HB_PLAY_OpenStreamEx(char* pFileHeadBuf, DWORD dwSize, DWORD dwReserved);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_InputData.
// Description:
//      Inputs stream data.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] pBuf - Stream data.
//      [in] dwnSize - Stream data size.
//      [in] dwReserved - Reserved, must be zero.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Valid when HB_PLAY_OpenStream or HB_PLAY_OpenStreamEx is called.
//      This function would return FALSE if the stream data is input too fast.
//      In this case, the stream data should be input once more.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_InputData(long nChl, char* pBuf, DWORD dwSize, DWORD dwReserved);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_CloseStream.
// Description:
//      Closes the stream, which is opened by HB_PLAY_OpenStream or HB_PLAY_OpenStreamEx..
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_CloseStream(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetMsgWnd.
// Description:
//      Sets the window to receive the message when the index has been created.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] hWnd - Handle to the window to receive the MSG_INDEX_OK message.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetMsgWnd(long nChl, HWND hWnd);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SnapShot.
// Description:
//      Catches a picture, and saves it to an assigned file.
// Parameters:
//      [in] nChl - Channel number.
//      [in] sFile - File name.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      There are two versions of the function, ANSI and Unicode:
//      HB_PLAY_SnapshotA, ANSI version.
//      HB_PLAY_SnapshotW, Unicode version.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SnapShot(long nChl, LPCSTR sFile);
HBDLLAPI BOOL __stdcall HB_PLAY_SnapshotA(long nChl, LPCSTR sFile);
HBDLLAPI BOOL __stdcall HB_PLAY_SnapshotW(long nChl, LPCWSTR sFile);

#ifdef _UNICODE
#define HB_PLAY_Snapshot HB_PLAY_SnapshotW
#else
#define HB_PLAY_Snapshot HB_PLAY_SnapshotA
#endif


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetDynamicBPS.
// Description:
//      Gets the real-time bit rate of the stream.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The real-time bit rate of the stream in kbps(kilobits per second).
// Remarks:
//      Returns nonzero values when played for a while.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI double __stdcall HB_PLAY_GetDynamicBPS(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetAverageBPS.
// Description:
//      Gets the average bit rate of the stream.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The average bit rate of the stream in kbps.
// Remarks:
//      Returns nonzero values when played for a while.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI double __stdcall HB_PLAY_GetAverageBPS(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetVideoColor.
// Description: 
//      Sets video color.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] nBrightness - Brightness, ranging from 0 to 127, 64 by default.
//      [in] nContrast - Contrast, ranging from 0 to 127, 64 by default.
//      [in] nSaturation - Saturation, ranging from 0 to 127, 64 by default4.
//      [in] nHue - Hue, ranging from 0 to 127, 64 by default.
//      [in] nReserved - Reserved.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetVideoColor(long nChl, long nBrightness,
    long nContrast, long nSaturation , long nHue, long nReserved);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetVideoColor.
// Description: 
//      Gets video color.
// Parameters: 
//      [in] nChl - Channel number.
//      [out] pBrightness - Pointer to brightness.
//      [out] pContrast - Pointer to contrast.
//      [out] pSaturation - Pointer to saturation.
//      [out] pHue - Pointer to hue.
//      [in] nReserved - Reserved.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_GetVideoColor(long nChl, long* pBrightness,
    long *pContrast, long *pSaturation , long *pHue, long nReserved);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetIndexInfoCallBack.
// Description: 
//      Sets callback function.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] IndexCBFunc - Callback function pointer.
//           void CALLBACK IndexCBFunc(long nChl, long nReserved);
//              - [in] nChl - Channel number.
//              - [in] nUserData - User data.
//      [in] nUserData - User data.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//	    This function would be called when the index has been creation.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetIndexInfoCallBack(long nChl, LPINDEXCBFUN IndexCBFun, long nUserData);


////////////////////////////////////////////////////////////////////////////////
// Function:
//      HB_PLAY_GetFileHead.
// Description:
//      Retrieves file header size and file header information.
// Parameters: 
//      [in] nChl - Channel number.
//      [out] pBuffer - The pointer to the file header information.
//      [in/out] pSize - The pointer to the size of the file header.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Call this function for twice. Retrives the file header size for the 
//      first call, and the file header information for the second call.
//      The second call would be valid only when the return value of the 
//      first call is nonzero.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_GetFileHead(long nChl, BYTE* pBuffer, DWORD* pSize);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetCPULimitEx.
// Description: 
//      Retrieves CPU limitation.
// Parameters: 
//      [in] nChl - Channel number.
//      [out] pCpuUse - Pointer to CPU usage limitation.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_GetCPULimitEx(long nChl, int* pCpuUse);


////////////////////////////////////////////////////////////////////////////////
// Function:
//      HB_PLAY_SetCPULimitEx.
// Description:
//      Limits CPU usage. If the CPU usage is larger than the limitation,
//      decoding will be delayed.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] nCpuUse - CPU usage limitation, from 50 to 100.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      The default limitation is 100, which means that CPU usage is unlimited.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetCPULimitEx(long nChl, int nCpuUse);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_DecodeOneFrame.
// Description: 
//      Decode an audio or video frame.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] pInputFrame - Stream data, including audio or video.
//      [in] dwInputSize - Stream size, in bytes.
//      [in] nOutFormat - The formats of output YUV data. For format details, refer
//           to the decoding formats for the output in Macro definition.
// Return:
//      Nonzero if successful; otherwise 0.
//      This function would return FALSE if the stream data is input too fast.
//      In this case, the stream data should be input once more.
// Remarks:
//      
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_DecodeOneFrame(long nChl, LPCVOID pInputFrame,
    DWORD dwInputSize, long nOutFormat, PHB_FRAME_INFO_EX pFrameInfo, long* pRemainderCount);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_ResetSourceBuffer.
// Description: 
//      Clear the data in the source buffer.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_ResetSourceBuffer(long nChl);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_SetSourceBufferCB.
// Description: 
//      Sets callback function.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] dwThreshold - Buffered frame threshold.
//      [in] pfnSourceBufCallback - Callback function pointer.
//           void CALLBACK pfnSourceBufCallback(long nChl, DWORD dwBufSize,
//              void* pUser, void* pResvered);
//              - [in] nChl - Channel number.
//              - [in] dwBufSize - The number of frames remains in the source buffer.
//              - [in] pUser - User context.
//              - [in] pResvered - Reserved.
//      [in] pUser - User context.
//      [in] pReserved - Reserved.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//	    This function would be called when the source buffer data is less than the
//      threshold value.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetSourceBufferCB(long nChl, DWORD dwThreshold,
    LPSOURCEBUFCALLBACK pfnSourceBufCallback, void* pUser, void* pReserved);


////////////////////////////////////////////////////////////////////////////////
// Function:
//      HB_PLAY_SetDecodeMode.
// Description:
//      Sets the type of hardware accelerated decoder.
// Parameters: 
//      [in] nChl - Channel number.
//      [in] nMode - The type of hardware accelerated decoder. For more details,
//           refer to enum type HB_HD_TYPE.
// Return:
//      Nonzero if successful; otherwise 0.
// Remarks:
//      Currently only HB_HD_NVIDIA_CUDA is supported.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetDecodeMode(long nChl, HB_HD_TYPE nMode);


////////////////////////////////////////////////////////////////////////////////
// Function: 
//      HB_PLAY_GetDecodeMode.
// Description: 
//      Retrieves the type of hardware accelerated decoder.
// Parameters: 
//      [in] nChl - Channel number.
// Return:
//      The type of hardware accelerated decoder.
// Remarks:
//      For more details of the return value, refer to enum type HB_HD_TYPE.
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI HB_HD_TYPE __stdcall HB_PLAY_GetDecodeMode(long nChl);


////////////////////////////////////////////////////////////////////////////////
//Obsolete Functions
////////////////////////////////////////////////////////////////////////////////
HBDLLAPI BOOL __stdcall HB_PLAY_SetStreamOpenMode(long nChl, DWORD nMode);
HBDLLAPI DWORD __stdcall HB_PLAY_GetStreamOpenMode(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_ConvertToBMPFile(long nChl, char* pBuf, long nSize,
    long nWidth, long nHeight, long nType, char* sFileName);
HBDLLAPI BOOL __stdcall HB_PLAY_ConvertToBMPFileEx(long nChl, char* pBuf,
    long nSize, long nWidth, long nHeight, long nType,
    unsigned char* pBufDestination, int* len, int* pBMPHeadLen);
HBDLLAPI BOOL __stdcall HB_PLAY_SetParseCallBack(long nChl, LPSRCDATAPARSECBFUN SrcDataParseCBFun, 
    long nReserved1,long nReserved2);
HBDLLAPI BOOL __stdcall HB_PLAY_SetPicQuality(long nChl, BOOL bHighQuality);
HBDLLAPI BOOL __stdcall HB_PLAY_GetPicQuality(long nChl, BOOL* bHighQuality);
HBDLLAPI BOOL __stdcall HB_PLAY_SetOverlayMode(long nChl, BOOL bOverlayMode, COLORREF colorKey);
HBDLLAPI BOOL __stdcall HB_PLAY_SetSnapShotCallBack(long nChl, LPSNAPSHOTCBFUN SnapshotCBFun);
HBDLLAPI void __stdcall HB_PLAY_EnableIndexThread(BOOL bEnable);
HBDLLAPI BOOL __stdcall HB_PLAY_GetIndexInfo(long nChl, HB_FRAME_POS* pBuffer, DWORD* pSize);
HBDLLAPI BOOL __stdcall HB_PLAY_SetCPULimit(int nCpuUse);
HBDLLAPI BOOL __stdcall HB_PLAY_GetCPULimit(int* pCpuUse);
HBDLLAPI DWORD __stdcall HB_PLAY_GetColorKey(long nChl);
HBDLLAPI DWORD __stdcall HB_PLAY_GetLastError(long nChl);
HBDLLAPI DWORD __stdcall HB_PLAY_GetSourceBufferRemain(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_RefreshPlay(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_InputVideoData(long nChl,char* pBuf,DWORD nSize);
HBDLLAPI BOOL __stdcall HB_PLAY_InputAudioData(long nChl,char* pBuf,DWORD nSize);
HBDLLAPI DWORD __stdcall HB_PLAY_GetCurrentFrameTime(long nChl);
HBDLLAPI BOOL __stdcall HB_PLAY_SetDisplayCallBack(long nChl, LPDISPLAYCBFUN DisplayCBFun);
HBDLLAPI BOOL __stdcall HB_PLAY_InitDDraw(HWND hWnd);
HBDLLAPI BOOL __stdcall HB_PLAY_RealeseDDraw();

////////////////////////////////////////////////////////////////////////////////
//End HB_PLAY API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif  // HBPLAYSDK_H