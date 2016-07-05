#ifndef CWIDGETVCARESULT_H
#define CWIDGETVCARESULT_H
#include "stdafx.h"
#include <QWidget>
#include "ui_cwidgetvcaresult.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"
#include "Render\D3DCalibrationRender.h"
#include "VCA\JpegCodec.h"
#include "ImageView.h"
#include <QFileInfoList>
#include <QMutex>

struct T_VCAResultNode
{
	int casePointIndex;
	int fileIndex;
};
Q_DECLARE_METATYPE(T_VCAResultNode);

class CWidgetVCAResult : public QWidget
{
	Q_OBJECT

public:
	CWidgetVCAResult(QWidget *parent = 0);
	~CWidgetVCAResult();
public:
	virtual void customEvent(QEvent * event);
public:
	void UpdateTreeWidgetResult(bool bFocusItem = false, int iResult = -1, int jResult = -1);
	void OnNotifyResult(int iPointVCA, int indexFile);
	void Reset();	
	static bool DeleteDirectory(QString &strPath);
protected slots:
	void OnTreeWidgetItemDbClick(QTreeWidgetItem* pItem, int column);
	void OnListWidgetItemDbClick(QListWidgetItem* pItem);
	void OnListWidgetItemClick(QListWidgetItem* pItem);
	void OnCheckRuleStateChanged(int state);
	void OnBtnResultDelClick();
	void OnBtnResultDelAllClick();
	void OnBtnPreClick();
	void OnBtnNextClick();
	void OnImageRefreshTimer();
private:	
	void UpdateListImage();

private:
	Ui::CWidgetVCAResult ui;

	CWidgetPlayWnd *m_PlayWnd;
	CFormPlayCtrl *m_PlayCtrl;
	CCalibrationD3DRender *m_Render;
	CVCADataMgr*	m_pDataMgr;

	QTreeWidgetItem *m_preItem;
	CJpegCodec *m_pJpegCodec;

	int m_curPage;
	int m_totalPage;
	QFileInfoList m_fileinfoList;

	QMutex m_mutex;
	bool m_bColor;

	CImageView *m_imageView;
	QString m_imagePath;
	QString m_imagePathName;

	QTimer *m_timer;
};

#endif // CWIDGETVCARESULT_H
