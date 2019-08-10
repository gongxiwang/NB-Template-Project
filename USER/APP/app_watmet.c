/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  app_watmet.c
  * @author  walle
  * @version  V1.0.0
  * @date  08/06/2019
  * @brief  水表传感器接口
  */ 


#include "bsp.h"
#include "app_modbus.h"
#include "bsp_modbus.h"
#include "app_uart.h"
#include "app_modbus.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "bsp_dma.h"
#include "app_nbiot.h"
#include "app_watmet.h"


#define READSTAR_ADDR 0x0000									/* 读寄存器起始地址 */
#define GATESTAR_ADDR 0x0001									/* 开关阀门控制的起始地址 */
#define WATMET_ADDRESS 0x69 									/* 宏定义水表的地址 */
static ST_WATMET_PACK st_ModuleBusPack_Watmet = {0};			/* 水表ModuleBusPackWatmet协议数据结构体 */


/**
  * @brief  水表初始化
 */
void Watmet_Init(void)
{
	u8 WatMetRcv = 0;
	OPENRS_485;
	/* 打开阀门 */
	CreatModBusPackWithSend(&st_ModuleBusPack_Watmet, EN_FUNCODE_CTLGATE, GATESTAR_ADDR, EN_VALUE_OFF);
	WatMetRcv = WatmetRecv(&st_ModuleBusPack_Watmet);
	if(WatMetRcv == EN_VALUE_ON)
	{
		debug_printf("Watmet Init Success\r\n");
	}
}

/**
  * @brief  水表用水量
 */
void Watmet_Read(void)
{
	u8 WatMetRcv = 0;
	u8 WatmetData[4] = {0};
	/* 读用水量 */
	CreatModBusPackWithSend(&st_ModuleBusPack_Watmet, EN_FUNCODE_READREG, READSTAR_ADDR, EN_USED_WATER);
	WatMetRcv = WatmetRecv(&st_ModuleBusPack_Watmet);
	if(WatMetRcv == 0x04)
	{
		WatmetData[0] = st_ModuleBusPack_Watmet.ucStarAdd_H;
		WatmetData[1] = st_ModuleBusPack_Watmet.ucStarAdd_L;
		WatmetData[2] = st_ModuleBusPack_Watmet.ucRegData_H;
		WatmetData[3] = st_ModuleBusPack_Watmet.ucRegData_L;
	}
	/* 发送写卡数据到NBIOT平台 */;
	GropPackNBMsgSend(WatmetData, WatMetRcv, 0x00);
}

/**
  * @brief  水表回应
 */
u8 WatmetRecv(PST_WATMET_PACK pst_ModBusPack_Watmet)
{
	static u8 Timeout = 200;	
	u8 UsartSensorData[100] = {0};								/* 传感器接收串口数据数组 */
	u32 ulReceiveLen = 0;										/* 保存的接收数据长度 */
	while(Timeout --)
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData);		/* 接收串口1传过来的数据 */
		if(ulReceiveLen > 0)	/* 如果接收到数据 */
		{
			return WatmetRecvAnalysis(pst_ModBusPack_Watmet, UsartSensorData);
		}
	}
	return -1;
}

/**
  * @brief  水表接收数据解析
 */
u8 WatmetRecvAnalysis(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 *RecvData)
{
	pst_ModBusPack_Watmet->ucADDRCODE = RecvData[0];
	pst_ModBusPack_Watmet->ucFUNCODE = RecvData[1];
	if(pst_ModBusPack_Watmet->ucFUNCODE == EN_FUNCODE_CTLGATE)/* 如果返回的是控制阀门命令 */
	{
		pst_ModBusPack_Watmet->ucStarAdd_H = RecvData[2];
		pst_ModBusPack_Watmet->ucStarAdd_L = RecvData[3];
		pst_ModBusPack_Watmet->ucRegData_H = RecvData[4];
		pst_ModBusPack_Watmet->ucRegData_L = RecvData[5];
		return pst_ModBusPack_Watmet->ucRegData_L;			/* 返回 */
	}
	else
	{
		pst_ModBusPack_Watmet->ucDataSize = RecvData[2];
		pst_ModBusPack_Watmet->ucStarAdd_H = RecvData[3];
		pst_ModBusPack_Watmet->ucStarAdd_L = RecvData[4];
		pst_ModBusPack_Watmet->ucWrittingData_H = RecvData[5];
		pst_ModBusPack_Watmet->ucWrittingData_L = RecvData[6];
		return pst_ModBusPack_Watmet->ucDataSize;
	}
}


/**
  * @brief 组包
 */
void CreatModBusPackWithSend(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 FuCode, u16 StartAdr, u16 RegData)
{
	u8 ModBusSendBuf[0x08] = {0};
	u16 ModBusCRC = 0;
	pst_ModBusPack_Watmet->ucADDRCODE 	= WATMET_ADDRESS;			/* 地址采用测试水表使用0x69 */
	pst_ModBusPack_Watmet->ucFUNCODE 	= FuCode;					/* 功能码 */
	pst_ModBusPack_Watmet->ucStarAdd_H 	= (StartAdr & 0xF0) >> 8;
	pst_ModBusPack_Watmet->ucStarAdd_L 	= StartAdr & 0x0F;
	pst_ModBusPack_Watmet->ucRegData_H 	= (RegData & 0xF0) >> 8;
	pst_ModBusPack_Watmet->ucRegData_L 	= RegData & 0x0F;
	
	ModBusSendBuf[0] = pst_ModBusPack_Watmet->ucADDRCODE;
	ModBusSendBuf[1] = pst_ModBusPack_Watmet->ucFUNCODE;
	ModBusSendBuf[2] = pst_ModBusPack_Watmet->ucStarAdd_H;
	ModBusSendBuf[3] = pst_ModBusPack_Watmet->ucStarAdd_L;
	ModBusSendBuf[4] = pst_ModBusPack_Watmet->ucRegData_H;
	ModBusSendBuf[5] = pst_ModBusPack_Watmet->ucRegData_L;

	ModBusCRC = CRC16_Modbus(ModBusSendBuf, 0x06);				/* 计算CRC校验码 */
	ModBusSendBuf[6] = ModBusCRC >> 8;
	ModBusSendBuf[7] = ModBusCRC;

	ModBusSendPack(ModBusSendBuf, 0x08);						/* 发送数据包 */
}


