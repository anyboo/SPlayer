// RawFileVideoSource.cpp: implementation of the CRawFileVideoSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RawFileVideoSource.h"
#include <malloc.h>
#include "CAP5BoardLibEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef TRACE

#include <atlbase.h>
#define TRACE AtlTrace
#endif

BOOL	CRawFileVideoSource::Open(LPCTSTR pszFile, DWORD param)
{

	if(m_bOpen){
		TRACE("CRawFileVideoSource : Already OPened \n");
		Close();
	}

	DWORD BufSize;
	
	if(!m_YUVReader.Open(pszFile, FALSE)){
		TRACE("CRawFileVideoSource : Can not Open [%s] \n", pszFile);
		return FALSE;
	}
	
	CYUVRec::YUVHEADER* header = m_YUVReader.GetHeader();
	BufSize		= GetFrameSize(header)+1024;
	m_pData		= (BYTE*)_aligned_malloc(BufSize, 16);

	m_bOpen		= TRUE;
	m_DummyIndex= 0;
	m_hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	return TRUE;
}


void	CRawFileVideoSource::Close()
{
	if(m_bOpen){
		
		if(m_pData){
			_aligned_free(m_pData);
			m_pData	= NULL;
		}
		
		m_YUVReader.Close();
		CloseHandle(m_hEvent);
		m_bOpen = FALSE; 

	}
}


BOOL	CRawFileVideoSource::GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF )
{
	if(!m_bOpen){	
		TRACE("CRawFileVideoSource : GetRawImage not Opened  before \n");
		return FALSE;
	}

	CYUVRec::YUVHEADER *header	= m_YUVReader.GetHeader();
	DWORD	BufSize		= GetFrameSize(header)+1024;
	

	GetSystemTimeAsFileTime(timestamp);
	FileTimeToLocalFileTime(timestamp, timestamp);
		
	if(m_ReadIndex >= m_YUVReader.GetTotalFrames()){
		if(m_DummyIndex == 0){
			memset(m_pData, 0, BufSize);
		}

		if(m_DummyIndex >DUMMY_DATA_OUT){
			*bEOF = 0;
			m_ReadIndex		= 0;
			m_DummyIndex	= 0;
			return FALSE;
		}else{
			*pRAWData	= m_pData;
			*bEOF = 0;
			m_DummyIndex++;
			return TRUE;
		}
	}
	
	if(!m_YUVReader.Read(m_ReadIndex, m_pData, BufSize)){
		TRACE("CRawFileVideoSource : GetRawImage not read index[%d] \n", m_ReadIndex);
		return FALSE;
	}

	m_ReadIndex++;
	*pRAWData	= m_pData;
	*bEOF = 0;
	return TRUE;
}


BOOL	CRawFileVideoSource::Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2)
{
	switch(cmd){
		case CMD_SET_VIDEOFORMAT:
		case CMD_SET_COLORFORMAT:
		case CMD_SET_IMGSIZE:
			return TRUE;
			
		case CMD_SET_FRAMERATE:
			m_FPS = (DWORD)param1;
			return TRUE;

		case CMD_GET_COLORFORMAT:
			(*(DWORD *) param1) = CAP5_COLOR_FORMAT_YV12;
			return TRUE;

		case CMD_GET_IMGSIZE:{
			CYUVRec::YUVHEADER *pHeader = m_YUVReader.GetHeader();
			(*(DWORD *) param1) = CMN5_MAKEIMGSIZE(pHeader->width, pHeader->height);
			return TRUE;
		 }
		
		case CMD_GET_FRAMERATE:
			(*(DWORD *) param1) = m_FPS;
			return TRUE;
		
		case CMD_GET_VIDEOFORMAT:
			(*(DWORD *) param1) = CMN5_VIDEO_FORMAT_NTSC_M;
			return TRUE;
	}
	
	return FALSE;
}
