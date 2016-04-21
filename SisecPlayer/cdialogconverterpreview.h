#ifndef CDIALOGCONVERTERPREVIEW_H
#define CDIALOGCONVERTERPREVIEW_H

#include <QDialog>
#include "ui_cdialogconverterpreview.h"
#include "NoFlameDlg.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"

struct T_ConverterFileInfo{
	QString strSrcFileName;
	QString strSrcPath;
	unsigned __int64 iFileSize;
	//unsigned __int64 iFileDuation;
	DWORD iFileDuation;
	int width;
	int height;
	int dstFormat;
	//	QString dstPath;
	int state;
	int progress;
	bool bFileCut;
	unsigned __int64 iStartTime;
	unsigned __int64 iEndTime;
};

class CDialogConverterPreview : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogConverterPreview(QWidget *parent = 0);
	~CDialogConverterPreview();
public:
	void SetConvertFileInfo(T_ConverterFileInfo*FileInfo);
protected slots:
	void OnBtnCloseClick();
	void OnBtnOKClick();
	void OnBtnTimeStartClick();
	void OnBtnTimeStopClick();
private:
	Ui::CDialogConverterPreview ui;

	CWidgetPlayWnd *m_PlayWnd;
	CFormPlayCtrl *m_PlayCtrl;

	T_ConverterFileInfo *m_FileInfo;
};

#endif // CDIALOGCONVERTERPREVIEW_H
