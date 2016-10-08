/** $Id$ $DateTime$
*  @file  dvxRemoteConfigInterface.h
*  @brief 全功能配置UI部分接口的声明文件
*  @version 0.0.1
*  @since 0.0.1
*  @author wubo<wubo@bstar.com.cn>
*  @date 2012-8-6   Created it
*/
/******************************************************************************
*@note
Copyright 2007, BeiJing Bluestar Corporation, Limited
ALL RIGHTS RESERVED
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products
to use or abstract this computer program for the sole purpose of implementing
a product based on BeiJing Bluestar, Inc. products. No other rights to
reproduce, use, or disseminate this computer program,whether in part or  in
whole, are granted. BeiJing Bluestar, Inc. makes no representation or
warranties with respect to the performance of this computer program, and
specifically disclaims any responsibility for any damages, special or
consequential, connected with the use of this program.
For details, see http://www.bstar.com.cn/
******************************************************************************/

#ifndef __DVXREMOTE_CONFIG_INTERFACE_H__
#define __DVXREMOTE_CONFIG_INTERFACE_H__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef DVXREMOTECONFIG_EXPORTS
#define DVXREMOTECONFIG_API EXTERNC __declspec(dllexport)	
#else	// else DVXREMOTECONFIG_EXPORTS
#define DVXREMOTECONFIG_API EXTERNC __declspec(dllimport)
#endif	// end DVXREMOTECONFIG_EXPORTS



//显示的题签页类型
#define DVXREMOTE_CONFIG_PAGE_PLAYBACK    (0x10000000)    // 支持回放、下载页

DVXREMOTECONFIG_API int dvxRemoteConfig( HANDLE hDvx, const char* szUser, const char* szAddr, LONG nLangID, int nFlag=0 );

#endif	// end of __DVXREMOTE_CONFIG_INTERFACE_H__