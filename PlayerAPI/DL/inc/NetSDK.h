#ifndef NETDLSDK_INTERFACE_H
#define NETDLSDK_INTERFACE_H

#include <winsock.h>

#ifdef  NETSDK_EXPORTS
#define NET_DLDVR_API __declspec(dllexport)
#else
#define NET_DLDVR_API __declspec(dllimport)
#endif

#define STDCALL __stdcall	

typedef		LONG						LOGIN_HANDLE;
typedef 	LONG						LIVE_HANDLE;
typedef 	LONG						PLAYBACK_HANDLE;
typedef		unsigned int				SOCKET;
typedef  unsigned char		U8;
typedef  unsigned short		U16;
typedef  unsigned int		U32;

typedef struct _MONITOR_INFO
{
	U8			has_audio;		//是否含有音频 1:含有,0:没有包含
	U8			quality;		//质量 0:"差", 1:"普通", 2:"好", 3:"较好", 4:"最好"
	U8			frame_rate;		//帧率 0 ~ 25
	U8			video_format;	//视频格式 0:PAL, 1:NTSC
	U8			resolution;		//分辨率
	U32			Mcast_IP;		//多播组地址(如果DVR不是使用多播方式则返回0)
	U16			Mcast_port;		//多播组地址(如果DVR不是使用多播方式则返回0)
}MONITOR_INFO;

/************************************************************************/
/* 错误值定义开始                                                       */
/************************************************************************/
#define NET_DLDVR_NOERROR 				0     //没有错误



#define NET_DLDVR_ERROR_CREATESOCKET	-10		//创建socket失败
#define NET_DLDVR_ERROR_CONNECT			-40		//建立连接失败
#define NET_DLDVR_ERROR_RECV			-50		//接收数据失败
#define NET_DLDVR_ERROR_STOP_STATUS		-60		//非法状态
#define NET_DLDVR_ERROR_MONITORSTOP		-100	//关闭监视失败
#define NET_DLDVR_ERROR_GETVIDEOEFFECT	-200	//获取视频参数失败
#define NET_DLDVR_ERROR_SETVIDEOEFFECT	-300	//设置视频参数失败
#define NET_DLDVR_ERROR_GENEIFRAME		-400	//生成关键帧失败
#define NET_DLDVR_ERROR_PTZCONTROL		-500	//云台控制失败
#define NET_DLDVR_ERROR_TRANSPTZ		-600	//透明云台控制失败
#define NET_DLDVR_ERROR_PTZPRESET		-700	//设置预置位失败
#define NET_DLDVR_ERROR_FINDFILEOPEN	-800	//开始查找文件失败
#define NET_DLDVR_ERROR_FINDFILENEXT	-900	//查找下一条文件失败
#define NET_DLDVR_ERROR_FINDFILECLOSE	-1000	//关闭查找文件失败

//查找文件和日志函数返回值
#define NET_DLDVR_ERROR_FINDFILE_SUCCESS	0	//获得文件信息
#define NET_DLDVR_ERROR_FILE_NOFIND			-1001	//没有文件
#define NET_DLDVR_ERROR_ISFINDING			-1002	//正在查找文件
#define	NET_DLDVR_ERROR_NOMOREFILE			-1003	//查找文件时没有更多的文件
#define	NET_DLDVR_ERROR_FILE_EXCEPTION		-1004	//查找文件时异常
#define	NET_DLDVR_ERROR_NO_PLAYSPACE		-1005	//没有回放空间，主机最多允许4个播放用户同时在线
#define	NET_DLDVR_ERROR_NO_POTENCE			-1006	//用户没有权限
#define NET_DLDVR_ERROR_ENDOF_FILE			-1007	//已到文件尾部

#define NET_DLDVR_HTTP_RESPON_ERROR		-1060		//Http回应出错

#define	NET_DLDVR_ERROR_PLAYBYNAME		-1100	//根据名字回放失败
#define	NET_DLDVR_ERROR_LOCKFILE		-1110	//锁定文件失败
#define	NET_DLDVR_ERROR_ENTER_CSLOCK	-1111	//不能获得互斥锁
#define NET_DLDVR_ERROR_PLAYBYTIME		-1200	//根据时间回放失败
#define	NET_DLDVR_ERROR_DATA_READ		-1210	//读取录像数据失败	
#define NET_DLDVR_ERROR_PLAYSEEK		-1300	//播放时移动位置失败
#define NET_DLDVR_ERROR_DOWNLOADBYNAME	-1400	//根据名字下载失败
#define NET_DLDVR_ERROR_DOWNLOADBYTIME	-1500	//根据时间下载失败
#define NET_DLDVR_ERROR_THREAD_FINISHED -1550	//下载线程已经结束
#define NET_DLDVR_ERROR_REPORTSTART		-1600	//启动实时上报失败
#define NET_DLDVR_ERROR_REPORTSTOP		-1700	//关闭实时上报失败
#define NET_DLDVR_ERROR_SETALARMOUT		-1800	//设置告警输出失败
#define NET_DLDVR_ERROR_GETALARMOUT		-1900	//获取告警输出失败
#define NET_DLDVR_ERROR_PANELKEY		-2000	//按下远程面板按钮出错
#define NET_DLDVR_ERROR_RECORDSTART		-2100	//开始录像失败
#define NET_DLDVR_ERROR_RECORDSTOP		-2200	//结束录像失败
#define NET_DLDVR_ERROR_LOGFINDOPEN		-2300	//开始查询日志失败
#define NET_DLDVR_ERROR_LOGFINDNEXT		-2400	//查询下一条日志失败
#define NET_DLDVR_ERROR_LOGFINDCLOSE	-2500	//关闭查询日志失败
#define NET_DLDVR_ERROR_GETDVRSTATE		-2600	//获取DVR状态失败
#define NET_DLDVR_ERROR_GETDVRVERSION	-2601	//获取DVR版本信息失败, 2007-07-27
#define NET_DLDVR_ERROR_RESTOREDVRCFG	-2700	//恢复默认值失败
#define NET_DLDVR_ERROR_SAVEDVGFLASH	-2800	//保存参数到Flash失败
#define NET_DLDVR_ERROR_REBOOTDVR		-2900	//重启DVR失败
#define NET_DLDVR_ERROR_SHUTDOWNDVR		-3000	//关闭DVR失败
#define NET_DLDVR_ERROR_UPDATEBEGIN		-3100	//开始更新失败
#define NET_DLDVR_ERROR_UPDATEDATA		-3200	//更新数据失败
#define NET_DLDVR_ERROR_UPDATEND		-3300	//结束更新失败
#define NET_DLDVR_ERROR_FLASHPEPERCENT	-3400	//获取dvr写Flash进度失败	
#define NET_DLDVR_ERROR_FORMATDISK		-3500	//格式化硬盘失败
#define NET_DLDVR_ERROR_FORMATDISKSTOP	-3600	//格式化硬盘结束失败
#define NET_DLDVR_ERROR_FORMATPROGRESS	-3601	//获取格式化硬盘进度失败
#define NET_DLDVR_ERROR_LOCALLOGINMAX		-3700	//本地登陆的DVR数已经最大
#define NET_DLDVR_ERROR_DVRREJECTLOGIN  	-3701	//主机拒绝了登陆请求
#define NET_DLDVR_ERROR_CREATEWINDOWEVENT  	-3702	//启动定时器时等待创建窗体失败
#define NET_DLDVR_ERROR_DVRREJECTTRANSSERIAL_START -3703	//主机拒绝了建立透明通道的请求
#define NET_DLDVR_ERROR_DVRREJECTTRANSSERIAL_CTRL  -3704	//主机拒绝了透明通道上的命令请求
#define NET_DLDVR_ERROR_DVRREJECTTRANSSERIAL_STOP  -3705	//主机拒绝了停止透明通道上的请求
#define NET_DLDVR_ERROR_NOLOGIN				-3800	//用户没有登陆
#define NET_DLDVR_ERROR_MAX_DOWNLOADING		-3900 //正在下载的文件数目已经达到最大值
#define NET_DLDVR_ERROR_MAX_UPGRADING		-4000 //正在升级的Image数目已经达到最大值
#define NET_DLDVR_ERROR_MAX_FORMAT			-4001 //正在格式化的硬盘数目已经达到最大值
#define NET_DLDVR_ERROR_MAX_SETRESOURCE		-4002 //正在设置的字库数目已经达到最大值
#define NET_DLDVR_ERROR_PLAYBACK_THROWBFRAME		-4010	//请求丢弃B帧失败


#define NET_DLDVR_ERROR_SETLANGUAGE_BEGIN			-4100	//开始设置自定义语言失败
#define NET_DLDVR_ERROR_SETLANGUAGE_DATA			-4200	//更新自定义语言数据失败
#define NET_DLDVR_ERROR_SETLANGUAGE_END				-4300	//结束自定义语言设置失败
#define NET_DLDVR_ERROR_SETLANGUAGE_FLASHPEPERCENT	-4400	//获取dvr写Flash进度失败
#define NET_DLDVR_ERROR_DOWNLOAD_LANGUAGE			-4410	//下载语言失败

#define NET_DLDVR_ERROR_SETLOGO_BEGIN			-4500	//开始设置语言失败
#define NET_DLDVR_ERROR_SETLOGO_DATA			-4510	//更新语言失败
#define NET_DLDVR_ERROR_SETLOGO_END				-4520	//结束语言设置失败
#define NET_DLDVR_ERROR_SETLOGO_FLASHPEPERCENT	-4530	//获取dvr写Flash进度失败

#define NET_DLDVR_ERROR_SETRESOURCE_BEGIN			-4600	//开始设置资源失败
#define NET_DLDVR_ERROR_SETRESOURCE_DATA			-4610	//更新资源数据失败
#define NET_DLDVR_ERROR_SETRESOURCE_END				-4620	//结束资源设置失败
#define NET_DLDVR_ERROR_SETRESOURCE_FLASHPEPERCENT	-4630	//获取dvr写Flash进度失败

//NVR特有的错误值
#define NET_DLDVR_ERROR_DOWNLOAD_MAIL	-4700	//下载Mail失败
#define NET_DLDVR_ERROR_UPLOAD_MAIL		-4800	//上传Mail失败


#define NET_DLDVR_ERROR_SEND			-5000	//发送数据失败
#define NET_DLDVR_ERROR_UDPBIND			-5001	//绑定UDP端口失败
#define NET_DLDVR_ERROR_MCBIND			-5002	//绑定多播端口失败
#define NET_DLDVR_ERROR_SETTIMEOUT		-5003	//设置超时时间失败
#define NET_DLDVR_ERROR_JOINMC			-5004	//加入多播组失败
#define NET_DLDVR_ERROR_ACCEPT			-5005	//accept失败
#define NET_DLDVR_ERROR_SELECT			-5006	//SELECT函数执行失败
#define NET_DLDVR_ERROR_CLOSESOCKET		-5007	//关闭套接字失败
#define NET_DLDVR_ERROR_JOINMUTICAST	-5008	//加入多播组失败
#define NET_DLDVR_ERROR_STARTUPSOCKET	-5009	//初始化套接字错误
#define NET_DLDVR_ERROR_CREATEEVENT		-5010	//创建事件错误
#define NET_DLDVR_ERROR_SELECTEVENT		-5011	//套接字绑定事件的时候出错
#define NET_DLDVR_ERROR_SETEVENT		-5012	//设置事件的状态时出错
#define NET_DLDVR_ERROR_INVALIDSOCKET	-5013	//错误的套接字
#define NET_DLDVR_ERROR_TIMEOUT			-5014	//发送接收超时
#define NET_DLDVR_ERROR_CONNRESET		-5015	//远程主机强迫关闭了现有的连接
#define NET_DLDVR_ERROR_CONNABORTED		-5016	//连接端口

//普通错误
#define NET_DLDVR_ERROR_OPENFILE			-6000	//打开文件失败
#define NET_DLDVR_ERROR_INVALIDPATH			-6001	//错误的文件或者文件夹
#define NET_DLDVR_ERROR_INVALIDDLL			-6002	//无法从动态链接库中获取该函数的地址
#define NET_DLDVR_ERROR_READFILE			-6003	//读文件错误
#define NET_DLDVR_ERROR_LOADDLL				-6004	//无法载入指定的DLL
#define NET_DLDVR_ERROR_LOADTALKDLL			-6005 //不能载入Talk DLL
#define NET_DLDVR_ERROR_LOADFRAMEDLL		-6006 //不能载入Frame DLL
#define NET_DLDVR_ERROR_FREEDLL				-6007	//无法释放指定的DLL
#define NET_DLDVR_ERROR_NOFUNCINDLL			-6008	//在dll中找不到需要的函数
#define NET_DLDVR_ERROR_ALREADSTARTUP		-6009	//DLL已经初始化过了
#define NET_DLDVR_ERROR_POORMEMORY			-6010	//申请内存的操作被操作系统拒绝
#define NET_DLDVR_ERROR_NEEDSTARTUP			-6011	//在使用之前需要先调用初始化函数
#define NET_DLDVR_ERROR_INVALIDFILEH		-6012	//错误的文件句柄
#define NET_DLDVR_ERROR_CREATETHREAD		-6013	//创建线程失败
#define NET_DLDVR_ERROR_CREATETIMER			-6014	//创建timer失败
#define NET_DLDVR_ERROR_AVIFILEOPEN			-6015	//打开一个AVI文件对话失败
#define NET_DLDVR_ERROR_CREATEAVISTREAM		-6016 //创建AVI流失败
#define NET_DLDVR_ERROR_CONVERTMP4TOAVI		-6017	//在MP4转换成AVI的过程中发生了一个错误
#define NET_DLDVR_ERROR_INVALIDPARA			-6018	//传递给动态库的参数有误
#define NET_DLDVR_ERROR_LOADDECODERDLL		-6019 //不能载入DLLDECODER_H264.dll
#define NET_DLDVR_ERROR_LOADIMAGECONVERTDLL	-6020 //不能载入ImageConverter_H264.dll
#define NET_DLDVR_ERROR_LOADIJL15DLL		-6021 //不能载入ijl15.dll
#define NET_DLDVR_ERROR_NOTSUPPORTDVR		-6022 //机型不支持该操作
#define NET_DLDVR_ERROR_FILE_NOTMATCHING	-6023	//文件类型不匹配  
#define NET_DLDVR_ERROR_FILE_VERSIOM		-6024	//要升级的文件版本比原先的主机版本低
#define NET_DLDVR_ERROR_PLAYER_NOTFIND_BYHANDLE		-6030	//根据handle没有找到Player句柄

//自定义错误:
//DVR操作错误
#define NET_DLDVR_ERROR_INVALIDPWD			-7000	//登陆时密码错误
#define NET_DLDVR_ERROR_TOOMANYUSER			-7001	//当前登陆到DVR的用户已经达到最大值了
#define NET_DLDVR_ERROR_POORRIGHT			-7002	//登陆时权限不够
#define NET_DLDVR_ERROR_STARTMONITOR		-7003	//向DVR请求视频服务失败
#define NET_DLDVR_ERROR_BROWSEPB			-7004	//回放导致的预览失败
#define NET_DLDVR_ERROR_BROWSEHW			-7005 //硬件导致的预览失败
#define NET_DLDVR_ERROR_SEARCHRECORD		-7006	//DVR拒绝了检索录像文件的请求
#define NET_DLDVR_ERROR_PLAYBACK			-7007	//DVR拒绝了回放的请求
#define NET_DLDVR_ERROR_DOWNLOAD			-7008	//DVR拒绝了下载录像文件的请求
#define NET_DLDVR_ERROR_GETSYSCFG			-7009	//DVR拒绝了获取系统参数的请求
#define NET_DLDVR_ERROR_GETPTZPRESET		-7010 //DVR拒绝了获取云台预制点的请求
#define NET_DLDVR_ERROR_SETPTZPRESET		-7011	//DVR拒绝了设置云台预制点的请求
#define NET_DLDVR_ERROR_REMOVEPTZPRESET		-7012	//DVR拒绝了删除云台预制点的请求
#define NET_DLDVR_ERROR_CALLPTZPRESET		-7013	//DVR拒绝了呼叫云台预制点的请求
#define NET_DLDVR_ERROR_STARTCRUISE			-7014	//DVR拒绝了巡航开始的请求
#define NET_DLDVR_ERROR_STOPCRUISE			-7015	//DVR拒绝了巡航结束的请求

#define NET_DLDVR_ERROR_LOADDEFAULT			-7100	//DVR拒绝了载入出厂设置的请求
#define NET_DLDVR_ERROR_GETCHANNELSTATE		-7101	//DVR拒绝了获取通道录像状态的请求
#define NET_DLDVR_ERROR_SETCHANNELSTATE		-7102	//DVR拒绝了设置通道录像状态的请求
#define NET_DLDVR_ERROR_GETSWITCHSTATE		-7103	//DVR拒绝了获取外接开关状态的请求
#define NET_DLDVR_ERROR_SETSWITCHSTATE		-7104	//DVR拒绝了设置外接开关状态的请求
#define NET_DLDVR_ERROR_REJECTNEWIMG		-7105	//DVR拒绝了更新系统软件的请求
#define NET_DLDVR_ERROR_UPGRADESYSTEM		-7106	//DVR拒绝接受新的系统软件
#define NET_DLDVR_ERROR_UPGRADEWEB			-7107	//DVR拒绝了更新内嵌WEB的请求
#define NET_DLDVR_ERROR_GETALARMLOG			-7108	//DVR拒绝了获取告警日志的请求
#define NET_DLDVR_ERROR_GETRUNLOG			-7109	//DVR拒绝了获取运行日志的请求
#define NET_DLDVR_ERROR_GETVERSION			-7110	//DVR拒绝了获取版本信息的请求
#define NET_DLDVR_ERROR_GETTIME				-7111	//DVR拒绝了获取系统时间的请求
#define NET_DLDVR_ERROR_SETTIME				-7112	//DVR拒绝了设置系统时间的请求
#define NET_DLDVR_ERROR_RESTART				-7113	//DVR拒绝了重新启动的请求
#define NET_DLDVR_ERROR_USBBACKUP			-7114	//DVR拒绝了USB备份的请求
#define NET_DLDVR_ERROR_CHECKSUM			-7115	//系统更新的时候发生了数据校验错误
#define NET_DLDVR_ERROR_REJECTRECVDATA		-7116	//系统更新的时候dvr中途拒绝了接收映象
#define NET_DLDVR_ERROR_UPGRADEING			-7117	//dvr在执行系统更新的时候发出了拒绝指令
#define NET_DLDVR_ERROR_TRANSPCHANNEL		-7118	//dvr执行透明通道的时候失败了
#define NET_DLDVR_ERROR_REGALARMUSER		-7119	//dvr拒绝了注册告警接收用户
#define NET_DLDVR_ERROR_SYSPARAUPDATE		-7120 //更新系统参数失败
#define NET_DLDVR_ERROR_RCDPARAUPDATE		-7121 //更新录像参数失败
#define NET_DLDVR_ERROR_NOTOPENDVRCFG		-7122 //没有打开DVR配置
#define	NET_DLDVR_ERROR_LOGOUT				-7123
#define NET_DLDVR_ERROR_NOT_SURPPORTED		-7124	//不支持该接口

#define NET_DLDVR_ERROR_CREATEOVERLAY	-7200	//不能创建Overlay,请使用普通方式初始化
#define NET_DLDVR_ERROR_STARTTALK		-7201	//开启对讲失败
#define NET_DLDVR_ERROR_CONFLICTVF		-7202	//相冲突的视频格式

#define NET_DLDVR_ERROR_DIRECTDRAWCREATE		-7210//创建一个DirectDraw对象失败
#define NET_DLDVR_ERROR_SETCOOPERATIVELEVEL		-7212//指定应用程序所要求的对显示硬件的控制程度失败
#define NET_DLDVR_ERROR_CREATESURFACE			-7213//创建主页面对象失败
#define NET_DLDVR_ERROR_CREATEBACKSURFACE		-7214//创建次页面对象失败
#define NET_DLDVR_ERROR_CREATECLIPPER			-7215//创建DirectDrawClipper接口失败
#define NET_DLDVR_ERROR_SETCLIPPER				-7216//剪裁器对象连接表面失败

#define NET_DLDVR_ERROR_ALREADYLOADDLL		-7300	//已经载入了相应的动态链接库
#define NET_DLDVR_ERROR_INVALIDDLLH			-7301	//工程并没有载入该DLL,却要求释放该DLL
#define NET_DLDVR_ERROR_PLAYSTARTUP			-7302	//播放库初始化失败
#define NET_DLDVR_ERROR_FRAMELIBSTARTUP		-7303 //帧放库初始出错
#define NET_DLDVR_ERROR_NOIDLEPLAYER		-7304	//播放器的数量已经达到最大值
#define NET_DLDVR_ERROR_OPENPLAYER			-7305 //调用NetOpenPlayer的时候出错
#define NET_DLDVR_ERROR_INVALIDMONITORS		-7306	//并没有打开监视,却要求关闭监视
#define NET_DLDVR_ERROR_INVALIDPLAYRS		-7307	//并没有打开回放,却要求关闭回放
#define NET_DLDVR_ERROR_INVALIDPLAYFS		-7308	//并没有打开文件播放,却要求关闭
#define NET_DLDVR_ERROR_INVALIDPLAYHD		-7309	//错误的Player句柄
#define NET_DLDVR_ERROR_INVALIDSPEED		-7310 //不支持的播放速度
#define NET_DLDVR_ERROR_INVALIDSTATE		-7311	//不支持的状态
#define NET_DLDVR_ERROR_INVALIDINITTYPE		-7312	//使用该功能需要Overlay模式
#define NET_DLDVR_ERROR_ALARMCENTERON		-7313	//告警中心已经启动
#define NET_DLDVR_ERROR_ALARMCENTEROFF		-7314	//告警中心还没有开启
#define NET_DLDVR_ERROR_REPORTON			-7315 //实时上报已经启动
#define NET_DLDVR_ERROR_REPORTOFF			-7316 //实时上报未启动
#define NET_DLDVR_ERROR_INVALIDTAIKH		-7320	//错误的对讲句柄
#define NET_DLDVR_ERROR_INVALIDTALKS		-7321	//并没有打开对讲,却要求关闭

#define NET_DLDVR_ERROR_FORBIDDEN			-7400	//监视禁止
#define NET_DLDVR_ERROR_NO_PREREC			-7401	//主机没有开预录
#define NET_DLDVR_ERROR_SETIFRAMEONLYCH		-7410	//选择帧放通道失败

#define NET_DLDVR_PASSWORD_ERROR 		-7500       //用户名密码错误
#define NET_DLDVR_NOENOUGHPRI 			-7510       //权限不足
#define NET_DLDVR_OPERNOPERMIT			-7520       //无此权限
#define NET_DLDVR_NOINIT 				-7530       //没有初始化
#define NET_DLDVR_OVER_MAXLINK 			-7540       //连接到DVR的客户端个数超过最大
#define NET_DLDVR_MAX_NUM				-7550	    //个数达到最大
#define NET_DLDVR_USERNOTEXIST			-7560	    //用户不存在
#define NET_DLDVR_CREATESOCKET_ERROR	-7570	//创建SOCKET出错
#define NET_DLDVR_NETWORK_FAIL_CONNECT	-7580 //连接服务器失败
#define NET_DLDVR_NETWORK_RECV_ERROR	-7590 //从服务器接收数据失败
#define NET_DLDVR_NETWORK_RECV_TIMEOUT	-7600 //从服务器接收数据超时
#define NET_DLDVR_WRITEFLASHERROR		-7610       //写FLASH出错
#define NET_DLDVR_NETWORK_SEND_ERROR	-7620       //向服务器发送失败
#define NET_DLDVR_PARAMETER_ERROR 		-7630       //参数错误
#define	NET_DLDVR_FILEOPENFAIL			-7640       //打开文件出错
#define NET_DLDVR_MAX_USERNUM			-7650	//用户数达到最大
#define NET_DLDVR_VERSIONNOMATCH		-8000	//版本不匹配
#define NET_DLDVR_CHANNEL_ERROR 		-8001	//通道号错误
#define NET_DLDVR_NETWORK_ERRORDATA		-8002	//传送的数据有误
#define NET_DLDVR_ORDER_ERROR			-8003	//调用次序错误
#define NET_DLDVR_COMMANDTIMEOUT		-8004	//DVR命令执行超时
#define NET_DLDVR_ERRORSERIALPORT		-8005	//串口号错误
#define NET_DLDVR_ERRORALARMPORT		-8006	//报警端口错误
#define NET_DLDVR_CHAN_EXCEPTION		-8007	//服务器通道处于错误状态
#define NET_DLDVR_NODISK				-8008	//没有硬盘
#define NET_DLDVR_ERRORDISKNUM			-8009	//硬盘号错误
#define NET_DLDVR_DISK_FULL				-8010	//服务器硬盘满
#define NET_DLDVR_DISK_ERROR			-8011	//服务器硬盘出错
#define NET_DLDVR_NOSUPPORT				-8012	//服务器不支持
#define NET_DLDVR_BUSY					-8013	//服务器忙
#define NET_DLDVR_MODIFY_FAIL			-8014	//服务器修改不成功
#define NET_DLDVR_PASSWORD_FORMAT_ERROR	-8015	//密码输入格式不正确
#define NET_DLDVR_DISK_FORMATING		-8016	//硬盘正在格式化，不能启动操作
#define NET_DLDVR_DVRNORESOURCE			-8017	//DVR资源不足
#define	NET_DLDVR_DVROPRATEFAILED		-8018	//DVR操作失败
#define NET_DLDVR_OPENHOSTSOUND_FAIL 	-8019	//打开PC声音失败
#define NET_DLDVR_DVRVOICEOPENED 		-8020	//服务器语音对讲被占用
#define	NET_DLDVR_TIMEINPUTERROR		-8021	//时间输入不正确
#define	NET_DLDVR_NOSPECFILE			-8022	//回放时服务器没有指定的文件
#define NET_DLDVR_CREATEFILE_ERROR		-8023	//创建文件出错
#define	NET_DLDVR_OPERNOTFINISH			-8024	//上次的操作还没有完成
#define	NET_DLDVR_GETPLAYTIMEFAIL		-8025	//获取当前播放的时间出错
#define	NET_DLDVR_PLAYFAIL				-8026	//播放出错
#define NET_DLDVR_FILEFORMAT_ERROR		-8027	//文件格式不正确
#define NET_DLDVR_DIR_ERROR				-8028	//路径错误
#define NET_DLDVR_ALLOC_RESOUCE_ERROR	-8029	//资源分配错误
#define NET_DLDVR_AUDIO_MODE_ERROR		-8030	//声卡模式错误
#define NET_DLDVR_NOENOUGH_BUF			-8031	//缓冲区太小
#define NET_DLDVR_SETSOCKET_ERROR		-8032	//设置SOCKET出错
#define NET_DLDVR_CARDHAVEINIT			-8034	//解码卡已经初始化过
#define NET_DLDVR_PLAYERFAILED			-8035	//播放器中错误
#define NET_DLDVR_GETLOCALIPANDMACFAIL	-8036	//获得客户端的IP地址或物理地址失败
#define NET_DLDVR_NOENCODEING			-8037	//该通道没有编码
#define NET_DLDVR_IPMISMATCH			-8038	//IP地址不匹配
#define NET_DLDVR_MACMISMATCH			-8039	//MAC地址不匹配
#define NET_DLDVR_UPGRADELANGMISMATCH	-8040	//升级文件语言不匹配

#define NET_DLDVR_UPGRADENETERROR		-8070	//网络断开，状态未知
#define NET_DLDVR_UPGRADEFAIL			-8080	//更新数据失败

#define NET_DLDVR_SETLANGUAGE_NETERROR	-8090	//网络断开，状态未知
#define NET_DLDVR_SETLANGUAGE_FAIL		-8091	//设置自定义语言失败

#define NET_DLDVR_SETLOGO_NETERROR		-8093	//网络断开，状态未知
#define NET_DLDVR_SETLOGO_FAIL			-8094	//设置自定义语言失败

#define NET_DLDVR_SETRESOURCE_NETERROR	-8095	//网络断开，状态未知
#define NET_DLDVR_SETRESOURCE_FAIL		-8096	//设置资源失败

#define NET_DLDVR_SETFILE_NETERROR		-8097	//网络断开，状态未知
#define NET_DLDVR_SETFILE_FAIL			-8098	//升级文件失败

//对讲相关	-9000 ~ -9500
#define NET_DLDVR_NOIDLETALKER			-9000		//对讲器的数量已经达到最大值

#define NET_DLDVR_ERROR_NOTRIGHT_TYPE	-9010	//该机型不支持该接口
#define NET_DLDVR_ERROR_TRANSSERIAL_NOTSTART	-9020	//没有建立透明通道

//2010-03-19 add,DM60相关
#define NET_DLDVR_ERROR_NOCMD_TYPE	-9101	//错误的命令值
#define NET_DLDVR_ERROR_GETDMCOLOR	-9102	//主机拒绝了获取DMColor的请求
#define NET_DLDVR_ERROR_SETDMCOLOR	-9103	//主机拒绝了设置DMColor的请求	
#define NET_DLDVR_ERROR_SET_DMCOMMAND	-9104	//主机拒绝了DMCommand命令
#define NET_DLDVR_ERROR_SET_DMCONFIG	-9105	//主机拒绝了设置DMConfig的请求
#define NET_DLDVR_ERROR_GET_DMCONFIG	-9106	//主机拒绝了获取DMConfig的请求
#define NET_DLDVR_ERROR_GET_DMMEASURE	-9107	//主机拒绝了获取测温参数的请求
#define NET_DLDVR_ERROR_SET_DMMEASURE	-9108	//主机拒绝了设置测温参数的请求
#define NET_DLDVR_ERROR_PARA	-9120	//输入的参数有误
#define NET_DLDVR_ERROR_PTZPROTOCOL_NOTSUPPORT	-9121	//不支持的云台协议
#define NET_DLDVR_ERROR_DMMEASURE_TYPE	-9122	//错误的测温目标类型
#define NET_DLDVR_ERROR_DMMEASURE_INFO	-9123	//错误的温度信息

#define NET_DLDVR_ERROR_QUERY_FUNCTION			-9130	//功能查询失败
#define NET_DLDVR_ERROR_QUERY_FUNCTION_OVERFLOW -9131	//功能查询范围越界
#define NET_DLDVR_ERROR_SETCHANNELSTREAM_EX		-9132	//一次性设置单台主机的相关通道码流状态


/************************************************************************/
/* 错误值定义结束                                                       */
/************************************************************************/


#define NET_DLDVR_UPGRADESUCESS			1000	//升级成功
#define NET_DLDVR_UPGRADEING			1010	//正在升级
#define NET_DLDVR_UPGRADEING_DOWNLOAD	1011	//正在升级-下载镜像
#define NET_DLDVR_UPGRADEING_APPLYING	1012	//正在升级-应用镜像

#define NET_DLDVR_SETLANGUAGE			1020	//正在设置自定义语言
#define NET_DLDVR_SETLANGUAGE_DOWNLOAD	1021	//正在设置自定义语言-下载语言文件
#define NET_DLDVR_SETLANGUAGE_APPLYING	1022	//正在设置自定义语言-应用语言文件（写flash）
#define NET_DLDVR_USETLANGUAGE_SUCESS	1023	//设置语言成功成功

#define NET_DLDVR_SETLOGO				1030	//正在设置自定义LOGO
#define NET_DLDVR_SETLOGO_DOWNLOAD		1031	//正在设置自定义LOGO-下载Logo文件
#define NET_DLDVR_SETLOGO_APPLYING		1032	//正在设置自定义LOGO-应用Logo文件（写flash）
#define NET_DLDVR_USETLOGO_SUCESS		1033	//设置LOGO成功成功

#define NET_DLDVR_SETRESOURCE				1040	//正在设置资源
#define NET_DLDVR_SETRESOURCE_DOWNLOAD		1041	//正在设置资源-下载资源文件
#define NET_DLDVR_SETRESOURCE_APPLYING		1042	//正在设置资源-应用资源文件（写flash）
#define NET_DLDVR_SETRESOURCE_SUCESS		1043	//设置资源成功

#define NET_DLDVR_SETFILE				1050	//正在升级文件
#define NET_DLDVR_SETFILE_SUCCESS		1051	//升级文件成功

//NET_DLDVR_IsSupport()返回值 
//1－9位分别表示以下信息（位与是TRUE)表示支持；
#define NET_DLDVR_SUPPORT_DDRAW			0x01//支持DIRECTDRAW，如果不支持，则播放器不能工作；
#define NET_DLDVR_SUPPORT_BLT				0x02//显卡支持BLT操作，如果不支持，则播放器不能工作；
#define NET_DLDVR_SUPPORT_BLTFOURCC		0x04//显卡BLT支持颜色转换，如果不支持，播放器会用软件方法作RGB转换；
#define NET_DLDVR_SUPPORT_BLTSHRINKX		0x08//显卡BLT支持X轴缩小；如果不支持，系统会用软件方法转换；
#define NET_DLDVR_SUPPORT_BLTSHRINKY		0x10//显卡BLT支持Y轴缩小；如果不支持，系统会用软件方法转换；
#define NET_DLDVR_SUPPORT_BLTSTRETCHX		0x20//显卡BLT支持X轴放大；如果不支持，系统会用软件方法转换；
#define NET_DLDVR_SUPPORT_BLTSTRETCHY		0x40//显卡BLT支持Y轴放大；如果不支持，系统会用软件方法转换；
#define NET_DLDVR_SUPPORT_SSE				0x80//CPU支持SSE指令，Intel Pentium3以上支持SSE指令；
#define NET_DLDVR_SUPPORT_MMX				0x100//CPU支持MMX指令集，Intel Pentium3以上支持SSE指令；

#define SET_PRESET						1	// 设置预置点 
#define CLE_PRESET						2	// 清除预置点 
#define GOTO_PRESET						3	// 转到预置点

#define LIGHT_PWRON						3	// 接通灯光电源 //2010-10-28   2------->3
#define WIPER_PWRON						2	// 接通雨刷开关 //2010-10-28   3------->2
#define FAN_PWRON						4	// 接通风扇开关 
#define HEATER_PWRON					5	// 接通加热器开关 
#define AUX_PWRON						6	// 接通辅助设备开关 

#define PTZ_STOP						10	//停止云台动作

#define ZOOM_IN							11	// 焦距以速度SS变大(倍率变大) 
#define ZOOM_OUT						12	// 焦距以速度SS变小(倍率变小) 
#define FOCUS_NEAR						13	// 焦点以速度SS前调 
#define FOCUS_FAR						14	// 焦点以速度SS后调 
#define IRIS_OPEN						15	// 光圈以速度SS扩大 
#define IRIS_CLOSE						16	// 光圈以速度SS缩小 
#define TILT_UP							21	// 云台以SS的速度上仰 
#define TILT_DOWN						22	// 云台以SS的速度下俯 
#define PAN_LEFT						23	// 云台以SS的速度左转 
#define PAN_RIGHT						24	// 云台以SS的速度右转 
#define PAN_AUTO						29	// 云台以SS的速度左右自动扫描 

#define FILL_PRE_SEQ	30 // 将预置点加入巡航序列 
#define SET_SEQ_DWELL	31 // 设置巡航点停顿时间 
#define SET_SEQ_SPEED	32 // 设置巡航速度 
#define CLE_PRE_SEQ		33 // 将预置点从巡航序列中删除 
#define RUN_CRUISE		36	// 开始轨迹 
#define RUN_SEQ			37	// 开始巡航 
#define STOP_SEQ		38	// 停止巡航 

#define Get_SEQ_ROUTE	39	//获取指定通道的巡航配置


//显示模式
enum{NORMALMODE = 0,OVERLAYMODE};

//发送模式
enum{PTOPTCPMODE,PTOPUDPMODE,MULTIMODE,RTPMODE,AUDIODETACH,NOUSEMODE};

#define NET_DLDVR_SYSHEAD 				0xEE	//系统头数据
#define NET_DLDVR_STREAMDATA 			0xEF	//流数据
#define NET_DLDVR_STREAMDATA_VIDEO 		0x1		//标准视频流
#define NET_DLDVR_STREAMDATA_AUDIO		0x2		//标准音频流

//播放控制命令宏定义 NET_DLDVR_PlayBackControl,NET_DLDVR_PlayControlLocDisplay,NET_DLDVR_DecPlayBackCtrl的宏定义
#define NET_DLDVR_PLAYSTART					1	//开始播放
#define NET_DLDVR_PLAYSTOP					2	//停止播放
#define NET_DLDVR_PLAYPAUSE					3	//暂停播放
#define NET_DLDVR_PLAYRESTART				4	//恢复播放
#define NET_DLDVR_PLAYFAST					5	//快放(4倍速)
#define NET_DLDVR_PLAYSLOW					6	//慢放(1/4倍速)
#define NET_DLDVR_PLAYNORMAL				7	//正常速度
#define NET_DLDVR_PLAYFRAME					8	//单帧放
#define NET_DLDVR_PLAYSTARTAUDIO			9	//打开声音
#define NET_DLDVR_PLAYSTOPAUDIO				10	//关闭声音
#define NET_DLDVR_PLAYAUDIOVOLUME			11	//调节音量
#define NET_DLDVR_PLAYSETPOS				12	//改变文件回放的进度
#define NET_DLDVR_PLAYGETPOS				13	//获取文件回放的进度
#define NET_DLDVR_PLAYGETTIME				14	//获取当前已经播放的时间
#define NET_DLDVR_PLAYGETFRAME				15	//获取当前已经播放的帧数
#define NET_DLDVR_GETTOTALFRAMES  			16	//获取当前播放文件总的帧数
#define NET_DLDVR_GETTOTALTIME    			17	//获取当前播放文件总的时间
#define NET_DLDVR_THROWBFRAME				20	//丢B帧
#define NET_DLDVR_GETPLAYAUDIOVOLUME		21	//获得当前的音量大小，2007-12-14，he
#define NET_DLDVR_PLAY_DOUBLESPEED			22	//2倍数
#define NET_DLDVR_PLAY_HALFSPEED			23	//1/2速度
#define NET_DLDVR_PLAY_SETPOS_BYTIME		24	//回放时按时间百分比跳转到某位置
#define NET_DLDVR_PLAY_OCTUPLESPEED			25	//8倍数,2010-07-06 add
#define NET_DLDVR_PLAY_SIXTEENSPEED			26	//16倍数,2010-07-06 add
#define NET_DLDVR_PLAY_32SPEED				27	//32倍数,2010-07-06 add

//NET_DLDVR_GetDVRConfig,NET_DLDVR_GetDVRConfig的命令定义
#define NET_DLDVR_GET_ALLCFG				0	//获得所有参数
#define NET_DLDVR_SET_ALLCFG				0	//获得所有参数
#define NET_DLDVR_GET_DEVICECFG				1	//获取设备参数
#define NET_DLDVR_SET_DEVICECFG				1	//设置设备参数
#define NET_DLDVR_GET_NETCFG				2	//获取网络参数
#define NET_DLDVR_SET_NETCFG				2	//设置网络参数
#define NET_DLDVR_GET_PICCFG				3	//获取图象参数
#define NET_DLDVR_SET_PICCFG				3	//设置图象参数
#define NET_DLDVR_GET_COMPRESSCFG			4	//获取压缩参数
#define NET_DLDVR_SET_COMPRESSCFG			4	//设置压缩参数
#define NET_DLDVR_GET_RECORDCFG				5	//获取录像时间参数
#define NET_DLDVR_SET_RECORDCFG				5	//设置录像时间参数
#define NET_DLDVR_GET_DECODERCFG			6	//获取解码器参数
#define NET_DLDVR_SET_DECODERCFG			6	//设置解码器参数
#define NET_DLDVR_GET_RS232CFG 				7	//获取232串口参数
#define NET_DLDVR_SET_RS232CFG				7	//设置232串口参数
#define NET_DLDVR_GET_ALARMINCFG 			8	//获取报警输入参数
#define NET_DLDVR_SET_ALARMINCFG			8	//设置报警输入参数
#define NET_DLDVR_GET_ALARMOUTCFG 			9	//获取报警输出参数
#define NET_DLDVR_SET_ALARMOUTCFG			9	//设置报警输出参数
#define NET_DLDVR_GET_TIMECFG 				10	//获取DVR时间
#define NET_DLDVR_SET_TIMECFG				10	//设置DVR时间
#define NET_DLDVR_GET_PREVIEWCFG 			11	//获取预览参数
#define NET_DLDVR_SET_PREVIEWCFG			11	//设置预览参数
#define NET_DLDVR_GET_VIDEOOUTCFG 			12	//获取视频输出参数
#define NET_DLDVR_SET_VIDEOOUTCFG			12	//设置视频输出参数
#define NET_DLDVR_GET_USERCFG 				13	//获取用户参数
#define NET_DLDVR_SET_USERCFG				13	//设置用户参数
#define NET_DLDVR_GET_EXCEPTIONCFG 			14	//获取异常参数
#define NET_DLDVR_SET_EXCEPTIONCFG			14	//设置异常参数
#define NET_DLDVR_GET_SHOWSTRING			15	//获取叠加字符参数
#define NET_DLDVR_SET_SHOWSTRING			15	//设置叠加字符参数
#define NET_DLDVR_GET_AUXOUTCFG				16	//获取辅助输出设置
#define NET_DLDVR_SET_AUXOUTCFG				16	//设置辅助输出设置
#define NET_DLDVR_GET_PICCFG_EX				17	//获取图象参数（扩展）
#define NET_DLDVR_SET_PICCFG_EX				17	//设置图象参数（扩展）
#define NET_DLDVR_GET_USERCFG_EX 			18	//获取用户参数
#define NET_DLDVR_SET_USERCFG_EX			18	//设置用户参数
#define NET_DLDVR_GET_DEVICEVERSION			19  //获取AUX_PARAMETER参数 
#define NET_DLDVR_SET_AUX_PARAMETER			19  //设置AUX_PARAMETER参数
#define NET_DVR_SET_SEQ_ROUTE				20  //设置指定通道的巡航配置
#define NET_DVR_GET_SEQ_ROUTE				20  //获取指定通道的巡航配置

//2011-02-23 add, 600及700机型恢复出厂设置的参数类型（即NET_DLDVR_RestoreConfigByType()接口的ConfigType值定义）
#define NET_DLDVR_RESTORE_SYSTEM 		1	//基本参数 
#define NET_DLDVR_RESTORE_DISPLAY		2	//显示参数
#define NET_DLDVR_RESTORE_RECORD		3  //录像参数
#define NET_DLDVR_RESTORE_USER			4  //用户参数
#define NET_DLDVR_RESTORE_NETWORK		5 //网络参数
#define NET_DLDVR_RESTORE_PTZ			6  //云台参数
#define NET_DLDVR_RESTORE_ALLCFG		7  //全部参数

//2009-01-18, 仅适用于NVR获取/设置系统参数配置的命令
#define NET_DVR_GET_SYSTEMCFG_NVR	30       //获得所有的参数
#define NET_DVR_GET_RVSCFG_NVR		31     //读取 视频源参数          
#define NET_DVR_GET_ALARMREC_NVR	32    //读取 报警录像参数
#define NET_DVR_GET_DEMOCFG_NVR		33    //读取 云台相关参数

#define NET_DVR_SET_SYSTEMCFG_NVR  30       //设置所有的参数
#define NET_DVR_SET_RVSCFG_NVR     31   //设置 视频源参数
#define NET_DVR_SET_ALARMREC_NVR	32    //读取 报警录像参数
#define NET_DVR_SET_DEMOCFG_NVR    33   //设置 云台相关参数

//2010-03-22,DM60 测温目标设置的相关命令
#define NET_DVR_GET_DMMEASURE_POINT	1
#define NET_DVR_SET_DMMEASURE_POINT	1
#define NET_DVR_GET_DMMEASURE_LINE	2
#define NET_DVR_SET_DMMEASURE_LINE	2
#define NET_DVR_GET_DMMEASURE_AREA	3
#define NET_DVR_SET_DMMEASURE_AREA	3
#define NET_DVR_CLEAR_DMMEASURE_POINT	1
#define NET_DVR_CLEAR_DMMEASURE_LINE		2
#define NET_DVR_CLEAR_DMMEASURE_AREA		3

//DM60测温目标命令定义
#define NET_DVR_GET_DMMEASURE_ALL	0
#define NET_DVR_GET_DMMEASURE_POINT	1
#define NET_DVR_GET_DMMEASURE_LINE	2
#define NET_DVR_GET_DMMEASURE_AREA	3

//DM60主机传过来的温度类型
#define NET_DVR_DMMEASURE_POINT	0
#define NET_DVR_DMMEASURE_LINE	1
#define NET_DVR_DMMEASURE_AREA	2


//DM60原始数据采集的状态
#define DM60_CAPTUREINFRARED_ING		1	// 正在采集
#define DM60_CAPTUREINFRARED_OVER		2	// 采集完成
#define DM60_CAPTUREINFRARED_ERROR		3	// 采集出错

//DM60文件更新类型
#define UPDATEFILE_TYPE_DM60_MID		0
#define UPDATEFILE_TYPE_DM60_FIREWARE	1
#define UPDATEFILE_TYPE_DM60_DATA		2

//DM60设置参数命令
#define NET_DLDVR_SET_ALLSYSCFG_DM60			0 //设置所有系统参数（对应SYSTEMCFG_DM60结构）
#define NET_DLDVR_SET_REVISEPARA_DM60			1 //设置测温修正系数
#define NET_DLDVR_SET_REVISETEMP_DM60			2 //设置测温修正温度(单位℃)
#define NET_DLDVR_SET_AIMDISTANCE_DM60			3 //设置距离，厘米为单位
#define NET_DLDVR_SET_AIMHIMIDITY_DM60			4 //设置湿度
#define NET_DLDVR_SET_SURROUNDTEMP_DM60			5 //设置环境温度
#define NET_DLDVR_SET_MEASURETEMPCLASS_DM60		6 //设置镜头选择即测温档位
#define NET_DLDVR_SET_AREATYPE_DM60				7 //设置区域测温类型，最高最低平均
#define NET_DLDVR_SET_ALARMENABLE_DM60			8 //设置报警使能
#define NET_DLDVR_SET_ALARMTEMP_DM60			9 //设置报警温度
#define NET_DLDVR_SET_ALARMCOLOR_DM60			10//设置报警色
#define NET_DLDVR_SET_LENSID_DM60				11//设置镜头选择即测温档位，现在含义同MeasureTempClass
#define NET_DLDVR_SET_AUTOSAVETIME_DM60			12//设置自动保存间隔时间，先暂时没用
#define NET_DLDVR_SET_AUTOZEROTIME_DM60			13//设置自动调零时间间隔
#define NET_DLDVR_SET_REFETEMPTYPE_DM60			14//设置参考温度类型0无参考测温，…
#define NET_DLDVR_SET_REFETEMPVALUE_DM60		15//设置参考温度值，当选择参考类型为参考时用到
#define NET_DLDVR_SET_COLORRULED_DM60			16//设置色标选择，与SCR功能同，但该项值设置色标，但不改变色标的反相或顺相
#define NET_DLDVR_SET_UPCOLORID_DM60			17//设置上限色标
#define NET_DLDVR_SET_DOWNCOLORID_DM60			18//设置下限色标
#define NET_DLDVR_SET_ISOTHERMCOLORID_DM60		19//设置等温色标
#define NET_DLDVR_SET_ISOTHERMTEMP_DM60			20//设置等温温度
#define NET_DLDVR_SET_ISOTHERMHIGHT_DM60		21//设置等温高度
#define NET_DLDVR_SET_SHOWCOLORRULE_DM60		22//设置是否显示色标
#define NET_DLDVR_SET_SHOWDATA_DM60				23//设置是否显示数据
#define NET_DLDVR_SET_AVGIMAGECOUNT_DM60		24//设置图像平均场数，现在没用
#define NET_DLDVR_SET_SHOWAREACURSOR_DM60		25//设置显示区域光标使能
#define NET_DLDVR_SET_LANGUAGETYPE_DM60			26//设置语言类型

#define NET_DLDVR_SET_VIDEOOUTTYPE_DM60			28//设置视频输出格式0:PAL;1:
#define NET_DLDVR_SET_LASERENABLE_DM60			29//设置激光开启控制
#define NET_DLDVR_SET_MEASURETEMPUNIT_DM60		30//设置温度单位：0:℃;1℉;2K;
#define NET_DLDVR_SET_TRANSMENU_DM60			31//设置菜单是否透明
#define NET_DLDVR_SET_CLOSELEDTIME_DM60			32//设置关闭屏幕时间
#define NET_DLDVR_SET_CLOSEPOWERTIME_DM60		33//设置关闭电源时间
#define NET_DLDVR_SET_TEMPDISPLAY_DM60			34//设置温度是否显示Yes/No
#define NET_DLDVR_SET_STEERSTARTPOS_DM60		35//设置舵机起始位置,是不是应该用坐标表示？
#define NET_DLDVR_SET_STEERENDPOS_DM60			36//设置舵机结束位置，是不是应该用坐标表示？
#define NET_DLDVR_SET_DIMFITERLEVEL_DM60		37//设置衰减片值
#define NET_DLDVR_SET_IMAGEMODE_DM60			38//设置图像模式

//回调的消息类型
#define COMM_ALARM						0x1100	//报警信息 对应结构 NET_DLDVR_ALARMINFO
#define COMM_ALARM_EX					0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
#define COMM_ALARM_NAME					0x1102	//报警信息 对应结构NET_DLDVR_ALARMINFO_USERNAME（带有用户名信息,2010-0901 add）
#define	COMM_TRADEINFO					0x1500 //ATMDVR主动上传交易信息
#define SERIAL_NUMBER					0x1600	//DVR上报的序列号,2008-03-12
#define DISKCHANNEL_STATE				0x1700 //主机定时上传硬盘状态、通道录像状态信息
#define RVS_INFO						0x1800	//IPCamera上传的RVS信息,2009-05-15 add
#define EXCEPTION_NOTIFY				0X1900 //网络异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息
#define DM_TEMP							0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
#define DM_TEMP_EX						0x2001	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP_EX
#define DMTEMP_ALARM					0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
#define DM_ANALYS_ALARM					0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
#define	PLAYBACK_OVER					0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION，（NET_DLDVR_EXCEPTION中的uMsg对应窗体通知消息）
#define RESOLUTION_CHANGE				0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
#define CAPTURE_INFRARED_FRAMENUM 		0x2023	//原始数据采集信息，对应结构NET_DLDVR_INFRAREDFRAME


//消息方式
//异常类型
//SDK通知消息 2008-09-01 add
//监视预览网络出错通知
#define WM_DLDVR_NET_EXCEPTION_NOTIFY		(WM_APP + 800)
//	wParam : 发生错误的句柄号
//	lParam : 0

//回放结束通知消息
#define WM_DLDVR_PLAYBACK_OVER_NOTIFY		(WM_APP + 801)
//	wParam : the handle of player
//	lParam : 0	

//对讲出错通知
#define WM_DLDVR_TALKER_EXCEPTION_NOTIFY	(WM_APP + 802)
//	wParam : 发生错误的句柄号
//	lParam : 0

//用户登出消息,与主机失去连接
#define WM_DLDVR_LOGOUT_NOTIFY				(WM_APP + 803)
//	wParam : 登出的用户ID
//	lParam : 0

//监视或回放时时分辨率发生改变的通知消息
//在收到该消息时，如果该通道正在录像，则必须先关闭录像后重新开始录像，否则会导致录像无法正常播放
#define WM_DLDVR_RESOLUTION_CHANGE_NOTIFY		(WM_APP + 804)
//	wParam : the handle of player
//	lParam : 

//监视或回放时时视频格式发生改变的通知消息
//在收到该消息时，如果该通道正在录像，则必须先关闭录像后重新开始录像，否则会导致录像无法正常播放
#define WM_DLDVR_VIDEOFORMAT_CHANGE_NOTIFY	(WM_APP + 805)
//	wParam : the handle of player
//	lParam : 

#define NAME_LEN 							32
#define SERIALNO_LEN 						48
#define MACADDR_LEN 						6
#define MAX_ETHERNET 						2
#define PATHNAME_LEN 						128
#define PASSWD_LEN 							16
#define MAX_CHANNUM 						16
#define MAX_ALARMOUT 						4
#define MAX_TIMESEGMENT						4
#define MAX_PRESET							128
#define MAX_DAYS 							7
#define PHONENUMBER_LEN						32
#define MAX_DISKNUM 						8
#define MAX_WINDOW 							16
#define MAX_VGA 							1
#define MAX_USERNUM 						16
#define MAX_EXCEPTIONNUM					16
#define MAX_LINK 							6
#define MAX_ALARMIN 						16
#define MAX_VIDEOOUT 						2
#define MAX_NAMELEN							16		//DVR本地登陆名
#define MAX_RIGHT							32		//权限
#define CARDNUM_LEN 						20
#define MAX_SHELTERNUM						4	//遮挡区域数
#define MAX_DECPOOLNUM						4
#define MAX_DECNUM							4
#define MAX_TRANSPARENTNUM					2
#define MAX_STRINGNUM						4
#define MAX_AUXOUT							4
#define MAX_CHANNUM_EX						32

// 网络接口定义 
#define NET_IF_10M_HALF						1					// 10M ethernet 
#define NET_IF_10M_FULL						2
#define NET_IF_100M_HALF					3					// 100M ethernet 
#define NET_IF_100M_FULL					4
#define NET_IF_AUTO							5

//设备型号(DVR类型)
#define DVR_C							0
#define DVR_L							1
#define DVR_LII							2
#define DVR_MPS							3 
#define DVR_MPF							4 
#define DVR_MPC							5
#define DVR_DVS							6	//杭州DVS
#define DVR_ENC_H264					7	//H264编码器
#define DVR_DEC_H264					8	//H264解码器
#define DVR_400T						9   //上海400T系列
#define DVR_IPCamera_GQ					10  //高清IpCameara(MG3500)	
#define DVR_NVR							11
#define DVR_400D						12  //杭州400D系列
#define DVR_MPT							13   //Dm642系列 MPT,所有通道只能设HD1分辩率
#define DVR_IPCamera_MG3512				14   //高清IpCameara	(MG3512)
#define DVR_DEC_MP4						15	 //大立C/L系列解码器
#define DVR_CMS							16	 //管理软件
#define DVR_408T4						17   //408T D1带智能机器
#define DVR_HISI32						18	//haisi3520平台下的16路机和32路机(3520平台下的600系列机型)
#define DVR_DM60						19	//DM60机型
#define DVR_HISI106						20	//3520平台下的106型机器，图像尺寸较大，固定为1280*1024
#define DVR_400D2						21  //杭州400D2系列,与400D之间的系统参数有所不同
#define DVR_716T						22 //2010-10-25 add， 14CIF+2D1的3520机型。即700机型(同于600的3520芯片）
#define DVR_IPCamera_MG3515				23   //高清IpCameara	(MG3515芯片， 与3512机型的区别在于3512的音频不进行编解码，3515的进行编解码处理)
#define DVR_401K						24   //401K机型(不带硬盘录像,参数配置和400D||代的有所不同)
#define DVR_400DX						25	//DVR400D系列，开发了一套新界面，目前暂定为DVR400DX，分4/8/16路
#define DVR_404D4						26	//基于DVR404D机型，开放全D1选项，目前暂定机型DVR404D4

//视频编码方式
#define   VIDEO_ENC_MPEG4    0
#define   VIDEO_ENC_H264     1
#define   VIDEO_ENC_DVS      2
#define   VIDEO_ENC_HI		 3 //H.264(海思)
#define   VIDEO_ENC_IPCamera 4 //IPCamera(MG3500)

//录像类型
#define   TIMER_RECORD		0 //定时录像
#define   MONITION_RECORD   1 //移动侦测	
#define   ALARM_RECORD      2 //报警触发
#define   CMD_RECORD		3 //命令触发
#define   MANUAL_RECORD		4 //手动录像 

#define REAL_PLAY		0	//2007-11-06,实时监视
#define JPEG_PLAY		1	//2007-11-06, JPEG监视

#define DEVICE_ID_LENGTH	128

//DM60机型的主板命令值，作为NET_DLDVR_SetDMCommand_DM60()函数的command参数
#define NET_DLDVR_CLEARALLPOINT		1	//清除所有点
#define NET_DLDVR_CLEARALLLINE		2	//清除所有线
#define NET_DLDVR_CLEARALLAREA		3	//清除所有区域
#define NET_DLDVR_ZEROADJUST		4	//红外调零
#define NET_DLDVR_KEYBOARD			5	//键盘命令，Value1中存放键盘码
#define NET_DLDVR_SETFOCUSPOS		6	//设置焦距预置位
#define NET_DLDVR_CALLFOCUSPOS		7	//调用焦距预置位
#define NET_DLDVR_FOCUSSTOP			8	//聚焦停止
#define NET_DLDVR_FOCUSSTEPNEAR		9	//向前单步聚焦
#define NET_DLDVR_FOCUSSTEPFAR		10	//向后单步聚焦
#define NET_DLDVR_FOCUSAUTO			11	//自动聚焦
#define NET_DLDVR_FOCUSSTEPS		12	//连续步进聚焦，正数往前，负数向后
#define NET_DLDVR_FOCUSSTEPSNEAR	13	//向前连续步进聚焦，直到FocusStop
#define NET_DLDVR_FOCUSSTEPSFAR		14	//向后连续步进聚焦，直到FocusStop
#define NET_DLDVR_VOLTAGELEVEL		15	//设置电平，Value1步长，+1, 表示增加1， -1, 表示减小1
#define NET_DLDVR_UPCOLORTEMP		16	//设置色标温度上限，Value1同VoltageLevel
#define NET_DLDVR_DOWNCOLORTEMP		17	//设置色标温度下限，Value1同VoltageLevel
#define	NET_DLDVR_IMAGEMODE			18	//设置图像模式，0-手动 1-自动

#pragma pack(1)
typedef struct  
{
	BYTE sSerialNumber[SERIALNO_LEN];	//序列号
	BYTE byAlarmInPortNum;				//DVR报警输入个数
	BYTE byAlarmOutPortNum;				//DVR报警输出个数
	BYTE byDiskNum;						//DVR 硬盘个数
	BYTE byDVRType;						/*DVR类型
										#define DVR_C							0
										#define DVR_L							1
										#define DVR_LII							2
										#define DVR_MPS							3 
										#define DVR_MPF							4 
										#define DVR_MPC							5
										#define DVR_HI3510						6
										#define DVR_ENC							7	//编码器
										#define DVR_DEC							8	//解码器
										#define DVR_HI3511						9   //
										#define IPCamera_GQ						10  //高清IpCameara	
										#define NVR								11
										#define DVR400D							12
										#define DVR_MPT							13   //Dm642系列 MPT,所有通道只能设HD1分辩率
										#define DVR_IPCamera_MG3512				14   //高清IpCameara	(MG3512)
										#define DVR_DEC_MP4						15	 //大立C/L系列解码器
										#define DVR_CMS							16	 //管理软件
										#define DVR_408T4						17   //408T D1带智能机器
										#define DVR_HISI32						18
										#define DVR_DM60						19	//DM60机型
										#define DVR_106							20	//3520系列下的106型机器，图像尺寸较大1280*1024
										#define DVR_400D2						21  //杭州400D2系列,与400D之间的系统参数有所不同
										#define DVR_716T						22 //2010-10-25 add， 14CIF+2D1的3520机型。即700机型(同于600的3520芯片）
										#define DVR_IPCamera_MG3515				23   //高清IpCameara	(MG3515芯片， 与3512机型的区别在于3512的音频不进行编解码，3515的进行编解码处理)
	*/
	BYTE byChanNum;						//DVR 通道个数
	BYTE byStartChan;					//IPC通道个数，2010-04-27 modifty
}NET_DLDVR_DEVICEINFO, *LPNET_DLDVR_DEVICEINFO;


typedef struct
{
	LONG lChannel;//通道号,从0开始（600机型有4个IPC通道，通道号从0x100+0->0x100+3）
	LONG lLinkMode;//最高位(31)为0表示主码流，为1表示子码流; 第30位表示是否是预录图像，0-不是，1-是；0－29位表示码流连接方式: 0：TCP方式,1：UDP方式,2：多播方式,3 - RTP方式，4-音视频分开(TCP)
					//监视IPCamera机型时，最高位(31)为0-MajorAV方式，1-MinorAV方式，2-Raw方式，3-YUV方式（Raw和YUV只保存原始数据，不显示图像）
	HWND hPlayWnd;//播放窗口的句柄,为NULL表示不播放图象
	CHAR* sMultiCastIP;//多播组地址
}NET_DLDVR_CLIENTINFO,*LPNET_DLDVR_CLIENTINFO;


typedef struct
{
	DWORD dwYear;		//年
	DWORD dwMonth;		//月
	DWORD dwDay;		//日
	DWORD dwHour;		//时
	DWORD dwMinute;		//分
	DWORD dwSecond;		//秒
}NET_DLDVR_TIME,*LPNET_DLDVR_TIME;

typedef struct
{
	//开始时间
    BYTE byStartHour;
	BYTE byStartMin;
	//结束时间
	BYTE byStopHour;
	BYTE byStopMin;
}NET_DLDVR_SCHEDTIME,*LPNET_DLDVR_SCHEDTIME;

//处理方式
#define NOACTION			0x0				//无响应
#define WARNONMONITOR		0x1				//监视器上警告
#define WARNONAUDIOOUT		0x2				//声音警告
#define UPTOCENTER			0x4				//上传中心
#define TRIGGERALARMOUT		0x8				//触发报警输出

typedef struct
{
	DWORD	dwHandleType;				//处理方式,处理方式的"或"结果
										//0x00: 无响应
										//0x01: 监视器上警告
										//0x02: 声音警告
										//0x04: 上传中心
										//0x08: 触发报警输出
	BYTE byRelAlarmOut[MAX_ALARMOUT];	//报警触发的输出通道,报警触发的输出,为1表示触发该输出
}NET_DLDVR_HANDLEEXCEPTION;

//DVR设备参数
typedef struct
{
	DWORD dwSize;
	BYTE sDVRName[NAME_LEN];		//DVR名称
	DWORD dwDVRID;					//DVR ID,用于遥控器
	DWORD dwRecycleRecord;			//是否循环录像,0:不是; 1:是
//以下不能更改
	BYTE sSerialNumber[SERIALNO_LEN];	//序列号
	DWORD dwSoftwareVersion;			//软件版本号,高16位是主版本,低16位是次版本
	DWORD dwSoftwareBuildDate;			//软件生成日期,0xYYYYMMDD
	DWORD dwDSPSoftwareVersion;			//DSP软件版本,高16位是主版本,低16位是次版本
	DWORD dwDSPSoftwareBuildDate;		// DSP软件生成日期,0xYYYYMMDD
	DWORD dwPanelVersion;				// 前面板版本,高16位是主版本,低16位是次版本
	DWORD dwHardwareVersion;			// 硬件版本,高16位是主版本,低16位是次版本
	BYTE byAlarmInPortNum;				//DVR报警输入个数
	BYTE byAlarmOutPortNum;				//DVR报警输出个数
	BYTE byRS232Num;					//DVR 232串口个数
	BYTE byRS485Num;					//DVR 485串口个数
	BYTE byNetworkPortNum;				//网络口个数
	BYTE byDiskCtrlNum;					//DVR 硬盘控制器个数
	BYTE byDiskNum;						//DVR 硬盘个数
	BYTE byDVRType;						//DVR类型, 
	BYTE byChanNum;						//DVR 通道个数
	BYTE byStartChan;					//起始通道号,例如DVS-1,DVR - 1
	BYTE byDecordChans;					//DVR 解码路数
	BYTE byVGANum;						//VGA口的个数
	BYTE byUSBNum;						//USB口的个数
	CHAR reservedData[3];				//保留 
}NET_DLDVR_DEVICECFG,*LPNET_DLDVR_DEVICECFG;

typedef struct
{
	WORD	wDiskCount;			//硬盘个数
	WORD	wDiskState[8];			//硬盘状态,1---正常,2---不正常,3………
	WORD	wDiskAddr[8];		//硬盘地址,高8位是硬盘的主地址(0-3),低8位是硬盘的从地址(0或1)
	DWORD	dwTotalSpace[8];	//硬盘总的空间(单位M)
	DWORD	dwFreeSpace[8];		//硬盘剩余空间
	
	WORD	wChannelCount;		//通道个数
	WORD	wChannelState[16];	//通道信息，高8位表示是否有视频信号，0---视频信号丢失，1---有信号
										//低8位表示是否在录像，0---不在录像，1---在录像
	DWORD	dwBiteRate[16];	//码率
	
	DWORD	dwAlarmInStatus;
	WORD	wAlarmOutStatus;
}NET_DLDVR_DISKCHANNELSTATE;

typedef struct 
{
	CHAR sDVRIP[16];					//DVR IP地址
	CHAR sDVRIPMask[16];				// DVR IP地址掩码
	DWORD dwNetInterface;				//网络接口 1-10MBase-T 2-10MBase-T全双工 3-100MBase-TX 4-100M全双工 5-10M/100M自适应
	WORD wDVRPort;						//端口号
	BYTE byMACAddr[MACADDR_LEN];		//服务器的物理地址
}NET_DLDVR_ETHERNET;

//网络配置结构
typedef struct
{
	DWORD dwSize;
	NET_DLDVR_ETHERNET struEtherNet[MAX_ETHERNET];		//以太网口
	CHAR sManageHostIP[16];								//远程管理主机地址
	WORD wManageHostPort;								//远程管理主机端口号
	CHAR sDNSIP[16];									//DNS服务器地址  
	CHAR sMultiCastIP[16];								//多播组地址
	CHAR sGatewayIP[16];       							//网关地址 
	CHAR sNFSIP[16];									//NAS主机IP地址	
	BYTE sNFSDirectory[PATHNAME_LEN];					//NAS目录
	DWORD dwPPPOE;										//0-不启用,1-启用
	BYTE sPPPoEUser[NAME_LEN];							//PPPoE用户名
	CHAR sPPPoEPassword[PASSWD_LEN];					//PPPoE密码
	CHAR sPPPoEIP[16];									//PPPoE IP地址(只读)
	WORD wHttpPort;										//HTTP端口号
}NET_DLDVR_NETCFG,*LPNET_DLDVR_NETCFG; 

//通道图象结构
//移动侦测
typedef struct
{
	BYTE byMotionScope[18][22];	//侦测区域,共有22*18个小宏块,为1表示该宏块是移动侦测区域,0-表示不是
	BYTE byMotionSensitive;		//移动侦测灵敏度, 0 - 5,越高越灵敏,0xff关闭
	BYTE byEnableHandleMotion;	// 是否处理移动侦测
	NET_DLDVR_HANDLEEXCEPTION strMotionHandleType;	// 处理方式
	NET_DLDVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//布防时间
	BYTE byRelRecordChan[MAX_CHANNUM]; //报警触发的录象通道,为1表示触发该通道	
}NET_DLDVR_MOTION,*LPNET_DLDVR_MOTION;

//遮挡报警区域为704*576
//2010-06-25修改，DM60机型遮挡报警区域为640*480
typedef struct
{
	DWORD dwEnableHideAlarm;			// 是否启动遮挡报警 ,0-否,1-低灵敏度 2-中灵敏度 3-高灵敏度
	WORD wHideAlarmAreaTopLeftX;			// 遮挡区域的x坐标 
	WORD wHideAlarmAreaTopLeftY;			// 遮挡区域的y坐标 
	WORD wHideAlarmAreaWidth;			// 遮挡区域的宽 
	WORD wHideAlarmAreaHeight;			//遮挡区域的高*
	NET_DLDVR_HANDLEEXCEPTION strHideAlarmHandleType;	// 处理方式 	
	NET_DLDVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//布防时间
}NET_DLDVR_HIDEALARM,*LPNET_DLDVR_HIDEALARM;

//信号丢失报警
typedef struct
{
	BYTE byEnableHandleVILost;	// 是否处理信号丢失报警 
	NET_DLDVR_HANDLEEXCEPTION strVILostHandleType;	// 处理方式 	
	NET_DLDVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//布防时间
}NET_DLDVR_VILOST,*LPNET_DLDVR_VILOST;

typedef struct
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN];
	DWORD dwVideoFormat;	// 只读：视频制式 1-NTSC 2-PAL*/
	BYTE byBrightness;  	//亮度,0-255*/
	BYTE byContrast;    	//对比度,0-255*/	
	BYTE bySaturation;  	//饱和度,0-255 
	BYTE byHue;    			//色调,0-255*/	
	//显示通道名
	DWORD dwShowChanName; // 预览的图象上是否显示通道名称,0-不显示,1-显示 区域为704*576 
	WORD wShowNameTopLeftX;				// 通道名称显示位置的x坐标 
	WORD wShowNameTopLeftY;				// 通道名称显示位置的y坐标 
	//信号丢失报警
	NET_DLDVR_VILOST struVILost;
	//移动侦测
	NET_DLDVR_MOTION struMotion;	
	//遮挡报警
	NET_DLDVR_HIDEALARM struHideAlarm;
	//遮挡
	DWORD dwEnableHide;		// 是否启动遮挡 ,0-否,1-是 区域为704*576
	////2010-06-25修改，DM60机型遮挡报警区域为640*480
	WORD wHideAreaTopLeftX;				// 遮挡区域的x坐标 
	WORD wHideAreaTopLeftY;				// 遮挡区域的y坐标 
	WORD wHideAreaWidth;				// 遮挡区域的宽 
	WORD wHideAreaHeight;				//遮挡区域的高
	//OSD
	DWORD dwShowOsd;	// 预览的图象上是否显示OSD,0-不显示,1-显示
	WORD wOSDTopLeftX;				// OSD的x坐标 
	WORD wOSDTopLeftY;				// OSD的y坐标 
	BYTE byOSDType;					// OSD类型(主要是年月日格式) 
							// 0: XXXX-XX-XX 年月日 
							// 1: XX-XX-XXXX 月日年 
							// 2: XXXX年XX月XX日 
							// 3: XX月XX日XXXX年 
	BYTE byDispWeek;				// 是否显示星期 	
	BYTE byOSDAttrib;				// OSD属性:透明，闪烁 
									// 1: 透明,闪烁 	
									// 2: 透明,不闪烁 
									// 3: 闪烁,不透明 
									// 4: 不透明,不闪烁 
}NET_DLDVR_PICCFG,*LPNET_DLDVR_PICCFG;

typedef struct{
	WORD wHideAreaTopLeftX;				// 遮挡区域的x坐标 
	WORD wHideAreaTopLeftY;				// 遮挡区域的y坐标 
	WORD wHideAreaWidth;				// 遮挡区域的宽 
	WORD wHideAreaHeight;				//遮挡区域的高
}NET_DLDVR_SHELTER,*LPNET_DLDVR_SHELTER;

//SDK_V14
typedef struct
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN];
	DWORD dwVideoFormat;	// 只读 视频制式 1-NTSC 2-PAL*/
	BYTE byBrightness;  	//亮度,0-255*/
	BYTE byContrast;    	//对比度,0-255*/	
	BYTE bySaturation;  	//饱和度,0-255 
	BYTE byHue;    			//色调,0-255*/	
	//显示通道名
	DWORD dwShowChanName; // 预览的图象上是否显示通道名称,0-不显示,1-显示 区域大小704*576
	WORD wShowNameTopLeftX;				// 通道名称显示位置的x坐标 
	WORD wShowNameTopLeftY;				// 通道名称显示位置的y坐标 
	//信号丢失报警
	NET_DLDVR_VILOST struVILost;
	//移动侦测
	NET_DLDVR_MOTION struMotion;	
	//遮挡报警
	NET_DLDVR_HIDEALARM struHideAlarm;
	//遮挡  区域大小704*576
	//2010-06-25修改，DM60机型遮挡报警区域为640*480
	DWORD dwEnableHide;		// 是否启动遮挡 ,0-否,1-是
	NET_DLDVR_SHELTER struShelter[MAX_SHELTERNUM]; 
	//OSD
	DWORD dwShowOsd;// 预览的图象上是否显示OSD,0-不显示,1-显示 区域大小704*576
	WORD wOSDTopLeftX;				// OSD的x坐标 
	WORD wOSDTopLeftY;				// OSD的y坐标 
	BYTE byOSDType;					// OSD类型(主要是年月日格式) 
	// 0: XXXX-XX-XX 年月日 
	// 1: XX-XX-XXXX 月日年 
	// 2: XXXX年XX月XX日 
	// 3: XX月XX日XXXX年 
	BYTE byDispWeek;				// 是否显示星期 	
	BYTE byOSDAttrib;				// OSD属性:透明，闪烁 
	// 1: 透明,闪烁 	
	// 2: 透明,不闪烁 
	// 3: 闪烁,不透明 
	// 4: 不透明,不闪烁 
}NET_DLDVR_PICCFG_EX,*LPNET_DLDVR_PICCFG_EX;

//压缩参数
typedef struct
{
	BYTE byStreamType;		//码流类型0-视频流,1-复合流
	BYTE byResolution;  	//分辨率0-DCIF 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF
	BYTE byBitrateType;		//码率类型0:变码率，1:定码率
	BYTE  byPicQuality;		//图象质量 0-最好 1-次好 2-较好 3-一般 4-较差 5-差	
	DWORD dwVideoBitrate; 	//视频码率 0-保留 1-保留 2-32K 3-48k 4-64K 5-80K 6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
				// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K 19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
				//最高位(31位)置成1表示是自定义码流, 0-30位表示码流值(MIN-16K MAX-8192K)。
	DWORD dwVideoFrameRate;	//帧率 0-全部; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4; 8-6; 9-8; 10-10; 11-12; 12-16; 13-20;
}NET_DLDVR_COMPRESSION_INFO,*LPNET_DLDVR_COMPRESSION_INFO;

typedef struct
{
	DWORD dwSize;
	NET_DLDVR_COMPRESSION_INFO struRecordPara; //录像
	NET_DLDVR_COMPRESSION_INFO struNetPara;	//网传
}NET_DLDVR_COMPRESSIONCFG,*LPNET_DLDVR_COMPRESSIONCFG;

//录像
typedef struct
{
	NET_DLDVR_SCHEDTIME struRecordTime;
	BYTE byRecordType;	//0:定时录像，1:移动侦测，2:报警录像， 3:命令触发, 4: 手动录像
	CHAR reservedData[3];
}NET_DLDVR_RECORDSCHED,*LPNET_DLDVR_RECORDSCHED;

typedef struct 
{
	WORD wAllDayRecord;				// 是否全天录像 
	BYTE byRecordType;				// 录象类型 0:定时录像，1:移动侦测，2:报警录像， 3:命令触发, 4: 手动录像
	CHAR reservedData;
}NET_DLDVR_RECORDDAY;

typedef struct 
{
	DWORD dwSize;
	DWORD dwRecord;  //是否录像 0-否 1-是
	NET_DLDVR_RECORDDAY struRecAllDay[MAX_DAYS];
	NET_DLDVR_RECORDSCHED struRecordSched[MAX_DAYS][MAX_TIMESEGMENT];
	DWORD dwRecordTime;	// 录象时间长度 
	DWORD dwPreRecordTime;	// 预录时间 0-不预录 1-5秒 2-10秒 3-15秒 4-20秒 5-25秒 6-30秒 7-0xffffffff(尽可能预录) 
}NET_DLDVR_RECORD,*LPNET_DLDVR_RECORD;

// PTZ type
#define NET_DLDVR_PELCO_P1	0
#define NET_DLDVR_PELCO_P2	1
#define NET_DLDVR_PELCO_D		2
#define NET_DLDVR_DL_0001		3
#define NET_DLDVR_FASTRAX		4	
#define NET_DLDVR_PANASONIC	5
#define NET_DLDVR_ELEC		6
#define NET_DLDVR_SAMSUNG		7
#define NET_DLDVR_KATATEL		8
#define NET_DLDVR_VC			9
#define NET_DLDVR_T3609HD		10
#define NET_DLDVR_LILIN		11
#define NET_DLDVR_PELCO9760	12
#define NET_DLDVR_KRE301		13
#define NET_DLDVR_PIH_1016	14
#define NET_DLDVR_PD_CONST	15
#define NET_DLDVR_PD_NW		16
#define NET_DLDVR_JC4116		17
#define NET_DLDVR_SONY		18
#define NET_DLDVR_YAAN		19
#define NET_DLDVR_ENKEL		20
#define NET_DLDVR_PLD			21
#define NET_DLDVR_P3			22
#define NET_DLDVR_W110		23
#define NET_DLDVR_BFL			24
#define NET_DLDVR_SUNELL		25
#define NET_DLDVR_KTD			26
#define NET_DLDVR_HD600		27

//云台解码器
typedef struct
{
	DWORD dwSize;
	DWORD dwBaudRate;//波特率(bps)，0－1200，1－2400，2－4800，3－9600，4－19200， 5－38400;;
	BYTE byDataBit;// 数据有几位 0－6位，1－7位，2－8位;
	BYTE byStopBit;// 停止位 0－1位，1－2位;
	BYTE byParity;// 校验 0－无校验，1－奇校验，2－偶校验;
	BYTE byFlowcontrol;// 0－无，1－软流控,2-硬流控
	WORD wDecoderType;/*解码器类型,
							0-PELCO-P1
							1-PELCO-P2
							2-PELCO-D
							3-DL-0001
							4-FASTRAX
							5-PANASONIC
							6-ELEC
							7-SAMSUNG
							8-KATATEL
							9-VC
							10-T3609HD
							11-LILIN
							12-PELCO9760
							13-KRE301
							14-PIH-1016
							15-PD-CONST
							16-PD-NW
							17-JC4116
							18-SONY
							19-YAAN
							20-ENKEL
							21-PLD
							22-P3
							23-W110
							24-BFL*/
	WORD wDecoderAddress;	//解码器地址:0 - 255*/
	BYTE bySetPreset[MAX_PRESET];		// 预置点是否设置,0-没有设置,1-设置
	BYTE bySetCruise[MAX_PRESET];		// 巡航是否设置: 0-没有设置,1-设置 
	BYTE bySetTrack[MAX_PRESET];		// 轨迹是否设置,0-没有设置,1-设置
}NET_DLDVR_DECODERCFG,*LPNET_DLDVR_DECODERCFG;

//RS232串口
typedef struct
{
	CHAR sRemoteIP[16];            //远端IP地址
	CHAR sLocalIP[16];             //本地IP地址
	CHAR sLocalIPMask[16];         //本地IP地址掩码
	BYTE sUsername[NAME_LEN];		// 用户名 
	BYTE sPassword[PASSWD_LEN];		// 密码 
	BYTE byPPPMode;            //PPP模式, 0－主动，1－被动	
	BYTE byRedial;            //是否回拨 ：0-否,1-是
	BYTE byRedialMode;        //回拨模式,0-由拨入者指定,1-预置回拨号码
	BYTE byDataEncrypt;	     //数据加密,0-否,1-是
	DWORD dwMTU;              //MTU
	CHAR sTelephoneNumber[PHONENUMBER_LEN];   //电话号码
}NET_DLDVR_PPPCFG,*LPNET_DLDVR_PPPCFG;

typedef struct
{
	DWORD dwSize;
	DWORD dwBaudRate;//波特率(bps)，0－1200，1－2400，2－4800，3－9600，4－19200， 5－38400;
	BYTE byDataBit;// 数据有几位 0－6位，1－7位，2－8位;
	BYTE byStopBit;// 停止位 0－1位，1－2位;
	BYTE byParity;// 校验 0－无校验，1－奇校验，2－偶校验;
	BYTE byFlowcontrol;// 0－无，1－软流控,2-硬流控
	DWORD dwWorkMode;// 工作模式，0－窄带传输(232串口用于PPP拨号)，1－控制台(232串口用于参数控制)，2－透明通道
	NET_DLDVR_PPPCFG struPPPConfig;
}NET_DLDVR_RS232CFG,*LPNET_DLDVR_RS232CFG;

//报警输入
typedef struct
{
	DWORD dwSize;
	BYTE sAlarmInName[NAME_LEN];	// 名称 
	BYTE byAlarmType;	//报警器类型,0：常开,1：常闭
	BYTE byAlarmInHandle;	// 是否处理 
	NET_DLDVR_HANDLEEXCEPTION struAlarmHandleType;	// 处理方式 
	NET_DLDVR_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT];//布防时间
	BYTE byRelRecordChan[MAX_CHANNUM]; //报警触发的录象通道,为1表示触发该通道
	BYTE byEnablePreset[MAX_CHANNUM];		// 是否调用预置点 
	BYTE byPresetNo[MAX_CHANNUM];			// 调用的云台预置点序号,一个报警输入可以调用多个通道的云台预置点, 0xff表示不调用预置点。
	BYTE byEnableCruise[MAX_CHANNUM];		// 是否调用巡航 
	BYTE byCruiseNo[MAX_CHANNUM];			// 巡航 
	BYTE byEnablePtzTrack[MAX_CHANNUM];		// 是否调用轨迹 
	BYTE byPTZTrack[MAX_CHANNUM];			// 调用的云台的轨迹序号 
}NET_DLDVR_ALARMINCFG,*LPNET_DLDVR_ALARMINCFG;


//2010-10-29 报警类型值定义
#define ALARMTYPE_SIGNAL_IN			0		//信号量输入报警	对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_DISKFULL			1		//硬盘满			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_VIDEOLOST			2	//视频信号丢失			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_MOTIONALARM		3	//移动侦测			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_DISK_UNFORMAT		4	//硬盘未格式化			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_DISKERROR			5	//硬盘错误			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_SHELTERALARM		6	//视频遮挡			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_PN_NOT_MATCH		7	//制式不匹配			对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_ILLEGEL_ACCESS	8	//非法访问				对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_TRIPWIRE			10				//绊线		对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_FORBIDDRNZONE	11				//禁区		对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_REMOVAL			12				//遗弃、滞留 对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_GATHER			13				//聚众       对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_RUSH				14               //快速奔跑	 对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_COUNTER			15				//人数统计   对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_HIDE_FACE		16				//人脸伪装检测  对应结构 NET_DLDVR_ALARMINFO和NET_DLDVR_ALARMINFO_600
#define ALARMTYPE_INTELLEGENCE_OUT_GUARD		17				////脱岗检测。600机型特有   对应结构 NET_DLDVR_ALARMINFO_600

//上传报警信息
typedef struct
{
	DWORD dwAlarmType;
	DWORD dwAlarmInputNumber;//报警输入端口
	DWORD dwAlarmOutputNumber[MAX_ALARMOUT];//报警输入端口对应的输出端口，哪一位为1表示对应哪一个输出
	DWORD dwAlarmRelateChannel[MAX_CHANNUM];//报警输入端口对应的录像通道，哪一位为1表示对应哪一路录像,dwAlarmRelateChannel[0]对应第1个通道
	DWORD dwChannel[MAX_CHANNUM];//dwAlarmType为2或3或10-16时，表示哪个通道，dwChannel[0]位对应第1个通道
	DWORD dwDiskNumber[MAX_DISKNUM];//dwAlarmType为4时,表示哪个硬盘
}NET_DLDVR_ALARMINFO,*LPNET_DLDVR_ALARMINFO;

//上传报警信息,600机型使用该结构
typedef struct
{
	DWORD dwAlarmType;
	DWORD dwAlarmInputNumber;//报警输入端口
	DWORD dwAlarmOutputNumber[MAX_ALARMOUT];//报警输入端口对应的输出端口，哪一位为1表示对应哪一个输出
	DWORD dwAlarmRelateChannel[MAX_CHANNUM_EX];//报警输入端口对应的录像通道，哪一位为1表示对应哪一路录像,dwAlarmRelateChannel[0]对应第1个通道
	DWORD dwChannel[MAX_CHANNUM_EX];//dwAlarmType为2或3或10-16时，表示哪个通道，dwChannel[0]位对应第1个通道
	DWORD dwDiskNumber[MAX_DISKNUM];//dwAlarmType为4时,表示哪个硬盘
}NET_DLDVR_ALARMINFO_600,*LPNET_DLDVR_ALARMINFO_600;

//上传报警信息,带用户名，2010-09-01 add
typedef struct
{
	DWORD dwAlarmType;
	DWORD dwAlarmInputNumber;//报警输入端口
	DWORD dwAlarmOutputNumber[MAX_ALARMOUT];//报警输入端口对应的输出端口，哪一位为1表示对应哪一个输出
	DWORD dwAlarmRelateChannel[MAX_CHANNUM_EX];//报警输入端口对应的录像通道，哪一位为1表示对应哪一路录像,dwAlarmRelateChannel[0]对应第1个通道
	DWORD dwChannel[MAX_CHANNUM_EX];//dwAlarmType为2或3或10-16时，表示哪个通道，dwChannel[0]位对应第1个通道
	DWORD dwDiskNumber[MAX_DISKNUM];//dwAlarmType为4时,表示哪个硬盘
	BYTE  byUsername[32];	//用户名
}NET_DLDVR_ALARMINFO_USERNAME,*LPNET_DLDVR_ALARMINFO_USERNAME;


//DVR报警输出
typedef struct
{
	DWORD dwSize;
	BYTE sAlarmOutName[NAME_LEN];	// 名称 
	DWORD dwAlarmOutDelay;	// 输出保持时间(-1为无限，手动关闭) 
	NET_DLDVR_SCHEDTIME struAlarmOutTime[MAX_DAYS][MAX_TIMESEGMENT];// 报警输出激活时间段 				
}NET_DLDVR_ALARMOUTCFG,*LPNET_DLDVR_ALARMOUTCFG;

//DVR本地预览参数
typedef struct
{
	DWORD dwSize;
	BYTE byPreviewNumber;//预览数目,0-1画面,1-4画面,2-9画面,3-16画面,0xff:最大画面
	BYTE byEnableAudio;//是否声音预览,0-不预览,1-预览
	WORD wSwitchTime;//切换时间,0-不切换,1-5s,2-10s,3-20s,4-60s,5-120s,6-300s
	BYTE bySwitchSeq[MAX_WINDOW];//切换顺序,如果lSwitchSeq[i]为 0xff表示不用
}NET_DLDVR_PREVIEWCFG,*LPNET_DLDVR_PREVIEWCFG;

//DVR视频输出
typedef struct 
{
	WORD wResolution;							// 分辨率 
	WORD wFreq;								// 刷新频率 
	DWORD dwBrightness;							// 亮度 
}NET_DLDVR_VGAPARA;

typedef struct
{
	WORD wDisplayLogo;						// 显示视频通道号 
	WORD wDisplayOsd;						// 显示时间 
}NET_DLDVR_MATRIXPARA;

typedef struct
{
	BYTE byVideoFormat;						// 输出制式,0-PAL,1-N	
	BYTE byMenuAlphaValue;					// 菜单与背景图象对比度 
	WORD wScreenSaveTime;					// 屏幕保护时间 
	WORD wVOffset;							// 视频输出偏移 
	WORD wBrightness;						// 视频输出亮度 
	BYTE byStartMode;						// 启动后视频输出模式(0:菜单,1:预览) 
	CHAR reservedData;
}NET_DLDVR_VOOUT;

typedef struct 
{
	DWORD dwSize;
	NET_DLDVR_VOOUT struVOOut[MAX_VIDEOOUT];
	NET_DLDVR_VGAPARA struVGAPara[MAX_VGA];	// VGA参数 
	NET_DLDVR_MATRIXPARA struMatrixPara;		// MATRIX参数 
}NET_DLDVR_VIDEOOUT,*LPNET_DLDVR_VIDEOOUT;

//DVR用户参数
typedef struct
{
	BYTE sUserName[NAME_LEN];		// 用户名 
	BYTE sPassword[PASSWD_LEN];		// 密码 
	DWORD dwLocalRight[MAX_RIGHT];	// 权限 
									//数组0: 本地控制云台
									//数组1: 本地手动录象
									//数组2: 本地回放
									//数组3: 本地设置参数
									//数组4: 本地查看状态、日志
									//数组5: 本地高级操作(升级，格式化，重启，关机)
	DWORD dwRemoteRight[MAX_RIGHT];	// 权限 
									//数组0: 远程控制云台
									//数组1: 远程手动录象
									//数组2: 远程回放
									//数组3: 远程设置参数
									//数组4: 远程查看状态、日志
									//数组5: 远程高级操作(升级，格式化，重启，关机)
									//数组6: 远程发起语音对讲
									//数组7: 远程预览
									//数组8: 远程请求报警上传、报警输出
									//数组9: 远程控制，本地输出
									//数组10: 远程控制串口
	CHAR sUserIP[16];					// 用户IP地址(为0时表示允许任何地址) 
	BYTE byMACAddr[MACADDR_LEN];		// 物理地址 
}NET_DLDVR_USER_INFO,*LPNET_DLDVR_USER_INFO;

typedef struct
{
	DWORD dwSize;
	NET_DLDVR_USER_INFO struUser[MAX_USERNUM];
}NET_DLDVR_USER,*LPNET_DLDVR_USER;

//DVR用户参数扩展
typedef struct
{
	BYTE sUserName[NAME_LEN];		// 用户名 
	BYTE sPassword[PASSWD_LEN];		// 密码 
	DWORD dwLocalRight[MAX_RIGHT];	// 权限 
	//数组0: 本地控制云台
	//数组1: 本地手动录象
	//数组2: 本地回放
	//数组3: 本地设置参数
	//数组4: 本地查看状态、日志
	//数组5: 本地高级操作(升级，格式化，重启，关机)
	DWORD dwLocalPlaybackRight;		// 本地可以回放的通道 bit0 -- channel 1*/
	DWORD dwRemoteRight[MAX_RIGHT];	// 权限 
	//数组0: 远程控制云台
	//数组1: 远程手动录象
	//数组2: 远程回放 
	//数组3: 远程设置参数
	//数组4: 远程查看状态、日志
	//数组5: 远程高级操作(升级，格式化，重启，关机)
	//数组6: 远程发起语音对讲
	//数组7: 远程预览
	//数组8: 远程请求报警上传、报警输出
	//数组9: 远程控制，本地输出
	//数组10: 远程控制串口
	DWORD dwNetPreviewRight;		// 远程可以预览的通道 bit0 -- channel 1*/
	DWORD dwNetPlaybackRight;		// 远程可以回放的通道 bit0 -- channel 1*/
	CHAR sUserIP[16];				// 用户IP地址(为0时表示允许任何地址) 
	BYTE byMACAddr[MACADDR_LEN];	// 物理地址 
}NET_DLDVR_USER_INFO_EX,*LPNET_DLDVR_USER_INFO_EX;

typedef struct
{
	DWORD dwSize;
	NET_DLDVR_USER_INFO_EX struUser[MAX_USERNUM];
}NET_DLDVR_USER_EX,*LPNET_DLDVR_USER_EX;

typedef struct
{
	CHAR sFileName[100];//文件名,格式为%s_%d(其中%d的值实际表示的是录像是否锁定的状态， 1-锁定 0-不锁定）
	NET_DLDVR_TIME struStartTime;//文件的开始时间
	NET_DLDVR_TIME struStopTime;//文件的结束时间
	DWORD dwFileSize;//文件的大小
	DWORD dwChannel;//文件所属的通道，2007-11-05增加
}NET_DLDVR_FIND_DATA,*LPNET_DLDVR_FIND_DATA;

typedef struct
{
	CHAR sFileName[128];//文件名
	U8      cLockStatus;//当前录像是否是锁定状态， 1-锁定 0-不锁定    
	NET_DLDVR_TIME struStartTime;//文件的开始时间
	NET_DLDVR_TIME struStopTime;//文件的结束时间
	DWORD dwFileSize;//文件的大小
	DWORD dwChannel;//文件所属的通道
	U8	  cRecordType;	
}NET_DLDVR_FIND_DATA_EX,*LPNET_DLDVR_FIND_DATA_EX;


//2011-01-11 add,以时间方式计算的文件
typedef struct
{
	NET_DLDVR_TIME struStartTime;//文件的开始时间
	NET_DLDVR_TIME struStopTime;//文件的结束时间
	WORD		Type;	//文件类型
}NET_DLDVR_FILEBYTIME_DATA,*LPNET_DLDVR_FILEBYTIME_DATA;

//------------------------

typedef struct {
	BYTE byRecordStatic; //通道是否在录像,0-不录像,1-录像
	BYTE bySignalStatic; //连接的信号状态,0-正常,1-信号丢失
	BYTE byHardwareStatic;//通道硬件状态,0-正常,1-异常,例如DSP死掉
	CHAR reservedData;
	DWORD dwBitRate;//实际码率
	DWORD dwLinkNum;//客户端连接的个数
	DWORD dwClientIP[MAX_LINK];//客户端的IP地址
}NET_DLDVR_CHANNELSTATE,*LPNET_DLDVR_CHANNELSTATE;

typedef struct 
{
	DWORD dwVolume;//硬盘的容量
	DWORD dwFreeSpace;//硬盘的剩余空间
	DWORD dwHardDiskStatic; //硬盘的状态,休眠,活动,不正常等
							/*
							1 disk ok
							2 无硬盘
							3 硬盘没有格式化
							4 硬盘致命错误
							5 硬盘读写错误
							6 硬盘正在格式化
							 */
	WORD	wDiskAddrMajor;	//硬盘的主地址
	WORD	wDiskAddrMinor;	//硬盘的从地址
}NET_DLDVR_DISKSTATE,*LPNET_DLDVR_DISKSTATE;

typedef struct
{
	DWORD dwDeviceStatic; 	//设备的状态,0-正常,1-CPU占用率太高,超过85%,2-硬件错误,例如串口死掉
	NET_DLDVR_DISKSTATE  struHardDiskStatic[MAX_DISKNUM]; 
	NET_DLDVR_CHANNELSTATE struChanStatic[MAX_CHANNUM];//通道的状态
	BYTE  byAlarmInStatic[MAX_ALARMIN]; //报警端口的状态,0-没有报警,1-有报警
	BYTE  byAlarmOutStatic[MAX_ALARMOUT]; //报警输出端口的状态,0-没有输出,1-有报警输出
	DWORD  dwLocalDisplay;//本地显示状态,0-正常,1-不正常
}NET_DLDVR_WORKSTATE,*LPNET_DLDVR_WORKSTATE;


typedef struct 
{
	DWORD dwVolume;//硬盘的容量
	DWORD dwFreeSpace;//硬盘的剩余空间
	DWORD dwHardDiskStatic; //硬盘的状态,休眠,活动,不正常等
							/*
							1 disk ok
							2 无硬盘
							3 硬盘没有格式化
							4 硬盘致命错误
							5 硬盘读写错误
							6 硬盘正在格式化
							 */
	WORD	wDiskAddrMajor;	//硬盘的主地址
	WORD	wDiskAddrMinor;	//硬盘的从地址
	CHAR	Vender[10]; //制造商
	CHAR	Model[32]; //模式
	INT		Temperature;//实时温度（摄氏度），-1表示失败
	INT		runningStatus;//硬盘是否休眠, -1 获取失败;  0-idle/sleep;  1-running
}NET_DLDVR_DISKSTATE_EX,*LPNET_DLDVR_DISKSTATE_EX;

typedef struct
{
	NET_DLDVR_DISKSTATE_EX  struHardDiskStatic[MAX_DISKNUM]; 	
}NET_DLDVR_DVRDISKSTATE,*LPNET_DLDVR_DVRDISKSTATE;

//系统信息, 2008-04-10 add
typedef struct 
{
    char mac[6]  ;				//mac地址
    WORD  sys_id  ;				//系统ID
    WORD  channel_num ;            //通道个数
    WORD  alarm_in_count ;         //告警输入端口数
    WORD  alarm_out_count;         //告警输出端口数
    WORD  epld_version   ;         //EPLD版本号  
	WORD  Dvr_type;				//0:C型 1: L型 2: LII型  3：mps 4:mpf 5:mpc 6: hi3510 7:编码器 8：解码器 9: hi3511 10:高清IPCamera
    BYTE  Audio_enc;				//0:G711 1: G726
	BYTE  Video_enc;				//0:MPEG4 1: H264
}NET_DLDVR_SYSTEMINFO, *LPNET_DLDVR_SYSTEMINFO ;

//2009-08-13,DVR工作状态（扩展）
typedef struct
{
	DWORD dwDeviceStatic; 	//设备的状态,0-正常,1-CPU占用率太高,超过85%,2-硬件错误,例如串口死掉
	NET_DLDVR_DISKSTATE  struHardDiskStatic[MAX_DISKNUM]; 
	NET_DLDVR_CHANNELSTATE struChanStatic[MAX_CHANNUM_EX];//通道的状态
	BYTE  byAlarmInStatic[MAX_ALARMIN]; //报警端口的状态,0-没有报警,1-有报警
	BYTE  byAlarmOutStatic[MAX_ALARMOUT]; //报警输出端口的状态,0-没有输出,1-有报警输出
	DWORD  dwLocalDisplay;//本地显示状态,0-正常,1-不正常
}NET_DLDVR_WORKSTATE_EX,*LPNET_DLDVR_WORKSTATE_EX;
//DVR日志

//主类型
#define MAJOR_All				0x00	// 全部
#define MAJOR_ALARM				0x1	// 报警 
#define MAJOR_EXCEPTION			0x2	// 异常
#define MAJOR_OPERATION			0x3	// 操作 
#define MAJOR_NET				0x4	//网络相关的日志
#define MAJOR_ACC				0x5 // 访问日志

// 报警 
//次类型
#define MINOR_ALARM_IN			0x1		// 报警输入 
#define MINOR_ALARM_OUT			0x2		// 报警输出 
#define MINOR_MOTDET_START		0x3		// 移动侦测报警开始 
#define MINOR_MOTDET_STOP		0x4		// 移动侦测报警结束 
#define MINOR_HIDE_ALARM_START	0x5		// 遮挡报警开始 
#define MINOR_HIDE_ALARM_STOP	0x6		// 遮挡报警结束
#define MINOR_ALARM_IN_STOP     0x7     //外部告警输入结束
#define MINOR_ALARM_MOTION_START		0x08	//报警监视开始，2009-08-12
#define MINOR_ALARM_MOTION_STOP		0x09	//报警监视结束，2009-08-12
#define MINOR_ALARM_REC_START		0x10	//报警录像开始，2009-08-12
#define MINOR_ALARM_REC_STOP		0x11	//报警录像结束，2009-08-12
#define MINOR_ALARM_END			0x12	//报警清除，2009-08-12

//2009-12-03 add,智能分析相关的日志类型
#define MINOR_ALARM_WIRE     0x90 //绊线
#define MINOR_ALARM_AREA     0x91 //禁区
#define MINOR_ALARM_ABANDON  0x92 //遗弃
#define MINOR_ALARM_TOGETHER 0x93 //聚集
#define MINOR_ALARM_RUN      0x94 //奔跑
#define MINOR_ALARM_COUNTER      0x95 //人数统计
#define MINOR_ALARM_HIDE_FACE      0x96 //人脸伪装


//次类型
#define MINOR_VI_LOST			0x21	// 信号丢失 
#define MINOR_ILLEGAL_ACCESS	0x22	// 非法访问 
#define MINOR_HD_FULL			0x23	// 硬盘满 
#define MINOR_HD_ERROR			0x24	// 硬盘错误 
#define MINOR_DCD_LOST			0x25	// MODEM 掉线 
#define MINOR_IP_CONFLICT		0x26	// IP地址冲突 
#define MINOR_NET_BROKEN		0x27	// 网线断 
#define MINOR_VI_RESUME		    0x28	// 信号恢复
#define MINOR_EXP_PASS		    0x29	// 密码错误，2009-08-12
#define MINOR_NO_HDD		    0x30	// 没有硬盘，2009-08-12
#define MINOR_WIRTE_HDD			0x31	// 访问硬盘错误，2009-08-12
#define MINOR_EXP_THERMAL		0x32	// 温度异常，2009-08-12
#define MINOR_EXP_FAN			0x33	// 风扇异常，2009-08-12

//次类型
#define MINOR_START_DVR			0x41	// 开机 
#define MINOR_STOP_DVR			0x42	// 关机 
#define MINOR_STOP_ABNORMAL		0x43	// 非法关机 

#define MINOR_LOCAL_LOGIN		0x50	// 本地登陆 
#define MINOR_LOCAL_LOGOUT		0x51	// 本地注销登陆 
#define MINOR_LOCAL_CFG_PARM	0x52	// 本地配置参数 
#define MINOR_LOCAL_PLAYBYFILE	0x53	// 本地按文件回放 
#define MINOR_LOCAL_PLAYBYTIME	0x54	// 本地按时间回放 
#define MINOR_LOCAL_START_REC	0x55	// 本地开始录像 
#define MINOR_LOCAL_STOP_REC	0x56	// 本地停止录像 
#define MINOR_LOCAL_PTZCTRL		0x57	// 本地云台控制 
#define MINOR_LOCAL_PREVIEW		0x58	// 本地预览 
#define MINOR_LOCAL_MODIFY_TIME	0x59	// 本地修改时间 
#define MINOR_LOCAL_UPGRADE		0x5a	// 本地升级 
#define MINOR_LOCAL_COPYFILE	0x5b	// 本地备份文件 
#define MINOR_LOCAL_REBOOT		0x5c	// 本地重启，2007-12-11
#define MINOR_LOCAL_FORMAYT	    0x5d	// 本地格式化硬盘，2007-12-11
#define MINOR_LOCAL_FACTORY	    0x5e	// 本地恢复出厂设置，2009-08-12
#define MINOR_LOCAL_SYSTIME	    0x5f	// 本地改变系统时间，2009-08-12

#define MINOR_LOCAL_TIEM_SET	0x60	// 定时录像设定变更，2009-08-12
#define MINOR_LOCAL_ALAM_SET	0x61	// 报警录像设定变更，2009-08-12
#define MINOR_LOCAL_HDD_OVER	0x62	// HDD满時允许覆盖，2009-08-12
#define MINOR_LOCAL_USER_LOGOUT	0x63	// 断开用户，2009-08-12
#define MINOR_LOCAL_HDD_SAFETY	0x64	// HDD安全模式通过，2009-08-12 
#define MINOR_LOCAL_COPY		0x65	// 执行复制，2009-08-12
#define MINOR_LOCAL_CLEAR_RVS	0x66	// RVS地址清除，2009-08-12
#define MINOR_LOCAL_RVS_SET		0x67	// RVS地址变更，2009-08-12 
#define MINOR_LOCAL_AUTO_DELETE	0x68	// 自动删除天数设定变更，2009-08-12 


#define MINOR_REMOTE_LOGIN		0x70	// 远程登录 
#define MINOR_REMOTE_LOGOUT		0x71	// 远程注销登陆 
#define MINOR_REMOTE_START_REC	0x72	// 远程开始录像 
#define MINOR_REMOTE_STOP_REC	0x73	// 远程停止录像 
#define MINOR_START_TRANS_CHAN	0x74	// 开始透明传输 
#define MINOR_STOP_TRANS_CHAN	0x75	// 停止透明传输 
#define MINOR_REMOTE_GET_PARM	0x76	// 远程获得参数 
#define MINOR_REMOTE_CFG_PARM	0x77	// 远程配置参数 
#define MINOR_REMOTE_GET_STATUS 0x78	// 远程获得状态 
#define MINOR_REMOTE_ARM		0x79	// 远程布防 
#define MINOR_REMOTE_DISARM		0x7a	// 远程撤防 
#define MINOR_REMOTE_REBOOT		0x7b	// 远程重启 
#define MINOR_START_VT			0x7c	// 开始语音对讲 
#define MINOR_STOP_VT			0x7d	// 停止语音对讲 
#define MINOR_REMOTE_UPGRADE	0x7e	// 远程升级 
#define MINOR_REMOTE_PLAYBYFILE	0x7f	// 远程按文件回放 
#define MINOR_REMOTE_PLAYBYTIME	0x80	// 远程按时间回放 
#define MINOR_REMOTE_PTZCTRL	0x81	// 远程云台控制 
#define MINOR_REMOTE_LOGO_UPDATE       0x82     ///远程更新LOGO  
#define MINOR_REMOTE_LANGUAGE_UPDATE   0x83     //远程升级自定义语言
#define MINOR_REMOTE_RESOURCE_UPDATE   0x84     //远程升级自定义资源
#define MINOR_REMOTE_RESOURCE_CLEAR    0x85     //远程清除自定义资源
#define MINOR_REMOTE_FORMAT_HDD    0x86     //远程格式化硬盘，2008-10-15
#define MINOR_REMOTE_COPY    0x87     //远程备份，2009-08-12

//2009-08-12 add, NVR网络相关日志
#define MINOR_NET_POP3_AUT	0x100	//POP3认证错误
#define MINOR_NET_POP3_SVR	0x101	//找不到POP3服务器
#define MINOR_NET_POP3_DNS	0x102	//从DNS无法解析POP3服务器地址
#define MINOR_NET_SMTP_AUT	0x103	//SMTP认证错误
#define MINOR_NET_SMTP_SVR	0x104	//找不到SMTP服务器
#define MINOR_NET_SMTP_DNS	0x105	//从DNS无法解析SMTP服务器地址
#define MINOR_NET_MAIL_ERR	0x106	//发送邮件时其他的错误
#define MINOR_NET_DNS_OTH	0x107	//DNS使用时其他的错误
#define MINOR_NET_NTP_OK	0x108	//用NTP时间同步成功
#define MINOR_NET_NTP_DNS	0x109	//从DNS无法解析NTP服务器地址
#define MINOR_NET_NTP_NOANS	0x110	//NTP服务器无应答
#define MINOR_NET_NTP_OTH	0x111	//NTP使用時的其他错误
#define MINOR_NET_DHCP_FAIL	0x112	//DHCP地址的Update失敗
#define MINOR_NET_DHCP_LOST	0x113	//DHCP地址的租期到期，地址失效
#define MINOR_NET_DHCP_OTH	0x114	//DHCP使用時的其他错误
#define MINOR_DDNSIPADDUPDATE_OK	0x115	//成功更新
#define MINOR_DDNSUSERASS_ERR	0x116	//在主机名指定的域名不对的情况,用户名和主机名不一致的情况
#define MINOR_DNSIPADDUPDATE_ERR	0x117	//在DNS发生错误的情况
#define MINOR_DNSOTHER	0x118	//其他的错误
#define MINOR_DDNSSVRRES_ERR	0x119	//从服务器没有应答的情况


#define  MINOR_ACCE_LOCAL_MANA_LOGIN    0x121               //本地登陆
#define  MINOR_ACCE_LOCAL_OPER_LOGIN    0x122              //本地登陆
#define  MINOR_ACCE_LOCAL_VIEW_LOGIN    0x123               //本地登陆
#define  MINOR_ACCE_REMOTE_MANA_LOGIN   0x124               //网络登陆
#define  MINOR_ACCE_REMOTE_OPER_LOGIN   0x125               //网络登陆
#define  MINOR_ACCE_REMOTE_VIEW_LOGIN   0x126              //网络登陆
#define  MINOR_ACCE_MANA_AUTO_LOGIN     0x127              //自动登录
#define  MINOR_ACCE_OPER_AUTO_LOGIN     0x128               //自动登录
#define  MINOR_ACCE_VIEW_AUTO_LOGIN     0x129              //自动登录


typedef struct 
{
	NET_DLDVR_TIME strLogTime;
	DWORD	dwMajorType;	//主类型 1-报警; 2-异常; 3-操作; 0xff-全部
	DWORD	dwMinorType;//次类型 0xff-全部; 
	BYTE	sUser[32]; //操作面板的用户名
//	BYTE	sNetUser[MAX_NAMELEN];//网络操作的用户名
	CHAR	sRemoteHostAddr[16];//远程主机地址
	DWORD	dwParaType;//参数类型
	DWORD	dwChannel;//通道号
	DWORD	dwDiskNumber;//硬盘号
	DWORD	dwAlarmInPort;//报警输入端口
	DWORD	dwAlarmOutPort;//报警输出端口		
}NET_DLDVR_LOG,*LPNET_DLDVR_LOG;

typedef struct 
{
	CHAR sRedirectorIP[16];	// 转发服务器IP地址
	WORD wRedirectorPort;	//端口号
}NET_DLDVR_REDIRECTORINFO, *LPNET_DLDVR_REDIRECTORINFO;

typedef struct
{
	BYTE Output[MAX_ALARMOUT];
}NET_DLDVR_ALARMOUTSTATUS,*LPNET_DLDVR_ALARMOUTSTATUS;

typedef struct
{
	USHORT m_Year;
	USHORT m_Month;
	USHORT m_Day;
	USHORT m_Hour;
	USHORT m_Minute;
	USHORT m_Second;
	BYTE DeviceName[24];	//设备名称
	DWORD dwChannelNumer;	//通道号
	BYTE CardNumber[32];	//卡号
	CHAR cTradeType[12];	//交易类型
	DWORD dwCash;			//交易金额
}NET_DLDVR_TRADEINFO,*LPNET_DLDVR_TRADEINFO;

//以下为ATM专用
//帧格式
typedef struct
{
	BYTE code[12];		// 代码 
}NET_DLDVR_FRAMETYPECODE;

typedef struct
{
	DWORD dwSize;	
	CHAR sATMIP[16];						// ATM IP地址 
	DWORD dwATMType;						// ATM类型 
	DWORD dwInputMode;						// 输入方式		
	DWORD dwFrameSignBeginPos;              // 报文标志位的起始位置
	DWORD dwFrameSignLength;				// 报文标志位的长度 
	BYTE  byFrameSignContent[12];			// 报文标志位的内容 
	DWORD dwCardLengthInfoBeginPos;			// 卡号长度信息的起始位置 
	DWORD dwCardLengthInfoLength;			// 卡号长度信息的长度 
	DWORD dwCardNumberInfoBeginPos;			// 卡号信息的起始位置 
	DWORD dwCardNumberInfoLength;			// 卡号信息的长度 
	DWORD dwBusinessTypeBeginPos;           // 交易类型的起始位置 
	DWORD dwBusinessTypeLength;				// 交易类型的长度 
	NET_DLDVR_FRAMETYPECODE frameTypeCode[10];// 类型 
}NET_DLDVR_FRAMEFORMAT,*LPNET_DLDVR_FRAMEFORMAT;

//DS-6001D/F 
//解码设备控制码定义
#define NET_DEC_STARTDEC		1
#define NET_DEC_STOPDEC			2
#define NET_DEC_STOPCYCLE		3
#define NET_DEC_CONTINUECYCLE	4
//连接的通道配置
typedef struct 
{
	CHAR sDVRIP[16];				// DVR IP地址 
	WORD wDVRPort;			 		// 端口号 
	BYTE sUserName[NAME_LEN];		// 用户名 
	BYTE sPassword[PASSWD_LEN];		// 密码 
	BYTE byChannel;					// 通道号 
	BYTE byLinkMode;				// 连接模式 	
	BYTE byLinkType;				// 连接类型 0－主码流 1－子码流 
}NET_DLDVR_DECCHANINFO, *LPNET_DLDVR_DECCHANINFO;

//每个解码通道的配置
typedef struct 
{	
	BYTE	byPoolChans;			//每路解码通道上的循环通道数量, 最多4通道
	NET_DLDVR_DECCHANINFO struchanConInfo[MAX_DECPOOLNUM];
	BYTE	byEnablePoll;			//是否轮巡 0-否 1-是
	BYTE	byPoolTime;				//轮巡时间 0-保留 1-10秒 2-15秒 3-20秒 4-30秒 5-45秒 6-1分钟 7-2分钟 8-5分钟 	
}NET_DLDVR_DECINFO, *LPNET_DLDVR_DECINFO;

//整个设备解码配置
typedef struct 
{
	DWORD	dwSize;
	DWORD	dwDecChanNum; 		//解码通道的数量
	NET_DLDVR_DECINFO struDecInfo[MAX_DECNUM];
}NET_DLDVR_DECCFG, *LPNET_DLDVR_DECCFG;
//2005-08-01
// 解码设备透明通道设置 
typedef struct 
{
	DWORD dwEnableTransPort;	// 是否启动透明通道 0－不启用 1－启用
	CHAR sDecoderIP[16];		// DVR IP地址 
	WORD wDecoderPort;			// 端口号 
	WORD wDVRTransPort;			// 配置前端DVR是从485/232输出，1表示485串口,2表示232串口 		
	CHAR cReserve[4];
}NET_DLDVR_PORTINFO, *LPNET_DLDVR_PORTINFO;

typedef struct 
{
	DWORD dwSize;
	NET_DLDVR_PORTINFO struTransPortInfo[MAX_TRANSPARENTNUM]; // 数组0表示485 数组1表示232 
}NET_DLDVR_PORTCFG, *LPNET_DLDVR_PORTCFG;

// 控制网络文件回放 
typedef struct 
{
	DWORD dwSize;
	CHAR sDecoderIP[16];		// DVR IP地址 
	WORD wDecoderPort;			// 端口号 
	WORD wLoadMode;				// 回放下载模式 1－按名字 2－按时间 
	union
	{
		BYTE byFile[100];		// 回放的文件名 
		struct 
		{
			DWORD dwChannel;
			BYTE sUserName[NAME_LEN];	//请求视频用户名
			BYTE sPassword[PASSWD_LEN];	// 密码 
			NET_DLDVR_TIME struStartTime;	// 按时间回放的开始时间 
			NET_DLDVR_TIME struStopTime;	// 按时间回放的结束时间 			
		}bytime;
	}mode_size;
}NET_DLDVR_PLAYREMOTEFILE,*LPNET_DLDVR_PLAYREMOTEFILE;

//当前设备解码连接状态
typedef struct 
{
	DWORD dwWorkType;		//工作方式：1：轮巡、2：动态连接解码、3：按文件回放 4：按时间回放
	CHAR sDVRIP[16];		//连接的设备ip*/
	WORD wDVRPort;			//连接端口号
	BYTE byChannel;					// 通道号 
	BYTE byLinkMode;				// 连接模式 
	DWORD	dwLinkType;		//连接类型 0－主码流 1－子码流
	union
	{
		struct 
		{
			BYTE sUserName[NAME_LEN];	//请求视频用户名
			BYTE sPassword[PASSWD_LEN];	// 密码 
			CHAR cReserve[52];
		}userInfo;
		struct 
		{			
			BYTE   fileName[100];		
		}fileInfo;
		struct 
		{
			DWORD	dwChannel;
			BYTE	sUserName[NAME_LEN];	//请求视频用户名
			BYTE	sPassword[PASSWD_LEN];	// 密码 
			NET_DLDVR_TIME struStartTime;		// 按时间回放的开始时间 
			NET_DLDVR_TIME struStopTime;		// 按时间回放的结束时间 	
		}timeInfo;
	}objectInfo;
}NET_DLDVR_DECCHANSTATUS, *LPNET_DLDVR_DECCHANSTATUS;

typedef struct 
{
	DWORD   dwSize;
	NET_DLDVR_DECCHANSTATUS struDecState[MAX_DECNUM];	
}NET_DLDVR_DECSTATUS, *LPNET_DLDVR_DECSTATUS;

//叠加字符
typedef struct 
{
	WORD wShowString;				// 预览的图象上是否显示字符,0-不显示,1-显示 区域大小704*576,单个字符的大小为32*32
	WORD wStringSize;				// 该行字符的长度，不能大于44个字符 
	WORD wShowStringTopLeftX;		// 字符显示位置的x坐标 
	WORD wShowStringTopLeftY;		// 字符名称显示位置的y坐标 
	CHAR sString[44];				// 要显示的字符内容 
}NET_DLDVR_SHOWSTRINGINFO, *LPNET_DLDVR_SHOWSTRINGINFO;

//叠加字符
typedef struct 
{
	DWORD dwSize;
	NET_DLDVR_SHOWSTRINGINFO struStringInfo[MAX_STRINGNUM];				// 要显示的字符内容 
}NET_DLDVR_SHOWSTRING, *LPNET_DLDVR_SHOWSTRING;

//图片质量
typedef struct
{
	WORD	wPicSize;				// 0=CIF, 1=QCIF, 2=D1 
	WORD	wPicQuality;			// 图片质量系数 0-最好 1-较好 2-一般 
}NET_DLDVR_JPEGPARA, *LPNET_DLDVR_JPEGPARA;

//HS设备辅助输出参数
typedef struct tagAUXOUT
{
	DWORD dwSize;
	DWORD dwAlarmOutChan;                       	// 选择报警弹出大画面的输出通道: 0:主输出/1:辅1/2:辅2/3:辅3/4:辅4 
	DWORD dwAlarmChanSwitchTime;                	// 报警通道切换时间：1:1秒 - 10:10秒 
	DWORD dwAuxSwitchTime[MAX_AUXOUT];		// 辅助输出切换时间: 0-不切换,1-5s,2-10s,3-20s,4-30s,5-60s,6-120s,7-300s 
	BYTE  byAuxOrder[MAX_AUXOUT][MAX_WINDOW];	// 辅助输出预览顺序, 0xff表示相应的窗口不预览 
}NET_DLDVR_AUXOUTCFG, *LPNET_DLDVR_AUXOUTCFG;

//DVR主机软件版本,2007-07-27增加
typedef struct
{
	char version[128];
}NET_DLDVR_DEVICEVERSION,*LPNET_DLDVR_DEVICEVERSION;

//2007-12-11 SY_add 
typedef struct _MONITOR_INFOEX
{
	BYTE		has_audio;		//是否含有音频 1:含有,0:没有包含
	BYTE		quality;		//质量 0:"差", 1:"普通", 2:"好", 3:"较好", 4:"最好"
	BYTE		frame_rate;		//帧率 0 ~ 25
	BYTE		video_format;	//视频格式 0:PAL, 1:NTSC
	BYTE		resolution;		//分辨率
	DWORD		Mcast_IP;		//多播组地址(如果DVR不是使用多播方式则返回0)
	WORD		Mcast_port;		//多播组地址(如果DVR不是使用多播方式则返回0)
}MONITOR_INFOEX;

//NVR日志结构-------------------------------------------------------------
typedef struct 
{
	unsigned int         log_time ;       //时间
    unsigned int         major_type;      //日志类型
    unsigned int         minor_type ;     //日志事件
    unsigned int         reason;          //（端口号，通道号，硬盘号，频道号,时间）
    unsigned int         remote_IP;      //IP 地址.2009-04-03 add
    unsigned char		user_name[10];   //登陆用户名
	void hton(){
		log_time	= htonl( log_time);
		major_type	= htonl( major_type );
		minor_type	= htonl( minor_type );
		reason		= htonl( reason );
		//	remote_IP   = htonl(remote_IP);
	}
	void ntoh(){
		hton();
	}
}LOG_RECORD_NVR,  *LPLOG_RECORD_NVR ;

//2009-08-19 added for 网络异常掉线等回调结构
typedef struct
{
	UINT Msg;
	UINT wParam;
	UINT lPara;
	UINT reserve;
}NET_DLDVR_EXCEPTION;

//原始数据采集信息， 2010-09-07
typedef struct
{
	int TotalNum;
	int CurrNum;
}NET_DLDVR_INFRAREDFRAME;

//以下NVR机型主寄结构， begin------------------------------
#define DECODE_MAX 4
#define MAX_VIDEO_CHANNEL_NVR 32
#define SYS_RVS_MAX   32

#define    MAX_PRESET_NUM          128
#define    MAX_ROUTE_NUM           8
#define    ORIGIN_PRESET_POSITION    1

#define    MAX_VIEWER_USER     20       /*浏览人员数 */
#define    MAX_OPERATOR_USER   8        /*操作人员数 */
#define    MAX_ADMIN_USER      4        /*管理人员数 */

#define    MAX_PASSWORD_MANAGER_CHAR  8 /* manager password max lenght */
#define    MAX_PASSWORD_OPERATOR_CHAR  6        /* operator password max lenght */
#define    MAX_PASSWORD_VIEWER_CHAR  4  /* viewer password max lenght */
#if 0
#define    DISPLAY_SINGLE      0
#define    DISPLAY_SEQUENCE    1
#define    DIAPLAY_BLANK       2
#endif

#define    MAX_SEND_TO_MAIL            4
#define    MAX_ADDRESS_CHAR            256
#define    MAX_MAIL_SUBJECT_CHAR       256
#define    MAX_USER_NAME_CHAR          256
#define    MAX_PASSWORD_CHAR           256

#define PPPOE_PASS           6

#define CHANNEL_NAME_LENGTH 16
#define RVS_PWD_LENGTH      16
#define NET_MAC_LENGTH      6

#define  VIEW_SPLIT_ONE      1  //单画面
#define  VIEW_SPLIT_TWO      2  //两画面
#define  VIEW_SPLIT_FOUR     4  //四画面
#define  VIEW_SPLIT_SIX      6  //六画面
#define  VIEW_SPLIT_NINE     8  //九画面

#define SCREEN_TIME     0
#define SCREEN_STAT     1
#define SCREEN_SHORTCUT 2

#define PLAN_REC_ONEDAY     0X01
#define PLAN_REC_EVERYDAY   0X02
#define PLAN_REC_WEEKLY     0X03
#define PLAN_REC_MONTHLY    0X04
#define PLAN_REC_ITEMS_MAX  1024

typedef struct
{
    U16  year  ;
    U8   month ;
    U8   date  ;
}DATE_INFO ;

// struct defined for time 
typedef struct
{
    U8   hour   ;
    U8   minute ;
    U8   second ;
    U8   reserved ;
}TIME_INFO ;

typedef struct
{
    DATE_INFO        date ;
    TIME_INFO        time ;
}DVR_TIME;

struct TIME_PHASE_NVR
{
    TIME_INFO start;        /*开始时间 */
    TIME_INFO end;          /*结束时间 */
    U8 active;              /*0: ON,   1:OFF */

    TIME_PHASE_NVR(){
		memset( this, 0x00, sizeof(TIME_PHASE_NVR) );
	}
};

struct IP
{
    U8 a;
    U8 b;
    U8 c;
    U8 d;

    IP(){
		memset( this, 0x00, sizeof(IP) );
	}
};

struct SUMMER_TIME
{
        /*进入夏令时时刻 */
    U32  in_time  ;     //夏令时进入时间
    U32  out_time ;     //夏令时出去时间

    SUMMER_TIME(){
		memset( this, 0x00, sizeof(SUMMER_TIME) );
	}
};


/* 系统设置结构 */
struct SYSTEM_SETUP_ADV
{
    U8 Date_fmt;            /*日期格式
                                   0 : yyyy-mm-dd  年-月-日
                                   1 : dd-mm-yyyy  日-月-年
                                   2 : dd-mmm-yyyy
                                   3 : mm-dd-yyyy  月-日-年
                                   4 : mmm-dd-yyyy */

    U8 Time_fmt;            /*时间格式
                                   0 : 24小时制
                                   1 : 12小时制 */
    U8 auto_time_adjust;    /*是否自动矫正时间  1:是;0:否  */
    U8 summer_time;         /*是否执行夏令时    1:是;0:否  */
    SUMMER_TIME summer_tbl[5];      /*夏令时表 */

    U16 overlay_days;       /* 硬盘向前自动覆盖的天数 0~200天 0不保留 */
    U8 search_time;         /* search 时间 */
    U8 video_overlay;       /* 硬盘录象满时，是否自动覆盖 1:是 0:否 */
    // NTP参数
    U8 NTP_time_adjust;     /* 是否NTP时间自动矫正 1:是;0:否 */
    U8 NTP_refresh_interval;        /*NTP时间矫正间隔 *//*
                                           0 : 1h
                                           0 : 2h
                                           0 : 6h
                                           0 : 12h
                                           0 : 24h
                                         */
    U8 NTP_name[64];       /* NTP server ip地址或主机名 */
    U16 NTP_port;
    int enable_sntp;  /*0：不允许，！：允许*/

    SYSTEM_SETUP_ADV(){
		memset( this, 0x00, sizeof(SYSTEM_SETUP_ADV) );
	}
};

struct NET_DLNVR_SYSTEM_SETUP
{
    U8 time_zone;           /* 时区， *//*
                                  0 : "GMT-12:00",
                                  1 :  "GMT-11:00",
                                  2 :  "GMT-10:00",
                                  3 :   "GMT-9:00",  
                                  4 :   "GMT-8:00",
                                  5 :   "GMT-7:00",
                                  6 :   "GMT-6:00",
                                  7 :   "GMT-5:00",
                                  8 :   "GMT-4:00",
                                  9 :   "GMT-3:00",
                                  10 :   "GMT-3:30",
                                  11 :  "GMT-2:00",
                                  12 :   "GMT-1:00",
                                  13 :   "GMT+0:00",
                                  14 :   "GMT+1:00",
                                  15 :   "GMT+2:00",
                                  16 :   "GMT+3:00",
                                  17 :   "GMT+3:30",
                                  18 :   "GMT+4:00",
                                  19 :   "GMT+4:30",
                                  20 :   "GMT+5:00",
                                  21 :   "GMT+5:30",
                                  22 :   "GMT+5:45",
                                  23 :   "GMT+6:00",
                                  24 :   "GMT+6:30",
                                  25 :   "GMT+7:00",
                                  26 :   "GMT+8:00",
                                  27 :   "GMT+9:00",
                                  28 :   "GMT+9:30",
                                  29 :   "GMT+10:00",
                                  30 :   "GMT+11:00",
                                  31 :   "GMT+12:00",
                                  32 :  "GMT+13:00",
                                     */

    U32 language;           /* 语言选择 *//*
                                   0 ： 英语
                                   1 ： 日语
                                   2 ： 中文
                                   3 ： custom */

    U32 auto_login_user;    /* 自动登陆的默认用户类型 *//*
                                    0 :  浏览类用户
                                   1 :  操作类用户
                                   2 :  管理类用户
                                   3:  没有自动登陆
                                 */
    U32 alarm_beep;         /* 告警时蜂鸣时间 0~180秒 0表示没有蜂鸣 */
    U16 auto_lock_time;     /* 无操作时，系统退出时间，5~999 秒 */
    U8 nvr_name[16];

    U8 address; //DVR地址 缺省值：1. 2010-08-05 add

    SYSTEM_SETUP_ADV system_setup_adv;

    NET_DLNVR_SYSTEM_SETUP()
    {
		memset( this, 0x00, sizeof(NET_DLNVR_SYSTEM_SETUP) );
	}
};

struct PLAN_REC_ITEM
{
    U16 year;
    U8  month;
    U8  day;
    U8  stage_check; //时段选中标志， 按位选中，0x01:时段一  0x02: 时段二 0x03: 时段一二
    TIME_INFO stage1_start;
    TIME_INFO stage1_end;
    TIME_INFO stage2_start;
    TIME_INFO stage2_end;
    U8  plan_type; //0x01:当天 0x02:每天 0x03:每周 0x04:每月

    PLAN_REC_ITEM()
    {
		memset( this, 0x00, sizeof(PLAN_REC_ITEM) );
	}
};

/* 录象参数设置结构 */
struct NET_DLNVR_RECORDING_SETUP {
    PLAN_REC_ITEM plan_rec[MAX_VIDEO_CHANNEL_NVR][PLAN_REC_ITEMS_MAX];
    U16 plan_rec_items_count[MAX_VIDEO_CHANNEL_NVR];

    NET_DLNVR_RECORDING_SETUP()
    {
		memset( this, 0x00, sizeof(NET_DLNVR_RECORDING_SETUP) );
	}
};

/*显示设置*/
struct SINGLE {
    U8 single;              /*0:Single   1:Sequence   2: -- */
    U8 channel;             /*频道号 */
    U8 spot;                /*0:不支持报警  1：支持报警切换 */

    SINGLE(){
		memset( this, 0x00, sizeof(SINGLE) );
	}
};

struct NET_DLNVR_DISPLAY_SETUP
{
    U8 screen_show_type;    /*   */
    U8 seq_dwell[VIEW_SPLIT_NINE];           /*通道轮巡时的切换时间 范围:5~30 单位:秒 */
    SINGLE split9[VIEW_SPLIT_NINE];
    U8 display;             /*720p:1   1080i:2   1080p:2*/
    U8 mg3500;          /*mg3500分辨率, Standard:0(该项主机不用）   720p:1   1080i:2 */
 //   U32 seq_ch;         /*巡监按位设置*/
    U32 seq_ch[VIEW_SPLIT_NINE];         /*巡监按位设置*/

    NET_DLNVR_DISPLAY_SETUP(){
		memset( this, 0x00, sizeof(NET_DLNVR_DISPLAY_SETUP) );
	}
};

/*通信设置PPPOE*/
struct PPPOE_SET
{
    U8 pppoe;               /*0:ON   1:OFF */
    U8 name[16];
    U8 pass[16];
    U32 ip;

    PPPOE_SET(){
		memset( this, 0x00, sizeof(PPPOE_SET) );
	}
};

struct SNTP_SET
{
    U8 sntp_add[256];
    U16 sntp_port;    /*默认为123*/
    int enable_sntp;  /*0：不允许，1：允许*/

    SNTP_SET(){
		memset( this, 0x00, sizeof(SNTP_SET) );
	}
};


struct NETWORK_SETUP
{
    U8 enable_DHCP;         /*是否允许DHCP 0:ON ， 1:OFF */
    U32 ip_address;         /*本机IP地址 */
    U32 net_mask;           /*子网隐码 */
    U32 net_gateway;        /*网关 */

    U8 line_speed;

    U16 server_port;        /* 端口1~65535 */
    U8 enable_DNS;          /* 是否打开域名服务  0:ON,1:OFF */
    U32 DNS_ip_address1;
    U32 DNS_ip_address2;
    U16 http_port;
    U32 alarm_add;
    U8 alarm_port;
    U32 alarm_center_ipaddr;
    U16 alarm_center_port;
   // SNTP_SET sntp_set;            //2009-02-13改动的地方
    PPPOE_SET pppoe_set;

    NETWORK_SETUP(){
		memset( this, 0x00, sizeof(NETWORK_SETUP) );
	}
} ;

/*RS485设置*/
struct REMOTE_SETUP
{
    U8 address;
    U8 baud_rate;           // 0:2400 ; 1:4800 ; 2:9600 ; 3:19200 ; 4:38400
    U8 data_bit;            // 数据位  0:6;  1:7;   2:8
    U8 stop_bit;            //停止位   0:1;  1:2
    U8 parity_bit;          //校验位   0:无; 1:奇校验;  2:偶校验
};

struct NVR_LIST
{
    U32 nvr_ip;
    int nvr_port;
    U8 password[MAX_PASSWORD_MANAGER_CHAR + 1];
    U8 username[10];
    
    NVR_LIST(){
		memset( this, 0x00, sizeof(NVR_LIST) );
	}
};

struct MAIL_SETUP
{
    U8 email;                /*email 0：不发送 1：发送*/
    U8 send_to[MAX_SEND_TO_MAIL][MAX_ADDRESS_CHAR+1] ; /* ?256*/
    U8 TRM[MAX_SEND_TO_MAIL] ;               /* 0 : ; 1 : */
    U8 ERR[MAX_SEND_TO_MAIL] ;               /* 0 : ; 1 : */  
    U8 smtp_server_address[MAX_ADDRESS_CHAR+1] ; /* ?256 */
    U16 smtp_port ;        /* 1~65535 */
    U8 pop_server_address[MAX_ADDRESS_CHAR+1] ; /* ?256 */
    U8 authentication ;    /* 1,OFF;2 : POP ;3 : SMTP */
    U8 user_name[MAX_USER_NAME_CHAR+1] ; /* 用户名 */
    U8 password[MAX_PASSWORD_CHAR+1] ;   /* 密码 */
    U8 from[MAX_ADDRESS_CHAR+1];
    U8 email_name[MAX_MAIL_SUBJECT_CHAR+1];

    MAIL_SETUP(){
		memset( this, 0x00, sizeof(MAIL_SETUP) );
	}

};

/*解码器*/
struct DECODE_CHAN
{
    U8 select;     /*选择监视还是巡检 00:都没有选择 01:监视 02:巡检*/
    U8 moni_chan;  /*监视频道号*/
    U32 seq_chan;  /*巡检频道号 按位操作*/
    U16 poll_time;  /*轮巡时间*/

    DECODE_CHAN(){
		memset( this, 0x00, sizeof(DECODE_CHAN) );
	}

};

struct DECODE_SETUP
{
    U16 decode_num;      /*解码器编号*/
    U32 decode_addr;      /*解码地址*/
    U8  decode_flag;     /*1：IPCAM连接 0:NVR*/
    U16 decode_port;     /*解码端口*/
    U8 decode_pass[16];/*解码密码*/
    U8 decode_user[32];/*解码用户名*/
    U8 decode_resol;     /*分辨率*/
  //  U8 channel_sel[6];  /*通道选择*/
 //   U8 decode_display;  /*显示控制*/
    U8 split;            /*画面分割  0 : 多分割  01: 1通道最大化  02: 2通道最大化*/
    DECODE_CHAN decode_chan[2]; /*按两个通道显示*/
    
    DECODE_SETUP(){
		memset( this, 0x00, sizeof(DECODE_SETUP) );
	}

} ;

struct DDNS_SETUP
{
    U8 ddns_server;
    char ddns_user[16];
    char ddns_pass[8];
    char ddns_domain[24];
    char ddns_server_addr[24];
    U8 ddns_port;

     DDNS_SETUP(){
		memset( this, 0x00, sizeof(DDNS_SETUP) );
	}
};

struct NET_DLNVR_COMMUNICATION_SETUP
{
    NETWORK_SETUP network;  /* 网络设定 */
   // REMOTE_SETUP remote;    /* 485口设定 */
    NVR_LIST nvrlist[8];    /*NVR 列表 */
    MAIL_SETUP mail_setup; /*email 设置, 2009-02-04 add*/
    DECODE_SETUP decode_setup[DECODE_MAX];/*2009-04-08 add */
    DDNS_SETUP ddns;

    NET_DLNVR_COMMUNICATION_SETUP(){
		memset( this, 0x00, sizeof(NET_DLNVR_COMMUNICATION_SETUP) );
	}
};

/*用户管理*/
struct USER_NAME_PASSWORD
{
    U8 name[ MAX_USER_NAME_CHAR];
    U8 password[MAX_PASSWORD_MANAGER_CHAR];
    U32 adv_rights;      /***高级 ***/
    U32 rec_rights;      /*录像权限，按位计算频道*/
    U32 playback_rights; /*回放权限，按位计算频道*/

    USER_NAME_PASSWORD(){
		memset( this, 0x00, sizeof(USER_NAME_PASSWORD) );
	}
};

struct NET_DLNVR_USER_MANAGE_SETUP
{
   USER_NAME_PASSWORD name_pass[32];

   NET_DLNVR_USER_MANAGE_SETUP(){
		memset( this, 0x00, sizeof(NET_DLNVR_USER_MANAGE_SETUP) );
	}
};

struct NET_DLNVR_SEARCH_NET_SETUP
{
    U32 from;
    U32 to;

    NET_DLNVR_SEARCH_NET_SETUP(){
		memset( this, 0x00, sizeof(NET_DLNVR_SEARCH_NET_SETUP) );
	}
};

/*RVS结构定义*/
struct  NET_DLNVR_SYS_RVS
{
    U32 ip;                             //IP地址
    U16  port;                           //连接的端口号
    U8   proto_type;                     // 连接方式 TCP，UDP，MC
    U8   type;                           //ipcamera, dvr, dvs等
        //0-IPC5  1-IPC3(此项不用) 2-编码器  3-景阳IPC
  //  U8   can_rec;                        //是否能够录像 1：能  0：不能
    U8   coder_path;                   //
    U8   name[CHANNEL_NAME_LENGTH];      //名称
    U8   password[RVS_PWD_LENGTH];
    U8   alias[CHANNEL_NAME_LENGTH];
    U8   talk_port;                     //对讲端口号
    
	NET_DLNVR_SYS_RVS(){
		memset( this, 0x00, sizeof(NET_DLNVR_SYS_RVS) );
	}
};

struct ALM_STAGE
{
    U32  start_time;     //开始时间
    U32  end_time;       //结束时间
    int flag;            //1:选中  0：没选
//    U32 input   ;       //输入事件，按位检索，前12位为后端输入其他为ipc报警

    ALM_STAGE(){
		memset( this, 0x00, sizeof(ALM_STAGE) );
	}
};

/* 本地告警设置 */
struct NET_DLNVR_EVENT_SETUP
{
    U32         alm_rec_time ;
    U32         pre_rec_time ;
    ALM_STAGE   stage[4] ;
    U8 ararm_port;       /*报警输入(1-12)*/
    U8 alarm_beep;         /*告警时是否蜂鸣 */
    U16 monitor_spot;       /*报警切换（0-32）0：不切换*/
    U8 alarm_out[4];         /*报警输出 0:否 1:是*/
    U16 alarm_time;          /*报警时间*/
    U8 alarm_channel[4];
    U8 alarm_in_flag;       /*报警输出0：OFF ，1：ON*/

    NET_DLNVR_EVENT_SETUP(){
		memset( this, 0x00, sizeof(NET_DLNVR_EVENT_SETUP) );
	}
} ;

/**********************串口设置************************************************/
struct SERIAL_CONFIG_NVR
{
    U8 baud_rate;   // 波特率
                        //0 1200
                        //1 2400
                        //2 4800
                        //3 9600
                        //4 19200
                        //5 38400
        /**************************************************************************
        串口控制字节，默认值为0x42，各比特定义如下：
            7   6   5   4   3   2   1   0
            E   A   A   B   C   C   D   D
        A 流控方式，0-不控， 1-硬件流控， 2-软件流控
        B 停止位， 0-1个， 1-2个
        C 奇偶校验，0-不验， 1-奇校验， 2-偶校验
        D 数据位， 0-6位，1-7位， 2-8位
        E 串口选择， 0-串口1， 1-串口1
        **************************************************************************/
    U8  com_config;

     SERIAL_CONFIG_NVR(){
		memset( this, 0x00, sizeof(SERIAL_CONFIG_NVR) );
	}
};


/**********************云台设置************************************************/
struct CRUISE_STRATEGY
{
    U8 route_id;//默认0xFF表示无效路径
    U8 start_hour ;    // 策略开始时间
    U8 start_min ;
    U8 end_hour ;      // 策略结束时间
    U8 end_min ;

    CRUISE_STRATEGY(){
		memset( this, 0x00, sizeof(CRUISE_STRATEGY) );
	}
};

struct ROUTE_NVR
{
    U8 stay_duration;   // 停留时间
    U8 speed;           // 云台运转速度
    U8 cruise[14];      // 巡航线路

    ROUTE_NVR(){
		memset( this, 0x00, sizeof(ROUTE_NVR) );
	}
};

#define CH_DOME_SPEED  4
//云台参数设定
struct NET_DLNVR_DOME_CONFIG
{
    SERIAL_CONFIG_NVR serial_cfg;
        /**************************************************************************
        云台控制字节，各比特定义如下：
        7   8   6   5   4   3   2   1   0
                    A   A   B   C   D   E
        A 云台速度 0～2
        B 连续转动 0-否， 1-是， 下同
        C 垂直反向
        D 水平反向
        E 是否巡航
        **************************************************************************/
    U8  dome_config;
    U8  type;         //云台类型
                          //和我们以前的一样
    U8  addr;         //云台地址 范围:0~255
    char preset_name[MAX_PRESET_NUM][32];
    ROUTE_NVR route[MAX_ROUTE_NUM];  // 巡航路径设置
    CRUISE_STRATEGY strategy[4];      //
    U8    is_ext_ptz; //0 : 不是  1； 是

     NET_DLNVR_DOME_CONFIG(){
		memset( this, 0x00, sizeof(NET_DLNVR_DOME_CONFIG) );
	}
};

/******************报警输入设置************************************************/
struct LINK_PTZ
{
    U8  type ;    //联动类型, factory value : 0
                      // 0 : call preset
                      // 1 : call cruise
                      // 2 : call auto cruise
    U8   no;      // 联动的预制点号或者巡航路径factory value : 0
    LINK_PTZ(){
		memset( this, 0x00, sizeof(LINK_PTZ) );
	}
};

/* IPCAM告警设置 */
struct NET_DLNVR_ALARM_PARAM
{
    U32         alm_rec_time ;
    U32         pre_rec_time ;
    ALM_STAGE   stage[4] ;
    U8  ararm_enable;        /*0:不允许1：允许*/
    U8 alarm_beep;         /*告警时是否蜂鸣 */
    U16 monitor_spot;       /*报警切换（0-32）0：不切换*/
    U8 alarm_out[4];         /*报警输出 0:否 1:是*/
    U16 alarm_time;          /*报警时间*/
    U8 alarm_channel[4];
    U8 enable_link_ptz; //2010-08-05 add
    LINK_PTZ ptz[SYS_RVS_MAX];    //2010-08-05 add

    NET_DLNVR_ALARM_PARAM(){
		memset( this, 0x00, sizeof(NET_DLNVR_ALARM_PARAM) );
	}
};


struct NET_DLNVR_SYSTEM_PARA_SET
{
  //  int run;    //2009-02-04,add，暂不使用
    U32 rec;                /*1:为录像，0：停止录像。主机自己使用的标识，后台无需设置*/
    U32 para;             //   后台用不到；
    U8 harddisk_alam;
    NET_DLNVR_SYSTEM_SETUP system_setup;
    NET_DLNVR_RECORDING_SETUP rec_setup;
    NET_DLNVR_EVENT_SETUP event_setup[12]; //event_setup[0]表示IPCamera报警， event_setup[1]-event_setup[12] 对应表示NVR的12个告警输入报警
    NET_DLNVR_DISPLAY_SETUP display_setup;
    NET_DLNVR_COMMUNICATION_SETUP communication;
    NET_DLNVR_USER_MANAGE_SETUP user_manage_setup;
    NET_DLNVR_SEARCH_NET_SETUP search_net_setup;  //2009-04-08, add
    NET_DLNVR_SYS_RVS rvs_data[SYS_RVS_MAX];
    NET_DLNVR_ALARM_PARAM alarm_para[SYS_RVS_MAX];

    NET_DLNVR_ALARM_PARAM motion_para[SYS_RVS_MAX];    //2010-08-05, add
    NET_DLNVR_DOME_CONFIG yun[SYS_RVS_MAX];       //2009-08-05, add

    NET_DLNVR_SYSTEM_PARA_SET(){
		memset( this, 0x00, sizeof(NET_DLNVR_SYSTEM_PARA_SET) );
	}
};
//NVR机型主寄结构， end----------------------------------------------------

//DM60机型主板参数-------------------------------------------------------------
typedef struct 
{
	WORD RevisePara100;	//测温修正系数*100
	WORD ReviseTemp100;	//测温修正温度*100
	WORD AimDistance;	//厘米为单位
	WORD AimHimidity;	//湿度
	WORD SurroundTemp;	//环境温度
	WORD MeasureTempClass;	//镜头选择即测温档位
	WORD AreaType;	//区域测温类型，最高最低平均
	WORD AlarmEnable;	//报警使能 1-On 0_OFf
	WORD AlarmTemp100;	//报警温度
	WORD AlarmColor;	//报警色
	WORD LensId;	//镜头选择即测温档位，现在含义同MeasureTempClass
	WORD AutoSaveTime;	//自动保存间隔时间，先暂时没用
	WORD AutoZeroTime;	//自动调零时间间隔
	WORD RefeTempType;	//参考温度类型0无参考测温，…
	WORD RefeTempValue;	//参考温度值，当选择参考类型为参考时用到
	WORD ColorRuleId;	//色标选择，与SCR功能同，但该项值设置色标，但不改变色标的反相或顺相
	WORD UpColorId;	//上限色标
	WORD DownColorId;	//下限色标
	WORD IsoThermColorId;	//等温色标
	WORD IsoThermTemp100;	//等温温度
	WORD IsoThermHight100;	//等温高度
	WORD ShowColorRule;	//是否显示色标
	WORD ShowData;	//是否显示数据
	WORD AvgImageCount;	//图像平均场数，现在没用
	WORD ShowAreaCursor;	//显示区域光标使能
	WORD LanguageType;	//设置语言类型
	WORD StorageType;	//存储类型.0:NandFlash；1:CF卡
	WORD VideoOutType;	//视频输出格式0:PAL;1:
	WORD LaserEnable;	//激光开启控制
	DWORD MeasureTempUnit;	//温度单位：0:℃;1℉;2K;
	WORD TransMenu;	//菜单是否透明
	WORD CloseLedTime;	//关闭屏幕时间
	WORD ClosePowerTime;	//关闭电源时间
	WORD TempDisplay;	//温度是否显示Yes/No
	WORD SteerStartPos;	//舵机起始位置,是不是应该用坐标表示？
	WORD SteerEndPos;	//舵机结束位置，是不是应该用坐标表示？
	WORD DimFiterLevel;	//衰减片值
	WORD ImageMode;	//图像模式 0 -手动 1-自动
}SYSTEMCFG_DM60,  *LPSYSTEMCFG_DM60 ;


typedef struct 
{
	int index;	//0～3
	int Enable;	//1-On; 0-Off
	int Mode;	//0-High; 1-Low; 2-Avg
	int StartX;	//目标起点坐标
	int StartY;	//目标起点坐标
	int F;	//辐射率*100
	int Value;//	温度值*100(无效)
}DMMEASURE_POINT, *LPDMMEASURE_POINT;

typedef struct 
{
	int Enable;	//1-On; 0-Off
	int LineType;	//0-Hori; 1-Vert; 2-Slash
	int Mode;	//0-High; 1-Low; 2-Avg
	int StartX;	//目标起点坐标
	int StartY;	//目标起点坐标
	int EndX;	//目标终点坐标
	int EndY;	//目标终点坐标
	int AnchorX;	//测温线中的测温点
	int AnchorY;	//测温线中的测温点
	int F;	//辐射率*100
	int Value;//	温度值*100(无效)
}DMMEASURE_LINE, *LPDMMEASURE_LINE;

typedef struct 
{
	int index;	//0～3
	int Enable;	//1-On; 0-Off
	int Mode;	//0-High; 1-Low; 2-Avg
	int StartX;	//目标起点坐标
	int StartY;	//目标起点坐标
	int EndX;	//目标终点坐标
	int EndY;	//目标终点坐标
	int F;	//辐射率*100
	int Value;//	温度值*100(无效)
}DMMEASURE_AREA, *LPDMMEASURE_AREA;

//上传实时测温信息
typedef struct
{
	DWORD dwTempType;//0-Point,1-Line,2-Area
	DWORD Index;//序号
	DWORD Gray;//灰度
	DWORD Temp;//温度
}NET_DLDVR_DMTEMP,*LPNET_DLDVR_DMTEMP;

//上传实时测温信息
typedef struct
{
	DWORD dwTempType;//0-Point,1-Line,2-Area
	DWORD Index;//序号
	float Gray;//灰度
	float Temp;//温度
}NET_DLDVR_DMTEMP_EX,*LPNET_DLDVR_DMTEMP_EX;

//上传实时测温信息
typedef struct
{
	DWORD dwTempNum;//温度信息个数
	NET_DLDVR_DMTEMP_EX TempValue[8];
}NET_DLDVR_DMTEMP_INFO,*LPNET_DLDVR_DMTEMP_INFO;


//DM60机型温度告警类型-----------------------------------------
#define TEMP_ALARM_SYSTEM	0	//系统告警
#define TEMP_ALARM_POINT	1	//测温点告警
#define TEMP_ALARM_LINE		2	//测温点告警
#define TEMP_ALARM_AREA		3	//测温点告警

//在告警类型为system时的告警状态
#define TEMP_ALARM_SYSTEM_OK				0	//系统OK
#define TEMP_ALARM_SYSTEM_SDRAM				1	//SDRAM存取失败
#define TEMP_ALARM_SYSTEM_FLASH				2	//Flash存取失败， 
#define TEMP_ALARM_SYSTEM_VRAM				3	//视频芯片设置失败， 
#define TEMP_ALARM_SYSTEM_INITSOFTWARE		4	//软件初始化失败，
#define TEMP_ALARM_SYSTEM_INITFPGA			5	//FPGA初始化失败， 
#define TEMP_ALARM_SYSTEM_DECOMPRESSION		6	//软解压失败， 
#define TEMP_ALARM_SYSTEM_EXCEPTIONAL		7	//软件异常， 
#define TEMP_ALARM_SYSTEM_SHUTDOWN			8	//机器正常关机， 
#define TEMP_ALARM_SYSTEM_TEMPEXCEPTIONAL	9	//机器内部温度超温关机，
#define TEMP_ALARM_SYSTEM_AREATEMPALARM		10	//区域测温高温或低温报警。

//在告警类型不是system时的告警状态
#define TEMP_ALARM_OK		0	//没有报警
#define TEMP_ALARM_HIGHER	1	//高温报警
#define TEMP_ALARM_LOWER	2	//低温报警

//DM60前端仪器状态操作命令
#define  NET_DVR_SENSORPOWER_ON		0 //打开仪器前端
#define  NET_DVR_SENSORPOWER_OFF	1 //关闭仪器前端	
#define  NET_DVR_SENSORPOWER_CHG	2 //改变仪器前端状态

//上传温度告警信息
typedef struct
{
	DWORD dwAlarmType;//0-系统，1-Point, 2-Line, 3-Area
	DWORD Index;//序号（dwAlarmType为0时该序号值无效）
	DWORD Status;//状态
	DWORD AlarmTemp;//报警温度
	DWORD ReferenceTemp;//报警参考温度
}NET_DLDVR_DMTEMP_ALARM,*LPNET_DLDVR_DMTEMP_ALARM;

//DM60智能分析告警的告警类型
#define ANALYS_ALARM_WIRE	0	//拌线报警
#define ANALYS_ALARM_AREA	1	//禁区报警

typedef struct
{
	DWORD dwAlarmType;//0-拌线, 1-禁区
	DWORD Index;//序号
	DWORD num;//报警目标个数
}NET_DLDVR_DMANALYS_ALARM, *LPNET_DLDVR_DMANALYS_ALARM;

//2009-02-27 added 集中管理软件配置的设备信息
struct NET_DLSERVER_DEVICEINFO
{
    char		 DeviceID[DEVICE_ID_LENGTH];        //DVR标识ID
    char         Name[80];             //视频源名称
    DWORD        AddrType;             //1为静态地址或域名 2为系统根据ID自动解析
    char         IPAddr[32];           //IP地址
    WORD         TCPPort;              //TCP连接端口
    DWORD        ChannleNum;           // 通道数量
    char         userName[32];         //20080109 08
    char         ManagePassWord[12];   // 管理员连接密码	
	
    char		 ParentID[DEVICE_ID_LENGTH];     //父区域的ID
    char         ZoneName[32];         //DVR所属的区域信息
};

typedef struct _ST_FUNC_PROPERTY
{
	U8				mainType; 	//主类型：0-接口 1-配置 2-功能
    U16				subType;		//根据主类型不同来具体定义
}NET_DLFUNC_PROPERTY,*LPNET_DLFUNC_PROPERTY;

typedef struct
{
	int	realHandle;	//监视句柄
	U8  mainStream;		//0-主码流 1-子码流
	U8	audioPresent;	//0-关闭音频 1-开启音频(为了节省码流传输，可以选择是否传音频)
	U8	sendVideo;	//0-只发帧头数据 1-恢复视频正常发送
}NET_DLDVR_STREAMSTATE,*LPNET_DVR_STREAMSTATE;



#pragma pack()

/************************************************************************/
//   回调函数定义                                                                  
/************************************************************************/
//2009-02-24 added 解码后的视频帧回调函数定义
//2010-08-26 dwDataType 修改为 dwFrameRate 表示帧率(1---25)
typedef VOID(CALLBACK *fYUVDataCallBack) (LONG lHandle, DWORD /*dwDataType*/dwFrameRate, BYTE *pBuffer, 
										  DWORD nWidth, DWORD nHeight, int err, DWORD dwUser);

//视频流回调函数	
//lRealHandle,NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()的返回值 
//dwDataType, 码流类型 0xEE：系统头数据   0xEF：流数据
//pBuffer,  存放监视数据的缓冲区 
//dwBufSize，pBuffer的大小 
//FrameType, 0x00-视频I帧  0x01-视频P帧 0x02-视频B帧 0x03-音频帧 0x09-AAC格式音频帧 0x81-JPEG。该值只在dwDataType为0xEF时有效
//FrameRate,实际帧率，即每秒多少帧(1---25)。该值只在dwDataType为0xEF时有效
//nWidth，图像宽度。该值只在dwDataType为0xEF时有效
//nHeight，图像高度。该值只在dwDataType为0xEF时有效
//time，帧时间。该值只在dwDataType为0xEF时有效
//err，错误码 
//dwUser，用户传入的数据 
typedef VOID(CALLBACK *fRealDataCallBack_EX)(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize, 
											 INT FrameType, INT FrameRate, DWORD nWidth, DWORD nHeight, 
											 NET_DLDVR_TIME* time, int err, DWORD dwUser);

extern "C"
{
/* 	功能说明:初始化SDK，调用其他SDK函数前必须先调用该函数
 *	输入参数: 无 
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: (无)  
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_Init();

/* 	功能说明:释放SDK资源
 *	输入参数: 无 
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: (无)  
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_Cleanup();

/* 	功能说明:设置消息的接收窗口句柄
 *		消息类型：#define WM_DLDVR_NET_EXCEPTION_NOTIFY		(WM_APP + 800)	//监视预览网络出错通知
 *                #define WM_DLDVR_PLAYBACK_OVER_NOTIFY		(WM_APP + 801)	//回放结束通知消息	
 *	              #define WM_DLDVR_TALKER_EXCEPTION_NOTIFY	(WM_APP + 802)	//对讲出错通知
 *				  #define WM_DLDVR_LOGOUT_NOTIFY			(WM_APP + 803)	//用户登出消息,与主机失去连接
 *				  #define WM_DLDVR_RESOLUTION_CHANGE_NOTIFY	(WM_APP + 804)//监视或回放时时分辨率发生改变的通知消息
 *			 	  #define WM_DLDVR_VIDEOFORMAT_CHANGE_NOTIFY	(WM_APP + 805)//监视或回放时时视频格式发生改变的通知消息
 *	输入参数: hWnd，接收消息的窗口句柄 
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: (无)  
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetRecvDVRMessageHWnd(HWND hWnd);//2008-09-01Modify

/* 	功能说明:注册回调函数，接收设备报警消息等（同时回调出设备IP，用以区分不同设备）
 *	输入参数: cbMessCallBack，回调函数
				BOOL(CALLBACK *fMessageCallBack)(LONG lCommand, char *sDVRIP, char *pBuf, DWORD dwBufLen);
				lCommand值：
					#define COMM_ALARM 0x1100 //报警信息 对应结构 NET_DLDVR_ALARMINFO 
					#define COMM_ALARM_EX	 0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
					#define COMM_TRADEINFO   0x1500 //ATMDVR主动上传交易信息 
					#define SERIAL_NUMBER	 0x1600 //DVR上报的序列号
					#define DISKCHANNEL_STATE 0x1700 //主机定时上传硬盘状态、通道录像状态信息 
					#define RVS_INFO		 0x1800 //IPCamera上传的RVS信息
					#define EXCEPTION_NOTIFY 0x1900 //异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息 
					#define DM_TEMP			0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
					#define DMTEMP_ALARM	0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
					#define DM_ANALYS_ALARM		0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
					#define	PLAYBACK_OVER		0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION
					#define RESOLUTION_CHANGE	0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
					#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
				sDVRIP:设备IP地址 
				pBuf:回调出的信息
				dwBufLen：回调出的信息长度
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetDVRMessCallBack(BOOL (CALLBACK *fMessCallBack)(LONG lCommand,CHAR *sDVRIP,CHAR *pBuf,DWORD dwBufLen));

/* 	功能说明:注册回调函数，接收设备报警消息等（同时回调出用户ID，用以区分不同用户）
 *	输入参数: cbMessCallBack，回调函数
				BOOL(CALLBACK *fMessageCallBack)(LONG lCommand, char *sDVRIP, char *pBuf, DWORD dwBufLen);
					lCommand值：
						#define COMM_ALARM 0x1100 //报警信息 对应结构 NET_DLDVR_ALARMINFO 
						#define COMM_ALARM_EX	 0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
						#define COMM_TRADEINFO   0x1500 //ATMDVR主动上传交易信息 
						#define SERIAL_NUMBER	 0x1600 //DVR上报的序列号
						#define DISKCHANNEL_STATE 0x1700 //主机定时上传硬盘状态、通道录像状态信息 
						#define RVS_INFO		 0x1800 //IPCamera上传的RVS信息
						#define EXCEPTION_NOTIFY 0x1900 //异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息 
						#define DM_TEMP			0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
						#define DMTEMP_ALARM	0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
						#define DM_ANALYS_ALARM					0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
						#define	PLAYBACK_OVER		0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION
						#define RESOLUTION_CHANGE	0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
						#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
					lUserID：用户登录ID，即NET_DLDVR_Login的返回值 
					pBuf:回调出的信息
					dwBufLen：回调出的信息长度
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetDVRMessCallBack_EX(BOOL (CALLBACK *fMessCallBack_EX)(LONG lCommand,LONG lUserID,CHAR *pBuf,DWORD dwBufLen));

/* 	功能说明:设置网络连接超时时间和连接尝试次数。
 *	输入参数: dwWaitTime，超时时间，单位毫秒，取值需大于300。实际最大超时时间为connect超时时间，超出部分无效  
			  dwTryTimes，连接尝试次数（该参数保留，目前没有使用）
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetConnectTime(DWORD dwWaitTime,DWORD dwTryTimes);

/* 	功能说明:获取SDK的版本信息（保留接口）
 *	输入参数:无
 *	输出参数: 无
 *	函数返回: SDK版本信息
 *	说明: 目前的SDK版本无法通过该接口获取
 */
NET_DLDVR_API DWORD			STDCALL NET_DLDVR_GetSDKVersion();

/* 	功能说明:启动监听，接收设备主动上传的报警等信息。
 *	输入参数:sLocalIP，PC 机的IP 地址，保留参数，目前该参数无效
			 wLocalPort，PC本地监听端口号，由用户设置
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StartListen(CHAR *sLocalIP,WORD wLocalPort);

/* 	功能说明:停止监听
 *	输入参数:无
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopListen();

/* 	功能说明:用户登录到DVR
 *	输入参数:sDVRIP，DVRIP
			 wDVRPort，DVR端口
			 sUserName， 用户名
			 sPassword，密码
 *	输出参数: lpDeviceInfo，DVR的设备信息
 *	函数返回: 用户ID，该值具有唯一性，后续对设备的操作都需要通过此ID实现。若登录失败则返回负数
 *	说明: 该接口不适用于DM60接口
 */
NET_DLDVR_API LOGIN_HANDLE	STDCALL NET_DLDVR_Login(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,LPNET_DLDVR_DEVICEINFO lpDeviceInfo);

/* 	功能说明:用户登出DVR
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_Logout(LONG lUserID);

/* 	功能说明:用于获取最后一次操作的错误码
 *	输入参数:无
 *	输出参数:无
 *	函数返回:错误码
 *	说明: 
 */
NET_DLDVR_API DWORD			STDCALL NET_DLDVR_GetLastError();

/* 	功能说明:开始实时监视
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lpClientInfo，预览参数 
 *	输出参数:无
 *	函数返回:监视句柄，负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_RealPlay(LONG lUserID,LPNET_DLDVR_CLIENTINFO lpClientInfo);

/* 	功能说明:关闭实时监视
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopRealPlay(LONG lRealHandle);

/* 	功能说明:设置预览视频显示参数
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwBrightValue，亮度，取值范围0-99 
			 dwContrastValue，对比度，取值范围0-99 
			 dwSaturationValue，饱和度，取值范围0-99 
			 dwHueValue，色度，取值范围0-99 
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_ClientSetVideoEffect(LONG lRealHandle,DWORD dwBrightValue,DWORD dwContrastValue, DWORD dwSaturationValue,DWORD dwHueValue);

/* 	功能说明:获取预览视频显示参数
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:pBrightValue，亮度，取值范围0-99 
			 pContrastValue，对比度，取值范围0-99 
			 pSaturationValue，饱和度，取值范围0-99 
			 pHueValue，色度，取值范围0-99 
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_ClientGetVideoEffect(LONG lRealHandle,DWORD *pBrightValue,DWORD *pContrastValue, DWORD *pSaturationValue,DWORD *pHueValue);

//接口已经实现
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_RigisterDrawFun(LONG lRealHandle,VOID (CALLBACK* fDrawFun)(LONG lRealHandle,HDC hDc, INT picWidth, INT picHeight, DWORD dwUser),DWORD dwUser);
/*保留接口*/
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetPlayerBufNumber(LONG lRealHandle,DWORD dwBufNum);

/* 	功能说明:开启声音
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_OpenSoundShare(LONG lRealHandle);

/* 	功能说明:关闭声音
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_CloseSoundShare(LONG lRealHandle);

/* 	功能说明:获取当前通道的音量大小
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:无
 *	函数返回:音量值（0-100），负数表示失败
 *	说明: 
 */
NET_DLDVR_API INT			STDCALL NET_DLDVR_GetVolume(LONG lRealHandle);	//2008-10-06

/* 	功能说明:设置当前通道的音量大小
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 wVolume，要设置的音量值，取值范围0-100
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetVolume(LONG lRealHandle,WORD wVolume);	//2008-10-06

/* 	功能说明:将监视数据保存到指定的文件中
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 sFileName，要保存的文件名
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SaveRealData(LONG lRealHandle,CHAR *sFileName);

/* 	功能说明:停止保存监视数据
 *	输入参数:lRealHandle，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopSaveRealData(LONG lRealHandle);

/* 	功能说明:设置获取实时回放视频数据的回调函数
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 realDataCallBack，获取监视数据的回调函数 
			 VOID realDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE pBuffer, DWORD dwBufSize, int err, DWORD dwUser);
				lRealHandle,NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()的返回值 
				dwDataType, 码流类型 0xEE：系统头数据  0xEF：流数据  0x1：标准视频流  0x2：标准音频流 
				pBuffer,  存放监视数据的缓冲区 
				dwBufSize，pBuffer的大小 
				err，错误码 
				dwUser，用户传入的数据 
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetRealDataCallBack(LONG lRealHandle,VOID(CALLBACK *fRealDataCallBack) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize, int err, DWORD dwUser),DWORD dwUser);

/* 	功能说明:将正在播放的监视画面抓图保存到bmp文件中
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 sPicFileName，要保存的图片名，传入的可以是文件路径（此时NetSDK会根据当前时间自动生成一个bmp文件），也可以是文件路径+文件名
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_CapturePicture(LONG lRealHandle,CHAR *sPicFileName);

/* 	功能说明:生成关键帧
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_MakeKeyFrame(LONG lUserID, LONG lChannel);

/* 	功能说明:云台控制，需要先进行监视
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZCommand，命令值
				#define LIGHT_PWRON						3	// 接通灯光电源 
				#define WIPER_PWRON						2	// 接通雨刷开关 
				#define FAN_PWRON						4	// 接通风扇开关 
				#define HEATER_PWRON					5	// 接通加热器开关 
				#define AUX_PWRON						6	// 接通辅助设备开关 

				#define ZOOM_IN							11	// 焦距以速度SS变大(倍率变大) 
				#define ZOOM_OUT						12	// 焦距以速度SS变小(倍率变小) 
				#define FOCUS_NEAR						13	// 焦点以速度SS前调 
				#define FOCUS_FAR						14	// 焦点以速度SS后调 
				#define IRIS_OPEN						15	// 光圈以速度SS扩大 
				#define IRIS_CLOSE						16	// 光圈以速度SS缩小 
				#define TILT_UP							21	// 云台以SS的速度上仰 
				#define TILT_DOWN						22	// 云台以SS的速度下俯 
				#define PAN_LEFT						23	// 云台以SS的速度左转 
				#define PAN_RIGHT						24	// 云台以SS的速度右转 
				#define PAN_AUTO						29	// 云台以SS的速度左右自动扫描 
			 dwAction，云台停止动作或开始动作：1－开始；0－停止
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZControl(LONG lRealHandle,DWORD dwPTZCommand,DWORD dwAction);

/* 	功能说明:云台控制，无需打开监视
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号
			 dwPTZCommand，命令值
					#define LIGHT_PWRON			3	// 接通灯光电源 
					#define WIPER_PWRON			2	// 接通雨刷开关 
					#define FAN_PWRON			4	// 接通风扇开关 
					#define HEATER_PWRON		5	// 接通加热器开关 
					#define AUX_PWRON			6	// 接通辅助设备开关 
					#define ZOOM_IN				11	// 焦距以速度SS变大(倍率变大) 
					#define ZOOM_OUT			12	// 焦距以速度SS变小(倍率变小) 
					#define FOCUS_NEAR			13	// 焦点以速度SS前调 
					#define FOCUS_FAR			14	// 焦点以速度SS后调 
					#define IRIS_OPEN			15	// 光圈以速度SS扩大 
					#define IRIS_CLOSE			16	// 光圈以速度SS缩小 
					#define TILT_UP				21	// 云台以SS的速度上仰 
					#define TILT_DOWN			22	// 云台以SS的速度下俯 
					#define PAN_LEFT			23	// 云台以SS的速度左转 
					#define PAN_RIGHT			24	// 云台以SS的速度右转 
					#define PAN_AUTO			29	// 云台以SS的速度左右自动扫描 	
			 dwAction，云台停止动作或开始动作：1－开始；0－停止
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_PTZControl_Other(LONG lUserID,LONG lChannel,DWORD dwPTZCommand,DWORD dwAction);

/* 	功能说明:透明云台控制，需要先打开监视
 *	输入参数:lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 pPTZCodeBuf，云台控制命令
			 dwBufSize，命令长度
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型。
 *  注：目前代码中还是将lRealHandle当做是登陆句柄来进行处理（2010-04-01）
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_TransPTZ(LONG lRealHandle,CHAR *pPTZCodeBuf,DWORD dwBufSize);

/* 	功能说明:透明云台控制，无需打开监视
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号
			 pPTZCodeBuf，云台控制命令
			 dwBufSize，命令长度
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_TransPTZ_Other(LONG lUserID,LONG lChannel,CHAR *pPTZCodeBuf,DWORD dwBufSize);

/* 	功能说明:云台预制位控制，需要先打开监视
 *	输入参数:lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZPresetCmd，云台预置点命令
					#define SET_PRESET		1	// 设置预置点 
					#define CLE_PRESET		2	// 清除预置点 
					#define GOTO_PRESET		3	// 转到预置点
			 dwPresetIndex，预置点的序号，最多支持128个预置点
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于600机型、IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZPreset(LONG lRealHandle,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);

/* 	功能说明:云台预制位控制，无需打开监视
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始
			 dwPTZPresetCmd，云台预置点命令
					#define SET_PRESET			1	// 设置预置点 
					#define CLE_PRESET			2	// 清除预置点 
					#define GOTO_PRESET			3	// 转到预置点
			 dwPresetIndex，预置点的序号，最多支持128个预置点
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明:该接口不适用于IPCamera和NVR机型 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZPreset_Other(LONG lUserID,LONG lChannel,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);

/* 	功能说明:巡航控制，无需打开监视
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，与巡航的云台相联的通道号，从0开始
			 PTZCruiseCmd:云台控制命令 
					#define FILL_PRE_SEQ	30 // 将预置点加入巡航序列 
					#define SET_SEQ_DWELL	31 // 设置巡航点停顿时间 
					#define SET_SEQ_SPEED	32 // 设置巡航速度 
					#define CLE_PRE_SEQ		33 // 将预置点从巡航序列中删除 
					#define RUN_SEQ			37	// 开始巡航 
					#define STOP_SEQ		38	// 停止巡航 
			byCruiseRoute，巡航路径，最多支持8条路径 
			byCruisePoint，最多支持14个点
			wInput，不同巡航命令时的值不同，预置点(最大128)、时间(最大255)、速度(最大15) 
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZCruise_Other(LONG lUserID,LONG lChannel,DWORD dwPTZCruiseCmd,BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput);

//带速度的云台控制,dwSpeed值:1-3
/* 	功能说明:带速度的云台控制，需要先打开监视
 *	输入参数:lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZCommand，命令值
					#define LIGHT_PWRON			3	// 接通灯光电源 
					#define WIPER_PWRON			2	// 接通雨刷开关 
					#define FAN_PWRON			4	// 接通风扇开关 
					#define HEATER_PWRON		5	// 接通加热器开关 
					#define AUX_PWRON			6	// 接通辅助设备开关 
					#define ZOOM_IN				11	// 焦距以速度SS变大(倍率变大) 
					#define ZOOM_OUT			12	// 焦距以速度SS变小(倍率变小) 
					#define FOCUS_NEAR			13	// 焦点以速度SS前调 
					#define FOCUS_FAR			14	// 焦点以速度SS后调 
					#define IRIS_OPEN			15	// 光圈以速度SS扩大 
					#define IRIS_CLOSE			16	// 光圈以速度SS缩小 
					#define TILT_UP				21	// 云台以SS的速度上仰 
					#define TILT_DOWN			22	// 云台以SS的速度下俯 
					#define PAN_LEFT			23	// 云台以SS的速度左转 
					#define PAN_RIGHT			24	// 云台以SS的速度右转 
					#define PAN_AUTO			29	// 云台以SS的速度左右自动扫描 	
			 dwAction，云台停止动作或开始动作：1－开始；0－停止
			 dwSpeed，云台速度，取值范围1-3
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZControlWithSpeed(LONG lRealHandle, DWORD dwPTZCommand, DWORD dwAction, DWORD dwSpeed);

/* 	功能说明:带速度的云台控制，无需打开监视
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始
			 dwPTZCommand，命令值
					#define LIGHT_PWRON			3	// 接通灯光电源 
					#define WIPER_PWRON			2	// 接通雨刷开关 
					#define FAN_PWRON			4	// 接通风扇开关 
					#define HEATER_PWRON		5	// 接通加热器开关 
					#define AUX_PWRON			6	// 接通辅助设备开关 
	
					#define ZOOM_IN				11	// 焦距以速度SS变大(倍率变大) 
					#define ZOOM_OUT			12	// 焦距以速度SS变小(倍率变小) 
					#define FOCUS_NEAR			13	// 焦点以速度SS前调 
					#define FOCUS_FAR			14	// 焦点以速度SS后调 
					#define IRIS_OPEN			15	// 光圈以速度SS扩大 
					#define IRIS_CLOSE			16	// 光圈以速度SS缩小 
					#define TILT_UP				21	// 云台以SS的速度上仰 
					#define TILT_DOWN			22	// 云台以SS的速度下俯 
					#define PAN_LEFT			23	// 云台以SS的速度左转 
					#define PAN_RIGHT			24	// 云台以SS的速度右转 
					#define PAN_AUTO			29	// 云台以SS的速度左右自动扫描 	
			 dwAction，云台停止动作或开始动作：1－开始；0－停止
			 dwSpeed，云台速度，取值范围1-3
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PTZControlWithSpeed_Other(LONG lUserID, LONG lChannel, DWORD dwPTZCommand, DWORD dwAction, DWORD dwSpeed);

//远程文件回放
/* 	功能说明:开始查找录像文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号,按位表示
			 dwFileType，文件类型  
					0xff-全部；0-定时录像；1-移动侦测；2-报警触发(此时查询到的录像会包括普通报警录像和智能分析报警录像)；3-命令触发；4-手动录像；
			 lpStartTime，开始时间
			 lpStopTime，结束时间
 *	输出参数:无
 *	函数返回:查找句柄，作为NET_DVR_FindClose()等函数的参数；返回负数表示失败
 *	说明: 不适用于IPCamera机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindFile(LONG lUserID,LONG lChannel,DWORD dwFileType, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);

/* 	功能说明:查找下一条录像文件
 *	输入参数:lFindHandle，查找句柄，即NET_DLDVR_FindFile()函数的返回值
 *	输出参数:lpFindData，查询到的文件信息
 *	函数返回:0：成功；返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindNextFile(LONG lFindHandle,LPNET_DLDVR_FIND_DATA lpFindData);

/* 	功能说明:关闭文件查找。
 *	输入参数:lFindHandle，查找句柄，即NET_DLDVR_FindFile()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 在查找结束或中止查找时均须调用此接口
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_FindClose(LONG lFindHandle);

/* 	功能说明:按录像名称进行远程回放
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 sPlayBackFileName，需要进行回放的录像文件名
			 hWnd，进行回放的窗口句柄
 *	输出参数:无
 *	函数返回: 播放句柄，作为NET_DLDVR_StopPlayBack等函数的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_PlayBackByName(LONG lUserID,CHAR *sPlayBackFileName, HWND hWnd);

/* 	功能说明:按时间段进行远程回放
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，1,3,0,36及以后的版本从0开始， 1,3,0,35及以前的版本从1开始（2010-09-15 modifty）
			 lpStartTime，开始时间
			 lpStopTime，结束时间
			 hWnd，进行回放的窗口句柄
 *	输出参数:无
 *	函数返回: 播放句柄，作为NET_DLDVR_StopPlayBack等函数的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_PlayBackByTime(LONG lUserID,LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, HWND hWnd);

/* 	功能说明:回放控制
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
			 dwControlCode，控制命令
				#define NET_DLDVR_PLAYSTOP					2//停止播放
				#define NET_DLDVR_PLAYPAUSE					3//暂停播放
				#define NET_DLDVR_PLAYRESTART				4//恢复播放
				#define NET_DLDVR_PLAYFAST					5//快放
				#define NET_DLDVR_PLAYSLOW					6//慢放
				#define NET_DLDVR_PLAYNORMAL				7//正常速度
				#define NET_DLDVR_PLAYFRAME					8//单帧放
				#define NET_DLDVR_PLAYSTARTAUDIO			9//打开声音
				#define NET_DLDVR_PLAYSTOPAUDIO				10//关闭声音
				#define NET_DLDVR_PLAYAUDIOVOLUME			11//调节音量
				#define NET_DLDVR_PLAYSETPOS				12//改变文件回放的进度
				#define NET_DLDVR_PLAYGETPOS				13//获取文件回放的进度
				#define NET_DLDVR_PLAYGETTIME				14//获取当前已经播放的时间
				#define NET_DLDVR_GETPLAYAUDIOVOLUME		21//获得当前的音量大小
				#define NET_DLDVR_PLAY_DOUBLESPEED			22	//2倍数
				#define NET_DLDVR_PLAY_HALFSPEED			23	//1/2速度
				#define NET_DLDVR_PLAY_SETPOS_BYTIME		24 //回放时按时间百分比跳转到某位置
				#define NET_DLDVR_PLAY_OCTUPLESPEED			25	//8倍数,2010-07-06 add
				#define NET_DLDVR_PLAY_SIXTEENSPEED			26	//16倍数,2010-07-06 add
				#define NET_DLDVR_PLAY_32SPEED				27	//32倍数,2010-07-06 add
			 dwInValue，设置的参数，如设置文件回放的进度(命令值NET_DLDVR_PLAYSETPOS)时，此参数表示进度值
 *	输出参数: lpOutValue，获取的参数，如获取音量大小（命令值NET_DLDVR_GETPLAYAUDIOVOLUME ），此参数就是当前的音量值
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PlayBackControl(LONG lPlayHandle,DWORD dwControlCode,DWORD dwInValue,DWORD *lpOutValue);

/* 	功能说明:停止远程回放
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopPlayBack(LONG lPlayHandle);


/* 	功能说明:保存远程回放的数据到指定文件
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
			 sFileName，文件名
 *	输出参数:无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PlayBackSaveData(LONG lPlayHandle,CHAR *sFileName);

/* 	功能说明:停止保存远程回放的数据
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopPlayBackSave(LONG lPlayHandle);

/* 	功能说明:获取录像回放时显示的OSD时间
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
 *	输出参数:lpOsdTime，OSD时间
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetPlayBackOsdTime(LONG lPlayHandle, LPNET_DLDVR_TIME lpOsdTime);

/* 	功能说明:将正在远程回放的画面抓图保存到bmp文件中
 *	输入参数:lPlayHandle，回放句柄，即NET_DLDVR_PlayBackByName()或NET_DLDVR_PlayBackByTime()函数的返回值
			 sFileName，要保存的图片名，传入的可以是文件路径（此时NetSDK会根据当前时间自动生成一个bmp文件），也可以是文件路径+文件名
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_PlayBackCaptureFile(LONG lPlayHandle,CHAR *sFileName);

/* 	功能说明:根据录像文件的名称下载文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 sDVRFileName，要下载的录像文件名
			 sSavedFileName，希望保存到本机的录像文件名
 *	输出参数:无
 *	函数返回:下载句柄，作为NET_DLDVR_StopGetFile()和NET_DLDVR_GetDownloadPos()的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_GetFileByName(LONG lUserID,CHAR *sDVRFileName,CHAR *sSavedFileName);

/* 	功能说明:根据时间段下载文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始(2010-09-17之前的版本通道号从1开始)
			 lpStartTime，开始时间
			 lpStopTime，结束时间
			 sSavedFileName，希望保存到本机的录像文件名
 *	输出参数:无
 *	函数返回:下载句柄，作为NET_DLDVR_StopGetFile()和NET_DLDVR_GetDownloadPos()的参数；返回负数表示失败
 *	说明: 所有包含在指定时间段内的文件将被保存到同一个文件中。该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_GetFileByTime(LONG lUserID,LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, CHAR *sSavedFileName);

/* 	功能说明:停止下载
 *	输入参数:lFileHandle，下载句柄，即NET_DLDVR_GetFileByName()或NET_DLDVR_GetFileByTime()函数的返回值		 
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopGetFile(LONG lFileHandle);

/* 	功能说明:获取下载进度
 *	输入参数:lFileHandle，下载句柄，即NET_DLDVR_GetFileByName()或NET_DLDVR_GetFileByTime()函数的返回值		 
 *	输出参数:无
 *	函数返回:进度值（0-100），返回负数表示获取失败
 *	说明: 
 */
NET_DLDVR_API INT			STDCALL NET_DLDVR_GetDownloadPos(LONG lFileHandle);

/* 	功能说明:播放本地文件
 *	输入参数:fileName，要播放的录像文件名
             hWnd，播放窗口句柄
 *	输出参数:无
 *	函数返回:播放句柄，作为NET_DLDVR_StopPlayFiles()、NET_DLDVR_PlayBackControl()等函数的参数
 *	说明: 
 */
NET_DLDVR_API INT			STDCALL NET_DLDVR_PlayFiles(CHAR* fileName,HWND hWnd);

/* 	功能说明:停止播放本地文件
 *	输入参数:lFileHandle，播放句柄，即NET_DLDVR_PlayFiles()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopPlayFiles(LONG lFileHandle);

/* 	功能说明:恢复出厂设置
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_RestoreConfig(LONG lUserID);

/* 	功能说明:保存参数到Flash中
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SaveConfig(LONG lUserID);

/* 	功能说明:远程重启DVR
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_RebootDVR(LONG lUserID);

/* 	功能说明:远程关闭DVR
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_ShutDownDVR(LONG lUserID);

//升级
/* 	功能说明:远程更新主机映像
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 sFileName，要更新的映像名称
 *	输出参数:无
 *	函数返回:更新ID，作为NET_DLDVR_GetUpgradeState()等函数的输入参数；返回负数表示失败
 *	说明:
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_Upgrade(LONG lUserID,CHAR *sFileName);

/* 	功能说明:获取远程更新的状态
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_Upgrade()函数的返回值
 *	输出参数:无
 *	函数返回:状态值
				#define NET_DLDVR_UPGRADESUCESS			1000	//升级成功
				#define NET_DLDVR_UPGRADEING			1010	//正在升级
 *	说明: 该接口不适用于IPCamare机型
 */
NET_DLDVR_API INT				STDCALL NET_DLDVR_GetUpgradeState(LONG lUpgradeHandle);

/* 	功能说明:获取远程更新的状态及进度
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_Upgrade()函数的返回值
 *	输出参数:nPostion，当前更新的进度，0-100
 *	函数返回:状态值
				#define NET_DLDVR_UPGRADEING_DOWNLOAD		1011	//正在升级-下载镜像
				#define NET_DLDVR_UPGRADEING_APPLYING		1012	//正在升级-应用镜像
 *	说明: 该接口不适用于IPCamare机型
 */
NET_DLDVR_API INT				STDCALL NET_DLDVR_GetUpgradeSubState(LONG lUpgradeHandle, INT &nPostion);

/* 	功能说明:取消更新
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_Upgrade()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_CloseUpgradeHandle(LONG lUpgradeHandle);


//WEB升级。2007-09-10
/* 	功能说明:远程更新主机的web
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 sFileName，要更新的映像名称
 *	输出参数:无
 *	函数返回:更新ID，作为NET_DLDVR_GetWebUpdateState()等函数的输入参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR接口
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_WebUpdate(LONG lUserID,CHAR *sFileName);
/* 	功能说明:获取web更新的状态
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_WebUpdate()函数的返回值
 *	输出参数:无
 *	函数返回:状态值
				#define NET_DLDVR_UPGRADESUCESS			1000	//升级成功
				#define NET_DLDVR_UPGRADEING				1010	//正在升级
 *	说明:该接口不适用于IPCamera和NVR接口 
 */
NET_DLDVR_API INT				STDCALL NET_DLDVR_GetWebUpdateState(LONG lUpgradeHandle);
/* 	功能说明:获取远程更新web的状态及进度
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_WebUpdate()函数的返回值
 *	输出参数:nPostion，当前更新的进度，0-100
 *	函数返回:状态值
				#define NET_DLDVR_UPGRADEING_DOWNLOAD		1011	//正在升级-下载镜像
				#define NET_DLDVR_UPGRADEING_APPLYING		1012	//正在升级-应用镜像
 *	说明: 该接口不适用于IPCamera和NVR接口
 */
NET_DLDVR_API INT				STDCALL NET_DLDVR_GetWebUpgradeSubState(LONG lUpgradeHandle, INT &nPostion);
/* 	功能说明:取消web更新
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_WebUpdate()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR接口
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_CloseWebUpdateHandle(LONG lUpgradeHandle);

/* 	功能说明:远程格式化硬盘
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lDiskNumber，硬盘号，从0 开始，0xff 表示所有硬盘	 
 *	输出参数:无
 *	函数返回:格式化ID，用作NET_DLDVR_GetFormatProgress()函数的参数
 *	说明: 该接口不适用于IPCamera和NVR接口
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FormatDisk(LONG lUserID,LONG lDiskNumber);
/* 	功能说明:获取格式化硬盘的进度
 *	输入参数:lFormatHandle，格式化句柄，即NET_DLDVR_FormatDisk()函数的返回值 
 *	输出参数:pCurrentFormatDisk,当前正在进行格式化的硬盘的硬盘号
			 pCurrentDiskPos,当前硬盘的格式化进度（0-100）
			 pFormatStatic,当前硬盘的格式化状态
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR接口
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetFormatProgress(LONG lFormatHandle,LONG *pCurrentFormatDisk,LONG *pCurrentDiskPos,LONG *pFormatStatic);

/* 	功能说明:布防，即通知主机上传告警信息
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:告警ID，作为撤防函数NET_DLDVR_CloseAlarmChan()的参数
 *	说明: 
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_SetupAlarmChan(LONG lUserID);
/* 	功能说明:撤防，即通知主机不再上传告警信息
 *	输入参数:lAlarmHandle，告警ID，即NET_DLDVR_SetupAlarmChan()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_CloseAlarmChan(LONG lAlarmHandle);

/* 	功能说明:获取设备告警输出的状态
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:lpAlarmOutState，告警输出状态
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetAlarmOut(LONG lUserID,LPNET_DLDVR_ALARMOUTSTATUS lpAlarmOutState);
/* 	功能说明:设置设备告警输出的状态
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lAlarmOutPort，告警输出端口号，从0开始,0xff表示全部
			 lAlarmOutStatic，告警输出端口的状态
 *	输出参数:lpAlarmOutState，告警输出状态，0-关闭 1-打开
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetAlarmOut(LONG lUserID,LONG lAlarmOutPort,LONG lAlarmOutStatic);

/* 	功能说明:打开语音对讲
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 fVoiceDataCallBack，回调函数
				typedef void(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag,  DWORD dwUser);
					lVoiceComHandle：NET_DLDVR_StartVoiceCom的返回值 
					pRecvDataBuffer：存放音频数据的缓冲区指针 
					dwBufSize：音频数据大小 
					byAudioFlag：音频数据类型：0－本地采集的数据（编码后）；1－设备发送过来的语音数据（编码后） 
					dwUser：用户数据 
 *	输出参数:无
 *	函数返回:对讲ID，作为NET_DLDVR_StopVoiceCom()函数的参数
 *	说明: 
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_StartVoiceCom(LONG lUserID, VOID(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle,CHAR *pRecvDataBuffer,DWORD dwBufSize,BYTE byAudioFlag,DWORD dwUser), DWORD dwUser);

/* 	功能说明:关闭语音对讲
 *	输入参数:lVoiceComHandle，对讲ID，即NET_DLDVR_StartVoiceCom()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopVoiceCom(LONG lVoiceComHandle);

/* 	功能说明:启动语音广播的PC端声音采集
 *	输入参数:无
 *	输出参数:无
 *	函数返回:广播ID，返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API INT			STDCALL NET_DLDVR_ClientAudioStart();
/* 	功能说明:停止语音广播的PC端声音采集
 *	输入参数:lVoiceComHandle，广播ID，即NET_DLDVR_ClientAudioStart()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_ClientAudioStop(INT lVoiceComHandle);
/* 	功能说明:将主机添加到可以接收PC机声音的广播组中
 *	输入参数:TalkHandle，广播ID，即NET_DLDVR_ClientAudioStart()函数的返回值
			 lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:主机设置在广播组中的ID，返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API INT			STDCALL NET_DLDVR_AddDVR(INT TalkHandle, LONG lUserID);	//2009-06-26 add TalkHandle（ NET_DLDVR_ClientAudioStart的返回值）

/* 	功能说明:将指定设备可接收PC机声音的广播组里删除
 *	输入参数:TalkHandle，广播ID，即NET_DLDVR_ClientAudioStart()函数的返回值
			 dvrIndex，主机设置在广播组中的ID，即NET_DLDVR_AddDVR()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_DelDVR(LONG TalkHandle, INT dvrIndex);

/* 	功能说明:启动语音转发，获取编码后的音频数据
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 fVoiceDataCallBack，回调函数
				typedef void(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, DWORD dwUser);
					lVoiceComHandle，NET_DLDVR_StartVoiceCom_MR的返回值 
					pRecvDataBuffer，存放音频数据的缓冲区指针 
					dwBufSize，音频数据大小 
					byAudioFlag，音频数据类型：1－设备发送过来的音频数据 
					dwUser，用户数据 
 *	输出参数:无
 *	函数返回:语音转发ID，作为NET_DLDVR_VoiceComSendData()等函数的参数
 *	说明: 
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_StartVoiceCom_MR(LONG lUserID, VOID(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle,CHAR *pRecvDataBuffer,DWORD dwBufSize,BYTE byAudioFlag,DWORD dwUser), DWORD dwUser);
/* 	功能说明:将编码后的音频数据转发给设备
 *	输入参数:lVoiceComHandle，语音转发ID，即NET_DLDVR_StartVoiceCom_MR()函数的返回值
			 pSendBuf，语音数据
			 dwBufSize， 语音数据大小 
 *	输出参数:无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_VoiceComSendData(LONG lVoiceComHandle,CHAR *pSendBuf,DWORD dwBufSize);
////////////////////////////////////////////////////////////

/* 	功能说明:远程控制本地显示
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lKeyIndex，控制码
 *	输出参数:无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 目前主机键盘上的功能没有完全包含，需要后续调试。该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_ClickKey(LONG lUserID,LONG lKeyIndex);

//远程控制手动录像
/* 	功能说明:远程打开主机录像
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始
			 lRecordType，保留参数
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StartDVRRecord(LONG lUserID,LONG lChannel,LONG lRecordType);
/* 	功能说明:远程关闭主机录像
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lChannel，通道号，从0开始
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopDVRRecord(LONG lUserID,LONG lChannel);

/* 	功能说明:远程获取设备工作状态
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:lpWorkState，设备工作状态
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用IPCamera、NVR机型和hisi3520机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetDVRWorkState(LONG lUserID,LPNET_DLDVR_WORKSTATE lpWorkState);

//日志
/* 	功能说明:开始查找日志文件
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lSelectMode，查询方式
							0－全部
							1－按类型
							2－按时间
							3－按时间和类型			 
			 dwMajorType，日志主类型
			    #define MAJOR_All		0x00	// 全部 
				#define MAJOR_ALARM		0x1	// 报警 
				#define MAJOR_EXCEPTION 0x2	// 异常
				#define MAJOR_OPERATION	0x3	// 操作 
				#define MAJOR_NET		0x4   //网络相关
				#define MAJOR_ACC		0x5 // 访问日志
			 dwMinorType：日志次类型
			 lpStartTime，开始时间
			 lpStopTime，结束时间
 *	输出参数:无
 *	函数返回:日志查找句柄，作为NET_DLDVR_FindNextLog()和NET_DLDVR_FindLogClose()函数的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera机型。在次类型填0时，返回指定主类型的所有日志
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindDVRLog(LONG lUserID,LONG lSelectMode, DWORD dwMajorType,DWORD dwMinorType, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);

/* 	功能说明:查找下一条日志文件
 *	输入参数:lLogHandle，日志查找句柄，即NET_DLDVR_FindDVRLog()函数的返回值
 *	输出参数:lpLogData，找到的日志信息
 *	函数返回:0：成功；返回负数表示失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindNextLog(LONG lLogHandle,LPNET_DLDVR_LOG lpLogData);

/* 	功能说明:关闭日志文件查找。
 *	输入参数:lLogHandle，日志查找句柄，即NET_DLDVR_FindDVRLog()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 在查找结束或中止查找时均须调用此接口，该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_FindLogClose(LONG lLogHandle);

//参数设置
/* 	功能说明:获取设备的系统参数配置
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwCommand，命令值
				#define NET_DLDVR_GET_ALLCFG				0	//获得所有参数
				#define NET_DLDVR_GET_DEVICECFG				1	//获取设备参数
				#define NET_DLDVR_GET_NETCFG				2	//获取网络参数
				#define NET_DLDVR_GET_PICCFG				3	//获取图象参数
				#define NET_DLDVR_GET_COMPRESSCFG			4	//获取压缩参数
				#define NET_DLDVR_GET_RECORDCFG				5	//获取录像时间参数
				#define NET_DLDVR_GET_DECODERCFG			6	//获取解码器参数
				#define NET_DLDVR_GET_RS232CFG 				7	//获取232串口参数
				#define NET_DLDVR_GET_ALARMINCFG 			8	//获取报警输入参数
				#define NET_DLDVR_GET_ALARMOUTCFG 			9	//获取报警输出参数
				#define NET_DLDVR_GET_TIMECFG 				10	//获取DVR时间
				#define NET_DLDVR_GET_PREVIEWCFG 			11	//获取预览参数
				#define NET_DLDVR_GET_VIDEOOUTCFG 			12	//获取视频输出参数
				#define NET_DLDVR_GET_USERCFG 				13	//获取用户参数
				#define NET_DLDVR_GET_EXCEPTIONCFG 			14	//获取异常参数
				#define NET_DLDVR_GET_SHOWSTRING			15	//获取叠加字符参数
				#define NET_DLDVR_GET_AUXOUTCFG				16	//获取辅助输出设置
				#define NET_DLDVR_GET_PICCFG_EX				17	//获取图象参数（扩展）
				#define NET_DLDVR_GET_USERCFG_EX 			18	//获取用户参数
				#define NET_DLDVR_GET_DEVICEVERSION			19  //获取AUX_PARAMETER参数 
				#define NET_DVR_GET_SEQ_ROUTE				20  //获取指定通道的巡航配置
				#define NET_DVR_GET_SYSTEMCFG_NVR	30       //获得所有的参数
				#define NET_DVR_GET_RVSCFG_NVR		31     //读取 视频源参数          
				#define NET_DVR_GET_ALARMREC_NVR	32    //读取 报警录像参数
				#define NET_DVR_GET_DEMOCFG_NVR		33    //读取 云台相关参数				
			 lChannel，通道号，从0开始；如果命令不需要通道号，该参数无效，置为-1即可 
			 lpOutBuffer，接收数据的缓冲指针 
			 dwOutBufferSize，接收数据的缓冲长度(以字节为单位) 
 *	输出参数:			 lpBytesReturned，实际收到的数据长度 
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpOutBuffer,DWORD dwOutBufferSize,LPDWORD lpBytesReturned);

/* 	功能说明:获取设备的系统参数配置
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwCommand，命令值
				#define NET_DLDVR_SET_ALLCFG				0	//获得所有参数
				#define NET_DLDVR_SET_DEVICECFG				1	//设置设备参数
				#define NET_DLDVR_SET_NETCFG				2	//设置网络参数
				#define NET_DLDVR_SET_PICCFG				3	//设置图象参数
				#define NET_DLDVR_SET_COMPRESSCFG			4	//设置压缩参数
				#define NET_DLDVR_SET_RECORDCFG				5	//设置录像时间参数
				#define NET_DLDVR_SET_DECODERCFG			6	//设置解码器参数
				#define NET_DLDVR_SET_RS232CFG				7	//设置232串口参数
				#define NET_DLDVR_SET_ALARMINCFG			8	//设置报警输入参数
				#define NET_DLDVR_SET_ALARMOUTCFG			9	//设置报警输出参数
				#define NET_DLDVR_SET_TIMECFG				10	//设置DVR时间
				#define NET_DLDVR_SET_PREVIEWCFG			11	//设置预览参数
				#define NET_DLDVR_SET_VIDEOOUTCFG			12	//设置视频输出参数
				#define NET_DLDVR_SET_USERCFG				13	//设置用户参数
				#define NET_DLDVR_SET_EXCEPTIONCFG			14	//设置异常参数
				#define NET_DLDVR_SET_SHOWSTRING			15	//设置叠加字符参数
				#define NET_DLDVR_SET_AUXOUTCFG				16	//设置辅助输出设置
				#define NET_DLDVR_SET_PICCFG_EX				17	//设置图象参数（扩展）
				#define NET_DLDVR_SET_USERCFG_EX			18	//设置用户参数
				#define NET_DLDVR_SET_AUX_PARAMETER			19  //设置AUX_PARAMETER参数
				#define NET_DVR_SET_SEQ_ROUTE				20  //设置指定通道的巡航配置
			 lChannel，通道号，从0开始；如果命令不需要通道号，该参数无效，置为-1即可 
			 lpInBuffer，发送数据的缓冲指针 
			 dwInBufferSize， 发送数据的缓冲长度(以字节为单位) 
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetDVRConfig(LONG lUserID, DWORD dwCommand,LONG lChannel, LPVOID lpInBuffer,DWORD dwInBufferSize);

/* 	功能说明:按卡号开始查找录像文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号,按位表示
			 dwFileType，文件类型
				0xff-全部；0-定时录像；1-移动侦测；2-报警触发；3-命令触发；4-手动录像；
			 bNeedCardNum：是否需要卡号 1-是 0-否
			 sCardNumber：卡号，只在bNeedCardNum为1时有效
			 lpStartTime，开始时间
			 lpStopTime，结束时间
 *	输出参数:无
 *	函数返回:查找句柄，作为NET_DVR_FindClose()等函数的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindFileByCard(LONG lUserID,LONG lChannel,DWORD dwFileType, BOOL bNeedCardNum, BYTE *sCardNumber, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);

//解码
NET_DLDVR_API VOID*			STDCALL NET_DLDVR_InitG722Decoder(INT nBitrate);
NET_DLDVR_API VOID			STDCALL NET_DLDVR_ReleaseG722Decoder(VOID *pDecHandle);
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_DecodeG722Frame(VOID *pDecHandle, BYTE* pInBuffer, BYTE* pOutBuffer);
//编码
NET_DLDVR_API VOID*			STDCALL NET_DLDVR_InitG722Encoder();
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_EncodeG722Frame(VOID *pEncodeHandle,BYTE* pInBuffer, BYTE* pOutBuffer);
NET_DLDVR_API VOID			STDCALL NET_DLDVR_ReleaseG722Encoder(VOID *pEncodeHandle);

//参数设置
/* 	功能说明:获取设备的系统参数配置，并保存到文件中
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，要保存的文件名
 *	输出参数:无		
 *	函数返回: TRUE：成功；FALSE：失败·
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_GetConfigFile(LONG lUserID, CHAR *sFileName);

/* 	功能说明:将配置文件应用到设备中
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，文件名
 *	输出参数: 无
 *	函数返回: TRUE：成功；FALSE：失败·
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetConfigFile(LONG lUserID, CHAR *sFileName);

/* 	功能说明:打开系统参数对话框
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 ParentWnd，父窗口句柄
 *	输出参数: 无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_OpenDvrCfgDialog(LONG lUserID, HWND ParentWnd);

/* 	功能说明:获取主机映像的版本
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: buf，远程版本信息
 *	函数返回: 0：成功，返回负数表示失败
 *	说明: 该接口不适用于IPCamera机型 
 */
NET_DLDVR_API INT		STDCALL NET_DLDVR_GetVersion(LONG lUserID, char *buf);	//2007-07-27

/* 	功能说明:开始查找JPEG图片
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lChannel，通道号，按位表示
			  lpStartTime，开始时间
			  lpStopTime，结束时间
 *	输出参数: 无
 *	函数返回: JPEG查找句柄，作为NET_DVR_FindClose()等函数的参数；返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG	STDCALL NET_DLDVR_FindJPEG( LONG lUserID, LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime );	//2007-07-27 add

/* 	功能说明:查找下一张JPEG图片
 *	输入参数: lFindHandle，JPEG查找句柄，即NET_DLDVR_FindJPEG()函数的返回值
 *	输出参数: lpFindData，查找到的jpeg数据
 *	函数返回: 返回大于等于0的表示成功，返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG	STDCALL NET_DLDVR_FindNextJPEG( LONG lFindHandle, LPNET_DLDVR_FIND_DATA lpFindData );	//2007-07-27 add
/* 	功能说明:停止查找JPEG图片
 *	输入参数: lFindHandle，JPEG查找句柄，即NET_DLDVR_FindJPEG()函数的返回值
 *	输出参数: 无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_FindJPEGClose( LONG lFindHandle);	//2007-07-27 add

//JPEG图片 显示	
/*	功能：初始化操作，为在窗口上显示JPEG图片做准备
 *	输入参数：HWND hWnd，显示jpeg图片的窗口句柄
 *	输出参数: 无
 *	函数返回：TRUE：成功； FALSE：失败
 *  说明: 
 */ 
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_ShowJPEG_ByName_Init( HWND hWnd );	//2007-10-09 add
/*  功能：在窗口上显示JPEG图片
 *	输入参数：lFindHandle，NET_DLDVR_Login()的返回值
			  sJPEGFileName，JPEG图片的名称
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *  说明: 
 */ 
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_ShowJPEG_ByName( LONG lFindHandle, char* sJPEGFileName );	//2007-07-27 add
/*  功能：释放资源
 *	输入参数：lFindHandle，NET_DLDVR_Login()的返回值
			  sJPEGFileName，JPEG图片的名称
 *	输出参数: 无
 *  函数返回：TRUE：成功； FALSE：失败
 *	说明: 在NET_DLDVR_ShowJPEG_ByName()之后调用
 */ 
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_ShowJPEG_ByName_Release( void);	//2007-10-09 add

/*	功能：根据指定的jpeg文件名，将其保存至指定的本地文件中
 *	输入参数：LONG lFindHandle，NET_DLDVR_Login()的返回值
 *		  char* pchJPEGFileName，需要保存的JPEG图片名
 *		  char* pchSaveFileName，保存jpeg图片的本地文件名
 *	输出参数: 无
 *  函数返回：TRUE：成功； FALSE：失败
 *	注释：
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_SaveJPEG_ByName( LONG lFindHandle, char* pchJPEGFileName, char* pchSaveFileName );	//2007-10-09 add

/* 	功能说明:开始JPEG监视
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lpClientInfo，预览参数 
 *	输出参数:无
 *	函数返回:JPEG监视句柄，负数表示失败
 *	说明: DM642机型只有在NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()调用成功以后，才能正确开启JPEG监视，否则会收不到JPEG数据
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_JpegRealPlay(LONG lUserID,LPNET_DLDVR_CLIENTINFO lpClientInfo);
/* 	功能说明: 停止JPEG监视
 *	输入参数: lRealHandle，监视ID，即NET_DLDVR_JpegRealPlay()函数的返回值
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_StopJpegRealPlay(LONG lRealHandle);

/* 	功能说明:强制登陆到DVR，不受主机在线用户数的控制
 *	输入参数:sDVRIP，DVR IP
			 wDVRPort，DVR端口
			 sUserName， 用户名
			 sPassword，密码
 *	输出参数: lpDeviceInfo，DVR的设备信息
 *	函数返回: 用户ID，该值具有唯一性，后续对设备的操作都需要通过此ID实现。若登录失败则返回负数
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LOGIN_HANDLE	STDCALL NET_DLDVR_ForceLogin(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,LPNET_DLDVR_DEVICEINFO lpDeviceInfo);

/* 	功能说明:打开流媒体播放
 *	输入参数:hWnd，视频播放窗口句柄
			 sysHead，数据流头数据，播放视频时需先读取数据流头来打开播放器。结构类型为DL_MP4FILE_HEAD 
			 len，数据长度 
			 mode，流模式（1-实时流/2-文件流）
			 video_enc，DVR类型
				#define DVR_C							0
				#define DVR_L							1
				#define DVR_LII							2
				#define DVR_MPS							3 
				#define DVR_MPF							4 
				#define DVR_MPC							5
				#define DVR_DVS							6	//杭州DVS
				#define DVR_ENC_H264					7	//H264编码器
				#define DVR_DEC_H264					8	//H264解码器
				#define DVR_400T						9   //上海400T系列
				#define DVR_IPCamera_GQ					10  //高清IpCameara(MG3500)	
				#define DVR_NVR							11
				#define DVR_400D						12  //杭州400D系列
				#define DVR_MPT							13   //Dm642系列 MPT,所有通道只能设HD1分辩率
				#define DVR_IPCamera_MG3512				14   //高清IpCameara	(MG3512)
				#define DVR_DEC_MP4						15	 //大立C/L系列解码器
				#define DVR_CMS							16	 //管理软件
				#define DVR_408T4						17   //408T D1带智能机器
				#define DVR_HISI32						18
				#define DVR_DM60						19	//DM60机型
				#define DVR_106							20	//3520系列下的106型机器，图像尺寸较大1280*1024
				#define DVR_400D2						21  //杭州400D2系列,与400D之间的系统参数有所不同
				#define DVR_716T						22 //2010-10-25 add， 14CIF+2D1的3520机型。即700机型(同于600的3520芯片）
				#define DVR_IPCamera_MG3515				23   //高清IpCameara	(MG3515芯片， 与3512机型的区别在于3512的音频不进行编解码，3515的进行编解码处理)	
				#define DVR_401K						24   //401K机型(不带硬盘录像,参数配置和400D||代的有所不同)
				#define DVR_400DX						25	//DVR400D系列，开发了一套新界面，目前暂定为DVR400DX，分4/8/16路
				#define DVR_404D4						26	//基于DVR404D机型，开放全D1选项，目前暂定机型DVR404D4
 *	输出参数: 无
 *	函数返回: 播放句柄，作为NET_DLDVR_CloseStream()等函数的输入参数；失败返回负数
 *	说明: 成功后可以通过NET_DLDVR_SetStreamBuf来对这个播放器填充视频数据进行播放
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_OpenStream(HWND hWnd, BYTE* sysHead, int len, int mode, int video_enc = 1 );	//2008-06-25,修改接口，增加编码方案类型

/* 	功能说明:关闭流媒体播放
 *	输入参数:handle，NET_DLDVR_OpenStream的返回值 
 *	输出参数: 无
 *	函数返回: 成功返回0；失败返回负数
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_CloseStream( LONG handle );

/* 	功能说明:设置播放器缓冲数据
 *	输入参数:handle，NET_DLDVR_OpenStream的返回值 
			 buf,视频数据 
			 len,数据长度 
 *	输出参数: 无
 *	函数返回: 大于等于0表示成功；失败返回负数
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetStreamBuf(LONG handle, BYTE *buf, int len);

/* 	功能说明:清空播放器缓冲数据
 *	输入参数:handle，NET_DLDVR_OpenStream的返回值 
 *	输出参数: 无
 *	函数返回: 成功返回大于等于0的整数；失败返回负数
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_EmptyStreamBuf(int handle);

/* 	功能说明:开始巡航
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lChannel，与巡航的云台相联的通道，从0开始
			 CruiseNo，巡航轨迹号
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_StartCruise( LONG lUserID, LONG lChannel, INT CruiseNo );

/* 	功能说明:结束巡航
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lChannel，与巡航的云台相联的通道，从0开始
			 CruiseNo，巡航轨迹号
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:该接口不适用于IPCamera和NVR机型 
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_StopCruise( LONG lUserID, LONG lChannel, INT CruiseNo );

/* 	功能说明:设置自定义语音文件
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，语音文件的名称
 *	输出参数: 无
 *	函数返回: 设置语音的ID，用作NET_DLDVR_GetCustomLanguageState()和NET_DLDVR_GetCustomLanguageSubState()的参数；
              若返回负数则表示操作失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetCustomLanguage( LONG lUserID,CHAR *sFileName );
/* 	功能说明:获取自定义语音设置的状态
 *	输入参数:lSetLanguageHandle，NET_DLDVR_SetCustomLanguage()的返回值
 *	输出参数: 无
 *	函数返回: 状态，
				#define NET_DLDVR_SETLANGUAGE				1020	//正在设置自定义语言
				#define NET_DLDVR_USETLANGUAGE_SUCESS		1023	//设置语言成功成功
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetCustomLanguageState(LONG lSetLanguageHandle);
/* 	功能说明:获取自定义语音设置的子状态
 *	输入参数:lSetLanguageHandle，NET_DLDVR_SetCustomLanguage()的返回值
 *	输出参数: nPostion，设置进度(0-100)
 *	函数返回: 子状态，
				#define NET_DLDVR_SETLANGUAGE_DOWNLOAD	1021	//正在设置自定义语言-下载语言文件
				#define NET_DLDVR_SETLANGUAGE_APPLYING	1022	//正在设置自定义语言-应用语言文件（写flash）
 *	说明: 该接口不适用于IPCamera和NVR机型 
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetCustomLanguageSubState(LONG lSetLanguageHandle, INT &nPostion);

/* 	功能说明:设置播放窗口
 *	输入参数:handle，播放句柄
			 hWnd，播放窗口句柄
 *	输出参数: 无
 *	函数返回: 返回大于等于0的整数，若返回负数表示失败
 *	说明:如果打开播放器时传入的参数hWnd为空,那么就可以在需要的时候调用该函数设置窗体句柄 
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_SetPlayHWnd( LONG handle, HWND hWnd );

/* 	功能说明:获得主机信息
 *	输入参数:sDVRIP，DVR IP
			 wDVRPort，DVR端口
 *	输出参数: lpSystemInfo，主机信息
 *	函数返回: 返回大于等于0的整数，若返回负数表示失败
 *	说明:该接口不适用于IPCamera机型
 */
NET_DLDVR_API DWORD STDCALL NET_DLDVR_GetSystemInfo(CHAR *sDVRIP,WORD wDVRPort, LPNET_DLDVR_SYSTEMINFO lpSystemInfo);

/* 	功能说明:回调函数，用于返回MP4文件转换成wmv文件的进度
 *	输入参数:pos，转换进度（0-100）
			 cbpara，用户自定义数据
 *	输出参数: 无
 *	函数返回: 返回大于等于0的整数，若返回负数表示失败
 *	说明:该接口不适用于IPCamera和NVR机型
 */
typedef INT (__stdcall *ConvertWMVCallBack)( INT pos, void* cbpara );
/* 	功能说明:将mp4文件转化为微软wmv文件
 *	输入参数:srcFile，需要进行转换的MP4文件
			 dstFile，转换的目标文件，即wmv文件
			 ConvertProc，回调函数，用于返回MP4文件转换成wmv文件的进度
			 cbpara，用户自定义数据
 *	输出参数: 无
 *	函数返回: 返回大于等于0的整数，若返回负数表示失败
 *	说明:该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_MP4ToWMV(char* srcFile, char* dstFile, ConvertWMVCallBack ConvertProc,
								void* cbpara = 0 );
/* 	功能说明:停止将mp4文件到微软wmv文件的转换
 *	输入参数:无
 *	输出参数: 无
 *	函数返回: 返回大于等于0的整数，若返回负数表示失败
 *	说明:该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_STOPMP4ToWMV();

/* 	功能说明:设置自定义的开机logo
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，logo文件的名称
 *	输出参数: 无
 *	函数返回: 设置log的ID，用作NET_DLDVR_GetCustomLogoState()和NET_DLDVR_GetCustomLogoSubState()的参数；
              若返回负数则表示操作失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetCustomLogo( LONG lUserID,CHAR *sFileName );
/* 	功能说明:获取开机logo设置的状态
 *	输入参数:lSetLogoHandle，即NET_DLDVR_SetCustomLogo()的返回值
 *	输出参数: 无
 *	函数返回: 状态，
				#define NET_DLDVR_SETLOGO				1030	//正在设置自定义LOGO
				#define NET_DLDVR_USETLOGO_SUCESS		1033	//设置LOGO成功成功
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetCustomLogoState(LONG lSetLogoHandle);
/* 	功能说明:获取开机logo设置的子状态
 *	输入参数:lSetLogoHandle，即NET_DLDVR_SetCustomLogo()的返回值
 *	输出参数: nPostion，设置进度(0-100)
 *	函数返回: 子状态，
				#define NET_DLDVR_SETLOGO_DOWNLOAD		1031	//正在设置自定义LOGO-下载Logo文件
				#define NET_DLDVR_SETLOGO_APPLYING		1032	//正在设置自定义LOGO-应用Logo文件（写flash）
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetCustomLogoSubState(LONG lSetLogoHandle, INT &nPostion);

/* 	功能说明:从DVR下载自定义语音文件
 *	输入参数:lSetLogoHandle，即NET_DLDVR_SetCustomLogo()的返回值
 *	输出参数: nPostion，设置进度(0-100)
 *	函数返回: 大于等于0表示成功，返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_GetLanguageFile(LONG lUserID, CHAR *sSavedFileName);

/* 	功能说明:设置字库
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，资源文件的名称
 *	输出参数: 无
 *	函数返回: 设置资源文件的ID，用作NET_DLDVR_GetResourceSetState()和NET_DLDVR_GetResourceSetSubState()的参数；
              若返回负数则表示操作失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetResource(LONG lUserID,CHAR *sFileName);
/* 	功能说明:获取字库设置的状态
 *	输入参数:lSetResourseHandle，即NET_DLDVR_SetResource()的返回值
 *	输出参数: 无
 *	函数返回: 状态，
				#define NET_DLDVR_SETRESOURCE				1040	//正在设置资源
				#define NET_DLDVR_SETRESOURCE_SUCESS		1043	//设置资源成功
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API INT  STDCALL NET_DLDVR_GetResourceSetState(LONG lSetResourseHandle);
/* 	功能说明:获取字库设置的子状态
 *	输入参数:lSetResourseHandle，即NET_DLDVR_SetResource()的返回值
 *	输出参数: nPostion，进度值(0-100)
 *	函数返回: 子状态，
				#define NET_DLDVR_SETRESOURCE_DOWNLOAD		1041	//正在设置资源-下载资源文件
				#define NET_DLDVR_SETRESOURCE_APPLYING		1042	//正在设置资源-应用资源文件（写flash）
 *	说明:该接口不适用于IPCamera和NVR机型 
 */
NET_DLDVR_API INT  STDCALL NET_DLDVR_GetResourceSetSubState(LONG lSetResourseHandle, INT &nPostion);

/* 	功能说明:清除字库
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: 无
 *	函数返回:  大于等于0表示成功，返回负数表示失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_ClearResource(LONG lUserID);

/* 	功能说明:清除告警
 *	输入参数:sDVRIP，DVR IP
			 port,DVR端口
 *	输出参数: 无
 *	函数返回:  大于等于0表示成功，返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_ClearAlarm(CHAR *sDVRIP, WORD port);

/* 	功能说明:根据卡号开始查找JPEG图片
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lChannel，通道号，按位表示
			  lpStartTime，开始时间
			  lpStopTime，结束时间
			  cardNo，卡号
 *	输出参数: 无
 *	函数返回: JPEG查找句柄，作为NET_DLDVR_FindNextJPEGByCardNo()和NET_DVR_FindClose()函数的输入参数；返回负数表示失败
 *	说明: 该接口只支持16路以下的DVR机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindJPEGByCardNo( LONG lUserID, LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, LPCTSTR cardNo );
/* 	功能说明:根据卡号查找下一张JPEG图片
 *	输入参数: lFindHandle，JPEG查找句柄，即NET_DLDVR_FindJPEGByCardNo()函数的返回值
			  nCardNo，卡号
 *	输出参数: lpFindData，查找到的jpeg数据
 *	函数返回: 返回大于等于0的表示成功，返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindNextJPEGByCardNo( LONG lFindHandle, LPNET_DLDVR_FIND_DATA lpFindData, CHAR *nCardNo );


/* 	功能说明:根据播放句柄设置接收DVR消息的窗口句柄
 *	输入参数:playHandle，播放句柄
			 hWnd，接收消息的窗口句柄
 *	输出参数: 无
 *	函数返回: TRUE
 *	说明:
 */
BOOL STDCALL NET_DLDVR_SetRecvDVRMessageHWndByHandle(long playHandle, HWND hWnd);

/* 	功能说明:将单帧数据以bmp格式保存到到Buf中(BMP格式，该buf写入文件直接就是bmp文件) 
 *	输入参数:lPlayHandle，播放句柄
			 bmpBuf，BMP数据
			 outBufLen，BMP数据长度
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:
 */
BOOL STDCALL NET_DLDVR_CaptureBmpPictureToBuf(LONG lPlayHandle,BYTE* bmpBuf, int* outBufLen);

/* 	功能说明:设置播放图象的视频参数 
 *	输入参数:lPlayHandle，播放句柄
			 nRegionNum：显示区域，如果只有一个显示区域(通常情况)设为0；
			 nBrightness: 亮度，默认50； 范围0-100；
			 nContrast: 对比度，默认50； 范围0-100；
			 nSaturation: 饱和度，默认50； 范围0-100；
			 nHue: 色调，默认50； 范围0-100；
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:如果全部为默认值将不进行颜色调节；该接口不适用于IPCamera和NVR
 */
BOOL STDCALL NET_DLDVR_SetColor(LONG lPlayHandle, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
/* 	功能说明:获取播放图象的视频参数 
 *	输入参数:lPlayHandle，播放句柄
			 nRegionNum：显示区域，如果只有一个显示区域(通常情况)设为0；
 *	输出参数: pBrightness: 亮度， 范围0-100；
			  pContrast: 对比度， 范围0-100；
			  pSaturation: 饱和度， 范围0-100；
			  pHue: 色调 范围0-100；
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:该接口不适用于IPCamera和NVR
 */
BOOL STDCALL NET_DLDVR_GetColor(LONG lPlayHandle, DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);

/* 	功能说明:登录到服务器
 *	输入参数:csIP，服务器IP
			 csPort，服务器端口
			 sUserName，用户名
			 sPassword，密码
 *	输出参数: 无
 *	函数返回: 登录句柄，作为NET_DLSERVER_Logout()等函数的输入参数；若返回负数表示失败
 *	说明:
 */
NET_DLDVR_API LOGIN_HANDLE	STDCALL NET_DLSERVER_Login(CHAR *csIP,WORD csPort,CHAR *sUserName,CHAR *sPassword);
/* 	功能说明:登出服务器
 *	输入参数:lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL			STDCALL NET_DLSERVER_Logout(LONG lUserID);
//获得服务器设备列表
/* 	功能说明:查找服务器设备列表
 *	输入参数:lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
 *	输出参数: 无
 *	函数返回: 0：成功； 负数：失败
 *	说明:
 */
NET_DLDVR_API LONG			STDCALL NET_DLSERVER_FindSvrDeviceInfo(LONG lUserID);
/* 	功能说明:查找下一个服务器设备信息
 *	输入参数: lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
 *	输出参数: lpFindData，服务器设备信息
 *	函数返回: 0：成功； 负数：失败
 *	说明:
 */
NET_DLDVR_API LONG			STDCALL NET_DLSERVER_FindSvrNextDeviceInfo(LONG lUserID,NET_DLSERVER_DEVICEINFO* lpFindData);
/* 	功能说明:停止查找服务器设备信息
 *	输入参数: lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
 *	输出参数: 无
 *	函数返回: TRUE
 *	说明:
 */
NET_DLDVR_API BOOL			STDCALL NET_DLSERVER_FindSvrClose(LONG lUserID);

/* 	功能说明:设置用于YUV视频回调的回调函数
 *	输入参数: lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()的返回值
			  yuvDataCallBack，回调函数，用于回调YUV数据
			  dwUser，用户自定义数据
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetYUVDataCallBack(LONG lRealHandle,fYUVDataCallBack yuvDataCallBack,DWORD dwUser);

//告警类型定义
#define ALARM_MOTION_TO_SERVER                							0  // 移动侦测报警
#define ALARM_VIDEO_LOST_TO_SERVER										1  //视频丢失告警
#define ALARM_VIDEO_RESTORE_TO_SERVER									1  //视频恢复
#define ALARM_HARDDISK_ERROR_TO_SERVER									2  //硬盘错误
#define ALARM_VIDEO_COVER_TO_SERVER										3  //视频遮挡
#define ALARM_EXTERNAL_TO_SERVER										4  //信号量告警输入
#define ALARM_VIDEOSOURCE_ONLINE_TO_SERVER								6  //DVR上线
#define ALARM_VIDEOSOURCE_LOST_TO_SERVER								7  //DVR掉线
#define ALARM_TRIPWIREALARM_TO_SERVER									8  //虚拟墙告警
/* 	功能说明:通知CS发生告警信息
 *	输入参数: lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
			  alarmDvrIP，产生告警的DVR IP
			  alarmType，告警类型
			  alarmChannel，告警通道
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SendAlarmToCS(LONG lUserID, char* alarmDvrIP, short alarmType, int alarmChannel);

/* 	功能说明:锁定录像文件
 *	输入参数: lUserID，登录句柄，即NET_DLSERVER_Login()的返回值
			  sRecFileName，录像名称
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:被锁定的录像文件在硬盘满自动覆盖的时候不会被删除。该接口不适用于IPCamera和NVR机型
 */
BOOL STDCALL NET_DLDVR_LockRecFile( LONG lUserID, CHAR *sRecFileName);

/* 	功能说明:将硬盘格式化为JPEG盘，用于保存JPEG图片
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lDiskNumber，硬盘号，从0 开始，0xff 表示所有硬盘	 
 *	输出参数:无
 *	函数返回:格式化ID，用作NET_DLDVR_GetFormatProgress()函数的参数
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
LONG STDCALL NET_DLDVR_FormatDiskToJPEG(LONG lUserID,LONG lDiskNumber);

/* 	功能说明:启动监听RVS自动上报信息。
 *	输入参数:无
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 该接口只适用于IPCamera机型
 */
BOOL STDCALL NET_DLDVR_ListenIPCamera();

/* 	功能说明:上传配置文件
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sFileName，文件名
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 该接口只适用于IPCamera机型
 */
LONG STDCALL NET_DLDVR_UploadConfigFile_IPCamera(LONG lUserID,CHAR *sFileName);

/* 	功能说明:开启DVR的报警功能
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: 无
 *	函数返回: 成功返回lUserID值； 失败返回负数
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
LONG STDCALL NET_DLDVR_SetDefend(LONG lUserID);
/* 	功能说明:关闭DVR的报警功能
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
BOOL STDCALL NET_DLDVR_SetUndefend(LONG lUserID);

/* 	功能说明:获取DVR的当前报警状态(是否开启告警功能)
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数: 无
 *	函数返回: 1-布防； 0-撤防； 返回负数表示获取失败
 *	说明: 该接口不适用于IPCamera和NVR机型
 */
INT STDCALL NET_DLDVR_GetDefendStatus(LONG lUserID);

/* 	功能说明:获取IPCamera的映像更新进度
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_Upgrade()函数的返回值
 *	输出参数:无
 *	函数返回:当前更新的进度(0-100)，返回负数表示失败
 *	说明: 该接口不适用除IPCamare以外的其他机型
 */
INT STDCALL NET_DLDVR_GetUpgradePos_IPCamera(LONG lUpgradeHandle);

/* 	功能说明:从NVR上下载日志
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 sSavedFileName，要保存的日志文件名
 *	输出参数:无
 *	函数返回:成功返回0：失败返回负数
 *	说明: 该接口不适用于除NVR以外的其他机型
 */
LONG STDCALL NET_DLDVR_GetLog_NVR(LONG lUserID, int LogType, CHAR *sSavedFileName);

/* 	功能说明:回调函数，用于获取从NVR下载指定的文件的进度
 *	输入参数:addr，保留参数
			 buf，保留参数
			 percent，当前下载的百分比
			 cbpara，用户自定义值
 *	输出参数:无
 *	函数返回:无
 *	说明: 该接口不适用于除NVR以外的其他机型
 */
typedef void (WINAPI *fDownloadCallBack) (LPCTSTR addr, char *buf, int percent, void* cbpara );

/* 	功能说明:从NVR上下载邮件模板
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 type，邮件类型   0：告警邮件  1：错误邮件
			 path，邮件保存路劲
			 setupdDataProc，回调函数
			 cbpara，用户自定义值
 *	输出参数:无
 *	函数返回:成功返回0：失败返回负数
 *	说明: 该接口不适用于除NVR以外的其他机型
 */
LONG STDCALL NET_DLDVR_DownloadMail_NVR(LONG lUserID, int type, LPCTSTR path, 
										fDownloadCallBack setupdDataProc, void* cbpara);

#define UPGRADE_UPLOAD_STATE 1 
#define UPGRADE_UPDATE_STATE 2
/* 	功能说明:回调函数，用于获取上传邮件模板到NVR的进度值
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 state，上传状态
				#define UPGRADE_UPLOAD_STATE 1 
				#define UPGRADE_UPDATE_STATE 2
			 len，当前已经上传的百分比
			 cbpara，用户自定义值
 *	输出参数:无
 *	函数返回:无
 *	说明: 该接口不适用于除NVR以外的其他机型
 */
typedef void (WINAPI *UpdateCallBack)(LONG lUserID, int state, int len, void* cbpara);
/* 	功能说明:上传邮件模板到NVR
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 type，邮件类型   0：告警邮件  1：错误邮件
			 filename，文件名
			 UpdateProc，回调函数，用于获取上传进度
			 cbpara，用户自定义值
 *	输出参数:无
 *	函数返回:无
 *	说明: 该接口不适用于除NVR以外的其他机型
 */
LONG STDCALL NET_DLDVR_UploadMail_NVR(LONG lUserID, int type, const char *filename, 
									  UpdateCallBack UpdateProc, void* cbpara);

/* 	功能说明:远程获取设备工作状态（扩展通道数到32路）
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:lpWorkState，设备工作状态
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: NVR和3520机型需要调用该接口来获取设备工作状态，同时该接口不适用于IPCamera机型,
 */
BOOL STDCALL NET_DLDVR_GetDVRWorkState_EX(LONG lUserID, LPNET_DLDVR_WORKSTATE_EX lpWorkState);

/* 	功能说明:，回调函数，用于获取标准码流
 *	输入参数: lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()的返回值
			  dwDataType，数据类型
						#define NET_DLDVR_SYSHEAD 				0xEE //系统头数据
						#define NET_DLDVR_STREAMDATA 			0xEF //流数据
						#define NET_DLDVR_STREAMDATA_VIDEO 		0x1	//标准视频流
						#define NET_DLDVR_STREAMDATA_AUDIO		0x2 //标准音频流
			  pBuffer：用于存放录像数据的缓存
			  dwBufSize：缓存区大小
			  err，错误码 
			  dwUser，用户自定义数据
 *	输出参数: 无
 *	函数返回: 无
 *	说明:
 */
typedef VOID(CALLBACK *fStdDataCallBack) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize, int err, DWORD dwUser);

/* 	功能说明:，设置用于获取标准码流的回调函数
 *	输入参数: lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()的返回值
			  stdDataCallBack，回调函数，用于获取标准码流
			  dwUser，用户自定义数据
 *	输出参数: 无
 *	函数返回: TRUE：成功； FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetStandardDataCallBack(LONG lRealHandle, fStdDataCallBack stdDataCallBack, DWORD dwUser);

/* 	功能说明: 设置视频显示的显示器
 *	输入参数: playHandle，播放句柄
			  nDeviceNum，监视器序号 1:表示第一个显示器     2:表示第二个显示器     3:表示第三个显示器等等
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDDrawDevice(LONG playHandle, DWORD nDeviceNum );
	
/* 	功能说明:，获取本地录像文件的开始时间和结束时间
 *	输入参数: sfile，文件名
 *	输出参数: start_time，开始时间
			  end_time，结束时间
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:
 */
BOOL STDCALL NET_DLDVR_GetLocalFileTime(CHAR *sfile,  DWORD &start_time, DWORD &end_time);	

/* 	功能说明:，按主次类型的方式开始查找录像文件
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lChannel，通道号，按位表示
			  dwFileType，录像文件的类型 0-定时录像， 1-移动侦测	2-报警触发  3-命令触发   4-手动录像 
			  dwAlarmType，告警类型，仅当dwFileType为2（即报警录像）时有效，
					0x90- 绊线
					0x91- 禁区
					0x92- 遗弃
					0x93- 聚集
					0x94- 奔跑
 *	输出参数: start_time，开始时间
			  end_time，结束时间
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:查找普通告警录像时调用NET_DLDVR_FindFile()接口，查找智能分析告警录像时调用该接口
 */
LONG STDCALL NET_DLDVR_FindFile_EX(LONG lUserID,LONG lChannel,DWORD dwFileType, DWORD dwAlarmType, 
								 LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);					


/* 	功能说明: 帧放通道选择，2010-02-08 add
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lChannel，通道号，从0开始
			  IFrameOnly，是否是全码流发送，0-全码流， 1-只发I帧
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:目前只适用于400系列机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SET_I_FRAME_ONLY_CHANNEL(LONG lUserID,LONG lChannel, DWORD IFrameOnly);					

/* 	功能说明: 设置指定通道的码流方式，2010-02-28 add
 *	输入参数: lRealHandle，实时视频句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			  lChannel，通道号，保留
			  Stream，码流方式，0-主码流， 1-次码流, 16-只传帧头， 17-恢复
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该功能只适用于400D机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetChannelStream_Ex(LONG lRealHandle, LONG lChannel, DWORD Stream );

/* 	功能说明: 获取DM60机型的主机系统参数，2010-03-19 add
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  dwCommand，命令值
			   dwCommand，命令值
				#define NET_DLDVR_GET_ALLCFG				0	//获得所有参数
			 lpOutBuffer，接收数据的缓冲指针 
			 dwOutBufferSize，接收数据的缓冲长度(以字节为单位) 
 *	输出参数: lpBytesReturned，实际接收到的数据长度
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDVRConfig_DM60(LONG lUserID, DWORD dwCommand,
									 LPVOID lpOutBuffer,DWORD dwOutBufferSize,LPDWORD lpBytesReturned);

/* 	功能说明:获取DM60设备的系统参数配置
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwCommand，命令值
				#define NET_DLDVR_SET_ALLCFG				0	//获得所有参数 
			 lpInBuffer，发送数据的缓冲指针 
			 dwInBufferSize， 发送数据的缓冲长度(以字节为单位) 
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDVRConfig_DM60(LONG lUserID, DWORD dwCommand,
									 LPVOID lpInBuffer,DWORD dwInBufferSize);

/* 	功能说明:执行DM60设备的命令
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwCommand，命令值
				#define NET_DLDVR_CLEARALLPOINT		1	//清除所有点
				#define NET_DLDVR_CLEARALLLINE		2	//清除所有线
				#define NET_DLDVR_CLEARALLAREA		3	//清除所有区域
				#define NET_DLDVR_ZEROADJUST		4	//红外调零
				#define NET_DLDVR_KEYBOARD			5	//键盘命令，Value1中存放键盘码
				#define NET_DLDVR_SETFOCUSPOS		6	//设置焦距预置位
				#define NET_DLDVR_CALLFOCUSPOS		7	//调用焦距预置位
				#define NET_DLDVR_FOCUSSTOP			8	//聚焦停止
				#define NET_DLDVR_FOCUSSTEPNEAR		9	//向前单步聚焦
				#define NET_DLDVR_FOCUSSTEPFAR		10	//向后单步聚焦
				#define NET_DLDVR_FOCUSAUTO			11	//自动聚焦
				#define NET_DLDVR_FOCUSSTEPS		12	//连续步进聚焦，正数往前，负数向后
				#define NET_DLDVR_FOCUSSTEPSNEAR	13	//向前连续步进聚焦，直到FocusStop
				#define NET_DLDVR_FOCUSSTEPSFAR		14	//向后连续步进聚焦，直到FocusStop
				#define NET_DLDVR_VOLTAGELEVEL		15	//设置电平，Value1步长，+1, 表示增加1， -1, 表示减小1
				#define NET_DLDVR_UPCOLORTEMP		16	//设置色标温度上限，Value1同VoltageLevel
				#define NET_DLDVR_DOWNCOLORTEMP		17	//设置色标温度下限，Value1同VoltageLevel
				#define NET_DLDVR_IMAGEMODE			18	//调整图像模式Value1=Manul/Auto
			 dwValue,要设置的参数值，不需要的时候填0 
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDMCommand_DM60(LONG lUserID, DWORD dwCommand, INT dwValue);

/* 	功能说明:获取当前正在使用的色标的色标号
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回: 正数成功，表示色标号，负数表示失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetDMColor_DM60(LONG lUserID);

/* 	功能说明:设置当前使用的色标及状态
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 RuleId，色标号，0-8
			 Status，该色标的状态，1-启用；0-未启用
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDMColor_DM60(LONG lUserID, DWORD RuleId, DWORD Status);

/* 	功能说明:获取指定测温目标的参数
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *			 dwCommand，命令值
 *				 #define NET_DVR_GET_DMMEASURE_POINT	1
 *				 #define NET_DVR_GET_DMMEASURE_LINE		2
 *				 #define NET_DVR_GET_DMMEASURE_AREA		3
 *			 Index，测温点的序号，0-3
 *			 lpOutBuffer，接收数据的缓冲指针 
 *			 dwOutBufferSize，接收数据的缓冲长度(以字节为单位) 
 *	输出参数:lpBytesReturned，实际收到的数据长度 		
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDMMeasureCfg_DM60(LONG lUserID, DWORD dwCommand, INT Index, LPVOID lpOutBuffer,DWORD dwOutBufferSize, LPDWORD lpBytesReturned);

/* 	功能说明:设置指定测温点的参数
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *			 dwCommand，命令值
 *				#define NET_DVR_SET_DMMEASURE_POINT	1
 *				#define NET_DVR_SET_DMMEASURE_LINE	2
 *				#define NET_DVR_SET_DMMEASURE_AREA	3
 *			 Index，测温点的序号，0-3
 *			 lpInBuffer，发送数据的缓冲指针 
 *			 dwInBufferSize， 发送数据的缓冲长度(以字节为单位) 
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDMMeasureCfg_DM60(LONG lUserID, DWORD dwCommand, INT Index,LPVOID lpInBuffer,DWORD dwInBufferSize);

/* 	功能说明:以字符串的方式获取DM60机器的主机参数
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:cfgBuf,主机的主板参数
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetConfigString_DM60(LONG lUserID, char *cfgBuf);

/* 	功能说明:以字符串的方式设置DM60机器的主机参数
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 cfgBuf,主机的主板参数
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetConfigString_DM60(LONG lUserID, char *cfgBuf);

/* 	功能说明:以字符串的方式设置DM60机器的测温目标
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 index,序号
			 type，测温目标类型，定义值如下
					#define NET_DVR_SET_DMMEASURE_POINT	1
					#define NET_DVR_SET_DMMEASURE_LINE	2
					#define NET_DVR_SET_DMMEASURE_AREA	3
			 DMMeasureBuf,测温目标参数
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDMMeasureString_DM60(LONG lUserID, int index, int type, char *DMMeasureBuf);

/* 	功能说明:以字符串的方式设置DM60机器的主机参数
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
				type，测温目标类型，定义值如下
					#define NET_DVR_GET_DMMEASURE_POINT	1
					#define NET_DVR_GET_DMMEASURE_LINE	2
					#define NET_DVR_GET_DMMEASURE_AREA	3
				index,序号
 *	输出参数:DMMeasureBuf,测温目标参数
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDMMeasureString_DM60(LONG lUserID, int type, int index, char* DMMeasureBuf);

/* 	功能说明:将DM60机器的主机参数从字符串形式对应到主机结构中
 *	输入参数:buf，从主机端收到的主机参数字符串，通过NET_DLDVR_GetConfigString_DM60()函数获得
		     
 *	输出参数:pSyscfg,主机参数
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_DM60Config_StringToStruct(char *buf, LPSYSTEMCFG_DM60 pSyscfg);

/* 	功能说明:将DM60机器的主机参数从主机结构形式对应到字符串形式
 *	输入参数:pSyscfg,需要进行转化的主机参数	 
 *	输出参数:buf, 主机参数字符串，用作NET_DLDVR_SetConfigString_DM60()函数的输入参数
 *	函数返回: 无
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API VOID STDCALL NET_DLDVR_DM60Config_StructToString( LPSYSTEMCFG_DM60 pSyscfg, char *buf);


/* 	功能说明:将index指定的测温目标(点、线、区域）设为不启用状态
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
			 dwCommand，命令值
				#define NET_DVR_CLEAR_DMMEASURE_POINT	1
				#define NET_DVR_CLEAR_DMMEASURE_LINE	2
				#define NET_DVR_CLEAR_DMMEASURE_AREA	3
			 index，测温目标编号，支持4个测温点（0 - 3），1个测温线（0），3个测温区域（0-2）
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_ClearMeasureType_DM60( LONG lUserID, DWORD dwCommand, INT index );

/* 	功能说明:设置仪器的IP地址
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 IP, IP地址
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetIPAddr_DM60( LONG lUserID, char* IP, char *SubMask, char *GateWay );

/* 	功能说明:设置仪器的MAC地址
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 MAC, MAC地址
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetMACAddr_DM60( LONG lUserID, char* MAC  );

/* 	功能说明:调整仪器时间
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 year			年
 *	   		 month		月
 *			 day			日
 *			 hour		时
 *			 min			分
 *			 sec			秒
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDateTime_DM60( LONG lUserID, int year,int month,int day,int hour,int min,int sec  );

/* 	功能说明:获取仪器的当前时间
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:DataTime,字符串，格式格式yyyy-mm-dd hh:mm:ss
 *				yyyy	年
 *	   			mm		月
 *				dd		日
 *				hh		时
 *				mm		分
 *				ss		秒
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDateTime_DM60( LONG lUserID, char *DataTime );

/* 	功能说明:恢复出厂设置
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_LoadDefault_DM60( LONG lUserID );

/* 	功能说明:获取仪器的系统信息
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:SysInfo,系统信息
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetSystemInfo_DM60( LONG lUserID, char *SysInfo  );

/* 	功能说明:获取仪器中各测温目标的实时温度
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 Type, 测温类型，0-获取所有测温目标的实时温度 1-测温点；2-测温线；3-测温区域
 *					//DM60测温目标定义
 *							#define NET_DVR_GET_DMMEASURE_ALL	0
 *							#define NET_DVR_GET_DMMEASURE_POINT	1
 *							#define NET_DVR_GET_DMMEASURE_LINE	2
 *							#define NET_DVR_GET_DMMEASURE_AREA	3
 *			 index，编号(0--3)
 *			 Mode,0：返回一次温度;  1：连续返回温度，间隔时间由仪器自动调节。
 *	输出参数:无
 *	函数返回: 大于等于0：成功；负数：失败
 *	说明: 该接口只适用于DM60机型，实时温度通过回调函数传出
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_GetDMTEMP_Start(LONG lUserID, int Type, int index,  int Mode);

/* 	功能说明:停止获取仪器的实时温度
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDMTEMP_Stop(LONG lUserID);

/* 	功能说明:建立透明通道进行串口控制
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 SerialPort，串口号, 0-串口1， 1-串口2
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_TransSerial_Start(LONG lUserID,LONG SerialPort);

/* 	功能说明:通过透明通道给串口发送命令
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 SerialPort，串口号， 0-串口1， 1-串口2
 *			 pCmdBuf，命令字符串
 *			 dwCmdBufSize，命令大小
 *	输出参数:pOutBuf，从串口收到的回应
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_TransSerial_Ctrl(LONG lUserID,LONG SerialPort,CHAR *pCmdBuf,DWORD dwCmdBufSize, CHAR *pOutBuf);

/* 	功能说明:关闭透明通道
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 SerialPort，串口号，  0-串口1， 1-串口2
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口不适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_TransSerial_Stop(LONG lUserID,LONG SerialPort);

/* 	功能说明:打开录像参数对话框
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 ParentWnd，父窗口句柄
 *	输出参数: 无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于400D机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_OpenDvrRecCfgDialog(LONG lUserID, HWND ParentWnd);

/* 	功能说明:注册回调函数，接收设备消息等（同时回调出设备IP，用以区分不同设备）
 *	输入参数: cbMessCallBack，回调函数
				BOOL(CALLBACK *fMessageCallBack)(LONG lCommand, char *sDVRIP, char *pBuf, DWORD dwBufLen);
					lCommand值：#define COMM_ALARM 0x1100 //报警信息 对应结构 NET_DLDVR_ALARMINFO 
								#define COMM_ALARM_EX	 0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
								#define COMM_TRADEINFO   0x1500 //ATMDVR主动上传交易信息 
								#define SERIAL_NUMBER	 0x1600 //DVR上报的序列号
								#define DISKCHANNEL_STATE 0x1700 //主机定时上传硬盘状态、通道录像状态信息 
								#define RVS_INFO		 0x1800 //IPCamera上传的RVS信息
								#define EXCEPTION_NOTIFY 0x1900 //异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息 
								#define DM_TEMP			0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
								#define DMTEMP_ALARM	0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
								#define DM_ANALYS_ALARM		0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
								#define	PLAYBACK_OVER		0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION
								#define RESOLUTION_CHANGE	0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
								#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
					 sDVRIP:设备IP地址 
					pBuf:回调出的信息
					dwBufLen：回调出的信息长度
			  messageType，消息类型，0-普通机型消息；1-DM60机型的（实时测温信息与温度告警信息）
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetAllMessCallBack(BOOL (CALLBACK *fMessCallBack)
										   (LONG lCommand,CHAR *sDVRIP,CHAR *pBuf,DWORD dwBufLen), INT messageType =0);

/* 	功能说明:云台初始化设置
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 Protocol, 云台控制协议
 *			 nAddrID, 云台地址
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_PTZSettings_DM60( LONG lUserID, int Protocol, int nAddrID );

/* 	功能说明:获取DVR的硬盘状态（只适用于600系列机型）
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 lpDiskState，硬盘状态
 *	输出参数:无
 *	函数返回: TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于600系列机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetDVRDiskState(LONG lUserID, LPNET_DLDVR_DVRDISKSTATE lpDiskState);

/* 	功能说明:用于获取主机映像的主机型号和版本信息（只有600机型需要进行检测）
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 sFileName，要升级的映像文件名称
 *	输出参数:dvrType，主机型号
 *			 Version，版本信息
 *	函数返回:0：成功；-1：失败，失败时可调用NET_DLDVR_GetLastError()获得失败原因
 *	说明: 该接口只适用于600系列机型，其他机型不进行检查直接返回0
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetImagefileInfo(LONG lUserID,CHAR *sFileName, char *dvrType, char *Version);

/* 	功能说明:获取各色标的正反相状态，以字符串方式返回
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:buf，色标的正反相状态
 *	函数返回:0：成功；-1：失败，失败时可调用NET_DLDVR_GetLastError()获得失败原因
 *	说明: 该接口只适用于DM60系列机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetDMColorRuleString_DM60(LONG lUserID, char *buf);

/* 	功能说明:以字符串方式设置各色标的正反相状态
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 buf，要进行设置的色标状态
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60系列机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SetDMColorRuleString_DM60(LONG lUserID, char *buf);

/* 	功能说明:开始原始数据采集
 *	输入参数:lUserID,用户ID，即NET_DLDVR_Login()函数的返回值
 *			 sFileName,保存文件的文件名（文件名后缀为.dlv)
 *			 Frame，所要采集的原始数据帧数，<0时一直采集直到调用NET_DLDVR_StopCaptureInfraredFrame
 * 			 Time，帧与帧之间的间隔时间(ms) >= 20ms，且必须是20的倍数。注意：Frame * Time <= 8秒。（预留参数，目前无效）
 *			 streamType，帧类型，2-RAW  3-YUV
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60系列机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_CaptureInfraredFrame(LONG lUserID, const char *sFileName, int Frame,int Time, int streamType);

/* 	功能说明:停止原始数据采集
 *	输入参数:lRealHandle,采集句柄，即NET_DLDVR_CaptureInfraredFrame()函数的返回值
 *	输出参数:无
 *	函数返回:TRUE：成功；FALSE：失败
 *	说明: 该接口只适用于DM60系列机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_StopCaptureInfraredFrame(LONG lRealHandle);

/* 	功能说明:云台预制位控制，需要先打开监视(适用于600机型)
 *	输入参数:lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZPresetCmd，云台预置点命令
					#define SET_PRESET		1	// 设置预置点 
					#define CLE_PRESET		2	// 清除预置点 
					#define GOTO_PRESET		3	// 转到预置点
			 dwPresetIndex，预置点的序号，最多支持128个预置点
			 sPresetName,预置点名称
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口只适用于600机型和NVR机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_PTZPreset_600(LONG lRealHandle,DWORD dwPTZPresetCmd,DWORD dwPresetIndex, const char *sPresetName);

/* 	功能说明:开始实时监视
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 lpClientInfo，预览参数 
 *	输出参数:info,打开监视时DVR返回的一些信息，如帧率、分辨率等
 *	函数返回:监视句柄，负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_RealPlay_EX(LONG lUserID,LPNET_DLDVR_CLIENTINFO lpClientInfo, MONITOR_INFO *info);

/* 	功能说明:云台控制
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwPTZCommand，云台控制命令， 0-云台 1-镜头 2-光圈
			 Movement，云台动作，
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_PTZControl_DM60(LONG lUserID,DWORD dwPTZCommand,DWORD Movement);

/* 	功能说明:云台预置位控制
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			 dwPTZCommand，云台预置位控制命令
			 dwPresetIndex，云台预置位
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_PTZPreset_DM60(LONG lUserID,DWORD dwPTZPresetCmd, DWORD dwPresetIndex);

/* 	功能说明:获取DM60的状态
 *	输入参数:lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
 *	输出参数:无
 *	函数返回:DM60的当前状态
 *	说明: 该接口只适用于DM60机型和IPCamera机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetDM60Status(LONG lUserID);

/* 	功能说明:获取DM60原始数据采集的状态
 *	输入参数:lRealHandle，采集句柄，即NET_DLDVR_CaptureInfraredFrame()函数的返回值
 *	输出参数:无
 *	函数返回:原始数据采集的当前状态
				#define DM60_CAPTUREINFRARED_ING		1	// 正在采集
				#define DM60_CAPTUREINFRARED_OVER		2	// 采集完成
				#define DM60_CAPTUREINFRARED_ERROR		3	// 采集出错
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetCaptureInfraredStatus_DM60(LONG lRealHandle);

/* 	功能说明:获取数据接收缓存的大小
 *	输入参数:lRealHandle，监视句柄或远程回放句柄
 *	输出参数:无
 *	函数返回:缓存大小
 *	说明: 
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetRecvCacheSize(LONG lRealHandle);

//600机型文件升级的文件类型
#define ALARMIN_RINGTONE		0	//外部告警的告警声
#define MOTIONALARM_RINGTONE	1	//移动侦测告警的告警声
#define SHELTERALARM_RINGTONE	2	//视频遮挡告警的告警声
#define VIDEOLOST_RINGTONE		3	//视频丢失告警的告警声
#define EXCEPTIONAL_RINGTONE	4	//异常告警的告警声

/* 	功能说明:远程更新主机文件（600机型）
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 sFileName，要更新的映像名称
			 fileType，文件类型
				0 外部告警alarmin
				1 移动侦测告警
				2 视频遮挡告警
				3 视频丢失告警
				4 异常告警			 
 *	输出参数:无
 *	函数返回:更新ID，作为NET_DLDVR_GetUpgradeFileState_EX()函数的输入参数；返回负数表示失败
 *	说明:该接口只适用于600机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FILE_UPDATE_EX( LONG lUserID, CHAR *sFileName, int fileType );

/* 	功能说明:获取文件远程更新的状态
 *	输入参数:lUpgradeHandle，更新ID，即NET_DLDVR_FILE_UPDATE_EX()函数的返回值
 *	输出参数:无
 *	函数返回:状态值
				#define NET_DLDVR_SETFILE_SUCCESS	1051	//升级成功
				#define NET_DLDVR_SETFILE			1050	//正在升级
				#define NET_DLDVR_SETFILE_NETERROR	-8097	//网络断开，状态未知
				#define NET_DLDVR_SETFILE_FAIL		-8098	//升级文件失败

 *	说明: 该接口只适用于600机型
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetUpgradeFileState_EX(LONG lUpgradeHandle);

/* 	功能说明:设置获取实时回放视频数据的回调函数
 *	输入参数:fRealDataCallBack_EX，视频数据回调函数
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_SetRealDataCallBack_EX(LONG lRealHandle, fRealDataCallBack_EX realDataCallBack, DWORD dwUser);

/* 	
	功能说明:将long类型的时间转换成NET_DLDVR_TIME结构的格式
 *	输入参数:dat，long类型的时间值 相对2000年0时0分0秒
 *	输出参数:time，转换成NET_DLDVR_TIME结构格式的时间
 *	函数返回:无
 *	说明: 
 */
NET_DLDVR_API VOID	STDCALL NET_DLDVR_LongToTime( DWORD dat , NET_DLDVR_TIME* time );

/* 	
	功能说明:将NET_DLDVR_TIME格式的时间转换成long类型的值
 *	输入参数:time，NET_DLDVR_TIME格式的时间值
 *	输出参数:无
 *	函数返回:转换后的时间值 相对2000年0时0分0秒
 *	说明: 
 */
NET_DLDVR_API DWORD STDCALL NET_DLDVR_TimeToLong(  NET_DLDVR_TIME* time );


/* 	功能说明:更新DM60指定类型的文件
 *	输入参数:lUserID，登陆句柄
 *			 sFileName，要进行更新的文件名
 *			 Type，文件类型
 *				#define UPDATEFILE_TYPE_DM60_MID		0
 *				#define UPDATEFILE_TYPE_DM60_FIREWARE	1
 *				#define UPDATEFILE_TYPE_DM60_DATA		2
 *	输出参数:无
 *	函数返回:返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_UpdateFile_DM60(LONG lUserID,CHAR *sFileName, int Type);

/* 	功能说明:获取DM60指定类型文件的更新进度
 *	输入参数:lUpdateHandle，更新句柄，即NET_DLDVR_UpdateFile_DM60（）函数的返回值
 *	输出参数:无
 *	函数返回:进度值（0-100）， 返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API INT STDCALL NET_DLDVR_GetUpdateFilePos_DM60( LONG lUpdateHandle );


/* 	功能说明:获取MAC地址
 *	输入参数:lUserID，登陆句柄
 *	输出参数:lpOutBuffer， MAC地址
 *	函数返回:返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_GetMacAddress_DM60(LONG lUserID, LPVOID lpOutBuffer);


/* 	功能说明:打开/关闭仪器前端
 *	输入参数:lUserID，登陆句柄
 *			 cmd，命令值
 *				#define  NET_DVR_SENSORPOWER_ON		0 
 *				#define  NET_DVR_SENSORPOWER_OFF	1	
 *				#define  NET_DVR_SENSORPOWER_CHG	2 
 *	输出参数:无
 *	函数返回:返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_SensorPower_DM60( LONG lUserID, int cmd  );

/* 	功能说明:本地回放时跳转到指定时间点进行播放
 *	输入参数:lPlayHandle，回放句柄
 *			 Time，希望跳转到的时间值
 *	输出参数:无
 *	函数返回:返回负数表示失败
 *	说明: 
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_PLAYFILE_GotoTime( LONG lPlayHandle, LPNET_DLDVR_TIME Time );


/* 	功能说明:开始查找单个通道的录像文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
			 lChannel，通道号,从0开始
			 dwFileType，文件类型
					0xff-全部；0-定时录像；1-移动侦测；2-报警触发(此时查询到的录像会包括普通报警录像和智能分析报警录像)；3-命令触发；4-手动录像；
			 lpStartTime，开始时间
			 lpStopTime，结束时间
 *	输出参数:无
 *	函数返回:查找句柄，作为NET_DVR_FindClose()等函数的参数；返回负数表示失败
 *	说明: 不适用于IPCamera机型
 */
NET_DLDVR_API LONG			STDCALL NET_DLDVR_FindFile_OneChanel(LONG lUserID,LONG lChannel,DWORD dwFileType, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);

/* 	功能说明:从主机上下载MTC文件
 *	输入参数:lUserID，用户ID，NET_DLDVR_Login()函数的返回值
 *			 path，文件所要存贮的路径
 *	输出参数:无
 *	函数返回:查找句柄，作为NET_DVR_FindClose()等函数的参数；返回负数表示失败
 *	说明: 该接口只适用于DM60机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_DownloadMTC_DM60(LONG lUserID, LPCTSTR path);

/* 	功能说明:，按主次类型的方式开始查找录像文件，可查询预览通道的路线
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  previewCh,是否查询预览通道录像，1-是 0-否
			  lChannel，通道号，按位表示
			  dwFileType，录像文件的类型 0-定时录像， 1-移动侦测	2-报警触发  3-命令触发   4-手动录像 
			  dwAlarmType，告警类型，仅当dwFileType为2（即报警录像）时有效，
					0x90- 绊线
					0x91- 禁区
					0x92- 遗弃
					0x93- 聚集
					0x94- 奔跑
			  start_time，开始时间
			  end_time，结束时间
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于400机型及600机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindFile_WithPreview(LONG lUserID, int previewCh, LONG lChannel,DWORD dwFileType, DWORD dwAlarmType, 
											LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime);


/* 	功能说明:，查找录像文件，所返回的文件已连续时间内的同类型录像为分段方式
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  previewCh,是否查询预览通道录像，1-是 0-否
			  lChannel，通道号，按位表示
			  start_time，开始时间
			  end_time，结束时间
			  MaxCount，返回的最大文件条数
			  buf，文件数组地址，数组大小为MaxCount，文件结构为NET_DLDVR_FILE_DATA
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于400机型及600机型
 */
LONG STDCALL NET_DLDVR_FindFileBYTIME(LONG lUserID,LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, 
									  WORD MaxCount, char *buf);

/* 	功能说明:，查找录像/JPEG文件
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lChannel，通道号，按位表示
			  fileType，文件类型.0-录像文件，1-JPEG文件
			  videoType,录像类型.0xff 全部	0 － 定时录像	1 - 移动侦测	2 － 报警触发	3 －命令触发	4 － 手动录像  
			  start_time，开始时间
			  end_time，结束时间
			  MaxCount，返回的最大文件条数
			  buf，文件数组地址，数组大小为MaxCount，文件结构为NET_DLDVR_FIND_DATA
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于400机型及600机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindFileOnce(LONG lUserID,LONG lChannel, DWORD fileType, DWORD videoType, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, 
									WORD MaxCount, char *buf);

/* 	功能说明:，查找日志文件
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  lSelectMode，查询方式，实际并不使用
			  dwMajorType,主类型 
			  dwMinorType，次类型
			  start_time，开始时间
			  end_time，结束时间
			  MaxCount，返回的最大文件条数
			  buf，文件数组地址，数组大小为MaxCount，文件结构为NET_DLDVR_LOG
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于400机型及600机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindLogOnce(LONG lUserID,LONG lSelectMode, DWORD dwMajorType,DWORD dwMinorType, 
								   LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, WORD MaxCount, char *buf);


/* 	功能说明:，为DM60机型设置机器类型和控制端口（因为DM60机型原5000端口上的登陆接口将取消）
 *	输入参数: sDVRIP，主机IP
 *     		  wDVRHttpPort，控制端口
			  sUserName, 用户名
			  sPassword，密码
 *	输出参数: 无
 *	函数返回: 用户ID，该值具有唯一性，后续对设备的操作都需要通过此ID实现。返回负数表示失败
 *	说明:该接口只适用于DM60机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetDvrType_DM60(CHAR *sDVRIP, WORD wDVRHttpPort, CHAR *sUserName, CHAR *sPassword);


/* 	功能说明:，为DM60机型设置视频端口
 *	输入参数: sDVRIP，主机IP
			  wDVRPort，视频端口
 *	输出参数: 无
 *	函数返回: 返回0成功，返回负数表示失败
 *	说明:该接口只适用于DM60机型
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetVideoPort_DM60(CHAR *sDVRIP, WORD wDVRPort);

/* 	功能说明:，按参数类型恢复各参数的出厂设置
 *	输入参数: lUserID，用户ID，即NET_DLDVR_Login()函数的返回值
			  ConfigType，参数类型参数类型
					#define NET_DLDVR_RESTORE_SYSTEM 		1	//基本参数 
					#define NET_DLDVR_RESTORE_DISPLAY		2	//显示参数
					#define NET_DLDVR_RESTORE_RECORD		3  //录像参数
					#define NET_DLDVR_RESTORE_USER			4  //用户参数
					#define NET_DLDVR_RESTORE_NETWORK		5 //网络参数
					#define NET_DLDVR_RESTORE_PTZ			6  //云台参数
					#define NET_DLDVR_RESTORE_ALLCFG		7  //全部参数
 *	输出参数: 无
 *	函数返回: TRUE：成功  FALSE：失败
 *	说明:该接口只适用于600及700机型
 */
NET_DLDVR_API BOOL STDCALL NET_DLDVR_RestoreConfigByType(LONG lUserID, int ConfigType);


/* 	功能说明:注册回调函数，接收设备报警消息等（同时回调出设备IP，用以区分不同设备）
 *	输入参数: index，序号
			  cbMessCallBack，回调函数
				BOOL(CALLBACK *fMessageCallBack)(LONG lCommand, char *sDVRIP, char *pBuf, DWORD dwBufLen);
				lCommand值：
					#define COMM_ALARM 0x1100 //报警信息 对应结构 NET_DLDVR_ALARMINFO 
					#define COMM_ALARM_EX	 0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
					#define COMM_TRADEINFO   0x1500 //ATMDVR主动上传交易信息 
					#define SERIAL_NUMBER	 0x1600 //DVR上报的序列号
					#define DISKCHANNEL_STATE 0x1700 //主机定时上传硬盘状态、通道录像状态信息 
					#define RVS_INFO		 0x1800 //IPCamera上传的RVS信息
					#define EXCEPTION_NOTIFY 0x1900 //异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息 
					#define DM_TEMP			0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
					#define DMTEMP_ALARM	0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
					#define DM_ANALYS_ALARM		0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
					#define	PLAYBACK_OVER		0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION
					#define RESOLUTION_CHANGE	0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
					#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
				sDVRIP:设备IP地址 
				pBuf:回调出的信息
				dwBufLen：回调出的信息长度
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_SetDVRMessCallBack_WithIndex(int index, BOOL (CALLBACK *fMessCallBack)(LONG lCommand,CHAR *sDVRIP,CHAR *pBuf,DWORD dwBufLen));

/* 	功能说明:注册回调函数，接收设备报警消息等（同时回调出用户ID，用以区分不同用户）
 *	输入参数: index,序号
			  cbMessCallBack，回调函数
				BOOL(CALLBACK *fMessageCallBack)(LONG lCommand, char *sDVRIP, char *pBuf, DWORD dwBufLen);
					lCommand值：
						#define COMM_ALARM 0x1100 //报警信息 对应结构 NET_DLDVR_ALARMINFO 
						#define COMM_ALARM_EX	 0x1101	//600机型的报警信息 对应结构 NET_DLDVR_ALARMINFO_600
						#define COMM_TRADEINFO   0x1500 //ATMDVR主动上传交易信息 
						#define SERIAL_NUMBER	 0x1600 //DVR上报的序列号
						#define DISKCHANNEL_STATE 0x1700 //主机定时上传硬盘状态、通道录像状态信息 
						#define RVS_INFO		 0x1800 //IPCamera上传的RVS信息
						#define EXCEPTION_NOTIFY 0x1900 //异常掉线等信息，对应结构 NET_DLDVR_EXCEPTION 对应窗体通知消息 
						#define DM_TEMP			0x2000	//DM60机型的实时测温信息，对应结构NET_DLDVR_DMTEMP
						#define DMTEMP_ALARM	0x2010	//DM60机型的温度告警信息，对应结构NET_DLDVR_DMTEMP_ALARM
						#define DM_ANALYS_ALARM					0x2011	//DM60机型的智能分析告警信息，对应结构NET_DLDVR_DMANALYS_ALARM
						#define	PLAYBACK_OVER		0x2020	//回放结束，对应结构NET_DLDVR_EXCEPTION
						#define RESOLUTION_CHANGE	0x2021	//分辨率改变，对应结构NET_DLDVR_EXCEPTION
						#define VIDEOFORMAT_CHANGE				0x2022	//视频格式改变，对应结构NET_DLDVR_EXCEPTION
					lUserID：用户登录ID，即NET_DLDVR_Login的返回值 
					pBuf:回调出的信息
					dwBufLen：回调出的信息长度
 *	输出参数: 无
 *	函数返回: TRUE:成功 FALSE:失败
 *	说明: 
 */
NET_DLDVR_API BOOL			STDCALL NET_DLDVR_SetDVRMessCallBack_EX_WithIndex(int index, BOOL (CALLBACK *fMessCallBack_EX)(LONG lCommand,LONG lUserID,CHAR *pBuf,DWORD dwBufLen));


/* 	功能说明:监视重连
 *	输入参数: lRealHandle，监视句柄
 *	输出参数: 无
 *	函数返回: 成功则返回lRealHandle，返回负数表示失败
 *	说明: 调用该函数时必须保证DVR没有掉线，否则无法成功重连
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_ReMonitor(LONG lRealHandle);

/* 	功能说明:查询是否支持某种接口
 *	输入参数: lUserID，登录句柄
			  mainType：0-接口 1-配置 2-功能 (目前只支持0)
			  subType: 具体的接口代码（见接口代码表）
 *	输出参数: 无
 *	函数返回: 成功时，返回>=0，返回负数表示失败
 *	说明: 无
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_QueryFunction(LONG lUserID, int mainType, int subType);

/* 	功能说明:查询主机所支持所有接口
 *	输入参数: lUserID，登录句柄
			  funcList：NET_DLFUNC_PROPERTY数组指针（上层用户不用分配内存）
			  size: 数组元素个数
 *	输出参数: 无
 *	函数返回: 成功时，返回>=0，返回负数表示失败
 *	说明: 具体的接口代码（见接口代码表）
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_QueryFunctionAll(LONG lUserID, LPNET_DLFUNC_PROPERTY &funcList, int &size);

/* 	功能说明: 一次性设置主机的监视用户的状态
 *	输入参数: state: NET_DLDVR_STREAMSTATE数组
			  size: 数组元素个数
 *	输出参数: 无
 *	函数返回: 成功时，返回>=0，返回负数表示失败
 *	说明: NET_DLDVR_STREAMSTATE数组元素都必须是属于同一台DVR主机的监视用户
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_SetMulChannelStream_EX(LPNET_DVR_STREAMSTATE state, int size);

/* 	功能说明: 查找主机日志
 *	输入参数: sDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
			dwMajorType，日志主类型
				#define MAJOR_All		0x00	// 全部 
				#define MAJOR_ALARM		0x1	// 报警 
				#define MAJOR_EXCEPTION 0x2	// 异常
				#define MAJOR_OPERATION	0x3	// 操作 
				#define MAJOR_NET		0x4   //网络相关
				#define MAJOR_ACC		0x5 // 访问日志
			dwMinorType：日志次类型
		  	lpStartTime，开始时间
			lpStopTime，结束时间
			buf：NET_DLDVR_LOG数组（由用户分配内存）
			nSize: 数组元素个数
			nStartPos: 表示从第几条记录开始查询，主要用于多次查询。首次查询时，该值填0.
			dwTotalCount:返回主机端该时间段内总共有多少条记录
 *	输出参数: 无
 *	函数返回: 成功时，返回实际检索到的日志条数，返回负数表示失败
 *	说明: 无
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindLogs_EX(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,DWORD dwMajorType,DWORD dwMinorType, LPNET_DLDVR_TIME lpStartTime, 
   											  LPNET_DLDVR_TIME lpStopTime, LPNET_DLDVR_LOG buf, int nSize, int nStartPos, int &dwTotalCount);
/* 	功能说明: 查找主机录像文件
 *	输入参数: sDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
			  lChannel，通道号,按位表示
			  dwFileType，文件类型 0xff-全部；0-定时录像；1-移动侦测；2-报警触发(此时查询到的录像会包括普通报警录像和智能分析报警录像)；3-命令触发；4-手动录像；
		  	  lpStartTime，开始时间
			  lpStopTime，结束时间
			  buf：NET_DLDVR_FIND_DATA_EX数组（由用户分配内存）
			  nSize: 数组元素个数
			  nStartPos: 表示从第几条记录开始查询，主要用于多次查询。首次查询时，该值填0.
			  dwTotalCount:返回主机端该时间段内总共有多少个文件
 *	输出参数: 无
 *	函数返回: 成功时，返回实际检索到的录像文件数量，返回负数表示失败
 *	说明: 无
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_FindFiles_EX(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,BOOL privewCh, LONG lChannel,DWORD dwFileType,LPNET_DLDVR_TIME lpStartTime, 
												  LPNET_DLDVR_TIME lpStopTime, LPNET_DLDVR_FIND_DATA_EX buf, int nSize, int nStartPos, int &dwTotalCount);
/* 	功能说明:根据录像文件的名称下载文件
 *	输入参数:sDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
			 sDVRFileName，要下载的录像文件名
			 sSavedFileName，希望保存到本机的录像文件名
 *	输出参数:无
 *	函数返回:下载句柄，作为NET_DLDVR_StopGetFile_EX()和NET_DLDVR_GetDownloadPos_EX()的参数；返回负数表示失败
 *	说明: 无
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_GetFileByName_EX(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,CHAR *sDVRFileName,CHAR *sSavedFileName);

/* 	功能说明:根据录像文件的时间下载文件
 *	输入参数:sDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
			 lChannel: 通道号，从0开始
			 sSavedFileName，希望保存到本机的录像文件名
 *	输出参数:无
 *	函数返回:下载句柄，作为NET_DLDVR_StopGetFile_EX()和NET_DLDVR_GetDownloadPos_EX()的参数；返回负数表示失败
 *	说明: 无
 */
NET_DLDVR_API LONG STDCALL NET_DLDVR_GetFileByTime_Ex(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, CHAR *sSavedFileName);

/* 	功能说明:停止下载
 *	输入参数:lFileHandle，下载句柄，即NET_DLDVR_GetFileByName()或NET_DLDVR_GetFileByTime()函数的返回值		 
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera机型
 */
NET_DLDVR_API BOOL	STDCALL NET_DLDVR_StopGetFile_EX(LONG lFileHandle);

/* 	功能说明:获取下载进度
 *	输入参数:lFileHandle，下载句柄，即NET_DLDVR_GetFileByName_EX()或NET_DLDVR_GetFileByTime_EX()函数的返回值		 
 *	输出参数:无
 *	函数返回:进度值（0-100），返回负数表示获取失败
 *	说明: 
 */
NET_DLDVR_API INT	STDCALL NET_DLDVR_GetDownloadPos_EX(LONG lFileHandle);

/*      功能说明:按录像名称进行远程回放
*     输入参数:lsDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
               sPlayBackFileName，需要进行回放的录像文件名
               hWnd，进行回放的窗口句柄
			   dwStartPos, 起始播放位置
*     输出参数:无
*     函数返回: 播放句柄，作为NET_DLDVR_StopPlayBack等函数的参数；返回负数表示失败
*     说明: 该接口不适用于IPCamera机型
*/
NET_DLDVR_API LONG  STDCALL NET_DLDVR_PlayBackByName_EX(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,CHAR *sPlayBackFileName, HWND hWnd, DWORD dwStartPos);

/*      功能说明:按时间段进行远程回放
*     输入参数:sDVRIP,wDVRPort, sUserName, sPassword用于指定DVR主机信息
               lChannel，通道号，从0开始
               lpStartTime，开始时间
               lpStopTime，结束时间
               hWnd，进行回放的窗口句柄
*     输出参数:无
*     函数返回: 播放句柄，作为NET_DLDVR_StopPlayBack等函数的参数；返回负数表示失败
*     说明: 该接口不适用于IPCamera和NVR机型
*/
NET_DLDVR_API LONG  STDCALL NET_DLDVR_PlayBackByTime_EX(CHAR *sDVRIP,WORD wDVRPort,CHAR *sUserName,CHAR *sPassword,LONG lChannel, LPNET_DLDVR_TIME lpStartTime, LPNET_DLDVR_TIME lpStopTime, HWND hWnd, DWORD dwStartPos);

/* 	功能说明:云台控制，需要先进行监视
 *	输入参数:lRealHandle，NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZCommand，命令值
				#define LIGHT_PWRON						3	// 接通灯光电源 
				#define WIPER_PWRON						2	// 接通雨刷开关 
				#define FAN_PWRON						4	// 接通风扇开关 
				#define HEATER_PWRON					5	// 接通加热器开关 
				#define AUX_PWRON						6	// 接通辅助设备开关 

				#define ZOOM_IN							11	// 焦距以速度SS变大(倍率变大) 
				#define ZOOM_OUT						12	// 焦距以速度SS变小(倍率变小) 
				#define FOCUS_NEAR						13	// 焦点以速度SS前调 
				#define FOCUS_FAR						14	// 焦点以速度SS后调 
				#define IRIS_OPEN						15	// 光圈以速度SS扩大 
				#define IRIS_CLOSE						16	// 光圈以速度SS缩小 
				#define TILT_UP							21	// 云台以SS的速度上仰 
				#define TILT_DOWN						22	// 云台以SS的速度下俯 
				#define PAN_LEFT						23	// 云台以SS的速度左转 
				#define PAN_RIGHT						24	// 云台以SS的速度右转 
				#define PAN_AUTO						29	// 云台以SS的速度左右自动扫描 
			 dwAction，云台停止动作或开始动作：1－开始；0－停止
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型，需要主机支持新接口REMOTE_PTZ_CONTROL_REQUEST_EX，不再使用主机返回登录用户ID  
 */
BOOL STDCALL NET_DLDVR_PTZControl_EX(LONG lRealHandle,DWORD dwPTZCommand,DWORD dwStop);

/* 	功能说明:云台预制位控制，需要先打开监视
 *	输入参数:lRealHandle，监视句柄，即NET_DLDVR_RealPlay()或NET_DLDVR_RealPlay_EX()函数的返回值
			 dwPTZPresetCmd，云台预置点命令
					#define SET_PRESET		1	// 设置预置点 
					#define CLE_PRESET		2	// 清除预置点 
					#define GOTO_PRESET		3	// 转到预置点
			 dwPresetIndex，预置点的序号，最多支持128个预置点
			 sPresetName,预置点名称
 *	输出参数:无
 *	函数返回:TRUE：成功； FALSE：失败
 *	说明: 该接口不适用于IPCamera和NVR机型，需要主机支持新接口REMOTE_PTZPRESET_CONTROL_REQUEST_EX，不再使用主机返回登录用户ID
 */
BOOL STDCALL NET_DLDVR_PTZPreset_EX(LONG lRealHandle,DWORD dwPTZPresetCmd,DWORD dwPresetIndex,const char *sPresetName);


}//extern "C"

#endif//#ifndef DLLSDK_INTERFACE_H
