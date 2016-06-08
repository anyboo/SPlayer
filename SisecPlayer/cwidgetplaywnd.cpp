#include "cwidgetplaywnd.h"
#include <QMouseEvent>
#include "cdialogmain.h"

#define MAXSCALE 5 //

CWidgetPlayWnd::CWidgetPlayWnd(int index,QWidget *parent)
: QWidget(parent), m_index(index), m_previewMode(false)
{
	ui.setupUi(this);

	m_bEnabled = false;
	m_bFix = false;
	//ui.widgetToolBar->setAttribute(Qt::WA_TranslucentBackground);
/*	ui.widgetToolBar->setAttribute(Qt::WA_NoSystemBackground,true);
	this->setAttribute(Qt::WA_NoSystemBackground, true);
	ui.widgetToolBar->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	ui.widgetToolBar->setWindowFlags(Qt::FramelessWindowHint );
	ui.widgetToolBar->setParent(this->parentWidget());*/
	ui.widgetToolBar->hide();

	this->setMouseTracking(true);
	ui.widgetToolBar->setMouseTracking(true);
	ui.widgetRender->setMouseTracking(true);

	connect(ui.BtnFix, SIGNAL(clicked()), this, SLOT(OnBtnFixClick()));
	connect(ui.BtnColor, SIGNAL(clicked()), this, SLOT(OnBtnColorClick()));
	connect(ui.BtnPic, SIGNAL(clicked()), this, SLOT(OnBtnPicClick()));
	connect(ui.BtnPicRepeat, SIGNAL(clicked()), this, SLOT(OnBtnPicRepeatClick()));
	connect(ui.BtnFullScreen, SIGNAL(clicked()), this, SLOT(OnBtnFullScreenClick()));

}

CWidgetPlayWnd::~CWidgetPlayWnd()
{
	
	
}

void CWidgetPlayWnd::resizeEvent(QResizeEvent * event)
{
	QRect rc = this->rect();
	int width = rc.width();
	ui.widgetToolBar->setGeometry(0, 0, width, 40);
}

void CWidgetPlayWnd::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (!m_previewMode)
	{
		painter.fillRect(this->rect(), QColor(0, 0, 0));
	}
	else
	{
		painter.fillRect(this->rect(), QColor(50, 50, 50));
	}
}

void CWidgetPlayWnd::mousePressEvent(QMouseEvent * event)
{
	QApplication::postEvent(this->parentWidget(), new CMyEvent(CUSTOM_PLAYWNDPRESSED_EVENT,this));
	QWidget::mousePressEvent(event);
}
//为辅助mouseMoveEvent离开后隐藏工具栏
void CWidgetPlayWnd::leaveEvent(QEvent * event)
{
	if (m_bEnabled&&!m_bFix)
	{
		ui.widgetToolBar->hide();
	}
	QWidget::leaveEvent(event);
}

void CWidgetPlayWnd::mouseMoveEvent(QMouseEvent * event)
{
	if (m_bEnabled)
	{
		CDialogMain *pMainWindow = NULL;
		if (!m_previewMode)
			pMainWindow = (CDialogMain*)this->parentWidget();

		QRect rc = ui.widgetToolBar->geometry();
		if (rc.contains(event->x(), event->y()))
		{
			if (!m_bFix){
				ui.widgetToolBar->show();
			}

			if (pMainWindow){
				pMainWindow->SetPlayCtrlVisible(true);
			}
		}
		else
		{
			
			bool bVisible = false;
			if (pMainWindow)
				bVisible=pMainWindow->IsPlayCtrlVisible(event->x(), event->y());
		
			if (bVisible)
			{
				ui.widgetToolBar->show();
			}
			else{
				if (!m_bFix){
					ui.widgetToolBar->hide();
				}
			}
		}
	}

	QWidget::mouseMoveEvent(event);
}

void CWidgetPlayWnd::wheelEvent(QWheelEvent * event)
{
	if (!m_bEnabled)
	{
		return;
	}	
	QRect rc = ui.widgetRender->geometry();
	if (!rc.contains(event->pos()))
	{
		return;
	}

	int numDegrees = event->delta() ;
	int size = m_listScaleRC.size();

	if (numDegrees > 0)//向前 放大
	{
		if (size==MAXSCALE)
		{
			return;
		}
		
		m_listScaleRC.push_back(rc);
		float scale = 1.2;
		int sWidth = rc.width()*scale;
		int sHeight = rc.height()*scale;

		float fx = (event->x()-rc.left()) /(float) rc.width();
		float fy = (event->y() - rc.top()) / (float)rc.height();
		int x = rc.left() -(scale - 1.0)* rc.width() / 2;
		int y = rc.top()-(scale - 1.0)* rc.height() / 2;
		int mousex = x + fx*sWidth;
		int mousey = y + fy*sHeight;
		
		if (mousex < 0)
		{
			int defultDif = rc.width()*0.1;
			int dif = abs(mousex) + defultDif;
			x += dif;
		}
		else if (mousex >event->x())
		{
			int dif = mousex - event->x();
			x -= dif;
		}

		if (mousey < 0)
		{
			int defultDif = rc.height()*0.1;
			int dif = abs(mousey) + defultDif;
			y += dif;
		}
		else if (mousey >event->y())
		{
			int dif =  mousey-event->y() ;
			y -= dif;
		}
	
		rc.moveTo(x, y);
		rc.setSize(QSize(sWidth, sHeight));
		
		ui.widgetRender->setGeometry(rc);
	}
	else//向后 缩小
	{
		if (size == 0)//已经恢复到原始图像大小
		{
			return;
		}
		 
		QRect rc = m_listScaleRC.back();
		m_listScaleRC.pop_back();
		ui.widgetRender->setGeometry(rc);
	}
	  
	QWidget::wheelEvent(event);
}

void CWidgetPlayWnd::mouseDoubleClickEvent(QMouseEvent * event)
{
	QApplication::postEvent(this->parentWidget(), new CMyEvent(CUSTOM_PLAYWNDDBCLICK_EVENT, this));
	QWidget::mouseDoubleClickEvent(event);
}
void CWidgetPlayWnd::OnBtnFixClick()
{
	m_bFix =!m_bFix;
	if (m_bFix)
	{
		ui.BtnFix->setToolTip(QStringLiteral("从不置顶"));
		ui.BtnFix->setStyleSheet("QPushButton{border-image: url(:/Skin/fix.png)0 0 0 22 repeat}QPushButton:hover{border-image: url(:/Skin/fix.png)22 0 0 22 repeat}QPushButton:pressed{border-image: url(:/Skin/fix.png)44 0 0 22 repeat}QPushButton:disabled{border-image: url(: / Skin/fix.png)66 0 0 22 repeat}");
	}
	else
	{
		ui.BtnFix->setToolTip(QStringLiteral("始终置顶"));
		ui.BtnFix->setStyleSheet("QPushButton{border-image: url(:/Skin/fix.png)0 0 0 0 repeat}QPushButton:hover{border-image: url(:/Skin/fix.png)22 0 0 0 repeat}QPushButton:pressed{border-image: url(:/Skin/fix.png)44 0 0 0 repeat}QPushButton:disabled{border-image: url(: / Skin/fix.png)66 0 0 0 repeat}");
	}
}

void CWidgetPlayWnd::OnBtnPicClick()
{
	QApplication::postEvent(this->parentWidget(), new CMyEvent(CUSTOM_PIC_EVENT, this));
}

void CWidgetPlayWnd::OnBtnPicRepeatClick()
{
	QApplication::postEvent(this->parentWidget(), new CMyEvent(CUSTOM_PICREPEAT_EVENT, this));
}

void CWidgetPlayWnd::OnBtnFullScreenClick()
{
	QApplication::postEvent(this->parentWidget(), new CMyEvent(CUSTOM_PLAYWNDDBCLICK_EVENT, this));
}

void CWidgetPlayWnd::OnBtnColorClick()
{
	emit ShowColorDlg();
}