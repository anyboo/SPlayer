#ifndef CDIALOGVCA_H
#define CDIALOGVCA_H
#include "stdafx.h"
#include <QDialog>
#include "ui_cdialogvca.h"
#include "noflamedlg.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"
//#include "IVCAObject.h"
//#include "SimpleVCA.h"
#include "Render\D3DCalibrationRender.h"
#include "VCA\Render\VCADataMgr.h"
#include "VCA\Render\ZoneSettingCtl.h"
#include "cwidgetvcaprogress.h"


class CDialogVCA : public QDialog
{
	Q_OBJECT

public:
	CDialogVCA(QWidget *parent = 0);
	~CDialogVCA();
public:
	virtual void customEvent(QEvent * event);
public:
	void Reset();
	void OnVCAFinish();
	HWND GetRenderHwnd();
	CVCADataMgr	*GetVCADataMgr();
	void SetProgress(CWidgetVCAProgress *pwidgetProgress)
	{
		m_widgetProgress = pwidgetProgress;
	}
	
protected slots:
//	void OnBtnCloseClick();
//	void OnBtnMinClick();
	void OnBtnStartClick();
	void OnBtnStartColorClick();
	void OnBtnRectClick();
	void OnBtnLineClick();
	void OnBtnLine2Click();
	void OnBtnDelClick();
	void OnBtnDelAllClick();
	void OnBtnFileAddClick();
	void OnBtnFileDelClick();
	void OnBtnFileDelAllClick();
	void OnBtnColorFromScreenClick();
	void OnBtnColorMoreClick();

	void OnBtnCaseAddClick();
	void OnBtnCasePointAddClick();

	void OnTreeWidgetItemDbClick(QTreeWidgetItem * item, int column);
	void  OnTreeItemChanged(QTreeWidgetItem *item, int column);
	void OnCheckColorEnabledChanged(int state);
	void  OnCurrentIndexChanged(int index);
private:
	void UpdateTreeWidget();
	void UpdateVCARuleText();
	void InsertRuleTable(QString &str);
	void InsertFileItem(QString strPathName, QTreeWidgetItem *pParent);
	bool Start(bool bZoneShow=true);
private:
	Ui::CDialogVCA ui;
	
	//检索条件设定
	CWidgetPlayWnd *m_PlayWnd;
	CFormPlayCtrl *m_PlayCtrl;	
	CCalibrationD3DRender *m_Render;

	CZoneSettingCtl *m_ZoneCtrl;
	CVCADataMgr		*m_pDataMgr;

	QList<QString> m_listRule;
	CWidgetVCAProgress *m_widgetProgress;
	
	int m_curPointVCA;
	QTreeWidgetItem *m_preItem;
};

#endif // CDIALOGVCA_H
