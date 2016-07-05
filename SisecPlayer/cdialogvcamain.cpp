#include "stdafx.h"
#include "cdialogvcamain.h"
#include <QDomComment>
#include <QTextStream>
#include <QProgressDialog>

#define FILEPLAYLISVCATXML "playlistVCA.xml"
#define ROOTTEXTVCA  "playFileListVCA"
/*
#define NOTETEXTVCA "file"
#define NOTETEXTVCA "fileName"
#define NOTETEXTVCA "Result"*/

QList<T_CasePoint> CDialogVCAMain::ListCasePoint;
CDialogVCAMain *pMainDlg = NULL;
CDialogVCAMain::CDialogVCAMain(QWidget *parent)
: NoFlameDlg(parent)
{
	ui.setupUi(this);

	pMainDlg = this;

	ReadPlayFileListVCA();

    m_dlgVCA = new CDialogVCA();
	m_widgetProgress = new  CWidgetVCAProgress(m_dlgVCA->GetVCADataMgr());
	m_widgetResult = new  CWidgetVCAResult;
	m_dlgVCA->SetProgress(m_widgetProgress);
	
	ui.tabWidget->addTab(m_dlgVCA, QStringLiteral("检索规则"));
	ui.tabWidget->addTab(m_widgetProgress, QStringLiteral("检索进度"));
	ui.tabWidget->addTab(m_widgetResult, QStringLiteral("检索结果"));

	connect(ui.BtnClose, SIGNAL(clicked()), this, SLOT(OnBtnCloseClick()));
	connect(ui.BtnMin, SIGNAL(clicked()), this, SLOT(OnBtnMinClick()));
	//connect(ui.BtnCaseAdd, SIGNAL(clicked()), this, SLOT(OnBtnCaseAddClick()));
	//connect(ui.BtnCasePointAdd, SIGNAL(clicked()), this, SLOT(OnBtnCasePointAddClick()));

	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(OnSetup()));
}

CDialogVCAMain::~CDialogVCAMain()
{

}

void CDialogVCAMain::OnBtnMinClick()
{
	this->showMinimized();
}

void CDialogVCAMain::OnBtnCloseClick()
{
	Close();
}

bool CDialogVCAMain::Close()
{
	if (m_widgetProgress->IsVCAProgressing())
	{
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("有视频正在检索，是否要停止关闭？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return false;;
		}
	}
	m_dlgVCA->Reset();
	m_widgetResult->Reset();
	m_widgetProgress->Reset();
	this->hide();
	return true;
}

void CDialogVCAMain::closeEvent(QCloseEvent *ev)
{
	//m_dlgVCA->close();
	//m_widgetProgress->close();
	//m_widgetResult->close();
	QDialog::closeEvent(ev);
}

void CDialogVCAMain::StartShow()
{
	if (this->isVisible())
	{
		if (this->isMinimized())
		{
			this->showNormal();
		}
		return;
	}
//	this->setEnabled(false);
	ui.frameDiabled->show();
	ui.frameDiabled->raise();

	ui.tabWidget->setCurrentIndex(0);

	m_dlgInit.setParent(this);
	m_dlgInit.setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
	m_dlgInit.setText(QStringLiteral("正在初始化..."));
	m_dlgInit.setStandardButtons(0);
	m_dlgInit.setModal(true);
	m_dlgInit.show();
	
	this->show();
	
	m_timer->start(1000);
}

void CDialogVCAMain::OnSetup()
{
	m_timer->stop();
	//QProgressDialog dlg;
	//dlg.setRange(0, 100);
//	dlg.setMinimumDuration(0);
	//Sleep(1000);
	//dlg.setValue(20);
	if (CSimpleVCA::VCASetup())
	{
		//dlg.setValue(100);
		m_dlgInit.accept();
	}
	else{
		m_dlgInit.accept();

		char strError[1024];
		 CSimpleVCA::GetLastErrorStr(strError);
		 QString qstrError;
		 qstrError.fromStdString(strError);
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("初始化失败!") + qstrError);
	}
	//this->setEnabled(true);
	ui.frameDiabled->hide();
	ui.frameDiabled->lower();
}

void  CDialogVCAMain::ReadPlayFileListVCA()
{
	QString strPlayListXml = QApplication::applicationDirPath() + "/" + FILEPLAYLISVCATXML;
	QFile file(strPlayListXml);
	QDomDocument doc;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}

	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	//读文件列表
	QDomElement root = doc.documentElement();
	if (root.isNull())
	{
		file.close();
		return;
	}

	QDomElement  noteCasePoint = root.firstChildElement();
	while (!noteCasePoint.isNull())
	{
		QString strCasePointName = noteCasePoint.nodeName();

		T_CasePoint casePoint;
		casePoint.strCasePointName = strCasePointName;
		QDomElement  noteFileName = noteCasePoint.firstChildElement();//file
		while (!noteFileName.isNull())
		{
			//QString strPathName = noteFileName.text();

			T_FileVCA tFileVCA;
			QDomElement  noteVCAResult = noteFileName.firstChildElement();			
			while (!noteVCAResult.isNull())
			{
				if (noteVCAResult.nodeName() == "fileName")
				{
					tFileVCA.strFileName = noteVCAResult.text();
				}
				else
				{
					QString strResult = noteVCAResult.text();//相对路径，显示需去掉前面部分
					tFileVCA.strVCAResultDataList.append(strResult);
				}
				noteVCAResult = noteVCAResult.nextSiblingElement();
			}
			casePoint.FileList.append(tFileVCA);
			noteFileName = noteFileName.nextSiblingElement();
		}
		ListCasePoint.append(casePoint);
		noteCasePoint = noteCasePoint.nextSiblingElement();
	}

	file.close();

}
void  CDialogVCAMain::WritePlayFileListVCA()
{
	//需要加锁，因为VCA检索列表更新和监控点文件增加会同时进行写文件
	//写播放列表
	QDomDocument doc;

	QDomNode instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement(ROOTTEXTVCA);
	QDomText nodeText = doc.createTextNode("");
	root.appendChild(nodeText);
	doc.appendChild(root);

	int sizeCase = ListCasePoint.size();
	for (int i = 0; i < sizeCase; i++)
	{
		QDomElement noteCase = doc.createElement(ListCasePoint.at(i).strCasePointName);
		//QDomText nodeText = doc.createTextNode("");
		//noteCase.appendChild(nodeText);
		int sizeFile = ListCasePoint.at(i).FileList.size();
		for (int j = 0; j < sizeFile; j++)
		{
			QDomElement noteFile = doc.createElement("file"); 

			QDomElement noteFileName= doc.createElement("fileName");
			QDomText nodeText = doc.createTextNode(ListCasePoint.at(i).FileList.at(j).strFileName);
			noteFileName.appendChild(nodeText);
			noteFile.appendChild(noteFileName);

			int sizeResult = ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size();
			for (int k = 0; k< sizeResult; k++)
			{
				QDomElement noteResult = doc.createElement("VCAResult");
				QDomText nodeText = doc.createTextNode(ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(k));
				noteResult.appendChild(nodeText);
				noteFile.appendChild(noteResult);
			}
			
			noteCase.appendChild(noteFile);
		}

		root.appendChild(noteCase);
	}


	QString strPlayListXml = QApplication::applicationDirPath() + "/" + FILEPLAYLISVCATXML;
	QFile file(strPlayListXml);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		return;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");
	doc.save(out, 4, QDomNode::EncodingFromTextStream);

	file.close();
}

void CDialogVCAMain::NotifyResult(int iPointVCA, int indexFile)
{
	pMainDlg->m_widgetResult->OnNotifyResult(iPointVCA, indexFile);
}