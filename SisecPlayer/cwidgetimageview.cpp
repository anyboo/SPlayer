
#include "cwidgetimageview.h"

CWidgetImageView::CWidgetImageView(QWidget *parent)
	: QWidget(parent), m_bShow(false)
{
	ui.setupUi(this);

	m_pDataMgr = new CVCADataMgr();
	m_pJpegCodec = new CJpegCodec;

	m_Render = new CCalibrationD3DRender((HWND)this->winId());//这个放在构造函数UI会Disabled,原因未知。
	m_Render->Setup(NULL, m_pDataMgr);//这里传NUL，因为构造函数已经传了hwnd
	m_Render->SetColorFormat(CD3d::CF_YUY2);

	m_pBuffer = NULL;
	m_dwBufferLength = 0;
	//m_pImageBuf = (BYTE *)_aligned_malloc(MAXWIDTH_D3DCANVAS * MAXHEIGHT_D3DCANVAS * 4, 16);	
	ZeroMemory(&m_jpeg, sizeof(m_jpeg));
}

CWidgetImageView::~CWidgetImageView()
{

}

void CWidgetImageView::paintEvent(QPaintEvent *ev)
{
	QWidget::paintEvent(ev);
/*	QPainter painter(this);
	if (m_bShow)
	{
		m_Render->RenderVCAImage(m_pBuffer, m_jpeg.pZone, m_jpeg.pRule, m_jpeg.pObject);
	}*/
}

void CWidgetImageView::Reset()
{
	m_bShow = false;
	this->repaint();
}
void CWidgetImageView::ShowImage(QString strImagePath)
{
	ZeroMemory(&m_jpeg, sizeof(m_jpeg));
	if (m_pBuffer)
	{
		memset(m_pBuffer, 0, m_dwBufferLength);
	}

	if (!m_pJpegCodec->GetJpegInformation((LPCTSTR)strImagePath.toStdWString().c_str(), &m_jpeg)) return;
	if (m_dwBufferLength < m_jpeg.dwLength) {
		if (m_pBuffer) _aligned_free(m_pBuffer);
		m_pBuffer = (BYTE *)_aligned_malloc(m_jpeg.dwLength, 16);
		m_dwBufferLength = m_jpeg.dwLength;
	}

	m_jpeg.pBuffer = (BYTE *)m_pBuffer;
	if (!m_pJpegCodec->Decode((LPCTSTR)strImagePath.toStdWString().c_str(), &m_jpeg)) {
		m_bShow = false;
		this->repaint();
		return;
	}

	//BITMAPCOREHEADER bm;
	//bm.bcWidth = jpeg.bm.biWidth
	RECT rc;
	QRect rc0 = this->geometry();
	rc.left = 0;
	rc.top = 0;
	rc.right = rc0.width();
	rc.bottom = rc0.height();
	m_Render->OnChangeClientRect(rc);
	m_Render->ChangeVCASourceInfo(&m_jpeg.bm);
	m_Render->RenderVCAImage(m_pBuffer, m_jpeg.pZone, m_jpeg.pRule, m_jpeg.pObject);
	m_bShow = true;
	//	int width = (jpeg.bm.biWidth <= 360) ? (2 * jpeg.bm.biWidth) : (jpeg.bm.biWidth);
	//int height = (jpeg.bm.biHeight <= 288) ? (2 * jpeg.bm.biHeight) : (jpeg.bm.biHeight);
}