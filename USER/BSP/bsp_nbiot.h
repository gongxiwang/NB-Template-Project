#ifndef __BSP_NBIOT__H
#define __BSP_NBIOT__H

#include "app_nbiot.h"
#include "bsp.h"


/**
  * @brief 定义接收消息类型
 */
typedef enum
{
  MSG_NONE,
  MSG_INIT,
  MSG_IMSI,
  MSG_MODULE_INFO,
  MSG_MID,     //制造商ID
  MSG_MMODEL,  //厂商型号
  MSG_MREV,    //厂家版本号
  MSG_BAND,    //工作频段
  MSG_IMEI,    //移动设备身份码
  MSG_SIGN,    //信号强度
  MSG_REG,
  MSG_UDP_CREATE,
  MSG_UDP_CLOSE,
  MSG_UDP_SEND,
  MSG_UDP_RECE,
  MSG_COAP,
  MSG_COAP_SEND,
  MSG_COAP_RECE,
  MSG_END
}msg_types_t;

/**
  * @brief undefine function
 */
typedef int (*NB_ReceCB)(msg_types_t, int, char*);

/**
  * @brief undefine function
 */
typedef int (*NB_Log)(char*, int);

/**
  * @brief NBIOT句柄
 */
typedef struct NB_Config *NB_Handle;

/**
  * @brief 对NB模块操作进行前期的初始化操作
 */
typedef int (*NB_Open)(NB_Handle);

/**
  * @brief 对NB模块操作的占用的资源进行释放
 */
typedef int (*NB_Close)(NB_Handle);

/**
  * @brief 自动发送相关初始化指令
 */
typedef int (*NB_AutoInitModule)(NB_Handle);

/**
  * @brief 获取NB模块的信息。比如：Manufacturer ID, Device Module,Firmware Version,Freqenucy band
 */
typedef int (*NB_ModuleInfo)(NB_Handle);

/**
  * @brief 获取NB模块的网络注册信息
 */
typedef int (*NB_NetRegisterInfo)(NB_Handle);

/**
  * @brief 获取NB模块SIM卡的信息
 */
typedef const char* (*NB_USIMInfo)(NB_Handle);

/**
  * @brief 获取NB模块信号强度
 */
typedef int (*NB_NetSign)(NB_Handle);

/**
  * @brief 定义NB模块通信上下文
 */
typedef int (*NB_DefPDP)(NB_Handle);

/**
  * @brief 失能NB模块上下文信息
 */
typedef int (*NB_DeactPDP)(NB_Handle);

/**
  * @brief 创建UDP
 */
typedef int (*NB_CreateUdp)(NB_Handle);

/**
  * @brief 关闭UDP
 */
typedef int (*NB_CloseUdp)(NB_Handle);

/**
  * @brief 发送UDP数据
 */
typedef int (*NB_SendUdpData)(NB_Handle,int,char*);

/**
  * @brief CoAP服务
 */
typedef int (*NB_CoAPServer)(NB_Handle, bool,char*);

/**
  * @brief CoAP发送提示方式设置
 */
typedef int (*NB_CoAPSentIndication)(NB_Handle,int code);

/**
  * @brief CoAP接收数据方式设置
 */
typedef int (*NB_CoAPReceMode)(NB_Handle,int);

/**
  * @brief CoAP协议信息发送
 */
typedef int (*NB_CoAPSentMsg)(NB_Handle,int,char*);

/**
  * @brief NB Modle reset cmd
 */
typedef int (*NB_Reset)(NB_Handle);

/**
  * @brief NB ping function
 */
typedef int (*NB_Ping)(NB_Handle);

/**
  * @brief 判断接收数据是不是Udp
 */
typedef int (*NB_isUdpReceData)(NB_Handle, int, char*);

/**
  * @brief 判断接收数据是否为CoAP数据
 */
typedef int (*NB_IsCoAPReceData)(NB_Handle,int,char*);

/**
  * @brief 接收UDP数据
 */
typedef int (*NB_ReceUdpData)(NB_Handle);

/**
  * @brief CoAP协议信息接收
 */
typedef int (*NB_CoAPReceMsg)(NB_Handle);

/**
  * @brief NB主线程
 */
typedef int (*NB_MainThread)(NB_Handle);

/**
  * @brief 定义NBIOT模块必须要实现的功能函数
 */
typedef struct
{
  NB_Open                 openFxn;
  NB_AutoInitModule       nbModuleInit;
  NB_ModuleInfo           getModuleInfo;
  NB_NetRegisterInfo      getModuleRegisterInfo;
  NB_USIMInfo             getUSIMinfo;
  NB_NetSign              getSign;
  NB_CreateUdp            createUdp;
  NB_CloseUdp             closeUdp;
  NB_SendUdpData          sendUdp;
  NB_CoAPServer           coapServer;
  NB_CoAPSentIndication   coapSentIndication;
  NB_CoAPReceMode         coapSetReceMode;
  NB_CoAPSentMsg          coapSentMsg;
  NB_MainThread           mainThread;
}NB_FxnTable;


/**
  * @brief NBIOT模块的功能结构数据
 */
typedef struct NB_Config
{
  NB_FxnTable        *fxnTablePtr;	  /* NB模块需要实现的函数 */
  void               *object;         /* NB uart对像指针 */
  NB_ReceCB          AppReceCB;
  NB_Log             log;
}NB_Config;

u16 NB_HexStrToNum(char* str);
u32 NB_Strtoul(const char* pStr,int base);
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen);



#endif
