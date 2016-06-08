#include "cdialogconfig.h"

#include "qfiledialog.h"
#include <qmessagebox.h>

#include <QDomComment>
#include <QTextStream>

CDialogConfig::CDialogConfig(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.edtPath->setText(m_strPicPath);
	ui.cmbPicType->setCurrentIndex(m_picType);
	ui.edtPicCount->setText(QString("%1").arg(m_picRepeatCount));

	QValidator *validator = new QIntValidator(1, 25, this);
	ui.edtPicCount->setValidator(validator);


	connect(ui.btnOpen, SIGNAL(clicked()), this, SLOT(OnBtnOpenClick()));
	connect(ui.btnConfig, SIGNAL(clicked()), this, SLOT(OnBtnConfigClick()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClick()));
}

CDialogConfig::~CDialogConfig()
{

}

void CDialogConfig::OnBtnOpenClick()
{
	QString pathName = QFileDialog::getExistingDirectory(this,
		QStringLiteral("请选择文件夹"));
	if (pathName != "")
	{
		ui.edtPath->setText(pathName);
	}
}

QString  CDialogConfig::m_strCutPath = "";
QString  CDialogConfig::m_strConvertPath = "";
QString  CDialogConfig::m_strPicPath="";
int  CDialogConfig::m_picType=1;//jpg
int  CDialogConfig::m_picRepeatCount=5;

void CDialogConfig::ReadConfigXml()
{
	QString strPlayListXml = QApplication::applicationDirPath() + "/config.xml";
	QFile file(strPlayListXml);
	QDomDocument doc;
	if (!file.open(QIODevice::ReadOnly))
	{
		m_strCutPath = QStringLiteral("D:/VideoCut");
		m_strConvertPath = QStringLiteral("D:/VideoConvert");
		m_strPicPath = QStringLiteral("D:/PictureCut");
		m_picType = 1;
		m_picRepeatCount = 5;
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

	QDomElement  note = root.firstChildElement();
	while (!note.isNull())
	{
		QString nodeName = note.nodeName();
		QString text = note.text();
		if (nodeName == "picPath")
		{
			m_strPicPath = text;
		}
		else if (nodeName == "videoCutPath")
		{
			m_strCutPath = text;
		}
		else if (nodeName == "videoConvertPath")
		{
			m_strConvertPath = text;
		}
		else if (nodeName == "picType")
		{
			m_picType = text.toInt();
		}
		else if (nodeName == "picRepeatCount")
		{
			m_picRepeatCount = text.toInt();
		}

		note = note.nextSiblingElement();
	}

	if (m_strCutPath.isEmpty())
	{
		m_strCutPath = QStringLiteral("D:/VideoCut");
	}
	if (m_strConvertPath.isEmpty())
	{
		m_strConvertPath = QStringLiteral("D:/VideoConvert");
	}
	
	file.close();
}

bool CDialogConfig::WriteConfigXml()
{
	QDomDocument doc;

	QDomNode instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement("config");
	QDomText nodeText = doc.createTextNode("");
	root.appendChild(nodeText);
	doc.appendChild(root);

	QDomElement noteCutPath = doc.createElement("videoCutPath");
	QDomText noteCutPathText = doc.createTextNode(m_strCutPath);
	noteCutPath.appendChild(noteCutPathText);
	root.appendChild(noteCutPath);

	QDomElement noteConvertPath = doc.createElement("videoConvertPath");
	QDomText noteConvertPathText = doc.createTextNode(m_strConvertPath);
	noteConvertPath.appendChild(noteConvertPathText);
	root.appendChild(noteConvertPath);

	QDomElement notePicPath = doc.createElement("picPath");
	QDomText notePicPathText = doc.createTextNode(m_strPicPath);
	notePicPath.appendChild(notePicPathText);
	root.appendChild(notePicPath);

	QDomElement notePicType = doc.createElement("picType");
	QDomText notePicTypeText = doc.createTextNode(QString("%1").arg(m_picType));
	notePicType.appendChild(notePicTypeText);
	root.appendChild(notePicType);

	QDomElement notePicCount = doc.createElement("picRepeatCount");
	QDomText notePicCountText = doc.createTextNode(QString("%1").arg(m_picRepeatCount));
	notePicCount.appendChild(notePicCountText);
	root.appendChild(notePicCount);

	QString strPlayListXml = QApplication::applicationDirPath() + "/config.xml";
	QFile file(strPlayListXml);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		return false;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");
	doc.save(out, 4, QDomNode::EncodingFromTextStream);

	file.close();
	return true;

}

void CDialogConfig::OnBtnConfigClick()
{
	m_strPicPath = ui.edtPath->text();
	m_picType=ui.cmbPicType->currentIndex();
	m_picRepeatCount = ui.edtPicCount->text().toInt();

	WriteConfigXml();

	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("保存成功！"));	
}

void CDialogConfig::OnBtnCancelClick()
{
	this->reject();
}