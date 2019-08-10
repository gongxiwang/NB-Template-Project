/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_lora.h
  * @author  walle
  * @version  V1.0
  * @date  20/05/2019
  * @brief  app_lora的头文件
  */
#ifndef __APP_LORA__H
#define __APP_LORA__H

/**
  * @brief  头文件
 */
#include "bsp.h"
#include "bsp_nbiot.h"

/**
  * @brief  LORA模块处理流程
 */
typedef enum 
{
	EN_LORAPROCESS_INIT,		/* 流程初始化 */
	EN_LORAPROCESS_OPEN,		/* 流程打开 */
	EN_LORAPROCESS_HANDLE,		/* 流程处理 */
	EN_LORAPROCESS_WIITRX,		/* 流程等待 */
	EN_LORAPROCESS_IDLE,		/* 流程空闲 */
}EN_LORA_PROCESS;

/**
  * @brief  LORA模块打开流程
 */
typedef enum
{
	EN_LORAOPEN_IDLE,			/* 打开状态空闲 			*/
	EN_LORAOPEN_ADD,			/* 打开状态AT测试 		*/	
	EN_LORAOPEN_REBOOT,			/* 打开状态重启 */
	EN_LORAOPEN_CHECKREBOOT,	/* 打开状态重启成功检查 */
	EN_LORAOPEN_MOD,			/* 打开状态设置工作模式 */
	EN_LORAOPEN_CLA,			/* 打开状态设置CLASS类 		*/
	EN_LORAOPEN_JON,			/* 打开状态启动加网 */
	EN_LORAOPEN_RJN,			/* 打开状态重新启动加网 */
	EN_LORAOPEN_END,			/* 打开状态结束 */
	EN_LORAOPEN_ERROR,			/* 打开错误 */
}EN_LORAOPEN_OPERATE;

/**
  * @brief  LORA模块操作结构体
 */
typedef struct 
{
	char*					CurCMD;
	u8 						ExecuCount;			/* 命令执行次数 */
	u8						LastOperateState;	/* 上次执行的状态 */
	u8						CurProcState;		/* 当前执行的处理状态 */
	u8						NextProcState;		/* 下次执行的处理状态 */
	u8						CurOperateState;	/* 下次执行的操作状态 */
	u8 						NextOperateState;	/* 当前执行的操作状态 */
	u32	  					WAIT_TIME;
	u8						LORATick_Flag;		/* LORA定时上报标志 */
	u8	  					ERRORCODE;
	u8*						ExpectRcv;
	u8*						ErrorRcv;
}ST_LORA_OPERAION, *PST_LORA_OPERATION;


/**
  * @brief  LORA模块操作回应错误码
 */
typedef enum
{
	LORA_ERROR_NORESPONSE 	= 0xF0,
	LORA_ERROR_TIMEOUT 	= 0xFF,


}EN_LORA_ERRORCODE;

/**
  * @brief  LORA模块操作回应码
 */
typedef enum
{
	LORA_RESPONSE_SUCCESS 	= 0xA0,
	LORA_RESPONSE_FAIL 	= 0xA1,
	LORA_OPEN_SUCCESS,
}EN_LORA_RESPONSECODE;


void App_LORA_TASK(void);
u8 LORA_send_cmd(char* ucCMD, u16 waittime);
static u8 LORA_MODULE_WAITRX(PST_LORA_OPERATION pst_lora_operation);
static void SplitLORAData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet);
void SendLORAMsgPack(P_Device_packet_t p_device_packet);
static void AnalysisLoRaMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet);
#endif
