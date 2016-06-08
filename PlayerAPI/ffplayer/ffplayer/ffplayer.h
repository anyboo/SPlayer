#pragma once

#include "../../../PlayerFactory/struct_def.h"

#include "Buffer.h"
#include "FileConverter.h"


#define LENGTH (1024)

#define MAX_SIZE (40)
#define MAX_PCM_SIZE (8)
#define MAX_YUV_SIZE (4)

//#define VideoPacketMaxLen (1024*1024*2.5)

#define PCMBUFLEN (192000) // 1 second of 48khz 32bit audio

#define MIN_VOLUME1 (0x0)
#define MAX_VOLUME (0xffff)


typedef struct T_RemainPCM{
	char buf[PCMBUFLEN];
	int len;
	int size;
	int64_t pts;
	int serial;
};

enum PLAY_STATE{
	NO_PLAY = -1,
	PLAYING = 0,
	PAUSE,
	STOP,
};

//同步方式
enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
//同步时钟
typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

class ffplayer
{
public:
	ffplayer();
	~ffplayer();
public:
	BOOL OpenFile(char *PFile);

	BOOL Play(HWND hwnd);
	BOOL Pause(DWORD nPause);
	BOOL Stop();
	BOOL Fast();
	BOOL Slow();
	BOOL OneByOne();
	BOOL OneByOneBack();
	BOOL PlaySoundShare();
	BOOL StopSoundShare();
	BOOL SetVolume(DWORD nVolume);
	DWORD GetVolume();

	BOOL  SetPlayPos(float fRelativePos);
	float GetPlayPos();
	DWORD  GetFileTime();
	DWORD  GetPlayedTime();
	BOOL  GetPictureSize(LONG *pWidth, LONG *pHeight);
	BOOL  CapturePic(char *pSaveFile, int iType);
	bool SetFileEndCallback(long nID,FileEndCallback fileCallBack, void *pUser);
	bool SetDisplayCallback(long nID, DisplayCallback disaplayCallBack, void *pUser);
	/*
	BOOL FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime);
	BOOL FileCutClose();
	int FileCutProcess();
	BOOL FileCutPause(DWORD nPause);*/

	BOOL FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara);
	BOOL FileConvertClose();
	int FileConvertProcess();
	BOOL FileConvertPause(DWORD nPause);
public:
	void FillAudio(Uint8 * stream, int len);
private:
	bool OpenVideoCode();
	bool OpenAudioCode();
	void CloseAudio();
	void CloseVideo();
	void Close();	
	void FlushAudioBuf();//拖动进度条时把当前音频和视频缓冲清空
	void FlushVideoBuf();
	bool IniSDL();
	void DemuxThreadRun();
	void ReadThreadRun();
	void RenderThreadRun();
	void AudioDecoderThreadRun();
	void VideoDecoderThreadRun();
	void decode(AVPacket &packet, int &got, int bFlush);
	int Async(__int64 &pts);
	void UpdateVideoPts(PInfo pYuvInfo);
	bool CreateRender();
	void RefreshRender();
	void DestoryRender();

	//同步相关函数
	double get_master_clock();
	double compute_target_delay(double delay);
private:
	static void WINAPI DemuxThread(void*lPalam);
	static void WINAPI ReadThread(void*lPalam);
	static void WINAPI RenderThread(void*lPalam);
	static void WINAPI AudioDecoderThread(void*lPalam);
	static void WINAPI VideoDecoderThread(void*lPalam);
	static void __cdecl SDL_Fill_audio(void *userdata, Uint8 * stream, int len);
private:
	static bool s_bIniSDL;

	SDL_Thread *m_demuxThread;
	SDL_Thread *m_renderThread;
	SDL_Thread *m_readThread;
	SDL_Thread *m_audioDeocderThread;
	SDL_Thread *m_videoDeocderThread;
	PLAY_STATE m_playState;
	bool m_bStop;
	bool m_bPause;
	bool m_bPausedSeek;
	/*unsigned __int64*/ double m_curTime;//时间 秒
	unsigned __int64 m_iLastSystemTime;
	__int64 m_curPTS;//
	double m_frame_timer;
	float m_fps;
	float m_fSeekRelativePos;
	bool m_bSeekState;
	bool m_bCanSeek;
	int m_serial;
	int m_lastserial;

	int m_iSpeed;

	CBuffer m_renderBuf;
	CBuffer m_freeRenderBuf;

	CBuffer m_videoPacketBuf;

	HWND m_hwnd;
	char m_strFileName[LENGTH];
	unsigned __int64 m_fileSize;

	
	AVFormatContext *m_pFormatCtx;
	AVCodecContext  *m_pCodecCtx;
	AVFrame         *m_pFrame;
	int m_iWidth;
	int m_iHeight;
	SDL_Window *m_sdlWindow;
	SDL_Renderer * m_pRender;
	SDL_Texture *m_pTexture ;
	bool m_bStepNext;
	bool m_bStepEnd;
	PInfo m_picSnopshot;

	AVCodecContext  *m_pAudioCodecCtx;
	AVCodec         *m_pAudioCodec;
	AVFrame         *m_pAudioFrame;
	SDL_AudioDeviceID m_devID;
	SwrContext *m_au_convert_ctx;
	int m_out_samples;
	int m_out_sample_fmt;
	int m_out_channels;
	int m_out_sample_rate;
	int m_hw_size;
	int m_bytes_per_sec;
	//char m_chPCMBuf[PCMBUFLEN];
//	int m_pcmPos;
//	int m_pcmLen;
	
	CBuffer m_AudioPackBuf;
	CBuffer m_FreeAudioPackBuf;

	CBuffer m_PcmPackBuf;
	CBuffer m_FreePcmPackBuf;
	std::recursive_mutex m_mutexAudioBuf;
	T_RemainPCM m_remainPCM;

	bool m_bHasAudio;
//	bool m_bStartAudio;
	bool m_bSilence;//静音
	float m_fVolume;//音量百分比

	int m_videoStream;
	int m_audioStream;
	unsigned __int64 m_frameCount;
	unsigned __int64 m_curFrameCount;
	unsigned __int64 m_tmpFrameCount;
	bool m_bReadFrameFinish;
	bool m_bPlay;

	FileEndCallback m_fileEndBak;
	void *m_pUser;
	long m_nID;
	bool m_bFileEnd;

	DisplayCallback m_displayBak;
	void *m_pDisplayUser;
	long m_nDisplayID;

	int  m_master_sync_type;
	Clock m_vidclk;
	Clock m_audclk;
	Clock m_extclk;

	CFileConverter *m_pFileConverter;
};

