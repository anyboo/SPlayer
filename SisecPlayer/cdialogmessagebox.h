#ifndef CDIALOGMESSAGEBOX_H
#define CDIALOGMESSAGEBOX_H

#include <QDialog>
#include "ui_cdialogmessagebox.h"

class CDialogMessageBox : public QDialog
{
	Q_OBJECT

public:
	CDialogMessageBox(QWidget *parent = 0);
	~CDialogMessageBox();

	void SetText1(QString text);
	void SetText2(QString text);
	void SetOpenFileName(QString fileName);
	
protected slots:
	void OnBtnOpenClicked();
	void OnBtnOKClicked();
private:
	Ui::CDialogMessageBox ui;

	QTimer *m_timer;
	QString m_fileNameToOpen;
};

#endif // CDIALOGMESSAGEBOX_H
