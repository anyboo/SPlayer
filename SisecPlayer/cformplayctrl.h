#ifndef CFORMPLAYCTRL_H
#define CFORMPLAYCTRL_H

#include <QWidget>
#include "ui_cformplayctrl.h"
#include <qtimer.h>

#include "../PlayerFactory/PlayerFactory.h"
#pragma comment(lib,"../bin/PlayerFactory.lib")

#define NUM (16)

enum PLAY_MODE{
	SINGLE_MODE = 0,
	TIME_MODE,
};
class CWidgetPlayWnd;

class CFormPlayCtrl : public QWidget
{
	Q_OBJECT

public:
	CFormPlayCtrl(int index, CWidgetPlayWnd*pPlayWnd, QWidget *parent = 0);
	~CFormPlayCtrl();
	//void customEvent(QEvent *e
	//≈‰“Ùœ‡πÿ
public:
	void SetAudioCtrl(CFormPlayCtrl*pAudioCtrl){ m_audioCtrl = pAudioCtrl; }
	void SetAudioAttrible(){ m_bAudioAtrrible = true; }
	void OpenAudioFile(QString strAudioFile);
	void PauseDubbingAudio(DWORD nPause);
	bool m_bAudioAtrrible;
public:
	bool  OpenAndPlayFile(QString strFileName, HWND hwnd);
	QString GetCurPlayFileName(){ return m_strCurPlayFileName; }
	DWORD GetFileTime(){ return Player_GetFileTime(m_nPort); }
	DWORD GetCurPlayTime(){ return Player_GetPlayedTime(m_nPort); }
	void  SetRenderMode();
	bool  GetPicSize(long *pWidth,long *pHeigth);
	bool  CapturePic(QString fileName, int iType);
	void  Stop();
	void  SetPlayMode(PLAY_MODE eMode);
	int  Index(){ return m_index; }
	bool IsPlaying(){ return m_bPlay; }
	void HideNotUsedButton();
	void StopAudio(){
		Player_StopSoundShare(m_nPort);
	}
public slots:
	void OnBtnPreviousClick();
	void OnBtnNextClick();
	void OnBtnStepBackClick();
	void OnBtnStepNextClick();
	void OnBtnSlowClick();
	void OnBtnPauseClick();
	void OnBtnStopClick();
	void OnBtnFastClick();
	void OnSliderPressed();
	void OnSliderMove(int value);
	void OnUpdateSlider();
	void OnBtnSoundClick();
	void OnSliderVolumePressed();
	void OnSliderVolumeMove(int value);
private:
	static void __stdcall FileEndCallback(long nID, void *pUser);
	void SetSpeed();
	void Init();
	void SetBtnCtrlEnabled(bool bEnabled);
	void UpdatePauseState();
private:
	Ui::CFormPlayCtrl ui;
	CFormPlayCtrl *m_audioCtrl;
	bool m_bDubbing;
	
	CWidgetPlayWnd *m_pPlayWnd;
	QTimer *m_sliderTimer;
	HWND m_playWnd;
	QString m_strCurPlayFileName;
	long m_nPort;
	bool m_bPause;
	bool m_bStep;
	bool m_bPlay;
	int m_iSpeed;
	bool m_bSoundOn;

	int m_index;
	PLAY_MODE m_playMode;
	float m_fStartPos;
	float m_fClosePos;
	unsigned int m_dStartTime;
	bool m_bGetStartTime;

	bool m_bDIVEventFinish;
};

#endif // CFORMPLAYCTRL_H
