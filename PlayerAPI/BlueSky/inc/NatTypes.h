#ifndef _NATTYPES_H
#define _NATTYPES_H


#define IN
#define OUT
#define INOUT

#define DEV_ID_LEN		64


/*NAT类型结构*/
typedef enum
{
	NAT_TYPE_UNKONWN = 0,
    NAT_TYPE_FULL_CONE = 1,   //完全圆锥体.一个完全圆锥体的NAT
    NAT_TYPE_RESTRICTED_CONE, //受限制的圆锥体.
    NAT_TYPE_PORT_RESTRICTED_CONE, //端口受限制的圆锥体NAT.
    NAT_TYPE_SYMMETRIC_CONE,  //对称的NAT
	NAT_TYPE_HOST             // 不需要nat

}NATTYPE;


/*设备类型*/
typedef enum
{
	EDEVICE_TYPE_NVR = 1,   
	EDEVICE_TYPE_DVR,
	EDEVICE_TYPE_IPC, 
	EDEVICE_TYPE_Bplayer,
	EDEVICE_TYPE_SDK

}DEVTYPE;

/*NAT连接的类型*/
typedef enum
{
	NAT_CON_TURN_UDP = 1, /*Turn 连接*/
	NAT_CON_TURN_TCP,
	NAT_CON_STUN_UDP,      /*Stun连接*/
	NAT_CON_STUN_TCP,
	NAT_CON_TURN_REAL_TCP
}NATCONTYPE;


//获取在线设备信息
typedef struct 
{
	char devId[DEV_ID_LEN];		/*设备唯一码*/
	int lastHeartTime;	/*最后心跳时间*/
	NATTYPE natType;   /*设备nat类型*/
}DevInfo;

/*服务器返回的在线设备*/
typedef struct 
{
	int count;
	DevInfo devInfo[0];
}DevListResponse;


typedef struct _NatSockaddr_in_
{
	short family;          /*AF_INET（地址族）PF_INET（协议族）*/
	unsigned short port;   /*端口号,需要用htons()函数转换成网络数据格式的数字)*/
	unsigned int naddr;           /* 按照网络字节顺序存储IP地址 */
	unsigned char sin_zero[8]; /* 扩展、预留 */
}NatSockaddr_in;


/*
 直接设置客户端数据接收的回调函数
*/
typedef int (*pfnNatClientRecvCallback)(int natSessionId, void* data, int len, void* context);


#endif