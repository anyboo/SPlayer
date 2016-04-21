#include "stdafx.h"
#include "untils.h"
#include "tableYUVRGB.h"
#include "Jpegfile.h"

#define _TABLE_KP
int YUV420_RGB_KP(AVFrame *YUV420Pic, unsigned char *pucArrRGBData, const int nWidth, const int nHeight)
{
	//	FastMutex::ScopedLock lock(m_mutex);
	long pixelNum = nWidth * nHeight;

	memset(pucArrRGBData, 0, pixelNum * 3);

	unsigned char *pucBmpData = pucArrRGBData;
	const unsigned char *pucYData;
	const unsigned char *pucUData;
	const unsigned char *pucVData;
	for (int j = nHeight - 1; j >= 0; j--)
	{
		pucYData = YUV420Pic->data[0] + j*YUV420Pic->linesize[0];
		pucUData = YUV420Pic->data[1] + j / 2 * (YUV420Pic->linesize[1]);
		pucVData = YUV420Pic->data[2] + j / 2 * (YUV420Pic->linesize[2]);
		for (int i = 0; i < nWidth; i++)
		{
			int iVal = 0;

			//计算RGB像素 
#ifdef _TABLE_KP
			iVal = (tableB_U[*pucUData]) >> 12;
			iVal = int((*pucYData) + iVal - 227);												//B
#else
			iVal = int((*pucYData) + 1.771 * (*pucUData - 128) + 0.5);							//B
#endif
			*pucBmpData++ = max(min(255, iVal), 0);

#ifdef _TABLE_KP			
			iVal = (tableG_V[*pucVData] + tableG_U[*pucUData]) >> 12;
			iVal = int(*pucYData - iVal + 136);												//G
#else
			iVal = int(*pucYData - 0.3456 * (*pucUData) - 0.7145 * (*pucVData) + 135.95);		//G
#endif
			*pucBmpData++ = max(min(255, iVal), 0);

#ifdef _TABLE_KP			
			iVal = (tableR_V[*pucVData]) >> 12;
			iVal = int(*pucYData + iVal - 180);													//R
#else
			iVal = int(*pucYData + 1.402 * (*pucVData - 128) + 0.5);							//R
#endif
			*pucBmpData++ = max(min(255, iVal), 0);
			pucYData++;

#if 0

			//			iVal        = (tableB_U[*pucUData]) >> 12;
			//			iVal        = int( (*pucYData) + iVal - 227);												//B
			iVal = int((*pucYData) + 1.771 * (*pucUData - 128) + 0.5);							//B
			*pucBmpData++ = max(min(255, iVal), 0);

			//			iVal        = (tableG_V[*pucVData] + tableG_U[*pucUData]) >> 12;
			//			iVal        = int( *pucYData - iVal + 136 );												//G
			iVal = int(*pucYData - 0.3456 * (*pucUData) - 0.7145 * (*pucVData) + 135.95);		//G
			*pucBmpData++ = max(min(255, iVal), 0);

			//			iVal        = (tableR_V[*pucVData]) >> 12;
			//			iVal        = int( *pucYData + iVal - 180);													//R
			iVal = int(*pucYData + 1.402 * (*pucVData - 128) + 0.5);							//R
			*pucBmpData++ = max(min(255, iVal), 0);
			pucYData++;

#endif
			//每四个像素更新一次UV
			if (1 == i % 2)
			{
				pucUData++;
				pucVData++;
			}
		}
	}
	return 0;
}

void writeBMPFile(char* file, char *pRGBBuf, int m_nWidth, int m_nHeight)
{
#ifdef _WIN32

	long RGB_SIZE = m_nWidth * m_nHeight * 3;

	FILE *fp;
	int count = 0;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFO bmpInfo;
	fopen_s(&fp, file, "wb");
	if (fp  == NULL)
	{
		return;
	}

	bmpHeader.bfType = 'MB';
	bmpHeader.bfSize = RGB_SIZE + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = m_nWidth;
	bmpInfo.bmiHeader.biHeight = m_nHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = RGB_SIZE;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	count = fwrite(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);
	count = fwrite(&(bmpInfo.bmiHeader), 1, sizeof(BITMAPINFOHEADER), fp);
	count = fwrite(pRGBBuf, 1, RGB_SIZE, fp);

	fclose(fp);

#endif
}


void writeJPGFile(char* file, char *pRGBBuf, int m_nWidth, int m_nHeight)
{
#ifdef _WIN32

	JpegFile::VertFlipBuf((BYTE *)pRGBBuf, m_nWidth * 3, m_nHeight);

	// we swap red and blue for display, undo that.
	JpegFile::BGRFromRGB((BYTE *)pRGBBuf, m_nWidth, m_nHeight);


	// save RGB packed buffer to JPG
	BOOL ok = JpegFile::RGBToJpegFile(file,
		(BYTE *)pRGBBuf,
		m_nWidth,
		m_nHeight,
		1,
		100);			// quality value 1-100.

#endif
}