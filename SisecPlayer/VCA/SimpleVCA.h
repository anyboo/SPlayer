#ifndef __SIMPLE_VCA_H__
#define __SIMPLE_VCA_H__
#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif
#include "VCA5CoreLib.h"
#include "vca/vcaMetalib.h"
#include "./IVCAObject.h"
#include "Render\VCARender.h"
#include "VCA\Render\VCADataMgr.h"


class CSimpleVCA :public IVCAObject
{
public:
	CSimpleVCA() :m_index(0), m_bSetZoneRuleFlag(false), m_pDataMgr(NULL), m_pVCARender(NULL), m_bOpen(false), 
		m_evenFilterCallback(NULL), m_eventFilterUser(NULL),
		m_pVCAMetaDateParser(NULL), m_VCAtotalTime(0), m_totalTime(0), m_frameCount(0), m_renderTime(0)
	{
		m_pVCAMetaDateParser = CreateVCAMetaLib(CVCAMetaLib::MSXML_PARSER);
		m_nWidth = 0;
		m_nHeight = 0;
		m_bColorFilter = false;
	}
	virtual ~CSimpleVCA(){}
public:
	virtual void ProcessVCA(DISPLAYCALLBCK_INFO *pInfo);
public:
	static BOOL	IsSetup(){ return s_bSetup; }
	static BOOL	VCASetup();
	static void	VCAEndup();
	static BOOL  Uda5CreateInstance(HMODULE hLib, REFIID riid, void ** ppInterface);
	static BOOL InitVCAApi(TCHAR* szDllName, char* szLicense, char* szUSN);
	static	BOOL ReadLicense(LPCTSTR szFIleName, LPSTR szLincens, DWORD nBufferLen, PDWORD pdwBytesRead);
	static void	GetLastErrorStr(char *pErrorBuf);
public:
	
	void Reset()
	{
		m_startTime = ::GetTickCount();
		m_totalTime = 0;
		VCAClose();
	}
	void GetVCATime(unsigned long long &FrameCount, unsigned long long &VCATotalTime)
	{
		FrameCount = m_frameCount;
		VCATotalTime = m_VCAtotalTime;
	}
	void GetTotalTime(unsigned long long &TotalTime)//解码+VCA+显示
	{
		TotalTime = m_totalTime;
	}
	void GetTotalTime2(unsigned long long &TotalTime)//解码+VCA
	{
		TotalTime = m_totalTime - m_renderTime;
	}
	void SetRender(IVCARender *pVCARender){
		m_pVCARender = pVCARender;
	}
	void SetDataMgr(CVCADataMgr *pDataMgr){
		m_pDataMgr = pDataMgr;
	}
	void SetEventFilterCallbck(DisplayCallback callback, long lUser)
	{
		m_evenFilterCallback = callback;
		m_eventFilterUser = lUser;
	}
	CVCAMetaLib* GetMetaDataParser(){
		return m_pVCAMetaDateParser;
	}
	BOOL	VCAOpen(VCA5_ENGINE_PARAMS* pEngineParam);
	void	VCAClose();

	void SetZoneRule();
	void SetZoneRuleFlag();

	BOOL	SetControl(ULONG ulVCA5Cmd, ULONG ulParam0=0, ULONG ulParam1=0, ULONG ulParam2=0, ULONG ulParam3=0);
	BOOL	SetConfig(VCA5_APP_ZONES *pZones, VCA5_APP_RULES *pRules, VCA5_APP_COUNTERS *pCounters);

//	BOOL	VCASetConf();
//	BOOL	VCASetConfFile();
	
private:
	static	IVCA5*				s_VCAApi;
	static	VCA5_SYSTEM_INFO	s_VCA5SysInfo;
	static VCA5_LICENSE_INFO	s_LicenseInfo;
	static bool s_bSetup;
//	static DWORD				s_nVCAEngine;

	CVCAMetaLib*		m_pVCAMetaDateParser;
	VCA5_ENGINE_PARAMS	m_EngineParam;
	IVCARender *m_pVCARender;
	CVCADataMgr *m_pDataMgr;

	int m_nWidth;
	int m_nHeight;

	DisplayCallback m_evenFilterCallback;//事件回调与YUV回调一样，只是lUsr不同
	long m_eventFilterUser;
	
	bool m_bOpen;
	bool m_bColorFilter;
	int m_index;
	bool m_bSetZoneRuleFlag;

	unsigned __int64 m_frameCount;
	unsigned long long m_VCAtotalTime;

	unsigned __int64 m_totalTime;
	unsigned __int64 m_startTime;

	unsigned __int64 m_renderTime;

};


#endif