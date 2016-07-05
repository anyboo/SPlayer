#include "cdialogcolorset.h"
#include "cformplayctrl.h"
CDialogColorSet::CDialogColorSet(QWidget *parent)
: NoFlameDlg(parent)
{
	ui.setupUi(this);

	ui.SliderBright->setRange(COLOR_MIN, COLOR_MAX);
	ui.SliderBright->setTickInterval(1);
	ui.SliderContrast->setRange(COLOR_MIN, COLOR_MAX);
	ui.SliderContrast->setTickInterval(1);
	ui.SliderSaturation->setRange(COLOR_MIN, COLOR_MAX);
	ui.SliderSaturation->setTickInterval(1);
	ui.SliderHue->setRange(COLOR_MIN, COLOR_MAX);
	ui.SliderHue->setTickInterval(1);

	ui.SliderSharp->setRange(10, 50);
	ui.SliderSharp->setTickInterval(1);

	ui.SliderDecNoise->setRange(10, 100);
	ui.SliderDecNoise->setTickInterval(1);

	ui.checkBoxSharp->setEnabled(false);
	ui.SliderSharp->setEnabled(false);
	ui.checkBoxDecNoise->setEnabled(false);
	ui.SliderDecNoise->setEnabled(false);

	connect(ui.BtnDefault, SIGNAL(clicked()), this, SLOT(OnBtnDefaultClicked()));
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.BtnClose, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.SliderBright, SIGNAL(sliderPressed()), this, SLOT(OnSliderBrightPressed()));
	connect(ui.SliderBright, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderBrightMove(int)));
	connect(ui.SliderSaturation, SIGNAL(sliderPressed()), this, SLOT(OnSliderSaturationPressed()));
	connect(ui.SliderSaturation, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderSaturationMove(int)));
	connect(ui.SliderContrast, SIGNAL(sliderPressed()), this, SLOT(OnSliderContrastPressed()));
	connect(ui.SliderContrast, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderContrastMove(int)));
	connect(ui.SliderHue, SIGNAL(sliderPressed()), this, SLOT(OnSliderHuePressed()));
	connect(ui.SliderHue, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderHueMove(int)));
	connect(ui.SliderSharp, SIGNAL(sliderPressed()), this, SLOT(OnSliderSharpPressed()));
	connect(ui.SliderSharp, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderSharpMove(int)));
	connect(ui.SliderDecNoise, SIGNAL(sliderPressed()), this, SLOT(OnSliderDecNoisePressed()));
	connect(ui.SliderDecNoise, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderDecNoiseMove(int)));
	connect(ui.checkBoxSharp, SIGNAL(stateChanged(int)), this, SLOT(OnCheckSharpStateChanged(int)));
	connect(ui.checkBoxDecNoise, SIGNAL(stateChanged(int)), this, SLOT(OnCheckDecNoiseStateChanged(int)));
}

CDialogColorSet::~CDialogColorSet()
{

}

void  CDialogColorSet::InitColor(CFormPlayCtrl *pPlayCtrl, int nBrightness, int nContrast, int nSaturation, int nHue)
{
	m_pPlayCtrl = pPlayCtrl;
	m_nBrightness = nBrightness;
	m_nContrast = nContrast;
	m_nSaturation = nSaturation;
	m_nHue = nHue;

	ui.SliderBright->setValue(m_nBrightness);
	ui.SliderContrast->setValue(m_nContrast);
	ui.SliderSaturation->setValue(m_nSaturation);
	ui.SliderHue->setValue(m_nHue);

}

void  CDialogColorSet::InitImageProcessValue(CFormPlayCtrl *pPlayCtrl, bool bCheck, int value,bool bDec, int decNoiseVaule)
{
	m_pPlayCtrl = pPlayCtrl;
	ui.checkBoxSharp->setEnabled(true);
	//ÉèÖÃ²»ÏìÓ¦
	disconnect(ui.checkBoxSharp, SIGNAL(stateChanged(int)), this, SLOT(OnCheckSharpStateChanged(int)));
	if (bCheck)
	{
		ui.checkBoxSharp->setChecked(true);
		ui.SliderSharp->setEnabled(true);
	}
	ui.SliderSharp->setValue(value);
	connect(ui.checkBoxSharp, SIGNAL(stateChanged(int)), this, SLOT(OnCheckSharpStateChanged(int)));

	//
	ui.checkBoxDecNoise->setEnabled(true);
	disconnect(ui.checkBoxDecNoise, SIGNAL(stateChanged(int)), this, SLOT(OnCheckDecNoiseStateChanged(int)));
	if (bCheck)
	{
		ui.checkBoxDecNoise->setChecked(true);
		ui.SliderDecNoise->setEnabled(true);
	}
	ui.SliderDecNoise->setValue(value);
	connect(ui.checkBoxDecNoise, SIGNAL(stateChanged(int)), this, SLOT(OnCheckDecNoiseStateChanged(int)));
}

void  CDialogColorSet::SetImageProcessFilter()
{
	bool bSharp = false;
	if (ui.checkBoxSharp->checkState() == Qt::Checked)
	{
		bSharp = true;
	}

	bool bDecNoise= false;
	if (ui.checkBoxDecNoise->checkState() == Qt::Checked)
	{
		bDecNoise = true;
	}

	int sharpValue = ui.SliderSharp->value();
	int decNoiseValue = ui.SliderDecNoise->value();
	((CFormPlayCtrl*)m_pPlayCtrl)->SetImageProcessFilter(bSharp, sharpValue, bDecNoise, decNoiseValue);
	
}

void  CDialogColorSet::OnCheckSharpStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		ui.SliderSharp->setEnabled(true);	
	}
	else
	{
		ui.SliderSharp->setEnabled(false);
	}
	SetImageProcessFilter();
}

void  CDialogColorSet::OnCheckDecNoiseStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		ui.SliderDecNoise->setEnabled(true);
	}
	else
	{
		ui.SliderDecNoise->setEnabled(false);
	}
	SetImageProcessFilter();
}

void CDialogColorSet::OnBtnDefaultClicked()
{
	InitColor(m_pPlayCtrl, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR);

	SendSetColorCMD();
}

void CDialogColorSet::SendSetColorCMD()
{
	((CFormPlayCtrl*)m_pPlayCtrl)->SetColor(&m_nBrightness, &m_nContrast, &m_nSaturation, &m_nHue);
}

void  CDialogColorSet::OnSliderSharpPressed()
{
	SetImageProcessFilter();
}

void  CDialogColorSet::OnSliderSharpMove(int value)
{
	SetImageProcessFilter();
}

void  CDialogColorSet::OnSliderDecNoisePressed()
{
	SetImageProcessFilter();
}

void  CDialogColorSet::OnSliderDecNoiseMove(int value)
{
	SetImageProcessFilter();
}

void CDialogColorSet::OnSliderBrightPressed()
{
	int value=ui.SliderBright->value();
	OnSliderBrightMove(value);
}

void CDialogColorSet::OnSliderBrightMove(int value)
{
	m_nBrightness = value;
	SendSetColorCMD();
}

void CDialogColorSet::OnSliderSaturationPressed()
{
	int value = ui.SliderSaturation->value();
	OnSliderSaturationMove(value);
}

void CDialogColorSet::OnSliderSaturationMove(int value)
{
	m_nSaturation = value;
	SendSetColorCMD();
}

void CDialogColorSet::OnSliderContrastPressed()
{
	int value = ui.SliderContrast->value();
	OnSliderContrastMove(value);
}

void CDialogColorSet::OnSliderContrastMove(int value)
{
	m_nContrast = value;
	SendSetColorCMD();
}

void CDialogColorSet::OnSliderHuePressed()
{
	int value = ui.SliderHue->value();
	OnSliderHueMove(value);
}

void CDialogColorSet::OnSliderHueMove(int value)
{
	m_nHue = value;
	SendSetColorCMD();
}

