#ifndef COD5BOARDLIB_EX_H
#define COD5BOARDLIB_EX_H

#include "Cmn5BoardLibEx.h"

#pragma pack( push, COD5BOARDLIB_EX_H )
#pragma pack(8)

extern "C" const IID IID_ICod5 ;

//IID for Cod5
// {5E11F454-7FDC-47ae-BF78-EE83BF8F733C}
DEFINE_GUID( IID_ICod5, 0x5e11f454, 0x7fdc, 0x47ae, 0xbf, 0x78, 0xee, 0x83, 0xbf, 0x8f, 0x73, 0x3c );

typedef struct _COD5_BOARD_INFO{
	ULONG		uBoardID;
	ULONG		uModelID;
	ULONG		uSlotNumber;

	ULONG		uMaxVP;
	ULONG		uMaxChannel;
	ULONG		uMaxDO;
	ULONG		uMaxDI;

	ULONG		uCodecType;
	ULONG		uVideoCodecType;
	ULONG		uAudioCodecType;

	const CMN5_RESOLUTION_INFO* pResInfo;	//132*sizeof(ULONG)

	ULONG		uHWRevision;		//add 2005, 8,2 

	ULONG		uVideoStreamsPerChannel;    
	ULONG		uAudioStreamsPerChannel;	
	ULONG		reserved[CMN5_MAX_RESERVED_BOARD_INFO - 14];
} COD5_BOARD_INFO;

// TODO: 헤더 파일 분리?
#define COD5_STREAMID_MAKE(streamidx, channelidx) ((streamidx)<<16|(channelidx))
#define COD5_STREAMID_GETSTREAMIDX(streamid) (((streamid)>>16)&0xff)
#define COD5_STREAMID_GETCHANNELIDX(streamid) ((streamid)&0xff)

#define COD5_VCP_BASELINE	0x01
#define COD5_VCP_MAIN		0x02
#define COD5_VCP_HIGH		0x03

#define COD5_VCE_CAVLC		0x01
#define COD5_VCE_CABAC		0x02

#define COD5_VCS_HALFPEL	0x01
#define COD5_VCS_QUARTERPEL	0x02

#define COD5_VCF_FRAME		0x01
#define COD5_VCF_FIELD		0x02
#define COD5_VCF_PAFF		0x03

//Frame Type for COD5_DATA_INFO
//enum FrameType {
#define	COD5_FT_MP4V_I_PIC		0x01
#define	COD5_FT_MP4V_P_PIC		0x02
#define	COD5_FT_MP4V_B_PIC		0x03
#define	COD5_FT_MD_1			0x04
#define	COD5_FT_MP2_PES_STREAM	0x05
#define	COD5_FT_PCM_AUDIO		0x06
#define	COD5_FT_H263_I_PIC		0x07
#define	COD5_FT_H263_P_PIC		0x08
#define	COD5_FT_G723_AUDIO		0x09
#define	COD5_FT_ADPCM_AUDIO		0x0A
#define	COD5_FT_MP1_AUDIO		0x0B
#define	COD5_FT_MP2_AUDIO		0x0C
#define	COD5_FT_MP3_AUDIO		0x0D
#define	COD5_FT_AAC_AUDIO		0x0E
#define	COD5_FT_MPEG_ES_HDR		0x0F
#define	COD5_FT_GSM610_AUDIO	0x10
#define	COD5_FT_MP2V_I_PIC		0x11
#define	COD5_FT_MP2V_P_PIC		0x12
#define	COD5_FT_MP2V_B_PIC		0x13
#define	COD5_FT_MJPEG			0x14
#define	COD5_FT_YV12			0x15
#define	COD5_FT_YUY2			0x16
#define	COD5_FT_H264_I_PIC		0x17
#define	COD5_FT_H264_P_PIC		0x18
#define	COD5_FT_H264_B_PIC		0x19
// HP4K only {
#define	COD5_FT_G711_AUDIO				0x1A	// add by HP4K uLaw
#define	COD5_FT_H264_SVC_SEI			0x1B	// add by HP4K
#define	COD5_FT_H264_SVC_PREFIX			0x1C	// add by HP4K
#define	COD5_FT_H264_SVC_SUBSET_SPS		0x1D	// add by HP4K
#define	COD5_FT_H264_SVC_SLICE_SCALABLE	0x1E	// add by HP4K
#define	COD5_FT_H264_SPS				0x1F	// add by HP4K
#define	COD5_FT_H264_PPS				0x20	// add by HP4K
#define	COD5_FT_PASSTHROUGH				0x80    // add by HP4K
// }
#define	COD5_FT_MD_2			0x21
#define	COD5_FT_ULAW_AUDIO		COD5_FT_G711_AUDIO
#define	COD5_FT_ALAW_AUDIO		0x23
//};

typedef struct _COD5_DATA_INFO{
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;				// OUT: Board ID[0..3]
	ULONG			uChannelNum;			// OUT: Channel ID of the Board
	ULONG			uHasNextData;			// OUT: The next data is or not in queue
	UCHAR			*pDataBuffer;			// OUT: The address of image
#if _MSC_VER >= 1300
	LARGE_INTEGER_CAP	TimeTag;			// OUT: KeQuerySystemTime(). DTS (Decoding Time Stamp) to avoid byte alignment problem
#else
	LARGE_INTEGER	TimeTag;				// OUT: KeQuerySystemTime(). DTS (Decoding Time Stamp)
#endif
	ULONG			uBufferIdx;				// Internal use! do not touch!
	
	//-- Cod5 extended
	ULONG			uDataSize;
	ULONG			uFrameType;
} COD5_DATA_INFO;

typedef struct _COD5_DATA_INFO_EX {
	_CMN5_DATA_INFO_DATA_EX_CMM_BODY;
	union {
		ULONG			reserved[32];
		struct{
			union{
				ULONG reservedCod5Cmn[16];
				struct {
					ULONG			uDataSize;
					ULONG			uFrameType;
#if _MSC_VER >= 1300
					LARGE_INTEGER_CAP	PTS;		// OUT: PTS (Presentation Time Stamp) to avoid byte alignment problem
#else
					LARGE_INTEGER	PTS;			// OUT: PTS (Presentation Time Stamp)
#endif
					ULONG			uFreeBufferSize; // OUT: The remained buffer size of video data
					BOOL			bPend;
					
					BOOL			bStartOfStream;	// OUT: To indicate the start of stream
					ULONG			uSubErrCode;	// OUT: The detailed information for the uErrCode
					ULONG			uSequenceNumber;// OUT: (HP4K)
				};
			};
			
			union{
				ULONG reservedCod5Model[16];
				
				//DMP Cod5SetEventData
				struct {
					ULONG uDMPSetOption;			//IN: set the frame's decoding option
					ULONG uDMPSetUserData0;			//IN: Application specific custom data 1/2
					ULONG uDMPSetUserData1;			//IN: Application specific custom data 2/2
				};
				//DMP Cod5GetEventData
				struct {
					ULONG uDMPGetImgSize;			//OUT: decoded frame's image size
					ULONG uDMPGetUserData0;			//OUT: Application specific custom data 1/2
					ULONG uDMPGetUserData1;			//OUT: Application specific custom data 2/2
				};
				
				//HP3000 Cod5GetEventData
				struct {
#if _MSC_VER >= 1300
					LARGE_INTEGER_CAP	CTS;		// OUT: CTS (Composition Time Stamp) to avoid byte alignment problem
#else
					LARGE_INTEGER		CTS;		// OUT: CTS (Composition Time Stamp)
#endif
				};
				
				//HM Series Cod5GetEventData
				struct {
#if _MSC_VER >= 1300
					LARGE_INTEGER_CAP	CTS;		// OUT: CTS (Composition Time Stamp) to avoid byte alignment problem
#else
					LARGE_INTEGER		CTS;		// OUT: CTS (Composition Time Stamp)
#endif				
					union{
						BYTE *pExtBuffer;
						BYTE *pMotionBitmap;
					};
					ULONG	uFirstNalSize;
				};

				//other board specific structure can be used here... 
			};
			
		};
	};
} COD5_DATA_INFO_EX;

// {
// exclusively used only at NVE
#define COD5_MAX_OSD_STRING_LENGTH		256

typedef struct _COD5_OSD_STRING_INFO {
	union {
		ULONG			reserved[128];
		struct {
			ULONG			uX;
			ULONG			uY;
			ULONG			uFgColor;
			ULONG			uBgColor;
			ULONG			uLength;
			char			String[COD5_MAX_OSD_STRING_LENGTH];
		};
	};
} COD5_OSD_STRING_INFO;
// }

// Motion Detection Area Type
#define COD5_MDT_RECT			(1<<0)
#define COD5_MDT_BITMAP			(1<<1)

// Motion Detection Info Type
#define COD5_MDI_FLAG			(1<<0)
#define COD5_MDI_COUNT			(1<<1)
#define COD5_MDI_BITMAP			(1<<2)
#define COD5_MDI_CONTINUOUS		(1<<3)

typedef struct _COD5_MD_PROPERTY {
	union {
		ULONG			reserved[8];
		struct {
			ULONG			uMaxLayerCount;
			ULONG			uMaxRectCount;
			ULONG			uAreaType;
			ULONG			uInfoSize;
		};
	};
} COD5_MD_PROPERTY;

typedef struct _COD5_MD_INFO {
	union {
		ULONG			reserved[16];
		struct {
			ULONG			uVersion;
			ULONG			uLayerID;
			ULONG			uEnable;
			ULONG			uActivity;
			ULONG			uThreshold;
			ULONG			uAreaCount;
			ULONG			uAreaStructSize;
			ULONG			uInfoType;
		};
	};
} COD5_MD_INFO;

typedef struct _COD5_MD_AREA_RECT {
	union {
		ULONG			reserved[16];
		struct {
			ULONG			uType;
			ULONG			uSize;
			ULONG			uRectID;
			ULONG			uX;
			ULONG			uY;
			ULONG			uWidth;
			ULONG			uHeight;
		};
	};
} COD5_MD_AREA_RECT;

typedef struct _COD5_MD_AREA_BITMAP {
	union {
		ULONG			reserved[16];
		struct {
			ULONG			uType;
			ULONG			uSize;
			ULONG			uPxlWidth;
			ULONG			uPxlHeight;
			ULONG			uBmWidth;
			ULONG			uBmHeight;
			ULONG			uBmSize;
			BYTE			*pBmImage;
		};
	};
} COD5_MD_AREA_BITMAP;

typedef struct _COD5_HM_MDBITMAP_HEADER{
    unsigned long version;
    unsigned long width;
    unsigned long height;
    unsigned long sampleTicks;
    unsigned long motionCounter;
    unsigned long motionBitmapSize;
} COD5_HM_MDBITMAP_HEADER;

typedef struct _COD5_MD2_DATA{
    int					controlObjectId;
    int					eventId;
    unsigned int        timestamp;
    unsigned int        streamID;
    unsigned int        index_transition;
    unsigned int        avgMot_MBInMot;
    unsigned int        PTS_MSB;
    unsigned int        PTS_LSB;
} COD5_MD2_DATA;

#define COD5_MD2_DATA_GET_INDEX(d)		((d).index_transition >> 16)
#define COD5_MD2_DATA_GET_LASTEVENT(d)	(((d).index_transition>>8) &0xFF)
#define COD5_MD2_DATA_GET_TRANSITION(d) ((d).index_transition &0xFF)
#define COD5_MD2_DATA_GET_AVGMOT(d)		((d).avgMot_MBInMot >> 16 )
#define COD5_MD2_DATA_GET_MBINMOT(d)	((d).avgMot_MBInMot & 0xFFFF)

//enum BitrateMode {
#define	COD5_BM_CBR		1					// Both  : uAvg
#define	COD5_BM_VBRR	2					// Kama  : uAvg, uMax
#define	COD5_BM_VBRQ	3					// Mizar : uQuant
#define	COD5_BM_VBRH	4					// Mizar : uMin, uMax
#define	COD5_BM_VBRIPQ 	5					// Guiana : Quat, Quant
#define COD5_BM_CQ		6					// HP4K : Quility (0~100) 100=>best
#define COD5_BM_VBRQR	7					// uMaxQP, uMinQP
//};

// video codec type
//enum VideoCodecType {
#define COD5_VCT_NONE		0x00 
#define	COD5_VCT_MP2_PS		0x21
#define COD5_VCT_MP2_ES		0x22
#define	COD5_VCT_MP4_ES		0x41
#define	COD5_VCT_H263_ES	0x61
#define COD5_VCT_H264_ES	0x62
#define COD5_VCT_H264_QBOX	0x63
#define COD5_VCT_H264_SVC	0x64		// HP4K
#define COD5_VCT_DCP4_ES	0x81
#define COD5_VCT_MJPEG		0xA1
#define COD5_VCT_YV12		0xC1
//};

// audio codec type
//enum AudioCodecType { 
#define COD5_ACT_NONE			0x00
#define	COD5_ACT_PCM			0x01
#define	COD5_ACT_ADPCM			0x02
#define	COD5_ACT_ULAW			0x03
#define	COD5_ACT_ALAW			0x04
#define	COD5_ACT_MP1		 	0x11
#define	COD5_ACT_MP2 			0x12
#define	COD5_ACT_MP3 			0x13
#define	COD5_ACT_MP2_QBOX		0x14
#define	COD5_ACT_AAC			0x21
#define	COD5_ACT_G723			0x31
//};



// DMP200 playback modes
#define COD5_PLAYBACK_MODE_FILE		0x00000001
#define COD5_PLAYBACK_MODE_REALTIME	0x00000002
#define COD5_PLAYBACK_MODE_CAPONLY	0x00000003

// DMP200 SUB/file playback modes
#define	COD5_SUB_PLAYBACK_MODE_INVALID		0x00
#define	COD5_SUB_PLAYBACK_MODE_RANDOMSEEK	0x01
#define	COD5_SUB_PLAYBACK_MODE_FORWARD		0x02
#define	COD5_SUB_PLAYBACK_MODE_BACKWARD		0x03
#define	COD5_FILE_PLAYBACK_MODE_INVALID		0x00
#define	COD5_FILE_PLAYBACK_MODE_RANDOMSEEK	0x01
#define	COD5_FILE_PLAYBACK_MODE_FORWARD		0x02
#define	COD5_FILE_PLAYBACK_MODE_BACKWARD	0x03



//enum QueryInfoCmd {
#define COD5_QIC_GET_BUFFER_CAPACITY	0x1000
// 0x3000 ~ 0x3FFF : CMN5 QueryInfoCmd

// };



enum AudioCodecSamplFreq { 
	COD5_ACSF_8K	 	= 8000,
	COD5_ACSF_16K	= 16000,
	COD5_ACSF_24K	= 24000,
	COD5_ACSF_32K	= 32000,
	COD5_ACSF_44_1K	= 44100,
	COD5_ACSF_48K	= 48000,
};

enum CodecStreamType {
	COD5_CST_VIDEO	= 0x00000001,
	COD5_CST_AUDIO	= 0x00000002,
	COD5_CST_MDBITMAP= 0x00000004,
	COD5_CST_MD		= 0x00000004,	// HP4K bitmap
	COD5_CST_VA		= 0x00000008,
	COD5_CST_MDROI= 0x00000010,
};

enum BufferOverflowHandling {
	COD5_BOH_CLEAR	= 1,
	COD5_BOH_KEEP	= 2,
};

enum CodecPropertyCmd {
	COD5_CPC_VIDEO_FORMAT		= 0x01000001,
	COD5_CPC_VIDEO_IMAGE_SIZE	= 0x01000004,
	COD5_CPC_SKIP_FRAME			= 0x01010001,
	COD5_CPC_BITRATE			= 0x01010002,
	COD5_CPC_GOP_SIZE			= 0x01010004,
	COD5_CPC_AUDIO_ATTR			= 0x01020001,
	COD5_CPC_STREAM_ENABLE		= 0x01030001,
	COD5_CPC_STREAM_TYPE		= COD5_CPC_STREAM_ENABLE,
	COD5_CPC_CODEC_TYPE			= 0x01030004,
	
	COD5_CPC_BUFFER_OVERFLOW_HANDLING	= 0x01030008, // exclusively used only at MP2000/3000
	COD5_CPC_OVERFLOW_INTERNAL_MGMT	= 0x01030009, // exclusively used only at MP2000/3000

	COD5_CPC_FRAMERATE			= 0x01031001, // exclusively used only at HP3000
	COD5_CPC_CODING_TYPE		= 0x01031002, // exclusively used only at HP3000
	COD5_CPC_PIXEL_AR			= 0x01031003, // exclusively used only at HP3000
	COD5_CPC_VIDEO_IMAGE_CROP	= 0x01031004, // exclusively used only at HP3000

	COD5_CPC_MAX_GOP			= 0x01040001, // exclusively used only at DMP200
	COD5_CPC_PLAYBACK_MODE		= 0x01040002, // exclusively used only at DMP200
	COD5_CPC_BEGIN_CHANNELMAP	= 0x01040003, // exclusively used only at DMP200
	COD5_CPC_CHANNELMAP			= 0x01040004, // exclusively used only at DMP200
	COD5_CPC_END_CHANNELMAP		= 0x01040005, // exclusively used only at DMP200

	COD5_CPC_LOOPBACK			= 0x01050001, // exclusively used only at NVE
	COD5_CPC_DENOISE_FILTER		= 0x01050002, // exclusively used only at NVE

	COD5_CPC_MD_PROPERTY		= 0x01060001,
	COD5_CPC_MD_INFO			= 0x01060002,

	COD5_CPC_PROFILE			= 0x01070001,
	COD5_CPC_LEVEL				= 0x01070002,
};

// HP4K
#define HP4K_CPC_STREAMING		0x01080000
#define COD5_CPC_ENABLE_CHANNEL		COD5_CPC_STREAM_ENABLE // HP4K only  
//#define COD5_CPC_VIDEO_IMAGE_SIZE
//#define COD5_CPC_FRAMERATE

enum CodecAdjustCmd {
	COD5_CAC_BRIGHTNESS			= 0x01000001,
	COD5_CAC_CONTRAST			= 0x01000002,
	COD5_CAC_SATURATION_U		= 0x01000004,
	COD5_CAC_SATURATION_V		= 0x01000008,
	COD5_CAC_HUE				= 0x01000010,

	COD5_CAC_SKIP_FRAME			= 0x01000011, // exclusively used only at MP2000/3000
	COD5_CAC_BITRATE			= 0x01000012, // exclusively used only at MP2000/3000
	COD5_CAC_VIDEO_IMAGE_SIZE	= 0x01000013, // exclusively used only at MP2000/3000
	COD5_CAC_AUDIO_ATTR			= 0x01000014, // exclusively used only at MP2000/3000
	COD5_CAC_GOP_SIZE			= 0x01000015, // exclusively used only at MP2000/3000
	COD5_CAC_OVERFLOW_FLAG		= 0x01000016, // exclusively used only at MP2000/3000
	COD5_CAC_GET_HEART_BEAT		= 0x01000017, // exclusively used only at MP2000/3000
	COD5_CAC_GET_FREE_BUFFER	= 0x01000018, // exclusively used only at MP2000/3000
	COD5_CAC_SET_FBS_TORESTART	= 0x01000019, // exclusively used only at MP2000/3000
	COD5_CAC_CHIP_RESET			= 0x09000001, // exclusively used only at MP2000/3000

	COD5_CAC_FRAMERATE			= 0x01010001, // exclusively used only at HP3000
	COD5_CAC_CODING_TYPE		= 0x01010002, // exclusively used only at HP3000
	COD5_CAC_FORCE_NEWGOP		= 0x01010003, // exclusively used only at HP3000
	COD5_CAC_VIDEO_IMAGE_CROP	= 0x01010004, // exclusively used only at HP3000
	COD5_CAC_CODEC_IS_READY		= 0x01010005, // exclusively used only at HP3000

	COD5_CAC_PLAYBACK_POS		= 0x01040001, // exclusively used only at DMP Series
	COD5_CAC_FILE_PLAYBACK_MODE	= 0x01040002, // exclusively used only at DMP Series
	COD5_CAC_SUB_PLAYBACK_MODE	= 0x01040002, // exclusively used only at DMP Series
	COD5_CAC_FLUSH_BUFFER		= 0x01040003, // exclusively used only at DMP Series
	COD5_CAC_PLAYBACK_SPEED		= 0x01040004, // exclusively used only at DMP Series
	COD5_CAC_TIME_OFFSET		= 0x01040005, // exclusively used only at DMP Series
	COD5_CAC_QUEUED_FRAME		= 0x01040006, // exclusively used only at DMP Series

	COD5_CAC_AUDIO_GAIN			= 0x01020002,

	COD5_CAC_VM_SEL_CHANNEL		= 0x01030001,

	COD5_CAC_OSD_STRING			= 0x01050001, // exclusively used only at NVE
	COD5_CAC_OSD_SHOW			= 0x01050002, // exclusively used only at NVE
	COD5_CAC_OSD_CLEAR			= 0x01050003, // exclusively used only at NVE
	COD5_CAC_SHARPNESS			= 0x01050004, // exclusively used only at NVE

	COD5_CAC_STREAM_ENABLE		= 0x01060001,
	COD5_CAC_SESSION_STREAM_ENABLE	= 0x01060002,
	COD5_CAC_CHANNEL_ENABLE		= 0x01060003, //equvalent to Cod5ChannelEnable

	COD5_CAC_BIT_ENABLE			= 0x01070001, // exclusively used only at HP Series (Burnt-in Text)
	COD5_CAC_BIT_PALETTE		= 0x01070002, // exclusively used only at HP Series (Burnt-in Text)
	COD5_CAC_BIT_UPDATE			= 0x01070003, // exclusively used only at HP Series (Burnt-in Text)

// HM reserved 0x01080001
	COD5_CAC_PIXEL_AR				= 0x01080002, // exclusively used only at HP3000, HM Series
//This parameter controls the alpha offset used by the deblocking filter to select the QP-based filter strength
//for Bs < 4.
//Parameters:
//value Alpha offset in the range of -6 to 6, where -6 is weakest deblocking filter strength, +6 is strongest
//deblocking strength
// param0: enable , param1:DEBLOCK_OFFSET_ALPHA, param2:DEBLOCK_OFFSET_BETA 

// HM reserved 0x01080003
// HM reserved 0x01080004
	COD5_CAC_FILTER_DEBLOCK			= 0x01080005,
	COD5_CAC_ENTROPY_CODING_TYPE	= 0x01080008,
// HM reserved 0x01080009
	COD5_CAC_ME_SEARCH_RANGE		= 0x0108000A,
	COD5_CAC_FIELD_CODING_TYPE		= 0x0108000B,
	COD5_CAC_MD_BITMAP_THRESHOLD	= 0x0108000C,//param0: threshold(1~255)
	COD5_CAC_PROFILE				= 0x0108000D,
	COD5_CAC_LEVEL					= 0x0108000E,
// HM reserved 0x0108000F ~ 0x01080012
	COD5_CAC_MD_ROI_ADD_RECT		= 0x01080013, //param0 index(0~15): param1:MAKELONG(width,height) param2:MAKELONG(offsetx,offsety) macroblock unit , not supported Cod5GetCodecAdjust
	COD5_CAC_MD_ROI_SUB_RECT		= 0x01080014, //param0 index(0~15): param1:MAKELONG(width,height) param2:MAKELONG(offsetx,offsety) macroblock unit , not supported Cod5GetCodecAdjust
	COD5_CAC_MD_ROI_ENABLE			= 0x01080015, // param0 index(0~15), param1: bEnable
	COD5_CAC_MD_ROI_MOTION_THRESHOLD= 0x01080016, // param0 index(0~15), param1: threshold(1~255)
	COD5_CAC_MD_ROI_PERCENT			= 0x01080017, // param0 index(0~15), param1: percent*100(0~10000)
	COD5_CAC_MD_ROI_DELAY			= 0x01080019, // param0 index(0~15), param1: delay event ms(mili-second)
	COD5_CAC_MBAFF_ENABLE			= 0x01080018, //applicable for frame,paff D1(interlace)
// HM reserved 0x01081000 ~ 0x01081FFF
};

// HP4K
#define	COD5_CAC_ENABLE_CHANNEL		COD5_CAC_STREAM_ENABLE // HP4K only

// Obsolete definitions
#define COD5_QR_BUFFER_CAPACITY		0x1000
#define COD5_QR_FREE_BUFFER_SIZE	0x1001


extern "C"
{
//////////////////////////////////////////////////////////////////////////////////
// APIs

BOOL CMN5_API Cod5GetLastErrorCode(CMN5_ERROR_CODE_ITEM *pEcode);

BOOL CMN5_API Cod5QueryInfo(ULONG uQueryInfoCmd, ULONG uIn, void *pOut);

BOOL CMN5_API Cod5GetSystemInfo(CMN5_SYSTEM_INFO *pInfo);
BOOL CMN5_API Cod5GetBoardInfo(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, COD5_BOARD_INFO *pInfo);

BOOL CMN5_API Cod5Activate(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]);

BOOL CMN5_API Cod5Setup(void);
BOOL CMN5_API Cod5Endup(void);
BOOL CMN5_API Cod5Run(void);
BOOL CMN5_API Cod5Stop(void);

BOOL CMN5_API Cod5ChannelEnable(ULONG uBdID, ULONG uCh, BOOL bEnable);

BOOL CMN5_API Cod5SetCaptureMethod(ULONG uFuncType, ULONG uMethod);
BOOL CMN5_API Cod5SetCallback(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext);

BOOL CMN5_API Cod5QueryData(void *pData, DWORD uTimeOut);
BOOL CMN5_API Cod5ReleaseData(const void *pData);
BOOL CMN5_API Cod5SetEventHandle(ULONG uType, HANDLE hHandle);
BOOL CMN5_API Cod5GetEventHandle(ULONG uType, HANDLE *pHandle);
BOOL CMN5_API Cod5GetEventData(ULONG uType, void *pData);
BOOL CMN5_API Cod5SetEventData(ULONG uType, void *pData);

BOOL CMN5_API Cod5SetImageSize(ULONG uBdID, ULONG uCh, ULONG uImageSize);
BOOL CMN5_API Cod5GetImageSize(ULONG uBdID, ULONG uCh, ULONG *puImageSize);
BOOL CMN5_API Cod5SetVideoFormat(ULONG uBdID, ULONG uFormat);
BOOL CMN5_API Cod5GetVideoFormat(ULONG uBdID, ULONG *puFormat);

BOOL CMN5_API Cod5SetCodecProperty(ULONG uBdID, ULONG uCh, ULONG uCodecPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Cod5GetCodecProperty(ULONG uBdID, ULONG uCh, ULONG uCodecPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);
BOOL CMN5_API Cod5SetCodecAdjust(ULONG uBdID, ULONG uCh, ULONG uCodecAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3);
BOOL CMN5_API Cod5GetCodecAdjust(ULONG uBdID, ULONG uCh, ULONG uCodecAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3);

BOOL CMN5_API Cod5SetMultiCodecProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cod5GetMultiCodecProperties(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cod5SetMultiCodecAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);
BOOL CMN5_API Cod5GetMultiCodecAdjusts(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds);

BOOL CMN5_API Cod5GetVideoStatus(ULONG uBdID, ULONG uMaxCh, ULONG *pbwData);

BOOL CMN5_API Cod5SetDO(ULONG uBdID, ULONG uMaxDO,const ULONG *pbwData);
BOOL CMN5_API Cod5GetDO(ULONG uBdID, ULONG uMaxDO, ULONG *pbwData);
BOOL CMN5_API Cod5GetDI(ULONG uBdID, ULONG uMaxDI, ULONG *pbwData);
BOOL CMN5_API Cod5SetExtVideoOut(ULONG uBdID, ULONG uCh);
BOOL CMN5_API Cod5GetExtVideoOut(ULONG uBdID, ULONG *puCh);
BOOL CMN5_API Cod5SetWatchdog(ULONG uBdID, ULONG uWatchdogCmd, ULONG uParam1, ULONG uParam2);

BOOL CMN5_API Cod5I2CTransfer(ULONG uBdID, ULONG uVPID, ULONG uDevID, ULONG uI2CCmd, ULONG uAddr, UCHAR *pData);
BOOL CMN5_API Cod5WriteUserEEPROM(ULONG uBdID, ULONG uVPID,const UCHAR *pData);
BOOL CMN5_API Cod5ReadUserEEPROM(ULONG uBdID, ULONG uVPID, UCHAR *pData);


// *********************************************************************** 
// Vendoer Specific IO API
// *********************************************************************** 
BOOL CMN5_API Cod5SetSpecialIOMode(ULONG uBdID, ULONG uData);
BOOL CMN5_API Cod5GetSpecialIOMode(ULONG uBdID, ULONG *puData);
BOOL CMN5_API Cod5WriteSpecialIOData(ULONG uBdID, ULONG uData);
BOOL CMN5_API Cod5ReadSpecialIOData(ULONG uBdID, ULONG *puData);
} // extern "C"

#if !defined(KERNEL_MODE_CODE)

// Interface definition for ICod5
interface ICod5 : IUnknown
{
	STDMETHOD_(BOOL, Cod5GetLastErrorCode)(CMN5_ERROR_CODE_ITEM *pEcode) PURE;

	STDMETHOD_(BOOL, Cod5QueryInfo)(ULONG uQueryInfoCmd, ULONG uIn, void *pOut) PURE;

	STDMETHOD_(BOOL, Cod5GetSystemInfo)(CMN5_SYSTEM_INFO *pInfo) PURE;
	STDMETHOD_(BOOL, Cod5GetBoardInfo)(ULONG uBdID,const CMN5_BOARD_INFO_DESC *pDesc, COD5_BOARD_INFO *pInfo) PURE;

	STDMETHOD_(BOOL, Cod5Activate)(ULONG uBdID,const UCHAR ActivationCode[CMN5_ACTIVATION_CODE_SIZE]) PURE;

	STDMETHOD_(BOOL, Cod5Setup)(void) PURE;
	STDMETHOD_(BOOL, Cod5Endup)(void) PURE;
	STDMETHOD_(BOOL, Cod5Run)(void) PURE;
	STDMETHOD_(BOOL, Cod5Stop)(void) PURE;

	STDMETHOD_(BOOL, Cod5ChannelEnable)(ULONG uBdID, ULONG uCh, BOOL bEnable) PURE;

	STDMETHOD_(BOOL, Cod5SetCaptureMethod)(ULONG uFuncType, ULONG uMethod) PURE;
	STDMETHOD_(BOOL, Cod5SetCallback)(ULONG uFuncType, CMN5_CALLBACK_ONE_PARAM fStart, CMN5_CALLBACK_TWO_PARAM fCall, CMN5_CALLBACK_ONE_PARAM fStop, void *pContext) PURE;

	STDMETHOD_(BOOL, Cod5QueryData)(void *pData, DWORD uTimeOut) PURE;
	STDMETHOD_(BOOL, Cod5ReleaseData)(const void *pData) PURE;
	STDMETHOD_(BOOL, Cod5SetEventHandle)(ULONG uType, HANDLE hHandle) PURE;
	STDMETHOD_(BOOL, Cod5GetEventHandle)(ULONG uType, HANDLE *pHandle) PURE;
	STDMETHOD_(BOOL, Cod5GetEventData)(ULONG uType, void *pData) PURE;
	STDMETHOD_(BOOL, Cod5SetEventData)(ULONG uType, void *pData) PURE;

	STDMETHOD_(BOOL, Cod5SetImageSize)(ULONG uBdID, ULONG uCh, ULONG uImageSize) PURE;
	STDMETHOD_(BOOL, Cod5GetImageSize)(ULONG uBdID, ULONG uCh, ULONG *puImageSize) PURE;
	STDMETHOD_(BOOL, Cod5SetVideoFormat)(ULONG uBdID, ULONG uFormat) PURE;
	STDMETHOD_(BOOL, Cod5GetVideoFormat)(ULONG uBdID, ULONG *puFormat) PURE;

	STDMETHOD_(BOOL, Cod5SetCodecProperty)(ULONG uBdID, ULONG uCh, ULONG uCodecPropertyCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Cod5GetCodecProperty)(ULONG uBdID, ULONG uCh, ULONG uCodecPropertyCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;
	STDMETHOD_(BOOL, Cod5SetCodecAdjust)(ULONG uBdID, ULONG uCh, ULONG uCodecAdjustCmd, UDA_ULONG uParam0, UDA_ULONG uParam1, UDA_ULONG uParam2, UDA_ULONG uParam3) PURE;
	STDMETHOD_(BOOL, Cod5GetCodecAdjust)(ULONG uBdID, ULONG uCh, ULONG uCodecAdjustCmd, UDA_ULONG *puParam0, UDA_ULONG *puParam1, UDA_ULONG *puParam2, UDA_ULONG *puParam3) PURE;

	STDMETHOD_(BOOL, Cod5SetMultiCodecProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cod5GetMultiCodecProperties)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cod5SetMultiCodecAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;
	STDMETHOD_(BOOL, Cod5GetMultiCodecAdjusts)(ULONG uNumCmd, CMN5_MULTI_COMMAND *pCmds) PURE;

	STDMETHOD_(BOOL, Cod5GetVideoStatus)(ULONG uBdID, ULONG uMaxCh, ULONG *pbwData) PURE;

	STDMETHOD_(BOOL, Cod5SetDO)(ULONG uBdID, ULONG uMaxDO,const ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cod5GetDO)(ULONG uBdID, ULONG uMaxDO, ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cod5GetDI)(ULONG uBdID, ULONG uMaxDI, ULONG *pbwData) PURE;
	STDMETHOD_(BOOL, Cod5SetExtVideoOut)(ULONG uBdID, ULONG uCh) PURE;
	STDMETHOD_(BOOL, Cod5GetExtVideoOut)(ULONG uBdID, ULONG *puCh) PURE;
	STDMETHOD_(BOOL, Cod5SetWatchdog)(ULONG uBdID, ULONG uWatchdogCmd, ULONG uParam0, ULONG uParam1) PURE;

	STDMETHOD_(BOOL, Cod5I2CTransfer)(ULONG uBdID, ULONG uVPID, ULONG uDevID, ULONG uI2CCmd, ULONG uAddr, UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Cod5WriteUserEEPROM)(ULONG uBdID, ULONG uVPID,const UCHAR *pData) PURE;
	STDMETHOD_(BOOL, Cod5ReadUserEEPROM)(ULONG uBdID, ULONG uVPID, UCHAR *pData) PURE;

	// *********************************************************************** 
	// Vendoer Specific IO API
	// *********************************************************************** 
	STDMETHOD_(BOOL, Cod5SetSpecialIOMode)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Cod5GetSpecialIOMode)(ULONG uBdID, ULONG *puData) PURE;
	STDMETHOD_(BOOL, Cod5WriteSpecialIOData)(ULONG uBdID, ULONG uData) PURE;
	STDMETHOD_(BOOL, Cod5ReadSpecialIOData)(ULONG uBdID, ULONG *puData) PURE;
};

#endif

#pragma pack( pop, COD5BOARDLIB_EX_H )


#endif