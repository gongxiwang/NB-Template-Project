/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_nbiot.h
  * @author  walle
  * @version  V1.0
  * @date  20/05/2019
  * @brief  app_nbiot的头文件
  */
#ifndef __NB_IOT__H
#define __NB_IOT__H


/**
  * @brief  头文件
 */
#include "bsp.h"
#include "bsp_nbiot.h"


/**
  * @brief  NB数据协议
 */
typedef struct
{
    u8 		Starting_mark;                     	/* 起始标志 */
    u16 	Message_number;                   	/* 消息序列号 */
    u8 		Node_type;                 			/* 节点类型 */
    u8 		Device_type;                        /* 设备类型     */
    u8 		Command_code;						/* 命令码     */
    u8 		ack;                   				/* 应答码 */
    u8  	data_size;  						/* 数据长度     */
	u8		data[64];							/* 数据 */
	u8 		Check_code;							/* 校验码 */
	u8 		End_mark;							/* 结束标志 */
}Device_packet_t, *P_Device_packet_t;


/**
  * @brief  NB模块处理流程
 */
typedef enum 
{
	EN_NBPROCESS_INIT,		/* 流程初始化 */
	EN_NBPROCESS_OPEN,		/* 流程打开 */
	EN_NBPROCESS_HANDLE,	/* 流程处理 */
	EN_NBPROCESS_WIITRX,	/* 流程等待 */
	EN_NBPROCESS_IDLE,		/* 流程空闲 */
}EN_NB_PROCESS;

/**
  * @brief  NB模块打开流程
 */
typedef enum
{
	EN_NBOPEN_IDLE,			/* 打开状态空闲 			*/
	EN_NBOPEN_REBOOT,		/* 打开状态重启 */
	EN_NBOPEN_NCFG,			/* 打开状态手动入网 */
	EN_NBOPEN_AT,			/* 打开状态AT测试 		*/
	EN_NBOPEN_ATE0,			/* 打开状态ATE0测试 */
	EN_NBOPEN_CMEE,			/* 打开状态CMEE测试 */
	EN_NBOPEN_CGSN,			/* 打开状态CGSN测试 */
	EN_NBOPEN_CGMI,			/*  */
	EN_NBOPEN_CGMM,			/*  */
	EN_NBOPEN_CIMI,			/* 打开状态CIMI测试 */
	EN_NBOPEN_CEREG1,		/* 打开状态使能网络注册CEREG测试 */
	EN_NBOPEN_IMEI,			/* 打开状态获取IMEI码测试 */
	EN_NBOPEN_CFUN0,		/* 打开状态关闭模块功能 */
	EN_NBOPEN_CFUN1,		/* 打开状态关闭模块功能 */
	EN_NBOPEN_CFGIP,		/* 打开状态配置IP */
	EN_NBOPEN_CPSMS0,		/* 关闭默认的睡眠设置 */
	EN_NBOPEN_eDRX0,		/* 关闭eDRX */
	EN_NBOPEN_NNMI1,		/* 打开状态打开自动接收信息 */
	EN_NBOPEN_NSMI1,		/* 打开状态打开发送开关信息 */
	EN_NBOPEN_CGATT1,		/* 打开网络附着 */
	EN_NBOPEN_CGATTIF,		/* 查询网络附着 */
	EN_NBOPEN_NRB,			/* 模块重启 */
	EN_NBOPEN_END,			/* 打开状态结束测试 */
	EN_NBOPEN_ERROR,		/* 打开状态错误 */
}EN_NBOPEN_OPERATE;

/**
  * @brief  NBIOT模块操作结构体
 */
typedef struct 
{
	u8* 					CurCMD;
	u8 						ExecuCount;			/* 命令执行次数 			  */
	u8						LastOperateState;	/* 上次执行的状态 */
	u8						CurProcState;		/* 当前执行的处理状态 */
	u8						NextProcState;		/* 下次执行的处理状态 */
	u8						CurOperateState;	/* 下次执行的操作状态 */
	u8 						NextOperateState;	/* 当前执行的操作状态 */
	u32	  					WAIT_TIME;
	u8						NBTick_Flag;		/* NB_IOT定时上报标志 */
	u8	  					ERRORCODE;
	u8*						ExpectRcv;
	u8*						ErrorRcv;
}ST_NB_OPERAION, *PST_NB_OPERATION;


/**
  * @brief  NBIOT模块操作回应错误码
 */
typedef enum
{
	NBIOT_ERROR_NORESPONSE 	= 0xF0,
	NBIOT_ERROR_TIMEOUT 	= 0xFF,


}EN_NBIOT_ERRORCODE;

/**
  * @brief  NBIOT模块操作回应码
 */
typedef enum
{
	NBIOT_RESPONSE_SUCCESS 	= 0xA0,
	NBIOT_RESPONSE_FAIL 	= 0xA1,
	NBIOT_OPEN_SUCCESS,

}EN_NBIOT_RESPONSECODE;


void App_NBIOT_TASK(void);
u8 NBIOT_send_cmd(char* ucCMD, u16 waittime);
u8 NBIOT_MODULE_PROCESS(PST_NB_OPERATION pst_nb_operation);
u8 NBIOT_MODULE_OPEN(PST_NB_OPERATION pst_nb_operation);
static u8 NBIOT_MODULE_WAITRX(PST_NB_OPERATION pst_nb_operation);
static void SetNB_OperatStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState);
static void SetNB_ProceStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState);
static u8 NBIOT_MODULE_INIT(PST_NB_OPERATION pst_nb_operation);
static void AnalysisCoapMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet);
static u8 NBIOT_MODULE_HANDLE(PST_NB_OPERATION pst_nb_operation);
void NBSystickTmr_callback(OS_TMR *ptmr, void *p_arg);
void GetNBMsgFrQue(P_Device_packet_t p_device_packet);
void SendNBMsgPack(P_Device_packet_t p_device_packet);
static unsigned long GetCoapMsgDataLen(char *coapMsg);
static void SplitCoapData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet);
void GropPackNBMsgSend(u8* Data, u32 DataLen, u8 ack);


#endif
