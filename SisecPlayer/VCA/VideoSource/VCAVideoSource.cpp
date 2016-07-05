#include "stdafx.h"
#include "RawFileVideoSource.h"
#include "Cap5VideoSource.h"
#include "CompressedVideoSource.h"
#include "StreamVideoSource.h"
#include "DShowVideoSource.h"
#include "VirtualVideoSource.h"


IVCAVideoSource* CreateVCAVideoSource(IVCAVideoSource::eSOURCETYPE type, LPCTSTR pszFileName, DWORD param)
{
	IVCAVideoSource *pSource = NULL;
	if (IVCAVideoSource::RAWFILESOURCE == type) {
		pSource = new CRawFileVideoSource;
	}else if(IVCAVideoSource::CAP5SOURCE == type) {
		pSource = new CCAP5VideoSource;
	}else if (IVCAVideoSource::COMPRESSEDFILESOURCE == type) {
		pSource = new CCompressedFileVideoSource;
	}else if (IVCAVideoSource::STREAMSOURCE == type ) {
		pSource = new CStreamVideoSource;
	}else if (IVCAVideoSource::DSHOWSOURCE == type ) {
		pSource = new CDShowVideoSource;
	}else if (IVCAVideoSource::VIRTUALSOURCE == type ) {
		pSource = new CVirtualVideoSource;
	}else{
		return NULL;
	}
	
	if(!pSource->Open(pszFileName, param)){
		DestroyVCAVideoSource(pSource);	
		return NULL;
	}
	return pSource;
}


void	DestroyVCAVideoSource(IVCAVideoSource* pVCAVideoSource)
{
	if (!pVCAVideoSource) return;

	IVCAVideoSource::eSOURCETYPE type = pVCAVideoSource->GetSourceType();
	pVCAVideoSource->Close();
	if (type == IVCAVideoSource::RAWFILESOURCE) {
		delete (CRawFileVideoSource*)pVCAVideoSource;
	}else if(type == IVCAVideoSource::CAP5SOURCE){
		delete (CCAP5VideoSource*)pVCAVideoSource;
	}else if (type == IVCAVideoSource::COMPRESSEDFILESOURCE) {
		delete (CCompressedFileVideoSource *)pVCAVideoSource;
	}else if(type == IVCAVideoSource::STREAMSOURCE){
		delete (CStreamVideoSource *)pVCAVideoSource;
	}else if (type == IVCAVideoSource::DSHOWSOURCE) {
		delete (CDShowVideoSource *)pVCAVideoSource;
	}else {
		ASSERT(0);
	}
}

