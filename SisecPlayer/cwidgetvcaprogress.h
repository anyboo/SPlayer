#ifndef CWIDGETVCAPROGRESS_H
#define CWIDGETVCAPROGRESS_H

#include <QWidget>
#include "ui_cwidgetvcaprogress.h"
#include "cwidgetplaywnd.h"
#include "cformplayctrl.h"
#include "IVCAObject.h"
#include "SimpleVCA.h"
#include "Render\D3DCalibrationRender.h"
#include "VCA\Render\VCADataMgr.h"
#include "VCA\JpegCodec.h"

enum FileVCA_Head{
	No = 0,
	FileNameVCA,
	FileSizeVCA,
	FileWidthHeightVCA,
	FileDuationVCA,
	ProgressVCA,
	VCAResult,
	EventCount,
	VCATime,
	Column_CountVCA,
};

struct T_FileChecked
{
	int index;
	QString strFileName;
	bool bFinish;
};

enum T_PlayVCAState
{
	VCANoStart,
	VCAPlay,
	VCAPause,
	VCAStop,
	VCAFinish,
};

class CWidgetVCAProgress : public QWidget
{
	Q_OBJECT

public:
	CWidgetVCAProgress(CVCADataMgr	*pDataMgr,QWidget *parent = 0);
	~CWidgetVCAProgress();
public:
	virtual void customEvent(QEvent * event);
public:
	static void OnEventFilterCallBack(DISPLAYCALLBCK_INFO *pInfo);
	void EventFilterProcess(DISPLAYCALLBCK_INFO *pInfo);
	void StartFileListVCA(int curCasePoint, QList<T_FileChecked> PlayFileListVCA, int width, int height,  bool bZoneShow=true);
	bool IsVCAProgressing()
	{
		if (m_VCAState != VCAFinish && m_VCAState != VCAStop&& m_VCAState != VCANoStart)
		{
			return true;
		}
		return false;
	}
	void Reset();
signals:
	void UpdateEventCount();
	void GetCurPlayTime();
protected slots:
	void OnCheckRenderShowChanged(int state);
	void OnGetVCATime();
	void OnBtnStartClick();
	void OnBtnStopClick();
	void OnUpdateEventCount();
	void OnGetCurPlayTime();
private:
	void InsertFileListVCA();
	void ClearTableFileVCA();
	void StartNextFile();
private:
	Ui::CWidgetVCAProgress ui;

	QTimer *m_timerGetVCATime;
	int m_curTask;
	int m_totalEventCount;
	int m_curPointVCA;
	//QList<QString> *m_pFileListVCA;
	QList<T_FileChecked> m_FileCheckedList;
	
	QString m_strVCADataPath;
	QString m_strVCADataTimePath;
	CJpegCodec *m_pJpegCodec;
	QString m_strPicTime;

	//检索过程
	CWidgetPlayWnd *m_PlayWndVCA;
	CFormPlayCtrl *m_PlayCtrlVCA;
	CCalibrationD3DRender *m_RenderVCA;
	CSimpleVCA *m_VCAObject;
	VCA5_PACKET_OBJECTS m_PrePacketObjects;
	CVCADataMgr		*m_pDataMgr;

	int m_nWidth;
	int m_nHeight;

	int m_VCAState;
	bool m_bPause;

	QList<int> m_listAlarmID;//记录报警ID，重复报的要忽略掉
};

#endif // CWIDGETVCAPROGRESS_H
