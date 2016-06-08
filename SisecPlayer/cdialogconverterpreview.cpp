#include "cdialogconverterpreview.h"
#include <qthread.h>
#include <qmessagebox.h>

CDialogConverterPreview::CDialogConverterPreview(QWidget *parent)
: NoFlameDlg(parent)
{
	ui.setupUi(this);

	m_PlayWnd = new CWidgetPlayWnd(0, this);
	m_PlayCtrl = new CFormPlayCtrl(0, m_PlayWnd, this);

	m_PlayWnd->SetPreViewMode();
	m_PlayWnd->setGeometry(5, 33, 680, 460);
	m_PlayCtrl->setGeometry(5, 33+460-5, 680, 70);
	m_PlayCtrl->HideNotUsedButton();
	m_PlayWnd->show();
	m_PlayCtrl->show();
	ui.groupBoxInfo->hide();
	ui.groupBoxExtern->hide();
	ui.groupBoxExtern->setChecked(false);
	ui.groupBoxCut->setChecked(false);

	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(OnBtnOKClick()));
	connect(ui.BtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCloseClick()));
	connect(ui.BtnClose, SIGNAL(clicked()), this, SLOT(OnBtnCloseClick()));
	connect(ui.BtnTimeStart, SIGNAL(clicked()), this, SLOT(OnBtnTimeStartClick()));
	connect(ui.BtnTimeStop, SIGNAL(clicked()), this, SLOT(OnBtnTimeStopClick()));

	
}

CDialogConverterPreview::~CDialogConverterPreview()
{
	delete m_PlayWnd;
	delete m_PlayCtrl;
}

void CDialogConverterPreview::OnBtnOKClick()
{
	 bool bCheck= ui.groupBoxCut->isChecked();
	 if (bCheck)
	{
		QTime t1 = ui.timeEditStart->time();
		unsigned long long dStartTime = t1.hour() * 3600 + t1.minute() * 60 + t1.second();

		QTime  t2 = ui.timeEditStop->time();
		unsigned long long dStopTime = t2.hour() * 3600 + t2.minute() * 60 + t2.second();

		if (dStartTime >= dStopTime)
		{
			QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("开始时间不能大于或等于结束时间！"));
			return;
		}

		int dTotalTime = m_FileInfo->iFileDuation;
		if (dStopTime>dTotalTime)
		{
			QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("结束时间不能大于文件总时间！"));
			return;
		}
		
		m_FileInfo->iStartTime = dStartTime;
		m_FileInfo->iEndTime = dStopTime;
	}
	m_FileInfo->bFileCut = bCheck;
	m_FileInfo->dstFormat = ui.comboBoxFormat->currentIndex();
	m_PlayCtrl->Stop();
	this->accept();
}

void CDialogConverterPreview::UkeyDownStop()
{
	if (m_PlayCtrl->IsPlaying())
	{
		m_PlayCtrl->Pause(1);
	}
}

void CDialogConverterPreview::OnBtnCloseClick()
{
	m_PlayCtrl->Stop();
	this->reject();
}

void CDialogConverterPreview::SetConvertFileInfo(T_ConverterFileInfo *FileInfo)
{
	m_FileInfo = FileInfo;
	QString strPathName = m_FileInfo->strSrcPath + "/" + m_FileInfo->strSrcFileName;
	bool bRet = m_PlayCtrl->OpenAndPlayFile(strPathName, (HWND)m_PlayWnd->GetPlayWndID());
	if (bRet)
	{
		QThread::usleep(1000);
		//m_PlayCtrl->Player_Pause(1);
		m_PlayWnd->SetTitleFileName(strPathName);
		m_PlayCtrl->SetRenderMode();
	}

	ui.comboBoxFormat->setCurrentIndex(m_FileInfo->dstFormat);
	ui.groupBoxCut->setChecked(m_FileInfo->bFileCut);
		
	DWORD dTime = m_FileInfo->iStartTime;
	int hours = dTime / 3600;
	int mins = dTime / 60 % 60;
	int secs = dTime % 60;
	QTime time(hours, mins, secs);
	ui.timeEditStart->setTime(time);

	dTime = m_FileInfo->iEndTime;
	hours = dTime / 3600;
	mins = dTime / 60 % 60;
	secs = dTime % 60;
	QTime time2(hours, mins, secs);
	ui.timeEditStop->setTime(time2);
}

void CDialogConverterPreview::OnBtnTimeStartClick()
{
	DWORD dTime = m_PlayCtrl->GetCurPlayTime();

	//ui.timeEditStart->setTimeSpec(Qt::TimeSpec spec)
	int hours = dTime / 3600;
	int mins = dTime / 60 % 60;
	int secs = dTime % 60;
	QTime time(hours, mins, secs);
	ui.timeEditStart->setTime(time);
}

void CDialogConverterPreview::OnBtnTimeStopClick()
{
	m_FileInfo->iFileDuation = m_PlayCtrl->GetFileTime();
	DWORD dTime = m_PlayCtrl->GetCurPlayTime();
	int hours = dTime / 3600;
	int mins = dTime / 60 % 60;
	int secs = dTime % 60;
	QTime time(hours, mins, secs);
	ui.timeEditStop->setTime(time);
}