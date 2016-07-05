#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif
#include "cformplayctrl.h"
#include <windows.h>
#include "cdialogmain.h"
#include <QTextCodec>
#include "cwidgetplaywnd.h"
#include <QTime>
#include <QThread>
#include "cdialogclock.h"
#include "cdialogcolorset.h"
#include "Render\D3DCalibrationRender.h"

#define MIN_DIVPLAYTIME (20)
#define DIV_NUM 4

QString s_strSpeed[] = { "1/16", "1/8", "1/4", "1/2", "1", "2", "4", "8", "16" };

CFormPlayCtrl::CFormPlayCtrl(int index, CWidgetPlayWnd*pPlayWnd, QWidget *parent)
: QWidget(parent), m_pPlayWnd(pPlayWnd)
{
	ui.setupUi(this);

	m_audioCtrl = NULL;
	m_bAudioAtrrible = false;

	m_playMode = SINGLE_MODE;
	m_index = index;

	float fDif = 1.0 / DIV_NUM;
	m_fStartPos = fDif*m_index;
	m_fClosePos = fDif*(m_index + 1);

	m_bSoundOn = true;

	ui.SliderPlaytime->setRange(0, 100);
	ui.SliderPlaytime->setTickInterval(10);
	ui.SliderVolume->setRange(0, 0xffff);
	ui.SliderVolume->setTickInterval(0xffff * 0.1);
	
	Init();

	m_VCAObject = NULL;
	m_Render = NULL;

	m_sliderTimer = new QTimer();
	connect(m_sliderTimer, SIGNAL(timeout()), this, SLOT(OnUpdateSlider()));
	connect(ui.btnSlow, SIGNAL(clicked()), this, SLOT(OnBtnSlowClick()));
	connect(ui.btnPause, SIGNAL(clicked()), this, SLOT(OnBtnPauseClick()));
	connect(ui.btnNext, SIGNAL(clicked()), this, SLOT(OnBtnNextClick()));
	connect(ui.btnPrevious, SIGNAL(clicked()), this, SLOT(OnBtnPreviousClick()));
	connect(ui.btnStepNext, SIGNAL(clicked()), this, SLOT(OnBtnStepNextClick()));
	connect(ui.btnStepBack, SIGNAL(clicked()), this, SLOT(OnBtnStepBackClick()));
	connect(ui.btnStop, SIGNAL(clicked()), this, SLOT(OnBtnStopClick()));
	connect(ui.btnFast, SIGNAL(clicked()), this, SLOT(OnBtnFastClick()));
	connect(ui.SliderPlaytime, SIGNAL(sliderPressed()), this, SLOT(OnSliderPressed()));//原本无sliderPressed响应，自己重写的sliderPressed信号
	connect(ui.SliderPlaytime, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderMove(int)));
	connect(ui.btnSound, SIGNAL(clicked()), this, SLOT(OnBtnSoundClick()));
	connect(ui.SliderVolume, SIGNAL(sliderPressed()), this, SLOT(OnSliderVolumePressed()));
	connect(ui.SliderVolume, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderVolumeMove(int)));
	connect(ui.btnClock, SIGNAL(clicked()), this, SLOT(OnBtnClockClick()));
	connect(m_pPlayWnd, SIGNAL(ShowColorDlg()), this, SLOT(OnShowColorDlg()));

	ui.SliderPlaytime->installEventFilter(this->parent());
	ui.btnSound->installEventFilter(this->parent());
	ui.btnSlow->installEventFilter(this->parent());
	ui.btnNext->installEventFilter(this->parent());
	ui.btnPrevious->installEventFilter(this->parent());
	ui.btnStepBack->installEventFilter(this->parent());
	ui.btnStop->installEventFilter(this->parent());
	ui.btnFast->installEventFilter(this->parent());
	ui.btnStepBack->installEventFilter(this->parent());
	this->installEventFilter(this->parent());
}

CFormPlayCtrl::~CFormPlayCtrl()
{
	delete m_sliderTimer;

}

void  CFormPlayCtrl::Init()
{
	m_bDubbing = false;

	m_bPlay = false;
	m_bPause = false;
	m_bStep = false;
	m_iSpeed = 4;
	m_bDIVEventFinish = false;
	m_bSetFileEndBck = false;

	m_bGetStartTime = false;
	m_dStartTime = 0;

	m_bManulClock = false;

	m_colorSetDlg = NULL;
	m_bGetColorSupport = false;
	m_bSetDisplayCallback = false;
	m_iBrightness= 0;
	m_iContrast= 0;
	m_iSaturation = 0; 
	m_iHue = 0;
	m_iUIBrightness = DEFAULT_COLOR;
	m_iUIContrast = DEFAULT_COLOR;
	m_iUISaturation = DEFAULT_COLOR;
	m_iUIHue = DEFAULT_COLOR;
	m_bSharp = false;
	m_bDecNoise = false;
	m_nSharpValue = 20;
	m_nDecNoiseValue = 20;
	m_nImagePort = -1;

	ui.SliderPlaytime->setValue(0);
	ui.SliderVolume->setValue(0xffff);
	ui.labelStartTime->setText("00:00:00");
	ui.labelEndTime->setText("00:00:00");
	m_ManulTime = QDateTime::currentDateTime();

	UpdatePauseState();
	SetSpeed();
	SetBtnCtrlEnabled(false);
}

void  CFormPlayCtrl::SetBtnCtrlEnabled(bool bEnabled)
{
	ui.SliderPlaytime->setEnabled(bEnabled);
	ui.btnPrevious->setEnabled(bEnabled);
	ui.btnFast->setEnabled(bEnabled);
	ui.btnNext->setEnabled(bEnabled);
	ui.btnPause->setEnabled(bEnabled);
	ui.btnSlow->setEnabled(bEnabled);
	ui.btnStop->setEnabled(bEnabled);	
	ui.btnStepNext->setEnabled(bEnabled);
	ui.btnStepBack->setEnabled(bEnabled);
	ui.btnClock->setEnabled(bEnabled);
}

void CFormPlayCtrl::HideNotUsedButton()
{
	ui.btnNext->hide();
	ui.btnPrevious->hide();
	ui.btnStepNext->hide();
	ui.btnStepBack->hide();
	ui.SliderVolume->hide();
	ui.btnSound->hide();
	ui.btnClock->hide();
	ui.frame->setStyleSheet("QFrame{background-color:rgb(100,100,100);}");
}

void CFormPlayCtrl::OnBtnPreviousClick()
{
	QApplication::postEvent(this->parent(), new CMyEvent((QEvent::Type)CUSTOM_PREVIOUS_EVENT,this));
}

void CFormPlayCtrl::OnBtnNextClick()
{
	QApplication::postEvent(this->parent(), new CMyEvent((QEvent::Type)CUSTOM_NEXT_EVENT,this));
}

void  CFormPlayCtrl::FileEndCallback(LONG nID, void *pUser)
{
	CFormPlayCtrl *pPlayerCtrl = (CFormPlayCtrl*)pUser;
	if (pPlayerCtrl)
	{
		if (!pPlayerCtrl->m_bAudioAtrrible)//配音文件不用自动关闭
		{
			QApplication::postEvent(pPlayerCtrl->parent(), new CMyEvent(CUSTOM_AutoStop_EVENT, pPlayerCtrl));//剪切窗口，不会自动关闭
		}	
	}
}

void  CFormPlayCtrl::DisplayCallback(DISPLAYCALLBCK_INFO *pstDisplayInfo)
{
	CFormPlayCtrl *pPlayerCtrl = (CFormPlayCtrl*)pstDisplayInfo->nUser;
	if (pPlayerCtrl)
	{
		pPlayerCtrl->ProcessColor((unsigned char*)pstDisplayInfo->pBuf, pstDisplayInfo->nWidth, pstDisplayInfo->nHeight);
		pPlayerCtrl->ImageProcess((unsigned char*)pstDisplayInfo->pBuf, pstDisplayInfo->nWidth, pstDisplayInfo->nHeight);

		if (pPlayerCtrl->m_Render)
		{
			//BYTE *pImage, BITMAPINFOHEADER *pbm, ULONG	ulRotate, CVCAMetaLib *pVCAMetaLib, RECT rcImageROI, RECT rcVCAROI, VCA5_TIMESTAMP *pTimestamp)
			//while (pPlayerCtrl->m_bPlay)
			{
				BITMAPINFOHEADER bm;
				bm.biWidth = pstDisplayInfo->nWidth;
				bm.biHeight = pstDisplayInfo->nHeight;
				pPlayerCtrl->m_Render->ChangeVCASourceInfo(&bm);

				ULONG	ulRotate = 0;
				RECT	rcImageROI = { 0, };
				RECT	rcVCAROI = { 0, };
				pPlayerCtrl->m_Render->RenderVCAData((BYTE*)pstDisplayInfo->pBuf, NULL, ulRotate, NULL, rcImageROI, rcVCAROI, NULL);

			}
			
		}

		if (pPlayerCtrl->m_VCAObject)
		{
			pPlayerCtrl->m_VCAObject->ProcessVCA(pstDisplayInfo);
		}
	}
}

void CFormPlayCtrl::OpenAudioFile(QString strAudioFile)
{
	if (m_audioCtrl)
	{
		m_audioCtrl->OpenAndPlayFile(strAudioFile, 0);
		m_bDubbing = true;
	}
}

bool  CFormPlayCtrl::OpenAndPlayFile(QString strFileName, HWND hwnd,bool bSpecify, int FileType)
{	
	int iFileType;
//	USES_CONVERSION;
//	char *pstrFile = (char *)strFileName.toStdString().c_str();//strFileName.toLocal8Bit().data();
	//char *pstrFile = strFileName.toLatin1().data();
//	QTextCodec *textcode = QTextCodec::codecForName("GBK");
	QTextCodec *textcode = QTextCodec::codecForName("GB18030");
	//textcode->fromUnicode(strFileName);
	//strcpy(str, string1);
	char szFileName[1024];
	strcpy(szFileName, textcode->fromUnicode(strFileName).data());
	if (bSpecify)
	{
		iFileType = FileType;
	}
	else{	
		//AnalyzeFileType(pstrFile, &iFileType);
		AnalyzeFileType(szFileName, &iFileType);

	}
	bool isSupport22Render = IsSupport22Renader(iFileType);

	bool ret = false;
	ret = GetPlayerInterface(&m_nPort);

	m_strCurPlayFileName = strFileName;

	if (ret)
	{
		ret = Player_OpenFile(m_nPort, szFileName, hwnd, iFileType);//unicode 转char 

		if (ret)
		{
			if (m_playMode == SINGLE_MODE)
			{
				Player_Play(m_nPort);
			}
			else if (m_playMode == TIME_MODE)
			{
				if (m_index != 0)//不是第一个窗口才设置pos,否则调用Player_SetPlayPos会导致中维的视频不能分段播放
				{
					bool bRet = Player_SetPlayPos(m_nPort, m_fStartPos);
					if (!bRet)
					{
						Player_Play(m_nPort);//为兼容海康视频要先Player_Play才Player_SetPlayPos有效；如果先Player_Play再Player_SetPlayPos，会导致开源播放器段时一开始读的时间不对，提早关闭
						QThread::msleep(10);
						Player_SetPlayPos(m_nPort, m_fStartPos);
					}
				}
				Player_Play(m_nPort);
				//	QThread::msleep(100);

			}
			m_bSetFileEndBck = Player_SetFileEndCallback(m_nPort, &FileEndCallback, this);
			if (isSupport22Render)//viss(播放mpg,ps文件),为false,因为Player_SetDisPlayCallback之后无显示图像
			{
				m_bSetDisplayCallback = Player_SetDisPlayCallback(m_nPort, &DisplayCallback, this);
			}
			if (m_bSetDisplayCallback&&CDialogMain::s_GetImgProInterfaceFun)
			{
				CDialogMain::s_GetImgProInterfaceFun(&m_nImagePort);
			}

			m_bPlay = true;
			UpdatePauseState();
			SetBtnCtrlEnabled(true);
			m_pPlayWnd->SetShowEnabled(true);

			if (m_bSoundOn)
			{
				Player_PlaySoundShare(m_nPort);
				int volume = Player_GetVolume(m_nPort);
				ui.SliderVolume->setValue(volume);		
			}
			else
			{
				Player_StopSoundShare(m_nPort);
			}

			
			m_sliderTimer->start(1000);
		}
		else//打开失败
		{
			Stop();
			if (!bSpecify)
			{
				int iFFmpegFactoryID = FindFFmpegFactoryID();
				if (iFileType != iFFmpegFactoryID)
				{
					return OpenAndPlayFile(strFileName, hwnd,true, iFFmpegFactoryID);
				}
				
			}
		}
	}
	return ret;
}

bool   CFormPlayCtrl::SetDisplayCallback()
{
	m_bSetDisplayCallback = Player_SetDisPlayCallback(m_nPort, &DisplayCallback, this);
	return m_bSetDisplayCallback;
}

bool   CFormPlayCtrl::GetPicSize(long *pWidth, long *pHeigth)
{
	return Player_GetPictureSize(m_nPort, pWidth, pHeigth);
}

bool  CFormPlayCtrl::CapturePic(QString fileName, int iType)
{
	QTextCodec *textcode = QTextCodec::codecForName("GB18030");
	char szFileName[1024];
	strcpy(szFileName, textcode->fromUnicode(fileName).data());
	return Player_CapturePic(m_nPort, szFileName, iType);
}


void CFormPlayCtrl::SetRenderMode(int renderMode)
{
	/*
	int width = 0;
	int height = 0;
	bool bRet = false;
	
	bRet = this->GetPicSize((long*)&width, (long*)&height);
	
	if (bRet&&width != 0 && height != 0)
	{

		QRect rc = m_pPlayWnd->geometry();
		float fVideo = (float)width / (float)height;
		float fWnd = (float)rc.width() / (float)rc.height();
		QRect rcVideo = rc;

		if ((fVideo - fWnd) >= 0.000001)//以Wnd长为准
		{
			int videoWidth = rc.width();
			int videoHeight = (float)videoWidth / (float)fVideo;
			rcVideo.setRect(0, (rc.height() - videoHeight) / 2, videoWidth, videoHeight);
		}
		else//以Wnd高为准
		{
			int videoHeight = rc.height();
			int videoWidth = videoHeight * fVideo;
			rcVideo.setRect((rc.width() - videoWidth) / 2, 0, videoWidth, videoHeight);
		}
		m_pPlayWnd->SetPlayWndPos(rcVideo);

	}
	else
	{
		QRect rc = m_pPlayWnd->geometry();
		QRect rcVideo = rc;
		rcVideo.setRect(0, 0, rc.width(), rc.height());
		m_pPlayWnd->SetPlayWndPos(rcVideo);
	}*/
	int width = 0;
	int height = 0;
	bool bRet = false;
	//	QThread::msleep(10);
	bRet =this->GetPicSize((long*)&width, (long*)&height);
	/*	if (width == 0 || height == 0)
	{
	QThread::usleep(1000);//大华的长宽来不及获取成功再获取一次 也不行?
	bRet = m_PlayCtrl[index]->GetPicSize((long*)&width, (long*)&height);
	}*/
	if (bRet&&width != 0 && height != 0)
	{
		QRect rc = m_pPlayWnd->geometry();
		float fVideo = (float)width / (float)height;
		float fWnd = (float)rc.width() / (float)rc.height();
		QRect rcVideo = rc;
		if (renderMode == Render4_3)
		{
			fWnd = (float)4 / (float)3;
		}
		if (renderMode == Render4_3)
		{
			fWnd = (float)16 / (float)9;
		}

		if ((fVideo - fWnd) >= 0.000001)//以Wnd长为准
		{
			int videoWidth = rc.width();
			int videoHeight = (float)videoWidth / (float)fVideo;
			rcVideo.setRect(0, (rc.height() - videoHeight) / 2, videoWidth, videoHeight);
		}
		else//以Wnd高为准
		{
			int videoHeight = rc.height();
			int videoWidth = videoHeight * fVideo;
			rcVideo.setRect((rc.width() - videoWidth) / 2, 0, videoWidth, videoHeight);
		}


		if (renderMode == RenderIni)
		{
			m_pPlayWnd->SetPlayWndPos(rcVideo);
		}
		else if (renderMode == RenderStrech)
		{
			rcVideo.setRect(0, 0, rc.width(), rc.height());
			m_pPlayWnd->SetPlayWndPos(rcVideo);
		}
		else if (renderMode == Render4_3)
		{
			m_pPlayWnd->SetPlayWndPos(rcVideo);
		}
		else if (renderMode == Render16_9)
		{
			m_pPlayWnd->SetPlayWndPos(rcVideo);
		}
	}
	else
	{
		QRect rc = m_pPlayWnd->geometry();
		QRect rcVideo = rc;
		rcVideo.setRect(0, 0, rc.width(), rc.height());
		m_pPlayWnd->SetPlayWndPos(rcVideo);
	}
}

void  CFormPlayCtrl::Stop()
{
	if (m_sliderTimer->isActive())
	{
		m_sliderTimer->stop();
	}
	if (m_bDubbing&&m_audioCtrl!=NULL)
	{
		m_audioCtrl->Stop();
		m_bDubbing = false;
	}
	m_pPlayWnd->SetShowEnabled(false);
	
	Player_Stop(m_nPort);
	FreePlayerInterface(m_nPort);

	if (CDialogMain::s_FreeImgProInterfaceFun)
	{
		CDialogMain::s_FreeImgProInterfaceFun(m_nImagePort);
	}

	if (m_colorSetDlg)
	{
		m_colorSetDlg->accept();
	}

	Init();
	
	m_pPlayWnd->Refresh();	
}

void CFormPlayCtrl::OnBtnSlowClick()
{
	if (!m_bPause)
	{
		if (m_iSpeed == 0)//已经是最小速度1/16，不再减小了
		{
			return;
		}
		if (m_bDubbing&&m_audioCtrl != NULL)
		{
			m_audioCtrl->OnBtnSlowClick();
		}
		
		bool bRet=Player_Slow(m_nPort);
		if (bRet)
		{
			m_iSpeed--;
			SetSpeed();
		}
	}
}

void CFormPlayCtrl::GoFastSpeed()
{
	OnBtnFastClick();
	OnBtnFastClick();
	OnBtnFastClick();
	OnBtnFastClick();
	Player_FastNoDelay(m_nPort);
}

void CFormPlayCtrl::OnBtnFastClick()
{
	if (!m_bPause)
	{	
		if (m_iSpeed == 8)//已经是最大速度16，不再增加了
		{
			return;
		}
		if (m_bDubbing&&m_audioCtrl != NULL)
		{
			m_audioCtrl->OnBtnFastClick();
		}
		bool bRet=Player_Fast(m_nPort);
		if (bRet)
		{
			m_iSpeed++;
			SetSpeed();
		}
	}
}

void CFormPlayCtrl::SetSpeed()
{
	if (m_iSpeed>=0 && m_iSpeed <= 8 && m_pPlayWnd){
		QString strSpeed = QString(QStringLiteral("速率*%1")).arg(s_strSpeed[m_iSpeed]);
		m_pPlayWnd->SetSpeedString(strSpeed);
	}
}

void CFormPlayCtrl::UpdatePauseState()
{
	if (m_bPause)
	{
		if (m_pPlayWnd){
		m_pPlayWnd->SetPauseString(QStringLiteral("暂停"));}

		ui.btnPause->setToolTip(QStringLiteral("播放"));
		ui.btnPause->setStyleSheet("QPushButton{border-image: url(:/Skin/control-button-play-pause.png)0 0 0 0 repeat}QPushButton:hover{border-image: url(:/Skin/control-button-play-pause.png)40 0 0 0 repeat}QPushButton:pressed{border-image: url(:/Skin/control-button-play-pause.png)80 0 0 0 repeat}QPushButton:disabled{border-image: url(:/Skin/control-button-play-pause.png)120 0 0 0 repeat}");
		ui.btnPause->repaint();

	}
	else
	{
		if (m_pPlayWnd){
			m_pPlayWnd->SetPauseString("");}

		ui.btnPause->setToolTip(QStringLiteral("暂停"));
		ui.btnPause->setStyleSheet("QPushButton{border-image: url(:/Skin/control-button-play-pause.png)0 0 0 40 repeat}QPushButton:hover{border-image: url(:/Skin/control-button-play-pause.png)40 0 0 40 repeat}QPushButton:pressed{border-image: url(:/Skin/control-button-play-pause.png)80 0 0 40 repeat}QPushButton:disabled{border-image: url(:/Skin/control-button-play-pause.png)120 0 0 40 repeat}");
		ui.btnPause->repaint();
	}
}

void CFormPlayCtrl::Pause(int nPause)
{
	if ((!m_bPause&&nPause) || (m_bPause&&!nPause))
	{
		if (Player_Pause(m_nPort, !m_bPause))
		{
			if (m_bDubbing&&m_audioCtrl != NULL)
			{
				DWORD dAudioTotalTime = m_audioCtrl->GetFileTime();
				DWORD dVideoTotalTime = this->GetFileTime();
				int value = ui.SliderPlaytime->value();
				int curTime = value* dVideoTotalTime / 100.0;
				if (curTime < dAudioTotalTime)
				{
					m_audioCtrl->OnBtnPauseClick();
				}

			}
			m_bPause = !m_bPause;
			UpdatePauseState();
		}
	}
}

void CFormPlayCtrl::OnBtnPauseClick()
{
	if (m_bStep)
	{
		Player_Play(m_nPort);
		m_iSpeed = 4;
		SetSpeed();
		m_bPause = false;
		m_bStep = false;
		UpdatePauseState();
	}
	else if (Player_Pause(m_nPort, !m_bPause))
	{
		if (m_bDubbing&&m_audioCtrl != NULL)
		{
			DWORD dAudioTotalTime = m_audioCtrl->GetFileTime();
			DWORD dVideoTotalTime = this->GetFileTime();
			int value = ui.SliderPlaytime->value();
			int curTime = value* dVideoTotalTime / 100.0;
			if (curTime < dAudioTotalTime)
			{
				m_audioCtrl->OnBtnPauseClick();
			}

		}
		m_bPause = !m_bPause;
		UpdatePauseState();
	}
}

void CFormPlayCtrl::OnBtnStopClick()
{

	QApplication::postEvent(this->parent(), new CMyEvent(CUSTOM_ManualStop_EVENT, this));
	
}

void CFormPlayCtrl::OnSliderPressed()
{
/*	if (m_bDubbing&&m_audioCtrl != NULL)
	{
		m_audioCtrl->OnSliderPressed();
	}*/
	int value = ui.SliderPlaytime->value();
	OnSliderMove(value);
}

void CFormPlayCtrl::PauseDubbingAudio(DWORD nPause)
{
	Player_Pause(m_nPort, nPause);
}

void CFormPlayCtrl::OnSliderMove(int value)
{	
	if (m_bDubbing&&m_audioCtrl != NULL)
	{		
		DWORD dAudioTotalTime = m_audioCtrl->GetFileTime();
		DWORD dVideoTotalTime = this->GetFileTime();
		int curTime = value* dVideoTotalTime / 100.0;
		//int dTimeDif = abs((int)dVideoTotalTime - (int)dAudioTotalTime);
		if (curTime <dAudioTotalTime)//如果当前拖动的时间比配音时间大，配音文件不用拖动,但暂停播放；否则拖动
		{
			m_audioCtrl->PauseDubbingAudio(0);
			int audioValue = curTime * 100 / (float)dAudioTotalTime;
			m_audioCtrl->OnSliderMove(audioValue);

		}
		else
		{
			m_audioCtrl->PauseDubbingAudio(1);
		}
		
	}

	float fPos = value / 100.0;
	if (m_playMode == SINGLE_MODE)
	{
	
		Player_SetPlayPos(m_nPort, fPos);
	}
	else if (m_playMode == TIME_MODE)
	{
		float fRealPos = m_fStartPos + fPos*(m_fClosePos - m_fStartPos);
		Player_SetPlayPos(m_nPort, fRealPos);
	}
	if (m_sliderTimer->isActive())
	{
		m_sliderTimer->stop();
	}
	QThread::msleep(1);
	m_sliderTimer->start(1000);
}

void CFormPlayCtrl::OnUpdateSlider()
{
	char buf[100];
	if (m_playMode == SINGLE_MODE)
	{
		float fPos = Player_GetPlayPos(m_nPort);
	//	m_sldBar.SetPos(fPos * 100);
		ui.SliderPlaytime->setValue(fPos * 100);

		DWORD dTotalTime = Player_GetFileTime(m_nPort);
		if (dTotalTime != 0)
		{
			ui.SliderPlaytime->setEnabled(true);
		}
		int hour = dTotalTime / 3600;
		int min = (dTotalTime - hour * 3600) / 60;
		int sec = dTotalTime - hour * 3600 - min * 60;
		sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
		QString strTime = QString("%1").arg(buf);
		ui.labelEndTime->setText(strTime);

		//取绝对时间
		/*
		unsigned long long llSystemTime = 0;
		if (Player_GetSystemTime(m_nPort, &llSystemTime))//暂不支持自动校准
		{
			m_pPlayWnd->setCorrectTime(llSystemTime);
		}*/

		//取相对时间
		DWORD dCurrentTime = Player_GetPlayedTime(m_nPort);
		if (dCurrentTime > 24 * 3600)
		{
			return;
		}		
		if (m_bManulClock)
		{
			m_pPlayWnd->setCorrectTime(m_ManulTime.toTime_t() + dCurrentTime);
		}
		hour = dCurrentTime / 3600;
		min = (dCurrentTime - hour * 3600) / 60;
		sec = dCurrentTime - hour * 3600 - min * 60;
		
		sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
		strTime = QString("%1").arg(buf);
		ui.labelStartTime->setText(strTime);

		if (!m_bSetFileEndBck)//设置文件结束回调不成功，需要自己比对时间或进度条结束。
		{
			if (dCurrentTime >= dTotalTime || fPos * 100 >= 99)
			{
				Sleep(2000);
				FileEndCallback(m_index, this);
			}
		}
	}
	else if (m_playMode == TIME_MODE)
	{
		float fPos = Player_GetPlayPos(m_nPort);
		float fRealPos = (fPos - m_fStartPos) / (m_fClosePos - m_fStartPos);
		//	m_sldBar.SetPos(fRealPos * 100);
		ui.SliderPlaytime->setValue(fRealPos * 100);

		DWORD dTotalTime = Player_GetFileTime(m_nPort);

		if (!m_bDIVEventFinish&&m_index == 0 && dTotalTime > 0)
		{
			if (dTotalTime >= MIN_DIVPLAYTIME)
			{
				QApplication::postEvent(this->parent(), new CDivEvent((QEvent::Type)CUSTOM_DIV_EVENT, m_strCurPlayFileName));
			}
			else
			{
				QApplication::postEvent(this->parent(), new QEvent((QEvent::Type)CUSTOM_NOTDIV_EVENT));
			}
			m_bDIVEventFinish = true;
		}
		
		int endTime = dTotalTime / DIV_NUM;
		int hour = endTime / 3600;
		int min = (endTime - hour * 3600) / 60;
		int sec = endTime - hour * 3600 - min * 60;
		sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
		QString strTime = QString("%1").arg(buf);
		ui.labelEndTime->setText(strTime);

		DWORD dCurrentTime = Player_GetPlayedTime(m_nPort);
		if (!m_bGetStartTime&&dCurrentTime != 0)
		{
			if (dCurrentTime < 24 * 3600)
			{
				m_dStartTime = dCurrentTime;
				m_bGetStartTime = true;
			}
			//	char buf[1024];
			//	::sprintf_s(buf, "1111111111111；m_index:%d ,m_dStartTime:%d \n ", m_index, m_dStartTime);
			//	USES_CONVERSION;
			//	OutputDebugString(A2T(buf));
		}
		if (!m_bGetStartTime)
		{
			return;
		}

		if (m_bManulClock)
		{
			m_pPlayWnd->setCorrectTime(m_ManulTime.toTime_t() + dCurrentTime);
		}
		///	char buf[1024];
		//	::sprintf_s(buf, "22222222222222:m_index:%d dCurrentTime:%d,m_dStartTime:%d  \n", m_index, dCurrentTime,m_dStartTime);
		//	USES_CONVERSION;
		//	OutputDebugString(A2T(buf));
		int timeDif = 0;
		if (dCurrentTime >= m_dStartTime)
		{
			timeDif = dCurrentTime - m_dStartTime;
			//char buf[1024];
			//::sprintf_s(buf, "m_index:%d m_dStartTime:%d  \n", m_index, m_dStartTime);
			//USES_CONVERSION;
			//OutputDebugString(A2T(buf));
		}



		//	CTimeSpan span(dCurrentTime - ((float)m_index*(float)dTotalTime / (float)NUM));
		//CTimeSpan span(dCurrentTime);
		//CString strTime = span.Format("%H:%M:%S");
		//	m_stCurTime.SetWindowText(strTime);
		//int timeDif = dCurrentTime - ((float)m_index*(float)dTotalTime / (float)NUM);
		hour = timeDif / 3600;
		min = (timeDif - hour * 3600) / 60;
		sec = timeDif - hour * 3600 - min * 60;
		sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
		strTime = QString("%1").arg(buf);
		ui.labelStartTime->setText(strTime);

		if (dTotalTime == 0)
		{
			return;
		}
		else
		{
			ui.SliderPlaytime->setEnabled(true);
		}
		if (timeDif >= endTime)
		{
			QApplication::postEvent(this->parent(), new CMyEvent(CUSTOM_AutoStop_EVENT, this));
		}
		
	}
	
}

void  CFormPlayCtrl::SetPlayMode(PLAY_MODE eMode)
{
	m_playMode = eMode;
}

void CFormPlayCtrl::OnBtnSoundClick()
{
	m_bSoundOn = !m_bSoundOn;

	if (m_bDubbing&&m_audioCtrl != NULL)
	{
		m_audioCtrl->OnBtnSoundClick();
	}
	if (m_bSoundOn)
	{
		Player_PlaySoundShare(m_nPort);
		int volume=Player_GetVolume(m_nPort);
		ui.SliderVolume->setValue(volume);
		ui.SliderVolume->setEnabled(true);
		ui.btnSound->setStyleSheet("QPushButton{border-image: url(:/Skin/sound.png)0 0 0 0 repeat}QPushButton:hover{border-image: url(:/Skin/sound.png)28 0 0 0 repeat}QPushButton:pressed{border-image: url(:/Skin/sound.png)56 0 0 0 repeat}QPushButton:disabled{border-image: url(: / Skin/sound.png)84 0 0 0 repeat}");
	}
	else
	{
		Player_StopSoundShare(m_nPort);
		ui.SliderVolume->setEnabled(false);
		ui.btnSound->setStyleSheet("QPushButton{border-image: url(:/Skin/soundOff.png)0 0 0 0 repeat}QPushButton:hover{border-image: url(:/Skin/soundOff.png)28 0 0 0 repeat}QPushButton:pressed{border-image: url(:/Skin/soundOff.png)56 0 0 0 repeat}QPushButton:disabled{border-image: url(: / Skin/soundOff.png)84 0 0 0 repeat}");
	}

}

void CFormPlayCtrl::SetPlaySliderPos(int valueDif)
{
	if (!this->IsPlaying())
	{
		return;
	}
	int value = ui.SliderPlaytime->value();
	value += valueDif;
	value = value > 100 ? 100 : value;
	value = value <0 ? 0 : value;
	ui.SliderPlaytime->setValue(value);
	OnSliderMove(value);
}

void CFormPlayCtrl::SetVolumeSliderPos(int valueDif)
{	
	if (!this->IsPlaying())
	{
		return;
	}
	int value = ui.SliderVolume->value();
	value += valueDif;
	value = value > 0xffff ? 0xffff : value;
	value = value <0 ? 0 : value;
	ui.SliderVolume->setValue(value);
	OnSliderVolumeMove(value);
}

void CFormPlayCtrl::OnSliderVolumePressed()
{
	int value = ui.SliderVolume->value();
	OnSliderVolumeMove(value);
}

void CFormPlayCtrl::OnSliderVolumeMove(int value)
{
	if (m_bSoundOn)
	{
		if (m_bDubbing&&m_audioCtrl != NULL)
		{
			m_audioCtrl->OnSliderVolumeMove(value);
		}

		Player_SetVolume(m_nPort, value);
	}
}

void CFormPlayCtrl::OnBtnStepBackClick()
{
	bool bRet=Player_OneByOneBack(m_nPort);
	if (bRet)
	{
		m_bStep = true;
		m_bPause = true;
		UpdatePauseState();
	}	
}

void CFormPlayCtrl::OnBtnStepNextClick()
{
	bool bRet = Player_OneByOne(m_nPort);
	if (bRet)
	{
		m_bStep = true;
		m_bPause = true;
		UpdatePauseState();
	}
}

void  CFormPlayCtrl::OnBtnClockClick()
{
	CDialogClock dlg;
	dlg.SetDateTime(m_ManulTime);
	if (QDialog::Accepted == dlg.exec())
	{
		m_bManulClock = true;
		m_ManulTime=dlg.GetDateTime();
	}
	else
	{
		m_bManulClock = false;
		m_pPlayWnd->setCorrectTime(0);
	}
}

/*

brightness -255~255
contrast   -100~100
saturation   -100~100
hue			-180~180

调亮度、对比度
Y' = ((Y - 16) x C) + B + 16 //((Y - 16) x C)这里其实只是调Y值，亮度，实际要调对比度应该是Y' = ((Y - 128) x C) + B + 128

调色度
U' = (U-128) x Cos(H) + (V-128) x Sin(H)
V' = (V-128) x Cos(H) - (U-128) x Sin(H)

调对比度、饱和度、色度
U'' = (((U-128) x Cos(H) + (V-128) x Sin(H)) x C x S) + 128
V'' = (((V-128) x Cos(H) - (U-128) x Sin(H)) x C x S) + 128
*/

void CFormPlayCtrl::build_Table(int brightness, int contrast, int saturation, int hue)
{//U'' = (((U-128) x Cos(H) + (V-128) x Sin(H)) x C x S) + 128
	//V'' = (((V-128) x Cos(H) - (U-128) x Sin(H)) x C x S) + 128
	float c = (100 + contrast) / 100.0f;
	float s = (100 + saturation) / 100.0f;
	for (int i = 0; i<256; i++)
	{
		m_y_Table[i] = (char)max(0, min(255, (int)((i - 128)* c + brightness + 128)));

		for (int j = 0; j<256; j++)
		{
			if (hue == 0)//色度为0，则默认不改变，不用cos,sin
			{
				m_uv_Table[i][j].u = (char)max(0, min(255, int((i - 128)*c*s + 128)));
				m_uv_Table[i][j].v = (char)max(0, min(255, int((j - 128)*c*s + 128)));

			}
			else
			{
				m_uv_Table[i][j].u = (char)max(0, min(255, int(((i - 128)*cos((float)hue) + (j - 128)*sin((float)hue))*c*s + 128)));
				m_uv_Table[i][j].v = (char)max(0, min(255, int((((j - 128)*cos((float)hue) - (i - 128)*sin((float)hue))*c*s + 128))));
			}
		}
	}
}

void CFormPlayCtrl::ImageProcess(unsigned char *pYUVData, int width, int height)
{
	if (!m_bSetDisplayCallback)//没有YUV回调，不支持图像处理
	{
		return;
	}
	if (CDialogMain::s_ImgProcessFun)
	{
		CDialogMain::s_ImgProcessFun(m_nImagePort, (char*)pYUVData,width,height,0);
	}
}

void CFormPlayCtrl::ProcessColor(unsigned char *pYUVData,  int width, int height)
{
	if (m_bGetColorSupport)
	{
		return;
	}
	if (m_iBrightness == 0 && m_iContrast == 0 && m_iSaturation == 0 && m_iHue == 0 )
	{
		return;
	}
	unsigned char *pY = pYUVData;
	unsigned char *pU = pYUVData + width*height;
	unsigned char *pV = pYUVData + width*height * 5 / 4;
	if (m_iBrightness != 0 || m_iContrast != 0)
	{
		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{
				*pY = m_y_Table[*pY];
				pY++;

				if ((m_iSaturation != 0 || m_iHue != 0) && (!(i % 2) && !(j % 2)))
				{
					unsigned char U = *pU;
					*pU = m_uv_Table[U][*pV].u;
					*pV = m_uv_Table[U][*pV].v;
					pU++;
					pV++;
				}
			}
		}
	}
	else if (m_iSaturation != 0 || m_iHue != 0)
	{
		for (int i = 0; i<height / 2; i++)
		{
			for (int j = 0; j<width / 2; j++)
			{
				unsigned char U = *pU;
				*pU = m_uv_Table[U][*pV].u;
				*pV = m_uv_Table[U][*pV].v;
				pU++;
				pV++;
			}
		}

	}
}

int CFormPlayCtrl::ScaleValue(int srcMin, int srcMax, int srcValue, int dstMin, int dstMax)
{
	int dstValue = 0;
	int srcRange = srcMax - srcMin;
	int dstRange = dstMax - dstMin;
	float scale = (float)srcValue / (float)srcRange;
	int midValue = srcRange / 2;
	if (srcValue == midValue+1)
	{
		return 0;
	}
	if (scale>0.5f)
	{
		dstValue = ((srcValue - midValue)*(float)dstRange / 2.0) / midValue;
	}
	else if (scale<0.5f)
	{
		dstValue = -((midValue - srcValue)*(float)dstRange / 2.0) / midValue;
	}
	else if (scale == 0.5f)//中间默认值都为0
	{

	}
	return dstValue;
}
void CFormPlayCtrl::OnShowColorDlg()
{
	int nBrightness, nContrast, nSaturation, nHue;
	bool bRet=Player_GetColor(m_nPort, 0, &nBrightness, &nContrast, &nSaturation, &nHue);
	m_bGetColorSupport = bRet;
	if (!m_bSetDisplayCallback&&!m_bGetColorSupport)//既没有获取YUV回调也没有SetColor函数，那不支持图像调节。
	{
		QMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("此视频不支持图像调节！"));
		return;	
	}

	if (!m_bGetColorSupport)//m_bSetDisplayCallback为true,通过回调来调节图像
	{	
		nBrightness = m_iUIBrightness;
		nContrast = m_iUIContrast;
		nSaturation = m_iUISaturation;
		nHue = m_iUIHue;
	}
	CDialogColorSet dlg;
	dlg.InitColor(this, nBrightness, nContrast, nSaturation, nHue);	
	if (m_bSetDisplayCallback)
	{
		dlg.InitImageProcessValue(this, m_bSharp, m_nSharpValue,m_bDecNoise,m_nDecNoiseValue);
	}
	m_colorSetDlg = &dlg;
	dlg.exec();
	m_colorSetDlg = NULL;
	
}
void CFormPlayCtrl::SetImageProcessFilter(bool bSharp, int sharpValue,bool bDecNoise ,int noiseValue)
{
	m_bSharp = bSharp;
	char filter_descr[256] = { 0 };
	m_nSharpValue = sharpValue;
	m_nDecNoiseValue = noiseValue;
	
	if (bSharp&&bDecNoise)
	{
		//sprintf(filter_descr, "unsharp=5:5:4.0:5:13:4.0;hqdn3d");
		sprintf(filter_descr, "unsharp=5:5:%0.1f:5:5:%0.1f,hqdn3d=%0.1f", m_nSharpValue / (float)10, m_nSharpValue / (float)10, m_nDecNoiseValue / (float)10);
	}
	else if (bSharp)
	{
		sprintf(filter_descr, "unsharp=5:5:%0.1f:5:5:%0.1f", m_nSharpValue / (float)10, m_nSharpValue / (float)10);
	}
	else if (bDecNoise)
	{
		sprintf(filter_descr, "hqdn3d=%0.1f:%0.1f:%0.1f:%0.1f", m_nDecNoiseValue / (float)10, 3.0*m_nDecNoiseValue / (4.0*(float)10), 6.0*m_nDecNoiseValue / (4.0*(float)10), 6.0*m_nDecNoiseValue / (4.0*(float)10));
	//	sprintf(filter_descr, "hqdn3d=%0.1f", m_nDecNoiseValue / (float)10);
		//sprintf(filter_descr, "hqdn3d");
		//	sprintf(filter_descr, "owdenoise=16:1000:1000");//太慢
	}
	//sprintf(filter_descr, "kerndeint = sharp = 1");
		
	

	if (CDialogMain::s_InitImgProcessFun)
	{
		int width = 0;
		int height = 0;
		Player_GetPictureSize(m_nPort,(long*) &width, (long*)&height);
		CDialogMain::s_InitImgProcessFun(m_nImagePort,filter_descr, width, height, 0);
	}

}

bool  CFormPlayCtrl::SetColor(int *pBrightness, int *pContrast, int *pSaturation, int *pHue)
{
	bool bRet = false;
	if (m_bGetColorSupport)
	{
		bRet=Player_SetColor(m_nPort, 0, *pBrightness, *pContrast, *pSaturation, *pHue);
	}
	else
	{
		m_iUIBrightness =*pBrightness;
		m_iUIContrast = *pContrast;
		m_iUISaturation = *pSaturation;
		m_iUIHue = *pHue;
		
		m_iBrightness=ScaleValue(COLOR_MIN, COLOR_MAX, *pBrightness, -255, 255);
		m_iContrast = ScaleValue(COLOR_MIN, COLOR_MAX, *pContrast, -100, 100);
		m_iSaturation = ScaleValue(COLOR_MIN, COLOR_MAX, *pSaturation, -100, 100);
		m_iHue = ScaleValue(COLOR_MIN, COLOR_MAX, *pHue, -180, 180);
		build_Table(m_iBrightness, m_iContrast, m_iSaturation, m_iHue);
		bRet = true;
	}
	return bRet;
}