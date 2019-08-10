/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2传感器应用函数
  */
#include "bsp.h"
#include "app_templg.h"
#include "bsp_modbus.h"
#include "bsp_uart.h"


/***************************************************
寄存器地址 PLC或组态地址 内容 操作
0000 H 40001 湿度 只读
0001 H 40002 温度 只读
0002 H 40003 光照度
（只在0~200000Lux
启用，单位1Lux）
只读
0003 H 40004
0006 H 40007 光照度
(0~65535单位1Lux
0~200000单位百Lux)
只读
***************************************************/


static ST_TEMPLG_CTL st_Templg_Ctl = {0};			/* CO2控制结构体 		*/
static ST_ModBusPack st_ModBusPack = {0};			/* ModBus结构体 		*/

/**
  * @brief 读取传感器CO2值
 */
void ReadTemplgSensorValue(u8 ValueType)
{
	u8 TempData[16] = {0};					/* 数据缓冲 */
	u32 timeout = st_Templg_Ctl.timeout;	/* 等待超时时间 */
	if(EN_TYPE_LIGHT == ValueType)			/* 光照 */
	{
		/************************************************************
		问询帧
		地址码 功能码 起始地址 数据长度 校验码低位 校验码高位
		0x01 0x03 0x00 0x06 0x00 0x01 0x64 0x0B
		*************************************************************/
		st_Templg_Ctl.TemplgType = EN_TYPE_LIGHT;
		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x06;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x01;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* 组包发送 */

		/* 等待接收数据 */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, TmeplgSensorRecvLen) == 0x00)	/* 等待接收数据 */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* 需要延迟10ms */	
		}

	}
	else if(EN_TYPE_TEPHUM == ValueType)	/* 温湿度 */
	{
		/************************************************************
		 读取设备地址 0x01 的温湿度值
		问询帧
		地址码 功能码 起始地址 数据长度 校验码低位 校验码高位
		0x01 0x03 0x00 0x00 0x00 0x02 0xC4 0x0B
		*************************************************************/
		st_Templg_Ctl.TemplgType = EN_TYPE_TEPHUM;
		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x00;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x02;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* 组包发送 */

		/* 等待接收数据 */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, TmeplgSensorRecvLen) == 0x00)	/* 等待接收数据 */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* 需要延迟10ms */	
		}
	}

	/* 判断是否是超时退出的循环 */
	if(timeout == 0)
	{
		st_Templg_Ctl.TryCount ++;
	}
	else
	{
		ModBusBufToPack(&st_ModBusPack, TempData);					/* 把数据赋值到PACK */
		CalculaTemplgSensorValue(&st_Templg_Ctl, st_ModBusPack.data, st_ModBusPack.datalen);/* 得出三合一的值 */
	}

	
}

/**
  * @brief CO2传感器初始化
 */
void InitTemplgSensor(void)
{
	st_Templg_Ctl.timeout = 5000;								/* 5秒左右 */
	st_Templg_Ctl.TryCount = 5;								/* 尝试发送5次 */
	st_Templg_Ctl.Sensor_addr = TemplgSensorAddr;					/* 地址赋值 */
	st_ModBusPack.address = st_Templg_Ctl.Sensor_addr;			/* ModBus设备地址赋值 */
	st_ModBusPack.function = TEMPLGFUNCODE;						/* 功能码 */
	OPENRS_485;												/* 打开RS485通信 */
}

/**
  * @brief 解析TemplgSensorValue
 */
u16 CalculaTemplgSensorValue(PST_TEMPLG_CTL pst_templgCtl, u8* TemplgData, u32 len) 
{
	if(len > 0)
	{
		switch(pst_templgCtl->TemplgType)
		{
			case EN_TYPE_LIGHT:				/* 光照 */
				pst_templgCtl->LightValue = ((TemplgData[0] << 8) | TemplgData[1]);
				break;
			case EN_TYPE_TEPHUM:			/* 温湿度 */
				pst_templgCtl->HumValue = ((TemplgData[0] << 8) | TemplgData[1]);
				pst_templgCtl->TempValue = ((TemplgData[2] << 8) | TemplgData[3]);
				break;
			default:
				break;
		}
	}
	return 0;
}
