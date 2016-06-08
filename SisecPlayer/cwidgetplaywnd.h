#ifndef CWIDGETPLAYWND_H
#define CWIDGETPLAYWND_H

#include <QWidget>
#include "ui_cwidgetplaywnd.h"
#include <qpainter.h>
#include <time.h>

enum COUSTOM_EVENT{
	CUSTOM_AutoStop_EVENT= 5000 + 1,//文件播放完自动回调
	CUSTOM_ListItemDbClick_EVENT,//双击播放列表事件，使用postEvent,防止正在分段分屏时又双击播放，可能会卡死
	CUSTOM_ManualStop_EVENT,//手动停止回放
	CUSTOM_PLAYWNDPRESSED_EVENT,//播放窗口单点击事件
	CUSTOM_PLAYWNDDBCLICK_EVENT,//播放窗口双击事件
	CUSTOM_PIC_EVENT,//抓图
	CUSTOM_PICREPEAT_EVENT,//连拍
	CUSTOM_NEXT_EVENT,//播放窗口双击事件
	CUSTOM_PREVIOUS_EVENT,//播放窗口双击事件
	CUSTOM_DIV_EVENT,//时间长度可分屏
	CUSTOM_NOTDIV_EVENT,//时间长度太短
	CUSTOM__SETSPEED_EVENT,//设置速率事件，通知更新
};

class CMyEvent :public QEvent
{
public:
	CMyEvent(int type, QObject *pObj) :QEvent((QEvent::Type)type), obj(pObj){}
	~CMyEvent(){}

	QObject *obj;
};

class CDivEvent :public QEvent
{
public:
	CDivEvent(int type, QString strFile) :QEvent((QEvent::Type)type), strFileName(strFile){}
	~CDivEvent(){}

	QString strFileName;
};

class CWidgetPlayWnd : public QWidget
{
	Q_OBJECT

public:
	CWidgetPlayWnd(int index,QWidget *parent = 0);
	~CWidgetPlayWnd();
signals:
	void ShowColorDlg();
public:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void leaveEvent(QEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void wheelEvent(QWheelEvent * event);
public:
	void setCorrectTime(unsigned long long timeStamp)
	{
		if (timeStamp == 0)
		{
			ui.labelCorrectTime->setText("");
			return;
		}
		struct tm tTime = { 0 };
		localtime_s(&tTime, (const time_t*)&(timeStamp));
		/*或下面也对
		time_t tt = timeStamp;
		struct tm *tTime2 = localtime(&tt);*/
		char strTime[20] = { 0 };
		sprintf(strTime, "%d-%02d-%02d %02d:%02d:%02d", tTime.tm_year + 1900, tTime.tm_mon + 1, tTime.tm_mday, tTime.tm_hour, tTime.tm_min, tTime.tm_sec);
	//	sprintf(desFileName, "%d", timeStamp / 1000);
		ui.labelCorrectTime->setText(QString(strTime));
		ui.labelCorrectTime->setToolTip(QString(strTime));
	}
	void SetPreViewMode(){ 
		m_previewMode = true; 
		ui.BtnFix->hide(); 
		ui.BtnPic->hide();
		ui.BtnPicRepeat->hide();
		ui.BtnFullScreen->hide();
		ui.BtnColor->hide();
	}
	void Refresh(){
		this->repaint();
		ui.widgetRender->repaint();
	}
	int Index(){ return m_index; }
	int GetPlayWndID(){ return ui.widgetRender->winId(); }
	void SetSpeedString(QString strSpeed){ ui.labelSpeed->setText(strSpeed); }
	void SetPauseString(QString strPause){ ui.labelPause->setText(strPause); }
	void SetTitleFileName(QString strFile){ 
		strFile.replace("\\", "/");
		int index = strFile.lastIndexOf("/");
		QString strName = strFile.mid(index + 1);
		ui.labelFileName->setText(strName); ui.labelFileName->setToolTip(strName);
	}
	void SetPlayWndPos(QRect &rc){
		m_listScaleRC.clear(); 
		ui.widgetRender->setGeometry(rc);
	}
	void SetShowEnabled(bool bShowEnabled){
		m_bEnabled = bShowEnabled;
		if (bShowEnabled)
		{
			if (m_bFix)
			{
				ui.widgetToolBar->show();
			}
		}
		else 
		{		
		/*	if (m_bFix)
			{
				OnBtnFixClick();
			}*/
			ui.widgetToolBar->hide();
			m_listScaleRC.clear();
			ui.labelCorrectTime->setText("");
		}
	}

protected slots:
	void OnBtnFixClick();	
	void OnBtnColorClick();
	void OnBtnPicClick();
	void OnBtnPicRepeatClick();
	void OnBtnFullScreenClick();
private:
	Ui::CWidgetPlayWnd ui;
	int m_index;
	bool m_bEnabled;
	bool m_bFix;//固定工具条
	QList<QRect> m_listScaleRC;
	bool m_previewMode;//预览模式
};

#endif // CWIDGETPLAYWND_H
