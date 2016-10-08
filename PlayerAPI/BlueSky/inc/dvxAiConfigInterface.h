

#ifndef __DVX_AICONFIG_INTERFACE_H__
#define __DVX_AICONFIG_INTERFACE_H__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef DVXAICONFIG_EXPORTS
#define DVXAICONFIG_API EXTERNC __declspec(dllexport)	
#else	// else BSRDVRSDK_EXPORTS
#define DVXAICONFIG_API EXTERNC __declspec(dllimport)
#endif	// end BSRDVRSDK_EXPORTS


DVXAICONFIG_API int dvxAiConfig( HANDLE hIpc, void *pParent, void **pAiPage ,LONG nLangID);

#endif	// end of __DVX_AICONFIG_INTERFACE_H__
