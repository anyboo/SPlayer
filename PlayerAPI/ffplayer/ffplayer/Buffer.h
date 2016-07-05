#pragma once
#include <stdlib.h>
#include <list>
#include <mutex>

#define FRAME_COUNT 4

typedef struct
{
	int		iFrameIndex;		///< 帧索引号
	short		iFrameType;		///< 帧类型
	unsigned int	iFrameLen;
	unsigned __int64		iFilePos;			///< 帧在文件中位置
	double	iTimestamp;		///< 帧时间戳差值
//	unsigned __int64		iTimestampObsolute;	///< 绝对时间
	int64_t 	iTimestampObsolute;	///< 绝对时间
	bool		bNewPos;	///< 标识是否发生了变化（seek),如果发生了变化，表示不用delay
	int serial;
}VIDEO_FRAME;


typedef struct T_Info//双向链表
{
	//AVPacket *pk;
	VIDEO_FRAME frameInfo;
	unsigned char *Data;
	int DataLen;
	int size;
	int width;
	int height;
}*PInfo;


class CBuffer
{
public:
	CBuffer();
	~CBuffer();

public:
	static void freeBuf(PInfo *pInfo);
	static PInfo createBuf(int len);
	//void createBuffer(int len=(2800*2800*3/2), int count = FRAME_COUNT);

	void destoryBuffer();

	void insertList(PInfo pInfo);

	PInfo getInfoFromList();

	PInfo getFreeInfoFromList(int len);

	int getCurCount();

public:
//	Event m_eventUsed;
//	Event m_eventValid;
private:
	std::list<PInfo> m_List;
	std::recursive_mutex m_mutexList;
};
