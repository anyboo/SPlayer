#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "YUVRec.h"


CYUVRec::CYUVRec(void)
{
	m_hFile			= NULL;
	m_TotalFrames	= 0;
	memset(&m_YUVHeader,0,sizeof(YUVHEADER));
}

CYUVRec::~CYUVRec(void)
{
	Close();
}


BOOL	CYUVRec::Open(LPCTSTR lpFilename,  BOOL bWrite, DWORD type, DWORD width, DWORD height)
{
	if(m_hFile){
		Close();
	}
	TCHAR* mode;

	if(bWrite){
		mode = _T("w+b");
	}else{
		mode = _T("rb");	
	}

	if (0 != _tfopen_s(&m_hFile, lpFilename, mode)) {
		return FALSE;
	}
	
	if(bWrite){
		m_YUVHeader.type	= type;
		m_YUVHeader.width	= width;
		m_YUVHeader.height	= height;
		fwrite(&m_YUVHeader, sizeof(YUVHEADER),1, m_hFile);
		m_TotalFrames = 0;
	}else{
		size_t size;
		size = fread(&m_YUVHeader, 1, sizeof(YUVHEADER), m_hFile);
		if(sizeof(YUVHEADER) < size){
			Close();
			return FALSE;
		}

		m_TotalFrames = GetTotalFrames_i();
	}

	
	m_bWritten = bWrite;
	return TRUE;
}


void	CYUVRec::Close()
{
	if(m_hFile){
		fclose(m_hFile);
		m_hFile	= NULL;
		m_TotalFrames = 0;
	}
}



UINT	CYUVRec::GetTotalFrames_i()
{
	UINT framesize, filesize;
	fseek( m_hFile, 0, SEEK_END );
	filesize = ftell(m_hFile);
    fseek( m_hFile, 0, SEEK_SET );

	framesize = GetFrameSize(m_YUVHeader);
	return (filesize-sizeof(YUVHEADER))/framesize;
}


BOOL	CYUVRec::Write(DWORD index, BYTE* buf, DWORD size)
{
	if(!m_hFile){
		return FALSE;
	}
	UINT pos = index*GetFrameSize(m_YUVHeader)+sizeof(YUVHEADER);
	fseek(m_hFile,pos, SEEK_SET);
	size_t wsize;

	wsize = fwrite(buf, 1, size, m_hFile);
	return ((DWORD)wsize==size);
}


BOOL	CYUVRec::Read(DWORD index, BYTE* buf, DWORD size)
{
	if(!m_hFile){
		return FALSE;
	}
	UINT pos = index*GetFrameSize(m_YUVHeader)+sizeof(YUVHEADER);
	fseek(m_hFile,pos, SEEK_SET);
	size_t rsize;
	
	if(GetFrameSize(m_YUVHeader) > size){
		return FALSE;
	}
	size = GetFrameSize(m_YUVHeader);

	rsize = fread(buf, 1, size, m_hFile);

	return ((DWORD)rsize==size);

	
}
	
