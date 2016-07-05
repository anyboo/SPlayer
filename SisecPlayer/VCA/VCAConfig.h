#pragma once

//---------------------------------------------------------------------------------
// {6F1970C8-60D1-4ea5-8AB4-A6CB6A06B746}
DEFINE_GUID(IID_IVCAConfig, 
0x6f1970c8, 0x60d1, 0x4ea5, 0x8a, 0xb4, 0xa6, 0xcb, 0x6a, 0x6, 0xb7, 0x46);

// {23246213-2399-4187-90AB-7EA6894FE094}
DEFINE_GUID(IID_IVCADataSource, 
0x23246213, 0x2399, 0x4187, 0x90, 0xab, 0x7e, 0xa6, 0x89, 0x4f, 0xe0, 0x94);

// {5E9F8272-BA8B-4596-94B9-8043BDA279C7}
DEFINE_GUID(IID_IVCAMediaSample, 
0x5e9f8272, 0xba8b, 0x4596, 0x94, 0xb9, 0x80, 0x43, 0xbd, 0xa2, 0x79, 0xc7);

// {CB028208-9A42-42dd-9925-22927AAE1C74}
DEFINE_GUID(IID_IVCAMediaBundle, 
0xcb028208, 0x9a42, 0x42dd, 0x99, 0x25, 0x22, 0x92, 0x7a, 0xae, 0x1c, 0x74);

//---------------------------------------------------------------------------------

typedef struct
{
	BITMAPINFOHEADER	bmih;
}
VCACONFIG_MEDIAVIDEOHDR;

typedef struct
{
}
VCACONFIG_MEDIAMETAHDR;

typedef enum
{
	VCACONFIG_MEDIA_VIDEO		= 0x00000001,
	VCACONFIG_MEDIA_METADATA	= 0x00000002,
	VCACONFIG_MEDIA_XXX			= 0x00000004
}
VCACONFIG_MEDIATYPES;

typedef struct
{
	VCACONFIG_MEDIATYPES	mediaType;
	union
	{
		VCACONFIG_MEDIAVIDEOHDR		videoHdr;
		VCACONFIG_MEDIAMETAHDR		metaHdr;
	}
	mediaHdr;

	__int64							timeStamp;
}
VCACONFIG_MEDIASAMPLEHDR;


typedef enum
{
	VCACONFIG_SRC_NULL,
	VCACONFIG_SRC_DSHOW,				// Provided by DShow URL
	VCACONFIG_SRC_CAP5,					// Provided by UDP Cap card
	VCACONFIG_SRC_VLC,					// Provided by VLC player
	VCACONFIG_SRC_IF					// Provided by calls over the interface
}
VCACONFIG_SRC_TYPE;

typedef struct
{
	int					iChannelId;
	unsigned int		fMediaTypeFlags;	// Which types of media will be provided (meta, video, etc)
	VCACONFIG_SRC_TYPE	videoSrcType;		// Type of video source
	BITMAPINFOHEADER	bmih;				// Detailed info about video provided
	char				szVideoData[1024];	// Data corresponding to source type (e.g. DShow URL, etc)
}
VCACONFIG_CHANNEL_SRC;




//--------------------------------------------------------------------------------------------------------

interface IVCADataSource;

//--------------------------------------------------------------------------------------------------------

interface IVCAMediaSample : IUnknown
{
	STDMETHOD( Create( int iLen ) ) PURE;
	STDMETHOD( GetDataPtr( void **ppvMem, int &iLen ) ) PURE;
	STDMETHOD( GetHeader( VCACONFIG_MEDIASAMPLEHDR &hdr ) ) PURE;
	STDMETHOD( SetHeader( VCACONFIG_MEDIASAMPLEHDR &hdr ) ) PURE;
};

//--------------------------------------------------------------------------------------------------------

interface IVCAMediaBundle : IUnknown
{
	STDMETHOD( AddSample( IVCAMediaSample *pSample ) ) PURE;
	STDMETHOD( GetSamplePtr( int iSampleIdx, IVCAMediaSample **ppSample ) ) PURE;
	STDMETHOD( GetNumSamples( int &iNumSamples ) ) PURE;
};

//--------------------------------------------------------------------------------------------------------

// Interface provided by VCAConfigApp
interface IVCAConfig : IUnknown
{
	// Tell the app that the config has been changed elsewhere - the app will then re-call GetConfig on you and update itself
	STDMETHOD( OnConfigUpdated( IVCADataSource *pSrc, int iChannel ) ) PURE;

	// Deliver some media to the parent app
	STDMETHOD( DeliverMedia( IVCADataSource *pSrc, int iChannel, IVCAMediaBundle *pBun ) ) PURE;

};


//---------------------------------------------------------------------------------------------------------

interface IVCADataSource : IUnknown
{
	// Notification from parent app that the world has started
	STDMETHOD( OnConnected( IVCAConfig *pConfig ) ) PURE;

	// Notification from the parent app that the world is ending
	STDMETHOD( OnDisconnected( IVCAConfig *pConfig) ) PURE;

	// Get the number of channels supported
	STDMETHOD( GetNumChannels( int &iNumChannels ) ) PURE;

	// Get the source assignment for each channel (is it dshow, etc)
	STDMETHOD( GetChannelSource( int iChannel, VCACONFIG_CHANNEL_SRC *pSrc ) ) PURE;

	// Start the stream on channel x
	STDMETHOD( StartStream( int iChannel ) ) PURE;

	// Stop the stream on channel x
	STDMETHOD( StopStream( int iChannel ) ) PURE;

	// Parent app requests the config from you for the specified channel
	STDMETHOD( GetConfig( int iChannel, char *pszConfig, int iMaxSize, int &iRetSize ) ) PURE;

	// The user has changed the config - you need to know
	STDMETHOD( OnConfigUpdated( int iChannel, char *pszConfig, int iLen ) ) PURE;
};
