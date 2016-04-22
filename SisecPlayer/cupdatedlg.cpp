#include "cupdatedlg.h"
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <QDomComment>
#include <QTextStream>
#include "singleapllication.h"
#include <qprocess.h>
#include "cdialogmain.h"

CUpdateDlg::CUpdateDlg(QWidget *parent)
: QDialog(parent),m_reply(0)
{
	ui.setupUi(this);

	m_progressDialog = new QProgressDialog(this);

	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

	ui.BtnOK->hide();
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(accept()));
}

CUpdateDlg::~CUpdateDlg()
{
	delete m_progressDialog;
	if (m_reply)
	{
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = 0;
	}

}

void  CUpdateDlg::CheckNewVersion()
{
	QString strXmlUrl, strPacketUrl;
	QString strXmlPath = QApplication::applicationDirPath() + "/" + "update.xml";
	if (ReadXml(strXmlPath, strXmlUrl, strPacketUrl, m_strCurVersion))
	{
		downloadFile(QUrl(strXmlUrl));
	}
}

bool  CUpdateDlg::ReadXml(QString xmlName,QString &strXmlUrl,QString &strPacketUrl,QString &strVersion)
{
	QString strXml = xmlName;
	QFile file(strXml);
	QDomDocument doc;
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	if (!doc.setContent(&file)) 
	{
		file.close();
		return false;
	}
	
	QDomElement root = doc.documentElement();
	if (root.isNull())
	{
		file.close();
		return false;
	}

	QDomElement  note = root.firstChildElement();
	QString strPathName = "";
	while (!note.isNull())
	{
		QString nodeName = note.nodeName();
		QString text = note.text();
		if (nodeName == "updatexmlurl")
		{
			strXmlUrl = text;
		}
		else if (nodeName == "version")
		{
			strVersion = text;
		}
		else if (nodeName == "updatepacketurl")
		{
			strPacketUrl = text;
		}	
		note = note.nextSiblingElement();
	}

	file.close();	
	return true;
}

void CUpdateDlg::downloadFile(QUrl url)
{
	QFileInfo fileInfo(url.path());
	QString fileName = QApplication::applicationDirPath() + "/" + fileInfo.fileName();
	m_strNewXmlName = fileName;


	m_file = new QFile(fileName);
	if (!m_file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, tr("HTTP"),
			tr("Unable to save the file %1: %2.")
			.arg(fileName).arg(m_file->errorString()));
		delete m_file;
		m_file = 0;
		return;
	}

	m_progressDialog->setWindowTitle(tr("HTTP"));
	m_progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));


	// schedule the request
	m_httpRequestAborted = false;
	startRequest(url);
}

void CUpdateDlg::startRequest(QUrl url)
{
	m_reply = m_qnam.get(QNetworkRequest(url));
	connect(m_reply, SIGNAL(finished()),
		this, SLOT(httpFinished()));
	connect(m_reply, SIGNAL(readyRead()),
		this, SLOT(httpReadyRead()));
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateDataReadProgress(qint64, qint64)));

}

void CUpdateDlg::cancelDownload()
{
	//m_statusLabel->setText(tr("Download canceled."));
	m_httpRequestAborted = true;
	m_reply->abort();
	//downloadButton->setEnabled(true);
}

void CUpdateDlg::httpFinished()
{
	if (m_httpRequestAborted) {
		if (m_file) {
			m_file->close();
			m_file->remove();
			delete m_file;
			m_file = 0;
		}
		m_reply->deleteLater();
		m_progressDialog->hide();
		return;
	}

	m_progressDialog->hide();
	m_file->flush();
	m_file->close();
	

	QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (m_reply->error()) {
		m_file->remove();
		ui.label->setText(QStringLiteral("检测失败: %1.")
			.arg(m_reply->errorString()));
		m_reply->deleteLater();
		m_reply = 0;
		delete m_file;
		m_file = 0;
		ui.BtnOK->show();
		//downloadButton->setEnabled(true);
	}
	else if (!redirectionTarget.isNull()) {
		/*QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		if (QMessageBox::question(this, tr("HTTP"),
			tr("Redirect to %1 ?").arg(newUrl.toString()),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			url = newUrl;
			m_reply->deleteLater();
			m_file->open(QIODevice::WriteOnly);
			m_file->resize(0);
			startRequest(url);
		}*/
	}
	else {
	//	QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
		//statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
		//downloadButton->setEnabled(true);m_reply->deleteLater();
		m_reply = 0;
		delete m_file;
		m_file = 0;
		ui.BtnOK->show();
		CompareVesion();
	}
	
}

void  CUpdateDlg::CompareVesion()
{
	QString strXmlUrl, strPacketUrl, strNewVersion;
	if (ReadXml(m_strNewXmlName, strXmlUrl, strPacketUrl, strNewVersion))
	{
		QString strCurTemp = m_strCurVersion;
		QString strNewTemp = strNewVersion;
		
		if (strCurTemp.length() <= 3)
		{
			strCurTemp.append('0');
		}
		if (strNewTemp.length() <= 3)
		{
			strNewTemp.append('0');
		}
		int iCurVersion = strCurTemp.remove('.').toInt();
		int iNewVersion = strNewTemp.remove('.').toInt();
		if (iCurVersion >= iNewVersion)
		{
			ui.label->setText(QStringLiteral("当前已经是最新版本!"));
		}
		else
		{
			if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
				QStringLiteral("检测到新版本%1，是否要下载更新？\n（点击\"yes\"会自动关闭当前播放器）").arg(strNewVersion), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
			{
				ui.label->setText(QStringLiteral("已取消下载更新！"));
				this->accept();
				
			}
			else
			{
				this->accept();
				QProcess *process = new QProcess();
				process->start("update.exe", QStringList() << strPacketUrl);
		
				
				CDialogMain *pMainW = (CDialogMain*)SingleApplication::instance()->mainWidget();
				pMainW->OnBtnCloseClick();
			}
			
		}

	}
	else
	{
		ui.label->setText(QStringLiteral("读取新版本失败!"));
	}
}

void CUpdateDlg::httpReadyRead()
{
	// this slot gets called every time the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply
	if (m_file)
		m_file->write(m_reply->readAll());
}

void CUpdateDlg::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	if (m_httpRequestAborted)
		return;

	m_progressDialog->setMaximum(totalBytes);
	m_progressDialog->setValue(bytesRead);
}


/*
void CUpdateDlg::slotAuthenticationRequired(QNetworkReply*, QAuthenticator *authenticator)
{
	QDialog dlg;
	Ui::Dialog ui;
	ui.setupUi(&dlg);
	dlg.adjustSize();
	ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(url.host()));

	// Did the URL have information? Fill the UI
	// This is only relevant if the URL-supplied credentials were wrong
	ui.userEdit->setText(url.userName());
	ui.passwordEdit->setText(url.password());

	if (dlg.exec() == QDialog::Accepted) {
		authenticator->setUser(ui.userEdit->text());
		authenticator->setPassword(ui.passwordEdit->text());
	}
}*/
/*
#ifndef QT_NO_SSL
void HttpWindow::sslErrors(QNetworkReply*, const QList<QSslError> &errors)
{
QString errorString;
foreach(const QSslError &error, errors) {
if (!errorString.isEmpty())
errorString += ", ";
errorString += error.errorString();
}

if (QMessageBox::warning(this, tr("HTTP"),
tr("One or more SSL errors has occurred: %1").arg(errorString),
QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
reply->ignoreSslErrors();
}
}
#endif*/
