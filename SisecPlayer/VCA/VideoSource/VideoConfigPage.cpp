// EngineSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "VideoConfigPage.h"
#include "VCAVideoSource.h"
#include "DShowCap.h"
#include "../VCADialog/VCADialog.h"
#include "../VCAEngine/VCAEngine.h"


// CVideoConfigPage dialog

IMPLEMENT_DYNAMIC(CVideoConfigPage, CConfigPage)

CVideoConfigPage::CVideoConfigPage(CWnd* pParent /*=NULL*/)
	: CConfigPage(CVideoConfigPage::IDD, pParent)
	, m_pEngineInfo(NULL)
{
	m_bPopulating = FALSE;
	m_pOldParent = NULL;
}

CVideoConfigPage::~CVideoConfigPage()
{
}

void CVideoConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CConfigPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoConfigPage, CConfigPage)
	ON_BN_CLICKED(IDC_BTN_FILEOPEN, &CVideoConfigPage::OnBnClickedBtnFileopen)
	ON_BN_CLICKED(IDC_RADIO_RAWFILE, &CVideoConfigPage::OnBnClickedRadioRawfile)
	ON_BN_CLICKED(IDC_RADIO_CAP5, &CVideoConfigPage::OnBnClickedRadioCap5)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEOFORMAT, &CVideoConfigPage::OnCbnSelchangeComboVideoformat)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL, &CVideoConfigPage::OnCbnSelchangeComboChannel)
	ON_BN_CLICKED(IDC_BTN_VLCPATHOPEN, &CVideoConfigPage::OnBnClickedBtnVlcpathopen)
	ON_BN_CLICKED(IDC_RADIO_VLC, &CVideoConfigPage::OnBnClickedRadioVlc)
	ON_BN_CLICKED(IDC_RADIO_DSHOW, &CVideoConfigPage::OnBnClickedRadioDshow)
	ON_BN_CLICKED(IDC_RADIO_NOT_SOURCE, &CVideoConfigPage::OnBnClickedRadioNotSource)
	ON_EN_CHANGE(IDC_ED_RAWFILEPATH, &CVideoConfigPage::OnEnChangeEdRawfilepath)
	ON_CBN_SELCHANGE(IDC_COMBO_BOARDNO, &CVideoConfigPage::OnCbnSelchangeComboBoardno)
	ON_CBN_SELCHANGE(IDC_COMBO_COLORFORMAT, &CVideoConfigPage::OnCbnSelchangeComboColorformat)
	ON_CBN_SELCHANGE(IDC_COMBO_RESOLUTION, &CVideoConfigPage::OnCbnSelchangeComboResolution)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAMERATE_CAP5, &CVideoConfigPage::OnCbnSelchangeComboFramerateCap5)
	ON_EN_CHANGE(IDC_ED_VLCEXEFILEPATH, &CVideoConfigPage::OnEnChangeEdVlcexefilepath)
	ON_EN_CHANGE(IDC_ED_STREAMPATH, &CVideoConfigPage::OnEnChangeEdStreampath)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICENUM_DSHOW, &CVideoConfigPage::OnCbnSelchangeComboDevicenumDshow)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_ROTATE, &CVideoConfigPage::OnCbnSelchangeComboVideoRotate)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAME_TYPE, &CVideoConfigPage::OnCbnSelchangeComboFrameType)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_ROI_X,IDC_EDIT_ROI_H, &CVideoConfigPage::OnEnChangeEditROI)
END_MESSAGE_MAP()


BOOL CVideoConfigPage::OnInitDialog()
{
	m_bPopulating = TRUE;

	CConfigPage::OnInitDialog();
	m_engId = m_pVCADialog->GetEngId();


	// Show VCA Dialog
	m_pOldParent = m_pVCADialog->SetParent( this );
	m_pVCADialog->ChangeViewMode( CVCADialog::VIEW_MODE_SOURCE_SELECT );


	CRect rc;
	GetDlgItem( IDC_VIDEO_PREVIEW )->GetWindowRect( &rc );
	ScreenToClient( &rc );

	rc.DeflateRect( 10, 13 );
	rc.OffsetRect( 0, 3 );
	m_pVCADialog->MoveWindow( rc );

	m_pVCADialog->ShowWindow( SW_SHOW );

	m_pEngineInfo = m_pVCADialog->GetVCADataMgr()->GetEngineInfo();//CAPPConfigure::Instance()->GetAPPEngineInfo(m_engId);

	InitCap5DlgCtrls();
	InitDShowDlgCtrls();

	VCA5_APP_VIDEOSRC_INFO *pInfo = &(m_pEngineInfo->tSourceData);

	// Initial setting for Source Type (Radio button setting)
	if( (((IVCAVideoSource::eSOURCETYPE)pInfo->SourceType) == IVCAVideoSource::RAWFILESOURCE) || 
		(((IVCAVideoSource::eSOURCETYPE)pInfo->SourceType) == IVCAVideoSource::COMPRESSEDFILESOURCE) ){
		CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_RAWFILE);
		btn->SetCheck(TRUE);
		LoadRawFileSetting();
	}else if( ((IVCAVideoSource::eSOURCETYPE)pInfo->SourceType) == IVCAVideoSource::CAP5SOURCE){
		CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_CAP5);
		btn->SetCheck(TRUE);
		LoadCap5Settings();
	}else if( ((IVCAVideoSource::eSOURCETYPE)pInfo->SourceType) == IVCAVideoSource::STREAMSOURCE){
		CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_VLC);
		btn->SetCheck(TRUE);
		LoadStreamSourceSetting();
	}else if( ((IVCAVideoSource::eSOURCETYPE)pInfo->SourceType) == IVCAVideoSource::DSHOWSOURCE){
		CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_DSHOW);
		btn->SetCheck(TRUE);
		LoadDShowSetting();
	}else{
		CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_NOT_SOURCE);
		btn->SetCheck(TRUE);
	}

	UpdateUsedChannelMask();

	EnableDlgControls();


	InitComboBox();

	DWORD Rotate	= pInfo->ulRotate;
	DWORD FrameTye	= pInfo->ulFrameType;
	
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_ROTATE);
	pComboBox->SetCurSel(Rotate);
	pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_FRAME_TYPE);
	pComboBox->SetCurSel(FrameTye);

	SetDlgItemInt(IDC_EDIT_ROI_X, pInfo->rcROI.x);
	SetDlgItemInt(IDC_EDIT_ROI_Y, pInfo->rcROI.y );
	SetDlgItemInt(IDC_EDIT_ROI_W, pInfo->rcROI.w );
	SetDlgItemInt(IDC_EDIT_ROI_H, pInfo->rcROI.h );
	
	m_bPopulating = FALSE;

	return TRUE;  
}


void CVideoConfigPage::InitComboBox()
{
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_ROTATE);
	pComboBox->InsertString(0, _T("Rotate 0"));
	pComboBox->InsertString(1, _T("Rotate 90"));
	pComboBox->InsertString(2,_T("Rotate 180"));
	pComboBox->InsertString(3,_T("Rotate 270"));

	pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_FRAME_TYPE);
	pComboBox->InsertString(0, _T("Auto"));
	pComboBox->InsertString(1, _T("Interleave"));
	pComboBox->InsertString(2, _T("Progress"));

}


void CVideoConfigPage::UpdateUsedChannelMask()
{
	CAPPConfigure *pAppCfg = CAPPConfigure::Instance();
	memset(&m_dwUsedChannelMask[0], 0, sizeof(m_dwUsedChannelMask));
	for (DWORD i = 0; i < pAppCfg->GetEngineCnt(); ++i) {
		VCA5_APP_ENGINE_INFO *pEngine = pAppCfg->GetAPPEngineInfo(i);
		if (pEngine->tSourceData.SourceType == IVCAVideoSource::CAP5SOURCE) {
			m_dwUsedChannelMask[pEngine->tSourceData.Bd] |= (1 << pEngine->tSourceData.Ch);
		}
	}
}

void CVideoConfigPage::OnDestroy()
{
	m_pVCADialog->ChangeViewMode( CVCADialog::VIEW_MODE_PREVIEW, TRUE );
	m_pVCADialog->SetParent( m_pOldParent );
	m_pVCADialog->ShowWindow( SW_HIDE );
}

void CVideoConfigPage::OnSize( UINT nType, int cx, int cy )
{

}

void CVideoConfigPage::EnableDlgControls()
{
	CButton* btn1 = (CButton*)GetDlgItem(IDC_RADIO_RAWFILE);
	CButton* btn2 = (CButton*)GetDlgItem(IDC_RADIO_CAP5);
	CButton* btn3 = (CButton*)GetDlgItem(IDC_RADIO_VLC);
	CButton* btn4 = (CButton*)GetDlgItem(IDC_RADIO_DSHOW);
	
	GetDlgItem(IDC_ED_RAWFILEPATH)->EnableWindow( (btn1->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_BTN_FILEOPEN)->EnableWindow( (btn1->GetCheck() ? TRUE : FALSE) );

	GetDlgItem(IDC_COMBO_BOARDNO)->EnableWindow( (btn2->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_COMBO_CHANNEL)->EnableWindow( (btn2->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_COMBO_RESOLUTION)->EnableWindow( (btn2->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_COMBO_FRAMERATE_CAP5)->EnableWindow( (btn2->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_COMBO_VIDEOFORMAT)->EnableWindow( (btn2->GetCheck() ? TRUE : FALSE) );

	GetDlgItem(IDC_ED_VLCEXEFILEPATH)->EnableWindow( (btn3->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_BTN_VLCPATHOPEN)->EnableWindow( (btn3->GetCheck() ? TRUE : FALSE) );
	GetDlgItem(IDC_ED_STREAMPATH)->EnableWindow( (btn3->GetCheck() ? TRUE : FALSE) );

	GetDlgItem(IDC_COMBO_DEVICENUM_DSHOW)->EnableWindow( ((btn4->GetCheck() && btn4->IsWindowEnabled())? TRUE : FALSE) );
}

void CVideoConfigPage::SyncToDataMgr()
{
	if( m_bPopulating )
	{
		return;
	}

	CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_RAWFILE);
	BOOL ret = FALSE;
	BOOL bFind = FALSE;
	if(btn->GetCheck() && btn->IsWindowEnabled()) {
		ret = SaveRawFileSetting();
		bFind = TRUE;
	}

	btn = (CButton*)GetDlgItem(IDC_RADIO_CAP5);
	if(!bFind && btn->GetCheck() && btn->IsWindowEnabled()) {
		ret = SaveCap5Settings();
		bFind = TRUE;
	}

	btn = (CButton*)GetDlgItem(IDC_RADIO_VLC);
	if(!bFind && btn->GetCheck() && btn->IsWindowEnabled()) {
		ret = SaveStreamSourceSetting();
		bFind = TRUE;
	}

	btn = (CButton*)GetDlgItem(IDC_RADIO_DSHOW);
	if(!bFind && btn->GetCheck() && btn->IsWindowEnabled()) {
		ret = SaveDShowSetting();
		bFind = TRUE;
	}

	btn = (CButton*)GetDlgItem(IDC_RADIO_NOT_SOURCE);
	if(bFind == FALSE && btn->GetCheck() && btn->IsWindowEnabled() ){	
		m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::NOTSETSOURCE;
		ret = TRUE;
	}

	CVCADataMgr *pDataMgr = m_pVCADialog->GetVCADataMgr();
	pDataMgr->FireEvent( IVCAConfigureObserver::VCA_ENGINE_UPDATE, NULL );
}

void CVideoConfigPage::OnBnClickedRadioRawfile()
{
	m_bPopulating = TRUE;
	ResetCap5Settings();
	LoadRawFileSetting();
	EnableDlgControls();
	m_bPopulating = FALSE;

	SyncToDataMgr();
}

void CVideoConfigPage::OnBnClickedRadioCap5()
{
	m_bPopulating = TRUE;
	LoadCap5Settings();
	EnableDlgControls();
	m_bPopulating = FALSE;

	SyncToDataMgr();
}

void CVideoConfigPage::OnBnClickedRadioVlc()
{
	m_bPopulating = TRUE;
	LoadStreamSourceSetting();
	EnableDlgControls();
	m_bPopulating = FALSE;

	SyncToDataMgr();
}

void CVideoConfigPage::OnBnClickedRadioDshow()
{
	m_bPopulating = TRUE;
	EnableDlgControls();
	m_bPopulating = FALSE;

	SyncToDataMgr();
}

void CVideoConfigPage::OnBnClickedRadioNotSource()
{
	m_bPopulating = TRUE;
	EnableDlgControls();
	m_bPopulating = FALSE;

	SyncToDataMgr();
}


void CVideoConfigPage::OnBnClickedBtnFileopen()
{
	TCHAR szFileter[] = _T("Media File | *.raw;*.avi;*.vob;*.mod;*.mov;*.wmv;*.asf;*.mpg;*.mp4;||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR, szFileter);

    if (IDOK == dlg.DoModal())
    {
		CString strFileName = dlg.GetPathName();
		((CEdit*)GetDlgItem(IDC_ED_RAWFILEPATH))->SetWindowText(strFileName);
    }    
}

void CVideoConfigPage::OnBnClickedBtnVlcpathopen()
{
	TCHAR szFileter[] = _T("VLC Exe File (*.exe) | *.exe;||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR, szFileter);

	if (IDOK == dlg.DoModal())
	{
		CString strFileName = dlg.GetPathName();
		((CEdit*)GetDlgItem(IDC_ED_VLCEXEFILEPATH))->SetWindowText(strFileName);
	}   
}



void CVideoConfigPage::OnCbnSelchangeComboVideoformat()
{
	// TODO: Add your control notification handler code here
	CComboBox* comboVideoformat = (CComboBox*)GetDlgItem(IDC_COMBO_VIDEOFORMAT);
	if(comboVideoformat->GetCurSel() == 0)
		m_ulCurVideoFormat = CMN5_VIDEO_FORMAT_NTSC_M;	// CMN5_VIDEO_FORMAT_NTSC_M = 1
	else if(comboVideoformat->GetCurSel() == 1)
		m_ulCurVideoFormat = CMN5_VIDEO_FORMAT_PAL_B;		// CMN5_VIDEO_FORMAT_PAL_B = 2

	InsertImgResolution();
	InsertFPSCAP5();

	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboChannel()
{
	int newBd = ((CComboBox *) GetDlgItem(IDC_COMBO_BOARDNO))->GetCurSel();
	int newCh = ((CComboBox *) GetDlgItem(IDC_COMBO_CHANNEL))->GetCurSel(); 

	if ((m_dwUsedChannelMask[newBd] & (1 << newCh)) != 0 && newCh != m_pEngineInfo->tSourceData.Ch) {
		AfxMessageBox(_T("Error, the channel is already assigned"));
	} else {
		m_dwUsedChannelMask[newBd] &= ~(1 << m_pEngineInfo->tSourceData.Ch);
		m_dwUsedChannelMask[newBd] |= (1 << newCh);
		m_pEngineInfo->tSourceData.Ch = newCh;
	}

	UpdateUsedChannelMask();

	SyncToDataMgr();
}


void CVideoConfigPage::InitCap5DlgCtrls()
{
	CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_CAP5);
	if( CAPPConfigure::Instance()->GetBoardCnt() == 0 ){
		btn->EnableWindow(FALSE);
		return;
	}

	int curBoardNum;
	if(IVCAVideoSource::IsFileSource(m_pEngineInfo->tSourceData.SourceType))
		curBoardNum = 0;
	else
		curBoardNum = m_pEngineInfo->tSourceData.Bd;

	// CAP5_VideoFormat
	CComboBox* comboVideoformat = (CComboBox*)GetDlgItem(IDC_COMBO_VIDEOFORMAT);
	comboVideoformat->AddString(_T("NTSC"));	// NTSC => idx(0)
	comboVideoformat->AddString(_T("PAL"));		// NTSC => idx(1)
	
	// CAP5_ColorFormat
	CComboBox* comboColorformat = (CComboBox*)GetDlgItem(IDC_COMBO_COLORFORMAT);
	comboColorformat->AddString(_T("YUY2"));	// temporarily asigned
	
	// CAP5_Board NO.
	CComboBox* comboBoardNo = (CComboBox*)GetDlgItem(IDC_COMBO_BOARDNO);
	
	int nMaxBoardNums = CAPPConfigure::Instance()->GetBoardCnt();
	for(int i=0; i<nMaxBoardNums; i++) {
		CString bdNum; bdNum.Format(_T("%d"),i);
		comboBoardNo->AddString(bdNum);	// temporarily asigned
	}

	// Cap5_Channel
	CComboBox* comboChNo = (CComboBox*)GetDlgItem(IDC_COMBO_CHANNEL);
	int maxCh = CAPPConfigure::Instance()->GetMaxChannel(curBoardNum);
	for(int i=0; i<maxCh; i++) {
		CString chNum; chNum.Format(_T("%d"),i);
		comboChNo->AddString(chNum);	// temporarily asigned
	}

	m_ulCurVideoFormat = m_pEngineInfo->tSourceData.ulVideoFormat;
}

void CVideoConfigPage::InitDShowDlgCtrls()
{
	DShowCap* pDshow = new DShowCap();

	CAPDEVICE devices[ MAX_DEVICES ];
	int iNumDevices = 0;

	if(!SUCCEEDED( pDshow->GetCapDevices( devices, iNumDevices )))
	{
		TRACE("!! DShowVideoSource : GetCapDevices Failed !! \n");
	}
	
	CButton* btn = (CButton*)GetDlgItem(IDC_RADIO_DSHOW);
	CComboBox* comboDeviceNum = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICENUM_DSHOW);


	if( iNumDevices )
	{
		for(int i=0; i<iNumDevices; i++){
			CString strDeviceInfo;
			strDeviceInfo.Format(_T("%d (%s)"), i, devices[i].tszName);
			int idx = comboDeviceNum->AddString(strDeviceInfo);
			comboDeviceNum->SetItemData( idx, i );
		}

		comboDeviceNum->SetCurSel(0);
	}
	else
	{
		btn->EnableWindow( FALSE );
		comboDeviceNum->EnableWindow( FALSE );
	}

	if(pDshow) delete pDshow;
}


BOOL CVideoConfigPage::SaveRawFileSetting()
{
	// RAWFILE_Path
	CString strRawFilePath;
	CEdit* ed = (CEdit*)GetDlgItem(IDC_ED_RAWFILEPATH);
	ed->GetWindowText(strRawFilePath);
	if(strRawFilePath.IsEmpty()) {
		AfxMessageBox(_T("Raw File Path is empty!"));
		ed->GetFocus();
		return FALSE;
	}

	strRawFilePath.MakeLower();
	if (strRawFilePath.Find(_T(".raw")) > 0) {
		m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::RAWFILESOURCE;
	} else {
		m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::COMPRESSEDFILESOURCE;
	}

	_sntprintf_s(m_pEngineInfo->tSourceData.szRawPath, _countof(m_pEngineInfo->tSourceData.szRawPath),
		_TRUNCATE, _T("%s"), (LPCTSTR) strRawFilePath);

	return TRUE;

}

BOOL CVideoConfigPage::SaveCap5Settings()
{
	m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::CAP5SOURCE;

	CComboBox* comboBoardNo = (CComboBox*)GetDlgItem(IDC_COMBO_BOARDNO);
	CComboBox* comboChNo = (CComboBox*)GetDlgItem(IDC_COMBO_CHANNEL);

	m_pEngineInfo->tSourceData.Bd = comboBoardNo->GetCurSel();
	m_pEngineInfo->tSourceData.Ch = comboChNo->GetCurSel();
	
	// CAP5_VideoFormat
	CComboBox* comboVideoformat = (CComboBox*)GetDlgItem(IDC_COMBO_VIDEOFORMAT);
	if(comboVideoformat->GetCurSel() == 0)
		m_pEngineInfo->tSourceData.ulVideoFormat = CMN5_VIDEO_FORMAT_NTSC_M;	// CMN5_VIDEO_FORMAT_NTSC_M = 1
	else if(comboVideoformat->GetCurSel() == 1)
		m_pEngineInfo->tSourceData.ulVideoFormat = CMN5_VIDEO_FORMAT_PAL_B;		// CMN5_VIDEO_FORMAT_PAL_B = 2

	// CAP5_ColorFormat
	CComboBox* comboColorformat = (CComboBox*)GetDlgItem(IDC_COMBO_COLORFORMAT);
//	if(comboColorformat->GetCurSel() == 0)		// default color format = YUY2
	m_pEngineInfo->tSourceData.ulColorFormat = VCA5_COLOR_FORMAT_YUY2;

	// Cap5_Framerate
	CComboBox* comboFramerate = (CComboBox*)GetDlgItem(IDC_COMBO_FRAMERATE_CAP5);

	// Cap5_ImageResolution
	CComboBox* comboImgResolution = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
	int Width, Height;
	CString curResolution;
	if(comboImgResolution->GetCount() != NULL) {
		comboImgResolution->GetLBText(comboImgResolution->GetCurSel(), curResolution);
		if(curResolution.IsEmpty()) {
			AfxMessageBox(_T("Cap5 ImageResolution is empty!"));
			comboImgResolution->SetFocus();
			return FALSE;
		}
		int token = curResolution.Find(_T('x'));
		Width = _ttoi(curResolution.Mid(0, token));
		Height = _ttoi(curResolution.Mid(token+1, curResolution.GetLength()-token));
		m_pEngineInfo->tSourceData.ulImageSize = CMN5_MAKEIMGSIZE(Width, Height);
	}

	TCHAR strFramerate[16];
	comboFramerate->GetLBText(comboFramerate->GetCurSel(),strFramerate);
	m_pEngineInfo->tSourceData.ulFrameRate = _ttoi(strFramerate);
	m_dwUsedChannelMask[m_pEngineInfo->tSourceData.Bd] |= (1 << m_pEngineInfo->tSourceData.Ch);
	return TRUE;
}

BOOL CVideoConfigPage::SaveStreamSourceSetting()
{
	// Vlc Exe Path
	CString strVlcExePath;
	CEdit* ed = (CEdit*)GetDlgItem(IDC_ED_VLCEXEFILEPATH);
	ed->GetWindowText(strVlcExePath);
	if(strVlcExePath.IsEmpty()) {
		AfxMessageBox(_T("Vlc Path is empty!"));
		ed->GetFocus();
		return FALSE;
	}

	// Vlc Stream Source Path
	CString strStreamPath;
	ed = (CEdit*)GetDlgItem(IDC_ED_STREAMPATH);
	ed->GetWindowText(strStreamPath);
	if(strStreamPath.IsEmpty()) {
		AfxMessageBox(_T("Vlc Stream Path is empty!"));
		ed->GetFocus();
		return FALSE;
	}
	
//	strStreamPath.Replace(_T("yuy"), _T("YUY"));

	m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::STREAMSOURCE;
	TCHAR* pszSrcPath = m_pEngineInfo->tSourceData.szVlcPath;
	size_t cntSrcPath = _countof(m_pEngineInfo->tSourceData.szVlcPath);
	_stprintf_s(pszSrcPath, cntSrcPath, _T("%s %s"), strVlcExePath, strStreamPath);

	return TRUE;
}

BOOL CVideoConfigPage::SaveDShowSetting()
{
	// CAP5_ColorFormat
	CComboBox* comboDeviceNo = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICENUM_DSHOW);
	if(comboDeviceNo->GetCount() == 0){
		comboDeviceNo->GetFocus();
		return FALSE;
	}

	m_pEngineInfo->tSourceData.SourceType = IVCAVideoSource::DSHOWSOURCE;
	m_pEngineInfo->tSourceData.ulDShowDeviceId = comboDeviceNo->GetItemData( comboDeviceNo->GetCurSel() );

/*	CString strComboString, strCurDeviceNo;
	comboDeviceNo->GetLBText(comboDeviceNo->GetCurSel(), strComboString);

	// GetDeviceNum
	int pos = strComboString.Find(_T(" ("));
	if( 0 == pos ) {
		AfxMessageBox(_T("Can't make Device Number"));
		return FALSE;
	}
	strCurDeviceNo = strComboString.Left(pos);
	strCurDeviceNo.Trim();

	m_pEngineInfo->SourceType = IVCAVideoSource::DSHOWSOURCE;

	_sntprintf_s(m_pEngineInfo->tSourceData.szVlcPath, _countof(m_pEngineInfo->tSourceData.szVlcPath),
		_TRUNCATE, _T("%s"), (LPCTSTR) strCurDeviceNo);
*/
	return TRUE;
}

void CVideoConfigPage::LoadRawFileSetting()
{
	// RAWFILE_Path
	TCHAR* pchRawFilePath; 
//	if(IVCAVideoSource::COMPRESSEDFILESOURCE == m_pEngineInfo->SourceType|| IVCAVideoSource::RAWFILESOURCE == m_pEngineInfo->SourceType)
		pchRawFilePath = m_pEngineInfo->tSourceData.szRawPath;
//	else
//		pchRawFilePath = NULL;

	SetDlgItemText(IDC_ED_RAWFILEPATH, pchRawFilePath);
}

void CVideoConfigPage::LoadCap5Settings()
{
	// Cap5_Board No. and Cap5_Channel
	if (m_pEngineInfo->tSourceData.SourceType != IVCAVideoSource::CAP5SOURCE)
	{
		UpdateUsedChannelMask();
		m_pEngineInfo->tSourceData.Bd = 0;
		GetFirstAvailableChannel(0, &m_pEngineInfo->tSourceData.Ch);
	}

	if( m_pEngineInfo->tSourceData.ulVideoFormat == CMN5_VIDEO_FORMAT_NONE )
	{
		m_pEngineInfo->tSourceData.ulVideoFormat = CMN5_DEFAULT_VIDEO_FORMAT;
	}

	CComboBox* comboBoardNo = (CComboBox*)GetDlgItem(IDC_COMBO_BOARDNO);	
	comboBoardNo->SetCurSel(m_pEngineInfo->tSourceData.Bd);	// assume that Bd Num starts with 0
		
	CComboBox* comboChNo = (CComboBox*)GetDlgItem(IDC_COMBO_CHANNEL);
	comboChNo->SetCurSel(m_pEngineInfo->tSourceData.Ch);	// assume that Bd Num starts with 0

	// CAP5_VideoFormat
	m_ulCurVideoFormat = m_pEngineInfo->tSourceData.ulVideoFormat;
	CComboBox* comboVideoformat = (CComboBox*)GetDlgItem(IDC_COMBO_VIDEOFORMAT);
	if(m_pEngineInfo->tSourceData.ulVideoFormat > CMN5_VIDEO_FORMAT_NONE)
		comboVideoformat->SetCurSel(m_pEngineInfo->tSourceData.ulVideoFormat-1);

	// Cap5_ImageResolution
	InsertImgResolution();
	if (m_pEngineInfo->tSourceData.SourceType != IVCAVideoSource::CAP5SOURCE){
		CComboBox* comboImgResolution = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
		comboImgResolution->Clear();
		comboImgResolution->SetCurSel(0);
	} else {
		CComboBox* comboImgResolution = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
		const CMN5_RESOLUTION_INFO *pResInfo = CAPPConfigure::Instance()->GetResolutionInfo(m_pEngineInfo->tSourceData.Bd);
		if(pResInfo == NULL) return;
		int curImgSize = m_pEngineInfo->tSourceData.ulImageSize;	
		CString curResolution;
		curResolution.Format(_T("%dx%d"), CMN5_GETIMGWIDTH(curImgSize), CMN5_GETIMGHEIGHT(curImgSize));

		comboImgResolution->Clear();
		CString comboItem;
		for(int i=0;i<comboImgResolution->GetCount();i++){
			comboImgResolution->GetLBText(i, comboItem);
			if(comboItem.Compare(curResolution)==0)
				comboImgResolution->SetCurSel(i);
		}
	}


	// CAP5_ColorFormat
	CComboBox* comboColorformat = (CComboBox*)GetDlgItem(IDC_COMBO_COLORFORMAT);
	//	if(m_pEngineInfo->ulColorFormat == VCA5_COLOR_FORMAT_YUY2)
	comboColorformat->SetCurSel(0);

	// Cap5_Framerate
	InsertFPSCAP5();
	CComboBox* comboFramerate = (CComboBox*)GetDlgItem(IDC_COMBO_FRAMERATE_CAP5);
	TCHAR strFramerate[16];
	ULONG ulFrameRate;
	for(int i=0;i<comboFramerate->GetCount();i++){
		comboFramerate->GetLBText(i, strFramerate);
		ulFrameRate = _ttoi(strFramerate);
		if(ulFrameRate == m_pEngineInfo->tSourceData.ulFrameRate)comboFramerate->SetCurSel(i);
	}
}

void CVideoConfigPage::LoadStreamSourceSetting()
{
	TCHAR* pchVlcexeStreamPath = m_pEngineInfo->tSourceData.szVlcPath;
	CString strVlcexeStreamPath = pchVlcexeStreamPath;

	int pos1 = 0;
	int pos2 = 0;
	int pos3 = 0;

	CString strVlcExePath, strVlcOption;

	if (m_pEngineInfo->tSourceData.SourceType == IVCAVideoSource::STREAMSOURCE) {
		// GetVlcExePath
		CString strTmp = strVlcexeStreamPath;
		strTmp.MakeLower();
		pos1 = strTmp.Find(_T("vlc.exe"));
		if( -1 == pos1 ) {
			AfxMessageBox(_T("Can't find vlc.exe path"));
			return;
		}
		strVlcExePath = strVlcexeStreamPath.Left( pos1 + _tcslen(_T("vlc.exe")) );
		strVlcExePath.Trim();

	
		strVlcOption = strVlcexeStreamPath.Right(strVlcexeStreamPath.GetLength() - (pos1 + _tcslen(_T("vlc.exe"))) );
		strVlcOption.Trim(); 
/*&
		// GetStreamPath
		pos2 = strTmp.Find(_T("rtsp"));
		if( -1 == pos2 ) {
			AfxMessageBox(_T("Can't find stream source path"));
//			return;
		}

		// GetVlcOptions
		pos3 = strTmp.Find(_T("--sout"));
		if( -1 == pos3 ) {
			AfxMessageBox(_T("Can't find stream source path"));
//			return;
		}

		if(-1 != pos2){
			strStreamPath = strVlcexeStreamPath.Mid( pos2 , pos3-pos2 );
			strStreamPath.Trim();
		}

		if(-1 != pos3){
			strVlcOption = strVlcexeStreamPath.Right( strVlcexeStreamPath.GetLength() - pos3 );
			strVlcOption.Trim();
		}
*/
	}else{
		//default VCA execute path
		strVlcExePath = _T("C:\\Program Files\\VideoLAN\\VLC\\vlc.exe");
		//default Option
		strVlcOption =  _T("--sout=#transcode{vcodec=YUY2}:duplicate{dst=display,dst=std{access=file,mux=avi,dst=-}} --repeat") ;
	}
	
	// Vlc Exe Path
 	SetDlgItemText(IDC_ED_VLCEXEFILEPATH, strVlcExePath);

	// Vlc Stream Source Path
	//SetDlgItemText(IDC_ED_STREAMPATH, strStreamPath + _T(" ") + strVlcOption);
	SetDlgItemText(IDC_ED_STREAMPATH, strVlcOption);
}

void CVideoConfigPage::LoadDShowSetting()
{
	int idx = m_pEngineInfo->tSourceData.ulDShowDeviceId;

	CComboBox* comboDeviceNo = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICENUM_DSHOW);	
	comboDeviceNo->SetCurSel(idx);
}


void CVideoConfigPage::ResetCap5Settings()
{
	if (m_pEngineInfo->tSourceData.SourceType == IVCAVideoSource::CAP5SOURCE) {
		m_dwUsedChannelMask[m_pEngineInfo->tSourceData.Bd] &= ~(1 << m_pEngineInfo->tSourceData.Ch);
	}
}

void CVideoConfigPage::InsertImgResolution()
{
	DWORD curBoardNum	= m_pEngineInfo->tSourceData.Bd;
	CComboBox* comboImgResolution = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
	const CMN5_RESOLUTION_INFO *pResInfo = CAPPConfigure::Instance()->GetResolutionInfo(curBoardNum);

	if(pResInfo == NULL) return; 

	comboImgResolution->ResetContent();

	for(ULONG i=0;i<pResInfo->uSizeCount;i++){
		ULONG ImageDesc = pResInfo->aImgDesc[i].uSizeDesc;
		
		if(m_ulCurVideoFormat == CMN5_VIDEO_FORMAT_NTSC_M){
			if(ImageDesc & CMN5_IMAGESIZE_SUPPORT_NTSC_M) {
				ULONG ImgSize = pResInfo->aImgDesc[i].uImageSize;
				CString strResolution;
				strResolution.Format(_T("%dx%d"), CMN5_GETIMGWIDTH(ImgSize), CMN5_GETIMGHEIGHT(ImgSize));
				comboImgResolution->AddString(strResolution);
			}
		}else{
			if(ImageDesc & CMN5_IMAGESIZE_SUPPORT_PAL_B) {
				ULONG ImgSize = pResInfo->aImgDesc[i].uImageSize;
				CString strResolution;
				strResolution.Format(_T("%dx%d"), CMN5_GETIMGWIDTH(ImgSize), CMN5_GETIMGHEIGHT(ImgSize));
				comboImgResolution->AddString(strResolution);
			}
		}
	}

	comboImgResolution->SetCurSel(0);
}


void CVideoConfigPage::InsertFPSCAP5()
{
	DWORD curBoardNum	= m_pEngineInfo->tSourceData.Bd;
	CComboBox* comboFramerate = (CComboBox*)GetDlgItem(IDC_COMBO_FRAMERATE_CAP5);
	const CMN5_RESOLUTION_INFO *pResInfo = CAPPConfigure::Instance()->GetResolutionInfo(curBoardNum);

	if(pResInfo == NULL) return; 

	comboFramerate->ResetContent();

	if(m_ulCurVideoFormat == CMN5_VIDEO_FORMAT_NTSC_M){
		for(int i=30; i>=15; i--) {
			CString frmrate;
			frmrate.Format(_T("%d"), i);
			comboFramerate->AddString(frmrate);
		}
	}else{
		for(int i=25; i>=13; i--) {
			CString frmrate;
			frmrate.Format(_T("%d"), i);
			comboFramerate->AddString(frmrate);
		}
	}
	
	comboFramerate->SetCurSel(0);
}


BOOL CVideoConfigPage::GetFirstAvailableChannel(DWORD dwBd, DWORD *pCh)
{
	for (DWORD i = 0; i <  CAPPConfigure::Instance()->GetMaxChannel(dwBd); ++i) {
		if ((m_dwUsedChannelMask[dwBd] & (1 << i)) == 0) {
			*pCh = i;
			return TRUE;
		}
	}
	return FALSE;
}




void CVideoConfigPage::OnEnChangeEdRawfilepath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CConfigPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboFramerateFile()
{
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboBoardno()
{
	// TODO: Add your control notification handler code here
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboColorformat()
{
	// TODO: Add your control notification handler code here
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboResolution()
{
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboFramerateCap5()
{
	SyncToDataMgr();
}

void CVideoConfigPage::OnEnChangeEdVlcexefilepath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CConfigPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	SyncToDataMgr();
}

void CVideoConfigPage::OnEnChangeEdStreampath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CConfigPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboDevicenumDshow()
{
	SyncToDataMgr();
}

void CVideoConfigPage::OnCbnSelchangeComboVideoRotate()
{
	// TODO: Add your control notification handler code here
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_ROTATE);

	m_pEngineInfo->tSourceData.ulRotate = pComboBox->GetCurSel();
	
	CVCADataMgr *pDataMgr = m_pVCADialog->GetVCADataMgr();
	pDataMgr->FireEvent( IVCAConfigureObserver::VCA_ENGINE_UPDATE, NULL );
}

void CVideoConfigPage::OnCbnSelchangeComboFrameType()
{
	// TODO: Add your control notification handler code here
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_FRAME_TYPE);
	m_pEngineInfo->tSourceData.ulFrameType = pComboBox->GetCurSel();
		
	CVCADataMgr *pDataMgr = m_pVCADialog->GetVCADataMgr();
	pDataMgr->FireEvent( IVCAConfigureObserver::VCA_ENGINE_UPDATE, NULL );
}


void CVideoConfigPage::OnEnChangeEditROI(UINT id)
{
	if(m_bPopulating) return;

	m_pEngineInfo->tSourceData.rcROI.x	= GetDlgItemInt(IDC_EDIT_ROI_X);
	m_pEngineInfo->tSourceData.rcROI.y	= GetDlgItemInt(IDC_EDIT_ROI_Y);
	m_pEngineInfo->tSourceData.rcROI.w	= GetDlgItemInt(IDC_EDIT_ROI_W);
	m_pEngineInfo->tSourceData.rcROI.h = GetDlgItemInt(IDC_EDIT_ROI_H);

	if(m_pEngineInfo->tSourceData.rcROI.x + m_pEngineInfo->tSourceData.rcROI.w > (USHORT)VCA5_GETIMGWIDTH(m_pEngineInfo->tSourceData.ulImageSize)){
		m_pEngineInfo->tSourceData.rcROI.x = 0;	
		m_pEngineInfo->tSourceData.rcROI.w = VCA5_GETIMGWIDTH(m_pEngineInfo->tSourceData.ulImageSize);
	}

	if(m_pEngineInfo->tSourceData.rcROI.y + m_pEngineInfo->tSourceData.rcROI.h > (USHORT)VCA5_GETIMGHEIGHT(m_pEngineInfo->tSourceData.ulImageSize)){
		m_pEngineInfo->tSourceData.rcROI.y = 0;	
		m_pEngineInfo->tSourceData.rcROI.h = VCA5_GETIMGHEIGHT(m_pEngineInfo->tSourceData.ulImageSize);
	}

	CVCADataMgr *pDataMgr = m_pVCADialog->GetVCADataMgr();
	pDataMgr->FireEvent( IVCAConfigureObserver::VCA_ENGINE_UPDATE, NULL );
}
