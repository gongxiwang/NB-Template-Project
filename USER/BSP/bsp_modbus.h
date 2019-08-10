/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_modbus.h
  * @author  walle
  * @version  V1.0.0
  * @date  08/07/2019
  * @brief  modbus驱动
  */ 
#ifndef __BSP_MODBUS__H
#define __BSP_MODBUS__H

#include "bsp.h"

#define M_MAX_FRAME_LENGTH	256		/* 最大帧长度为261字节 地址+功能+帧号+数据长度+数据+CRC(CRC16有2字节) */
#define M_MIN_FRAME_LENGTH	5		/* 最小帧长度为5字节 地址+功能+帧号+数据长度+CRC8/XOR/SUM */
#define FRAM_DATA_LEN		16		/* 帧数据长度 */



/**
  * @brief 返回结果：错误类型定义
*/
typedef enum
{
	MR_OK=0,						/* 正常 */
	MR_FRAME_FORMAT_ERR = 1,		/* 帧格式错误 */	 
	MR_FRAME_CHECK_ERR = 2,			/* 校验值错位 */ 
	MR_MEMORY_ERR = 3,				/* 内存错误 */ 
}EN_MODBUS_RESULT;


/**
  * @brief MODBUF帧结构体
*/
typedef struct
{ 
	u8 address;						/* 设备地址：0，广播地址；1~255，设备地址 */
	u8 function;					/* 帧功能，0~255 */
	u8 count;						/* 帧编号 */
	u8 datalen;						/* 有效数据长度 */
	u8 data[FRAM_DATA_LEN];			/* 数据存储区 */
	u16 chkval;						/* 校验值  */
}ST_ModBusPack, *PST_ModBusPack;



/**
  * @brief 函数声明
*/
u16 CRC16_Modbus(u8*_pBuf, u16 _usLen);
void ModBusSendPack(u8* sendbuf, u8 len);
void ModBusTxPack(u8* modbus_TxBuffer, PST_ModBusPack pst_ModBusPack);
EN_MODBUS_RESULT ModBusBufToPack(PST_ModBusPack pst_ModBusPack, u8* modbus_RxBuffer);
#endif
