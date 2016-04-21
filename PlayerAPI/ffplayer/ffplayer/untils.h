#include "ffplayer.h"


int YUV420_RGB_KP(AVFrame *YUV420Pic, unsigned char *pucArrRGBData, const int nWidth, const int nHeight);

void writeBMPFile(char* file, char *pRGBBuf, int m_nWidth, int m_nHeight);

void writeJPGFile(char* file, char *pRGBBuf, int m_nWidth, int m_nHeight);