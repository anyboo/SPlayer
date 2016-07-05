#include "stdafx.h"
#include "CAP5VideoSource.h"
#include "CAP5System.h"
#include <VCA5CoreLib.h>

CCAP5VideoSource::CCAP5VideoSource(void)
{
	m_pCAP5System	= NULL;
	m_BdId			= 0xFFFFFFFF;
	m_ChId			= 0xFFFFFFFF;
	m_pQueue		= NULL;
	m_hEvent		= NULL;

}

CCAP5VideoSource::~CCAP5VideoSource(void)
{
	delete m_pQueue;
	
}

float GetBytePerPixelOfVCA5Color(DWORD VCA5ColorType)
{
	switch(VCA5ColorType){
		case VCA5_COLOR_FORMAT_YUY2:
		case VCA5_COLOR_FORMAT_UYVY:
		case VCA5_COLOR_FORMAT_RGB16:
			return 2;

		case VCA5_COLOR_FORMAT_YV12:
			return 1.5;

		case VCA5_COLOR_FORMAT_RGB24:
			return 3;

		case VCA5_COLOR_FORMAT_Y8:
			return 1;
	}

	return 4;
}

BOOL	CCAP5VideoSource::Open(LPCTSTR pszFile, DWORD param)
{
	if(m_pCAP5System){
		TRACE("m_pCAP5System Not set");	
		return FALSE;
	}

	m_pCAP5System	= (CCap5System*)pszFile;
	m_BdId	= ((param&0xFFFF0000) >> 16);
	m_ChId	= (param&0x0000FFFF);
	

	if(!m_pCAP5System->RegisterVideoSource(m_BdId, m_ChId, this)){
		TRACE("Can not register to CAP5System BD[%d] CH[%d] \n", m_BdId, m_ChId);
		goto EXIT;
	}
	m_hEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);

	if(m_pQueue == NULL) {
		DWORD	Data;
		ICap5*	pCAP5API = m_pCAP5System->GetCAP5Interface();
		if( !pCAP5API->Cap5GetImageSize(m_BdId, m_ChId, &Data) ){
			goto EXIT;
		}

		DWORD width	= VCA5_GETIMGWIDTH(Data);
		DWORD height= VCA5_GETIMGHEIGHT(Data);

		if( !pCAP5API->Cap5GetColorFormat(m_BdId, &Data) ){
			goto EXIT;
		}

		float fMaxImgSize = width * height * GetBytePerPixelOfVCA5Color(Data);
		m_pQueue = new CImageDataQueue(5, (size_t)fMaxImgSize);
	}


	return TRUE;

EXIT:
	m_pCAP5System	= NULL;
	m_BdId	= m_ChId = 0xFFFFFFFF;
	return FALSE;
}

void	CCAP5VideoSource::Close()
{
	if(m_pCAP5System){
		m_pCAP5System->UnRegisterVideoSource(m_BdId, m_ChId);
		m_pCAP5System = NULL;
	}
	m_BdId	= m_ChId = 0xFFFFFFFF;
	CloseHandle(m_hEvent);
}


BOOL	CCAP5VideoSource::GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF )
{
	CImageDataQueue::IMAGEDATA 	*pImageData = m_pQueue->Get();
	*bEOF	= 0;
	if(!pImageData){
		return FALSE;
	}
	*pRAWData = pImageData->pData;
	*timestamp= pImageData->TimeStamp;
	return TRUE;
}


BOOL	CCAP5VideoSource::InsertImage(BYTE *pRAWData, FILETIME timestamp, DWORD len)
{
	CImageDataQueue::IMAGEDATA ImageData = {pRAWData, timestamp, len};
	if(!m_pQueue->Add(&ImageData)){
		SetEvent(m_hEvent);
		//TRACE("Can not Insert ImageQueue BD[%d] CH[%d] \n", m_BdId, m_ChId);
		return FALSE;
	}
	SetEvent(m_hEvent);
	return TRUE;
}


BOOL	CCAP5VideoSource::Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2)
{
	ICap5*	pCAP5API = m_pCAP5System->GetCAP5Interface();
	DWORD dwParam1 = (DWORD)param1;
	BOOL	bResult = FALSE;
	DWORD	Data;

	switch(cmd){
		case CMD_SET_VIDEOFORMAT:
			if(pCAP5API->Cap5SetVideoFormat(m_BdId, dwParam1)){
				bResult = TRUE;
			}break;
			
		case CMD_SET_COLORFORMAT:
			if(pCAP5API->Cap5SetColorFormat(m_BdId, dwParam1)){
				bResult = TRUE;
			}break;
			
		case CMD_SET_IMGSIZE:
			if(pCAP5API->Cap5SetImageSize(m_BdId, m_ChId, dwParam1)){
				bResult = TRUE;
			}break;	

		case CMD_SET_FRAMERATE:
			if(m_pCAP5System->SetFramerate(m_BdId, m_ChId, dwParam1)){
				bResult = TRUE;
			}break;	

		case CMD_GET_VIDEOFORMAT:
			if(pCAP5API->Cap5GetVideoFormat(m_BdId, &Data)){
				(*(DWORD *) param1) = Data;
				bResult = TRUE;
			}break;
			
		case CMD_GET_COLORFORMAT:
			if(pCAP5API->Cap5GetColorFormat(m_BdId, &Data)){
				(*(DWORD *) param1) = Data;
				bResult = TRUE;
			}break;
			
		case CMD_GET_IMGSIZE:
			if(pCAP5API->Cap5GetImageSize(m_BdId, m_ChId, &Data)){
				(*(DWORD *) param1) = Data;
				bResult = TRUE;
			}break;
			
		case CMD_GET_FRAMERATE:
			(*(DWORD *) param1) = m_pCAP5System->GetFramerate(m_BdId, m_ChId);
			bResult = TRUE;
			break;

		default:
			break;
	}

	return bResult;
}
