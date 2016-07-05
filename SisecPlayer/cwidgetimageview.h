#ifndef CWIDGETIMAGEVIEW_H
#define CWIDGETIMAGEVIEW_H
#include "stdafx.h"
#include <QWidget>
#include "ui_cwidgetimageview.h"
#include "Render\D3DCalibrationRender.h"
#include "VCA\JpegCodec.h"
#include <qpainter.h>

class CWidgetImageView : public QWidget
{
	Q_OBJECT

public:
	CWidgetImageView(QWidget *parent = 0);
	~CWidgetImageView();
	virtual void paintEvent(QPaintEvent *ev);
public:
	void ShowImage(QString strImagePath);
	void Reset();
private:
	Ui::CWidgetImageView ui;

	CJpegCodec *m_pJpegCodec;
	Jpeg m_jpeg;
	CCalibrationD3DRender *m_Render;
	CVCADataMgr*	m_pDataMgr;

	BYTE		*m_pBuffer;
	DWORD		m_dwBufferLength;

	bool m_bShow;
	//BYTE*	m_pImageBuf;
};

#endif // CWIDGETIMAGEVIEW_H
