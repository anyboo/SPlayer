#ifndef __VCA_FEATUREDEF_H__
#define __VCA_FEATUREDEF_H__



#define VCA_FEATURE_UNLIMITED	0xffffffff

typedef enum
{
	VCA_FEATURE_DEF_NULL,
	VCA_FEATURE_DEF_VCASYS,
	VCA_FEATURE_DEF_VCASTAB,
}
VCA_FEATURE_STRUCT_TYPE_E;

typedef struct tVCABASICFEATURES
{
	unsigned char	pData[64];		// 128 bytes placeholder
}
VCA_BASIC_FEATURES_T;

/*
DO NOT CHANGE THE ORDER OF THESE .
ADD NEW FEATURES BEFORE VCA_SYS_FEATURE_COUNTING

MAKE SURE THE ORDER OF THERSE ARE THE SAME AS VCA_RULE_E 
 */
typedef enum 
{
	VCA_SYS_FEATURE_PRESENCE	= 0x0001,
	VCA_SYS_FEATURE_ENTER,
	VCA_SYS_FEATURE_EXIT,
	VCA_SYS_FEATURE_APPEAR,
	VCA_SYS_FEATURE_DISAPPEAR,
	VCA_SYS_FEATURE_STOPPED,
	VCA_SYS_FEATURE_DWELL,
	VCA_SYS_FEATURE_DIRECTION,
	VCA_SYS_FEATURE_SPEED,
	VCA_SYS_FEATURE_AREA,
	VCA_SYS_FEATURE_TAILGATING,
	VCA_SYS_FEATURE_ABOBJ,
	VCA_SYS_FEATURE_LINECOUNTER,
	VCA_SYS_FEATURE_LINECOUNTER_DUMMY,
	VCA_SYS_FEATURE_RMOBJ,
	VCA_SYS_FEATURE_COUNTING,
	VCA_SYS_FEATURE_CALIBRATION,
	VCA_SYS_FEATURE_PEOPLETRACKING,
	VCA_SYS_FEATURE_AUTOTRACKING,
	VCA_SYS_FEATURE_AUTOTRACKINGLAYON,
	VCA_SYS_FEATURE_CAMSHAKE,
	VCA_SYS_FEATURE_TAMPER,
	VCA_SYS_FEATURE_TRACKINGENGINE,
	VCA_SYS_FEATURE_BETA,
	VCA_SYS_FEATURE_SMOKE,
	VCA_SYS_FEATURE_FIRE,
	VCA_SYS_FEATURE_ABRMOBJ
}
VCA_SYS_FEATURES_E;


typedef enum 
{
	VCA_FEATURE_BIT_PRESENCE		= 0x00000002,
	VCA_FEATURE_BIT_ENTER			= 0x00000004,
	VCA_FEATURE_BIT_EXIT			= 0x00000008,
	VCA_FEATURE_BIT_APPEAR			= 0x00000010,
	VCA_FEATURE_BIT_DISAPPEAR		= 0x00000020,
	VCA_FEATURE_BIT_STOPPED			= 0x00000040,
	VCA_FEATURE_BIT_DWELL			= 0x00000080,
	VCA_FEATURE_BIT_DIRECTION		= 0x00000100,
	VCA_FEATURE_BIT_SPEED			= 0x00000200,
	VCA_FEATURE_BIT_ABOBJ			= 0x00000400,
	VCA_FEATURE_BIT_TAILGATING		= 0x00000800,
	VCA_FEATURE_BIT_LINECOUNTER		= 0x00001000,
	//reserved for filter 0-15 bit and 16-31 bit for other functions
	VCA_FEATURE_BIT_COUNTING		= 0x00010000,
	VCA_FEATURE_BIT_CALIBRATION		= 0x00020000,
	VCA_FEATURE_BIT_METADATA		= 0x00040000
}
VCA_FEATURES_BIT_E;


typedef struct tVCASYSFEATURES
{
	//--------------------------------------
	// VCASys features
	unsigned	fPresence		: 1;
	unsigned	fEnter			: 1;
	unsigned	fExit			: 1;
	unsigned	fAppear			: 1;
	unsigned	fDisappear		: 1;
	unsigned	fStopped		: 1;
	unsigned	fDwell			: 1;
	unsigned	fDirection		: 1;
	unsigned	fSpeed			: 1;
	unsigned 	fCounting		: 1;
	unsigned	fCalibration	: 1;
	unsigned	fAbObj 			: 1;
	unsigned	fTailgating		: 1;
	unsigned	fAutotracking	: 1;
	unsigned	fATLayOn		: 1;
	unsigned	fCamShake		: 1;
	unsigned	fTamper			: 1;
	unsigned	fTrackingEngine	: 1;
	unsigned	fSmoke			: 1;
	unsigned	fFire			: 1;
	unsigned	fSpare1			: 12;	// Round up to 32-bits
	//--------------------------------------
	// VCAretail features
	unsigned	fCountingLine	: 1;
	unsigned	fPeopleTracking : 1;
	unsigned	fSpare2			: 30;
	//--------------------------------------

	//--------------------------------------
	// Global features
	unsigned		fEvaluation : 1;
	unsigned		fMetadata	: 1;
	unsigned		fBeta		: 1;
	unsigned		fSpare3		: 29;

	//---------------------------------------
	// End of features marker
	// Don't put anything other than features before this line
	unsigned char		ucEndFeaturesMarker;

	unsigned int	uiNumZones;
	unsigned int	uiNumRules;
	unsigned int	uiNumCounters;
	unsigned int	uiNumChannels;
}
VCA_SYS_FEATURES_T;

typedef struct tVCASTABFEATURES
{
	unsigned	fAllOrNothing	: 1;
	unsigned	fEvaluation		: 1;
	unsigned	fSpare			: 30;

	unsigned int	uiNumChannels;
}
VCA_STAB_FEATURES_T;


typedef struct tVCAFEATURESUPPORT
{
	VCA_FEATURE_STRUCT_TYPE_E	eFeatureType;
	VCA_BASIC_FEATURES_T		tFeatures;
		
}
VCA_FEATURE_SUPPORT_T;

typedef struct tVCALICENSEPROPERTIES
{
	char			szLicenseDesc[128];
	VCA_FEATURE_SUPPORT_T	tFeatureSupport;
}
VCA_LICENSE_PROPERTIES_T;


#endif // __VCA_FEATUREDEF_H__


