#ifndef __INTERFACE_VCA_VIDEO_SOURCE_H__
#define __INTERFACE_VCA_VIDEO_SOURCE_H__


class IVCAVideoSource
{
public:
	typedef enum {RAWFILESOURCE, CAP5SOURCE, COMPRESSEDFILESOURCE, STREAMSOURCE, DSHOWSOURCE, VIRTUALSOURCE, NOTSETSOURCE = -1} eSOURCETYPE;
	typedef enum {CMD_SET_VIDEOFORMAT = 1, CMD_SET_COLORFORMAT, CMD_SET_IMGSIZE, CMD_SET_FRAMERATE, 
		CMD_GET_VIDEOFORMAT = 0x10, CMD_GET_COLORFORMAT, CMD_GET_IMGSIZE, CMD_GET_FRAMERATE, CMD_GET_NUM_DEVICES, CMD_GET_DEVICE, CMD_SET_DEVICE,
		CMD_GET_NUM_MEDIAFORMATS, CMD_GET_MEDIAFORMAT, CMD_SET_MEDIAFORMAT, CMD_ADD_MEDIASAMPLE } eCMD;

public:
	virtual BOOL	Open(LPCTSTR pszFile, DWORD param) = 0;
	virtual void	Close()		= 0;
	virtual BOOL	Start()		= 0;
	virtual void	Stop()		= 0;
	virtual BOOL	Control(eCMD cmd, DWORD_PTR param1, DWORD_PTR param2) = 0;

	virtual BOOL	GetRawImage(BYTE **pRAWData, FILETIME *timestamp, DWORD *bEOF)	= 0;
	virtual BOOL	ReleaseRawImage() = 0;

	virtual BOOL	GetBufferLen( unsigned int *puiMaxLen, unsigned int *puiCurLen ) { return FALSE; }

	virtual	eSOURCETYPE GetSourceType() = 0;
	virtual	HANDLE	GetEvent() { return NULL; }

	static LPCTSTR	GetSourceTypeStr( eSOURCETYPE eType )
	{
		switch( eType )
		{
		case RAWFILESOURCE: return _T("RAW");
		case COMPRESSEDFILESOURCE:	return _T("COMPRESSED");
		case STREAMSOURCE:		return _T("STREAM");
		case DSHOWSOURCE:		return _T("DIRECTSHOW");
		case CAP5SOURCE:		return _T("CAP5");
		case NOTSETSOURCE:		return _T("UNCONFIGURED");
		case VIRTUALSOURCE:		return _T("VIRTUAL");
		default:				return _T("UNKNOWN");
		}
	}

	static BOOL IsFileSource(DWORD type) {return (  
			((eSOURCETYPE) type == RAWFILESOURCE) || 
			((eSOURCETYPE) type == COMPRESSEDFILESOURCE) || 
			((eSOURCETYPE) type == STREAMSOURCE) || 	
			((eSOURCETYPE) type == DSHOWSOURCE) );}
};


IVCAVideoSource* CreateVCAVideoSource(IVCAVideoSource::eSOURCETYPE type, LPCTSTR pszFileName, DWORD param);
void	DestroyVCAVideoSource(IVCAVideoSource* pVCAVideoSource);

#endif