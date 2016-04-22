/**********************************************************************
* @note
* Copyright 2009, Beijing BlueSky Technologies Inc.
* ALL RIGHTS RESERVED
* Permission is hereby granted to licensees of Beijing
* BlueSky Technologies Inc. Products to use or abstract
* this computer program for the sole purpose of implementing
* a product based on Beijing BlueSky Technologies Inc.
* No other rights to reproduce, use, or disseminate this
* computer program, whether in part or in whole, are granted.
* Beijing BlueSky Technologies Inc. makes no representation or
* warranties with respect to the performance of this computer
* program, and specifically disclaims any responsibility for
* any damages, special or consequential, connected with the
* use of this program.
* For details, see http://www.bstar.com.cn/
***********************************************************************/
/*****************************************************************************
*
* Copyright (C) BeiJing BlueStar Software Tech. Ltd., 2010
*
* $Id: BstarStandard.h 11 2010-12-08 16:14:25Z zhaoxiong $
*
* Bstar out IVS stream definition(1.3.06).
*
****************************************************************************/
#ifndef _BSTARSTANDARD_H_
#define _BSTARSTANDARD_H_

#define MAX_POLY_NUM 48
#define MAX_POLY_POINT_NUM 16
/*#define MAX_RULE_NUM 48*/
#define MAX_DEPTH_NUM 3

#define MAX_ALGEVENT_NUM 11
#define MAX_CHANNEL_NUM 16

//#define MAX_STDOBJ_NUM 4 //最大检测物和校验个数
#define MAX_STDDEPTH_NUM 4 //最大检测物和校验个数(标的检测)
/** @struct MotPoint
* @brief points definition
*/
typedef struct
{
	unsigned short x; //输入参数, 点坐标x
	unsigned short y; //输入参数, 点坐标y
} MotPoint;


/** @struct MotQuad
* @brief quad definition
*/
typedef struct
{
	MotPoint topleft; //区域左上点
	MotPoint bottomright; //区域右下点
	MotPoint topright; //区域右上点
	MotPoint bottomleft; //区域左下点
} MotQuad;


/** @struct MotDeepT
* @brief depth definition
*/
typedef struct
{
	MotPoint top; ///< 输入参数, 上点坐标
	MotPoint bottom; ///< 输入参数, 下点坐标
} MotDeepT;



/** @struct MotRect
* @brief rectangle definition
*/
typedef struct
{
	MotPoint topleft; //区域左上点
	MotPoint bottomright; //区域右下点
} MotRect;

typedef struct
{
	MotPoint pt1; //
	MotPoint pt2; //
	int direction;
} trapwriePoint;

/** @struct MotObject
* @brief object structure definition
*/
typedef struct
{
	int id; //对象实例的唯一标识(同一类型的相同对象实例,如同一个人其标识相同，不同实例序号递增)
	int attributes; //对象的特征参数bitmap，如人与车种类
	MotQuad figure; //对象的位置和外部轮廓(四边形)
} MotObject;



/** @struct MotPoly
* @brief area structure definition
*/
typedef struct
{
	unsigned char vertices; //(v1.3)每个多边形的顶点个数
  unsigned char poly_type;  //多边形类型
	unsigned char res; //(v1.3)
	unsigned char zones; //(v1.3)内部子区位个数(普通为0 个，岗哨类型为岗位区位和排除区位共1+x 个)
	MotPoint points[MAX_POLY_POINT_NUM]; //顶点位置列表；
} MotPoly;


/** @struct MotDeep
* @brief depth structure definition
*/
typedef struct
{
	int depthHeight; // 标定：实际人的高度
	MotDeepT depthT[MAX_DEPTH_NUM]; // 标定：标定的个数；
	MotDeepT    depthCheck;
} MotDeep;



// 输出码流结构
typedef struct
{
	MotPoly spot; //所在的敏感范围(多边形区域)
	unsigned char type; //探测的类型
	unsigned char behavior; //对象的行为(实际发生的结果)
	short arg; //行为的参数
	char concerns; //(v1.3+)配置的感兴趣行为表(bitmap),如ENTER|EXIT
	char res[3]; //(v1.3+)保留字段(v1.3 以前为int res)
	int cnt; //对象的个数
} MotIAEvent;


typedef enum
{
	ALL_RESET = 0,       //智能功能：全局重置
	BORDER = 1,          //智能功能：周界保护
	UNATTENDANT = 2,     //智能功能：遗留物分析
	REMOVE = 3,          //智能功能：物品看护
	PERSONFIGHT = 4,           //智能功能：斗殴检测
	PERSONLINGER = 5,          //智能功能：人员滞留
	PEOPLE_CLOSE = 6,    //智能功能：人员接近
	TRAPWIRE = 8,        //智能功能：绊线检测
	MOT_PEOPLE_COUNT = 10, //智能功能：人数统计
	FACE_DETECT = 20,      //智能功能：人脸检测
	SENTRY_TRESPASS = 30,  //智能功能：哨兵检测
	SENTRY_GUARDER = 31,   //智能功能：哨兵检测
	ATM_PROTECTION = 40    //智能功能：ATM 看护
} IVSType;


//周界保护
typedef enum
{
	BORDER_RESET = 0, //周界保护：重置
	ENTER = 1, //周界保护：移入
	ESCER = 2, //周界保护：移出
	SETTLE = 4, //周界保护：滞留
	BORDER_APPEAR = 8, //周界保护：出现
	BORDER_DISAPPEAR = 16, //周界保护：消失
	BORDER_APPER_ENTER = 32 //周界保护：移入和出现两种功能同时实现
} BorderBehavior;


//遗留物检测
typedef enum
{
	UNATTENDANT_RESET = 0, //遗留物：重置
	APPEAR = 1 //遗留物：从无到有
} UnattendantBehavior;


//物品看护
typedef enum
{
	REMOVE_RESET = 0, //物品看护：重置
	DISAPPEAR = 1 //物品看护：从有到无
} RemoveBehavior;


//拌线
typedef enum
{
	TRAPWIRE_RESET = 0, //绊线：重置
	CROSS_ABOVE = 1, //绊线：由上往下穿越
	CROSS_BELOW = 2, //绊线：由下往上穿越
	CROSS_TWO = 3, //绊线：两种情况都发生
	TOUCH_ABOVE = 4, //绊线：由上往下碰到
	TOUCH_BELOW = 8 //绊线：由下往上碰到
} TrapwireBehavior;


//人数统计
typedef enum
{
	PEOPLECOUNT_RESET = 0, //人数统计：重置
	EQUALS = 1, //人数统计：等于
	MORE_THAN = 2, //人数统计：大于
	LESS_THAN = 4 //人数统计：小于
} PeopleCountBehavior;


//哨兵检测
typedef enum
{
	SENTRYTRESPASS_RESET = 0, //哨兵检测外圈看护：重置
	INTRUDE = 1 //哨兵检测外圈看护：入侵
} SentryTrespassBehavior;


typedef enum
{
	SENTRYGUARDER_RESET = 0, //哨兵检测内圈看护：重置
	GUARD_OK = 4, //哨兵检测内圈看护：正常
	GUARD_LEAVE = 8, //哨兵检测内圈看护：离岗
	GUARD_SQUAT = 16, //哨兵检测内圈看护：蹲岗
	GUARD_LIE = 32 //哨兵检测内圈看护：睡岗
} SentryGuarderBehavior;


//人脸检测
typedef enum
{
	FACEDETECTIN_RESET = 0, //人脸检测：重置
	EXIST = 1, //人脸检测：检测有人脸存在(从无到有)
	NONEXIST = 2 //人脸检测：检测没有人脸存在(从有到无)
} FaceDetectBehavior;

typedef enum
{
	REFPOINT_BOTTOM = 0,//基准位置 底部
	REFPOINT_MIDDLE = 1,//基准位置 1/4处
	REFPOINT_TOP = 2//基准位置 1/2处
}RefPoint;

/////////////////////////////////////////////////////////////



/*
普通参数-start
BSCP_DVX7-B(录放现场)
118
*/
typedef struct
{
	int res; ///< 周界保护没有普通参数，保留接口
} IntrusionCustomerParam;


typedef struct
{
	int res; ///< 绊线没有普通参数，保留接口
} DirectionCustomerParam;


typedef struct
{
	int maxObjHeight; ///< 物体最大高度
	int minObjHeight; ///< 物体最小高度
	int timeToAlarm; ///< 输入参数, 物体停留超过该时间后报警（5-1800）
} UnattendedCustomerParam;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int maxObjHeight; ///< 物体最大高度
	int minObjHeight; ///< 物体最小高度
	int timeToAlarm; ///< 输入参数, 物体停留超过该时间后报警（5-1800）
} RemoveCustomerParam;


typedef struct
{
	int counterUpLimit; ///< 上限人数限制(0-10)
	int counterDownLimit; ///< 上限人数限制(0-10)
	int timeToAlarm; ///< 报警检测时间（1-600）
} PeopleCountCustomerParam;



typedef struct
{
	int res; ///< 人脸检测预留参数
} NoFaceDetecCustomerParam;


typedef struct
{
	int alarmStartTime; ///< 报警检测时间
} FightCustomerParam;


typedef struct
{
	MotPoint start; ///< ATM 边界设置定点
	MotPoint end; ///< ATM 边界设置低点
	int timeToAlarm; ///< 报警检测时间
} PeopleCloseCustomerParam;



typedef struct
{
	int modeSignal; ///< 模式信号
	int sceneSignal; ///< 场景信号
	MotPoly spot;    //脱岗区域  
} GuardCustomerParam;


typedef struct
{
	int lingertimetoAlarm; ///< 报警检测时间
} LingerCustomerParam;


typedef struct
{
	MotPoint cardArea[4]; ///< 卡口
	MotPoint kbArea[4]; //键盘
	MotPoint activeArea[4]; //活动区域
	MotPoint mark[2][2]; //标定 左上右下点
	int timeToAlarm; ///< 报警检测时间
} AtmCustomerParam;

/*
普通参数-end
*/


//////////////////////////////////////////////////////////////////
/*
Ipc多路智能并发专用参数-start
BSCP_DVX7-B(录放现场)
0x0003.0519
*/
typedef struct
{
	int behavior; ///< 周界保护没有普通参数，保留接口1
} IntrusionCustomerParamEx;


typedef struct
{
	int res; ///< 绊线没有普通参数，保留接口
} DirectionCustomerParamEx;


typedef struct
{
	int targetTextureThres; ///< 目标检测纹理阈值
	int peopleDetecFlag; ///< 人检测标志
	int maxObjHeight; ///< 物体最大高度
	int minObjHeight; ///< 物体最小高度
	int timeToAlarm; ///< 输入参数, 物体停留超过该时间后报警（5-1800）
	int timeToStopAlarm; ///< 报警持续时间
} UnattendedCustomerParamEx;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int targetTextureThres; ///< 目标检测纹理阈值
	int peopleDetecFlag; ///< 人检测标志
	int maxObjHeight; ///< 物体最大高度
	int minObjHeight; ///< 物体最小高度
	int timeToAlarm; ///< 输入参数, 物体停留超过该时间后报警（5-1800）
	int timeToStopAlarm; ///< 报警持续时间
} RemoveCustomerParamEx;


typedef struct
{
	int counterUpLimit; ///< 上限人数限制(0-10)
	int counterDownLimit; ///< 上限人数限制(0-10)
	int timeToAlarm; ///< 报警检测时间（1-600）
} PeopleCountCustomerParamEx;



typedef struct
{
	int res; ///< 人脸检测预留参数
} NoFaceDetecCustomerParamEx;


typedef struct
{
	int objectMinWidth; ///< 探测物最小宽度
	int objectMinHeight; ///< 探测物最小高度
	int objectMaxWidth; ///< 探测物最大宽度
	int objectMaxHeight; ///< 探测物最大高度
	int alarmEndTime; ///< 警后延报时间
} FightCustomerParamEx;


typedef struct
{
	MotPoint start; ///< 线段, 顶点
	MotPoint end; ///< 线段, 底点
} MotSeg;


typedef struct
{
	unsigned char vertices; ///< 每个多边形的顶点个数
	MotPoint points[MAX_POLY_POINT_NUM]; ///< 顶点位置列表；
} MotPolyArea;


typedef struct
{
	int maxHeight; ///< 探测物最大高度
	int minHeight; ///< 探测物最小高度
	int atmMachineNums; ///< ATM 机个数
	int timeToAlarm; ///< 报警检测时间
	MotSeg atmOne; ///< ATM 一米线
	MotSeg atmDouble; ///< ATM 两倍线
	MotPolyArea atmFloor; ///< ATM 地面区域
} PeopleCloseCustomerParamEx;


typedef struct
{
	int modeSignal; ///< 模式信号
	int offSiteSensitivity; ///< 脱岗区域灵敏度
	MotPoly spot;    //脱岗区域  
} GuardCustomerParamEx;

typedef struct
{
	unsigned short  endNum;     // 报警端子 
	unsigned short  changeTime; // 换岗时间 
}GuarderAppParamEx;


typedef struct {
	GuardCustomerParamEx cusparam;
	GuarderAppParamEx apparam;
	unsigned char poly_type; ///多边形类型: 1: 多边形，2：矩形	3.伴线
	unsigned char res[3];
}GuardCustParamEx;


typedef struct
{
	int lingertimetoAlarm; ///< 报警检测时间
} LingerCustomerParamEx;


typedef struct
{
	int timeToAlarm; ///< 报警检测时间
	int timeToStopAlarm; ///< 报警持续时间
	int activeSensitivity; ///< 活动区检测灵敏度
	int keyCardSensitivity; ///< 敏感区检测灵敏度
	int textureSensitivity; ///< 目标纹理检测灵敏度
	MotPoint cardArea[4]; ///< 卡口
	MotPoint kbArea[4]; ///< 键盘
	MotPoint activeArea[4]; ///< 活动区域
} AtmCustomerParamEx;

/*
普通参数-end
*/


//////////////////////////////////////////////////////////////////


/*
高级参数-start
*/
typedef struct
{
	int objectMinWidth; ///< 探测物最小宽度
	int objectMinHeight; ///< 探测物最小高度
	int statictimeToAlarm; ///< 探测物最大静止时间
	int referencePoint;//基准位置
} IntrusionAdvanceParam;



typedef struct
{
	int objectMinWidth; ///< 探测物最小宽度
	int objectMinHeight; ///< 探测物最小高度
	int statictimeToAlarm; ///< 探测物最大静止时间
	int referencePoint;//基准位置
} DirectionAdvanceParam;


typedef struct
{
	int sceneLevels; ///< 光影抑制标志
	int peopleDetecFlag; ///< 人检测标志
	int frameInterval ; ///< 更新率
	int timeToStopAlarm; ///< 报警持续时间
	int referencePoint;//基准位置
} UnattendedAdvanceParam;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int sceneLevels; ///< 光影抑制标志
	int peopleDetecFlag; ///< 人检测标志
	int frameInterval ; ///< 更新率
	int timeToStopAlarm; ///< 报警持续时间
	int referencePoint;//基准位置
} RemoveAdvanceParam;


typedef struct
{
	int objectMinWidth; ///< 探测物最小宽度
	int objectMinHeight; ///< 探测物最小高度
	int statictimeToAlarm; ///< 探测物最大静止时间
	int referencePoint;//基准位置
} PeopleCountAdvanceParam;


typedef struct
{
	int res; ///< 人脸检测保留接口
	int referencePoint;//基准位置
} NoFaceAdvanceParam;


typedef struct
{
	int objectMinWidth;     ///< 探测物最小宽度
	int objectMinHeight;    ///< 探测物最小高度
	int objectMaxWidth;     ///< 探测物最小宽度
	int objectMaxHeight;    ///< 探测物最小高度
	int alarmEndTime;       ///< 警后延报时间
	int referencePoint;//基准位置
} FightAdvanceParam;


typedef struct
{
	int maxHeight; ///< 探测物最大高度
	int minHeight; ///< 探测物最小高度
	//int referencePoint;//基准位置
} PeopleCloseAdvanceParam;


typedef struct
{
	int       objectMinWidth;            ///< 探测物最小宽度
	int       objectMinHeight;           ///< 探测物最小高度
	int       statictimeToAlarm;         ///< 最大静止时间
	int referencePoint;//基准位置
} GuardAdvanceParam;


typedef struct
{
	int objectMinWidth; ///< 探测物最小宽度
	int objectMinHeight; ///< 探测物最小高度
	int statictimeToAlarm; ///< 最大静止时间
	int referencePoint;//基准位置
} LingerAdvanceParam;


typedef struct
{
	int timeToStopAlarm; ///< 报警持续时间
	int frameInterval; ///< 更新率
	int referencePoint;//基准位置
} AtmAdvanceParam;

/*
高级参数-end
*/




//////////////////////////////////////////////////////////////////////////////////


struct CustomerParamsObjects_V3
{        
  int       maxObjwidth;  ///< 物体最大宽度(<=1920 pixel) 
  int       maxObjHeight; ///< 物体最大高度(<=1080 pixel) 
  int       minObjwidth;  ///< 物体最小宽度(>=36 pixel) 
  int       minObjHeight; ///< 物体最小高度(>=36 pixel) 
  int       timeToAlarm;  ///<物体停留超过该时间后报警，0无效 
  int       res;            ///< 保留字段 
}; 


typedef struct
{
	unsigned char type; ///< 智能类型
	unsigned char behavior; ///< 感兴趣的行为
	short res[13]; ///< 保留字段
	union
	{
		int customerParams[32];                           ///< 用户普通参数使用的内存
		IntrusionCustomerParam intrusionCusParam;         ///< 智能功能：周界保护
		DirectionCustomerParam directionCusParam;         ///< 智能功能：绊线检测
		UnattendedCustomerParam unattendedCusParam;       ///< 智能功能：遗留物分析
		RemoveCustomerParam removeCusParam;               ///< 智能功能：物品看护
		PeopleCountCustomerParam peopleCountCusParam;     ///< 智能功能：人数统计
		NoFaceDetecCustomerParam noFaceDetecCusParam;     ///< 智能功能：人脸检测
		FightCustomerParam fightCusParam;                 ///< 智能功能：斗殴检测
		PeopleCloseCustomerParam peopleCloseCusParam;     ///< 智能功能：人员接近
		GuardCustomerParam guardCusParam;                 ///< 智能功能：哨兵检测
		LingerCustomerParam lingerCusParam;               ///< 智能功能：人员滞留
		AtmCustomerParam atmCusParam;                     ///< 智能功能：ATM 看护
    CustomerParamsObjects_V3 objectsV3;               //IPC Xarina 智能新添 20131206
	} CustomerParams;

	int advancedParamEnable; ///< 是否开启高级参数

	union
	{
		int advancedParams[16]; ///< 高级参数使用的内存
		IntrusionAdvanceParam intrusionAdvanceParam;      ///< 智能功能：周界保护
		DirectionAdvanceParam directionAdvanceParam;      ///< 智能功能：绊线检测
		UnattendedAdvanceParam unattendedAdvanceParam;    ///< 智能功能：遗留物分析
		RemoveAdvanceParam removeAdvanceParam;            ///< 智能功能：物品看护
		PeopleCountAdvanceParam peopleCountAdvanceParam;  ///< 智能功能：人数统计
		NoFaceAdvanceParam noFaceAdvanceParam;            ///< 智能功能：人脸检测  //暂不支持
		FightAdvanceParam fightAdvanceParam;              ///< 智能功能：斗殴检测
		PeopleCloseAdvanceParam peopleCloseAdvanceParam;  ///< 智能功能：人员接近
		GuardAdvanceParam guardAdvanceParam;              ///< 智能功能：哨兵检测
		LingerAdvanceParam lingerAdvanceParam;            ///< 智能功能：人员滞留
		AtmAdvanceParam atmAdvanceParam;                  ///< 智能功能：ATM 看护
	} AdvanceParams;
} ConfigureRule;



typedef struct
{
	MotPoly spot; ///< 多边形区域信息
	unsigned int enable; ///< 本区域设置的规则是否启用
	unsigned int sensitivity; ///< 灵敏度
	unsigned int res[16]; ///< 保留字段
	ConfigureRule rule; ///< 规则信息
} ConfigureIAEvent;


typedef struct
{
	short version; ///< 智能信息格式的版本号，当前为1
	short res; ///< 保留字段
	MotDeep depth; ///< 摄像机标定参数
	unsigned int polyCount; ///< 本通道包含的区域个数(1-255) 目前 48个
	ConfigureIAEvent polyList[MAX_POLY_NUM]; ///< 区域规则表(polyCount 个)
} ConfigureIAFrame;


/*
 *接口参数*
 */

typedef struct 
{
	UCHAR start;  //是否启动 0:不启动 1:启动
	UCHAR bHour;  //开始时间的小时  0-23
	UCHAR eHour;    //结束时间 0-23
	UCHAR bMinute;  //开始的分0-59
	UCHAR eMinute;  //结束的分0-59
	UCHAR sensity;  //灵敏度1-9
	UCHAR rec[2];   //
}VcaTimeSeg;

//
/*
* 和算法相关的应用参数
*/
typedef struct
{
	unsigned short  endNum;     /**< 报警端子 */
	unsigned short  changeTime; /**< 换岗时间 */
}GuarderAppParam;


typedef struct 
{
	UCHAR vcaType;       /**< 符合IVSType,0表示全部 */
	UCHAR start;         /**< 是否启动，0：不启动，1：启动 */
	UCHAR playDisplay;   /**< 是否显示回放，0：不启动，1：启动 */
	UCHAR senceDisplay;  /**< 是否现场显示，0：不启动，1：启动 */
	UCHAR videoSize;     /**< 处理视频分辨率的大小，1：DCIF 2：CIF 
											3：1/2D1 4：D1 5：QCIF。暂时只支持CIF */
#if 1 
	/*
	* 目前只有哨兵管控应用参数，以后可能有所有的智能参数
	*/
	UCHAR res[3];

	union
	{
		int AppParam[1];                    /*<  算法应用参数大下，目前智能是4个字节，以后可扩展*/
		GuarderAppParam  guarderAppParam;   /**< 哨兵应用参数 */
	}AlgAppParam;
#endif
	VcaTimeSeg time[7][6];/**< 检测时间段，7天每天6个 */
}VcaAppParam;



typedef ConfigureIAFrame VcaAlgParam;


typedef struct 
{
	UCHAR count;                 //配置的算法类型个数
	UCHAR res[7];                // 保留
	VcaAppParam appParam[MAX_ALGEVENT_NUM]; // 应用参数
	VcaAlgParam algParam;        //算法参数
}VcaParam;

typedef struct  
{
	unsigned int channel;
	VcaParam vcaParams[MAX_CHANNEL_NUM];
}BscpVcaPara;

typedef struct
{
	unsigned long long channel;
	VcaParam vcaParams[64];
}BscpVcaParaEx;
//现有协议支持：一个通道对应4个区域(暂时)，每个区域对应多种智能算法


/** @struct CalibrationCheck
* 标定校验结构体
*/
typedef struct
{
	int channelID;
	MotDeep depth;
} CalibrationCheck;


struct VcaAppParamEx
{
  unsigned char   vca_type; //智能算法类型 
  unsigned char   res[15];  
  union
  { 
    unsigned char   vcaParam[64]; 
    IntrusionCustomerParamEx       insPara;             ///< 智能功能：周界 
    DirectionCustomerParamEx       directionCusParam;   ///< 智能功能：绊线检测 
    UnattendedCustomerParamEx      unattendedCusParam;  ///< 智能功能：遗留物分析 
    RemoveCustomerParamEx          removeCusParam;      ///< 智能功能：物品看护 
    PeopleCountCustomerParamEx     peopleCountCusParam; ///< 智能功能：人数统计 
    NoFaceDetecCustomerParamEx     noFaceDetecCusParam; ///< 智能功能：人脸检测 
    FightCustomerParamEx           fightCusParam;       ///< 智能功能：斗殴检测 
    PeopleCloseCustomerParamEx     peopleCloseCusParam; ///< 智能功能：人员接近 
    GuardCustParamEx           guardCusParam;       ///< 智能功能：哨兵检测 
    LingerCustomerParamEx          lingerCusParam;      ///< 智能功能：人员滞留 
    AtmCustomerParamEx             atmCusParam;         ///< 智能功能：ATM看护 
    CustomerParamsObjects_V3       objectsV3;               //IPC Xarina 智能新添 wubo 20131206
  }uIaPara;////智能参数容器,  参见附录4中ConfigureRule中union的各个参数类型； 

  VcaTimeSeg time[7][6]; //检测时间段。7天每天6 个 
};

struct VcaRule
{
  unsigned char vca_type;      //< 智能类型 
  unsigned char behavior;  ///<  感兴趣的行为 
  unsigned char res[2];     ///<  填充字段 
};

struct VcaPolyEvent
{
  unsigned char poly_type; ///多边形类型 
  unsigned char poly_info; ///如用作伴线标示伴线方向
  unsigned char res[2]; 
  MotPoly spot;    ///<  多边形区域信息 
  unsigned int vca_rule_num; ///< 本区域包含的智能功能个数 
  VcaRule rule[10];  ///< 规则信息 /// 有效个数取决于vca_rule_num 值； 
};

struct ScenceParam
{
  int       minObjWidth;                    ///< 输入参数, 最小目标宽度 
  int       minObjHeight;                  ///< 输入参数, 最小目标高度 
  int       statictimeToAlarm;               ///< 最大静止时间 
  int       dayStartTime;                    ///< 白天起始时间 
  int       dayEndTime;                      ///< 白天结束时间 
  int       shadowControl;                   ///< 阴影抑制标志 
  int       updateRate;                      ///< 背景更新率 
  int       noiseControl;                    ///< 图像噪声抑制标志 
  int       objDecSensitivity;                  ///< 目标检测灵敏度，（0, 1, 2）//默认是 2 
  int       objTraceSensitivity;             ///< 目标跟踪灵敏度 
  int       reciprocate;                     ///< 往复性扰动抑制 
  int       lightControl;                    ///< 光照变化抑制标志 
};

struct ScenceVcaCfg
{
  unsigned char vcamode ;    ///场景模式 0:SCENCE, 1:ATM 
  unsigned char scencetype;     ///场景类型 0:in door ,1: out side, 2:ATM Panel 
    unsigned char analysis_pattern;   ///分析模式0:Area , 1:behavior   
    unsigned char valid_polycount;    ///有效多边形个数 < 8 
    VcaPolyEvent polylist[8];    ///多边形 
    ScenceParam scencevca;       ///参见附录四 
};

struct MotFlat 
{ 
  MotRect   flatRect[2];       ///< 平面标定：标定框:2个 
}; 

struct CalibrationParam
{
  unsigned char calibration_type ; ///标定类型： ///0：DEEP 景深标定，1：FLAT平面标定 
  unsigned char res[3]; 
  union
  { 
    MotDeep deep ; ///景深标定
    MotFlat   flat;  ///平面标定
  }uMot;  
};

struct VcaCfgPara
{
  unsigned char enable; 
  unsigned char refPos;     ///基准位置 
  unsigned char vcacount;     /// 最大为 16 当前的有效智能类型个数，保持参数 
  unsigned char scence_num;   ///default == 4，见下表格 
  unsigned char enalbleScenceId;   /// scence[]中使能的参数 Id  
  unsigned char calibrate_id;     ///标定参数ID 
  unsigned char res[6]; 
  CalibrationParam calibrate_param[2];  ///标定参数 ///有效 ID  由 calibrate_id  决定； 
  VcaAppParamEx param[16];            //各智能类型的配置参数，有效个数由 vcacount指定 
  ScenceVcaCfg scence[6];               ///最多支持6种场景配置方式,具体个数由scence_num指定；  
};

struct VcaParamIpc
{
  unsigned int count;     ///通道个数； 
  VcaCfgPara cfgEx;       ///通道个数：IPC默认为 1，DVR为实际通道数，默认 16；按定长实现； 
};

struct VcaParamDvr
{
  unsigned int count;     ///通道个数； 
  VcaCfgPara cfgEx[16];   ///通道个数：IPC默认为 1，DVR为实际通道数，默认 16；按定长实现； 
};

//智能检测类别
typedef enum   
{   
  VCA_DECT_MIN = 0, //无效值,小值守护码(但在智能查询时表示全部智能类型)   
  VCA_VIDEO_TOTALNUM, //人数统计   
  VCA_VIDEO_LEFTOBJECT, //遗留物   
  VCA_VIDEO_STUMLINE, //拌线   
  VCA_VIDEO_PRIMITER, //周界保护   
  VCA_VIDEO_NONFACE, //非人脸检测   
  VCA_VIDEO_LOSSOBJECT, //丢失检测   
  VCA_VIDEO_SENTRY , //岗哨检测   
  VCA_VIDEO_CONDUCT,	//操行检测
  VCA_AUDIO_DETECT,  //音频报警   
  VCA_DECT_MAX       //无效值,大值守护码   
}VCA_ALGTYPE;   

//智能检测区域(注意:智能设置的各种区域边界范围为[(2,2), (350,286)],也就是说有2 像素 的边界)
typedef enum   
{   
  VCA_RECTWIDTH = 350,  //区域设置的坐标系X 最大值   
  VCA_RECTHEIGHT = 280, //区域设置的坐标系Y 最大值   
  VCA_RECTBORDERX = 2,  //水平方向的边界   
  VCA_RECTBORDERY = 2   //垂直方向的边界   
};

//点坐标
typedef struct tagBSCP_INTERF_POINT
{
  USHORT x;				//横坐标
  USHORT y;				//纵坐标
}BSCP_INTERF_POINT, *PBSCP_INTERF_POINT;
typedef BSCP_INTERF_POINT  VCA_POINT;
typedef PBSCP_INTERF_POINT  PVCA_POINT;

typedef struct tagVCA_LINE 
{   
  VCA_POINT begin; //起点   
  VCA_POINT end;   //终点   
}VCA_LINE, *PVCA_LINE;   

//针对非CIF格式需要自行进行大小比例映射(绘制时) 
typedef struct tagVCA_RECT   
{   
  VCA_POINT left_up;    //左上角坐标   
  VCA_POINT right_down; //右下角坐标   
}VCA_RECT, *PVCA_RECT;   

typedef struct tagVCA_QUAD   
{   
  VCA_POINT points[4]; //从左上角开始顺时针   
}VCA_QUAD, *PVCA_QUAD;   

//通用区域参数 
typedef struct tagVCA_SPOT   
{   
  UCHAR vertices;       //2-16 
  UCHAR enabled;        //YES(1)|NO(0) 
  UCHAR type;           //警戒区域(1) |  辅助区域(2) |  排除区域(3) 
  UCHAR concerns;       //关注的行为集(bitmap)，取值见《智能信息帧格式》 
  VCA_POINT points[16]; //从左上角开始顺时针   
}VCA_SPOT, *PVCA_SPOT;   

//通用行为参数 
typedef struct tagVCA_PARAMS   
{ 
  USHORT delay_time; //持续时间(秒)，0未设置 
  USHORT alarm_time; //报警重置时间(秒),10-600,默认为30，仅遗留物/丢失物有效 
  USHORT lo_limit; //下限(0-10)，0未设置, //仅人数统计有效 
  USHORT hi_limit; //上限(0-10)，0未设置，//仅人数统计有效 
  USHORT aux_port; //辅助输入端子(1-8)，0未设置，仅人脸监测和岗哨有效 
  USHORT objectMinWidth;         /* 周界-绊线-人数 */
  USHORT objectMinHeight;        /* 周界-绊线-人数 */
  USHORT statictimeToAlarm;       /* 周界-绊线-人数 */
  USHORT maxObjHeight;          /* 遗留-丢失 */
  USHORT minObjHeight;          /* 遗留-丢失 */
  unsigned char removeTreeSwayFlag;    /* 遗留-丢失 */
  unsigned char peopleDetecFlag;        /* 遗留-丢失 */
  unsigned char frameInterval;           /* 遗留-丢失 */
  unsigned char res[9];
} VCA_PARAMS, PVCA_PARAMS; 

//通用对象参数 
typedef struct tagVCA_OBJECT   
{ 
  USHORT object_height;       //标准探测物实际高度(单位: cm)   
  UCHAR res;   
  UCHAR object_depth;         //目标景深，标的物个数：3--岗哨,1--其他   
  VCA_RECT object_figures[3]; //标准探测物外形轮廓表   
} VCA_OBJECT, PVCA_OBJECT; 

typedef struct tagVCA_TIME   
{   
  UCHAR bMinute; //开始时间的分 0-59   
  UCHAR eMinute; //结束时间的分 0-59   
  UCHAR bHour;   //开始时间的小时 0-23   
  UCHAR eHour;   //结束时间的小时 0-23   
  UCHAR sensitivity; //对象灵敏度0-9,默认值自动0(前景与背景的差异)   
  UCHAR tolerance;   //对象宽容度0-9,默认值5(对象尺寸变化百分比%)   
  UCHAR rev;         //保留   
  UCHAR start;       //是否启动 0:不启动  1:启动   
}VCA_TIME, *PVCA_TIME;   

//某一类型分析算法   
typedef struct tagVCA_ALG   
{   
  UCHAR enabled;    //YES(1)|NO(0) 
  UCHAR algType;    //算法类型，取值VCA_ALGTYPE 
  UCHAR res[2];    //保留； 
  VCA_PARAMS defined_params;    //行为的设置 
  VCA_OBJECT    defined_object; //对象的设置 
  VCA_SPOT defined_spots[4];    //区域的设置   
}VCA_ALG, *PVCA_ALG;   

typedef struct tagVCA_NONFACE   
{   
  VCA_PARAMS defined_params;     //行为的设置 
  VCA_OBJECT    defined_object;  //对象的设置 
  VCA_SPOT defined_spots[4];     //区域的设置   
}VCA_NONFACE, *PVCA_NONFACE;   

//周界保护阈值定义   
#define VCA_PERIMTER_AREA_NUM (4) //周界区域最大数   
#define VCA_STUM_LINE_NUM (4) //绊线最大数   
#define VCA_LEFT_AREA_NUM (4) //遗留物最大数   
#define VCA_LOSS_AREA_NUM (4) //丢失物最大数   
#define VCA_SENTRY_AREA_NUM (1) //岗哨区域最大数  

//周界保护   
typedef struct tagVCA_PERIMETER   
{   
  VCA_PARAMS defined_params;    //行为的设置 
  VCA_OBJECT    defined_object; //对象的设置 
  VCA_SPOT defined_spots[4];    //区域的设置   
}VCA_PERIMETER, *PVCA_PERIMETER;   

//绊线组设置   
typedef struct tagVCA_STUMLINE   
{   
  VCA_PARAMS   defined_params;   //行为的设置 
  VCA_OBJECT    defined_object;  //标准探测物设置 
  VCA_SPOT defined_spots[4];     //拌线设置   
}VCA_STUMLINE, *PVCA_STUMLINE;  

//人数统计   
typedef struct tagVCA_TOTALPEPSONNUM   
{   
  VCA_PARAMS   defined_params;  //行为的设置 
  VCA_OBJECT    defined_object; //标准探测物设置 
  VCA_SPOT defined_spots[4];    //技术区域设置   
}VCA_TOTALPEPSONNUM, *PVCA_TOTALPEPSONNUM;   

//遗留物检测   
typedef struct tagVCA_LEFTOBJECT   
{   
  VCA_PARAMS defined_params;    //行为的设置 
  VCA_OBJECT    defined_object; //标准探测物设置 
  VCA_SPOT defined_spots[4];    //技术区域设置   
}VCA_LEFTOBJECT, *PVCA_LEFTOBJECT;   

//丢失物品检测   
typedef struct tagVCA_LOSSOBJECT   
{   
  VCA_PARAMS defined_params; //行为的设置 
  VCA_OBJECT    defined_object; //标准探测物设置 
  VCA_SPOT defined_spots[4]; //技术区域设置   
}VCA_LOSSOBJECT, *PVCA_LOSSOBJECT;   

//岗哨   
typedef struct tagVCA_SENTRY   
{   
  VCA_PARAMS defined_params; //行为的设置 
  VCA_OBJECT    defined_object; //标准探测物设置 
  VCA_SPOT defined_spots[4]; //区域设置,包括警戒区[0]/岗哨区[1]/排除区[2-3]   
}VCA_SENTRYOBJECT, *PVCA_SENTRYOBJECT; 

//某一通道的智能分析总结构 
typedef struct tagVCA_CFG   
{   
  UCHAR start; //是否启动   
  UCHAR playDisplay; //是否回放显示,0-不启动,1-启动   
  UCHAR sceneDisplay; //是否现场显示,0-不启动,1-启动   
  UCHAR videoSize; //处理视频的分辨率大小,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,暂时只支持CIF   
  UINT rev; //保留   
  VCA_TIME time[7][4]; //检测时间段,一周7天,0-7表示周日,周一...到周六   
  //具体算法 
  VCA_ALG alg[8];    //每通道最多可启用4 种不同类型算法,通过 alg[i].enabled判断 
  //当前仅使用[0]   
}VCA_CFG, *PVCA_CFG; 

typedef struct tagVCA_ALGDECTVIDEOPARA   
{   
  char channel; //通道编号：1-16 
  char res[3];    // 
  VCA_CFG vcaPara;
}VCA_ALGDECTVIDEOPARA, *PVCA_ALGDECTVIDEOPARA;

typedef struct tagBSRALGDECT_LINE   
{   
  BSCP_INTERF_POINT begin; //起点   
  BSCP_INTERF_POINT end; //终点   
}BSRALGDECT_LINE, *PBSRALGDECT_LINE;

//单条拌线设置
typedef struct tagBSRALGDECT_PER_STUMLINE_EX
{
  BSRALGDECT_LINE object_area; //对象区域
  INT direction;     //方向,1-向外, 2-向内, 0-表示无效
}BSRALGDECT_PER_STUMLINE_EX, *PBSRALGDECT_PER_STUMLINE_EX;

#define MAX_STDOBJ_NUM 3 //最大检测物个数

//针对非CIF格式需要自行进行大小比例映射
typedef struct tagBSRALGDECT_RECT
{
  VCA_POINT left_up;		//左上角坐标
  VCA_POINT right_down;	//右下角坐标
}BSRALGDECT_RECT, *PBSRALGDECT_RECT;

typedef struct tagBSRALGDECT_OBJECT
{
  USHORT object_height; //标准探测物实际高度(单位: cm)  
  UCHAR res; 
  UCHAR object_depth; //目标景深，1 表示仅 object_size 有效，   
  BSRALGDECT_RECT object_figures[MAX_STDOBJ_NUM]; //标准探测物大小   
}BSRALGDECT_OBJECT, *PBSRALGDECT_OBJECT;

//绊线组设置
typedef struct tagBSRALGDECT_STUMLINE_EX
{     
  BSRALGDECT_OBJECT  defined_size;//标准探测物设置
  BSRALGDECT_PER_STUMLINE_EX Defined_area[BSRALGDECT_STUM_LINE_NUM]; //拌线设置
}BSRALGDECT_STUMLINE_EX, *PBSRALGDECT_STUMLINE_EX;



//关于智能  qinyf  2011.5.24
/************************************************************************/
/*     本版本智能分为两个部分：应用部分App 智能参数部分Alg 、
1    与dvr的通信，一次把16个通道的智能信息全部获取
2    每个通道的智能信息包含上面说的两部分 app和alg，
     app里存储了全部10种智能信息的应用情况 比如哪个启用等
		 alg部分存储了这个通道的智能参数，这里智能参数以区域为载体来区分，
		 最大支持48个区域，每个区域指定为不同的智能所属，包含不同的智能参数信息*/
/************************************************************************/

//*******************************************************/
//IPC Xarina 需求新添智能协议,目前应用于“人脸检测”和“运动检测”。 20131206 wubo

struct GetVcaParamIn_V3
{
  UCHAR   vcaclass;   // 应用算法的分类：1：人脸检测，2.运动检测 
  UCHAR  res[3]; 
};

typedef struct 
{
  UCHAR enable;       //是否使能； 
  UCHAR count;        //配置的有效智能类型个数； 
  UCHAR vcaclass ;    // 应用算法的分类：1：人脸检测，2.运动检测 
  UCHAR valid_polycount;   //有效多边形个数  < 16 
  UCHAR res[8];                  //保留 
  VcaAppParamEx appParam[16];   //应用参数 
  VcaPolyEvent polyList[16];      //多边形 
}VcaParam_V3;

typedef struct
{
  unsigned int channel;
  VcaParam_V3 vcaParams[16];
}BscpVcaParaEx_V3;
//IPC Xarina 需求新添智能协议 20131206 wubo
/*******************************************************/

struct TrapwirePoint
{
	int nCount;
	trapwriePoint trapwirePt[16];
};

#endif /* _BSTARPARAM_H_ */