#ifndef CDIALOGCLOCK_H
#define CDIALOGCLOCK_H

#include <QDialog>
#include "ui_cdialogclock.h"
#include <qdatetime.h>

class CDialogClock : public QDialog
{
	Q_OBJECT

public:
	CDialogClock(QWidget *parent = 0);
	~CDialogClock();
	//virtual void close();
public:
	void SetDateTime(QDateTime& dateTime){	
		ui.dateTimeEdit->setDateTime(dateTime);
		ui.dateTimeEdit->setCurrentSectionIndex(4);
	}
	QDateTime GetDateTime()
	{

		return ui.dateTimeEdit->dateTime();
	}
private:
	Ui::CDialogClock ui;
};

#endif // CDIALOGCLOCK_H
