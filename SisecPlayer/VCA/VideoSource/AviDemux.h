#pragma once

struct _avimainheader;
typedef struct _avimainheader AVIMAINHEADER;
struct _avistreamheader;
typedef struct _avistreamheader AVISTREAMHEADER;

class AviDemuxCallback
{
public:
	virtual void OnMainHeader( AVIMAINHEADER &avimh, void *pUserData ) {;}
	virtual void OnStreamHeader( AVISTREAMHEADER &avish, void *pUserData ) {;}
	virtual void OnStreamFormat( unsigned char *pFmtBuf, unsigned int uiLen, void *pUserData ) {;}
	virtual void OnNewFrame( DWORD dwFourCC, unsigned char *pBuf, unsigned int uiLen, void *pUserData ) {;}
	virtual void OnStreamEnd( void *pUserData) {;}
};

class AviDemux
{
public:
	AviDemux(void);
	~AviDemux(void);

	void SetCallback( AviDemuxCallback *pIf );
	void DataAdded( unsigned int uiLen, void *pUserData );
	unsigned char *GetWritePtr();
	unsigned int GetBufferSize();

protected:
	unsigned char *ProcessChunk( unsigned char *p, bool &bCarryOn, void *pUserData );

private:
	AviDemuxCallback	*m_pIf;
	unsigned char		*m_pBuf;
	unsigned char		*m_pTail;
	unsigned char		*m_pHead;
	unsigned char		*m_pEnd;
};
