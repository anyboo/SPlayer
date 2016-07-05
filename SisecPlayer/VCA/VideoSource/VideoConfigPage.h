#pragma once

#include "../common/APPConfigure.h"
#include "../VCADialog/ConfigPage.h"

// CVideoConfigPage dialog

class CVideoConfigPage : public CConfigPage
{
	DECLARE_DYNAMIC(CVideoConfigPage)

public:

	CVideoConfigPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVideoConfigPage();
	enum { IDD = IDD_ENGINE_SETTING_DIALOG };

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnFileopen();
	afx_msg void OnBnClickedBtnVlcpathopen();
	afx_msg void OnBnClickedRadioRawfile();
	afx_msg void OnBnClickedRadioCap5();
	afx_msg void OnBnClickedRadioVlc();
	afx_msg void OnCbnSelchangeComboVideoformat();
	afx_msg void OnCbnSelchangeComboChannel();
	afx_msg void OnDestroy();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	

private:

	BOOL				m_bPopulating;
	DWORD				m_engId;
	VCA5_APP_ENGINE_INFO* m_pEngineInfo;
	ULONG				m_ulCurVideoFormat;
	DWORD				m_dwUsedChannelMask[CMN5_SYSTEM_MAX_BOARD];

	void	InitCap5DlgCtrls();
	void	InitDShowDlgCtrls();
	void	EnableDlgControls();
	void	InitComboBox();

	void	LoadCap5Settings();
	void	LoadRawFileSetting();
	void	LoadStreamSourceSetting();
	void	LoadDShowSetting();
	BOOL	SaveRawFileSetting();
	BOOL	SaveCap5Settings();
	BOOL	SaveStreamSourceSetting();
	BOOL	SaveDShowSetting();
	void	ResetCap5Settings();
		

	CWnd	*m_pOldParent;
	
	void	SyncToDataMgr();
	void	InsertImgResolution();
	void	InsertFPSCAP5();
	BOOL	GetFirstAvailableChannel(DWORD dwBd, DWORD *pCh);

	void	UpdateUsedChannelMask();

protected:
	afx_msg void OnBnClickedRadioDshow();
	afx_msg void OnBnClickedRadioNotSource();
	afx_msg void OnEnChangeEdRawfilepath();
	afx_msg void OnCbnSelchangeComboFramerateFile();
	afx_msg void OnCbnSelchangeComboBoardno();
	afx_msg void OnCbnSelchangeComboColorformat();
	afx_msg void OnCbnSelchangeComboResolution();
	afx_msg void OnCbnSelchangeComboFramerateCap5();
	afx_msg void OnEnChangeEdVlcexefilepath();
	afx_msg void OnEnChangeEdStreampath();
	afx_msg void OnCbnSelchangeComboDevicenumDshow();
	afx_msg void OnCbnSelchangeComboVideoRotate();
	afx_msg void OnCbnSelchangeComboFrameType();
	afx_msg void OnEnChangeEditROI(UINT id);
};
