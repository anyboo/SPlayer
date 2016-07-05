#ifndef CDIALOGCASEADD_H
#define CDIALOGCASEADD_H

#include <QDialog>
#include "ui_cdialogcaseadd.h"
#include "NoFlameDlg.h"

class CDialogCaseAdd : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogCaseAdd(QWidget *parent = 0);
	~CDialogCaseAdd();
public:
	void GetCaseFileList(QString &caseName, QString &pointName, QStringList& strFileList){
		caseName = m_strCaseName;
		pointName = m_strCasePointName;
		strFileList= m_strFileList;
	}
	void SetCasePointAddMode()
	{
		ui.label->hide();
		ui.lineEditCaseName->hide();
		this->setWindowTitle(QStringLiteral("新建监控点"));
		ui.labelTitle_2->setText(QStringLiteral("新建监控点"));
	}
protected slots:
	void OnBtnFileAddClick();
	void OnBtnOKClick();
private:
	Ui::CDialogCaseAdd ui;
	QStringList m_strFileList;
	QString m_strCaseName;
	QString m_strCasePointName;
};

#endif // CDIALOGCASEADD_H
