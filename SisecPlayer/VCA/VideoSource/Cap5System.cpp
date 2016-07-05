#include "stdafx.h"
#include <initguid.h>
#include "Cap5System.h"
#include "XMLUtils.h"

static BOOL Uda5CreateInstance(HMODULE hLib,REFIID riid,void ** ppInterface)
{
	if (hLib) {

		BOOL rs;
		IUnknown* pUnknown;
		BOOL (FAR WINAPI*_CreateInstance)(IUnknown ** ppInterface);
		FARPROC test_proc=GetProcAddress(hLib,"Cmn5CreateInstance");
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


static IXMLDOMDocument2Ptr	LoadModelInfo(TCHAR * szFilename)
{
	IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr;
	hr = pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if(S_OK != hr){
		MessageBox(NULL,_T("Can not Create XMLDOMDocument Install MSXML6.0"), _T("Error"), MB_OK);
		return NULL;
	}
	
	pXMLDoc->resolveExternals = VARIANT_TRUE;
	pXMLDoc->async = VARIANT_FALSE;
	pXMLDoc->validateOnParse = VARIANT_TRUE;

	_variant_t varXml(szFilename);
	_variant_t varOut((bool)TRUE);

	varOut = pXMLDoc->load(szFilename);
	if ((bool)varOut == FALSE){
		TCHAR strTemp[100];
		_stprintf_s(strTemp, _countof(strTemp), _T("Can not Read XML File %s\n"), szFilename);
		MessageBox(NULL,strTemp, _T("Error"), MB_OK);
		return NULL;
	}
	
	return pXMLDoc;	
}


IXMLDOMNodePtr FindNamedFirstChild(IXMLDOMNodePtr nodeParent,BSTR findingName)
{
	for(IXMLDOMNodePtr node = nodeParent->firstChild;node;node=node->nextSibling){
		if(_wcsicmp(findingName, node->nodeName)==0){
			return node;
		}
	}
	return NULL;
}


static BOOL	FindModelActivateCode(IXMLDOMDocument2Ptr pXMLDoc, DWORD HWID, BYTE *pActivateCode)
{
	IXMLDOMElementPtr nodeModels, nodeModel;
	nodeModels	= pXMLDoc->documentElement->getElementsByTagName(_T("Models"))->item[0];

	TCHAR buffer[128];
	DWORD ModelID;
	for_ChildNodeEnum(nodeModels,nodeModel){
		ModelID =	GetAttrValueHex(nodeModel,_T("DefModelID"));
		if(ModelID == HWID){
			GetAttrValueString(nodeModel,_T("ActivationCode"),buffer, 128);
			
			UINT a[16];
			BYTE* b=pActivateCode;
			_stscanf_s(buffer,_T("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x"),
				a+0,a+1,a+2,a+3,a+4,a+5,a+6,a+7,a+8,a+9,a+10,a+11,a+12,a+13,a+14,a+15);
			for(int i=0;i<16;i++)b[i]=a[i];
			return TRUE;
		}
	}
	return FALSE;
}

CCap5System::CCap5System(void)
{
	m_Status	= STATUS_FREE;
	m_pCAP5API	= NULL;
	m_nCapBoards= 0;
	m_pCapBoardInfo		= NULL;
	m_pCAP5VideoSource	= NULL;
	
}

CCap5System::~CCap5System(void)
{
	if(STATUS_FREE != m_Status){
		Endup();
	}

	TRACE("CCap5System delete [%d] \n", m_Status);
}

BOOL	CCap5System::Setup(TCHAR *szDllName, TCHAR *szModelXMLName)
{
	if(STATUS_FREE != m_Status){
		TRACE("CCap5System setuped before");
		return TRUE;
	}
		
	HMODULE hLib=NULL;
	TCHAR szErr[256];
	CMN5_SYSTEM_INFO sysInfo;
	IXMLDOMDocument2Ptr pXMLDoc;


	if(_tcscmp(szDllName, _T("OPEN")) == 0){
		TRACE(_T("OPEN Version License \n"));
		return FALSE;
	}

	hLib=LoadLibrary(szDllName);
	if(!hLib){
		//_stprintf_s(szErr, _countof(szErr), _T("Can not load CAP5DLL[%s] "), szDllName);
		//AfxMessageBox(szErr);
		return FALSE;
	}
		
	pXMLDoc = LoadModelInfo(szModelXMLName);
	if(!pXMLDoc){
		_stprintf_s(szErr, _countof(szErr), _T("Can not load ModelXML[%s] "), szModelXMLName);
		MessageBox(NULL,szErr, _T("Error"), MB_OK);
		return FALSE;
	}

	if (!Uda5CreateInstance(hLib,IID_ICap5,(void**)&m_pCAP5API)){
		_stprintf_s(szErr, _countof(szErr), _T("Cap5 board not Installed CAP5DLL[%s] "), szDllName);
		MessageBox(NULL,szErr, _T("Error"), MB_OK);
		return FALSE;
	}

	if(!m_pCAP5API->Cap5GetSystemInfo(&sysInfo)){
		_stprintf_s(szErr, _countof(szErr), _T("Cap5 board not Installed CAP5DLL[%s] "), szDllName);
		MessageBox(NULL,szErr, _T("Error"), MB_OK);
		goto FAILED;
	}

	//Check DLL and SYS version for support VCAEngine
	DWORD bd,ch;
	m_nCapBoards = sysInfo.uNumOfBoard;
	m_pCapBoardInfo=new CAP5_BOARD_INFO[m_nCapBoards];
	
	BOOL bActivate=FALSE;
	BYTE ActivateCode[20];
	for(bd=0;bd<m_nCapBoards;bd++) {
		CMN5_BOARD_INFO_DESC desc={sizeof(CAP5_BOARD_INFO),1,0,};
		m_pCAP5API->Cap5GetBoardInfo(bd,&desc,&m_pCapBoardInfo[bd]);
		if(!FindModelActivateCode(pXMLDoc, m_pCapBoardInfo[bd].uModelID, ActivateCode)){
			_stprintf_s(szErr, _countof(szErr), _T("Can not Found Activation code [0x%X] "), m_pCapBoardInfo[bd].uModelID);
			MessageBox(NULL,szErr, _T("Error"), MB_OK);
			goto FAILED;
		}

		if(!m_pCAP5API->Cap5Activate(bd, ActivateCode)){
			_stprintf_s(szErr, _countof(szErr), _T("Can not Activate Board [0x%X] "), m_pCapBoardInfo[bd].uModelID);
			MessageBox(NULL,szErr, _T("Error"), MB_OK);
			goto FAILED;
		}
	}

	DWORD TotalCh = 0;
	for(bd=0;bd<m_nCapBoards;bd++) TotalCh += m_pCapBoardInfo[bd].uMaxChannel;
	m_pCAP5VideoSource = new CCAP5VideoSource* [TotalCh];
	memset(m_pCAP5VideoSource, 0, sizeof(CCAP5VideoSource*)*TotalCh);
	m_TotalCh = TotalCh;

	if(!m_pCAP5API->Cap5Setup()){
		TRACE("Can not setup CA5\n");
		goto FAILED;
	}

	//SetProperty CAP5
	for(bd=0;bd<m_nCapBoards;bd++) {
		m_pCAP5API->Cap5SetColorFormat(bd,CAP5_COLOR_FORMAT_YUY2);
		for(ch=0;ch<m_pCapBoardInfo[bd].uMaxChannel;ch++) {
			m_pCAP5API->Cap5VideoEnable(bd,ch,FALSE);
		}
		
		memset(m_USN[bd], 0, 9);
		DWORD cmd	= CAP5_QR_GET_USN|(bd<<16);
		m_pCAP5API->Cap5QueryInfo(cmd, 16 ,m_USN[bd]);
	}

	memset(m_FrameRate, 0, sizeof(m_FrameRate));

	m_Status = STATUS_SETUP;
	return TRUE;

FAILED:
	Endup();
	return FALSE;
}

void	CCap5System::Endup()
{
	Stop();
	if(m_pCAP5API){
		m_pCAP5API->Cap5Endup();
		m_pCAP5API->Release();
		m_pCAP5API = NULL;
	}

	SAFE_DELETE_ARRAY(m_pCapBoardInfo);
	SAFE_DELETE_ARRAY(m_pCAP5VideoSource);

	m_Status = STATUS_FREE;
}


BOOL	CCap5System::Run()
{
	if (STATUS_SETUP != m_Status) {
		TRACE("Can not CCap5System::Run Current State [%d] \n", m_Status);
		return FALSE;
	}

	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hKillEvent) {
		return FALSE;
	}
	DWORD threadID;
	m_hThread=CreateThread(NULL,0,CAP5ProcessThreadWrapper,this,NULL,&threadID);
	if (NULL == m_hThread) {
		CloseHandle(m_hKillEvent);
		return FALSE;
	}

	m_pCAP5API->Cap5Run();
	
	for(DWORD bd=0;bd<m_nCapBoards;bd++) {
		m_pCAP5API->Cap5SetFrameRate(bd, CAP5_FRC_USER_FIXED, m_FrameRate[bd], m_pCapBoardInfo[bd].uMaxChannel);
	}

	m_Status = STATUS_RUN;	
	return TRUE;
}

void	CCap5System::Stop()
{
	if (STATUS_RUN != m_Status) {
		TRACE("Can not CCap5System::Stop Current State [%d] \n", m_Status);
		return;
	}

	m_pCAP5API->Cap5Stop();

	SetEvent(m_hKillEvent);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	CloseHandle(m_hKillEvent);
	
	m_Status = STATUS_SETUP;
}

BOOL	CCap5System::RegisterVideoSource(DWORD bd, DWORD ch, CCAP5VideoSource *pVideoSource)
{
	if((bd<m_nCapBoards) && (ch < m_pCapBoardInfo[bd].uMaxChannel)){
		DWORD LinearCh = GetLinearCh(bd, ch);
		m_pCAP5API->Cap5VideoEnable(bd, ch, TRUE);
		if(m_pCAP5VideoSource[LinearCh]){
			TRACE("Bd[%d] Ch[%d] already registered \n", bd, ch);
			return FALSE;
		}
		m_pCAP5VideoSource[LinearCh] = 	pVideoSource;
		return TRUE;
	}
	return FALSE;
}


BOOL	CCap5System::SetFramerate(DWORD bd, DWORD ch, DWORD FPS)
{
	if((bd>=m_nCapBoards) || (ch >= m_pCapBoardInfo[bd].uMaxChannel )){
		TRACE("Invailed Parameter BD[%d] CH[%d] \n", bd, ch);
		return FALSE;
	}

	/* Check FPS
	DWORD i, TotalFPS = 0;
	for(i = 0 ; i < m_pCapBoardInfo[bd].uMaxChannel ; i++){
		TotalFPS += m_FrameRate[bd][i];
	}
	
	if(TotalFPS+FPS > m_pCapBoardInfo[bd].uMaxChannel * ){
	}
	*/


	DWORD OldFPS = m_FrameRate[bd][ch];
	m_FrameRate[bd][ch] = (BYTE)FPS;
	if(!m_pCAP5API->Cap5SetFrameRate(bd, CAP5_FRC_USER_FIXED, m_FrameRate[bd], m_pCapBoardInfo[bd].uMaxChannel)){
		m_FrameRate[bd][ch] = (BYTE)OldFPS;
		return FALSE;
	}
	return TRUE;
}

BYTE CCap5System::GetFramerate(DWORD bd, DWORD ch)
{
	if( ((bd>=m_nCapBoards) || (ch >= m_pCapBoardInfo[bd].uMaxChannel )) )
	{
		return 0;
	}
	else
	{
		return m_FrameRate[bd][ch];
	}
}

void	CCap5System::UnRegisterVideoSource(DWORD bd, DWORD ch)
{
	if(m_Status == STATUS_FREE) return;

	if((bd<m_nCapBoards) && ( ch < m_pCapBoardInfo[bd].uMaxChannel))
	{
		DWORD LinearCh = GetLinearCh(bd, ch);
		m_pCAP5API->Cap5VideoEnable(bd, ch, FALSE);
		m_pCAP5VideoSource[LinearCh] = NULL;
		m_FrameRate[bd][ch] = 0;
	}
}

DWORD	CCap5System::CAP5ProcessThreadWrapper(LPVOID pParam)
{
	return ((CCap5System *) pParam)->CAP5ProcessThread();
}


UINT	CCap5System::CAP5ProcessThread()
{
	HANDLE pEvents[2];

	int idxVid, idx=0;
	pEvents[idx++]=m_hKillEvent;
	HANDLE hEvent;
	
	// CapApi
	if (m_pCAP5API->Cap5GetEventHandle(CMN5_DT_VIDEO,&hEvent)) {
		idxVid=idx;
		pEvents[idx++]=hEvent;
	}
	
	DWORD ImageLen;
	BOOL rs;
	FILETIME Timestamp;
	while (TRUE) {
		DWORD waitObj=WaitForMultipleObjects(idx,pEvents,FALSE,INFINITE);
		if (waitObj==WAIT_OBJECT_0+0) {
			break;
		}else if ((waitObj==WAIT_OBJECT_0+idxVid) || (waitObj==WAIT_TIMEOUT)) {
			CAP5_DATA_INFO capData;
			do { 
				ZeroMemory(&capData, sizeof(capData));
				rs = m_pCAP5API->Cap5GetEventData(CMN5_DT_VIDEO,&capData);
				if (rs) {
					DWORD linearCh = GetLinearCh(capData.uBoardNum, capData.uChannelNum);
					if(m_TotalCh > linearCh && m_pCAP5VideoSource[linearCh]){
						ImageLen = CMN5_GETIMGWIDTH(capData.uImageSize)*CMN5_GETIMGHEIGHT(capData.uImageSize)*2;
						
						FileTimeToLocalFileTime((FILETIME *)&(capData.TimeTag.QuadPart), (FILETIME *)&Timestamp);
						m_pCAP5VideoSource[linearCh]->InsertImage(capData.pDataBuffer, Timestamp, ImageLen);
					}
					m_pCAP5API->Cap5ReleaseData(&capData);
				}
			} while (rs && capData.uHasNextData);
		}	else if (waitObj==WAIT_FAILED) {
			TRACE("WaitFailed!!\n");
		} 
	}

	TRACE("End of Cap5Thread \n");
	return 0;
}


