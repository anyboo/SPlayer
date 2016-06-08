#include "stdafx.h"
#include <string.h>
#include "FileConverter.h"

#define AUDIO_BUFF_MAX_SIZE (192000)


CFileConverter::CFileConverter() :m_bWriteHeader(false)
{
	m_bVideoValid = true;
	m_bAudioValid = true;

	m_formatContext = 0;
	m_videoStream = 0;
	m_audioStream = 0;
	m_transcodeCodec = 0;
	m_audioDecodedFrame = NULL;
	m_toEncframe = NULL;

	m_swr_ctx = NULL;

	m_audioInputBuffSize = 0;
	m_audioInputBuff = new char[AUDIO_BUFF_MAX_SIZE];

	m_audioOutputSamples = NULL;
	m_samples_size = 0;

	m_bWriteHeader = false;

	m_bCanSeek = false;
	m_videoIndex = -1;
	m_audioIndex = -1;
	m_bReadFrameFinish = false;
	m_frameTotalCount = 0;
	m_bStop = false;
	m_progress = 0;
	m_startTime = 0;
	m_stopTime = 0;
}


CFileConverter::~CFileConverter()
{
	if (m_audioInputBuff)
	{
		delete m_audioInputBuff;
	}
}

BOOL CFileConverter::FileConvertStart(const char* srcFileName, const char* destFileName, unsigned __int64 startTime, unsigned __int64 endTime, bool bConvert, T_ConverterParameters *pConvertPara)
{
	bool bRet = OpenSrcFile((char*)srcFileName);
	if (!bRet)
	{
		return false;
	}

	bRet = OpenDstFile((char*)destFileName);
	if (!bRet)
	{
		return false;
	}
	m_startTime = startTime;
	m_stopTime = endTime;
	/*
	if (!m_bCanSeek)
	{
		int status;
		m_getTotalFrameThread = SDL_CreateThread((SDL_ThreadFunction)GetTotalFrameThread, NULL, this);
		Sleep(200);

	}*/
	m_convertThread = SDL_CreateThread((SDL_ThreadFunction)ConvertThread, NULL, this);

	return true;
}


BOOL CFileConverter::FileConvertClose()
{
	m_bStop = true;
	int status = 0;
	SDL_WaitThread(m_convertThread, &status);
	CloseDstFile();
	CloseSrcFile();
	return true;
}

int CFileConverter::FileConvertProcess()
{
	return m_progress;
}

BOOL CFileConverter::FileConvertPause(DWORD nPause)
{
	return 0;
}


void CFileConverter::GetTotalFrameThread(void *pPara)
{
	try
	{
		CFileConverter*pConverter = (CFileConverter*)pPara;
		if (pConverter)
		{
			pConverter->GetTotalFrameThreadRun();
		}
	}
	catch (...)
	{
	}
}

void CFileConverter::ConvertThread(void *pPara)
{
	try
	{
		CFileConverter*pConverter = (CFileConverter*)pPara;
		if (pConverter)
		{
			pConverter->ConvertThreadRun();
		}
	}
	catch (...)
	{
	}
}
/*
int CFileConverter::PushVideoFrame(const unsigned char *pVideoData, int len, __int64 istamp, const int frameType)
{
}*/
int CFileConverter::PushVideoFrame(AVPacket &inPacket)
{

	return 0;
}

int CFileConverter::PushAudioFrame(AVPacket &inPacket)
{

	return 0;
}

// 打开音频转码context
AVCodecContext* CFileConverter::OpenAudioTranscode(enum AVCodecID audioCodec)
{
	AVCodecContext *cc;
	AVCodec* codec;

	/* find the audio decoder */
	codec = avcodec_find_decoder(audioCodec);
	if (!codec)
	{
		return 0;
	}

	cc = avcodec_alloc_context3(codec);

	/* put sample parameters */
	cc->sample_fmt = AV_SAMPLE_FMT_S16;
	cc->bit_rate = 64000;
	if (m_dstConvertPara.samplerate == 0)//按默认的，应该等于原来的
	{
		//m_iSampleRate = DEFAULT_AUDIO_SAMPLE;
		cc->sample_rate = m_srcFileInfo.samplerate;
	}
	else{
		cc->sample_rate = m_dstConvertPara.samplerate;
	}

	cc->channel_layout = AV_CH_LAYOUT_MONO;
	if (m_dstConvertPara.channel == 0)
	{
		cc->channels = m_srcFileInfo.channel;
	}
	else{
		cc->channels = m_dstConvertPara.channel; //1:mono,单声道
	}
	cc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// 打开解码器
	if (OpenCodec(codec, cc) < 0)
	{
		return 0;
	}
	return cc;
}

void  CFileConverter::CloseDstFile()
{

	/* Write the trailer, if any. The trailer must be written before you
	* close the CodecContexts open when you wrote the header; otherwise
	* av_write_trailer() may try to use memory that was freed on
	* av_codec_close(). */
	if (m_bWriteHeader)
	{
		int ret = 0;
		if ((ret = av_write_trailer(m_formatContext)) != 0)
		{
			PrintError(ret);
		}
	}

	//if(pFile)
	//{
	//	fclose(pFile);
	//	pFile=NULL;
	//}
	/* Close each codec. */
/*	if (m_videoStream&&m_videoStream->codec)
	{
		avcodec_close(m_videoStream->codec);
	}*/
	if (m_audioStream)
	{
		avcodec_close(m_audioStream->codec);
	}
	if (m_transcodeCodec)
	{
		avcodec_close(m_transcodeCodec);
	}

	/* Free the streams. */
	int i = 0;
	if (m_formatContext)
	{
		for (; i < m_formatContext->nb_streams; i++)
		{
			av_freep(&m_formatContext->streams[i]->codec);
			av_freep(&m_formatContext->streams[i]);
		}

		if (!(m_formatContext->oformat->flags & AVFMT_NOFILE))
		{
			/* Close the output file. */
			avio_close(m_formatContext->pb);
		}

		/* free the stream */
		av_free(m_formatContext);
	}

	if (m_audioOutputSamples)
	{
		delete[] m_audioOutputSamples;
		m_audioOutputSamples = 0;
	}

	if (m_audioInputBuff)
	{
		delete[] m_audioInputBuff;
		m_audioInputBuff = 0;
	}

	
	if (m_toEncframe)
	{
		av_free(&m_toEncframe);
	}

	if (m_swr_ctx)
	{
		swr_free(&m_swr_ctx);
		m_swr_ctx = NULL;
	}

}

void  CFileConverter::CloseSrcFile()
{
	if (m_pVideoFrame)
		av_free(m_pVideoFrame);

	// Close the codec
	if (m_pSrcVideoCodecCtx)
		avcodec_close(m_pSrcVideoCodecCtx);

	m_pSrcVideoCodecCtx = 0;
	m_pVideoFrame = 0;
	m_videoIndex = -1;

	//音频
	if (m_audioDecodedFrame)
	{
		av_free(&m_audioDecodedFrame);
	}

	if (m_pSrcAudioCodecCtx)
		avcodec_close(m_pSrcAudioCodecCtx);

	m_pSrcAudioCodecCtx = NULL;
	m_audioDecodedFrame = NULL;
	m_videoIndex = -1;

	// Close the video file
	if (m_srcFormatContext)
		avformat_close_input(&m_srcFormatContext);
	
	m_srcFormatContext = 0;

}

bool CFileConverter::OpenSrcFile(char *pSrcFile)
{
	av_register_all();

	// Open the video file
	int ret = avformat_open_input(&m_srcFormatContext, pSrcFile, NULL, NULL);
	if (ret < 0)
	{
		printf("Could not open the video file:%s", pSrcFile);
		return false;
	}

	// Retrieve the stream information
	if (avformat_find_stream_info(m_srcFormatContext, 0) < 0)
	{
		//m_errorMsg = "Could not find the stream information";
		CloseSrcFile();
		return false;
	}

	// Find the first video stream

	for (unsigned i = 0; i < m_srcFormatContext->nb_streams; i++)
	{
		if (m_srcFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoIndex = i;
			m_pSrcVideoCodecCtx = m_srcFormatContext->streams[i]->codec;

		}
		else if (m_srcFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioIndex = i;
			m_pSrcAudioCodecCtx = m_srcFormatContext->streams[i]->codec;
		}
	}

	if (m_videoIndex == -1 && m_audioIndex == -1)
	{
		//	m_errorMsg = "Could not find a playable video/audio stream";
		CloseSrcFile();
		return false;
	}

	if (m_videoIndex != -1){
		// Find the decoder for the video stream
		AVCodec *pCodec = avcodec_find_decoder(m_pSrcVideoCodecCtx->codec_id);
		if (pCodec == NULL)
		{
			//	m_errorMsg = "Could not find a video decoder";
			CloseSrcFile();
			return false;
		}
		if (OpenCodec(pCodec, m_pSrcVideoCodecCtx) == -1)
		{
			CloseSrcFile();
			return false;
		}
		m_srcFileInfo.videoCode = m_pSrcVideoCodecCtx->codec_id;
		m_srcFileInfo.width = m_pSrcVideoCodecCtx->width;
		m_srcFileInfo.height = m_pSrcVideoCodecCtx->height;
		m_srcFileInfo.fps = (float)(m_srcFormatContext->streams[m_videoIndex]->r_frame_rate.num) / (float)(m_srcFormatContext->streams[m_videoIndex]->r_frame_rate.den) + 0.5f;
		m_srcFileInfo.videoBitrate = m_pSrcVideoCodecCtx->bit_rate;
		if (m_srcFileInfo.fps > 60)
		{
			m_srcFileInfo.fps = 25;
		}

	}

	if (m_audioIndex != -1){
		// Find the decoder for the video stream
		AVCodec *pCodec = avcodec_find_decoder(m_pSrcAudioCodecCtx->codec_id);
		if (pCodec == NULL)
		{
			//	m_errorMsg = "Could not find a video decoder";
			CloseSrcFile();
			return false;
		}
		if (OpenCodec(pCodec, m_pSrcAudioCodecCtx) == -1)
		{
			CloseSrcFile();
			return false;
		}
		m_srcFileInfo.audioCode = m_pSrcAudioCodecCtx->codec_id;	
		m_srcFileInfo.channel = m_pSrcAudioCodecCtx->channels;
		m_srcFileInfo.samplerate = m_pSrcAudioCodecCtx->sample_rate;
		m_srcFileInfo.audioBitrate = m_pSrcAudioCodecCtx->bit_rate;
	}
	else
	{
		m_bAudioValid = false;
	}


	if (m_srcFormatContext && m_srcFormatContext->duration / AV_TIME_BASE > 0.0)
	{
		m_bCanSeek = true;
	}
	return true;
}

bool CFileConverter::OpenDstFile(char *pDstFile)
{
	/* allocate the output media context */
	avformat_alloc_output_context2(&m_formatContext, NULL, NULL, pDstFile);

	if (!m_formatContext)
	{
		printf("Could not deduce output format from file extension: using avi.\n");
		avformat_alloc_output_context2(&m_formatContext, NULL, "avi", pDstFile);
	}
	if (!m_formatContext)
	{
		return false;
	}

	int ret = 0;
	AVOutputFormat *fmt = 0;
	fmt = m_formatContext->oformat;


	///* open the output file, if needed */
	//if (!(fmt->flags & AVFMT_NOFILE))
	//{
	//	//CData strFileName;
	//	//CCharacterSet::GB2312ToUTF_8(strFileName, fileName, strlen(fileName));
	//	//if ((ret = avio_open(&m_formatContext->pb, strFileName.c_str(), AVIO_FLAG_WRITE)) < 0)
	//	if ((ret = avio_open(&m_formatContext->pb, fileName, AVIO_FLAG_WRITE)) < 0)
	//	{
	//		PrintError(ret);
	//		return -1;
	//	}
	//}
	AVCodec* videoCodec = 0;
	AVCodec* audioCodec = 0;
	/* and initialize the codecs. */

	if (m_bVideoValid)
	{
		AVCodecID codec_id = m_formatContext->oformat->video_codec;
		char *zxx = strchr(pDstFile, '.');
		zxx++;
		if ((strcmp(zxx, "avi") == 0 || strcmp(zxx, "mp4") == 0) &&
			(m_srcFileInfo.videoCode == AV_CODEC_ID_H264 || m_srcFileInfo.videoCode == AV_CODEC_ID_MPEG4))
		{
			fmt->video_codec = (AVCodecID)m_srcFileInfo.videoCode;
		}
	}

	if (m_bAudioValid)
	{
	//	if (m_dstConvertPara.)
		/*if (m_isrcAudioType != m_idstAudioType || m_isrcAudioType != AAC || m_isrcAudioType != AAC)
		{
			//非MP4容器支持格式，需要转码
			m_idstAudioType = ZNV_AAC;//mp4 aac mp3
			AVCodecID codeID = GetAudioCodecId(m_isrcAudioType);
			m_transcodeCodec = OpenAudioTranscode(codeID);

			fmt->audio_codec = AV_CODEC_ID_AAC;
		}
		else
		{
			m_bAudioValid = false;
		}*/

	}


	if (m_bVideoValid && fmt->video_codec != AV_CODEC_ID_NONE)
	{
		
		m_videoStream = AddVideoStream(m_formatContext, &videoCodec, fmt->video_codec);
	}

	if (m_bAudioValid && fmt->audio_codec != AV_CODEC_ID_NONE)
	{
		m_audioStream = AddAudioStream(m_formatContext, &audioCodec, fmt->audio_codec);

	}
	/* Now that all the parameters are set, we can open the audio and
	* video codecs and allocate the necessary encode buffers. */
	if (m_videoStream)
	{
	//	OpenCodec(videoCodec, m_videoStream->codec);
	}


	if (m_audioStream)
	{
		if (OpenCodec(audioCodec, m_audioStream->codec) == -1)
		{
			avcodec_close(m_audioStream->codec);
			m_audioStream = NULL;
			m_bAudioValid = false;
		}
		else
		{
			AVCodecContext *pEncodeContext = m_audioStream->codec;
			m_samples_size = av_samples_get_buffer_size(NULL, pEncodeContext->channels, pEncodeContext->frame_size, pEncodeContext->sample_fmt, 1);
			int bytePerSample = av_get_bytes_per_sample(pEncodeContext->sample_fmt);
			m_nRequireSize = m_samples_size / 2;//per channel

		}
	}


	av_dump_format(m_formatContext, 0, pDstFile, 1);

	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE))
	{
		//CData strFileName;
		//CCharacterSet::GB2312ToUTF_8(strFileName, fileName, strlen(fileName));
		//if ((ret = avio_open(&m_formatContext->pb, strFileName.c_str(), AVIO_FLAG_WRITE)) < 0)
		if ((ret = avio_open(&m_formatContext->pb, pDstFile, AVIO_FLAG_WRITE)) < 0)
		{
			PrintError(ret);
			return false;
		}
	}

	/* Write the stream header, if any. */
	if ((ret = avformat_write_header(m_formatContext, NULL)) < 0)
	{
	//	LogStart << " --------- avformat_write_header Error ret =" << ret << LogEnd;
		PrintError(ret);
		return false;
	}
	m_bWriteHeader = true;

	/* Write the sdp info when begin. */
	//InputVideoStream(m_sdkKeyInfo.c_str(), m_sdkKeyInfo.length());

	return true;
}



/* Add a video output stream. */
AVStream* CFileConverter::AddVideoStream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
	AVCodecContext *c;
	AVStream *st;


	/* find the video encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec))
	{
		return 0;
	}

	//
	st = avformat_new_stream(oc, *codec);
	//st = avformat_new_stream(oc, NULL);
	if (!st)
	{
		return 0;
	}
	c = st->codec;
c->codec_tag = 0;
	c->codec_type = AVMEDIA_TYPE_VIDEO;


	avcodec_get_context_defaults3(c, *codec);
//	avcodec_get_context_defaults3(c, m_pSrcVideoCodecCtx->codec);

	if (m_pSrcVideoCodecCtx)
	{	
		
	/*	avcodec_copy_context(c, m_pSrcVideoCodecCtx);//不要拷贝，会导致(巨峰科技.h264)转mp4不成功. 但没这句又会导致中维.mp4转avi失败
		if (m_pSrcVideoCodecCtx->extradata!=NULL )//不要去拷贝，拷贝之后openCodec会导致avcodec_close(m_videoStream->codec)崩溃
		{
			c->extradata =(uint8_t*) new char[m_pSrcVideoCodecCtx->extradata_size];
			memcpy(c->extradata, m_pSrcVideoCodecCtx->extradata, m_pSrcVideoCodecCtx->extradata_size);
		}*/
	}

	c->codec_id = codec_id;

	/* Put sample parameters. */

	c->bit_rate = m_srcFileInfo.videoBitrate;//DEFAULT_VIDEO_RATE
	
	if (c->bit_rate == 0)
	{
		c->bit_rate = 10000;
	}
//	c->bit_rate = 0;
	//c->bit_rate =10000;//DEFAULT_VIDEO_RATE
	/* Resolution must be a multiple of two. */

	c->width = m_srcFileInfo.width ;
	c->height = m_srcFileInfo.height;


	/* timebase: This is the fundamental unit of time (in seconds) in terms
	* of which frame timestamps are represented. For fixed-fps content,
	* timebase should be 1/framerate and timestamp increments should be
	* identical to 1. */
	//if (m_videoFrameRate.num == 0 || m_videoFrameRate.den == 0)
	{
		if (m_srcFileInfo.fps != 0)
		{
			/*
	c->time_base.num = 1;
			c->time_base.den = m_srcFileInfo.fps;;

	//		st->time_base = m_srcFormatContext->streams[m_videoIndex]->time_base;//这句话导致文件特别大，沃仕达.dav转为.avi变107M
*/
			st->time_base.den = m_srcFileInfo.fps;
			st->time_base.num = 1;

			st->r_frame_rate.num = m_srcFileInfo.fps;
			st->r_frame_rate.den = 1;

		}
		else
		{
			c->time_base.den = 25;
			c->time_base.num = 1;
		}
	}
	
//	c->gop_size = 25;// m_context->gop_size;//25; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->max_b_frames = 0; //important when write frame, b frame num is 0


	c->profile = m_pSrcVideoCodecCtx->profile;
	c->level = m_pSrcVideoCodecCtx->level;
	//c->profile = m_profile;
	//c->level = m_level;
	//这个extradata很重要,没有设置extradata(SPS,PPS),mediaPlayer播放不了mp4格式
//	c->extradata = m_extradata;
//	c->extradata_size = m_extradata_size;

	//以下可设可不设
	//c->me_range = m_pSrcVideoCodecCtx->me_range;
	//c->max_qdiff = m_pSrcVideoCodecCtx->max_qdiff;

	//c->qmin = m_pSrcVideoCodecCtx->qmin;
	//c->qmax = m_pSrcVideoCodecCtx->qmax;

	//c->qcompress = m_pSrcVideoCodecCtx->qcompress;

	//c->rc_max_rate = m_pSrcVideoCodecCtx->rc_max_rate;
	//c->rc_buffer_size = m_pSrcVideoCodecCtx->rc_buffer_size;
	//c->field_order = m_pSrcVideoCodecCtx->field_order;

	//c->bits_per_coded_sample = m_pSrcVideoCodecCtx->bits_per_coded_sample;

	//AVRational avr{ 0, 1 };
	//if (!c->sample_aspect_ratio.num) {
	//                c->sample_aspect_ratio   =
	//                st->sample_aspect_ratio =
	//				m_pSrcVideoCodecCtx->sample_aspect_ratio.num ? m_pSrcVideoCodecCtx->sample_aspect_ratio :
	//				m_pSrcVideoCodecCtx->sample_aspect_ratio.num ?
	//				m_pSrcVideoCodecCtx->sample_aspect_ratio : avr;
	//            }
	//st->avg_frame_rate = m_srcFormatContext->streams[m_videoIndex]->avg_frame_rate;

	/* Some formats want stream headers to be separate. */
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
	{
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	return st;
}

/* Add a audio output stream. */
AVStream* CFileConverter::AddAudioStream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
	AVCodecContext *c;
	AVStream *st;

	/* find the audio encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec))
	{
		return 0;
	}

	st = avformat_new_stream(oc, NULL);
//	st = avformat_new_stream(oc, *codec);
	if (!st)
	{
		return 0;
	}

	avcodec_get_context_defaults3(st->codec, *codec);
	c = st->codec;

	// some formats want stream headers to be separate
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
	{
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	if (m_pSrcAudioCodecCtx)
	{
		c->channels = m_srcFileInfo.channel;
		c->channels = 1;

		c->channels = m_srcFileInfo.channel;
		c->sample_rate = m_srcFileInfo.samplerate;
		c->bit_rate = m_srcFileInfo.audioBitrate;

		c->channel_layout = m_pSrcAudioCodecCtx->channel_layout;

		c->sample_fmt = m_pSrcAudioCodecCtx->sample_fmt;
	}

	return st;
}

// 打开编解码
int CFileConverter::OpenCodec(AVCodec *codec, AVCodecContext *cc)
{
	int ret;
	/* open the codec */
	AVDictionary *opts = NULL;
	printf("start \n");
	//-strict -2
	if (codec->id == AV_CODEC_ID_AAC)
	{
		av_dict_set(&opts, "strict", "experimental", 0);
		//av_dict_set(&opts, "strict", "2", 0);
	}

	ret = avcodec_open2(cc, codec, &opts);
	av_dict_free(&opts);
	printf("succed\n");
	if (ret < 0)
	{
		PrintError(ret);
		return -1;
	}

	return 0;
}

// 输出错误码
int CFileConverter::PrintError(int errorCode)
{
	char errbuf[1024];
	//const char *errbuf_ptr = errbuf;

	if (av_strerror(errorCode, errbuf, sizeof(errbuf)) < 0)
	{
		strerror_s(errbuf, 1024, errorCode);
		//::OutputDebugString(errbuf_ptr);
		printf(errbuf);
	}
	return 0;
}


// 音频编码
int CFileConverter::EncodeAudio(AVFormatContext* formatContext, AVCodecContext* codecContext, AVFrame* frame)
{
	int ret = 0;
	int gotOutput = 0;

	AVPacket avpkt;

	// add this to prevent encode error
	avpkt.data = 0; // packet data will be allocated by the encoder
	avpkt.size = 0;
	av_init_packet(&avpkt);

	/* encode the samples */
	ret = avcodec_encode_audio2(codecContext, &avpkt, frame, &gotOutput);
	//printf("ret:%d\n",ret);
	if (ret < 0)
	{
		PrintError(ret);
		return -1;
	}

	if (gotOutput)
	{
	/*	if (codecContext->coded_frame && codecContext->coded_frame->pts != AV_NOPTS_VALUE)
		{
			//avpkt.pts= av_rescale_q(codecContext->coded_frame->pts, codecContext->time_base, m_audioStream->time_base);
		}*/
		avpkt.flags |= AV_PKT_FLAG_KEY;
		avpkt.stream_index = 1;
		ret = av_interleaved_write_frame(formatContext, &avpkt);
		if (ret != 0)
		{
			PrintError(ret);
			return -1;
		}

		av_packet_unref(&avpkt);
	}

	return 0;
}


// 音频转码
int CFileConverter::TranscodeAudio(AVCodecContext* codecContext, const char* buff, unsigned long dwSize)
{
	int len = 0;
	int gotFrame = 0;
	AVPacket avpkt;
	av_init_packet(&avpkt);


	/* decode until eof */
	avpkt.data = (uint8_t*)buff;
	avpkt.size = dwSize;

	while (avpkt.size > 0)
	{
		if (!m_audioDecodedFrame)
		{
			if (!(m_audioDecodedFrame = av_frame_alloc()))
			{
				return -1;
			}
		}

		len = avcodec_decode_audio4(codecContext, m_audioDecodedFrame, &gotFrame, &avpkt);
		if (len < 0)
		{
			return -1;
		}


		if (gotFrame)
		{
			int pcmLen = m_audioDecodedFrame->linesize[0];

			int remainSize = pcmLen;
			while (remainSize>0)
			{
				int copySize = 0;
				if ((m_audioInputBuffSize + remainSize) <= AUDIO_BUFF_MAX_SIZE)
				{
					copySize = remainSize;
				}
				else
				{
					copySize = AUDIO_BUFF_MAX_SIZE - m_audioInputBuffSize;
					copySize = remainSize <= copySize ? remainSize : copySize;
				}
				int offset = pcmLen - remainSize;
				if (offset>0)
				{
					int offset = pcmLen - remainSize;
				}
				memcpy((unsigned char *)m_audioInputBuff + m_audioInputBuffSize, m_audioDecodedFrame->data[0] + (pcmLen - remainSize), copySize);

				remainSize -= copySize;
				m_audioInputBuffSize += copySize;

				while (m_audioInputBuffSize >= m_nRequireSize)
				{
					AVCodecContext *pEncodeContext = m_audioStream->codec;

					if (!m_toEncframe)
					{
						m_toEncframe = av_frame_alloc();
						m_toEncframe->nb_samples = pEncodeContext->frame_size;
					}


					if (!m_audioOutputSamples)
					{
						m_audioOutputSamples = new char[m_samples_size];;
						memset(m_audioOutputSamples, '0', m_samples_size);

						avcodec_fill_audio_frame(m_toEncframe, pEncodeContext->channels, pEncodeContext->sample_fmt, (uint8_t *)m_audioOutputSamples, m_samples_size, 1);
					}

					if (!m_swr_ctx)
					{
						if (pEncodeContext->channel_layout != codecContext->channel_layout || pEncodeContext->sample_fmt != codecContext->sample_fmt || pEncodeContext->sample_rate != codecContext->sample_rate)
						{
							m_swr_ctx = swr_alloc();
						}
					}

					if (m_swr_ctx)
					{
						swr_alloc_set_opts(m_swr_ctx, pEncodeContext->channel_layout, pEncodeContext->sample_fmt, pEncodeContext->sample_rate, codecContext->channel_layout, codecContext->sample_fmt, codecContext->sample_rate, NULL, NULL);

						int ret = 0;
						if ((ret = swr_init(m_swr_ctx)) < 0)
						{
							PrintError(ret);
							swr_free(&m_swr_ctx);
							m_swr_ctx = NULL;
						}
						else
						{
							//if(!pFile)
							//{
							//	pFile = fopen("pcm.pcm","wb");
							//}
							//if(pFile)
							//{
							//	fwrite(m_audioInputBuff,1,m_nRequireSize,pFile);
							//}
							////int size = swr_convert(m_swr_ctx,(unsigned char **)&m_audioOutputSamples,m_toEncframe->nb_samples,(const uint8_t**)&m_decodedFrame->data[0],m_decodedFrame->nb_samples);
							int size = swr_convert(m_swr_ctx, (unsigned char **)&m_audioOutputSamples, m_toEncframe->nb_samples, (const uint8_t**)&m_audioInputBuff, m_nRequireSize);

							//	int dst_bufsize = len * pEncodeContext->channels * av_get_bytes_per_sample(pEncodeContext->sample_fmt);

							//printf("dst_bufsize = %d",dst_bufsize);
						}
					}

					EncodeAudio(m_formatContext, pEncodeContext, m_toEncframe);

					m_audioInputBuffSize -= m_nRequireSize;
					if (m_audioInputBuffSize>0)
					{
						memmove(m_audioInputBuff, m_audioInputBuff + m_nRequireSize, m_audioInputBuffSize);
					}
				}//while(m_audioInputBuffSize>=m_nRequireSize)	

			}//while(remainSize>0)

		}

		avpkt.size -= len;
		avpkt.data += len;
		avpkt.dts = AV_NOPTS_VALUE;
		avpkt.pts = AV_NOPTS_VALUE;
	}

	av_packet_unref(&avpkt);

	return 0;
}
void CFileConverter::GetTotalFrameThreadRun()
{
	/*
	FILE *fd = NULL;
	int error = fopen_s(&fd, m_strFileName, "rb");
	if (fd)
	{
		fseek(fd, 0, SEEK_END);
		m_fileSize = ftell(fd);
		fclose(fd);
	}*/

	
}

void CFileConverter::ConvertThreadRun()
{
	if (!m_bCanSeek){
		AVFormatContext *pFormatCtx = NULL;
		// Open the video file
		int ret = avformat_open_input(&pFormatCtx, m_srcFormatContext->filename, NULL, NULL);
		if (ret < 0)
		{
			if (pFormatCtx)
			{
				avformat_close_input(&pFormatCtx);
			}
			return;
		}

		while (1){
			if (m_bStop){
				return;
			}
			AVPacket packet;
			int ret = av_read_frame(pFormatCtx, &packet);
			if (ret == 0){
				if (packet.stream_index == m_videoIndex){
					m_frameTotalCount++;
					av_packet_unref(&packet);
				}
				else{
					av_packet_unref(&packet);
				}
			}
			else{
				break;
			}
		}

		 //av_seek_frame(m_srcFormatContext, m_videoIndex, 0, 0);
		m_bReadFrameFinish = true;
		if (pFormatCtx)
		{
			avformat_close_input(&pFormatCtx);
		}
	}
	

	double duaration = (double)1 / (double)m_srcFileInfo.fps;
/*	FILE *pfd = NULL;
	int ret = fopen_s(&pfd, m_srcFormatContext->filename, "rb");
	if (pfd)//打开文件成功
	{
		char buf[8];
		memset(buf, 0, 8);
		fread(buf, 8, 1, pfd);

		if (strncmp(buf, "264DV", 4) == 0)
		{
			duaration = (double)1 / (double)m_srcFileInfo.fps * 2;
		}
		fclose(pfd);
	}
	*/
	double time = 0;
	int curFrameCount = 0;
	bool bCut = false;
	if (m_stopTime != 0)
	{
		bCut = true;
	}

	bool bFirst = true;
	while (1){
		if (m_bStop){
			return;
		}
		//printf("while\n");
		AVPacket packet;
		av_init_packet(&packet);
		int ret = av_read_frame(m_srcFormatContext, &packet);
		if (ret == 0){
			if (packet.stream_index == m_videoIndex){
				//	PushVideoFrame(packet);
				//	printf("m_videoIndex\n");
				//	packet.pts = 0; 
				//av_rescale_q
				/*
				packet.pts = av_rescale_q_rnd(statrtPts,
				m_videoStream->codec->time_base,
				m_videoStream->time_base,
				(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));*/

				curFrameCount++;
				unsigned long long curTime = 0;
				if (!m_bCanSeek&&m_srcFileInfo.fps != 0)
				{
					curTime = curFrameCount / m_srcFileInfo.fps;
				}
				else
				{
					int64_t pts = packet.pts == AV_NOPTS_VALUE ? packet.dts : packet.pts;
					if (pts == AV_NOPTS_VALUE)
					{
						pts = 0;
					}
					curTime =pts*av_q2d(m_srcFormatContext->streams[m_videoIndex]->time_base);
				}


				if (m_startTime != 0 &&curTime<m_startTime)
				{
					av_packet_unref(&packet);
					continue;
				}
				/*
				if (bFirst)
				{
					if (!packet.flags&AV_PKT_FLAG_KEY)//非I帧
					{
						av_packet_unref(&packet);
						continue;
					}
					bFirst = false;
				}
				*/
				packet.pts = time * m_videoStream->time_base.den / m_videoStream->time_base.num;
				packet.dts = packet.pts;
				packet.duration = duaration * m_videoStream->time_base.den / m_videoStream->time_base.num;
				time += duaration;
				
				/*
				packet.pts = av_rescale_q_rnd(packet.pts, m_srcFormatContext->streams[m_videoIndex]->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

				packet.dts = av_rescale_q_rnd(packet.dts, m_srcFormatContext->streams[m_videoIndex]->time_base, m_videoStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

				packet.duration = av_rescale_q(packet.duration, m_srcFormatContext->streams[m_videoIndex]->time_base, m_videoStream->time_base);

				packet.pos = -1;*/


				if (!m_bCanSeek&&m_frameTotalCount!=0)
				{
					
					m_progress = curFrameCount * 100 / m_frameTotalCount;
				}
				else
				{
					m_progress = curTime * 100 / ((float)m_srcFormatContext->duration / AV_TIME_BASE);
					
				}
				if (bCut && (curTime - m_startTime) > 0){
						m_progress = (curTime - m_startTime) * 100 / (m_stopTime - m_startTime);
					}

				if (m_progress >= 100)
					m_progress = bCut ? 100 : 99;//没有截取片段的，整个文件转码，应该读取完文件后自动关闭。自己算的m_progress会有些误差，可能会大于100，设小点

			//	frame->pts = av_frame_get_best_effort_timestamp(frame);
		//		ret = filter_encode_write_frame(frame, stream_index);
				int bRet=av_interleaved_write_frame(m_formatContext, &packet);
				av_packet_unref(&packet);
				if (bRet < 0)
				{
				//	m_progress = -1;
				//	break;
				}
				else
				{
					//m_progress = 0;
				}

				if (bCut &&  curTime >= m_stopTime)
				{
					m_progress = 100;
					break;
				}
			}
			else if (packet.stream_index == m_audioIndex){
				//printf("m_audioIndex\n");
			//	av_interleaved_write_frame(m_formatContext, &packet);
				av_packet_unref(&packet);
			}
			else{
				av_packet_unref(&packet); 
			}
		}
		else{
			m_progress = 100;
			break;
		}
	}
	//avio_tell
	

}
