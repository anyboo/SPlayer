#ifndef CDIALOGCOLORSET_H
#define CDIALOGCOLORSET_H

#include <QDialog>
#include "ui_cdialogcolorset.h"
#include "NoFlameDlg.h"

class CFormPlayCtrl;

#define COLOR_MIN (0)
#define COLOR_MAX (127)
#define DEFAULT_COLOR (64)
class CDialogColorSet : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogColorSet(QWidget *parent = 0);
	~CDialogColorSet();
public:
	void InitColor(CFormPlayCtrl *pPlayCtrl, int nBrightness, int nContrast, int nSaturation, int nHue);
	void InitImageProcessValue(CFormPlayCtrl *pPlayCtrl, bool bCheck, int value, bool bDec, int decNoiseVaule);
protected slots:
	 void OnBtnDefaultClicked();

	void OnSliderBrightPressed();
	void OnSliderBrightMove(int value);
	void OnSliderSaturationPressed();
	void OnSliderSaturationMove(int value);
	void OnSliderContrastPressed();
	void OnSliderContrastMove(int value);
	void OnSliderHuePressed();
	void OnSliderHueMove(int value);
	void OnSliderSharpPressed();
	void OnSliderSharpMove(int value);
	void OnSliderDecNoisePressed();
	void OnSliderDecNoiseMove(int value);
	void OnCheckSharpStateChanged(int state);
	void OnCheckDecNoiseStateChanged(int state);
private:
	void SendSetColorCMD();
	void  SetImageProcessFilter();
private:
	Ui::CDialogColorSet ui;

	int m_nBrightness;
	int m_nContrast;
	int m_nSaturation;
	int m_nHue;

	CFormPlayCtrl *m_pPlayCtrl;
};
 
#endif // CDIALOGCOLORSET_H

