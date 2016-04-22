#ifndef CDIALOGCUT_H
#define CDIALOGCUT_H

#include <QDialog>
#include "ui_cdialogcut.h"
#include "noflamedlg.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"

class CDialogCut : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogCut(QWidget *parent = 0);
	~CDialogCut();
	virtual bool eventFilter(QObject *obj, QEvent *e);
protected slots:
	void OnBtnMinClick();
	void OnBtnCloseClick();

	void OnBtnStartCutClick();
	void OnBtnCutStopClick();
	void OnBtnFilePathClick();
	void OnBtnTimeStartClick();
	void OnBtnTimeStopClick();

	void OnBtnFileOpenClick();
	
	void TimeGetProgress();
private:
	void StopTask();
	void SetCtrlEnabled(bool bEnabled)
	{
		ui.BtnCutStop->setEnabled(!bEnabled);
		ui.progressBar->setEnabled(!bEnabled);

		ui.BtnStartCut->setEnabled(bEnabled);
		m_PlayCtrl->setEnabled(bEnabled);
		ui.frameTime->setEnabled(bEnabled);
		ui.framePath->setEnabled(bEnabled);

	}
private:
	Ui::CDialogCut ui;

	CWidgetPlayWnd *m_PlayWnd;
	CFormPlayCtrl *m_PlayCtrl;

	QTimer *m_getProgressTimer;
	long m_nPort;
	QString m_strDstPath;
};

#endif // CDIALOGCUT_H
