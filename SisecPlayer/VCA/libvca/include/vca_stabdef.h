#ifndef _vca_stabdef_h_
#define _vca_stabdef_h_

#ifndef NORMALISATION_RANGE
#define NORMALISATION_RANGE 65535
#endif

typedef enum 
{
	VCASTAB_OUTFORMAT_FULL_YUV = 0,	
	VCASTAB_OUTFORMAT_FULL_GRAY,
	VCASTAB_OUTFORMAT_SPLITSCREEN_DEMO,		
	VCASTAB_OUTFORMAT_SPLITSCREEN_DEBUG,		
	VCASTAB_OUTFORMAT_HALF_GRAY = 100,
	VCASTAB_OUTFORMAT_QUARTER_GRAY,
	VCASTAB_OUTFORMAT_HALF_GRAY_PLUS_UV,
	VCASTAB_OUTFORMAT_FULL_GRAY_PLUS_UV,
//JC : Support any size in Win32
	VCASTAB_OUTFORMAT_ANY_GRAY,	
	VCASTAB_OUTFORMAT_ANY_GRAY_PLUS_UV,	//Current does not support 
} eVCASTAB_OutputFormat;

typedef enum 
{
	VCASTAB_FADESPEED_OFF = 0,
	VCASTAB_FADESPEED_FAST,	
	VCASTAB_FADESPEED_SLOW
	

} eVCASTAB_FadeSpeed;

typedef enum
{
	VCASTAB_STABMODE_STATIC = 0,
	VCASTAB_STABMODE_DYNAMIC,
	VCASTAB_STABMODE_SLOW

} eVCASTAB_StabMode;

typedef enum
{
	VCASTAB_ALGOPERFORMANCE_FASTEST 	= 0,
	VCASTAB_ALGOPERFORMANCE_BALANCED    	= 1,
	VCASTAB_ALGOPERFORMANCE_BEST 		= 100

} eVCASTAB_AlgoPerformance;

typedef enum
{
	VCASTAB_VIDEOSTANDARD_NTSC,
	VCASTAB_VIDEOSTANDARD_PAL

} eVCASTAB_VideoStandard;

typedef enum
{
	VCASTAB_CONTENTTYPE_PROGRESSIVE,
	VCASTAB_CONTENTTYPE_INTERLACED

} eVCASTAB_ContentType;

typedef enum 
{
	VCASTAB_RESET_NONE 	= 0,
	VCASTAB_RESET_LOWDETAIL,	
	VCASTAB_RESET_SCENECHANGE,
	VCASTAB_RESET_STABDISABLED,
	VCASTAB_RESET_PTZMOVEMENT,
	VCASTAB_RESET_ATTEMPTING_TO_REAQUIRE,
	
} eVCASTAB_Reset;

#define	VCASTAB_ALGO_DETECT_BORDER		0x1
#define VCASTAB_ALGO_STATIC_BORDER		0x2
#define VCASTAB_ALGO_INFILL_BORDER		0x4 
#define VCASTAB_ALGO_DISABLE_STAB		0x8
#define VCASTAB_ALGO_DISABLE_ON_PANTILT		0x10
#define VCASTAB_ALGO_DISABLE_ON_LOWDETAIL 	0x20
#define VCASTAB_ALGO_ENABLE_RESET		0x40  // Enable re-centering image when theres a scene change

////// !!!!!DO NOT USE THIS UNLESS YOU KNOW BETTER!!!!! //////
#define VCASTAB_ALGO_VCA_MODE			0x80000000
 /////////////////////////////////////////////////////////////

#define VCASTAB_FIELDORDER_BOTTOM_FIRST 	0
#define VCASTAB_FIELDORDER_TOP_FIRST		1

//// config options used by the codec
typedef struct
{
	eVCASTAB_OutputFormat 		OutFormat;
	unsigned			AlgoOptions;
	eVCASTAB_FadeSpeed 		FadeSpeed;
	eVCASTAB_StabMode		StabMode;
	eVCASTAB_AlgoPerformance	AlgoPerform;	
	int	 			iMaxXShift;
	int		 		iMaxYShift;

	int 				FieldOrder;			/// 0 = bottom field first(defualt), 1 = top first		
	eVCASTAB_ContentType 		OutType;			/// output video mode: VCASTAB_CONTENTTYPE_INTERLACED or VCASTAB_CONTENTTYPE_PROGRESSIVE				

	int				iWidthResizeFactor;
	int				iHeightResizeFactor;

}sVCASTAB_ConfigOptions;

typedef struct
{
	unsigned		bDoLowResSearch;		// do low resolution search first 
	unsigned		bUseSmallTemplates;		// use smaller templates	  
	unsigned		uTemplateSkip;		 	// Skip templates (use fewer templates: 1=use all, 2=use half, 4=use a quarter)
	unsigned		uMaxSearchIterations;		// Max num iterations for high res search 

}sVCASTAB_PerformanceProfile;

/// config options used by the user-interface
typedef struct
{
	eVCASTAB_OutputFormat 		OutFormat;
	unsigned			AlgoOptions;
	eVCASTAB_FadeSpeed 		FadeSpeed;
	eVCASTAB_StabMode		StabMode;
	eVCASTAB_AlgoPerformance	AlgoPerform;
	int	 			iMaxXShift;
	int		 		iMaxYShift;

	eVCASTAB_VideoStandard		VideoStandard;
	unsigned			bEnableOSD;
	unsigned			bStabEnabled;
}STAB_UIOptions;


typedef struct
{
	int x;
	int y;
} sPoint;


typedef struct 
{
	sPoint 		displacementFlt[2];
	eVCASTAB_Reset 	reset_status[2];
	unsigned char	*pOutBytes;

}sStab_RetData;

typedef struct
{
	int	fEnable;			// > 1 = STAB ON, 0 = STAB OFF
}
VCA_STAB_ENABLE_T;


#endif /// _vca_stabdef_h_
