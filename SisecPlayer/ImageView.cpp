#include "stdafx.h"
#include "ImageView.h"


CImageView::CImageView(QWidget *pWidget)
{
	m_pDataMgr = new CVCADataMgr();
	m_pJpegCodec = new CJpegCodec;

	m_Render = new CCalibrationD3DRender((HWND)pWidget->winId());//这个放在构造函数UI会Disabled,原因未知。
	m_Render->Setup(NULL, m_pDataMgr);//这里传NUL，因为构造函数已经传了hwnd
	m_Render->SetColorFormat(CD3d::CF_YUY2);

	RECT rc;
	QRect rc0 = pWidget->geometry();
	rc.left = 0;
	rc.top = 0;
	rc.right = rc0.width();
	rc.bottom = rc0.height();
	m_Render->OnChangeClientRect(rc);

	m_pBuffer = NULL;
	m_dwBufferLength = 0;
	//m_pImageBuf = (BYTE *)_aligned_malloc(MAXWIDTH_D3DCANVAS * MAXHEIGHT_D3DCANVAS * 4, 16);
}


CImageView::~CImageView()
{
}

/*
void CImageView::ShowImage(DISPLAYCALLBCK_INFO *pstDisplayInfo)
{
	BITMAPINFOHEADER bm;
	bm.biWidth = pstDisplayInfo->nWidth;
	bm.biHeight = pstDisplayInfo->nHeight;
	m_Render->ChangeVCASourceInfo(&bm);

	m_Render->RenderVCAImage((BYTE*)pstDisplayInfo->pBuf, NULL,NULL,NULL);
}*/
void CImageView::ShowImage(QString strImagePath)
{

	Jpeg jpeg;
	ZeroMemory(&jpeg, sizeof(jpeg));

	if (!m_pJpegCodec->GetJpegInformation((LPCTSTR)strImagePath.toStdWString().c_str(), &jpeg)) return;
	if (m_dwBufferLength < jpeg.dwLength) {
		if (m_pBuffer) _aligned_free(m_pBuffer);
		m_pBuffer = (BYTE *)_aligned_malloc(jpeg.dwLength, 16);
		m_dwBufferLength = jpeg.dwLength;
	}

	jpeg.pBuffer = (BYTE *)m_pBuffer;
	if (!m_pJpegCodec->Decode((LPCTSTR)strImagePath.toStdWString().c_str(), &jpeg)) {
		return;
	}
	
	//BITMAPCOREHEADER bm;
	//bm.bcWidth = jpeg.bm.biWidth
	m_Render->ChangeVCASourceInfo(&jpeg.bm);	
	m_Render->RenderVCAImage(m_pBuffer, jpeg.pZone, jpeg.pRule, jpeg.pObject);
//	int width = (jpeg.bm.biWidth <= 360) ? (2 * jpeg.bm.biWidth) : (jpeg.bm.biWidth);
	//int height = (jpeg.bm.biHeight <= 288) ? (2 * jpeg.bm.biHeight) : (jpeg.bm.biHeight);
}