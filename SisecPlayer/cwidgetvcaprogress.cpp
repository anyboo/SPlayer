#include "stdafx.h"
#include "cwidgetvcaprogress.h"
#include <QDir>
#include "cdialogvcamain.h"
#include "cdialogconfig.h"

extern CDialogVCAMain *pMainDlg;


typedef enum{
	YUV_NONE,
	YUY2,
	UYVY,
	YV12,
	GRAY,
}YUVTYPE;

typedef struct
{
	DWORD	type;
	DWORD	width;
	DWORD	height;
}YUVHEADER;

//#define YUVWRITE
#ifdef YUVWRITE
static FILE *fd = NULL;
#endif

CWidgetVCAProgress::CWidgetVCAProgress(CVCADataMgr	*pDataMgr,QWidget *parent)
: QWidget(parent), m_pDataMgr(pDataMgr), m_nWidth(0), m_nHeight(0), m_totalEventCount(0),
m_curPointVCA(-1), m_VCAState(VCANoStart),m_bPause(false)
{
	ui.setupUi(this);


	m_PlayWndVCA = new CWidgetPlayWnd(1, this);
	m_PlayCtrlVCA = new CFormPlayCtrl(1, m_PlayWndVCA, this);

	m_PlayWndVCA->SetPreViewMode();
	m_PlayWndVCA->setGeometry(150, 5,400, 300);
	m_PlayWndVCA->show();

	m_PlayCtrlVCA->hide();

	ui.tableWidgetFileVCA->setColumnWidth(0, 50);
	ui.tableWidgetFileVCA->setColumnWidth(1, 180);
	//ui.tableWidgetFileVCA->setColumnWidth(1, 220);
	//ui.tableWidgetFileVCA->setColumnWidth(VCAResult, 160);

	m_pJpegCodec = new CJpegCodec;

	m_VCAObject = new CSimpleVCA();
	m_RenderVCA = new CCalibrationD3DRender((HWND)m_PlayWndVCA->GetPlayWndID());//这个放在构造函数UI会Disabled,原因未知。
	m_RenderVCA->Setup(NULL, m_pDataMgr);//这里传NUL，因为构造函数已经传了hwnd
	m_RenderVCA->SetColorFormat(CD3d::CF_YV12);

	m_PlayWndVCA->hide();
	//m_VCAObject->SetRender(m_RenderVCA);
	if (CDialogConfig::GetVCARenderVisible())
	{
		ui.checkBoxRenderShow->show();
	}
	else
	{
		ui.checkBoxRenderShow->hide();
		
	}
	ui.labelInform->hide();
	ui.tableWidgetFileVCA->setGeometry(0, 30, 937, 571);

	m_VCAObject->SetDataMgr(m_pDataMgr);
	m_VCAObject->SetEventFilterCallbck((DisplayCallback)&OnEventFilterCallBack, (long)this);
	m_PlayCtrlVCA->SetVCAObject(m_VCAObject);

	ui.BtnStart->setEnabled(false);
	ui.BtnStop->setEnabled(false);

	m_timerGetVCATime = new QTimer(); 
	connect(m_timerGetVCATime, SIGNAL(timeout()), this, SLOT(OnGetVCATime()));
	connect(ui.BtnStart, SIGNAL(clicked()), this, SLOT(OnBtnStartClick()));
	connect(ui.BtnStop, SIGNAL(clicked()), this, SLOT(OnBtnStopClick()));
	connect(ui.checkBoxRenderShow, SIGNAL(stateChanged(int)), this, SLOT(OnCheckRenderShowChanged(int)));
	connect(this, SIGNAL(UpdateEventCount()), this, SLOT(OnUpdateEventCount()));
	connect(this, SIGNAL(GetCurPlayTime()), this, SLOT(OnGetCurPlayTime()));
}

CWidgetVCAProgress::~CWidgetVCAProgress()
{

}

void CWidgetVCAProgress::OnCheckRenderShowChanged(int state)
{
	if (state == Qt::Checked)
	{
		m_VCAObject->SetRender(m_RenderVCA);
		m_PlayWndVCA->show();
		ui.tableWidgetFileVCA->setGeometry(0, 308, 937, 288);
		ui.labelInform->show();
	}
	else
	{
		m_VCAObject->SetRender(NULL);
		m_PlayWndVCA->hide();
		ui.labelInform->hide();
		ui.tableWidgetFileVCA->setGeometry(0, 30, 937, 571);
	}

}

void CWidgetVCAProgress::OnGetVCATime()
{
	/*
	unsigned long long FrameCount = 0;
	unsigned long long VCATotalTime = 0;
	m_VCAObject->GetVCATime(FrameCount, VCATotalTime);

	ui.labelFrameCount->setText(QString("%1").arg(FrameCount));
	ui.labelVCATotalTime->setText(QString("%1 ms").arg(VCATotalTime));
	if (FrameCount != 0)
		ui.labelDifTime->setText(QString("%1 ms").arg(VCATotalTime / FrameCount));
	*/
	unsigned long long totalTime = 0;
	m_VCAObject->GetTotalTime(totalTime);
	char buf[32];
	//int ms = totalTime % 1000;
	totalTime /= 1000;
	int hour = totalTime / 3600;
	int min = (totalTime - hour * 3600) / 60;
	int sec = totalTime - hour * 3600 - min * 60;

	sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
	QString strTime = QString("%1").arg(buf);
	//ui.labelTotalTime->setText(strTime);
	QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCATime);
	pItem->setText(strTime);

//	int ulTotalEvents = m_VCAObject->GetMetaDataParser()->GetEvents()->ulTotalEvents;
	//pItem = ui.tableWidgetFileVCA->item(m_curTask, EventCount);
	//pItem->setText(QString("%1").arg(m_totalEventCount));

/*	m_VCAObject->GetTotalTime2(totalTime);
	ms = totalTime % 1000;
	totalTime /= 1000;
	hour = totalTime / 3600;
	min = (totalTime - hour * 3600) / 60;
	sec = totalTime - hour * 3600 - min * 60;
	//	ui.labelTotalTimeNotDis->setText(QString("%1 s").arg(totalTime / 1000));
	sprintf(buf, "%02d:%02d:%02d:%03d", hour, min, sec, ms);
	strTime = QString("%1").arg(buf);
//	ui.labelTotalTimeNotDis->setText(strTime);*/
	
	long width = 0;
	long height = 0;
	m_PlayCtrlVCA->GetPicSize(&width, &height);
	pItem = ui.tableWidgetFileVCA->item(m_curTask, FileWidthHeightVCA);
	pItem->setText(QString("%1*%2").arg(width).arg(height));

	totalTime = m_PlayCtrlVCA->GetFileTime();
	hour = totalTime / 3600;
	min = (totalTime - hour * 3600) / 60;
	sec = totalTime - hour * 3600 - min * 60;
	sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
	strTime = QString("%1").arg(buf);
	pItem = ui.tableWidgetFileVCA->item(m_curTask, FileDuationVCA);
	pItem->setText(QString("%1").arg(buf));

	DWORD dCurrentTime = m_PlayCtrlVCA->GetCurPlayTime();
	hour = dCurrentTime / 3600;
	min = (dCurrentTime - hour * 3600) / 60;
	sec = dCurrentTime - hour * 3600 - min * 60;
	sprintf(buf, "%02d_%02d_%02d", hour, min, sec);
	m_strPicTime = QString("%1").arg(buf);

	int nPos = m_PlayCtrlVCA->GetPlayPos() * 100;
	nPos = nPos > 100 ? 100 : nPos;
	pItem = ui.tableWidgetFileVCA->item(m_curTask, ProgressVCA);
	pItem->setText(QString("%1\%").arg(nPos));

	if (m_VCAState==VCAPause)
	{
		pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
		pItem->setText(QStringLiteral("暂停"));
		m_PlayCtrlVCA->Pause(1);
	}
	else if (m_VCAState == VCAPlay)
	{
		m_PlayCtrlVCA->Pause(0);
	}
	else if (m_VCAState == VCAStop)
	{
		m_timerGetVCATime->stop();
		m_PlayCtrlVCA->Stop();
		Sleep(10);
		ClearTableFileVCA();
		m_listAlarmID.clear();
		CWidgetVCAResult::DeleteDirectory(m_strVCADataPath);
		pMainDlg->m_dlgVCA->OnVCAFinish();
	}
}

void CWidgetVCAProgress::OnEventFilterCallBack(DISPLAYCALLBCK_INFO *pInfo)
{
	//有事件通知，截图保存
	CWidgetVCAProgress *pDlg = (CWidgetVCAProgress*)pInfo->nUser;
	if (pDlg)
	{
#ifdef YUVWRITE
		if (fd == NULL)
		{
			fd=fopen("color.raw", "wb");
			YUVHEADER header;
			header.type = YV12;
			header.width = pInfo->nWidth;
			header.height = pInfo->nHeight;
			fwrite(&header, sizeof(YUVHEADER), 1, fd);
		}
		if (fd)
		{
			fwrite(pInfo->pBuf, 1, pInfo->nWidth*pInfo->nHeight*3/2, fd);
			
		}
#endif
		pDlg->EventFilterProcess(pInfo);
	}
}

void CWidgetVCAProgress::OnGetCurPlayTime()
{
	DWORD dCurrentTime = m_PlayCtrlVCA->GetCurPlayTime();
	int hour = dCurrentTime / 3600;
	int min = (dCurrentTime - hour * 3600) / 60;
	int sec = dCurrentTime - hour * 3600 - min * 60;

	char buf[32];
	sprintf(buf, "%02d_%02d_%02d", hour, min, sec);
	QString strPicTime = QString("%1").arg(buf); 
}

void CWidgetVCAProgress::EventFilterProcess(DISPLAYCALLBCK_INFO *pInfo)
{
	CVCAMetaLib *pVCAMetaLib = m_VCAObject->GetMetaDataParser();
	
	VCA5_PACKET_EVENT *pEvent = NULL;
	VCA5_PACKET_OBJECT *pObject = NULL;
	VCA5_RULE *pRule = NULL;
	VCA5_APP_ZONE *pZone = NULL;
	VCA5_PACKET_EVENTS *pPacketEvents = pVCAMetaLib->GetEvents();
	VCA5_PACKET_OBJECTS *pPacketObjects = pVCAMetaLib->GetObjects();
	int ulTotalEvents = pVCAMetaLib->GetEvents()->ulTotalEvents;
	
	int dCurrentTime = m_PlayCtrlVCA->GetCurPlayTime();
	int dCurrentTime = pInfo->nStamp;
	
	int hour = dCurrentTime / 3600;
	int min = (dCurrentTime - hour * 3600) / 60;
	int sec = dCurrentTime - hour * 3600 - min * 60;

	char buf[32];
	sprintf(buf, "%02d_%02d_%02d", hour, min, sec);
	QString strPicTime = QString("%1").arg(buf);
//	int dCurrentTime = 0;
	//emit GetCurPlayTime();
	//QString strPicTime = m_strPicTime;
	

	QString strPicPathName = m_strVCADataPath;
	strPicPathName.append("/" + strPicTime + ".jpg");
	//m_PlayCtrlVCA->CapturePic(strPicPathName, 1);
	BITMAPINFOHEADER bm;
	bm.biWidth = pInfo->nWidth;
	bm.biHeight = pInfo->nHeight;
	//	bm.biCompression = mmioFOURCC('Y', 'V', '1', '2');
	bm.biCompression = mmioFOURCC('Y', 'V', '1', '2');
	
	bool bNew = false;
	for (int i = 0; i < ulTotalEvents; i++)
	{
		pEvent = &pPacketEvents->Events[i];
		
		switch (pEvent->ulStatus)
		{
			case VCA5_EVENT_STATUS_INSTANT:// 跨线等于瞬间出现
			case VCA5_EVENT_STATUS_START:
				for (DWORD j = 0; j < pPacketObjects->ulTotalObject; j++)
				{
					if (pPacketObjects->Objects[j].ulId == pEvent->ulObjId)
					{
						pObject = &pPacketObjects->Objects[j];
					}
				}	
				if (!pObject)
				{
					for (DWORD j = 0; j < m_PrePacketObjects.ulTotalObject; j++)
					{
						if (m_PrePacketObjects.Objects[j].ulId == pEvent->ulObjId)
						{
							pObject = &m_PrePacketObjects.Objects[j];
						}
					}
				}
			/*	if (!pObject)
				{
					continue;
				}*/
				pZone = m_pDataMgr->GetZoneById(pEvent->ulZoneId);
				
				if (pZone->usZoneStyle == VCA5_ZONE_STYLE_POLYGON)
				{
					if (!m_listAlarmID.contains(pEvent->ulId))
					{	
						QFile file(strPicPathName);
						if (file.exists())
						{
							QString strJpg = QString("_(%1).jpg").arg(pEvent->ulId);
							strPicPathName = m_strVCADataPath + "/" + strPicTime + strJpg;
						}
						m_pJpegCodec->Encode((LPCTSTR)strPicPathName.toStdWString().c_str(), (BYTE*)pInfo->pBuf, &bm, dCurrentTime, pZone, pRule, pObject);
						m_listAlarmID.append(pEvent->ulId);
						m_totalEventCount++;
						bNew = true;
					}
				}
				else if (pZone->usZoneStyle == VCA5_ZONE_STYLE_TRIPWIRE)
				{	
					pRule = m_pDataMgr->GetRuleByRealId(pEvent->ulRuleId);
					
					if (!pRule) 
						continue;

					if (pRule&&pRule->usRuleType == VCA5_RULE_TYPE_DIRECTION || pRule->usRuleType == VCA5_RULE_TYPE_LINECOUNTER_A || pRule->usRuleType == VCA5_RULE_TYPE_LINECOUNTER_B)
					{
						if (!m_listAlarmID.contains(pEvent->ulId))
						{
							QFile file(strPicPathName);
							if (file.exists())
							{
								QString strJpg = QString("_(%1).jpg").arg(pEvent->ulId);
								strPicPathName = m_strVCADataPath + "/" + strPicTime + strJpg;
							}

							m_pJpegCodec->Encode((LPCTSTR)strPicPathName.toStdWString().c_str(), (BYTE*)pInfo->pBuf, &bm, dCurrentTime, pZone, pRule, pObject);
							m_listAlarmID.append(pEvent->ulId);
							m_totalEventCount++;
							bNew = true;
						}
					}				
				//	pEvent->ulRuleId
				}
				//VCA5_RULE_TYPE_PRESENCE				
				
				break;
			case VCA5_EVENT_STATUS_END:
			//	UpdateAlarm(al);
				break;
			case VCA5_EVENT_STATUS_UPDATE:
			//	UpdateAlarm(al);
				break;
		}
	}

	if (bNew)
	{
		emit UpdateEventCount();
	}

	memcpy(&m_PrePacketObjects, pPacketObjects, sizeof(VCA5_PACKET_OBJECTS));
}

void CWidgetVCAProgress::OnUpdateEventCount()
{
	QTableWidgetItem * pItem = ui.tableWidgetFileVCA->item(m_curTask, EventCount);
	if (pItem)
	{
		pItem->setText(QString("%1").arg(m_totalEventCount));
	}
}

void CWidgetVCAProgress::customEvent(QEvent * event)
{
	if (event->type() == CUSTOM_AutoStop_EVENT)
	{
		CMyEvent *myEvent = (CMyEvent *)event;
		CFormPlayCtrl *pPlayerCtrlVCA = (CFormPlayCtrl*)myEvent->obj;
		int index = pPlayerCtrlVCA->Index();

		if (index == 1)
		{
			if (m_timerGetVCATime->isActive())
			{
				m_timerGetVCATime->stop();
			}
			//收到文件完毕时，VCA还没处理完，sleep后等待处理完
			Sleep(1000);
			if (pPlayerCtrlVCA->IsPlaying())
			{
				pPlayerCtrlVCA->Stop();
			}
			//Sleep(10);
			if (m_curTask < m_FileCheckedList.size())
			{
				QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, ProgressVCA);
				pItem->setText(QString("%1\%").arg(100));

				pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
				pItem->setText(QStringLiteral("完成"));
				m_FileCheckedList[m_curTask].bFinish = true;
				if (m_totalEventCount != 0)
				{
					int indexFile = m_FileCheckedList[m_curTask].index;
					CDialogVCAMain::ListCasePoint[m_curPointVCA].FileList[indexFile].strVCAResultDataList.append(m_strVCADataTimePath);
					CDialogVCAMain::WritePlayFileListVCA();
					CDialogVCAMain::NotifyResult(m_curPointVCA, indexFile);
				}
				else
				{
					CWidgetVCAResult::DeleteDirectory(m_strVCADataPath);//无事件结果清空文件夹
				}
				StartNextFile();
			}
			
		}
		event->accept();
		return;

	}
	QWidget::customEvent(event);
}


void CWidgetVCAProgress::InsertFileListVCA()
{
	ClearTableFileVCA();
	int size = m_FileCheckedList.size();
	for (int i = 0; i < size; i++)
	{
		QString strPathName = m_FileCheckedList.at(i).strFileName;
		strPathName.replace("\\", "/");
		int index = strPathName.lastIndexOf("/");
		//QString strPath = strPathName.left(index);
		QString strName = strPathName.mid(index + 1);

		qint64 iFileSize = 0;
		QFile file(strPathName);
		if (file.open(QIODevice::ReadOnly))
		{
			iFileSize = file.size();
			file.close();
		}

		ui.tableWidgetFileVCA->insertRow(i);

		QTableWidgetItem *pItem = NULL;
		for (int j = 0; j < Column_CountVCA; j++)
		{
			pItem = new QTableWidgetItem();
			pItem->setTextAlignment(Qt::AlignCenter);
			if (j == FileVCA_Head::No)
			{
				pItem->setText(QString("%1").arg(i + 1));
			}
			else if (j == FileNameVCA)
			{
				pItem->setText(strName);
				pItem->setToolTip(strName);
			}
			else if (j == VCAResult)
			{
				pItem->setText(QStringLiteral("等待"));
			}
			else if (j == FileSizeVCA)
			{
				float fG = iFileSize / (float)(1024 * 1024 * 1024);
				float fM = iFileSize / (float)(1024 * 1024);
				char buf[256];
				sprintf_s(buf, "%.2fM", fM);
				QString strFileSize = QString("%1").arg(buf);
				if (fM > 1000.0)
				{
					sprintf_s(buf, "%.2fG", fG);
					strFileSize = QString("%1").arg(buf);
				}
				if (fM < 1.0)
				{
					sprintf_s(buf, "%.2fK", iFileSize / (float)(1024));
					strFileSize = QString("%1").arg(buf);
				}
				pItem->setText(strFileSize);
			}
			ui.tableWidgetFileVCA->setItem(i, j, pItem);
		}
	}
}

void CWidgetVCAProgress::StartFileListVCA(int curCasePoint, QList<T_FileChecked> pPlayFileListVCA, int width, int height,bool bZoneVisible)
{

	m_RenderVCA->SetZonesVisible(bZoneVisible);

	m_curTask = -1;
	m_curPointVCA = curCasePoint;
	m_nWidth = width;
	m_nHeight = height;
	m_FileCheckedList.clear();
	m_FileCheckedList = pPlayFileListVCA;
	InsertFileListVCA();
	ui.BtnStart->setEnabled(true);
	ui.BtnStop->setEnabled(true);
	pMainDlg->m_dlgVCA->setEnabled(false);

	m_VCAState = VCAPlay;
	m_bPause = false;
	StartNextFile();
	
}

void CWidgetVCAProgress::ClearTableFileVCA()
{
	while (int count = ui.tableWidgetFileVCA->rowCount() > 0)
	{
		ui.tableWidgetFileVCA->removeRow(count - 1);
	}
}

void CWidgetVCAProgress::StartNextFile()
{
	if (m_VCAState == VCAPause)
	{
		QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
		pItem->setText(QStringLiteral("暂停"));
		return;
	}
	else if (m_VCAState == VCAStop)
	{
		//不继续下一个播放，清空列表，置初始化值
		ClearTableFileVCA();
		pMainDlg->m_dlgVCA->OnVCAFinish();
		return;
		
	}

	m_curTask++;
	m_VCAObject->Reset();
	m_totalEventCount = 0;
	m_strPicTime = "";
	m_listAlarmID.clear();
	memset(&m_PrePacketObjects, 0, sizeof(VCA5_PACKET_OBJECTS));
	if (m_curTask >= m_FileCheckedList.size())
	{
		//
		//	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("检索完成！"));
		m_VCAState = VCAFinish;
		pMainDlg->m_dlgVCA->OnVCAFinish();
#ifdef YUVWRITE
		if (fd)
		{
			fclose(fd);
		}
#endif
		return;
	}
	
	QString strPathName = m_FileCheckedList.at(m_curTask).strFileName;
	strPathName.replace("\\", "/");
	int index = strPathName.lastIndexOf("/");
	QString strPath = strPathName.left(index);

	QString strName = strPathName.mid(index + 1);
	int zzIndex = strName.indexOf(".");
	QString strDorpSuffixName = strName.left(zzIndex);
	QDateTime dateTime = QDateTime::currentDateTime();
	m_strVCADataTimePath = "/VCAData/" + strDorpSuffixName + "/" + dateTime.toString("yyyy_MM_dd_hh_mm_ss");
	strPath.append(m_strVCADataTimePath);

	m_strVCADataPath = strPath;

	QDir dir(strPath);
	if (!dir.exists())
	{
		if (!dir.mkpath(strPath))
		{
			QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
			pItem->setText(QStringLiteral("创建检索结果文件夹失败！"));
			StartNextFile();
			return;
		}
	}
	//保存规则
	CVCAConfigure *pVcaCfg = CVCAConfigure::Instance();
	VCA5_APP_ENGINE_CONFIG *pEngCfg = pVcaCfg->GetEngineConf(0);

	m_pDataMgr->GetData(&(pEngCfg->Zones), &(pEngCfg->Rules), &(pEngCfg->Counters), &(pEngCfg->CalibInfo), &(pEngCfg->ClsObjects));
	memcpy(&(pEngCfg->AdvInfo), m_pDataMgr->GetAdvancedInfo(), sizeof(VCA5_APP_ADVANCED_INFO));
	memcpy(&(pEngCfg->TamperInfo), m_pDataMgr->GetTamperInfo(), sizeof(VCA5_TAMPER_INFO));
	memcpy(&(pEngCfg->SceneChangeInfo), m_pDataMgr->GetSceneChangeInfo(), sizeof(VCA5_SCENECHANGE_INFO));

	QString strZoneFile = m_strVCADataPath + "/zone.xml";
	pVcaCfg->SaveEngine(0, (LPTSTR)strZoneFile.toStdWString().c_str());
	m_VCAObject->SetZoneRuleFlag();//设置保存规则

	bool bRet = m_PlayCtrlVCA->OpenAndPlayFile(m_FileCheckedList.at(m_curTask).strFileName, NULL);
	if (!bRet)
	{

		QFile file(m_FileCheckedList.at(m_curTask).strFileName);
		QString str;
		if (!file.exists())
		{
			str = QStringLiteral("文件路径不存在！");

		}
		else
		{
			str = QStringLiteral("无法播放！文件可能被损坏！");
		}
		QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
		pItem->setText(str);
		StartNextFile();
		return;
	}

	m_PlayWndVCA->HideToolBar();

	long width = 0;
	long height = 0;
	m_PlayCtrlVCA->GetPicSize(&width, &height);
	QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, FileWidthHeightVCA);
	pItem->setText(QString("%1*%2").arg(width).arg(height));
	if (width != m_nWidth || height != m_nHeight)//分辨率不一样，则不支持检索
	{
		QString str = QStringLiteral("屏幕大小不一样，不支持同等条件检索！");
		QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
		pItem->setText(str);
		m_PlayCtrlVCA->Stop();
		StartNextFile();
		return;
	}

	if (!m_PlayCtrlVCA->IsYUVCallback())//播放后判断是否已设置YUV回调
	{
		m_PlayCtrlVCA->SetDisplayCallback();
		if (!m_PlayCtrlVCA->IsYUVCallback())//不支持YUV回调，则不支持检索
		{
			QString str = QStringLiteral("此文件不支持检索！");
			QTableWidgetItem *pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
			pItem->setText(str);
			m_PlayCtrlVCA->Stop();
			StartNextFile();
			return;
		}
	}

	m_PlayCtrlVCA->GoFastSpeed();
	m_PlayCtrlVCA->SetRenderMode(RenderStrech);//不用设置为原比例模式，铺满窗口，DemoVCA是铺满窗口
	pItem = ui.tableWidgetFileVCA->item(m_curTask, VCAResult);
	pItem->setText(QStringLiteral("进行中"));

	pItem = ui.tableWidgetFileVCA->item(m_curTask, EventCount);
	pItem->setText(QStringLiteral("0"));


	RECT rc;
	QRect rc0 = m_PlayWndVCA->GetRenderClientRect();
	rc.left = 0;
	rc.top = 0;
	rc.right = rc0.width();
	rc.bottom = rc0.height();
	m_RenderVCA->OnChangeClientRect(rc);

	m_timerGetVCATime->start(1000);

}

void CWidgetVCAProgress::OnBtnStartClick()
{
	if (m_VCAState == VCAFinish)
	{
		return;
	}
	m_bPause = !m_bPause;
	if (m_bPause)
	{
		m_VCAState = VCAPause;
	}
	else{
		m_VCAState = VCAPlay;
		if (!m_PlayCtrlVCA->IsPlaying())//刚好暂停在列表文件之间，重新开始要调Start
		{
			StartNextFile();
		}
	}
}

void CWidgetVCAProgress::OnBtnStopClick()
{
	if (m_VCAState == VCAFinish)
	{
		ClearTableFileVCA();
		ui.BtnStart->setEnabled(false);
		ui.BtnStop->setEnabled(false);
		return;
	}
	
	
	//中断
	m_timerGetVCATime->stop();
	m_PlayCtrlVCA->Stop();
//	Sleep(10);
	ClearTableFileVCA();
	m_listAlarmID.clear();
	CWidgetVCAResult::DeleteDirectory(m_strVCADataPath);
	pMainDlg->m_dlgVCA->OnVCAFinish();

	ui.BtnStart->setEnabled(false);
	ui.BtnStop->setEnabled(false);
	m_VCAState = VCAStop;
	//Sleep(1000);

}

void CWidgetVCAProgress::Reset()
{
	OnBtnStopClick();
}