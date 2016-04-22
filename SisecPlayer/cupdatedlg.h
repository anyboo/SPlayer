#ifndef CUPDATEDLG_H
#define CUPDATEDLG_H

#include <QDialog>
#include "ui_cupdatedlg.h"
#include <qprogressdialog.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <qfile.h>

class CUpdateDlg : public QDialog
{
	Q_OBJECT

public:
	CUpdateDlg(QWidget *parent = 0);
	~CUpdateDlg();
public:
	void CheckNewVersion();
	static bool ReadXml(QString xmlName, QString &strXmlPath, QString &strPacketPath, QString &strVersion);
private:
	void startRequest(QUrl url);
	void CompareVesion();
	void downloadFile(QUrl url);
private slots:
	void cancelDownload();
	void httpFinished();
	void httpReadyRead();
	void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
	//void slotAuthenticationRequired(QNetworkReply*, QAuthenticator *);
private:
	Ui::CUpdateDlg ui;

	QProgressDialog*m_progressDialog;
	QNetworkAccessManager m_qnam;
	QNetworkReply *m_reply;
	QFile *m_file;
	int m_httpGetId;
	bool m_httpRequestAborted;

	QString m_strCurVersion;
	QString m_strNewXmlName;
};

#endif // CUPDATEDLG_H
