#ifndef CFORMPLAYCTRL_H
#define CFORMPLAYCTRL_H

#include <QWidget>
#include "ui_cformplayctrl.h"
#include <qtimer.h>
#include <qdatetime.h>
#include "ImageView.h"
#include "../PlayerFactory/PlayerFactory.h"
#pragma comment(lib,"../bin/PlayerFactory.lib")

#include "IVCAObject.h"


#define NUM (16)

enum PLAY_MODE{
	SINGLE_MODE = 0,
	TIME_MODE,
};
typedef struct T_UV_Value
{
	unsigned char u;
	unsigned char v;
};
enum RENDER_MODE
{
	RenderIni = 0,
	RenderStrech,
	Render4_3,
	Render16_9,
};
class CWidgetPlayWnd;
class CDialogColorSet;
class CCalibrationD3DRender;

class CFormPlayCtrl : public QWidget
{
	Q_OBJECT

public:
	CFormPlayCtrl(int index, CWidgetPlayWnd*pPlayWnd, QWidget *parent = 0);
	~CFormPlayCtrl();
public:	
//CVCA 接口

	void SetVCAObject(IVCAObject *pObject){ m_VCAObject = pObject; }
	void SetRender(CCalibrationD3DRender *pRender){ m_Render = pRender; }
	bool IsYUVCallback(){return m_bSetDisplayCallback;  }
	bool  SetDisplayCallback();
	void GoFastSpeed();
	float GetPlayPos() {
		return Player_GetPlayPos(m_nPort);
	}
	void SetPlayPos(float fPos) {
		if (!this->IsPlaying())
		{
			return;
		}
		int value = fPos*100;
		ui.SliderPlaytime->setValue(value);
		OnSliderMove(value);
	}
	bool SetPlayedTime(DWORD  nTime) {
		if (!this->IsPlaying())
		{
			return false;
		}
		DWORD dTotalTime = Player_GetFileTime(m_nPort);
		int value = nTime * 100 / dTotalTime;
		ui.SliderPlaytime->setValue(value);

		return Player_SetPlayedTimeEx(m_nPort, nTime);
	}
//配音相关
public:
	void SetAudioCtrl(CFormPlayCtrl*pAudioCtrl){ m_audioCtrl = pAudioCtrl; }
	void SetAudioAttrible(){ m_bAudioAtrrible = true; }
	void OpenAudioFile(QString strAudioFile);
	void PauseDubbingAudio(DWORD nPause);
	bool m_bAudioAtrrible;
public:
	bool  OpenAndPlayFile(QString strFileName, HWND hwnd, bool bSpecify=false, int FileType=-1);
	QString GetCurPlayFileName(){ return m_strCurPlayFileName; }
	DWORD GetFileTime(){ return Player_GetFileTime(m_nPort); }
	DWORD GetCurPlayTime(){ return Player_GetPlayedTime(m_nPort); }
	void  SetRenderMode(int renderMode = RenderIni);
	bool  GetPicSize(long *pWidth,long *pHeigth);
	bool  SetColor(int *pBrightness, int *pContrast, int *pSaturation, int *pHue);
	bool  CapturePic(QString fileName, int iType);
	void SetPlaySliderPos(int valueDif);//按键调节进度
	void SetVolumeSliderPos(int valueDif);
	void Pause(int nPause);
	void  Stop();
	void  SetPlayMode(PLAY_MODE eMode);
	int  Index(){ return m_index; }
	bool IsPlaying(){ return m_bPlay; }
	void HideNotUsedButton();
	void StopAudio(){
		Player_StopSoundShare(m_nPort);
	}
	void build_Table(int brightness, int contrast, int saturation, int hue);
	int ScaleValue(int srcMin, int srcMax, int srcValue, int dstMin, int dstMax);
	void ProcessColor(unsigned char *pYUVData,int width, int height);
	void ImageProcess(unsigned char *pYUVData, int width, int height);
	void SetImageProcessFilter(bool bSharp, int SharpValue, bool bDecNoise, int noiseValue);
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
	void OnBtnClockClick();
	void OnShowColorDlg();
private:
	static void __stdcall FileEndCallback(long nID, void *pUser);
	static void __stdcall DisplayCallback(DISPLAYCALLBCK_INFO *pstDisplayInfo);
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
	QString m_strCurPlayFileName;
	long m_nPort;
	int m_nImagePort;//图像处理接口
	bool m_bSharp;//锐化是否启动了
	int m_nSharpValue;
	bool m_bDecNoise;//降噪是否启动了
	int m_nDecNoiseValue;
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
	bool m_bSetFileEndBck;

	bool m_bManulClock;
	QDateTime m_ManulTime;

	bool m_bGetColorSupport;
	bool m_bSetDisplayCallback;
	CDialogColorSet *m_colorSetDlg;
	unsigned char m_y_Table[256];
	T_UV_Value m_uv_Table[256][256];

	int m_iUIBrightness, m_iBrightness;
	int m_iUIContrast, m_iContrast;
	int m_iUISaturation, m_iSaturation;
	int m_iUIHue, m_iHue;
public:
	IVCAObject *m_VCAObject;
	CCalibrationD3DRender *m_Render;
	CImageView *m_pView;
};

#endif // CFORMPLAYCTRL_H
