#include "stdafx.h"
#include "ffplayer.h"
#include <atlconv.h>
#include "untils.h"

std::recursive_mutex g_mutexFFmpeg;
std::recursive_mutex g_mutexSDL;
int g_Speed[] = { -16, -8, -4, -2, 1, 2, 4, 8, 16 };


/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

#define max_frame_duration 16.0
static double get_clock(Clock *c)
{
	//if (*c->queue_serial != c->serial)
	//	return NAN;
	if (c->paused) {
		return c->pts;
	}
	else {
		double time = av_gettime_relative() / 1000000.0;
		return c->pts;
	//	return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
	}
}
static void set_clock_at(Clock *c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

static void set_clock(Clock *c, double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(c, pts, serial, time);
}
static void sync_clock_to_slave(Clock *c, Clock *slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}
/* get the current master clock value */

double ffplayer::get_master_clock()
{
	double val;

	switch (m_master_sync_type) {
	case AV_SYNC_VIDEO_MASTER:
		val = get_clock(&m_vidclk);
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = get_clock(&m_audclk);
		break;
	default:
		val = get_clock(&m_extclk);
		break;
	}
	return val;
}


double ffplayer::compute_target_delay(double delay)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (m_master_sync_type != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		duplicating or deleting a frame */
		diff = get_clock(&m_vidclk) - get_master_clock();

		/* skip or repeat frame. We take into account the
		delay to compute the threshold. I still don't know
		if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) <max_frame_duration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}


ffplayer::ffplayer()
{
	m_demuxThread = NULL;
	m_renderThread = NULL;
	m_readThread = NULL;
	m_audioDeocderThread = NULL;
	m_videoDeocderThread = NULL;

	m_pFormatCtx = 0;
	m_pCodecCtx = 0;
	m_pFrame = 0;
	m_sdlWindow = NULL;
	m_pRender = NULL;
	m_pTexture = NULL;
	m_picSnopshot = NULL;

	m_pAudioCodecCtx = 0;
	m_pAudioCodec = 0;
	m_pAudioFrame = 0;
	m_bHasAudio = false;
	m_out_samples = 0;
	m_out_sample_fmt = 0;
	m_out_channels = 0;
	m_out_sample_rate = 0;
	m_hw_size = 0;
	m_bytes_per_sec = 0;
	//m_bStartAudio = false;
	m_bSilence = false;
	m_fVolume = 1.0;
	m_devID = -1;
	m_au_convert_ctx = NULL;
	memset(&m_remainPCM, 0, sizeof(T_RemainPCM));

	m_videoStream = -1;
	m_audioStream = -1;
	m_iWidth = 0;
	m_iHeight = 0;
	m_playState = NO_PLAY;
	m_bStop = false;
	m_bPause = false;
	m_bPausedSeek = false;
	m_curPTS = 0;
	m_curTime = 0;
	m_iLastSystemTime = 0;
	m_master_sync_type= AV_SYNC_AUDIO_MASTER;//默认视频同步音频
	m_vidclk = { 0 };
	m_audclk = { 0 };
	m_bSeekState = false;
	m_bCanSeek = false;
	m_serial = 0;
	m_fps = 0;
	m_frameCount = 0;
	m_curFrameCount = 0;
	m_tmpFrameCount = 0;
	m_bReadFrameFinish = false;
	m_bPlay = false;
	m_fileEndBak = NULL;
	m_bFileEnd = false;
	m_pUser = NULL;
	m_nID = 0;
	m_fSeekRelativePos = 0.0;
	m_fileSize = 0;
	memset(m_strFileName, 0, LENGTH);


	m_iSpeed = 4;//
	m_bStepNext = false;
	m_bStepEnd = false;

	m_pFileConverter = NULL;
}


ffplayer::~ffplayer()
{
	Close();
	FileConvertClose();
	m_freeRenderBuf.destoryBuffer();
	m_renderBuf.destoryBuffer();

	m_videoPacketBuf.destoryBuffer();

	m_AudioPackBuf.destoryBuffer();
	m_FreeAudioPackBuf.destoryBuffer();

	m_PcmPackBuf.destoryBuffer();
	m_FreePcmPackBuf.destoryBuffer();
}


BOOL ffplayer::OpenFile(char *PFile)
{
	// See http://dranger.com/ffmpeg/tutorial01.html
	strcpy_s(m_strFileName, PFile);
	Close();
	av_register_all();

	// Open the video file
	int ret = avformat_open_input(&m_pFormatCtx, PFile, NULL, NULL);
	if (ret < 0)
	{
		//m_errorMsg = "Could not open the video file";
		Close();
		return false;
	}

	// Retrieve the stream information
	if (avformat_find_stream_info(m_pFormatCtx, 0) < 0)
	{
		//m_errorMsg = "Could not find the stream information";
		Close();
		return false;
	}

	// Find the first video stream
	m_videoStream = -1;

	for (unsigned i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoStream = i;
			//break;
		}
		else if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioStream = i;
		}
	}

	if (m_videoStream == -1 && m_audioStream == -1)
	{
		//	m_errorMsg = "Could not find a playable video/audio stream";
		Close();
		return false;
	}

	bool bVideoRet = OpenVideoCode();

	bool bAudioRet = OpenAudioCode();

	if (!bVideoRet&&!bAudioRet)
	{
		Close();
		return false;
	}


	if (!IniSDL()) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return false;
	}
	

	if (m_pFormatCtx && m_pFormatCtx->duration / AV_TIME_BASE > 0.0)
	{
		m_bCanSeek = true;
	}

	return true;

}

bool ffplayer::s_bIniSDL = false;

bool ffplayer::IniSDL()
{
	if (!s_bIniSDL)
	{
		std::lock_guard<std::recursive_mutex> lock(g_mutexSDL);
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
		{
			return false;
		}
		s_bIniSDL = true;
	}
	return true;
}


bool ffplayer::OpenVideoCode()
{
	if (m_videoStream == -1)
	{
		return false;
	}
	// Get a pointer to the codec context for the video stream
	m_pCodecCtx = m_pFormatCtx->streams[m_videoStream]->codec;

	// Find the decoder for the video stream
	AVCodec *pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);

	if (pCodec == NULL)
	{
		//	m_errorMsg = "Could not find a video decoder";
		CloseVideo();
		return false;
	}

	// Open the codec
	if (avcodec_open2(m_pCodecCtx, pCodec, 0) < 0)
	{
		//	m_errorMsg = "Could not open the video decoder";
		CloseVideo();
		return false;
	}

	m_iWidth = m_pCodecCtx->width;
	m_iHeight = m_pCodecCtx->height;
	// Allocate video frames
	m_pFrame = av_frame_alloc();

	if (!m_pFrame)
	{
		//	m_errorMsg = "Could not allocate memory for a frame";
		CloseVideo();
		return false;
	}

	m_fps = (float)(m_pFormatCtx->streams[m_videoStream]->r_frame_rate.num) / (float)(m_pFormatCtx->streams[m_videoStream]->r_frame_rate.den) + 0.5f;
	
	return true;

}

bool ffplayer::OpenAudioCode()
{
	if (m_audioStream == -1)
	{
		return false;
	}
	
	m_pAudioCodecCtx = m_pFormatCtx->streams[m_audioStream]->codec;

	// Find the decoder for the video stream
	m_pAudioCodec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);

	if (m_pAudioCodec == NULL)
	{
		//	m_errorMsg = "Could not find a video decoder";
		CloseAudio();
		return false;
	}

	// Open the codec
	if (avcodec_open2(m_pAudioCodecCtx, m_pAudioCodec, 0) < 0)
	{
		//	m_errorMsg = "Could not open the video decoder";
		CloseAudio();
		return false;
	}

	m_pAudioFrame = av_frame_alloc();

	if (!m_pAudioFrame)
	{
		CloseAudio();
		return false;
	}


	m_bHasAudio = true;
}

void ffplayer::Close()
{
	CloseAudio();
	CloseVideo();

	// Close the video file
	if (m_pFormatCtx)
		avformat_close_input(&m_pFormatCtx);

	m_pFormatCtx = 0;
	
}

void ffplayer::CloseVideo()
{
	if (m_pFrame)
		av_free(m_pFrame);

	// Close the codec
	if (m_pCodecCtx)
		avcodec_close(m_pCodecCtx);
	/*
	if (m_picSnopshot)
	{
		delete m_picSnopshot->Data;
		delete m_picSnopshot;
		m_picSnopshot = NULL;
	}*/

	m_pCodecCtx = 0;
	m_pFrame = 0;
	m_videoStream = -1;
}

void ffplayer::CloseAudio()
{
	if (m_pAudioFrame)
		av_free(m_pAudioFrame);

	if (m_pAudioCodecCtx)
		avcodec_close(m_pAudioCodecCtx);

	if (m_au_convert_ctx)
		av_free(m_au_convert_ctx);

	m_pAudioCodecCtx = NULL;
	m_au_convert_ctx = NULL;
	m_pAudioCodec = NULL;
	m_pAudioFrame = NULL;
	m_audioStream = -1;

	SDL_CloseAudioDevice(m_devID);
}

BOOL ffplayer::Play(HWND hwnd)
{
	if (m_bStepNext||m_bPause)
	{
		m_iSpeed = 4;
		m_bStepNext = false;
		m_bStepEnd = true;
		m_bPause = false;
		return true;
	}
	if (m_bPlay)
	{
		return true;
	}
	m_bPlay = true;
	m_hwnd = hwnd;	
	if (!m_bCanSeek)
	{
		int status;
		m_readThread = SDL_CreateThread((SDL_ThreadFunction)ReadThread, NULL, this);
		Sleep(200);

	}

	m_demuxThread = SDL_CreateThread((SDL_ThreadFunction)DemuxThread, NULL, this);
	m_videoDeocderThread = SDL_CreateThread((SDL_ThreadFunction)VideoDecoderThread, NULL, this);
	m_renderThread = SDL_CreateThread((SDL_ThreadFunction)RenderThread, NULL, this);
	if (m_bHasAudio)
	{
		m_out_samples = 1024;
		//	m_out_samples = 1152;
		m_out_sample_fmt = AV_SAMPLE_FMT_S16;
		m_out_channels = m_pAudioCodecCtx->channels;
		m_out_sample_rate = m_pAudioCodecCtx->sample_rate;
		//结构体，包含PCM数据的相关信息
		SDL_AudioSpec wanted_spec, spec;
		wanted_spec.freq = m_out_sample_rate;
		wanted_spec.format = AUDIO_S16SYS;

		//#define AUDIO_S8        0x8008  /**< Signed 8-bit samples */
		//#define AUDIO_U16LSB    0x0010  /**< Unsigned 16-bit samples */
		//#define AUDIO_S16LSB    0x8010  /**< Signed 16-bit samples */
		wanted_spec.channels = m_out_channels;
		wanted_spec.silence = 0;

		//nb_samples: AAC - 1024 MP3 - 1152
		//wanted_spec.samples = m_pAudioCodecCtx->frame_size;

		wanted_spec.samples = m_out_samples;

		wanted_spec.callback = (SDL_AudioCallback)SDL_Fill_audio;
		wanted_spec.userdata = this;

		m_devID = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (m_devID == 0)//步骤（2）打开音频设备 
		{
			printf("can't open audio.\n");
			m_bHasAudio = 0;
			return true;
		}
		if (spec.format != AUDIO_S16SYS) {
			fprintf(stderr, "SDL advised audio format %d is not supported!\n", spec.format);
		//	return;
		}
		if (spec.channels != wanted_spec.channels) {
			/*wanted_channel_layout = av_get_default_channel_layout(spec.channels);
			if (!wanted_channel_layout) {
			fprintf(stderr, "SDL advised channel count %d is not supported!\n", spec.channels);
			return -1;
			}*/
		}
		m_hw_size = spec.size;
		m_bytes_per_sec = av_samples_get_buffer_size(NULL, m_out_channels, m_out_sample_rate, (AVSampleFormat)m_out_sample_fmt, 1);
		if (m_devID>0)
			SDL_PauseAudioDevice(m_devID, 0);

		m_audioDeocderThread = SDL_CreateThread((SDL_ThreadFunction)AudioDecoderThread, NULL, this);
	}


	return true;
}

BOOL ffplayer::Pause(DWORD nPause)
{
	if (m_bStepNext)
	{
		return false;
	}
	m_bPause = nPause;
	if (m_devID > 0)
	{
		SDL_PauseAudioDevice(m_devID, m_bPause);
	}
	return true;
}

BOOL ffplayer::Stop()
{
	m_bStop = true;
	int status;
	if (!m_bCanSeek)
	{
		SDL_WaitThread(m_readThread, &status);
	}
	if (m_bHasAudio)
	{
		SDL_WaitThread(m_audioDeocderThread, &status);
	}
	SDL_WaitThread(m_demuxThread, &status);
	SDL_WaitThread(m_renderThread, &status);
	SDL_WaitThread(m_videoDeocderThread, &status);

	Close();
	return true;
}

BOOL ffplayer::Fast()
{	
	if (m_iSpeed >8)
	{
		return false;
	}

	m_iSpeed++;
	return true;
}

BOOL ffplayer::Slow()
{
	if (m_iSpeed < 0)
	{
		return false;
	}
	m_iSpeed--;
	return true;
}

BOOL ffplayer::OneByOne()
{
	if (m_bPause)
	{
		if (m_devID > 0)
		{
			SDL_PauseAudioDevice(m_devID, !m_bPause);
		}
		m_bPause = false;
	}
	
	m_bStepNext = true;
	m_bStepEnd = false;
	return true;
}

BOOL ffplayer::OneByOneBack()
{
	return false;
}

BOOL ffplayer::PlaySoundShare()
{
	m_bSilence = false;
	return true;
}

BOOL  ffplayer::StopSoundShare()
{
	m_bSilence = true;
	return true;
}

BOOL ffplayer::SetVolume(DWORD nVolume)
{
	m_fVolume = (float)nVolume / (float)MAX_VOLUME;
	return true;
}

DWORD ffplayer::GetVolume()
{
	return MAX_VOLUME*m_fVolume;
}

BOOL  ffplayer::SetPlayPos(float fRelativePos)
{
	FlushAudioBuf();
	FlushVideoBuf();
	m_bSeekState = true;
	m_fSeekRelativePos = fRelativePos;
	
//	SDL_CondSignal
	return true;
}

void ffplayer::FlushAudioBuf()
{
	PInfo pInfo = m_AudioPackBuf.getInfoFromList();
	while (pInfo)//事件等待
	{
		m_FreeAudioPackBuf.insertList(pInfo);
		pInfo = m_AudioPackBuf.getInfoFromList();
	}

	pInfo = m_PcmPackBuf.getInfoFromList();
	while (pInfo)//事件等待
	{
		m_FreePcmPackBuf.insertList(pInfo);
		pInfo = m_PcmPackBuf.getInfoFromList();
	}
	std::lock_guard<std::recursive_mutex> lock(m_mutexAudioBuf);
	//	m_pcmPos = 0;
	//	m_pcmLen = 0;
	memset(&m_remainPCM, 0, sizeof(T_RemainPCM));
}

void ffplayer::FlushVideoBuf()
{
	m_videoPacketBuf.destoryBuffer();

	PInfo pInfo = m_renderBuf.getInfoFromList();
	while (pInfo)//事件等待
	{
		m_freeRenderBuf.insertList(pInfo);
		pInfo = m_renderBuf.getInfoFromList();
	}
}

float ffplayer::GetPlayPos()
{
	float fPos = 0;

	if (m_bCanSeek)
	{
		fPos = GetPlayedTime() / ((float)m_pFormatCtx->duration / AV_TIME_BASE);
	}
	else
	{
		if (m_frameCount != 0 && m_bReadFrameFinish)
		{
			fPos = (float)m_curFrameCount / (float)m_frameCount;
		}
	}
	return fPos;
}

DWORD ffplayer::GetFileTime()
{
	if (m_bCanSeek)
	{
		return m_pFormatCtx->duration / AV_TIME_BASE;
	}
	else
	{
		if (m_fps>0 && m_bReadFrameFinish)
		{		
			return m_frameCount / m_fps;
		}
	}

	return 0.0;
}

DWORD ffplayer::GetPlayedTime()
{
	if (m_bCanSeek)
	{
		if (m_audclk.pts != 0 && m_bHasAudio&&g_Speed[m_iSpeed]==1&&!m_bPause)
		{
			return m_audclk.pts;
		
		}
		else
		{
			return m_curTime;
		}
	}
	else
	{
		if (m_fps&& m_bReadFrameFinish)
		{
			return	(m_curFrameCount + m_fps )/ (float)m_fps;
		}
	}
}

BOOL  ffplayer::GetPictureSize(LONG *pWidth, LONG *pHeight)
{
	if (m_iWidth != 0 && m_iHeight != 0)
	{
		*pWidth = m_iWidth;
		*pHeight = m_iHeight;
		return true;
	}
	return false;
}


BOOL  ffplayer::CapturePic(char *pSaveFile, int iType)
{
	if (m_iWidth == 0 || m_iHeight == 0 || !m_picSnopshot)
	{
		return false;
	}

	unsigned char *buf = NULL;

	if (NULL == buf)
	{
		buf = (unsigned char*)malloc(m_iWidth*m_iHeight * 3);

	}

	AVFrame dstFrame;

	if (NULL != buf)
	{
		av_image_fill_arrays(dstFrame.data, dstFrame.linesize, (uint8_t*)m_picSnopshot->Data, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 1);
		YUV420_RGB_KP(&dstFrame, buf, m_iWidth, m_iHeight);//YUV420_RGB_KP是内部函数
		
	}

	if (strstr(pSaveFile, ".jpg") || strstr(pSaveFile, ".JPG") || strstr(pSaveFile, ".jpeg"))
	{
		writeJPGFile(pSaveFile, (char*)buf, m_iWidth, m_iHeight);
	}
	else
	{
		writeBMPFile(pSaveFile, (char*)buf, m_iWidth, m_iHeight);
	}

	if (buf)
	{
		free(buf);
		buf = NULL;
	}
	return true;
}

bool ffplayer::SetFileEndCallback(long nID,FileEndCallback fileCallBack, void *pUser)
{
	m_fileEndBak = fileCallBack;
	m_pUser = pUser;
	m_nID = nID;
	return true;
}

void ffplayer::DemuxThread(void*lPalam)
{
	try
	{
		ffplayer*pPlayer = (ffplayer*)lPalam;
		if (pPlayer)
		{
			pPlayer->DemuxThreadRun();
		}
	}
	catch (...)
	{
	}
}

void ffplayer::VideoDecoderThread(void*lPalam)
{
	try
	{
		ffplayer*pPlayer = (ffplayer*)lPalam;
		if (pPlayer)
		{
			pPlayer->VideoDecoderThreadRun();
		}
	}
	catch (...)
	{
	}
}


void ffplayer::RenderThread(void*lPalam)
{
	try
	{
		ffplayer*pPlayer = (ffplayer*)lPalam;
		if (pPlayer)
		{
			pPlayer->RenderThreadRun();
		}
	}
	catch (...)
	{
	}
}

void ffplayer::AudioDecoderThread(void*lPalam)
{
	try
	{
		ffplayer*pPlayer = (ffplayer*)lPalam;
		if (pPlayer)
		{
			pPlayer->AudioDecoderThreadRun();
		}
	}
	catch (...)
	{
	}
}

void ffplayer::ReadThread(void*lPalam)
{
	try
	{
		ffplayer*pPlayer = (ffplayer*)lPalam;
		if (pPlayer)
		{
		//	SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
			pPlayer->ReadThreadRun();
		}
	}
	catch (...)
	{
	}
}
void ffplayer::ReadThreadRun()
{
	FILE *fd=NULL;
	int error=fopen_s(&fd,m_strFileName, "rb");
	if (fd)
	{
		fseek(fd, 0, SEEK_END);
		m_fileSize=ftell(fd);
		fclose(fd);
	}
	//std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
	AVFormatContext *pFormatCtx=NULL;
	//AVCodecContext  *pCodecCtx;

	// Open the video file
	int ret = avformat_open_input(&pFormatCtx, m_strFileName, NULL, NULL);
	if (ret < 0)
	{
		//m_errorMsg = "Could not open the video file";
		//Close();
		if (pFormatCtx)
		{
			avformat_close_input(&pFormatCtx);
		}
		return ;
	}

	// Retrieve the stream information
	if (avformat_find_stream_info(pFormatCtx, 0) < 0)
	{
		//m_errorMsg = "Could not find the stream information";
		if (pFormatCtx)
		{
			avformat_close_input(&pFormatCtx);
		}
		return ;
	}
	m_frameCount = 0;
	while (1){
		if (m_bStop){
			break; 
		}
		AVPacket packet;		
	//	std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
		int ret = av_read_frame(pFormatCtx, &packet);
		if (ret == 0){
			if (packet.stream_index == m_videoStream){
			//	int pts = packet.duration/AV_TIME_BASE;

				m_frameCount++;
				av_packet_unref(&packet);
			}
		}else{
			break;
		}
	}
	if (pFormatCtx)
	{
		avformat_close_input(&pFormatCtx);
	}
	m_curFrameCount = m_fSeekRelativePos*m_frameCount;
	m_curFrameCount += m_tmpFrameCount;
	m_bReadFrameFinish = true;
}

void  ffplayer::SDL_Fill_audio(void *userdata, Uint8 * stream, int len)
{
	ffplayer *pPlayer = (ffplayer*)userdata;
	{
		pPlayer->FillAudio(stream, len);
	}
}


void ffplayer::FillAudio(Uint8 * stream, int len)
{
	SDL_memset(stream, 0, len);
	if (m_bPause || m_iSpeed != 4)//暂停或速率不等于1时不用播放音频
	{
		return;
	}

	int tempLen = 0;
	if (m_bSeekState)
	{
		return;
	}
	double audioClock = 0;
	int serial = -1;
	{
		std::lock_guard<std::recursive_mutex> lock(m_mutexAudioBuf);

		if (m_remainPCM.len > 0)
		{			
		
			if (m_remainPCM.serial != m_serial)
			{
				memset(&m_remainPCM, 0, sizeof(m_remainPCM));
				return;
			}
			int dstLen = m_remainPCM.len > len ? len : m_remainPCM.len;		
			memcpy(stream, m_remainPCM.buf, dstLen);
			tempLen += dstLen;
			m_remainPCM.len -= dstLen;
			serial = m_remainPCM.serial;
			audioClock = m_remainPCM.pts * av_q2d(m_pFormatCtx->streams[m_audioStream]->time_base) + (double)m_remainPCM.size / (double)m_bytes_per_sec;
			if (m_remainPCM.len > 0)
			{
				memmove(m_remainPCM.buf, m_remainPCM.buf + dstLen, m_remainPCM.len);				

			}
			
		}
	}
	
	while (tempLen < len)
	{
		if ((m_bPause &&!m_bPausedSeek) || m_iSpeed != 4 || m_bStop)//暂停或速率不等于1时不用播放音频
		{
			return;
		}
		PInfo pInfo = m_PcmPackBuf.getInfoFromList();
		if (!pInfo)
		{
			av_usleep(1000);
			continue;
		}
		if (pInfo->frameInfo.serial != m_serial)
		{
			SDL_memset(stream, 0, len);
			m_FreePcmPackBuf.insertList(pInfo);
			continue;
		}
		int dstLen = (len - tempLen) > pInfo->DataLen ? pInfo->DataLen : len - tempLen;
		memcpy(stream + tempLen, pInfo->Data, dstLen);
		tempLen += dstLen;
		if (pInfo->DataLen > dstLen)
		{
			memcpy(m_remainPCM.buf, pInfo->Data + dstLen, pInfo->DataLen - dstLen);
			m_remainPCM.len = pInfo->DataLen - dstLen;
			m_remainPCM.pts = pInfo->frameInfo.iTimestampObsolute;
			m_remainPCM.size = pInfo->DataLen;
			m_remainPCM.serial = pInfo->frameInfo.serial;
			
		}

		serial = pInfo->frameInfo.serial;
	
		audioClock = (pInfo->frameInfo.iTimestampObsolute == AV_NOPTS_VALUE) ? NAN : pInfo->frameInfo.iTimestampObsolute * av_q2d(m_pFormatCtx->streams[m_audioStream]->time_base) + (double)pInfo->DataLen / (double)m_bytes_per_sec;
		m_FreePcmPackBuf.insertList(pInfo);
	}

	set_clock(&m_audclk, audioClock - (double)(2 * m_hw_size + m_remainPCM.len) / (double)m_bytes_per_sec, serial);

	if (m_bSilence)
	{
		SDL_memset(stream, 0, len);
	}
	/*
	char buf[1024];
	memset(buf, 0, 1024);
	sprintf_s(buf, "pts:%d,audioClock:%f\n", m_remainPCM.pts, audioClock - (double)(2 * m_hw_size + m_remainPCM.len) / (double)m_bytes_per_sec);
	USES_CONVERSION;
	::OutputDebugString(A2T(buf));
	*/

}

void ffplayer::decode(AVPacket &packet,int &got,int bFlush)
{ 
	std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
	if (bFlush)
	{
		avcodec_flush_buffers(m_pCodecCtx);
	}
	avcodec_decode_video2(m_pCodecCtx, m_pFrame, &got, &packet);
}

void ffplayer::VideoDecoderThreadRun()
{
	bool bFirst = true;
	while (1)
	{
		SDL_Delay(1);
		if (m_bStop){
			break;
		}
		
		PInfo pFrameInfo = m_videoPacketBuf.getInfoFromList();
		if (!pFrameInfo)//事件等待
		{
			continue;
		}

		AVPacket packet;
		av_init_packet(&packet);
		packet.data = pFrameInfo->Data;
		packet.size = pFrameInfo->DataLen;
		packet.pts = pFrameInfo->frameInfo.iTimestampObsolute;

		int got = 0;	
		decode(packet, got, pFrameInfo->frameInfo.bNewPos);

		if (got)
		{	
			int size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 1);
			PInfo pYuvInfo = NULL;
			int count = m_freeRenderBuf.getCurCount(); //为了暂停后抓图，指针指向的pInfo没有被覆盖，保留最后一帧
			if (count == 1)
			{
			//	pYuvInfo = CBuffer::createBuf(m_pCodecCtx->width* m_pCodecCtx->height * 3 / 2);
			//	pYuvInfo = CBuffer::createBuf(m_pFrame->linesize[0]* m_pCodecCtx->height * 3 / 2);
				pYuvInfo = CBuffer::createBuf(size);
			}
			else
			{
				//pYuvInfo = m_freeRenderBuf.getFreeInfoFromList(m_pCodecCtx->width* m_pCodecCtx->height * 3 / 2);
				//pYuvInfo = m_freeRenderBuf.getFreeInfoFromList(m_pFrame->linesize[0] * m_pCodecCtx->height * 3 / 2);;
				pYuvInfo = m_freeRenderBuf.getFreeInfoFromList(size);
			}
				

			AVFrame dstFrame;
			av_image_fill_arrays(dstFrame.data, dstFrame.linesize, (uint8_t*)pYuvInfo->Data, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 1);
			
			if (m_pFrame->format != AV_PIX_FMT_YUV420P)
			{			
				struct SwsContext* img_convert_ctx = NULL;
				img_convert_ctx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, (AVPixelFormat)m_pFrame->format,
					m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);


				int n = sws_scale(img_convert_ctx, m_pFrame->data, m_pFrame->linesize,
					0, m_pCodecCtx->height, dstFrame.data, dstFrame.linesize);
				sws_freeContext(img_convert_ctx);
			}
			else
			{
				av_image_copy(dstFrame.data, dstFrame.linesize, (const uint8_t **)m_pFrame->data, m_pFrame->linesize, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);
			}
					
			pYuvInfo->width = m_pCodecCtx->width;
			pYuvInfo->height = m_pCodecCtx->height;
			m_iWidth = m_pCodecCtx->width;
			m_iHeight = m_pCodecCtx->height;
			double pts= av_frame_get_best_effort_timestamp(m_pFrame);//av_frame_get_best_effort_timestamp
			
			pYuvInfo->frameInfo.iTimestampObsolute = pts;
			pYuvInfo->frameInfo.bNewPos = pFrameInfo->frameInfo.bNewPos;
			pYuvInfo->frameInfo.serial = pFrameInfo->frameInfo.serial;

			while (m_renderBuf.getCurCount() >= MAX_YUV_SIZE)
			{
				SDL_Delay(10);
				if (m_bStop){
					CBuffer::freeBuf(&pYuvInfo);
					CBuffer::freeBuf(&pFrameInfo);
					return;
				}
			}

			m_renderBuf.insertList(pYuvInfo);
			
		}
		
		CBuffer::freeBuf(&pFrameInfo);
	}//while
}

void ffplayer::AudioDecoderThreadRun()
{
	while (1){
		SDL_Delay(10);
		if (m_bStop){
			break;
		}
		PInfo pInfo = m_AudioPackBuf.getInfoFromList();
		if (!pInfo)//事件等待
		{
			continue;
		}
		
		AVPacket packet;
		av_init_packet(&packet);
		packet.data = pInfo->Data;
		packet.size = pInfo->DataLen;

		int got;
		int len=avcodec_decode_audio4(m_pAudioCodecCtx, m_pAudioFrame, &got, &packet);

		if (len > 0&&got)
		{
			Uint8 resampleBuf[PCMBUFLEN];

			//	int decodeLen = AudioResampling(m_pAudioCodecCtx, m_pAudioFrame, (Uint8*) resampleBuf);

			if (!m_au_convert_ctx){
				m_au_convert_ctx = swr_alloc();
				int channel_layout = av_get_default_channel_layout(m_pAudioCodecCtx->channels);

				m_au_convert_ctx = swr_alloc_set_opts(m_au_convert_ctx, channel_layout, AV_SAMPLE_FMT_S16, m_pAudioCodecCtx->sample_rate,
					channel_layout, m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0, NULL);
				swr_init(m_au_convert_ctx);

				//	av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, m_pAudioCodecCtx->channels,
				//	2048, m_pAudioCodecCtx->sample_fmt, 0);
			}
			Uint8 *dst_data[] = { resampleBuf };
			int ret = swr_convert(m_au_convert_ctx, (Uint8**)dst_data, PCMBUFLEN, (const uint8_t **)m_pAudioFrame->data, m_pAudioFrame->nb_samples);
			int decodeLen = ret* m_out_channels * av_get_bytes_per_sample((AVSampleFormat)m_out_sample_fmt);
			if (m_fVolume == 0.0)
			{
				memset(resampleBuf, 0, decodeLen);
			}
			else if (m_fVolume>0.0&&m_fVolume<1.0)
			{
				short *pcm = (short*)resampleBuf;
				for (int i = 0; i < decodeLen / 2; i++)
				{
					*pcm = (*pcm)*m_fVolume;
					pcm++;
				}
			}

			PInfo pPcmInfo = m_FreePcmPackBuf.getFreeInfoFromList(decodeLen);

			memcpy(pPcmInfo->Data, resampleBuf, decodeLen);
			pPcmInfo->DataLen = decodeLen;
			pPcmInfo->frameInfo.iTimestampObsolute = pInfo->frameInfo.iTimestampObsolute;
			pPcmInfo->frameInfo.serial = pInfo->frameInfo.serial;
			/*
			AVRational tb;
				tb.num = 1;
			tb.den = m_pAudioFrame->sample_rate;
			//(AVRational){ 1, m_pAudioFrame->sample_rate };
			if (m_pAudioFrame->pts != AV_NOPTS_VALUE)
				m_pAudioFrame->pts = av_rescale_q(m_pAudioFrame->pts, m_pAudioCodecCtx->time_base, tb);
			else if (m_pAudioFrame->pkt_pts != AV_NOPTS_VALUE)
				m_pAudioFrame->pts = av_rescale_q(m_pAudioFrame->pkt_pts, av_codec_get_pkt_timebase(m_pAudioCodecCtx), tb);
			//pPcmInfo->frameInfo.iTimestampObsolute = m_pAudioFrame->pkt_pts;*/
			
			while (m_PcmPackBuf.getCurCount() >= MAX_PCM_SIZE)
			{
				SDL_Delay(10);
				if (m_bStop){
					CBuffer::freeBuf(&pPcmInfo);
					return;
				}
			}
			if (m_bSeekState || pPcmInfo->frameInfo.serial!=m_serial)
			{
				m_FreePcmPackBuf.insertList(pPcmInfo); 
			}
			else
			{
				m_PcmPackBuf.insertList(pPcmInfo);
			}

		}
		m_FreeAudioPackBuf.insertList(pInfo);	
	}
}

void ffplayer::DemuxThreadRun()
{
	bool bNewPos = false;
	while (1){
		SDL_Delay(1);
		if (m_bStop){
			break;
		}
		if (m_bSeekState){
			SDL_Delay(10);
			FlushAudioBuf();
			FlushVideoBuf();
			m_serial++;
			if (m_bPause)
			{
				m_bPausedSeek = true;
			}
			
			if (m_bCanSeek)
			{
				std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
				double time = m_fSeekRelativePos*m_pFormatCtx->duration / AV_TIME_BASE;//转为时间秒

				//	int64_t timestamp = (int64_t)(time * AV_TIME_BASE * av_q2d(m_pCodecCtx->time_base));//失败
				//int64_t timestamp = (int64_t)(time  *AV_TIME_BASE *av_q2d(m_pFormatCtx->streams[m_videoStream]->time_base));//失败
				int64_t timestamp = (int64_t)(time  * AV_TIME_BASE);//转为又AV_TIME_BASE
				//		int ret = av_seek_frame(m_pFormatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);//-1表示以AV_TIME_BASE为基准，否则以指定流的基准
				//	int ret = av_seek_frame(m_pFormatCtx, -1, timestamp, AVSEEK_FLAG_ANY);//会花屏
				int ret = av_seek_frame(m_pFormatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
				if (ret >= 0){
					//	avcodec_flush_buffers(m_pCodecCtx);//在解码线程调用，此处调用会导致缓冲中的解码花屏
				}
				m_bSeekState = false;
				bNewPos = true;
			}
			else{
				std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
				int ret = av_seek_frame(m_pFormatCtx, -1, m_fSeekRelativePos*m_fileSize, AVSEEK_FLAG_BYTE);//-1表示以AV_TIME_BASE为基准，否则以指定流的基准
				//	int ret = av_seek_frame(m_pFormatCtx, -1, m_fSeekRelativePos*m_frameCount, AVSEEK_FLAG_BACKWARD);
				//int ret = av_seek_frame(m_pFormatCtx, -1, m_fSeekRelativePos*m_frameCount, AVSEEK_FLAG_FRAME);
				//	int ret = avformat_seek_file(m_pFormatCtx, -1, 0, m_fSeekRelativePos*m_frameCount, m_frameCount, AVSEEK_FLAG_FRAME);
				//AVSEEK_FLAG_FRAME
				if (ret >= 0){
					//avcodec_flush_buffers(m_pCodecCtx);
					if (m_bReadFrameFinish)
					{
						m_curFrameCount = m_fSeekRelativePos*m_frameCount;
					}
				}
				m_bSeekState = false;
				bNewPos = true;
			}
		
		}
		AVPacket packet;
		av_init_packet(&packet);
		int ret = 1;
		{
			//	std::lock_guard<std::recursive_mutex> lock(g_mutexFFmpeg);
			ret = av_read_frame(m_pFormatCtx, &packet);
		}
	
		if (ret != 0)//读取失败，
		{
			if (ret == AVERROR_EOF || avio_feof(m_pFormatCtx->pb))//说明读完了
			{
				m_bFileEnd = true;
				continue;
			}
			if (m_pFormatCtx->pb && m_pFormatCtx->pb->error)//出错
			{
				continue;
			}

		}

		if (ret == 0)//读到一帧解码
		{
			if (packet.stream_index == m_videoStream)
			{
				/*
				if (bNewPos)
				{
					if (!packet.flags&AV_PKT_FLAG_KEY)
					{
						av_packet_unref(&packet);
						continue;
					}

				}*/

				while (m_videoPacketBuf.getCurCount()>=MAX_SIZE)
				{
					if (m_bStop){
						av_packet_unref(&packet);
						return;
					}
					SDL_Delay(10);
				}

				PInfo pPacketInfo = pPacketInfo = CBuffer::createBuf(packet.size);

				packet.pts = packet.pts == AV_NOPTS_VALUE ? packet.dts : packet.pts;
				
				pPacketInfo->frameInfo.iTimestampObsolute = packet.pts;
				pPacketInfo->frameInfo.serial = m_serial;
				pPacketInfo->frameInfo.bNewPos = bNewPos;
				bNewPos = false;
			
				memcpy(pPacketInfo->Data, packet.data, packet.size);
				pPacketInfo->DataLen = packet.size;
				
				m_videoPacketBuf.insertList(pPacketInfo);
				av_packet_unref(&packet);

			}
			else if (packet.stream_index == m_audioStream&&m_bHasAudio)
			{
				if (m_iSpeed != 4)//暂停或速率不等于1时不用播放音频
				{
					av_packet_unref(&packet);
					continue;
				}
				/*
				while (m_bPause)
				{
					if (m_bStop){
						av_packet_unref(&packet);
						return;
					}
					Sleep(1);
					continue;
				}*/

				while (m_AudioPackBuf.getCurCount() >= MAX_SIZE)
				{
					if (m_bStop){
						av_packet_unref(&packet);
						return;
					}
					SDL_Delay(1);
				}
				PInfo pPacketInfo = m_FreeAudioPackBuf.getFreeInfoFromList(packet.size);
				pPacketInfo->frameInfo.iTimestampObsolute = packet.pts;			
				memcpy(pPacketInfo->Data, packet.data, packet.size);
				pPacketInfo->DataLen = packet.size;
				pPacketInfo->frameInfo.serial = m_serial;

				
				m_AudioPackBuf.insertList(pPacketInfo);
				av_packet_unref(&packet);
			}
			else//其它的包
			{
				av_packet_unref(&packet);
			}
		}
	}
}

int  ffplayer::Async(__int64 &pts)//同步
{
	int dif = 0;
	if (m_bCanSeek)
	{

		float vd = pts - m_curPTS;
	/*	char buf[1024];
		sprintf_s(buf, "pts:%d\n", pts);
		USES_CONVERSION;
		::OutputDebugString(A2T(buf));
		sprintf_s(buf, "dif1:%f\n", dif1);
		::OutputDebugString(A2T(buf));*/
		double duation = vd* av_q2d(m_pFormatCtx->streams[m_videoStream]->time_base);


		if (g_Speed[m_iSpeed] == 1 && m_bHasAudio&&m_audclk.pts != 0.0)//速率为1要音视频同步，否则不用
		{
			double delay = compute_target_delay(duation);
			dif = delay*1000.0;
		/*	sprintf_s(buf, "dif2:%d\n", dif);
			::OutputDebugString(A2T(buf));*/
		}else
			dif = duation* 1000;
				
	}else
	{
		dif = (float)1000 / (float)m_fps;
	
	}

	int iSpeed = g_Speed[m_iSpeed];
	if (iSpeed > 0)
	{
		dif /= iSpeed;
	}
	else{
		dif *= abs(iSpeed);
	}

	if (dif > 0)
	{
		//还要计算耗时
		unsigned __int64 iCurSystemTime = ::GetTickCount();
		int difSys = iCurSystemTime - m_iLastSystemTime;
	/*	char buf[1024];
		memset(buf, 0, 1024);
		sprintf_s(buf, "dif:%f,difSys:%d\n",dif,difSys);
		USES_CONVERSION;
		::OutputDebugString(A2T(buf));*/

		dif = dif - difSys;
		if (dif > 0)
		{
		//	Sleep(dif);//毫秒
			av_usleep(dif*1000);//微秒
		}
	}
	
	return dif;
}

bool ffplayer::CreateRender()
{
	std::lock_guard<std::recursive_mutex> lock(g_mutexSDL);
	m_sdlWindow = SDL_CreateWindowFrom((void*)m_hwnd);

	//	SDL_ShowWindow(sdlWindow);
		//获取当前可用画图驱动 window中有3个，第一个为d3d，第二个为opengl，第三个为software
		//创建渲染器，第二个参数为选用的画图驱动，0代表d3d
	if (m_sdlWindow)
	 m_pRender = SDL_CreateRenderer(m_sdlWindow, 0, SDL_RENDERER_ACCELERATED);

   return true;
}

void ffplayer::RefreshRender()
{
	if (m_pRender&&m_pTexture)
	{
		SDL_RenderPresent(m_pRender);
	}
}

void ffplayer::DestoryRender()
{
	std::lock_guard<std::recursive_mutex> lock(g_mutexSDL);
	if (m_pTexture != NULL)
	{
		SDL_DestroyTexture(m_pTexture);
		m_pTexture = NULL;
	}

	if (m_pRender != NULL)
	{
		//	SDL_RenderClear(pRender);
		SDL_DestroyRenderer(m_pRender);
		m_pRender = NULL;
	}
	if (NULL != m_sdlWindow)
	{
		//	SetWindowLongPtr
		//SDL_RestoreWindow(sdlWindow);
		//	SDL_DestroyWindow(sdlWindow);
		//	sdlWindow = NULL;

	}
}

void ffplayer::UpdateVideoPts(PInfo pYuvInfo)
{
	//转换为UI上显示的时间
	m_curTime = pYuvInfo->frameInfo.iTimestampObsolute*av_q2d(m_pFormatCtx->streams[m_videoStream]->time_base);

	m_curPTS = pYuvInfo->frameInfo.iTimestampObsolute;//没转换的
	m_lastserial = pYuvInfo->frameInfo.serial;

	set_clock(&m_vidclk, m_curTime, m_lastserial);
	m_iLastSystemTime = ::GetTickCount();
}

void ffplayer::RenderThreadRun()
{
	CreateRender();

	int iWidth = 0;
	int iHeight = 0;

	bool bFirst = true;
	while (1)
	{
		Sleep(10);
		if (m_bStop)
			break;
		
		/*
		if (bFirst)
		{
			if (m_renderBuf.getCurCount() < 2)//这里会导致音乐.mp3的画面没显示
			{
				continue;
			}
		}*/
		if (m_bStepNext&&m_bStepEnd){
			RefreshRender();
			continue;
		}
		if (m_bPause &&!m_bPausedSeek){		
			RefreshRender();
			continue;
		}
	
		PInfo pYuvInfo = m_renderBuf.getInfoFromList();
		if (!pYuvInfo){
			RefreshRender();
			if (m_bFileEnd&&m_fileEndBak){
				m_fileEndBak(m_nID, m_pUser);
				break;
			}
			continue;
		}else{
			if (bFirst){
				bFirst = false;
			}else{
				
				if (pYuvInfo->frameInfo.serial == m_serial/*&&!m_bSeekState&&m_serial == m_audclk.serial*/)//暂停后再seek后，声音暂停了m_audclk.serial还是原来的，不等于m_serial
				{
					if (pYuvInfo->frameInfo.serial == m_lastserial){
					//	if (m_serial == m_audclk.serial)//有这句会导致无音频的视频不会Async
						int dif = Async(pYuvInfo->frameInfo.iTimestampObsolute);
					/*	if (dif < 0)
						{
							UpdateVideoPts(pYuvInfo);
							m_freeRenderBuf.insertList(pYuvInfo);
							continue;
						}*/
						if (m_bPausedSeek)
							m_bPausedSeek = false;

					 }else{
						
						UpdateVideoPts(pYuvInfo);
						m_freeRenderBuf.insertList(pYuvInfo);//只更新pts和serial，不显示，显示会花屏
						continue;
					}
				}else{
					m_freeRenderBuf.insertList(pYuvInfo);//另一个系列不用显示，忽略
					continue;		
				}
			}
		//	m_freeRenderBuf.insertList(pYuvInfo);//另一个系列不用显示，忽略
		//	continue;
			int w, h;
			RECT rc;
			GetWindowRect(m_hwnd, &rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
			if (iWidth != w || iHeight != h){
				iWidth = w;
				iHeight = h;
				SDL_SetWindowSize(m_sdlWindow, w, h);
			}

			if (m_pTexture == NULL)
				m_pTexture = SDL_CreateTexture(m_pRender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pYuvInfo->width, pYuvInfo->height);


			//计算yuv一行数据占的字节数
			int iPitch = pYuvInfo->width*SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_IYUV);

			int i = SDL_UpdateTexture(m_pTexture, NULL, pYuvInfo->Data, iPitch);

			SDL_RenderClear(m_pRender);
			SDL_RenderCopy(m_pRender, m_pTexture, NULL, NULL);
			SDL_RenderPresent(m_pRender);

			if (m_bReadFrameFinish)
				m_curFrameCount++;
			else
				m_tmpFrameCount++;			

			m_picSnopshot = pYuvInfo;

			UpdateVideoPts(pYuvInfo);
			m_freeRenderBuf.insertList(pYuvInfo);
			m_bStepEnd = true;		
		}
	}

	DestoryRender();	
}
/*
BOOL ffplayer::FileCutStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime)
{
	return false;
}

BOOL ffplayer::FileCutClose()
{
	return false;
}

int ffplayer::FileCutProcess()
{
	return 0;
}

BOOL ffplayer::FileCutPause(DWORD nPause)
{
	return false;
}*/

BOOL ffplayer::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{	
	if (m_pFileConverter)
		FileConvertClose();
	
	m_pFileConverter = new CFileConverter;
	return m_pFileConverter->FileConvertStart(srcFileName, destFileName, startTime, endTime, bConvert, pConvertPara);

}

BOOL ffplayer::FileConvertClose()
{
	if (m_pFileConverter)
		m_pFileConverter->FileConvertClose();

	if (m_pFileConverter){
		delete m_pFileConverter;
		m_pFileConverter = NULL;
	}
	return true;
}

int ffplayer::FileConvertProcess()
{
	if (m_pFileConverter)
		return m_pFileConverter->FileConvertProcess();
	return 0;
}

BOOL ffplayer::FileConvertPause(DWORD nPause)
{
	if (m_pFileConverter)
		return m_pFileConverter->FileConvertPause(nPause);

	return false;
}