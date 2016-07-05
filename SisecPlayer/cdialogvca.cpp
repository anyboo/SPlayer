#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif
//#include "stdafx.h"

#include "cdialogvca.h"

#include "VCA/SimpleVCA.h"
#include "CAP5BoardLibEx.h"
#include "cdialogmessagebox.h"
#include "public_def.h"

#include "cdialogcaseadd.h"
#include <qmessagebox.h>

#include <QFileDialog>
#include <QColorDialog>
#include "cdialogvcamain.h"

int g_usThreshold[5] = { 254 * 0.1+0.5, 254 * 0.25+0.5, 254 * 0.5+0.5, 254 * 0.8+0.5, 254 * 1 };

CDialogVCA::CDialogVCA(QWidget *parent) :m_Render(NULL), m_curPointVCA(-1), m_preItem(NULL)
, QDialog(parent)
{
	ui.setupUi(this);

	m_PlayWnd = new CWidgetPlayWnd(0, this);
	m_PlayCtrl = new CFormPlayCtrl(0, m_PlayWnd, this);

	m_pDataMgr = new CVCADataMgr();

	m_ZoneCtrl = new CZoneSettingCtl();
	m_ZoneCtrl->Setup(this);//传递了m_pDataMgr

	m_Render = new CCalibrationD3DRender((HWND)m_PlayWnd->GetPlayWndID());//这个放在构造函数UI会Disabled,原因未知。
	m_Render->Setup(NULL, m_pDataMgr);//这里传NUL，因为构造函数已经传了hwnd
	m_PlayCtrl->SetRender(m_Render);

	m_PlayWnd->SetPreViewMode();
	//m_PlayWnd->setGeometry(5, 33, 680, 400);
	m_PlayWnd->setGeometry(150, 33, 570, 500);
	m_PlayCtrl->setGeometry(150, 33 + 500 - 5, 570, 70);
	m_PlayCtrl->HideNotUsedButton();
	m_PlayWnd->show();
	m_PlayCtrl->show();
	m_PlayWnd->lower();
	m_PlayCtrl->lower();
	//	ui.frame_2->lower();
	ui.groupBox->hide();
	ui.tableWidgetRule->hide();
	ui.BtnDel1->hide();
	ui.BtnDelAll1->hide();
	ui.label_11->hide();
	ui.labelPoint->hide();
	ui.BtnColorFromScreen->hide();
	ui.checkBoxColorEnabled->hide();
//	ui.frameColor->hide();
	ui.labelRuleL->hide();
	ui.labelRule->hide();

	ui.tableWidgetRule->setColumnWidth(0, 50);
	ui.tableWidgetRule->setColumnWidth(1, 150);

	ui.tableWidgetFileVCA->hide();

	ui.BtnCaseAdd->hide();

	ui.labelColor->setAutoFillBackground(true);
	//QPalette pl = ui.labelColor->palette();
	//设置颜色
	//pl.setColor(QPalette::Background, QColor(Qt::black));
	//ui.labelColor->setPalette(pl);
	

	UpdateTreeWidget();
	connect(ui.comboBoxColor, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentIndexChanged(int)));
	connect(ui.BtnCaseAdd, SIGNAL(clicked()), this, SLOT(OnBtnCaseAddClick()));
	connect(ui.BtnCasePointAdd, SIGNAL(clicked()), this, SLOT(OnBtnCasePointAddClick()));
	connect(ui.BtnStart, SIGNAL(clicked()), this, SLOT(OnBtnStartClick()));
	connect(ui.BtnStartColor, SIGNAL(clicked()), this, SLOT(OnBtnStartColorClick()));
	connect(ui.BtnRect, SIGNAL(clicked()), this, SLOT(OnBtnRectClick()));
	connect(ui.BtnLine, SIGNAL(clicked()), this, SLOT(OnBtnLineClick()));
	connect(ui.BtnLine2, SIGNAL(clicked()), this, SLOT(OnBtnLine2Click()));
	connect(ui.BtnDel, SIGNAL(clicked()), this, SLOT(OnBtnDelClick()));
	connect(ui.BtnDelAll, SIGNAL(clicked()), this, SLOT(OnBtnDelAllClick()));
	connect(ui.BtnFileAdd, SIGNAL(clicked()), this, SLOT(OnBtnFileAddClick()));
	connect(ui.BtnFileDel, SIGNAL(clicked()), this, SLOT(OnBtnFileDelClick()));
	connect(ui.BtnFileDelAll, SIGNAL(clicked()), this, SLOT(OnBtnFileDelAllClick()));
	connect(ui.BtnColorMore, SIGNAL(clicked()), this, SLOT(OnBtnColorMoreClick()));
	connect(ui.BtnColorFromScreen, SIGNAL(clicked()), this, SLOT(OnBtnColorFromScreenClick()));
	connect(ui.checkBoxColorEnabled, SIGNAL(stateChanged(int)), this, SLOT(OnCheckColorEnabledChanged(int)));

	connect(ui.treeWidgetFileList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnTreeWidgetItemDbClick(QTreeWidgetItem*, int)));
	connect(ui.treeWidgetFileList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(OnTreeItemChanged(QTreeWidgetItem*, int)));
	
	ui.comboBoxColor->setCurrentIndex(2);
}

CDialogVCA::~CDialogVCA()
{
	delete m_PlayWnd;
	delete m_PlayCtrl;
	if (m_Render)
	{
		delete m_Render;
	}
}
/*
void CDialogVCA::OnBtnMinClick()
{
	this->showMinimized();
}*/

void CDialogVCA::Reset()
{
	m_PlayCtrl->Stop();
	ui.frameRule->setEnabled(false);
	ui.labelPoint->setText(QStringLiteral("无"));
	ui.labelRule->setText(QStringLiteral("无"));
	m_listRule.clear();
	m_ZoneCtrl->RemoveZones();
	m_ZoneCtrl->Reset();
	if (m_preItem)
	{
		m_preItem->setBackgroundColor(0, QColor(106, 106, 106));
	}
	m_preItem = NULL;
}

void CDialogVCA::OnVCAFinish()
{
	if (!m_Render->GetZonesVisible())//说明是颜色的
	{
		m_listRule.clear();
		m_ZoneCtrl->RemoveZones();
		m_ZoneCtrl->Reset();
		m_Render->SetZonesVisible(true);
	}
	this->setEnabled(true);
}

void CDialogVCA::customEvent(QEvent * event)
{
	if (/*event->type() == CUSTOM_AutoStop_EVENT || */event->type() ==CUSTOM_ManualStop_EVENT)
	{
		Reset();
	}
	else if (event->type() == CUSTOM_AutoStop_EVENT )
	{
		m_PlayCtrl->SetPlayPos(0.0);
	}
}

void  CDialogVCA::UpdateTreeWidget()
{
	QTreeWidgetItem *groupCasePoint = NULL;
	int size = CDialogVCAMain::ListCasePoint.size();
	for (int i = 0; i < size; i++)
	{
		QString strCasePointName = CDialogVCAMain::ListCasePoint.at(i).strCasePointName;
		groupCasePoint = new QTreeWidgetItem(ui.treeWidgetFileList);
		groupCasePoint->setText(0, strCasePointName);
		groupCasePoint->setToolTip(0, strCasePointName);
		groupCasePoint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		groupCasePoint->setCheckState(0, Qt::Unchecked);

		int fileSize = CDialogVCAMain::ListCasePoint.at(i).FileList.size();
		for (int j = 0; j < fileSize; j++)
		{
			InsertFileItem(CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strFileName, groupCasePoint);
		}
	}
	ui.treeWidgetFileList->expandAll();
}

void  CDialogVCA::OnTreeItemChanged(QTreeWidgetItem *item, int column)
{
	if (item->childCount())
	{
		if (item->checkState(0) == Qt::PartiallyChecked)
		{
			return;
		}
		for (int i = 0; i < item->childCount(); i++)
		{
			item->child(i)->setCheckState(0, item->checkState(0));
			OnTreeItemChanged(item->child(i), 0);  //递归改变父节点下的子节点check state
		}
	}
	else
	{
		QTreeWidgetItem* parent = item->parent();
		while (parent != NULL)
		{
			if (item->checkState(0) != parent->checkState(0))
			{
				int ncheck = 0;
				for (int i = 0; i < parent->childCount(); i++)
				{
					if (parent->child(i)->checkState(0) == Qt::Checked)
					{
						++ncheck;  //统计父节点下子节点勾选个数
					}
				}
				if (ncheck == parent->childCount())
					parent->setCheckState(0, Qt::Checked);
				else if (ncheck == 0)
					parent->setCheckState(0, Qt::Unchecked);
				else
					parent->setCheckState(0, Qt::PartiallyChecked);
			}
			parent = parent->parent();
		}
	}
}

HWND CDialogVCA::GetRenderHwnd()
{
	return (HWND)m_PlayWnd->GetPlayWndID();
}

CVCADataMgr* CDialogVCA::GetVCADataMgr()
{
	return m_pDataMgr;
}


void CDialogVCA::OnBtnCaseAddClick()
{
	
}

void CDialogVCA::OnBtnCasePointAddClick()
{
	CDialogCaseAdd dlg(this);
	dlg.SetCasePointAddMode();
	if (QDialog::Accepted == dlg.exec())
	{
		QString strCaseName, strCasePointName;
		QStringList fileList;
		dlg.GetCaseFileList(strCaseName, strCasePointName, fileList);

		QTreeWidgetItem * group = new QTreeWidgetItem(ui.treeWidgetFileList);
		group->setText(0, strCasePointName);
		group->setToolTip(0, strCasePointName);
		group->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		group->setCheckState(0, Qt::Unchecked);

		T_CasePoint casePoint;
		casePoint.strCasePointName = strCasePointName;

		int size = fileList.size();
		for (int i = 0; i < size; i++)
		{
			T_FileVCA tFileVCA;
			tFileVCA.strFileName = fileList.at(i);
			casePoint.FileList.append(tFileVCA);
			InsertFileItem(fileList.at(i), group);				
		}
		CDialogVCAMain::ListCasePoint.append(casePoint);
		CDialogVCAMain::WritePlayFileListVCA();
	}
}

void CDialogVCA::InsertFileItem(QString strPathName, QTreeWidgetItem *pParent)
{
	strPathName.replace("\\", "/");
	int index = strPathName.lastIndexOf("/");
	QString strName = strPathName.mid(index + 1);

	QTreeWidgetItem * fileItem = new QTreeWidgetItem(pParent);
	fileItem->setText(0, strName);
	fileItem->setToolTip(0, strPathName);
	fileItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	fileItem->setCheckState(0, Qt::Unchecked);
}

void  CDialogVCA::OnBtnFileAddClick()
{
	QTreeWidgetItem *pItem=ui.treeWidgetFileList->currentItem();
	if (!pItem)
	{
	//	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择一个监控点！"));
		CDialogMessageBox dlg;
		dlg.SetText2(QStringLiteral("请选择一个监控点！"));
		dlg.SetText1("");
		dlg.exec();
		return;
	}
	QStringList fileList = QFileDialog::getOpenFileNames(this,
		QStringLiteral("请选择文件"), "", FileFilter);

	int size = fileList.size();
	if (size != 0)
	{
		QTreeWidgetItem *pTopItem = pItem;
		if (pItem->parent())
		{
			pTopItem = pItem->parent();
		}
		int casePointIndex = ui.treeWidgetFileList->indexOfTopLevelItem(pTopItem);
		for (int i = 0; i < size; i++)
		{
			T_FileVCA tFileVCA;
			tFileVCA.strFileName = fileList.at(i);
			CDialogVCAMain::ListCasePoint[casePointIndex].FileList.append(tFileVCA);
			InsertFileItem(fileList.at(i), pTopItem);
		}
	//	pTopItem->setExpanded(true);
		ui.treeWidgetFileList->expandAll();
		CDialogVCAMain::WritePlayFileListVCA();
	}
}

void  CDialogVCA::OnBtnFileDelClick()
{
	QTreeWidgetItem *pItem = ui.treeWidgetFileList->currentItem();

	QTreeWidgetItem *pTopItem = pItem;
	if (pItem->parent())
	{
		pTopItem = pItem->parent();
		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("确定要删除此视频？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return;
		}
		
		int index=pTopItem->indexOfChild(pItem);
		int casePointIndex = ui.treeWidgetFileList->indexOfTopLevelItem(pTopItem);
		if (CDialogVCAMain::ListCasePoint[casePointIndex].FileList[index].strVCAResultDataList.size() > 0)
		{
		//	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先清除此视频的检索结果！"));
			CDialogMessageBox dlg;
			dlg.SetText2(QStringLiteral("请先清除此视频的检索结果！"));
			dlg.SetText1("");
			dlg.exec();
			return;
		}
		CDialogVCAMain::ListCasePoint[casePointIndex].FileList.removeAt(index);
		if (m_preItem&&m_preItem == pItem)
		{
			Reset();
		}
		pTopItem->takeChild(index);
	}
	else
	{

		if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("确定要删除此监控点？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		{
			return;
		}
		int casePointIndex = ui.treeWidgetFileList->indexOfTopLevelItem(pTopItem);
		int size = CDialogVCAMain::ListCasePoint[casePointIndex].FileList.size();
		for (int j = 0; j<size; j++)
		{
			if (CDialogVCAMain::ListCasePoint[casePointIndex].FileList[j].strVCAResultDataList.size() > 0)
			{
			//	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先清除此监控点的所有检索结果！"));
				CDialogMessageBox dlg;
				dlg.SetText2(QStringLiteral("请先清除此监控点的所有检索结果！"));
				dlg.SetText1("");
				dlg.exec();
				return;
			}
		}
		if (m_preItem&&m_preItem->parent() == pItem)
		{
			Reset();
		}
		CDialogVCAMain::ListCasePoint.removeAt(casePointIndex);
		ui.treeWidgetFileList->takeTopLevelItem(casePointIndex);
	}

	CDialogVCAMain::WritePlayFileListVCA();
}

void  CDialogVCA::OnBtnFileDelAllClick()
{
	if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
		QStringLiteral("是否要清空所有？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
	{
		return;
	}

	int size = CDialogVCAMain::ListCasePoint.size();
	for (int i = 0; i < size; i++)
	{
		int fileSize = CDialogVCAMain::ListCasePoint.at(i).FileList.size();
		for (int j = 0; j < fileSize; j++)
		{
			if (CDialogVCAMain::ListCasePoint.at(i).FileList.at(j).strVCAResultDataList.size()>0)
			{
			//	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请清除所有检索结果！"));
				CDialogMessageBox dlg;
				dlg.SetText2(QStringLiteral("请清除所有检索结果！"));
				dlg.SetText1("");
				dlg.exec();
				return;
			}
		}
	}
	Reset();
	ui.treeWidgetFileList->clear();
	CDialogVCAMain::ListCasePoint.clear();
	CDialogVCAMain::WritePlayFileListVCA();
}

void CDialogVCA::OnTreeWidgetItemDbClick(QTreeWidgetItem * item, int column)
{
	if (item->parent()&&column==0)
	{
		if (m_listRule.size() > 0)
		{
			if (QMessageBox::No == QMessageBox::question(this, QStringLiteral("提示"),
				QStringLiteral("确定要退出此视频并清除规则？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
			{
				return;
			}
			m_listRule.clear();
		}

		if (m_preItem)
		{
			m_preItem->setBackgroundColor(0, QColor(106, 106, 106));
		}
		Reset();
		item->setBackgroundColor(0, QColor(182, 0, 0));
		m_preItem = item;

		QString strFileName=item->toolTip(column);

		bool bRet = m_PlayCtrl->OpenAndPlayFile(strFileName, NULL);
		
		this->setEnabled(true);
		if (!bRet)
		{
			m_PlayCtrl->Stop();
			CDialogMessageBox dlg;
			QFile file(strFileName);
			if (!file.exists())
			{
				dlg.SetText1(QStringLiteral("文件路径不存在！"));

			}
			else
			{
				dlg.SetText1(QStringLiteral("无法播放！文件可能被损坏！"));
			}
			dlg.SetText2(strFileName);
			dlg.exec();
			return;
		}
	//	m_PlayCtrl->GetPicSize((long*)&m_nWidth, (long*)&m_nHeight);
		

		if (!m_PlayCtrl->IsYUVCallback())//播放后判断是否已设置YUV回调
		{
			m_PlayCtrl->SetDisplayCallback();
			if (!m_PlayCtrl->IsYUVCallback())//不支持YUV回调，则不支持检索
			{
				//QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("此文件不支持检索！"));
				m_PlayCtrl->Stop();
				CDialogMessageBox dlg;
				dlg.SetText2(QStringLiteral("此文件不支持检索！"));
				dlg.SetText1("");
				dlg.exec();
				return;
			}
		}
		m_curPointVCA = ui.treeWidgetFileList->indexOfTopLevelItem(item->parent());
		item->setCheckState(0,Qt::Checked);
		ui.frameRule->setEnabled(true);
		ui.labelPoint->setText(item->parent()->text(0));
	//	Qt::ItemFlags flags = item->flags();
		//item->setFlags(flags&~Qt::ItemIsEnabled);
		
		m_PlayWnd->HideToolBar();
		m_PlayCtrl->SetRenderMode(RenderStrech);//不用设置为原比例模式，铺满窗口，DemoVCA是铺满窗口
		m_PlayWnd->SetZoneCtrl(m_ZoneCtrl);
		RECT rc;
		QRect rc0 = m_PlayWnd->GetRenderClientRect();
		rc.left = 0;
		rc.top = 0;
		rc.right = rc0.width();
		rc.bottom = rc0.height();
		m_Render->OnChangeClientRect(rc);
		m_ZoneCtrl->OnChangeClientRect(rc);
		
	}
}

void CDialogVCA::OnBtnStartClick()
{

	//或者color
	//
	//m_listRule.size();	
	if (m_listRule.size() <= 0)
	{
		CDialogMessageBox dlg;
		dlg.SetText2(QStringLiteral("请设置检索规则！"));
		dlg.SetText1("");
		dlg.exec();
		return;
	}
	Start();
}

bool CDialogVCA::Start(bool bZoneShow)
{
	if (!CSimpleVCA::IsSetup())
	{
		CDialogMessageBox dlg;
		dlg.SetText2(QStringLiteral("初始化失败不能检索！"));
		dlg.SetText1("");
		dlg.exec();		
		return false;
	}

	QList<T_FileChecked> FileCheckedList;

	QTreeWidgetItem* pItem = ui.treeWidgetFileList->topLevelItem(m_curPointVCA);
	if (pItem&&pItem->checkState(0) != Qt::Unchecked)
	{
		int count = pItem->childCount();
		for (int i = 0; i < count; i++)
		{
			QTreeWidgetItem* pItemChild = pItem->child(i);
			if (pItemChild->checkState(0) == Qt::Checked)
			{
				T_FileChecked tFileChecked;
				tFileChecked.index = i;
				tFileChecked.strFileName = CDialogVCAMain::ListCasePoint.at(m_curPointVCA).FileList.at(i).strFileName;// pItemChild->toolTip(0);
				tFileChecked.bFinish = false;
				FileCheckedList.append(tFileChecked);
			}
		}
	}
	
	int size = FileCheckedList.size();
	if (size <= 0)
	{
		CDialogMessageBox dlg;
		dlg.SetText2(QStringLiteral("请选择要检索的文件！"));
		dlg.SetText1("");
		dlg.exec();
		return false;
	}
	//开始检索
	int nWidth = 0;
	int nHeight = 0;
	m_PlayCtrl->GetPicSize((long*)&nWidth, (long*)&nHeight);	
	
	m_widgetProgress->StartFileListVCA(m_curPointVCA, FileCheckedList, nWidth, nHeight,bZoneShow);
	QTabWidget *tabWidget = (QTabWidget*)this->parentWidget()->parentWidget();
	tabWidget->setCurrentIndex(1);

	return true;
	
}
/*
void CDialogVCA::OnBtnCloseClick()
{
	m_PlayCtrl->Stop();
	m_VCAObject->VCAClose();


	m_listRule.clear();
	ui.tableWidgetFileVCA->clear();
	ui.tableWidgetRule->clear();
	m_curTask = -1;
	this->hide();
	//this->reject();
}
*/
void  CDialogVCA::InsertRuleTable(QString &str)
{
/*	int rowCount = ui.tableWidgetRule->rowCount();
	ui.tableWidgetRule->insertRow(rowCount);
	QTableWidgetItem *pItem = new QTableWidgetItem();
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(QString("%1").arg(rowCount + 1));
	ui.tableWidgetRule->setItem(rowCount, 0, pItem);

	pItem = new QTableWidgetItem();
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(str);
	ui.tableWidgetRule->setItem(rowCount, 1, pItem);*/
}

void  CDialogVCA::OnBtnRectClick()
{
	m_ZoneCtrl->AddZone(true);

	QString ruleName = QStringLiteral("区域");
	m_listRule.append(ruleName);
	//InsertRuleTable(ruleName);
	UpdateVCARuleText();
}

void  CDialogVCA::OnBtnLineClick()
{
	m_ZoneCtrl->AddZone(false,true);

	QString ruleName = QStringLiteral("单向跨线");
	m_listRule.append(ruleName);
//	InsertRuleTable(ruleName);
//	UpdateVCARuleText();
}

void  CDialogVCA::OnBtnLine2Click()
{
	m_ZoneCtrl->AddZone(false);

	QString ruleName = QStringLiteral("双向跨线");
	m_listRule.append(ruleName);
	//InsertRuleTable(ruleName);
//	UpdateVCARuleText();
}

void  CDialogVCA::UpdateVCARuleText()
{
	VCA5_APP_ZONES zones;
	VCA5_APP_COUNTERS counters;
	VCA5_APP_RULES rules;
	VCA5_APP_CLSOBJECTS clsObjs;
	VCA5_APP_CALIB_INFO calibInfo;

	m_pDataMgr->GetData(&zones, &rules, &counters, &calibInfo, &clsObjs);

	//Counterline rule setting
	/*
	for (ULONG i = 0; i < rules.ulTotalRules; ++i){
	if (VCA5_RULE_TYPE_LINECOUNTER_A == rules.pRules[i].usRuleType ||
	VCA5_RULE_TYPE_LINECOUNTER_B == rules.pRules[i].usRuleType){
	if (rules.pRules[i].ucWidthCalibrationEnabled == 0){
	rules.pRules[i].tRuleDataEx.tLineCounter.ulCalibrationWidth = 0;
	}
	rules.pRules[i].tRuleDataEx.tLineCounter.ulShadowFilterEnabled = rules.pRules[i].ucShadowFilterEnabled;
	}
	}*/

	int rectCount = 0;
	QString strLine;
	int singleLinetCount = 0;
	int twoLineCount = 0;
	for (int i = 0; i < zones.ulTotalZones; i++)
	{
		if (zones.pZones[i].usZoneStyle == VCA5_ZONE_STYLE_POLYGON)
		{
			rectCount++;
		}
		else if (zones.pZones[i].usZoneStyle == VCA5_ZONE_STYLE_TRIPWIRE)
		{
			//singleLinetCount++;
			
			for (int j = 0; j < rules.ulTotalRules; j++)
			{
				
				if (rules.pRules[j].usRuleType== VCA5_RULE_TYPE_LINECOUNTER_A)
				{
						singleLinetCount++;
				}
				else if(zones.pZones[i].usZoneType == VCA5_RULE_TYPE_LINECOUNTER_A)
				{
					//	twoLineCount++;
				}
			}
		}
	}
	QString strRect;
	if (rectCount != 0)
	{
		strRect= QString(QStringLiteral("+%1区域")).arg(rectCount);
	}
	QString strSingLine;
	if (singleLinetCount != 0)
	{
		strSingLine=QString(QStringLiteral("+%1单向跨线")).arg(singleLinetCount);
	}
	QString strTwoLine;
	if (twoLineCount != 0)
	{
		strTwoLine = QString(QStringLiteral("+%1双向跨线")).arg(twoLineCount);
	}
	QString strVCARule = QStringLiteral("所有目标") + strRect + strSingLine + strTwoLine;
	if (rectCount == 0 && singleLinetCount == 0 && twoLineCount == 0)
	{
		ui.labelRule->setText(QStringLiteral("无"));
	}
	else
	{
		ui.labelRule->setText(strVCARule);
	}

}

void  CDialogVCA::OnBtnDelClick()
{
	int ruleIndex = m_ZoneCtrl->GetCurActRule();
	if (NOT_IN_USE != ruleIndex)//说明选中了
	{
		m_listRule.removeAt(ruleIndex);
	//	ui.tableWidgetRule->removeRow(ruleIndex);
	}

	m_ZoneCtrl->RemoveZone();
	//UpdateVCARuleText();
}

void  CDialogVCA::OnBtnDelAllClick()
{
	m_ZoneCtrl->RemoveZones();
	//UpdateVCARuleText();
}

void CDialogVCA::OnBtnColorFromScreenClick()
{

}

void CDialogVCA::OnBtnColorMoreClick()
{
	QColorDialog dlg;
	dlg.exec();
}

void CDialogVCA::OnCheckColorEnabledChanged(int state)
{
	if (state == Qt::Checked)
	{
		//ui.frameColor->setEnabled(true);
	}
	else
	{
		//ui.frameColor->setEnabled(false);
	}
}


void  CDialogVCA::OnBtnStartColorClick()
{
	m_listRule.clear();
	m_ZoneCtrl->RemoveZones();
	m_ZoneCtrl->Reset();

	m_ZoneCtrl->AddZone(true, false, true);
	m_Render->SetZonesVisible(false);

	int nZoneID = m_ZoneCtrl->GetCurActRule();
	VCA5_APP_RULE *pRule = m_pDataMgr->GetRule(nZoneID, VCA5_RULE_TYPE_COLSIG);

	int usColBin = ui.comboBoxColor->currentIndex();
	int usThreshold = g_usThreshold[ui.comboBoxPercen->currentIndex()];
	pRule->tRuleDataEx.tColorFilter.usColBin = usColBin;
	pRule->tRuleDataEx.tColorFilter.usThreshold = usThreshold;

	/*pRule->tRuleDataEx.tSpeed.usSpeedLo = usColBin;
	pRule->tRuleDataEx.tSpeed.usSpeedUp = usThreshold;

	pRule->tRuleDataEx.tArea.usAreaLo = usColBin;
	pRule->tRuleDataEx.tArea.usAreaUp = usThreshold;

	pRule->tRuleDataEx.tDirection.sStartAngle = usColBin;
	pRule->tRuleDataEx.tDirection.sFinishAngle = usThreshold;
	
		pRule->tRuleDataEx.tTimer.ulTimeThreshold = 1638403;
		pRule->tRuleDataEx.tLineCounter.ulCalibrationWidth = 1638403;*/


	bool bRet = Start(false);
	if (!bRet)
	{
		m_listRule.clear();
		m_ZoneCtrl->RemoveZones();
		m_ZoneCtrl->Reset();
		m_Render->SetZonesVisible(true);
	}
}

void  CDialogVCA::OnCurrentIndexChanged(int index)
{
	QPalette pl = ui.labelColor->palette();
	//设置颜色
	pl.setColor(QPalette::Background, ui.comboBoxColor->color());
	ui.labelColor->setPalette(pl);
}