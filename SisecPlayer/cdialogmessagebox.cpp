/*
#include <shellapi.h>*/
#include "cdialogmessagebox.h"
#include <qtimer.h>



CDialogMessageBox::CDialogMessageBox(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

	ui.label->setText("");
	ui.plainTextEdit->setPlainText("");
	m_timer = new QTimer();

	ui.BtnOpen->hide();

	connect(m_timer, SIGNAL(timeout()), this, SLOT(OnBtnOKClicked()));
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(OnBtnOKClicked()));

	m_timer->start(1000);

}

CDialogMessageBox::~CDialogMessageBox()
{
	delete m_timer;
}

void CDialogMessageBox::SetOpenFileName(QString fileName)
{
	m_fileNameToOpen = fileName;
	ui.BtnOpen->show();

}

void CDialogMessageBox::SetText1(QString text)
{
	ui.label->setText(text);
}

void CDialogMessageBox::SetText2(QString text)
{
	ui.plainTextEdit->setPlainText(text);
}

void CDialogMessageBox::OnBtnOpenClicked()
{
//	ShellExecute(GetDesktopWindow(), _T("open"), m_fileNameToOpen, NULL, NULL, SW_SHOWNORMAL);
}

void CDialogMessageBox::OnBtnOKClicked()
{
	this->accept();
}