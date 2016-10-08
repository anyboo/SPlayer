/** $Id$ $DateTime$
 *  @file  dvxConfigInterface.h
 *  @brief DvxSdk的全功能配置UI部分接口的声明文件
 *  @version 0.0.1
 *  @since 0.0.1
 *  @author SongZhanjun<songzhj@bstar.com.cn>
 *  @date 2009-8-6    Created it
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

#ifndef __TULIPB_CONFIG_INTERFACE_H__
#define __TULIPB_CONFIG_INTERFACE_H__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC 
#endif

#ifdef BSRDVRSDK_EXPORTS
#define DVXSDKCONFIG_API EXTERNC __declspec(dllexport)	
#else	// else BSRDVRSDK_EXPORTS
#define DVXSDKCONFIG_API EXTERNC __declspec(dllimport)
#endif	// end BSRDVRSDK_EXPORTS



//显示的题签页类型
#define DVX_CONFIG_PAGE_PLAYBACK    (0x10000000)    // 支持回放、下载页

DVXSDKCONFIG_API int TulipBConfig( DvxHandle hDvr, const char* szUser, const char* szAddr, LONG nLangID, int nFlag=0, BOOL bOnlyPlay = FALSE, unsigned __int64  npurview = 1 );

#endif	// end of__TULIPB_CONFIG_INTERFACE_H__
