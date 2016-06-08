#ifndef CDIALOGCONFIG_H
#define CDIALOGCONFIG_H

#include <QDialog>
#include "ui_cdialogconfig.h"

class CDialogConfig : public QDialog
{
	Q_OBJECT

public:
	CDialogConfig(QWidget *parent = 0);
	~CDialogConfig();
public:
	static void ReadConfigXml();
	static bool WriteConfigXml();
	static void GetPicParam(QString &path, int &picType, int &picRepeatCount)
	{
		path = m_strPicPath;
		picType = m_picType;
		picRepeatCount = m_picRepeatCount;
	}
protected slots:
	void OnBtnOpenClick();
	void OnBtnConfigClick();
	void OnBtnCancelClick();
private:
	Ui::CDialogConfig ui;
public:
	static QString m_strCutPath;
	static QString m_strConvertPath;
	static QString m_strPicPath;
	static int m_picType;
	static int m_picRepeatCount;
};

#endif // CDIALOGCONFIG_H
