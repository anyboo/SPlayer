#ifndef CDIALOGMAIN_H
#define CDIALOGMAIN_H

#include "NoFlameDlg.h"
#include "ui_cdialogmain.h"
#include "cformplayctrl.h"
#include"cwidgetplaywnd.h"
#include <QTreeWidgetItem>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <qthread.h>
#include "cdialogdubbing.h"
#include <qmessagebox.h>

enum PLAYLIST_MODE{
	SINGLE = 0,
	LIST,
	SINGLE_REPEAT,
	LIST_REPEAT,
};
enum RENDER_MODE
{
	RenderIni=0,
	RenderStrech,
	Render4_3,
	Render16_9,
};

enum WND_MODE
{
	WND1 = 0,
	WND4,
	WND9,
	WND16,
};
enum CapturePicType
{
	BMP=0,
	JPG,
};

class UkeyThread :public QThread
{
	Q_OBJECT
public:
	UkeyThread();
	~UkeyThread();
signals:
	void ukeyUp();
	void ukeyDown();
public:
	//void SetOwner(QDialog *pOwner){ m_pOwner = pOwner; }
	bool VerifyUkey();
	void Stop();
private:
	bool m_stopFlag;
//	QDialog *m_pOwner;
protected:
	void run();

};

class CDialogMain : public NoFlameDlg
{
	Q_OBJECT

public:
	CDialogMain(QWidget *parent = 0);
	~CDialogMain();
	virtual void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *e);
/*	virtual void keyPressEvent(QKeyEvent * event);
	void keyReleaseEvent(QKeyEvent * keyEvent);*/
	void dropEvent(QDropEvent * event);
	void dragEnterEvent(QDragEnterEvent *event);
/*	void dragMoveEvent(QDragMoveEvent *event);*/
public:
	static bool VerifyUkey();
	void ShellExe(QString &strFile);
	void ShellExe2();
	bool IsPlayCtrlVisible(int x,int y);
	void SetPlayCtrlVisible(bool bShow);
protected:
	bool  OpenAndPlayFile(CFormPlayCtrl *pPlayCtrl,QString strFileName);
	void  StopAll();
	void SetPlayMode(PLAY_MODE eMode);
	void ReadPlayFileList();
	void WritePlayFileList();
	void SetPlayCtrlPos();
	void SetBtnHideListPos();
	void AddToPlayListWidget(QString strPathName);
	void SetCurPlayListIndex(QString strPathName);
	void SetFullScreen(QWidget *pWnd);
	void PlayIndex(CFormPlayCtrl *pPlayCtrl,bool bNext, bool bListRepeat = false);
	void SetRenderMode(int index);
	void IniSysMenu();
	void IniWndMenu();
	void IniPlayListMenu();
	void PaintWndRect(QPainter &painter, int num);
	void SetWndGrid(int &top, int &left, int &twidth, int &theight, int num,int grid);
	bool CheckPicPath(QString picPath);
public slots:
	void OnBtnCloseClick();
protected slots:
	void OnBtnFullScreenClick();
	void OnBtnMaxClick();
	void OnBtnMinClick();
	void OnBtnMoreClick();
	void OnBtnDiv4TimeClick();
	void OnBtnFileOpenClick();
	void OnBtnFileAddClick();
	void OnBtnFileDelClick();
	void OnBtnFileDelAllClick();
	void OnActivated(int index);
	void OnBtnHideListClick();
	void OnListWidgetItemDbClick(QListWidgetItem * item);
	void OnPlayModeTriggered(QAction*act);
	void OnRenderModeTriggered(QAction*act);
	void OnWndModeTriggered(QAction*act);
	void OnConfigTriggered(); 
	void OnConvertTriggered(); 
	void OnCutTriggered();
	void OnUpdateTriggered();
	void OnHelpTriggered();
	void OnUkeyDown();
	void OnUkeyUp();
	void OnTerminated();
private:
	CFormPlayCtrl *m_PlayCtrl[NUM];
	CWidgetPlayWnd *m_PlayWnd[NUM];
	CFormPlayCtrl *m_AudioDubCtrl;
	CDialogDubbing m_DubDlg;
	PLAY_MODE m_PlayMode;
	int m_iCurFocus;
//	bool m_bPlay;
//	QString m_strCurPlayFileName;
	bool m_bHide;
//	QRect m_rcFrameSystemMenu;
	QRect m_rcFrameSysBar;
	QRect m_rcFrameFileList;
	QRect m_rcFrameToolBar;
	QRect m_rcFileList;

	QRect m_rcComboxFileOpen;

	int m_iScreenWidth;
	int m_iScreenHeigth;

	QList<QString> m_strPlayFileList;
	bool m_bFullScreen;
	QRect m_rcPlayWndNormal;
	QRect m_rcPlayCtrlNormal;

	QString m_picPath;
	int m_picType;
	int m_picRepeatCount;

private:
	Ui::CDialogMain ui;
	QAction *m_singlePlayAct;
	QAction *m_listPlayAct;
	QAction *m_singleRepeatAct;
	QAction *m_listRepeatAct;
	QActionGroup *m_ActGroup;
	int m_playListMode;


	QAction *m_configAct;
	QAction *m_converterAct;
	QAction *m_cutAct;
	QAction *m_updateAct;
	QAction *m_helpAct;
	QAction *m_renderIniAct;
	QAction *m_rendeStrechAct;
	QAction *m_render4_3Act;
	QAction *m_rendeStrech16_9Act;
	QActionGroup *m_renderActGroup;
	int m_renderMode;

	QAction *m_Wnd1Act;
	QAction *m_Wnd4Act;
	QAction *m_Wnd9Act;
	QAction *m_Wnd16Act;
	QActionGroup *m_WndActGroup;
	int m_WndMode;
	int m_curWndNum;

	static UkeyThread s_ukeyThread;
	QMessageBox m_ukeyDownMsgdlg;
	QDialog *m_pConvertDlg;//指向转码对话框，如果ukey断开后要立即停止转码
	QDialog *m_pCutDlg;//指向剪切对话框，如果ukey断开后要立即停止转码
};

#endif // CDIALOGMAIN_H
