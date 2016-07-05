#ifndef _VCA_METARENDERAPI_H_
#define _VCA_METARENDERAPI_H_

#include <ddraw.h>

#ifdef _VMR_EXPORT
#define VMR_API  __declspec(dllexport) __stdcall
#else
#define VMR_API  __declspec(dllimport) __stdcall
#endif

#define VMR_MAX_HANDLES			16

#define VMR_MAX_NAME_LENGTH			32
#define VMR_MAX_NUM_ZONE_POINTS		40
#define VMR_MAX_NUM_ZONES			40
#define VMR_MAX_NUM_COUNTERS		20

#define VMR_NA						-1
#define VMR_ALL					0xFFFE


typedef void* VMR_HANDLE;

typedef enum
{
	VMRERR_SUCCESS			= 0x00000000,
	VMRERR_EXCEED_VMRHANDLE_LIMITS,
	VMRERR_INVALID_VMRHANDLE,
	VMRERR_ALLOCATE_MEMORY,
	VMRERR_INVALID_MODE,
	VMRERR_INVALID_ZONEID,
	VMRERR_INVALID_COUNTERID,
	VMRERR_INVALID_MODELID,
	VMRERR_INVALID_CALLBACK_PTR,
	VMRERR_INVALID_DIRECTIONARROW,
	VMRERR_INVALID_METADATA_PTR,
	VMRERR_CALLBACK_NOT_REGISTERED,
	VMRERR_CALLBACK_ALREADY_REGISTERED,
	VMRERR_INTERNAL_ERROR,
	VMRERR_INVALID_CAPS
}VMR_ERROR_E;

typedef enum
{
	VMR_ZNCTCFG			= 0x0001,
	VMR_CALIBCFG,
	VMR_CFG_RESERVED
}VMR_MODE_E;

typedef enum
{
	VOZ_ZONE			= 0x0001,
	VOZ_COUNTER,
	VOZ_NONE
}VMR_ZNCT_STATUS_E;

typedef enum 
{
	VRF_NOTHING			= 0x00000000,
	VRF_ZONES			= 0x00000001,
	VRF_COUNTERS		= 0x00000002,
	VRF_BLOBS			= 0x00000004,
	VRF_OBJECTS			= 0x00000008,
	VRF_NON_ALARMS		= 0x00000010,
	VRF_OBJECT_SPEED	= 0x00000020,
	VRF_OBJECT_HEIGHT	= 0x00000040,
	VRF_OBJECT_AREA		= 0x00000080,
	VRF_OBJECT_CLASS	= 0x00000100,
	VRF_OBJECT_WAIT_TIME= 0x00000200,
	VRF_OBJECT_COLSIG	= 0x00000400,
	VRF_OBJECT_MULTICOLOR= 0x00000800,
	VRF_CALIB_HORIZON	= 0x00001000,
	VRF_CALIB_SOLIDGRID	= 0x00002000,
	VRF_SYSMSG			= 0x10000000,
	VRF_ALL				= 0xFFFFFFFF
}VMR_RENDER_FLAGS_E; 

typedef enum 
{
	VRC_BLOB			= 0x00000000,
	VRC_ALARM,
	VRC_NONALARM,
	VRC_NUM_RENDERCOLORS
}VMR_RENDER_COLORS_E; 

// All point values are normalized to 16bits, which is 0-65535
typedef struct
{
	unsigned short	x;		// 16-bit
	unsigned short	y;		// 16-bit
}VMR_POINT_T;

typedef enum
{
	VMR_ALARM_ZONE	= 0x0001,
	VMR_PREALARM_ZONE,
	VMR_NONDETECTION_ZONE
}VMR_ZONE_TYPE_E;

typedef enum
{
	VMR_ZONE_LINE		= 0x0001,
	VMR_ZONE_POLYGON	= 0x0002
}VMR_ZONE_STYLE_E;

typedef enum
{
	VMR_ZONE_DISPLAY_DIRECTION_ARROWS	= 0x0000001,
	VMR_ZONE_DISPLAY_LINECOUNTER_A		= 0x0000002,
	VMR_ZONE_DISPLAY_LINECOUNTER_B		= 0x0000004,
}VMR_ZONE_DISPLAY_FLAGS;

typedef enum
{
	VMR_OBJECT_SELECTED	= 0x0001,
	VMR_OBJECT_HOVERED	= 0x0002
}VMR_OBJECT_STATUS_E;

typedef struct
{
	unsigned short		usZoneId;		// [0, VMR_MAX_NUM_ZONES-1]
	VMR_ZONE_TYPE_E		eZoneType;		// VMR_ZONE_TYPE_E
	VMR_ZONE_STYLE_E	eZoneStyle;		// VMR_ZONE_STYLE_E
	char				strZoneName[VMR_MAX_NAME_LENGTH];	// Zone Name
	unsigned int		uiColor;		// RGB color
	unsigned char		ucDisplay;		// Zone visible?
	unsigned int		uiTotalPoints;	// [0, VMR_MAX_NUM_ZONE_POINTS-1]
	VMR_POINT_T			pPoints[VMR_MAX_NUM_ZONE_POINTS];	// Zone points
	float				fStartAngle;	// Start acceptance angle in degress, [-720.00, 720.00]
	float				fFinishAngle;	// Finish acceptance angle in degrees, [-720.00, 720.00]
	unsigned int		uiCalibrationWidth;	// LineCounter calibration width
	unsigned int		uiDisplayFlags;	// Zone display options (display direction arrows, etc)
	unsigned int		uiStatus;		// Zone status, selected/hovered?
}VMR_ZONE_T;

typedef struct
{
	unsigned short		usCounterId;						// [0, VMR_MAX_NUM_COUNTERS-1]
	char				strCounterName[VMR_MAX_NAME_LENGTH];// CounterName
	unsigned int		uiColor;							// RGB color
	unsigned char		ucDisplay;							// Show counter?
	RECT				rcRegion;							// Rectangle region to display counter result	
	int					iCounterValue;						// Counter Value
	unsigned int		uiStatus;							// Counter status, selected/hovered?
}VMR_COUNTER_T;

typedef struct
{
	VMR_ZNCT_STATUS_E		eType;
	int						iObjectIdx;
	int						iNodeIdx;
}VMR_ZNCT_STATUS_T;


typedef struct
{
	float fHeight;
	float fTilt;
	float fRoll;
	float fFOV;
	float fPan;

}VMR_CALIB_INFO_T;

typedef struct
{
	unsigned short	usModelId;
	unsigned int	uiColor;
	VMR_POINT_T		tPosition;
}VMR_MODEL_INFO_T;

typedef enum
{
	VMR_CALLBACK_NONE		=	0x0000,
	VMR_CALLBACK_ZONE		=	0x0001,
	VMR_CALLBACK_COUNTER	=	0x0002,
	VMR_CALLBACK_CALIBMODEL	=	0x0100,
	VMR_CALLBACK_CALIBPARAM	=	0x0200,
	VMR_CALLBACK_ALL		=	0xFFFF
}VMR_CALLBACK_FLAGS_E;

typedef enum
{
	VMR_ZONE_ADD		=	0x00000001,
	VMR_ZONE_CHG		=	0x00000002,
	VMR_ZONE_DEL		=	0x00000004,
	VMR_ZONE_UPD		=	0x00000008,
	VMR_ZONE_RULE		=	0x00000010,
	VMR_ZONE_MSK		=	0x0000001F,


	VMR_COUNTER_ADD		=	0x00000100,
	VMR_COUNTER_CHG		=	0x00000200,
	VMR_COUNTER_DEL		=	0x00000400,
	VMR_COUNTER_UPD		=	0x00000800,
	VMR_COUNTER_MSK		=	0x00000F00,

	VMR_CALIBMODEL_ADD	=	0x01000000,
	VMR_CALIBMODEL_CHG	=	0x02000000,
	VMR_CALIBMODEL_DEL	=	0x04000000,
	VMR_CALIBMODEL_UPD	=	0x08000000,
	VMR_CALIBMODEL_MSK	=	0x0F000000,

	VMR_CALIBPARAM_ADD	=	0x10000000,
	VMR_CALIBPARAM_CHG	=	0x20000000,
	VMR_CALIBPARAM_DEL	=	0x40000000,
	VMR_CALIBPARAM_UPD	=	0x80000000,
	VMR_CALIBPARAM_MSK	=	0xF0000000
}VMR_CALLBACK_STATUS_E;

typedef void (__stdcall *VMR_CB)( VMR_HANDLE hVMRHandle, unsigned int uiCBStatus, void *pData, void *pUserData );

typedef struct
{
	unsigned int	uiFlags;
	void			*pUserData;
	VMR_CB			pCB;
}VMR_CALLBACK_INFO_T;

#ifdef __cplusplus
extern "C"
{
#endif

VMR_ERROR_E VMR_API VMR_InitVCAMetaRender( VMR_HANDLE *hhVMRHandle );
VMR_ERROR_E VMR_API VMR_DeInitVCAMetaRender( VMR_HANDLE *hhVMRHandle );
VMR_ERROR_E VMR_API VMR_SetMode( VMR_HANDLE hVMRHandle, VMR_MODE_E eMode );

// for configuration using keyboard/mouse
VMR_ERROR_E VMR_API VMR_OnMessage( VMR_HANDLE hVMRHandle, HWND hWnd, UINT nCode, WPARAM wParam, LPARAM lParam );

// set rendering flags
VMR_ERROR_E	VMR_API VMR_SetRenderingFlags( VMR_HANDLE hVMRHandle, int nRenderFlags );

// color part
VMR_ERROR_E	VMR_API VMR_SetColor( VMR_HANDLE hVMRHandle, VMR_RENDER_COLORS_E eColorIdx, COLORREF crColor );

// metadata part
VMR_ERROR_E	VMR_API VMR_SetMetaData( VMR_HANDLE hVMRHandle, unsigned char *pMetadata, int nLength );
VMR_ERROR_E	VMR_API	VMR_ResetMetaData( VMR_HANDLE hVMRHandle );

// render
VMR_ERROR_E VMR_API VMR_Render( VMR_HANDLE hVMRHandle, LPDIRECTDRAWSURFACE7 lpDDDstSurface, LPRECT lpDstRect );

// set font size
VMR_ERROR_E VMR_API VMR_SetFontSize( VMR_HANDLE hVMRHandle, int nFontSize );

// register/unregister callback function
VMR_ERROR_E	VMR_API VMR_RegisterCallBack( VMR_HANDLE hVMRHandle, VMR_CALLBACK_INFO_T *pCallBackInfo );
VMR_ERROR_E	VMR_API VMR_UnRegisterCallBack( VMR_HANDLE hVMRHandle );

/**************** The following parts are valid when eMode == VMR_ZNCTCFG **********************/
// zone part
VMR_ERROR_E	VMR_API VMR_SetZone( VMR_HANDLE hVMRHandle, VMR_ZONE_T *pZone );
VMR_ERROR_E VMR_API VMR_RemoveZone( VMR_HANDLE hVMRHandle, unsigned short usZoneId );
VMR_ERROR_E VMR_API VMR_GetZone( VMR_HANDLE hVMRHandle, VMR_ZONE_T *pZone );

// counter part
VMR_ERROR_E	VMR_API VMR_SetCounter( VMR_HANDLE hVMRHandle, VMR_COUNTER_T *pCounter );
VMR_ERROR_E VMR_API VMR_RemoveCounter( VMR_HANDLE hVMRHandle, unsigned short usCounterId );
VMR_ERROR_E VMR_API VMR_GetCounter( VMR_HANDLE hVMRHandle, VMR_COUNTER_T *pCounter );

// get/set the current selected/hovered parts, zone/counter? node?
VMR_ERROR_E VMR_API VMR_GetObject( VMR_HANDLE hVMRHandle, VMR_OBJECT_STATUS_E eObject, VMR_ZNCT_STATUS_T *pObject );
VMR_ERROR_E VMR_API	VMR_SetObject( VMR_HANDLE hVMRHandle, VMR_OBJECT_STATUS_E eObject, VMR_ZNCT_STATUS_T *pObject );

// object alarm timer part
VMR_ERROR_E VMR_API VMR_SetAlarmTimer( VMR_HANDLE hVMRHandle, long nDwellTime, long nFadeTime );	// in milliseconds

/*************** The following parts are valid when eMode == VMR_CALIBCFG **********************/
VMR_ERROR_E	VMR_API VMR_SetCalibParams( VMR_HANDLE hVMRHandle, VMR_CALIB_INFO_T *pCalibInfo );
VMR_ERROR_E VMR_API VMR_GetCalibParams( VMR_HANDLE hVMRHandle, VMR_CALIB_INFO_T *pCalibInfo );
VMR_ERROR_E VMR_API VMR_SetModel( VMR_HANDLE hVMRHandle, VMR_MODEL_INFO_T *pModelInfo );
VMR_ERROR_E VMR_API VMR_CentreModels( VMR_HANDLE hVMRHandle );
VMR_ERROR_E VMR_API VMR_RemoveModel( VMR_HANDLE hVMRHandle, unsigned short usModelId );

#ifdef __cplusplus
}
#endif

#endif // _VCA_METARENDERAPI_H_