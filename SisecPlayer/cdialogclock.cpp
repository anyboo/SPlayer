#include "cdialogclock.h"

CDialogClock::CDialogClock(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::WindowSystemMenuHint);

	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.BtnCancel, SIGNAL(clicked()), this, SLOT(reject()));

}

CDialogClock::~CDialogClock()
{

}
