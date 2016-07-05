#include "cwidgetvcaresult.h"
#include "cdialogvcamain.h"
#include "CDialogMessageBox.h"
#include <QDir>


#define W_ICON 65
#define H_ICON 65

#define LISTIMAGE_COUNT (20)
extern char *gPieColourNames[];
CWidgetVCAResult::CWidgetVCAResult(QWidget *parent)
: QWidget(parent), m_preItem(NULL), m_curPage(-1), m_totalPage(0), m_bColor(false)
{
	ui.setupUi(this);

	m_PlayWnd = new CWidgetPlayWnd(0, this);
	m_PlayCtrl = new CFormPlayCtrl(0, m_PlayWnd, this);

	m_pDataMgr = new CVCADataMgr();
	//m_imageView = new CImageView(ui.widgetImage);

	m_Render = new CCalibrationD3DRender((HWND)m_PlayWnd->GetPlayWndID());//这个放在构造函数UI会Disabled,原因未知。
	m_Render->Setup(NULL, m_pDataMgr);//这里传NUL，因为构造函数已经传了hwnd
	m_PlayCtrl->SetRender(m_Render);


	m_PlayWnd->SetPreViewMode();
	m_PlayWnd->setGeometry(165, 33, 570, 500);
	m_PlayCtrl->setGeometry(165, 33 + 500 - 5, 570, 70);
	m_PlayCtrl->HideNotUsedButton();

	ui.listWidgetImage->setViewMode(QListView::IconMode);
	//ui.listWidgetImage->setResizeMode(QListView::Adjust);
	ui.listWidgetImage->setGridSize(QSize(W_ICON+15, W_ICON+15));
	ui.listWidgetImage->setSpacing(10);
	ui.listWidgetImage->setIconSize(QSize(W_ICON, W_ICON));

	ui.checkBoxRule->setCheckState(Qt::Checked);
	m_Render->SetZonesVisible(true);

	ui.checkBoxObject->hide();
	ui.labelRule->hide();
	ui.frameColor->hide();
	ui.labelColor->setAutoFillBackground(true);

	UpdateTreeWidgetResult();

	connect(ui.checkBoxRule, SIGNAL(stateChanged(int)), this, SLOT(OnCheckRuleStateChanged(int)));
	connect(ui.treeWidgetVCA, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnTreeWidgetItemDbClick(QTreeWidgetItem*, int)));
	connect(ui.listWidgetImage, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnListWidgetItemDbClick(QListWidgetItem*)));
	connect(ui.listWidgetImage, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnListWidgetItemClick(QListWidgetItem*)));
	connect(ui.BtnDel, SIGNAL(clicked()), this, SLOT(OnBtnResultDelClick()));
	connect(ui.BtnDelAll, SIGNAL(clicked()), this, SLOT(OnBtnResultDelAllClick()));
	connect(ui.BtnPre, SIGNAL(clicked()), this, SLOT(OnBtnPreClick()));
	connect(ui.BtnNext, SIGNAL(clicked()), this, SLOT(OnBtnNextClick()));

//	m_timer = new QTimer();
	//connect(m_timer, SIGNAL(timeout()), this, SLOT(OnImageRefreshTimer()));
	//m_timer->start(1000);
}

CWidgetVCAResult::~CWidgetVCAResult()
{

}

void  CWidgetVCAResult::OnCheckRuleStateChanged(int state)
{
	if (state == Qt::Checked)
	{		
		if (m_bColor)
		{
			m_Render->SetZonesVisible(false);
			ui.frameColor->show();
		}
		else
		{
			m_Render->SetZonesVisible(true);		
		}
	}
	else
	{
		m_Render->SetZonesVisible(false);
		if (m_bColor)
		{
			ui.frameColor->hide();
		}
	}
}

bool CWidgetVCAResult::DeleteDirectory(QString &path)
{
	if (path.isEmpty())
		return false;

	QDir dir(path);
	if (!dir.exists())
		return true;

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	foreach(QFileInfo fi, fileList)
	{
		if (fi.isFile())
			fi.dir().remove(fi.fileName());
		else
			DeleteDirectory(fi.absoluteFilePath());
	}
	return dir.rmpath(dir.absolutePath());
}

void CWidgetVCAResult::customEvent(QEvent * event)
{
	if (event->type() == CUSTOM_AutoStop_EVENT || event->type() ==CUSTOM_ManualStop_EVENT)
	{
		CMyEvent *myEvent = (CMyEvent *)event;
		CFormPlayCtrl *pPlayerCtrlVCA = (CFormPlayCtrl*)myEvent->obj;

		Reset();
		
	}
}

void  CWidgetVCAResult::OnBtnResultDelClick()
{
	QMutexLocker lock(&m_mutex);
	QTreeWidgetItem *pItem = ui.treeWidgetVCA->currentItem();
	if (!pItem)
	{
		return;
	}
	QVariant var = pItem->data(0, Qt::UserRole);
	T_VCAResultNode &resultNode = var.value<T_VCAResultNode>();
	int i = resultNode.casePointIndex;
	int j = resultNode.fileIndex;

	QTreeWidgetItem *pTopItem = pItem;
	int childcount=pItem->childCount();
	if (!pItem->parent())//无父节点，监控点节点
	{
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("确定要清除此监控点下的所有检索结果？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return;
		}
		int fileSize = CDialogVCAMain::ListCasePoint[i].FileList.size();
		for (int j = 0; j<fileSize; j++)
		{		
			QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;
			int index = strFilePathName.lastIndexOf("/");
			QString strFilePath = strFilePathName.left(index);

			int resultSize = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size();
			for (int k = 0; k < resultSize; k++)
			{
				QString strResultPathTime = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(k);
				QString path = strFilePath + strResultPathTime;	
				DeleteDirectory(path);
			}
			
			CDialogVCAMain::ListCasePoint[i].FileList[j].strVCAResultDataList.clear();
			
		}
		if (m_preItem&&m_preItem->parent()->parent() == pItem)
		{
			Reset();
		}
		int indexItem = ui.treeWidgetVCA->indexOfTopLevelItem(pItem);
		ui.treeWidgetVCA->takeTopLevelItem(indexItem);
	}
	else if (pItem->parent() && childcount>0)//文件名节点
	{
		pTopItem = pItem->parent();//pTopItem监控点
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("确定此文件所有检索结果？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return;
		}
		
		QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;
		int index = strFilePathName.lastIndexOf("/");
		QString strFilePath = strFilePathName.left(index);

		int resultSize = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size();
		for (int k = 0; k < resultSize; k++)
		{
			QString strResultPathTime = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(k);
			QString path = strFilePath + strResultPathTime;
			DeleteDirectory(path);
		}
		CDialogVCAMain::ListCasePoint[i].FileList[j].strVCAResultDataList.clear();
//QMutexLocker lock(&m_mutex);
		if (m_preItem&&m_preItem->parent() == pItem)
		{
			Reset();
		}
		int indexItem = pTopItem->indexOfChild(pItem);
		pTopItem->takeChild(indexItem);

		if (pTopItem->childCount() <= 0)//此监控点下的文件为空了，要把上面的节点清空
		{
			indexItem = ui.treeWidgetVCA->indexOfTopLevelItem(pTopItem);
			ui.treeWidgetVCA->takeTopLevelItem(indexItem);
		}

	}
	else if(pItem->parent() && childcount==0)//VCA结果结点
	{
		pTopItem = pItem->parent();//pTopItem监控文件节点
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("确定删除此检索结果？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return;
		}
		QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;
		int index = strFilePathName.lastIndexOf("/");
		QString strFilePath = strFilePathName.left(index);

		int indexItem = pTopItem->indexOfChild(pItem);
		QString strResultPathTime = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(indexItem);
		QString path = strFilePath + strResultPathTime;
		DeleteDirectory(path);
	
		if (m_preItem&&m_preItem == pItem)
		{
			Reset();
		}
		
		pTopItem->takeChild(indexItem);
		CDialogVCAMain::ListCasePoint[i].FileList[j].strVCAResultDataList.removeAt(indexItem);

		if (pTopItem->childCount() <= 0)//此文件下的结果被清空了，要把上面的节点也一起清空
		{
			QTreeWidgetItem *pTopItemParent = pTopItem->parent();
			indexItem = pTopItemParent->indexOfChild(pTopItem);
			pTopItemParent->takeChild(indexItem);//清除文件节点

			if (pTopItemParent->childCount() <= 0)
			{
				indexItem = ui.treeWidgetVCA->indexOfTopLevelItem(pTopItemParent);
				ui.treeWidgetVCA->takeTopLevelItem(indexItem);
			}
		}
	}
	int casePointIndex = ui.treeWidgetVCA->indexOfTopLevelItem(pTopItem);
	CDialogVCAMain::WritePlayFileListVCA();
}

void CWidgetVCAResult::Reset()
{	
	ui.widgetImage->Reset();
	ui.listWidgetImage->clear();
	ui.frameColor->hide();
	ui.BtnPre->setEnabled(false);
	ui.BtnNext->setEnabled(false);
	ui.labelTotal->setText(QStringLiteral("共0条结果"));
	ui.labelRemain->setText(QStringLiteral("剩0条结果"));
	m_curPage = -1;
	m_totalPage = 0;
	if (m_preItem)
	{
		m_preItem->setBackgroundColor(0, QColor(106, 106, 106));
	}
	m_preItem = NULL;
	m_PlayCtrl->Stop();
}

void  CWidgetVCAResult::OnBtnResultDelAllClick()
{
	int count = ui.treeWidgetVCA->topLevelItemCount();
	if (count <= 0)
	{
		return;
	}
	if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
		QStringLiteral("确定清除所有检索结果？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
	{
		return;
	}
	QMutexLocker lock(&m_mutex);
	int size = CDialogVCAMain::ListCasePoint.size();	
	for (int i = 0; i < size; i++)
	{
		
		int fileSize = CDialogVCAMain::ListCasePoint.at(i).FileList.size();
		for (int j = 0; j < fileSize; j++)
		{
			QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;	
			int index = strFilePathName.lastIndexOf("/");		
			QString strFilePath = strFilePathName.left(index);
		
			int resultSize = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size();
			for (int k = 0; k < resultSize;k++)
			{
				QString strResultPathTime = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(k);
				//删除并删除结果
				QString path = strFilePath + strResultPathTime;
				DeleteDirectory(path);
			}//for resultSize
			CDialogVCAMain::ListCasePoint[i].FileList[j].strVCAResultDataList.clear();
		}//for j

		
	}//for i
	Reset();
	ui.treeWidgetVCA->clear();
	
	CDialogVCAMain::WritePlayFileListVCA();
}

void  CWidgetVCAResult::UpdateTreeWidgetResult(bool bFocusItem , int iResult , int jResult)
{
	QTreeWidgetItem *groupCasePoint = NULL;
	QTreeWidgetItem *fileItem = NULL;
	QTreeWidgetItem *reusltItem = NULL;
	QTreeWidgetItem *pFocusItem = NULL;
	int size = CDialogVCAMain::ListCasePoint.size();
	bool bResult = false;
	for (int i = 0; i < size; i++)
	{
		QString strCasePointName = CDialogVCAMain::ListCasePoint.at(i).strCasePointName;
		groupCasePoint = NULL;

		int fileSize = CDialogVCAMain::ListCasePoint.at(i).FileList.size();
		for (int j = 0; j < fileSize; j++)
		{
			//InsertFileItem(CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName, groupCasePoint);
			
			int resultSize = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size();
			if (resultSize>0)
			{
				bResult = true;
				T_VCAResultNode resultNode;
				resultNode.casePointIndex = i;
				resultNode.fileIndex = j;
				QVariant var;
				var.setValue(resultNode);

				if (!groupCasePoint)
				{
					groupCasePoint = new QTreeWidgetItem(ui.treeWidgetVCA);
					groupCasePoint->setText(0, strCasePointName);
					groupCasePoint->setToolTip(0, strCasePointName);
					//groupCasePoint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
					groupCasePoint->setData(0, Qt::UserRole, var);
				}

				QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;
				strFilePathName.replace("\\", "/");
				int index = strFilePathName.lastIndexOf("/");
				QString strName = strFilePathName.mid(index + 1);

				fileItem = new QTreeWidgetItem(groupCasePoint);
				fileItem->setText(0, strName);
				fileItem->setToolTip(0, strFilePathName);
				//fileItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				fileItem->setData(0, Qt::UserRole, var);
	
				for (int k = 0; k < resultSize; k++)
				{
					QString strResultPathTime=CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(k);
					int index = strResultPathTime.lastIndexOf("/");
					QString strTimeName = strResultPathTime.mid(index + 1);

					reusltItem = new QTreeWidgetItem(fileItem);
					reusltItem->setText(0, strTimeName);
					T_VCAResultNode resultNode;
					resultNode.casePointIndex = i;
					resultNode.fileIndex = j;

					QVariant var;
					var.setValue(resultNode);
					reusltItem->setData(0, Qt::UserRole, var);
					reusltItem->setToolTip(0, strTimeName);

				}
				if (bFocusItem&&iResult == i&&jResult == j&&reusltItem)
				{
					pFocusItem = reusltItem;
				}
				//fileItem->setExpanded(true);
			}//if size
		}//for j
	}//for i
	ui.treeWidgetVCA->expandAll();
	if(pFocusItem)
	{
		ui.treeWidgetVCA->scrollToItem(pFocusItem);
	}
	if (bResult)
	{
		ui.BtnDel->setEnabled(true);
		ui.BtnDelAll->setEnabled(true);
	}
}

void CWidgetVCAResult::OnTreeWidgetItemDbClick(QTreeWidgetItem* pItem, int column)
{
	if (pItem->childCount() == 0)//没子节点，说明是时间结果点
	{
		if (m_preItem)
		{
			m_preItem->setBackgroundColor(0, QColor(106, 106, 106));
		}
		Reset();
		pItem->setBackgroundColor(0, QColor(182, 0, 0));
		m_preItem = pItem;

		QVariant var = pItem->data(0, Qt::UserRole);
		T_VCAResultNode &resultNode = var.value<T_VCAResultNode>();
		int i = resultNode.casePointIndex;
		int j = resultNode.fileIndex;
		//int k = resultNode.resultIndex;
		int resultIndex = pItem->parent()->indexOfChild(pItem);

		QString strFilePathName = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName;

		if (m_PlayCtrl->IsPlaying())
		{
			m_PlayCtrl->Stop();
		}
		bool bRet = m_PlayCtrl->OpenAndPlayFile(strFilePathName, NULL);
		if (!bRet)
		{
			CDialogMessageBox dlg;
			QFile file(strFilePathName);
			if (!file.exists())
			{
				dlg.SetText1(QStringLiteral("文件路径不存在！"));

			}
			else
			{
				dlg.SetText1(QStringLiteral("无法播放！文件可能被损坏！"));
			}
			dlg.SetText2(strFilePathName);
			dlg.exec();
			return;
		}

		if (!m_PlayCtrl->IsYUVCallback())//播放后判断是否已设置YUV回调
		{
			m_PlayCtrl->SetDisplayCallback();
			if (!m_PlayCtrl->IsYUVCallback())//不支持YUV回调，则不支持规则显示
			{
				CDialogMessageBox dlg;
				dlg.SetText2(QStringLiteral("此文件不支持显示规则！"));
				dlg.SetText1("");
				dlg.exec();
				return;
			}
		}

		m_PlayWnd->HideToolBar();
		m_PlayCtrl->SetRenderMode(RenderIni);//
		RECT rc;
		QRect rc0 = m_PlayWnd->GetRenderClientRect();
		rc.left = 0;
		rc.top = 0;
		rc.right = rc0.width();
		rc.bottom = rc0.height();
		m_Render->OnChangeClientRect(rc);

		int index = strFilePathName.lastIndexOf("/");
		QString strFilePath = strFilePathName.left(index);
		QString strResultPathTime = CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.at(resultIndex);
		QString imagePath = strFilePath + strResultPathTime;
		m_imagePath = imagePath;
		//读规则
		QString strZoneFileName = imagePath + "/zone.xml";

		QFile file(strZoneFileName);
		if (file.exists())
		{
			CVCAConfigure *pVcaCfg = CVCAConfigure::Instance();
			pVcaCfg->LoadEngine(0, (LPTSTR)strZoneFileName.toStdWString().c_str());

			VCA5_APP_ENGINE_CONFIG *pEngCfg = CVCAConfigure::Instance()->GetEngineConf(0);
			m_pDataMgr->SetData(&(pEngCfg->Zones), &(pEngCfg->Rules), &(pEngCfg->Counters), &(pEngCfg->CalibInfo), &(pEngCfg->ClsObjects));

			m_pDataMgr->SetAdvancedInfo(&(pEngCfg->AdvInfo));
			m_pDataMgr->SetTamperInfo(&(pEngCfg->TamperInfo));
			m_pDataMgr->SetSceneChangeInfo(&(pEngCfg->SceneChangeInfo));

			m_bColor = false;
			for (ULONG i = 0; i < m_pDataMgr->GetZones()->ulTotalZones; ++i)
			{
				VCA5_APP_ZONE *pZone = &(m_pDataMgr->GetZones()->pZones[i]);
				VCA5_APP_RULE	*pRule = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_COLSIG);
				if (pRule&&pRule->ucTicked)
				{
					m_bColor = true;
					QPalette pl = ui.labelColor->palette();
					//设置颜色
					pl.setColor(QPalette::Background, QColor(gPieColourNames[pRule->tRuleDataEx.tColorFilter.usColBin]));
					ui.labelColor->setPalette(pl);
					int precen = ((float)pRule->tRuleDataEx.tColorFilter.usThreshold / (float)255 + 0.005) * 100;
					ui.labelColorPrecen->setText(QString("%1\%").arg(precen));
				}
				pRule = m_pDataMgr->GetRule(i, VCA5_RULE_TYPE_DIRECTION);
				if (pRule&&(pRule->tRuleDataEx.tDirection.sFinishAngle - pRule->tRuleDataEx.tDirection.sStartAngle)>1900)
				{					
					pRule->reserved[0] = 2;
				}
			}

			OnCheckRuleStateChanged(ui.checkBoxRule->checkState());
		}
		QDir dir(imagePath);
		QStringList listFilter;
		listFilter << "*.jpg";
		m_fileinfoList = dir.entryInfoList(listFilter);
		int fileSize = m_fileinfoList.size();
		if (fileSize == 0)
		{
			return;
		}
		ui.labelTotal->setText(QStringLiteral("共%1条结果").arg(fileSize));
		m_totalPage = fileSize / LISTIMAGE_COUNT;
		if (fileSize% LISTIMAGE_COUNT)//有20的余数
		{
			m_totalPage += 1;
		}
		m_curPage = 0;
		UpdateListImage();
	}
}

void CWidgetVCAResult::UpdateListImage()
{
	ui.listWidgetImage->clear();
	int fileSize = m_fileinfoList.size();
	int startIndex = m_curPage*LISTIMAGE_COUNT;
	int n = 0;
	int i = startIndex;
	for (i; i < fileSize; i++)
	{
		if (m_fileinfoList.at(i).isFile())
		{
			if (n >= LISTIMAGE_COUNT)
			{
				break;
			}
			n++;
			
			QString imageFile = m_fileinfoList.at(i).absoluteFilePath(); //.scaled(QSize(W_ICON, H_ICON)
			if (n == 1)
			{
				m_imagePathName = imageFile;
				//m_imageView->ShowImage(imageFile);
				ui.widgetImage->ShowImage(imageFile);
			}
			QListWidgetItem *pItem = new QListWidgetItem(QIcon(QPixmap(imageFile).scaled(QSize(W_ICON, H_ICON))), m_fileinfoList.at(i).baseName(), ui.listWidgetImage);
			
		}
	}
	
	if (m_curPage==m_totalPage-1)//相当于最后一页
	{
		ui.labelRemain->setText(QStringLiteral("剩0条结果"));
	}else
	{
		int remain = fileSize - (m_curPage+1)*LISTIMAGE_COUNT;
		ui.labelRemain->setText(QStringLiteral("剩%1条结果").arg(remain));
	}

	ui.BtnPre->setEnabled(true);
	ui.BtnNext->setEnabled(true);
	if (m_curPage <= 0)
	{
		ui.BtnPre->setEnabled(false);
	}
	
	if(m_curPage==m_totalPage-1)
	{
		ui.BtnNext->setEnabled(false);
	}
}

void CWidgetVCAResult::OnBtnPreClick()
{
	m_curPage--;
	UpdateListImage();
}

void CWidgetVCAResult::OnBtnNextClick()
{
	m_curPage++;
	UpdateListImage();
}

void CWidgetVCAResult::OnListWidgetItemDbClick(QListWidgetItem* pItem)
{
	ui.widgetImage->Reset();

	QString str = pItem->text();
	int index = str.indexOf("(");
	if (index != -1)
	{
		str = str.left(index - 1);
	}
	QTime t = QTime::fromString(str, "hh_mm_ss");
	DWORD dImageTime = 0;
	dImageTime += t.hour() * 3600;
	dImageTime += t.minute() * 60;
	dImageTime += t.second();


//	bool bRet = m_PlayCtrl->SetPlayedTime(dImageTime);
//	if(!bRet)
	{
		DWORD dTotalTime = m_PlayCtrl->GetFileTime();
		float fPos = (float)dImageTime / (float)dTotalTime;
		m_PlayCtrl->SetPlayPos(fPos);
	}

	m_imagePathName = m_imagePath + "/" + pItem->text() + ".jpg";	
	ui.widgetImage->ShowImage(m_imagePathName);
}

void CWidgetVCAResult::OnListWidgetItemClick(QListWidgetItem* pItem)
{
	ui.widgetImage->Reset();

	m_imagePathName = m_imagePath + "/" + pItem->text() + ".jpg";
	ui.widgetImage->ShowImage(m_imagePathName);
}

void CWidgetVCAResult::OnNotifyResult(int iPointVCA, int indexFile)
{
	//1.遍历结果树，查找要插入的地方
	//...
	if (ui.treeWidgetVCA->topLevelItemCount())
	{
	}
	//2.记录当前点，清空生成，再scroll到当前点。与删除或清空按钮要加锁。
	QMutexLocker lock(&m_mutex);
	QString strCurResultTime;
	if (m_preItem)
	{
		strCurResultTime = m_preItem->text(0);
		m_preItem = NULL;
	}
	ui.treeWidgetVCA->clear();
	UpdateTreeWidgetResult(true, iPointVCA, indexFile);

	if (!strCurResultTime.isEmpty())
	{
		QList<QTreeWidgetItem *> listItems = ui.treeWidgetVCA->findItems(strCurResultTime, Qt::MatchRecursive);
		if (listItems.size() > 0)
		{
			m_preItem = listItems.at(0);
			m_preItem->setBackgroundColor(0, QColor(182, 0, 0));
		}
	}
	//ui.treeWidgetVCA->scrollToItem(m_preItem);
}

void CWidgetVCAResult::OnImageRefreshTimer()
{
	//m_imageView->ShowImage(m_imagePathName);
}