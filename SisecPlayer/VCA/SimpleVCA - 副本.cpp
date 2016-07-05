// StartVCA5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <atlfile.h>

#define _MEMDUMPERVALIDATOR_H

#include "Cap5BoardLibEx.h"
#include "SimpleVCA.h"
#include "VCAMetaLib.h"
#include "GDIViewer.h"
//#include "VideoSource/VCAVideoSource.h"
#include "VCAConfigure.h"


CGDIViewer		g_GDIViewer;
//extern IVCAVideoSource	*g_pVideoSource;

#define DbgMsg TRACE

#define	_V(x) \
	do { \
		if (x==0) { \
			TRACE("%s return 0\n", #x); \
			return FALSE; \
		} \
	} while (0)




DWORD	ConvertGDI2VCAColorType(DWORD VCA5ColorType)
{
	if(VCA5_COLOR_FORMAT_RGB24 == VCA5ColorType ) return CGDIViewer::COLORTYPE_RGB24;
	else if(VCA5_COLOR_FORMAT_RGB16 == VCA5ColorType ) return CGDIViewer::COLORTYPE_RGB16;
	else if(VCA5_COLOR_FORMAT_YUY2 == VCA5ColorType ) return CGDIViewer::COLORTYPE_YUY2;
	else if(VCA5_COLOR_FORMAT_UYVY == VCA5ColorType ) return CGDIViewer::COLORTYPE_UYVY;
	else if(VCA5_COLOR_FORMAT_YV12 == VCA5ColorType ) return CGDIViewer::COLORTYPE_YV12;

	return CGDIViewer::COLORTYPE_UNKNOWN;
}

 BOOL  CSimpleVCA::Uda5CreateInstance(HMODULE hLib, REFIID riid, void ** ppInterface)
{
	if (hLib) {

		BOOL rs;
		IUnknown* pUnknown;
		BOOL (FAR WINAPI*_CreateInstance)(IUnknown ** ppInterface);
		FARPROC test_proc=GetProcAddress(hLib,"VCA5CreateInstance");
		if (test_proc) {
			*(FARPROC*)&_CreateInstance=test_proc;
			rs=(*_CreateInstance)(&pUnknown);
			if (rs) {
				HRESULT hr;
				hr=pUnknown->QueryInterface(riid,ppInterface);
				pUnknown->Release();
				if (SUCCEEDED(hr))
					return TRUE;
			}
		}
	}
	
	return FALSE;
}

IVCA5*				CSimpleVCA::s_VCAApi = NULL;
bool				CSimpleVCA::s_bSetup = false;
VCA5_SYSTEM_INFO	CSimpleVCA::s_VCA5SysInfo = { 0 };
 VCA5_LICENSE_INFO CSimpleVCA::s_LicenseInfo = { 0 };
BOOL CSimpleVCA::InitVCAApi(TCHAR* szDllName, char* szLicense, char* szUSN)
{
	HMODULE hLib=NULL;
	USES_CONVERSION;

	if (szDllName) {
		_V((hLib=LoadLibrary(szDllName)));
	}

	_V(Uda5CreateInstance(hLib,IID_IVCA5,(void**)&s_VCAApi));
		
	//Set Version
	BOOL (FAR WINAPI*_VCA5Init)(ULONG ulVersion);
	FARPROC test_proc=GetProcAddress(hLib,"VCA5Init");
	if (test_proc) {
		*(FARPROC*)&_VCA5Init=test_proc;
		(*_VCA5Init)(VCA5_VERSION);
	}

	CSimpleVCA::s_LicenseInfo.szUSN = szUSN;
	CSimpleVCA::s_LicenseInfo.szLicense = szLicense;
//	g_LicenseInfo.szDrvDllPath	= T2A(szDrvDllPath);
	
	_V((s_VCAApi->VCA5Activate(1,  (VCA5_LICENSE_INFO*)&s_LicenseInfo)));
/*	if (FALSE == s_VCAApi->VCA5Activate(1, (VCA5_LICENSE_INFO*)&s_LicenseInfo))
	{
	
	}*/
	_V((s_VCAApi->VCA5GetSystemInfo(&s_VCA5SysInfo)));

	
//	DbgMsg("Licesne[%d] EngineCnt [%d] Function [0x%08X] Decription [%s] \n", 
	//	g_LicenseInfo.nLicenseID, g_LicenseInfo.ulNumOfEngine, g_LicenseInfo.ulFunction, g_LicenseInfo.szLicenseDesc);

	//Just One Engine Run
//	g_nVCAEngine	= 1;

	return TRUE;
}

void	CSimpleVCA::GetLastErrorStr(char *pErrorBuf)
{
	strcpy(pErrorBuf, "Unknown error");
	VCA5_ERROR_CODE_ITEM item;
	if (s_VCAApi->VCA5GetLastErrorCode(&item))
	{
		switch (VCA5_EXTRACT_ERRCODE2(item.ErrorCode))
		{
		case VCA5_ERRCMN_F_LICENSE_EXPIRED:
			sprintf(pErrorBuf, "license has expired : %s", item.AuxMsg);
			break;

			// Intentional durchfall...
		case VCA5_ERRCMN_F_ALREADY_ACTIVATED:{
			// Already activated));
			sprintf(pErrorBuf, "license has already activated.");
			break;
		}

		default:{
					sprintf(pErrorBuf, "Unable to activate VCA5: %s", item.AuxMsg);
			break;
		}
		}
	}
}

BOOL	CSimpleVCA::ReadLicense(LPCTSTR szFIleName, LPSTR szLincens, DWORD nBufferLen, PDWORD pdwBytesRead)
{
	CAtlFile fLicense;
	ULONGLONG nFileSize = 0;
	DWORD dwBytesRead = 0;

	fLicense.Create(szFIleName, FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	fLicense.GetSize( nFileSize );
	fLicense.Read( szLincens, nBufferLen, dwBytesRead );

	if ( pdwBytesRead )
		*pdwBytesRead = dwBytesRead;

	if ( nFileSize == dwBytesRead )
		return TRUE;

	DbgMsg("Can not Open file [%s]\n", szFIleName);

	return FALSE;
}


typedef BOOL (WINAPI* LPFCAP5QUERYINFO)(ULONG , ULONG uIn, void *);
BOOL	CSimpleVCA::VCASetup()
{
	if (s_bSetup)
	{
		return true;
	}
	TCHAR	*szVCADll=_T("VCA5Lib.dll");
	TCHAR	*szDrvDll = NULL;
	char	szLicense[1024];
	char	szUSN[16];
	TCHAR	szLicenseFilePath[255] = _T("license.lic");
	
	HMODULE hCAP5Lib	= NULL;
	LPFCAP5QUERYINFO	pCap5QueryInfo;

	memset(szLicense, 0, _countof(szLicense));
	memset(szUSN,0,_countof(szUSN));

	//Get USN Number
	if (szDrvDll) {
		_V((hCAP5Lib = LoadLibrary(szDrvDll)));
	}

	if(hCAP5Lib){
		pCap5QueryInfo = (LPFCAP5QUERYINFO)GetProcAddress(hCAP5Lib, "Cap5QueryInfo");
		_V(pCap5QueryInfo(CAP5_QR_GET_USN, 16, szUSN));
		FreeLibrary(hCAP5Lib);
	}


	DWORD dwLicenseLength = 0;
	_V((ReadLicense(szLicenseFilePath, szLicense, _countof(szLicense), &dwLicenseLength)));
	szLicense[dwLicenseLength] = 0;

	_V((InitVCAApi(szVCADll, szLicense, szUSN)));

	s_bSetup = true;

	return TRUE;
}


void	CSimpleVCA::VCAEndup()
{/*
	if(!s_nVCAEngine){
		return;
	}

	for(DWORD i=0;i<s_nVCAEngine;i++) {
		s_VCAApi->VCA5Close(i);
	}
	*/
	s_VCAApi->Release();


}

void CSimpleVCA::SetZoneRuleFlag()
{
	m_bSetZoneRuleFlag = true;
}

void CSimpleVCA::SetZoneRule()
{
	VCA5_APP_ZONES zones;
	VCA5_APP_COUNTERS counters;
	VCA5_APP_RULES rules;
	VCA5_APP_CLSOBJECTS clsObjs;
	VCA5_APP_CALIB_INFO calibInfo;

	m_pDataMgr->GetData(&zones, &rules, &counters, &calibInfo, &clsObjs);

	//Counterline rule setting
	for (ULONG i = 0; i < rules.ulTotalRules; ++i){
		if (VCA5_RULE_TYPE_LINECOUNTER_A == rules.pRules[i].usRuleType ||
			VCA5_RULE_TYPE_LINECOUNTER_B == rules.pRules[i].usRuleType){
			if (rules.pRules[i].ucWidthCalibrationEnabled == 0){
				rules.pRules[i].tRuleDataEx.tLineCounter.ulCalibrationWidth = 0;
			}
			rules.pRules[i].tRuleDataEx.tLineCounter.ulShadowFilterEnabled = rules.pRules[i].ucShadowFilterEnabled;
		}
	}

	//2. Apply Engine
	
	SetConfig(&zones, &rules, &counters);//(m_DataMgr.GetZones(), &TempRules, &TempCounters);
	

	VCA5_APP_ENGINE_CONFIG *pEngCfg = CVCAConfigure::Instance()->GetEngineConf(0);

	VCA5_APP_COUNTERS *pCounters = m_pDataMgr->GetCounters();
	for (ULONG i = 0; i < pCounters->ulTotalCounters; ++i)
	{
		if (pCounters->pCounters[i].uiStatus&VCA5_APP_COUNTER_STATUS_NOT_USED)
		{
			ClearAreaStatusBit(&(pCounters->pCounters[i]), VCA5_APP_COUNTER_STATUS_NOT_USED);
		}
		else{
			//Update counter result 
			SetControl(VCA5_CMD_SETCOUNTERRES, pCounters->pCounters[i].usCounterId, pCounters->pCounters[i].iCounterResult);
		}
	}

	m_pDataMgr->GetData(&(pEngCfg->Zones), &(pEngCfg->Rules), &(pEngCfg->Counters), &(pEngCfg->CalibInfo), &(pEngCfg->ClsObjects));

	m_pDataMgr->SetData(&(pEngCfg->Zones), &(pEngCfg->Rules), &(pEngCfg->Counters), &(pEngCfg->CalibInfo), &(pEngCfg->ClsObjects));
	m_pDataMgr->ClearCountingLineEvents();
}

void CSimpleVCA::ProcessVCA(DISPLAYCALLBCK_INFO *pInfo)
{
	if (!m_bOpen)
	{
		memset(&m_EngineParam, 0, sizeof(VCA5_ENGINE_PARAMS));
		m_EngineParam.ulColorFormat = VCA5_COLOR_FORMAT_YV12;
		m_EngineParam.ulVideoFormat = VCA5_VIDEO_FORMAT_PAL_B;//VCA5_VIDEO_FORMAT_NTSC_M
		m_EngineParam.ulLicenseCnt = 1;
		m_EngineParam.ucLicenseId[0] = 0;
		m_EngineParam.ulImageSize = CMN5_MAKEIMGSIZE(pInfo->nWidth, pInfo->nHeight);
		m_nWidth = pInfo->nWidth;
		m_nHeight = pInfo->nHeight;

		m_bOpen = VCAOpen(&m_EngineParam);

		if (m_bOpen)
		{
		/*	VCA5_TIME_STRUCT vcaTs;

			_timeb ts;
			_ftime_s(&ts);

			vcaTs.ulSec = (ULONG)ts.time;
			vcaTs.ulMSec = 0;

			s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETSYNCTIME, (ULONG)&vcaTs);*/

			ULONG MetaFmt;
			if (s_VCAApi->VCA5Control(m_index, VCA5_CMD_GETMETAFMT, (ULONG)&MetaFmt))
			{
					MetaFmt = MetaFmt&(~VCA5_METAFMT_BLOB);
					MetaFmt = MetaFmt&(~VCA5_METAFMT_SMOKEFIRE);

					s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETMETAFMT, MetaFmt | VCA5_METAFMT_TAMPER | VCA5_METAFMT_SCENECHANGE | VCA5_METAFMT_COLORSIG);
			}
		}
		m_startTime = ::GetTickCount();
	}

	if (m_nWidth != pInfo->nWidth||m_nHeight != pInfo->nHeight)
	{		
		m_EngineParam.ulImageSize = CMN5_MAKEIMGSIZE(pInfo->nWidth, pInfo->nHeight);
		s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETENGINEPARAMS, (ULONG)&m_EngineParam);
		m_nWidth = pInfo->nWidth;
		m_nHeight = pInfo->nHeight;
	}

	if (m_pVCARender)
	{
		BITMAPINFOHEADER bm;
		bm.biWidth = pInfo->nWidth;
		bm.biHeight = pInfo->nHeight;
		m_pVCARender->ChangeVCASourceInfo(&bm);
	}
	bool bFirst = false;
	if (m_bSetZoneRuleFlag)
	{
		SetZoneRule();
		m_bSetZoneRuleFlag = false;
		bFirst = true;
	}

	FILETIME timestamp;
	timestamp.dwLowDateTime += 40;
	GetSystemTimeAsFileTime(&timestamp);
	FileTimeToLocalFileTime(&timestamp, &timestamp);

	BYTE	pResult[VCA5_MAX_OUTPUT_BUF_SIZE];
	DWORD	nResult = VCA5_MAX_OUTPUT_BUF_SIZE;
	unsigned __int64 iCurSystemTime = ::GetTickCount();

	if (bFirst)
	{
	/*	static  char *pClear = NULL;
		if (pClear == NULL)
		{
			pClear = new char[2048 * 2048 * 3 / 2];
			memset(pClear, 0, 2048 * 2048 * 3 / 2);
		}
		for (int i = 0; i < 2; i++)
		{
			timestamp.dwLowDateTime += 40;
			if (s_VCAApi->VCA5Process(m_index, (unsigned char*)pClear, (VCA5_TIMESTAMP*)&timestamp, &nResult, pResult))
			{
				m_pVCAMetaDateParser->ParseMetaData(pResult, nResult);
				if (m_pVCARender)
				{
					BITMAPINFOHEADER bm;
					ULONG	ulRotate = 0;
					RECT	rcImageROI = { 0, };
					RECT	rcVCAROI = { 0, };
					m_pVCARender->RenderVCAData((BYTE*)pClear, &bm, ulRotate, m_pVCAMetaDateParser, rcImageROI, rcVCAROI, (VCA5_TIMESTAMP*)&timestamp);
				}
				
				m_pVCAMetaDateParser->ClearCountLineEvents();
				Sleep(10);
			}
			else
			{
				m_pVCAMetaDateParser->ParseMetaData(pResult, nResult);
				BITMAPINFOHEADER bm;
				ULONG	ulRotate = 0;
				RECT	rcImageROI = { 0, };
				RECT	rcVCAROI = { 0, };
				m_pVCARender->RenderVCAData((BYTE*)pClear, &bm, ulRotate, m_pVCAMetaDateParser, rcImageROI, rcVCAROI, (VCA5_TIMESTAMP*)&timestamp);
			}
	}*/
	}
	/*
	static char *pTemp = new char[2048 * 2048 * 3 / 2];
	memcpy(pTemp, pInfo->pBuf + pInfo->nWidth*pInfo->nHeight, pInfo->nWidth*pInfo->nHeight * 1 / 4);
	memmove(pInfo->pBuf + pInfo->nWidth*pInfo->nHeight, pInfo->pBuf + pInfo->nWidth*pInfo->nHeight * 5 / 4, pInfo->nWidth*pInfo->nHeight * 1 / 4);
	memcpy(pInfo->pBuf + pInfo->nWidth*pInfo->nHeight * 5 / 4, pTemp, pInfo->nWidth*pInfo->nHeight * 1 / 4);
*/
	if (s_VCAApi->VCA5Process(m_index, (unsigned char*)pInfo->pBuf, (VCA5_TIMESTAMP*)&timestamp, &nResult, pResult)){
		unsigned __int64 dif = ::GetTickCount() - iCurSystemTime;
		m_VCAtotalTime += dif;
		m_frameCount++;

		unsigned __int64 iCurSystemTime1 = ::GetTickCount();
		m_pVCAMetaDateParser->ParseMetaData(pResult, nResult);//129  640
		/*	if (nResult != 512 || nResult != 256)
			{
			int ulein = 2;
			}
			if (nResult == 640)
			{
			int ulein = 2;
			}*/
		if (m_pVCAMetaDateParser->GetEvents()->ulTotalEvents >0)
		{
	/*	static int i = 0;
		FILE *fd;
		char buf[32];
		sprintf(buf, "./xml/frame%d.xml", i);
		fd = fopen(buf, "wb");
		if (fd)
		{
			fwrite(pResult, 1, nResult, fd);
			fclose(fd);
		}
		i++;*/
	}
		if (m_evenFilterCallback != NULL/*&&m_pVCAMetaDateParser->GetEvents()->ulTotalEvents != 0*/)
		{
			DISPLAYCALLBCK_INFO eventInfo;
			eventInfo.pBuf = pInfo->pBuf;
			eventInfo.nWidth = pInfo->nWidth;
			eventInfo.nHeight = pInfo->nHeight;
			eventInfo.nStamp = pInfo->nStamp;
			eventInfo.nUser = m_eventFilterUser;
			m_evenFilterCallback(&eventInfo);
		}
		if (m_pVCARender)
		{
			BITMAPINFOHEADER bm;
			ULONG	ulRotate = 0;
			RECT	rcImageROI = { 0, };
			RECT	rcVCAROI = { 0, };
			m_pVCARender->RenderVCAData((unsigned char*)pInfo->pBuf, &bm, ulRotate, m_pVCAMetaDateParser, rcImageROI, rcVCAROI, (VCA5_TIMESTAMP*)&timestamp);
			dif = ::GetTickCount() - iCurSystemTime1;
			m_renderTime += dif;
		}
		m_pVCAMetaDateParser->ClearCountLineEvents();
	}
	m_totalTime = ::GetTickCount() - m_startTime;
}

BOOL	 CSimpleVCA::VCAOpen( VCA5_ENGINE_PARAMS* pEngineParam)
{
	if(!s_VCAApi){
		DbgMsg("Open VCA Fail VCA5 instance does not create before\n");
		return FALSE;
	}

	// Close GDI viewer
//	g_GDIViewer.Endup();
	DWORD Width, Height;

	Width	= VCA5_GETIMGWIDTH(pEngineParam->ulImageSize);
	Height	= VCA5_GETIMGHEIGHT(pEngineParam->ulImageSize);

	int Rotate = CGDIViewer::ROTATE_0;
	pEngineParam->ulImageSize = VCA5_SETIMAGEROTATE(pEngineParam->ulImageSize, Rotate);
	/*
	if(CGDIViewer::ROTATE_90 == Rotate || CGDIViewer::ROTATE_270 == Rotate){
		g_iHeight	= Width;
		g_iWidth	= Height;
	}else{
		g_iHeight	= Height;
		g_iWidth	= Width;
	}

	// Re-open GDI viewer
	int gdiColorType = ConvertGDI2VCAColorType(pEngineParam->ulColorFormat);
	g_GDIViewer.Setup( hWnd, Width, Height, gdiColorType, Rotate);*/

	// Re-open the VCA engines
	
	_V( s_VCAApi->VCA5Open(m_index, pEngineParam) );

	return TRUE;
}


void	 CSimpleVCA::VCAClose()
{
/*	if(!s_nVCAEngine){
		return;
	}
	
	for(DWORD i=0;i<s_nVCAEngine;i++) {
		s_VCAApi->VCA5Close(i);
	}
	*/
	if (!m_bOpen)
		return;
	{
	}
	m_bOpen = false;
	s_VCAApi->VCA5Close(0);
/*	if (m_pVCAMetaDateParser)
	{
		delete m_pVCAMetaDateParser;
		m_pVCAMetaDateParser = NULL;
	}
	if (m_pVCARender)
	m_pVCARender->Endup();*/
}



BOOL	CSimpleVCA::SetControl(ULONG ulVCA5Cmd, ULONG ulParam0, ULONG ulParam1, ULONG ulParam2, ULONG ulParam3)
{
	//	CHECK_ENGINE_READY();
	CHECK_FUNC_AND_RETURN_FALSE(s_VCAApi->VCA5Control(m_index, ulVCA5Cmd, ulParam0, ulParam1, ulParam2, ulParam3),
		_T("fail to control object"));
	return TRUE;
}

BOOL	CSimpleVCA::SetConfig(VCA5_APP_ZONES *pZones, VCA5_APP_RULES *pRules, VCA5_APP_COUNTERS *pCounters)
{

	if (pZones) {
		s_VCAApi->VCA5Control(m_index, VCA5_CMD_CLEARZONE, VCA5_ID_ALL);
	}
	if (pRules) {
		s_VCAApi->VCA5Control(m_index, VCA5_CMD_CLEARRULE, VCA5_ID_ALL);
	}
	if (pCounters) {
		//		m_pVCA5API->VCA5Control(m_ulEngineId, VCA5_CMD_SETCOUNTERRES, VCA5_ID_ALL);
		s_VCAApi->VCA5Control(m_index, VCA5_CMD_CLEARCOUNTER, VCA5_ID_ALL);
	}

	bool bColorFilter = false;
	// update
	if (pZones) {
		for (ULONG i = 0; i < pZones->ulTotalZones; ++i)
			CHECK_FUNC_AND_RETURN_FALSE(s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETZONE, (ULONG)(&(pZones->pZones[i]))),
			TEXT("fail to update zones"));
	}
	VCA5_ERROR_CODE_ITEM err;
	if (s_VCAApi->VCA5GetLastErrorCode(&err))
	{
		CString sReason = CString(err.AuxMsg);
	}
	if (pRules) {
		for (ULONG i = 0; i < pRules->ulTotalRules; ++i)
		{
			if (pRules->pRules[i].usRuleType == VCA5_RULE_TYPE_COLSIG)
			{
				bColorFilter = true;
			}
		CHECK_FUNC(s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETRULE, (ULONG)(&(pRules->pRules[i]))),
			TEXT("fail to update rules"));
		}
	}
	if (pCounters) {
		for (ULONG i = 0; i < pCounters->ulTotalCounters; ++i){
			CHECK_FUNC(s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETCOUNTER, (ULONG)(&pCounters->pCounters[i])),
				TEXT("fail to update counters"));

			if (pCounters->pCounters[i].uiStatus&VCA5_APP_COUNTER_STATUS_NOT_USED){
				s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETCOUNTERRES, pCounters->pCounters[i].usCounterId, 0);
			}
		}
	}
	VCA5_APP_ADVANCED_INFO *pInfo = m_pDataMgr->GetAdvancedInfo();

	//s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETSTABENABLE, (ULONG)&(pInfo->bEnableStab));
	//s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETRETRIGTIME, (ULONG)&(pInfo->ulRetrigTime));
	memset(pInfo, 0, sizeof(VCA5_APP_ADVANCED_INFO));
	pInfo->TrackerParams.ulMinObjAreaPix = 10;
	pInfo->TrackerParams.ulSecsToHoldOn = 60;
	pInfo->TrackerParams.bAdvTraEnabled = 1;
	pInfo->TrackerParams.bAbObjEnabled =1;
	pInfo->TrackerParams.bMovObjEnabled = 1;
	pInfo->TrackerParams.bCntLineEnabled =0;
	if (bColorFilter)
	{
		pInfo->TrackerParams.bCntLineEnabled = 0;
	}
	//pInfo->TrackerParams.
	s_VCAApi->VCA5Control(m_index, VCA5_CMD_SETTRACKERPARAMS, (ULONG)&(pInfo->TrackerParams));
	return TRUE;
}




///////

typedef struct{
	VCA5_PACKET_OBJECT	object;
	BOOL		bAlarm;		
}OBJECT_STATUS;


#define			MAX_TARGET_NUM	32
__declspec( align(16) )
DWORD			g_ObjectNum;
OBJECT_STATUS	g_ObjectStatus[MAX_TARGET_NUM];


#ifndef PIXELTOPERCENT
#define PIXELTOPERCENT( normalizeValue, pixelValue, maxValue ) \
	normalizeValue = (unsigned short) ( (pixelValue * NORMALISATOR) / ( maxValue -1) ); \
	PERCENTASSERT( normalizeValue );
#endif

#ifndef PERCENTTOPIXEL
#define PERCENTTOPIXEL( pixelValue, normalizeValue, maxValue ) \
	pixelValue = (( maxValue -1 ) * normalizeValue) / NORMALISATOR;
#endif


#define NORMALISATOR	65535	
#define PERCENTTOPIXEL_W(normalizeValue) \
	(( g_iWidth - 1 ) * normalizeValue) / NORMALISATOR

#define PERCENTTOPIXEL_H(normalizeValue) \
	(( g_iHeight - 1 ) * normalizeValue) / NORMALISATOR


inline	void	ConvertPercent2PixelRect(int x, int y, int w, int usH, RECT& rc)
{
	/*rc.left		= (LONG)(PERCENTTOPIXEL_W((x - w * 0.5)));
	rc.top		= (LONG)(PERCENTTOPIXEL_H((y - usH * 0.5)));
	rc.right	= (LONG)(PERCENTTOPIXEL_W((x + w * 0.5)));
	rc.bottom	= (LONG)(PERCENTTOPIXEL_H((y + usH * 0.5)));
	*/
	rc.left		= max(0, rc.left); 
	rc.top		= max(0, rc.top);
	rc.right	= min(MAX_IMAGE_WIDTH  - 1, rc.right);
	rc.bottom	= min(MAX_IMAGE_HEIGHT - 1, rc.bottom);
}

BOOL	AnalysisResult(CVCAMetaLib*	pVCAMetaDateParser, DWORD *ObjectNum, OBJECT_STATUS *pObjectStatus)
{
	VCA5_PACKET_HEADER *pHeader = pVCAMetaDateParser->GetHeader();
	if(VCA5_PACKET_STATUS_ERROR == pHeader->ulVCAStatus) return FALSE;
	
	VCA5_PACKET_OBJECTS *pObjects = pVCAMetaDateParser->GetObjects();
	ULONG i, j;
	*ObjectNum = pObjects->ulTotalObject;
	for(i = 0; i < pObjects->ulTotalObject ; i++){
		pObjectStatus[i].object = pObjects->Objects[i];
		pObjectStatus[i].bAlarm	= FALSE;
	}

	VCA5_PACKET_EVENTS *pEvents = pVCAMetaDateParser->GetEvents();
	
	for(i = 0; i < pEvents->ulTotalEvents ; i++){
		for(j = 0; j < pObjects->ulTotalObject ; j++){
			if(pEvents->Events[i].ulObjId == pObjects->Objects[j].ulId){
					pObjectStatus[j].bAlarm = TRUE;
					break;
			}
		}
	}
	
	return TRUE;
}

/*
BOOL	 CSimpleVCA::VCASetConf()
{
	ULONG EngId	= 0;
	ULONG	i;
	VCA5_APP_ENGINE_CONFIG *pConfig;

	CVCAConfigure *pCfg = CVCAConfigure::Instance();
	if(pCfg->LoadEngine(0, _T("VCAConf.xml"))){
		return FALSE;
	}

	pConfig	= pCfg->GetEngineConf(0);
	if(!pConfig) return FALSE;

	VCA5_APP_ZONES*		pZones = &(pConfig->Zones);
	VCA5_APP_RULES*		pRules = &(pConfig->Rules);	
	VCA5_APP_COUNTERS*	pCounters = &(pConfig->Counters);

	s_VCAApi->VCA5Control(EngId, VCA5_CMD_CLEARZONE, VCA5_ID_ALL);
	s_VCAApi->VCA5Control(EngId, VCA5_CMD_CLEARRULE, VCA5_ID_ALL);
	s_VCAApi->VCA5Control(EngId, VCA5_CMD_CLEARCOUNTER, VCA5_ID_ALL);


	for(i = 0 ; i < pZones->ulTotalZones ; i++)
		s_VCAApi->VCA5Control(EngId, VCA5_CMD_SETZONE, (ULONG)(&(pZones->pZones[i])));

	for(i = 0 ; i < pRules->ulTotalRules ; i++)
		s_VCAApi->VCA5Control(EngId, VCA5_CMD_SETRULE, (ULONG)(&(pRules->pRules[i])));

	for(i = 0 ; i < pCounters->ulTotalCounters ; i++)
		s_VCAApi->VCA5Control(EngId, VCA5_CMD_SETCOUNTER, (ULONG)(&pCounters->pCounters[i]));

	s_VCAApi->VCA5Control( EngId, VCA5_CMD_SETTRACKERPARAMS, (ULONG) &(pConfig->AdvInfo.TrackerParams));

	return TRUE;
}
*/



BOOL	VCAPlay(HWND hWnd)
{
	//if(!g_pVideoSource){
	//	return FALSE;
	//}

	BYTE *pRawData;
	FILETIME timestamp; 
	DWORD bEOF;

	BYTE	pResult[VCA5_MAX_OUTPUT_BUF_SIZE];
	DWORD	nResult = VCA5_MAX_OUTPUT_BUF_SIZE;
	
	/*
	if(g_pVideoSource->GetRawImage(&pRawData, &timestamp, &bEOF)){

		g_GDIViewer.DrawImage(pRawData);
		DrawZones();

		if(g_VCAApi->VCA5Process(0, pRawData, (VCA5_TIMESTAMP*)&timestamp, &nResult, pResult)){

			g_pVCAMetaDateParser->ParseMetaData(pResult, nResult);
			if (nResult != 256)
			{
				int ulein = 2;
			}

			AnalysisResult(g_pVCAMetaDateParser, &g_ObjectNum, g_ObjectStatus);
			DrawObject(g_ObjectNum, g_ObjectStatus);

			// Uncomment the following to draw the "blobs"
			//DrawBlobs();
		}

		HDC hDC = GetDC(hWnd);
	//	g_GDIViewer.Update(hDC, 0,0);
		ReleaseDC(hWnd, hDC);

		//g_pVideoSource->ReleaseRawImage();
//	}
*/
	return TRUE;
}


