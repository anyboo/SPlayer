#pragma once
#include "VCAConfigure.h"
#include "VCAMetalib.h"
#include "JpegCodec.h"

// CAlarmViewDlg dialog
#define		IMAGE_NAME_LEN	16 // XXX_XXX_XXXX.JPG

class CD3d;
class CImageView
{
public:
	CImageView();
	~CImageView();
public:	
	void ShowImage(QString &strPathName);
private:
	void	DrawImage(int width, int height, BYTE* pImage, BITMAPINFOHEADER bm);
	void	DrawSingleZone(RECT rcVCAROI, VCA5_APP_ZONE *pZone);
	void	DrawTimeStampAndRuleType(LONG width, LONG height, __int64 i64TimeStamp, int iruleType);
	void	DrawObject(RECT rcCanvas, LONG width, LONG height, VCA5_PACKET_OBJECT *pObject);
	void	DrawBoundinxBox(RECT clientRect, SIZE SourceSize, RECT rcVCAROI, VCA5_RECT	bBox, BYTE alpha, COLORREF color);
	void	DrawObjectInformation(RECT clientRect, SIZE SourceSize, VCA5_PACKET_OBJECT *pObject, BYTE alpha, COLORREF Color);

private:
	CD3d*		m_pD3d;
	int			m_nCurImageIdx;

	RECT			m_ClientRect;


	CJpegCodec	m_JpegCodec;
	BYTE		*m_pBuffer;
	DWORD		m_dwBufferLength;


	BYTE*	m_pImageBuf;

};
