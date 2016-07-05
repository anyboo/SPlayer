#include "cdialogcaseadd.h"
#include "public_def.h"
#include <qfiledialog.h>
#include <qmessagebox.h>

CDialogCaseAdd::CDialogCaseAdd(QWidget *parent)
: NoFlameDlg(parent)
{
	ui.setupUi(this);

	connect(ui.BtnClose_2, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.BtnFileAdd, SIGNAL(clicked()), this, SLOT(OnBtnFileAddClick()));
	connect(ui.BtnOK, SIGNAL(clicked()), this, SLOT(OnBtnOKClick()));
	connect(ui.BtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

CDialogCaseAdd::~CDialogCaseAdd()
{

}

void CDialogCaseAdd::OnBtnFileAddClick()
{
	QStringList fileList = QFileDialog::getOpenFileNames(this,
		QStringLiteral("请选择文件"), "", FileFilter);
	
	int size = fileList.size();
	if (size != 0)
	{
		m_strFileList = fileList;
		QString strFile;
		for (int i = 0; i < size; i++)
		{
			strFile.append(fileList.at(i)).append("\n");
		}
		ui.plainTextEdit->setPlainText(strFile);
	}
}

void CDialogCaseAdd::OnBtnOKClick()
{
	if (ui.lineEditCaseName->text().isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("案件名称不能为空！"));
		return;
	}
	if (ui.lineEditCasePointName->text().isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("监控点不能为空！"));
		return;
	}
	m_strCaseName = ui.lineEditCaseName->text();
	m_strCasePointName = ui.lineEditCasePointName->text();
	this->accept();
}