#include "cupdatedlg.h"
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <QDomComment>
#include <QTextStream>

CUpdateDlg::CUpdateDlg(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);

	m_progressDialog = new QProgressDialog(this);
	m_progressDialog->setMinimumDuration(0);
	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

	ui.BtnOK->hide();
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(accept()));
}

CUpdateDlg::~CUpdateDlg()
{
	delete m_progressDialog;
}


void CUpdateDlg::downloadFile(char *purl)
{
	QUrl url = QUrl(purl);
	QFileInfo fileInfo(url.path());
//	QString fileName = fileInfo.fileName();
	QString fileName = QApplication::applicationDirPath() + "/" + fileInfo.fileName();

	m_file = new QFile(fileName);
	if (!m_file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, tr("HTTP"),
			tr("Unable to save the file %1: %2.")
			.arg(fileName).arg(m_file->errorString()));
		delete m_file;
		m_file = 0;
		return;
	}

	m_progressDialog->setWindowTitle(QStringLiteral("下载进度"));
	m_progressDialog->setLabelText(tr("Downloading...").arg(fileName));

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
	m_progressDialog->setValue(0);
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

		ui.label->setText(QStringLiteral("已取消下载！"));
		if (m_file) {
			m_file->close();
			m_file->remove();
			delete m_file;
			m_file = 0;
		}
		m_reply->deleteLater();
		m_progressDialog->hide();
		ui.BtnOK->show();
		return;
	}

	m_progressDialog->hide();
	m_file->flush();
	m_file->close();
	

	QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (m_reply->error()) {
		m_file->remove();
		ui.label->setText(QStringLiteral("下载失败: %1.")
			.arg(m_reply->errorString()));
		m_reply->deleteLater();
		m_reply = 0;
		delete m_file;
		m_file = 0;
		ui.BtnOK->show();
		return;
		//downloadButton->setEnabled(true);
	}
	else if (!redirectionTarget.isNull()) {
	/*	QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		if (QMessageBox::question(this, tr("HTTP"),
			tr("Redirect to %1 ?").arg(newUrl.toString()),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			url = newUrl;
			m_reply->deleteLater();
			m_file->open(QIODevice::WriteOnly);
			m_file->resize(0);
			startRequest(url);
			return
		};*/
	}
	else {
	//	QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
		//statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
		//downloadButton->setEnabled(true);
		ui.label->setText(QStringLiteral("下载完成！")
			.arg(m_reply->errorString()));
		m_reply->deleteLater();
		m_reply = 0;
		delete m_file;
		m_file = 0;

		ui.label->setText(QStringLiteral("正在更新...请稍后！"));
		m_processThread = new CProcessThread();
		connect(m_processThread, SIGNAL(ResultString(QString)), this, SLOT(OnResultString(QString)));
		m_processThread->start();

	}	
}

void CUpdateDlg::OnResultString(QString str)
{
	ui.label->setText(str);
	ui.BtnOK->show();
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
