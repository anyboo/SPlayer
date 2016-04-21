#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets/QDialog>
#include <string>

#include "ui_dialog.h"

using namespace std;

class Dialog : public QDialog
{
	Q_OBJECT
public: 
    static QString getApllyCode();
public:
	Dialog(QWidget *parent = 0);
	~Dialog();
    static bool hasKey();
public slots:
	void ClickButn();
    void ClickTempAuth();
private:
	Ui::DialogClass ui;

private:
	QString m_strApply;
};

#endif // DIALOG_H
