#pragma once

/** 
 * APP1: TimeStamp, length 8
 * APP2: Zone, length 
 * APP3: BLOB, length width*height/8
 */

#include "VCAConfigure.h"
#include "VCAMetaLib.h"


#define MAX_BYTEBLOB_SIZE	(25920)
#define MAX_BITBLOB_SIZE	(3240)

typedef struct
{
	// IN
	BYTE	*pBuffer;
	
	// OUT
	BITMAPINFOHEADER	bm;
	DWORD		dwLength;
	__int64		i64TimeStamp;

	// Theses point to pBuffer
	BYTE				*pImage;
	VCA5_APP_ZONE		*pZone;
	VCA5_RULE			*pRule;
	VCA5_PACKET_OBJECT	*pObject;
} Jpeg;

class CJpegCodec
{
public:
	CJpegCodec(void);
	~CJpegCodec(void);
	
	BOOL	Encode(LPCTSTR pszPath, BYTE *pImage, BITMAPINFOHEADER *pbm,  __int64 i64TimeStamp,
					  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject);
	// Get Jpeg dwLength and bm fields for a given JPEG
	DWORD	GetJpegInformation(LPCTSTR pszPath, Jpeg *pJpeg);
	BOOL	Decode(LPCTSTR pszPath, Jpeg *pJpeg);
private:
//	static CCriticalSection m_cs;
	BYTE	*m_pYUY2Buffer;
	DWORD	m_dwYUY2Length;
	BYTE	*m_pJpgBuffer;
	BYTE	*m_pBlobBuf;
	DWORD	m_dwJpgLength;
	BYTE	m_pHeader[20];
	BYTE	m_pTempBITBlob[MAX_BITBLOB_SIZE];
	BYTE	m_pTempBYTEBlob[MAX_BYTEBLOB_SIZE];


	void ReallocBuffer(BITMAPINFOHEADER *pbm);
	DWORD CompressYUY2(BYTE *pImage, BITMAPINFOHEADER *pbm, BYTE *pDst, DWORD dwLength);
	DWORD CompressRGB(BYTE *pImage, BITMAPINFOHEADER *pbm, BYTE *pDst, DWORD dwLength);
	BOOL SaveToFile(LPCTSTR pszPath, BYTE *pImage, BITMAPINFOHEADER *pbm,  __int64 i64TimeStamp,
					  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject);

	DWORD InsertAppBlocks(BYTE *pJpgBuffer, DWORD dwLength,  BITMAPINFOHEADER *pbm, __int64 i64TimeStamp,
					  VCA5_APP_ZONE *pZone, VCA5_RULE *pRule, VCA5_PACKET_OBJECT *pObject);
	DWORD GetSizeOfAppBlocks(LPCTSTR pszPath, Jpeg *pJpeg);
	DWORD ParseAppBlocks(LPCTSTR pszPath, Jpeg *pJpeg, DWORD AppBlockSize);	
	BOOL ParseApp0Block(LPCTSTR pszPath, BITMAPINFOHEADER &bi);
};
