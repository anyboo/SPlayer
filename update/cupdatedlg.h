#ifndef CUPDATEDLG_H
#define CUPDATEDLG_H

#include <QDialog>
#include "ui_cupdatedlg.h"
#include <qprogressdialog.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <qfile.h>
#include <qthread.h>
#include <qprocess.h>

class CProcessThread :public QThread
{

	Q_OBJECT

public:
	CProcessThread(){}
	~CProcessThread(){}
signals:
	void ResultString(QString str);
public:
	virtual void run()
	{
		QProcess *process = new QProcess();

	
		//QString strPath = QApplication::applicationDirPath() + "/" + "update.bat";
		QString strPath = QApplication::applicationDirPath();
		//QString strPath = "update.bat";
		process->setWorkingDirectory(strPath);
		process->start("update.bat");
		//process->start(strPath);
		//	QProcess::execute("update.bat");//这种方法会阻塞

		
		if (!process->waitForStarted())
		{
			emit ResultString(QStringLiteral("开始更新失败！"));
			return;
		}
		
		
		if (!process->waitForFinished())//有这句会卡住
		{
			emit ResultString(QStringLiteral("更新失败！"));
			return;
		}
	
		emit ResultString(QStringLiteral("更新完成！请重新运行程序！"));
	

	}
};

class CUpdateDlg : public QDialog
{
	Q_OBJECT

public:
	CUpdateDlg(QWidget *parent = 0);
	~CUpdateDlg();
public:
	void downloadFile(char *purl);
private:
	void startRequest(QUrl url);
	
private slots:
	
	void cancelDownload();
	void httpFinished();
	void httpReadyRead();
	void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
	//void slotAuthenticationRequired(QNetworkReply*, QAuthenticator *);
	void OnResultString(QString str);
private:
	Ui::CUpdateDlg ui;

	QProgressDialog*m_progressDialog;
	QNetworkAccessManager m_qnam;
	QNetworkReply *m_reply;
	QFile *m_file;
	int m_httpGetId;
	bool m_httpRequestAborted;

	CProcessThread *m_processThread;

};

#endif // CUPDATEDLG_H
