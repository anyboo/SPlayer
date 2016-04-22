#ifndef CDIALOGHELP_H
#define CDIALOGHELP_H

#include <QDialog>
#include "ui_cdialoghelp.h"

class CDialogHelp : public QDialog
{
	Q_OBJECT

public:
	CDialogHelp(QWidget *parent = 0);
	~CDialogHelp();

protected slots:
	void OnBtnOKClick();
private:
	Ui::CDialogHelp ui;
};

#endif // CDIALOGHELP_H
