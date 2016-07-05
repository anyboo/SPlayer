#include "StdAfx.h"
#include "AviDemux.h"
#include <assert.h>

#include <dshow.h>
#include <aviriff.h>

#define BUFFER_SIZE 10 * 1024 * 1024
#define BUFFER_HEADROOM 1024 * 1024

AviDemux::AviDemux(void)
{
	m_pIf	= NULL;
	m_pBuf	= NULL;

	m_pBuf = new unsigned char[ BUFFER_SIZE ];
	m_pHead = m_pBuf;
	m_pTail = m_pBuf;
	m_pEnd = m_pBuf + BUFFER_SIZE;
}

AviDemux::~AviDemux(void)
{
	delete [] m_pBuf;
}

void AviDemux::SetCallback( AviDemuxCallback *pIf )
{
	m_pIf = pIf;
}

unsigned char *AviDemux::GetWritePtr()
{
	return m_pHead;
}

unsigned int AviDemux::GetBufferSize()
{
	return (unsigned int)(m_pEnd - m_pHead);
}

// NB - plenty of assumptions in here about the minimum packet length delivered
// aim is to keep the code simple, so much error checking omitted
void AviDemux::DataAdded( unsigned int uiLen, void *pUserData )
{
	// Check for new stream
	RIFFCHUNK *pChunk = (RIFFCHUNK *)m_pHead;
	if( FOURCC_RIFF == pChunk->fcc )
	{
		// Reset tail pointer to skip past old data
		m_pTail = m_pHead;
	}

	m_pHead += uiLen;

	// Check for buffer wrap
	if( m_pEnd - m_pHead < BUFFER_HEADROOM )
	{
		int iDataLen = int(m_pHead - m_pTail);

		// Not much room left, move everything back to the beginning
		assert( m_pTail > m_pBuf );

		// Should be at least 50% free (i.e. make sure no overlap in this copy)
		assert( iDataLen < (m_pTail - m_pBuf) );

		memcpy( m_pBuf, m_pTail, iDataLen );
		m_pTail = m_pBuf;
		m_pHead = m_pTail + iDataLen;
	}


	unsigned char *p = m_pTail;
	int iDataLen = int(m_pHead - m_pTail);

	bool bCarryOn = true;
	while( bCarryOn )
	{
		pChunk = (RIFFCHUNK *)p;

		switch( pChunk->fcc )
		{
			case FOURCC_RIFF:
			{
				// RIFF Header
				p += 12;
		//		p = ProcessChunk( p, bCarryOn );

				// ASSUMPTION: There will always be enough to get past the RIFF header
			}
			break;

			case FOURCC_LIST:
			{
				// List chunk
				RIFFLIST *pList = (RIFFLIST *)pChunk;
				p += 12;

				p = ProcessChunk( p, bCarryOn, pUserData );
			}
			break;



			default:
			{
//				CString s;
//				char *c = (char *)&(pChunk->fcc);
//				s.Format( _T("Got CHUNK size %d FCC:%c%c%c%c\n"), pChunk->cb, c[0], c[1], c[2], c[3] );
//				OutputDebugString(s);

				p = ProcessChunk( p, bCarryOn, pUserData );
			}
		}

		if( bCarryOn )
		{
			// We processed at least one chunk, so move the tail forward
			m_pTail = p;
		}
	}
}

unsigned char *AviDemux::ProcessChunk( unsigned char *p, bool &bCarryOn, void *pUserData )
{
	// See if there's enough data to fully process this chunk
	int iDataLen = int(m_pHead - p);

	RIFFCHUNK *pChunk = (RIFFCHUNK *)p;

	if( iDataLen < sizeof( RIFFCHUNK ) )
	{
		bCarryOn = false;
		return p;
	}

	if( iDataLen >= (int)(pChunk->cb + 8))
	{
		// OK
		// Process the chunk

		switch( pChunk->fcc )
		{
			case ckidMAINAVIHEADER:
			{
				AVIMAINHEADER *pHdr = (AVIMAINHEADER *)p;

				m_pIf->OnMainHeader( *pHdr, pUserData );
			}
			break;

			case ckidSTREAMHEADER:
			{
				AVISTREAMHEADER *pHdr = (AVISTREAMHEADER *)p;
				m_pIf->OnStreamHeader( *pHdr, pUserData );

			}
			break;

			case ckidSTREAMFORMAT:
			{
		//		AVISTREAMFORMAT *pFmt = (AVISTREAMFORMAT *)p;

				m_pIf->OnStreamFormat( p+8, pChunk->cb, pUserData );
			}
			break;

			default:
			{
				// Default - data chunk
				// Do the callback
				m_pIf->OnNewFrame( pChunk->fcc, p + 8, pChunk->cb, pUserData );

			}
		}

		// Move the data pointer on since this chunk is fully processed now


		
		pChunk = RIFFNEXT( pChunk );
		bCarryOn = true;
	}
	else
	{
		bCarryOn = false;
	}

	return (unsigned char *)pChunk;
}
