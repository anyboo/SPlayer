#include "stdafx.h"
#include "jpegcodec.h"
#include "ijl.h"
//#include <mmsystem.h>
#pragma comment(lib,"./vca/libijl/ijl15l.lib")//相对于工程目录

#undef SAFE_FREE
#define SAFE_FREE(ptr) \
	if (ptr) { \
		_aligned_free(ptr); \
		ptr = NULL; \
	}

#define JPG_HEADER_LEN	(400)
#define TIMESTAMP_LEN	(12)
#define	ZONE_LEN		(sizeof(VCA5_APP_ZONE) + 2)
#define	RULE_LEN		(sizeof(VCA5_RULE) + 2)
#define OBJECT_LEN		(sizeof(VCA5_PACKET_OBJECT) + 2)
#define JPG_BUFFER_MIN_SIZE(pbm)	((JPG_HEADER_LEN + TIMESTAMP_LEN + ZONE_LEN) + 1 + OBJECT_LEN)

#define YUY2_IMG_LEN(pbm) ((pbm)->biHeight*(pbm)->biWidth*2)
#define JPG_FILE_LEN(pbm)	(YUY2_IMG_LEN(pbm) + JPG_HEADER_LEN + TIMESTAMP_LEN + ZONE_LEN + OBJECT_LEN)
#define JPG_DECODED_LEN(pbm) ((pbm)->biWidth*(pbm)->biHeight*3 + JPG_HEADER_LEN + TIMESTAMP_LEN + ZONE_LEN + RULE_LEN + OBJECT_LEN)


//CCriticalSection CJpegCodec::m_cs;

CJpegCodec::CJpegCodec(void)
{
	m_pYUY2Buffer = NULL;
	m_dwYUY2Length = 0;
	m_pJpgBuffer = NULL;
	m_dwJpgLength = 0;
}

CJpegCodec::~CJpegCodec(void)
{
	SAFE_FREE(m_pYUY2Buffer);
	SAFE_FREE(m_pJpgBuffer);
}

void CJpegCodec::ReallocBuffer(BITMAPINFOHEADER *pbm)
{
	if (( NULL == m_pYUY2Buffer ) || ( m_dwYUY2Length < (DWORD) YUY2_IMG_LEN(pbm) )) {
		SAFE_FREE(m_pYUY2Buffer);
		m_dwYUY2Length = YUY2_IMG_LEN(pbm);
		m_pYUY2Buffer = (BYTE *) _aligned_malloc(YUY2_IMG_LEN(pbm), 16);
	} 

	if (( NULL == m_pJpgBuffer) || ( m_dwJpgLength < (DWORD) JPG_FILE_LEN(pbm) )) {
		SAFE_FREE(m_pJpgBuffer);
		m_dwJpgLength = JPG_FILE_LEN(pbm);
		m_pJpgBuffer = (BYTE *) _aligned_malloc(JPG_FILE_LEN(pbm), 16);
	}
}

BOOL CJpegCodec::Encode(LPCTSTR pszPath, BYTE *pImage, BITMAPINFOHEADER *pbm,  __int64 i64TimeStamp,
					  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject)
{
//	m_cs.Lock();
	ReallocBuffer(pbm);

	BOOL ret = SaveToFile(pszPath, pImage, pbm, i64TimeStamp, pZone, pRule, pObject);
//	m_cs.Unlock();
	return ret;
}

DWORD CJpegCodec::CompressYUY2(BYTE *pImage, BITMAPINFOHEADER *pbm, BYTE *pDst, DWORD dwLength)
{
	JPEG_CORE_PROPERTIES jcp;
	IJLERR jerr;

	jerr = ijlInit(&jcp);
	if (IJL_OK != jerr) {
		TRACE("Fail to init jpeg library\n");
		goto EXIT;
	}

	jcp.DIBBytes	= pImage;
	jcp.DIBPadBytes	= 0;
	jcp.DIBHeight	= pbm->biHeight;
	jcp.DIBWidth	= pbm->biWidth;
	jcp.DIBChannels	= 3;
	jcp.DIBColor	= IJL_YCBCR;
	jcp.DIBSubsampling	= IJL_422;

	jcp.JPGFile		= NULL;
	jcp.JPGSizeBytes = dwLength; // buffer size
	jcp.JPGBytes	= pDst;
	jcp.JPGHeight	= pbm->biHeight;
	jcp.JPGWidth	= pbm->biWidth;
	jcp.JPGChannels	= 3;
	jcp.JPGColor	= IJL_YCBCR;
	jcp.JPGSubsampling	= IJL_422;
	jcp.jquality	= 75;

	jerr = ijlWrite(&jcp, IJL_JBUFF_WRITEWHOLEIMAGE);
	if (IJL_OK != jerr) {
		TRACE("Error at jilWrite\n");
		goto EXIT;
	}

EXIT:
	ijlFree(&jcp);
	return jcp.JPGSizeBytes;
}

DWORD CJpegCodec::CompressRGB(BYTE *pImage, BITMAPINFOHEADER *pbm, BYTE *pDst, DWORD dwLength)
{
	JPEG_CORE_PROPERTIES jcp;
	IJLERR jerr;

	jerr = ijlInit(&jcp);
	if (IJL_OK != jerr) {
		TRACE("Fail to init jpeg library\n");
		goto EXIT;
	}

	jcp.DIBBytes	= pImage;
	jcp.DIBPadBytes	= 0;
	jcp.DIBHeight	= pbm->biHeight;
	jcp.DIBWidth	= pbm->biWidth;
	jcp.DIBChannels	= pbm->biBitCount / 8;
	//jcp.DIBColor	= IJL_RGB;
	jcp.DIBColor = IJL_YCBCR;
	jcp.DIBSubsampling	= IJL_NONE;

	jcp.JPGFile		= NULL;
	jcp.JPGSizeBytes = dwLength; // buffer size
	jcp.JPGBytes	= pDst;
	jcp.JPGHeight	= pbm->biHeight;
	jcp.JPGWidth	= pbm->biWidth;
	jcp.JPGChannels	= 3;
	jcp.JPGColor	= IJL_YCBCR;
	jcp.JPGSubsampling	= IJL_422;
	jcp.jquality	= 75;

	jerr = ijlWrite(&jcp, IJL_JBUFF_WRITEWHOLEIMAGE);
	if (IJL_OK != jerr) {
		TRACE("Error at jilWrite\n");
		goto EXIT;
	}

EXIT:
	ijlFree(&jcp);
	return jcp.JPGSizeBytes;
}

DWORD CJpegCodec::InsertAppBlocks(BYTE *pJpgBuffer, DWORD dwLength, BITMAPINFOHEADER *pbm, __int64 i64TimeStamp,
								  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject)
{

	// Is the given buffer valid? Check SOI and APP0 markers
	if ( ( 0xFF == pJpgBuffer[0] ) && ( 0xD8 == pJpgBuffer[1] ) &&
		( 0xFF == pJpgBuffer[2] ) && ( 0xE0 == pJpgBuffer[3] )) {
			DWORD	dwApp0Length = ((pJpgBuffer[4] << 8) | pJpgBuffer[5]) + 4; // SOI and APP0 block length
			BYTE	*pImage = pJpgBuffer + dwApp0Length;
			//DWORD	dwAppOffset = dwApp0Length;
			DWORD	dwAppOffset = dwLength;
			DWORD	dwApp1Length = 8 + 2;
			DWORD	dwApp2Length = sizeof(VCA5_APP_ZONE) + 2;
			DWORD	dwApp3Length = sizeof(VCA5_RULE) + 2;
			DWORD	dwApp4Length = sizeof(VCA5_PACKET_OBJECT) + 2;

			DWORD	dwInsertedLength = 0;//(dwApp1Length+2) + (dwApp2Length+2) + (dwApp3Length+2) + (dwApp4Length+2);

			if(pbm) {
				pJpgBuffer[14] = (BYTE) ((pbm->biWidth & 0x0FF00) >> 8);
				pJpgBuffer[15] = (BYTE) (pbm->biWidth & 0x0FF);
				pJpgBuffer[16] = (BYTE) ((pbm->biHeight & 0x0FF00) >> 8);
				pJpgBuffer[17] = (BYTE) (pbm->biHeight & 0x0FF);
			}

			// 1. Insert APP1 block(TimeStamp)
			//	1.1 Insert APP1 marker
			pJpgBuffer[dwAppOffset + 0] = 0xFF;
			pJpgBuffer[dwAppOffset + 1] = 0xE1;
			pJpgBuffer[dwAppOffset + 2] = (BYTE) ((dwApp1Length & 0x0FF00) >> 8);
			pJpgBuffer[dwAppOffset + 3] = (BYTE) (dwApp1Length & 0x0FF);
			//	1.2 Insert time stamp
			CopyMemory(&pJpgBuffer[dwAppOffset + 4], &i64TimeStamp, 8);

			dwAppOffset += dwApp1Length+2;
			dwInsertedLength += dwApp1Length+2;

			// 2. Insert APP2 block(Zone)
			//	2.1 Insert APP2 marker
			ASSERT(dwApp2Length <= USHRT_MAX);

			if(pZone) {
				pJpgBuffer[dwAppOffset + 0] = 0xFF;
				pJpgBuffer[dwAppOffset + 1] = 0xE2;
				pJpgBuffer[dwAppOffset + 2] = (BYTE) ((dwApp2Length & 0x0FF00) >> 8);
				pJpgBuffer[dwAppOffset + 3] = (BYTE) (dwApp2Length  & 0x0FF);
			//	2.2 Insert Zone
				CopyMemory(&pJpgBuffer[dwAppOffset+4], pZone, sizeof(VCA5_APP_ZONE));
				dwAppOffset += dwApp2Length+2;
				dwInsertedLength += dwApp2Length+2;
			}
			
			// 3. Insert APP3 block(Rule)
			//	3.1 Insert APP3 marker
			if(pRule) {
				pJpgBuffer[dwAppOffset + 0] = 0xFF;
				pJpgBuffer[dwAppOffset + 1] = 0xE3;
				pJpgBuffer[dwAppOffset + 2] = (BYTE) ((dwApp3Length & 0x0FF00) >> 8);
				pJpgBuffer[dwAppOffset + 3] = (BYTE) (dwApp3Length & 0x0FF);
				// 2.2 Insert Rule
				CopyMemory(&pJpgBuffer[dwAppOffset+4], pRule, sizeof(VCA5_RULE));
				dwAppOffset += dwApp3Length+2;
				dwInsertedLength += dwApp3Length+2;
			}

			// 4. Insert APP3 block(Object)
			//	4.1 Insert APP3 marker
			if(pObject) {
				pJpgBuffer[dwAppOffset + 0] = 0xFF;
				pJpgBuffer[dwAppOffset + 1] = 0xE4;
				pJpgBuffer[dwAppOffset + 2] = (BYTE) ((dwApp4Length & 0x0FF00) >> 8);
				pJpgBuffer[dwAppOffset + 3] = (BYTE) (dwApp4Length & 0x0FF);
				// 4.2 Insert Object
				CopyMemory(&pJpgBuffer[dwAppOffset+4], pObject, sizeof(VCA5_PACKET_OBJECT));
				dwAppOffset += dwApp4Length+2;
				dwInsertedLength += dwApp4Length+2;
			}
			
			return (dwLength + dwInsertedLength);
	}
	return 0;
}

BOOL CJpegCodec::SaveToFile(LPCTSTR pszPath, BYTE *pImage, BITMAPINFOHEADER *pbm, __int64 i64TimeStamp,
						  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject)
{
	DWORD dwJpgLength = 0;

	if (mmioFOURCC('Y','U','Y','2') == pbm->biCompression) {
		dwJpgLength = CompressYUY2(pImage, pbm, m_pJpgBuffer, m_dwJpgLength);
	} else if (mmioFOURCC('Y','V','1','2') == pbm->biCompression) {

		// Convert YV12 to YUY2
		for (DWORD y = 0; y < (DWORD) pbm->biHeight; y+=2) {
			for (DWORD x = 0; x < (DWORD) pbm->biWidth; x+=2) {
				BYTE *pY1 = &pImage[(y+0)*pbm->biWidth + x];
				BYTE *pY2 = &pImage[(y+1)*pbm->biWidth + x];
				//BYTE *pU = &pImage[pbm->biWidth*pbm->biHeight + y*pbm->biWidth/4 + x/2];
			//	BYTE *pV = &pImage[pbm->biWidth*pbm->biHeight*5/4 + y*pbm->biWidth/4 + x/2];
				BYTE *pV = &pImage[pbm->biWidth*pbm->biHeight + y*pbm->biWidth / 4 + x / 2];
				BYTE *pU = &pImage[pbm->biWidth*pbm->biHeight * 5 / 4 + y*pbm->biWidth / 4 + x / 2];

				m_pYUY2Buffer[y*pbm->biWidth*2 + (x+0)*2+0] = *(pY1+0);
				m_pYUY2Buffer[y*pbm->biWidth*2 + (x+0)*2+1] = *pU;
				m_pYUY2Buffer[y*pbm->biWidth*2 + (x+1)*2+0] = *(pY1+1);
				m_pYUY2Buffer[y*pbm->biWidth*2 + (x+1)*2+1] = *pV;
				m_pYUY2Buffer[(y+1)*pbm->biWidth*2 + (x+0)*2+0] = *(pY2+0);
				m_pYUY2Buffer[(y+1)*pbm->biWidth*2 + (x+0)*2+1] = *pU;
				m_pYUY2Buffer[(y+1)*pbm->biWidth*2 + (x+1)*2+0] = *(pY2+1);
				m_pYUY2Buffer[(y+1)*pbm->biWidth*2 + (x+1)*2+1] = *pV;
			}
		}
		dwJpgLength = CompressYUY2(m_pYUY2Buffer, pbm, m_pJpgBuffer, m_dwJpgLength);

	} else if( BI_RGB == pbm->biCompression ) {
		dwJpgLength = CompressRGB(pImage, pbm, m_pJpgBuffer, m_dwJpgLength);
	}

	if (dwJpgLength > 0) {
		FILE *fp;
		errno_t err;

		dwJpgLength = InsertAppBlocks(m_pJpgBuffer, dwJpgLength, pbm, i64TimeStamp, pZone, pRule, pObject);
		
		err = _tfopen_s(&fp, pszPath, _T("wb"));
		if (err) {
			TRACE(_T("Fail to create %s\n"), pszPath);
			return FALSE;
		}

		fwrite(m_pJpgBuffer, sizeof(BYTE), dwJpgLength, fp);
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

DWORD	CJpegCodec::GetJpegInformation(LPCTSTR pszPath, Jpeg *pJpeg)
{
	// FIXME: memory leak in ijlRead with IJL_JFILE_READPARAMS!

	//JPEG_CORE_PROPERTIES jcp;
	//IJLERR	jerr;
	//BOOL	rs = FALSE;

	//jerr = ijlInit(&jcp);
	//if (IJL_OK != jerr) {
	//	TRACE("Fail to init jpeg library\n");
	//	goto EXIT;
	//}


	//jcp.JPGFile = pszPath;
	//jerr = ijlRead(&jcp, IJL_JFILE_READPARAMS);
	//if (IJL_OK != jerr) {
	//	TRACE("Fail to init jpeg library\n");
	//	goto EXIT;
	//}

	BITMAPINFOHEADER &bm = pJpeg->bm;

	if (!ParseApp0Block(pszPath, bm)) {
		return FALSE;
	}

	bm.biSize	= sizeof(BITMAPINFOHEADER);
	bm.biBitCount = 16;
	bm.biPlanes	= 1;
	bm.biCompression = mmioFOURCC('Y','U','Y','2');
	bm.biSizeImage = 0;
	bm.biXPelsPerMeter = 1;
	bm.biYPelsPerMeter = 1;
	bm.biClrImportant = 0;
	bm.biClrUsed = 0;

	pJpeg->dwLength = JPG_DECODED_LEN(&bm);
	return TRUE;
	//rs = TRUE;
	//EXIT:
	//ijlFree(&jcp);
	//return rs;
}


DWORD	CJpegCodec::GetSizeOfAppBlocks(LPCTSTR pszPath, Jpeg *pJpeg)
{
	FILE *fp;
	BYTE *pBuffer = pJpeg->pBuffer;
	LONG Length;

	// 0. read APP blocks
	if (_tfopen_s(&fp, pszPath, _T("rb"))) {
		TRACE("Fail to open %s\n", pszPath);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	Length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(pJpeg->pBuffer, 1, Length, fp);
	fclose(fp);

	DWORD offset = 0;
	while(offset <= (DWORD)Length) {
		if (0xFF == pBuffer[offset]) {
			if (0xE1 == pBuffer[offset+1]) {	// TimeStamp
				break;
			}
		}
		offset++;
	}

	return (Length - offset);
}

DWORD	CJpegCodec::ParseAppBlocks(LPCTSTR pszPath, Jpeg *pJpeg, DWORD AppBlockSize)
{
	FILE *fp;
	BYTE *pJpgBuffer = pJpeg->pBuffer;
	LONG Length;

	// 0. read APP blocks
	if (_tfopen_s(&fp, pszPath, _T("rb"))) {
		TRACE("Fail to open %s\n", pszPath);
		return 0;
	}
	DWORD	dwApp0Length = ((pJpgBuffer[4] << 8) | pJpgBuffer[5]) + 4;
	DWORD	dwApp1Length = 8 + 2;
	DWORD	dwApp2Length = sizeof(VCA5_APP_ZONE) + 2;
	DWORD	dwApp3Length = sizeof(VCA5_RULE) + 2;
	DWORD	dwApp4Length = sizeof(VCA5_PACKET_OBJECT) + 2;
	DWORD	dwInsertedLength = (dwApp1Length+2) + (dwApp2Length+2) + (dwApp3Length+2) + (dwApp4Length+2);
	DWORD	dwAppOffset = pJpeg->bm.biHeight*pJpeg->bm.biWidth*3;

	fseek(fp, 0, SEEK_END);
	Length = ftell(fp);

	LONG AppFileOffset = Length - AppBlockSize;
	if (AppFileOffset < 0) {
		return 0;
	}

	if((dwAppOffset+AppBlockSize) >= pJpeg->dwLength) {
		return 0;
	}

	fseek(fp, AppFileOffset, SEEK_SET);
	fread(pJpeg->pBuffer+dwAppOffset , 1, AppBlockSize, fp);
	fclose(fp);

	DWORD offset = (DWORD)dwAppOffset;
	while(offset <= pJpeg->dwLength) {
		if (0xFF == pJpgBuffer[offset]) {

			if (0xE1 == pJpgBuffer[offset+1]) {	// TimeStamp
				dwApp1Length = (pJpgBuffer[offset+2] << 8) | pJpgBuffer[offset+3];
				CopyMemory(&pJpeg->i64TimeStamp, &pJpgBuffer[offset+4], dwApp1Length-2);
				offset += dwApp1Length+2;
				continue;
			} else if (0xE2 == pJpgBuffer[offset+1]) {	// Zone
				dwApp2Length = (pJpgBuffer[offset+2] << 8) | pJpgBuffer[offset+3];
				pJpeg->pZone = (VCA5_APP_ZONE *) &pJpgBuffer[offset+4];
				offset += dwApp2Length+2;
				continue;
			} else if (0xE3 == pJpgBuffer[offset+1]) {	// Rule
				dwApp3Length = (pJpgBuffer[offset+2] << 8) | pJpgBuffer[offset+3];
				pJpeg->pRule = (VCA5_RULE *) &pJpgBuffer[offset+4];
				offset += dwApp3Length+2;
				continue;
			} else if (0xE4 == pJpgBuffer[offset+1]) {	// Object
				dwApp4Length = (pJpgBuffer[offset+2] << 8) | pJpgBuffer[offset+3];
				pJpeg->pObject = (VCA5_PACKET_OBJECT *) &pJpgBuffer[offset+4];
				offset += dwApp4Length+2;
				continue;
			}
		}
		offset++;
	}

	return (dwAppOffset - offset);
}

BOOL	CJpegCodec::ParseApp0Block(LPCTSTR pszPath, BITMAPINFOHEADER &bi)
{
	BOOL rs;
	FILE *fp;

	rs = TRUE;
	if (_tfopen_s(&fp, pszPath, _T("rb"))) {
		return FALSE;
	}

	fread(&m_pHeader[0], sizeof(BYTE), sizeof(m_pHeader), fp);
	if ( ( 0xFF == m_pHeader[0] ) && ( 0xD8 == m_pHeader[1] ) &&
		( 0xFF == m_pHeader[2] ) && ( 0xE0 == m_pHeader[3] )) {
			bi.biWidth = (m_pHeader[14] << 8) | m_pHeader[15];
			bi.biHeight = (m_pHeader[16] << 8) | m_pHeader[17];
	} else {
		rs = FALSE;
	}

	fclose(fp);
	return rs;
}

BOOL	CJpegCodec::Decode(LPCTSTR pszPath, Jpeg *pJpeg)
{
	JPEG_CORE_PROPERTIES jcp;
	IJLERR jerr;
	BOOL	rs = FALSE;

	if (NULL == pJpeg->pBuffer || pJpeg->dwLength < (DWORD) JPG_BUFFER_MIN_SIZE(&pJpeg->bm)) {
		return rs;
	}

	//m_cs.Lock();

	DWORD nAppBlockSize = GetSizeOfAppBlocks(pszPath, pJpeg);
	jerr = ijlInit(&jcp);
	if (IJL_OK != jerr) {
		TRACE("Fail to init jpeg library\n");
		goto EXIT;
	}

	USES_CONVERSION;
	jcp.JPGFile		= T2A((LPTSTR)pszPath);
	jcp.JPGColor	= IJL_YCBCR;

	jcp.DIBWidth	= pJpeg->bm.biWidth;
	jcp.DIBHeight	= pJpeg->bm.biHeight;
	jcp.DIBChannels	= 2;
	jcp.DIBColor	= IJL_YCBCR;
	jcp.DIBSubsampling	= IJL_422;
	jcp.DIBPadBytes	= 0;
	jcp.DIBBytes	= pJpeg->pBuffer;

	jerr = ijlRead(&jcp, IJL_JFILE_READWHOLEIMAGE);
	if (IJL_OK != jerr) {
		TRACE("Error at ijlRead\n");
		goto EXIT;
	}
	
	pJpeg->pImage = jcp.DIBBytes;

	rs = ParseAppBlocks(pszPath, pJpeg, nAppBlockSize) > 0 ? TRUE : FALSE;
EXIT:
	ijlFree(&jcp);
//	m_cs.Unlock();
	return rs;
}

BOOL SaveBMP(LPCTSTR pszPath, BITMAPINFOHEADER &bmih, BYTE *pImage)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bm;

	FILE *fp;

	bmfh.bfType = 'MB';
	bmfh.bfSize = bmih.biWidth*bmih.biHeight*2 + sizeof(BITMAPFILEHEADER) 
		+ sizeof(BITMAPINFOHEADER);
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bm = bmih;
	bm.biHeight *= -1;

	if (_tfopen_s(&fp, pszPath, _T("wb"))) {
		return FALSE;
	}

	fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&bm, 1, sizeof(BITMAPINFOHEADER), fp);
	fwrite(pImage, 1, bmih.biWidth*bmih.biHeight*3, fp);

	fclose(fp);
	return TRUE;
}

