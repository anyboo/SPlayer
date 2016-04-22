#include "cdialogdubbing.h"
#include "qfiledialog.h"
#include <qmessagebox.h>
#include "public_def.h"


CDialogDubbing::CDialogDubbing(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setModal(true);

	connect(ui.BtnVideoFileOpen, SIGNAL(clicked()), this, SLOT(OnBtnVideoFileOpenClick()));
	connect(ui.BtnAudioFileOpen, SIGNAL(clicked()), this, SLOT(OnBtnAudioFileOpenClick()));
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(OnBtnOKClick()));
	connect(ui.BtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClick()));
}

CDialogDubbing::~CDialogDubbing()
{

}
void CDialogDubbing::OnBtnVideoFileOpenClick()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		QStringLiteral("请选择文件"), "", FileFilter);
	if (fileName != "")
	{	
		ui.CmbVideoFile->setCurrentText(fileName);
	}
}

void CDialogDubbing::OnBtnAudioFileOpenClick()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		QStringLiteral("请选择文件"), "", AudioFilter);
	if (fileName != "")
	{
		ui.CmbAudioFile->setCurrentText(fileName);
	}
}

void CDialogDubbing::OnBtnOKClick()
{
	QString videoFile=ui.CmbVideoFile->currentText();
	QString audioFile = ui.CmbAudioFile->currentText();
	if (videoFile == "")
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择视频文件！"));
		return;
	}
	if (audioFile == "")
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择配音文件！"));
		return;
	}

	if (-1 == ui.CmbVideoFile->findText(videoFile))
	{
		ui.CmbVideoFile->addItem(videoFile);
	}
	if (-1 == ui.CmbAudioFile->findText(audioFile))
	{
		ui.CmbAudioFile->addItem(audioFile);
	}

	this->accept();
}

void CDialogDubbing::OnBtnCancelClick()
{
	this->reject();
}

QString CDialogDubbing::GetVideoFileName()
{
	return ui.CmbVideoFile->currentText();
}

QString CDialogDubbing::GetAudioFileName()
{
	return ui.CmbAudioFile->currentText();
}