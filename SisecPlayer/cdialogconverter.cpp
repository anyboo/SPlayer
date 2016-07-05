#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif

#include "cdialogconverter.h"
#include <QtGui>
#include <qfiledialog.h>
#include "public_def.h"
#include <qitemdelegate.h>
#include <qprogressdialog.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

#include "cdialogconfig.h"

class TableViewDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	inline TableViewDelegate(QWidget *w) : QItemDelegate(w) {}

	inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
	{
		if (index.column() != 2) {
			QItemDelegate::paint(painter, option, index);
			return;
		}

		// Set up a QStyleOptionProgressBar to precisely mimic the
		// environment of a progress bar.
		QStyleOptionProgressBar progressBarOption;
		progressBarOption.state = QStyle::State_Enabled;
		progressBarOption.direction = QApplication::layoutDirection();
		progressBarOption.rect = option.rect;
		progressBarOption.fontMetrics = QApplication::fontMetrics();
		progressBarOption.minimum = 0;
		progressBarOption.maximum = 100;
		progressBarOption.textAlignment = Qt::AlignCenter;
		progressBarOption.textVisible = true;

		// Set the progress and text values of the style option.
		//int progress = qobject_cast<CDialogConverter *>(parent())->GetRowProgress(index.row());
	/*	int progress = ((CDialogConverter *)(parent())->GetRowProgress(index.row());
		progressBarOption.progress = progress < 0 ? 0 : progress;
		progressBarOption.text = QString().sprintf("%d%%", progressBarOption.progress);*/

		// Draw the progress bar onto the view.
		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
	}
};

Thread::Thread()
{

}

Thread::~Thread()
{

}

void Thread::run()
{
	CDialogConverter *pDlg = (CDialogConverter*)m_pOwner;
	pDlg->StartReadSrcFileInfo();
}

CDialogConverter::CDialogConverter(QWidget *parent)
: NoFlameDlg(parent), m_nPort(-1), m_curTask(-1), m_pPreviewDlg(NULL), m_bAbort(true)
{
	ui.setupUi(this);

	CDialogConfig::ReadConfigXml();
	ui.lineEditDstPath->setText(CDialogConfig::m_strConvertPath);

	m_PlayWnd = new CWidgetPlayWnd(0, this);
	m_PlayCtrl = new CFormPlayCtrl(0, m_PlayWnd, this);

	m_getProgressTimer = new QTimer(this);
	//m_PlayWnd->setGeometry(2, 40, 500, 500);
	//m_PlayWnd->raise();
	//m_PlayCtrl->setGeometry(2, 40 + 500 + 2, 500, 70);
	m_PlayWnd->hide();
	m_PlayCtrl->hide();
//	ui.frameTool->BtnListUp->setEnabled(false);
//	ui.frameTool->BtnListDown->setEnabled(false);
	

	SetCtrlEnabled(true);
	//ui.tableWidget->setItemDelegate(new TableViewDelegate(ui.tableWidget));
	ui.tableWidget->horizontalHeader()->setSectionsClickable(false);
	ui.tableWidget->setColumnWidth(0, 150);

	connect(this, SIGNAL(finish()), this, SLOT(FinishGetSrcFileInfo()));
	connect(m_getProgressTimer, SIGNAL(timeout()), this, SLOT(TimeGetProgress()));
	connect(ui.BtnFileAdd, SIGNAL(clicked()), this, SLOT(OnBtnFileAddClick()));
	connect(ui.BtnFileDel, SIGNAL(clicked()), this, SLOT(OnBtnFileDelClick()));
	connect(ui.BtnEdit, SIGNAL(clicked()), this, SLOT(OnBtnEditClick()));
	connect(ui.BtnFileDelAll, SIGNAL(clicked()), this, SLOT(OnBtnFileDelAllClick()));
	connect(ui.BtnDstPathOpen, SIGNAL(clicked()), this, SLOT(OnBtnFilePathClick()));

	connect(ui.BtnClose, SIGNAL(clicked()), this, SLOT(OnBtnCloseClick()));
	connect(ui.BtnMin, SIGNAL(clicked()), this, SLOT(OnBtnMinClick()));

	connect(ui.BtnStart, SIGNAL(clicked()), this, SLOT(OnBtnStartClick()));
	connect(ui.BtnStop, SIGNAL(clicked()), this, SLOT(OnBtnStopClick()));
	
	connect(ui.tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(OnItemDoubleClicked(QTableWidgetItem*)));
}

CDialogConverter::~CDialogConverter()
{
	delete m_PlayWnd;
	delete m_PlayCtrl;
	delete m_getProgressTimer;
}

void CDialogConverter::OnBtnCloseClick()
{
	if (!ui.BtnStart->isEnabled())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先停止任务！"));
		return;
	}
	CDialogConfig::m_strConvertPath = ui.lineEditDstPath->text();
	CDialogConfig::WriteConfigXml();
	this->accept();
}

void CDialogConverter::OnBtnMinClick()
{
	this->showMinimized();
}

void CDialogConverter::FinishGetSrcFileInfo()
{
	m_msgDlg.accept();
}

void CDialogConverter::StartReadSrcFileInfo()
{
	for (int i = 0; i < 5; i++)
	{
		Sleep(1000);
		if (m_PlayCtrl->GetFileTime() != 0)
		{
			break;
		}
	}
	emit finish();//因为此线程不能直接操作UI，要发送信号来操作 m_msgDlg.accept();
}

void CDialogConverter::OnBtnFileAddClick()
{
	QStringList fileList = QFileDialog::getOpenFileNames(this,
		QStringLiteral("请选择文件"), "", FileFilter);
	int size = fileList.size();
	if (size != 0)
	{		
		for (int i = 0; i < size; i++)
		{
			QString strPathName = fileList.at(i);
			strPathName.replace("\\", "/");
			int index = strPathName.lastIndexOf("/");
			QString strPath = strPathName.left(index);
			QString strName = strPathName.mid(index + 1);

			
		//	bool bRet = m_PlayCtrl->OpenAndPlayFile(strPathName, NULL);
			bool bRet = m_PlayCtrl->OpenAndPlayFile(strPathName, (HWND)m_PlayWnd->GetPlayWndID());
			if (bRet)
			{
			//	m_PlayCtrl->SetRenderMode();
				m_PlayCtrl->StopAudio();
				
			//	m_getSrcFileDurationTimer->start(10);//用定时器会卡住页面m_msgDlg对话框,用线程
				m_threadReadSrcFile.SetOwner(this);
				m_threadReadSrcFile.start();
				m_msgDlg.setStandardButtons(QMessageBox::NoButton);
				m_msgDlg.setWindowTitle(QStringLiteral("提示"));
				m_msgDlg.setText(QStringLiteral("正在获取文件信息..."));
				m_msgDlg.exec();

				long width = 0;
				long height = 0;
				m_PlayCtrl->GetPicSize(&width, &height);
				T_ConverterFileInfo tConvertFileInfo;
				//	memset(&tConvertFileInfo, 0, sizeof(T_ConverterFileInfo));//有QString 不能memset
				tConvertFileInfo.progress = 0;
				tConvertFileInfo.state = NOTSTART;
				tConvertFileInfo.strSrcPath = strPath;
				tConvertFileInfo.strSrcFileName = strName;
				tConvertFileInfo.width = width;
				tConvertFileInfo.height = height;
				tConvertFileInfo.bFileCut = false;
				tConvertFileInfo.iStartTime = 0;
				tConvertFileInfo.iEndTime = 0;
				tConvertFileInfo.iFileDuation = m_PlayCtrl->GetFileTime();
				tConvertFileInfo.dstFormat = ui.cmbDstFormat->currentIndex();

				FILE *pfd = NULL;
				QTextCodec *textcode = QTextCodec::codecForName("GB18030");
				char szFileName[1024];
				strcpy(szFileName, textcode->fromUnicode(strPathName).data());
				int ret = fopen_s(&pfd, szFileName, "rb");
				if (pfd)//打开文件成功
				{
					fseek(pfd, 0, SEEK_END);
					tConvertFileInfo.iFileSize = ftell(pfd);
					fclose(pfd);
				}

				m_convertFileInfoList.append(tConvertFileInfo);

				int rowCount = ui.tableWidget->rowCount();
				ui.tableWidget->insertRow(rowCount);
				QTableWidgetItem *pItem = NULL;
				for (int i = 0; i < Column_Count; i++)
				{
					pItem = new QTableWidgetItem();
					pItem->setTextAlignment(Qt::AlignCenter);
					if (i == FileName)
					{
						pItem->setText(tConvertFileInfo.strSrcFileName);
						pItem->setToolTip(tConvertFileInfo.strSrcFileName);
					}
					else if (i == FilePath)
					{
						pItem->setText(tConvertFileInfo.strSrcPath);
						pItem->setToolTip(tConvertFileInfo.strSrcPath);
					}
					else if (i == FileSize)
					{
						float fG=tConvertFileInfo.iFileSize / (float)(1024 * 1024 * 1024);
						float fM = tConvertFileInfo.iFileSize / (float)(1024 * 1024 );
						char buf[256];
						sprintf_s(buf, "%.2fM", fM);
						QString strFileSize = QString("%1").arg(buf);
						if (fM>1000.0)
						{
							sprintf_s(buf, "%.2fG", fG);
							strFileSize = QString("%1").arg(buf);
						}
						if (fM<1.0)
						{
							sprintf_s(buf, "%.2fK", tConvertFileInfo.iFileSize / (float)(1024));
							strFileSize = QString("%1").arg(buf);
						}
						pItem->setText(strFileSize);
					}
					else if (i == FileWidthHeight)
					{
						pItem->setText(QString("%1*%2").arg(tConvertFileInfo.width).arg(tConvertFileInfo.height));
					}
					else if (i == FileDuation)
					{
						char buf[256];
						memset(buf, 0, 256);
						sprintf(buf, "%02d:%02d:%02d", tConvertFileInfo.iFileDuation / 3600, (tConvertFileInfo.iFileDuation / 60) % 60, tConvertFileInfo.iFileDuation % 60);
						//unsigned __int64这样直接写打印出来是38:00:49 ,因为是64位的 %ld也不行，要%lld才正常，
						//因为%ld是打印long型，在32位机中long相当于int ,因此和%d是一样效果.%lld也有可能出错，最好用%I64d 结论：int %d, long %ld ,long long %lld ,int64 %I64d.

						pItem->setText(QString("%1").arg(buf));
						pItem->setText(buf);
					}
					else if (i == DstFormat)
					{
						if (tConvertFileInfo.dstFormat == 0)
						{
							pItem->setText("avi");
						}
						else
						{
							pItem->setText("mp4");
						}
					}
					else if (i == State)
					{
						pItem->setText(QStringLiteral("未开始"));
					}

					ui.tableWidget->setItem(rowCount, i, pItem);
				}

			}//bRet		
			
			m_PlayCtrl->Stop();
		}
			
	}
}

void CDialogConverter::OnBtnFileDelClick()
{
	int row = ui.tableWidget->currentRow();
	if (row >= 0)
	{
		m_convertFileInfoList.removeAt(row);
		ui.tableWidget->removeRow(row);
	}

	ui.tableWidget->setCurrentCell(row, 0);
}

void CDialogConverter::OnBtnFileDelAllClick()
{
	int row= ui.tableWidget->rowCount();
	if (row == 0)
	{
		return;
	}
	if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
		QStringLiteral("确定要清空列表？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
	{
		return;
	}
	m_convertFileInfoList.clear();

	
	while ((row-1) >=0)
	{
		ui.tableWidget->removeRow(row-1);
		row = ui.tableWidget->rowCount();
	}
	
}

void  CDialogConverter::OnBtnEditClick()
{
	int row = ui.tableWidget->currentRow();
	if (row >= 0)
	{
		EditConvertInfo(row);
	}
}

void  CDialogConverter::OnItemDoubleClicked(QTableWidgetItem*item)
{
	int row = item->row();
	EditConvertInfo(row);
}

void CDialogConverter::OnBtnFilePathClick()
{
	QString pathName = QFileDialog::getExistingDirectory(this,
		QStringLiteral("请选择文件夹"));
	if (pathName != "")
	{
		ui.lineEditDstPath->setText(pathName);
	}
}

void  CDialogConverter::EditConvertInfo(int row)
{
	CDialogConverterPreview dlg(this);
	dlg.SetConvertFileInfo(&m_convertFileInfoList[row]);
	m_pPreviewDlg = &dlg;

	int iScreenWidth = QApplication::desktop()->width();
	int iScreenHeigth = QApplication::desktop()->height();
	QRect rc = dlg.geometry();
	dlg.move((iScreenWidth - rc.width()) / 2, (iScreenHeigth - rc.height()) / 2);
	if (QDialog::Accepted == dlg.exec())
	{
		QTableWidgetItem *pItem = ui.tableWidget->item(row, DstFormat);
		if (pItem)
		{
			if (m_convertFileInfoList[row].dstFormat == 0)
			{
				pItem->setText("avi");
			}
			else
			{
				pItem->setText("mp4");
			}
		}

		pItem = ui.tableWidget->item(row, Cut);
		if (pItem)
		{
			pItem->setText("");
		}
		if (m_convertFileInfoList[row].bFileCut)
		{
			DWORD dTime = m_convertFileInfoList[row].iStartTime;
			int hours = dTime / 3600;
			int mins = dTime / 60 % 60;
			int secs = dTime % 60;
			char buf[256];
			sprintf_s(buf, "%d:%d:%d", hours, mins, secs);
			QString strCutTime = QString("%1").arg(buf);

			dTime = m_convertFileInfoList[row].iEndTime;
			hours = dTime / 3600;
			mins = dTime / 60 % 60;
			secs = dTime % 60;
			sprintf_s(buf, "%d:%d:%d", hours, mins, secs);
			strCutTime.append(QString("-%1").arg(buf));

			if (pItem)
			{
				pItem->setText(strCutTime);
			}

		}
	}
	m_pPreviewDlg = NULL;
}

void CDialogConverter::OnBtnStartClick()
{
	int size = m_convertFileInfoList.size();
	if (size == 0)
	{
		return;
	}

	if (ui.lineEditDstPath->text().isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请输入保存路径！"));
		return;
	}

	QDir dir(ui.lineEditDstPath->text());
	if (!dir.exists())
	{
		if (!dir.mkpath(ui.lineEditDstPath->text()))
		{
			QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("创建保存路径失败！"));
			return;
		}
	}

	int row = ui.tableWidget->rowCount();
	m_bAbort = true;
	if (row > 0&&m_convertFileInfoList.at(0).state != NOTSTART)
	{
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("是否跳过已转换过的？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			m_bAbort = false;
		}
	}
	for (int i = 0; i < row; i++)
	{	
		if (m_bAbort&&(m_convertFileInfoList.at(i).state != NOTSTART && m_convertFileInfoList.at(i).state != STOP))
		{
			continue;
		}
		QTableWidgetItem *pItem = ui.tableWidget->item(i, State);
		if (pItem)
		{
			pItem->setText(QStringLiteral("等待中"));
		}
		pItem = ui.tableWidget->item(i, Progress);
		if (pItem)
		{
			pItem->setText(QStringLiteral(""));
		}
	}
	m_curTask = -1;
	GoNextConvert();
}

void CDialogConverter::GoNextConvert()
{
	m_curTask++;
	if ( m_curTask >= m_convertFileInfoList.size())
	{
		SetCtrlEnabled(true);
		m_nPort = -1;
		m_curTask = -1;
		return;
	}
	if (m_bAbort&&(m_convertFileInfoList.at(m_curTask).state != NOTSTART&&m_convertFileInfoList.at(m_curTask).state != STOP))
	{
		GoNextConvert();
		return;
	}
	SetCtrlEnabled(false);

	QString srcFileName = m_convertFileInfoList.at(m_curTask).strSrcPath + "/" + m_convertFileInfoList.at(m_curTask).strSrcFileName;
	QTextCodec *textcode = QTextCodec::codecForName("GB18030");

	char szFileName[1024];
	strcpy(szFileName, textcode->fromUnicode(srcFileName).data());

	QString dstFile = ui.lineEditDstPath->text() + "/" + m_convertFileInfoList.at(m_curTask).strSrcFileName.left(m_convertFileInfoList.at(m_curTask).strSrcFileName.indexOf('.'));
	


	unsigned long long iStartTime = 0;
	unsigned long long iEndTime = 0;
	if (m_convertFileInfoList.at(m_curTask).bFileCut)
	{
		iStartTime = m_convertFileInfoList.at(m_curTask).iStartTime;
		iEndTime = m_convertFileInfoList.at(m_curTask).iEndTime;
	
		int hours = iStartTime / 3600;
		int mins = iStartTime / 60 % 60;
		int secs = iStartTime % 60;
		char buf[32];
		sprintf_s(buf, "%02d%02d%02d", hours, mins, secs);

		hours = iEndTime / 3600;
		mins = iEndTime / 60 % 60;
		secs = iEndTime % 60;
		char buf2[32];
		sprintf_s(buf2, "%02d%02d%02d", hours, mins, secs);
		dstFile.append(QString("(%1-%2)").arg(buf).arg(buf2));

	}

	if (m_convertFileInfoList.at(m_curTask).dstFormat == 0)
		dstFile.append(".avi");
	else if (m_convertFileInfoList.at(m_curTask).dstFormat == 1)
		dstFile.append(".mp4");
	

	char szDstFileName[1024];
	strcpy(szDstFileName, textcode->fromUnicode(dstFile).data());
	m_convertFileInfoList[m_curTask].dstPathFileName = dstFile;

	bool ret = GetPlayerInterface(&m_nPort);
	if (ret)
	{
		if (Player_FileConvertStart(m_nPort, szFileName, szDstFileName, iStartTime, iEndTime, false, NULL))//unicode 转char 
		{
			m_getProgressTimer->start(1000);
			QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, State);
			if (pItem)
			{
				pItem->setText(QStringLiteral("进行中"));
			}
		}
		else{
			QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, State);
			if (pItem)
			{
				//pItem->setText(QStringLiteral("失败！无法读取源文件有效数据"));
				pItem->setText(QStringLiteral("失败(此文件不支持)"));
			}
			StopCurTask();
			GoNextConvert();
		}
	}
}

void CDialogConverter::SetCtrlEnabled(bool bEnabled)
{
	ui.frameTool->setEnabled(bEnabled);
	ui.lineEditDstPath->setEnabled(bEnabled);
	ui.BtnStart->setEnabled(bEnabled);
	ui.BtnDstPathOpen->setEnabled(bEnabled);
	ui.cmbDstFormat->setEnabled(bEnabled);
	
	ui.BtnStop->setEnabled(!bEnabled);
}

void CDialogConverter::StopCurTask()
{
	if (m_getProgressTimer->isActive())
	{
		m_getProgressTimer->stop();
	}
	Player_FileConvertClose(m_nPort);
	FreePlayerInterface(m_nPort);
}

void CDialogConverter::UkeyDownStop()
{
	if (m_pPreviewDlg)
	{
		((CDialogConverterPreview*)m_pPreviewDlg)->UkeyDownStop();
	}
	if (!ui.BtnStart->isEnabled())
	{
		OnBtnStopClick();
	}
}


void CDialogConverter::OnBtnStopClick()
{
	StopCurTask();
	m_convertFileInfoList[m_curTask].state = STOP;
	QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, State);
	if (pItem)
	{
		pItem->setText(QStringLiteral("停止"));
	}
	SetCtrlEnabled(true);
	m_nPort = -1;
	m_curTask = -1;
}

void  CDialogConverter::TimeGetProgress()
{
	if (m_nPort != -1)
	{
		if (m_curTask == -1 || m_curTask >= m_convertFileInfoList.size())
		{
			return;
		}
		int progress=Player_FileConvertProcess(m_nPort);
		m_convertFileInfoList[m_curTask].progress = progress;
		ui.tableWidget->repaint();

		QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, Progress);
		if (pItem)
		{
			pItem->setText(QString("%1\%").arg(progress));
		}
		if (progress == 100)
		{		
			StopCurTask();

			QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, State);
			bool bRet = m_PlayCtrl->OpenAndPlayFile(m_convertFileInfoList.at(m_curTask).dstPathFileName,NULL);
			if (bRet)
			{
				m_PlayCtrl->StopAudio();
				pItem->setText(QStringLiteral("成功"));
			}
			else
			{
	
				pItem->setText(QStringLiteral("失败(此文件不支持)"));
				pItem = ui.tableWidget->item(m_curTask, Progress);
				pItem->setText(QString("%1\%").arg(0));
				
			}
			m_PlayCtrl->Stop();

			m_convertFileInfoList[m_curTask].state = FINISH;	

			GoNextConvert();
		}
		/*else if (progress == -1)//出错
		{
			m_convertFileInfoList[m_curTask].state = CONVERTERROR;
			QTableWidgetItem *pItem = ui.tableWidget->item(m_curTask, State);
			if (pItem)
			{
				pItem->setText(QStringLiteral("出错"));
			}
			StopCurTask();
			GoNextConvert();
		}*/
	
	}
}

int CDialogConverter::GetRowProgress(int row)
{
	return m_convertFileInfoList.at(m_curTask).progress;
}