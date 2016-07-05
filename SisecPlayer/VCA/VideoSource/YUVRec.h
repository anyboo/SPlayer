#ifndef __YUVREC_H__
#define __YUVREC_H__


class CYUVRec
{

public:
typedef enum{
	YUV_NONE,
	YUY2, 
	UYVY, 
	YV12,
	GRAY,
}YUVTYPE;

typedef struct
{
	DWORD	type;
	DWORD	width;
	DWORD	height;
}YUVHEADER;


public:
	CYUVRec(void);
	~CYUVRec(void);

	BOOL	Open(LPCTSTR lpFilename,  BOOL bWrite, DWORD type = 0, DWORD width = 0, DWORD height = 0);
	void	Close();
	BOOL	Write(DWORD index, BYTE* buf, DWORD size);
	BOOL	Read(DWORD index, BYTE* buf, DWORD size);
	
	YUVHEADER* GetHeader(){return &m_YUVHeader;}
	UINT	GetTotalFrames(){return (m_hFile)?m_TotalFrames:0;}

private:
	FILE*		m_hFile;
	YUVHEADER	m_YUVHeader;
	BOOL		m_bWritten;
	UINT		m_TotalFrames;

	UINT		GetTotalFrames_i();
};

inline UINT	GetFrameSize(CYUVRec::YUVHEADER &h)
{
	if(CYUVRec::GRAY == h.type){
		return (h.width*h.height);	
	}else if(CYUVRec::YV12 == h.type){
		return (h.width*h.height*3)/2;	
	}else if(CYUVRec::YUY2 == h.type || CYUVRec::UYVY == h.type){
		return (h.width*h.height*2);	
	}
	return 0;
}

inline UINT	GetFrameSize(CYUVRec::YUVHEADER *h)
{
	if(CYUVRec::GRAY == h->type){
		return (h->width*h->height);	
	}else if(CYUVRec::YV12 == h->type){
		return (h->width*h->height*3)/2;	
	}else if(CYUVRec::YUY2 == h->type || CYUVRec::UYVY == h->type){
		return (h->width*h->height*2);	
	}
	return 0;
}



#endif