#pragma once


struct T_ConverterParameters{
	int videoCode;
	int width;
	int height;
	int fps;
	int videoBitrate;
	int audioCode;
	int samplerate;
	int channel;
	int audioBitrate;
};

typedef struct
{
	long nPort;
	HDC hdc;
	char * pBuf;
	long nBufLen;
	long nWidth;
	long nHeight;
	unsigned long long nStamp;
	long nType;
	long nUser;
}DISPLAYCALLBCK_INFO;

typedef struct SYSTEM_TIME //绝对时间
{
	unsigned long dwYear;   //年
	unsigned long  dwMon;    //月
	unsigned long  dwDay;    //日
	unsigned long  dwHour;   //时
	unsigned long  dwMin;    //分
	unsigned long  dwSec;    //秒
	unsigned long  dwMs;     //毫秒
}SYSTEM_TIME;

/*
typedef void(CALLBACK* FileEndCallback)(long nID, void *pUser);
typedef void(CALLBACK* DisplayCallback)(DISPLAYCALLBCK_INFO *pstDisplayInfo);
*/
typedef void(__stdcall* FileEndCallback)(long nID, void *pUser);
typedef void(__stdcall* DisplayCallback)(DISPLAYCALLBCK_INFO *pstDisplayInfo);