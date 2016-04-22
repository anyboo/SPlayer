#ifndef CDIALOGDUBBING_H
#define CDIALOGDUBBING_H

#include <QDialog>
#include "ui_cdialogdubbing.h"



class CDialogDubbing : public QDialog
{
	Q_OBJECT

public:
	CDialogDubbing(QWidget *parent = 0);
	~CDialogDubbing();
public:
	QString GetVideoFileName();
	QString GetAudioFileName();
protected slots:
	void OnBtnVideoFileOpenClick();
	void OnBtnAudioFileOpenClick();
	void OnBtnOKClick();
	void OnBtnCancelClick();

private:
	Ui::CDialogDubbing ui;
};

#endif // CDIALOGDUBBING_H
