#ifndef CDIALOGCONVERTER_H
#define CDIALOGCONVERTER_H

#include <QDialog>
#include "ui_cdialogconverter.h"
#include "NoFlameDlg.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"
#include "cdialogconverterpreview.h"
#include <qmessagebox.h>
#include <qthread.h>

enum View_Head{
	FileName = 0,
	FilePath,
	FileSize,
	FileDuation,
	FileWidthHeight,
	DstFormat,
	Progress,
	State,
	Cut,
	Column_Count,
};
enum Convert_State{
	NOTSTART,
	CONVERTING,
	PAUSE,
	STOP,
	FINISH,
	ERROR,
};

class CDialogConverter;

class Thread :public QThread
{
	Q_OBJECT
public:
	Thread();
	~Thread();
public:
	void SetOwner(QDialog *pOwner){ m_pOwner = pOwner; }
private:
//	bool m_stopFlag;
	QDialog *m_pOwner;
protected:
	void run();

};
class CDialogConverter : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogConverter(QWidget *parent = 0);
	~CDialogConverter();
public:
	int GetRowProgress(int row);
	void StartReadSrcFileInfo();
private:
	void GoNextConvert();
	void StopCurTask();
	void SetCtrlEnabled(bool bEnabled);
	void EditConvertInfo(int row);
signals:
	void finish();
protected slots:
	void OnBtnMinClick();
	void OnBtnCloseClick();
	void OnBtnFileAddClick();
	void OnBtnFileDelClick();
	void OnBtnEditClick();
	void OnBtnFileDelAllClick();
	void OnBtnStartClick();
	void OnBtnStopClick();
	void OnItemDoubleClicked(QTableWidgetItem*item);
	void TimeGetProgress();
	void FinishGetSrcFileInfo();

private:
	Ui::CDialogConverter ui;

	QList<T_ConverterFileInfo> m_convertFileInfoList;
	CWidgetPlayWnd *m_PlayWnd;
	CFormPlayCtrl *m_PlayCtrl;
	long m_nPort;
	

	QTimer *m_getProgressTimer;
	int m_curTask;
	
	QMessageBox m_msgDlg;

	Thread m_threadReadSrcFile;

};

#endif // CDIALOGCONVERTER_H
