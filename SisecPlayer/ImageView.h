#pragma once
#include "stdafx.h"
#include "Render\D3DCalibrationRender.h"
#include "VCA\JpegCodec.h"
#include "../../PlayerFactory/struct_def.h"
#include <QWidget>
class CImageView
{
public:
	CImageView(QWidget *pWidget);
	~CImageView();
public:
	void ShowImage(QString strImagePath);
private:
	CJpegCodec *m_pJpegCodec;
	CCalibrationD3DRender *m_Render;
	CVCADataMgr*	m_pDataMgr;

	BYTE		*m_pBuffer;
	DWORD		m_dwBufferLength;
	//BYTE*	m_pImageBuf;
};

