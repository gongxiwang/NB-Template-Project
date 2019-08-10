/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2传感器应用函数
  */
#include "bsp.h"
#include "app_CO2.h"
#include "bsp_modbus.h"
#include "bsp_uart.h"


/***************************************************
寄存器地址 内容 操作 范围及定义说明
0000H  湿度值 只读 0~1000
0001H  温度值 只读 -400~1000
0002H  CO2浓度值 只读 0~5000
0030H  温度上限报警值 读写 -400~1000
0031H  温度下限报警值 读写 -400~1000
0032H  温度报警回差值 读写 0~1000
0033H  温度校准值 读写 -400~1000
0034H  温度继电器使能 读写 1为使能0为失能
0035H  湿度上限报警值 读写 0~1000
0036H  湿度下限报警值 读写 0~1000
0037H  湿度报警回差值 读写 0~1000
0038H  湿度校准值 读写 -400~1000
0039H  湿度继电器使能 读写 1为使能0为失能
***************************************************/


static ST_CO2_CTL st_CO2_Ctl = {0};			/* CO2控制结构体 		*/
static ST_ModBusPack st_ModBusPack = {0};	/* ModBus结构体 		*/

/**
  * @brief 读取传感器CO2值
 */
void ReadCO2SensorValue(void)
{
	u8 TempData[16] = {0};			/* 数据缓冲 */
	u32 timeout = st_CO2_Ctl.timeout;
	if(st_CO2_Ctl.TryCount != 0)
	{
		/************************************************************
		读取设备地址 0x01 的 CO2 值
		地址码 功能码 		起始地址 		数据长度 		校验码低位 校验码高位
		0x01 	0x03 	0x00 0x02 	0x00 0x01 	0x25 		0xCA
		*************************************************************/

		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x02;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x01;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* 组包发送 */

		/* 等待接收数据 */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, CO2SensorRecvLen) == 0x00)	/* 等待接收数据 */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* 需要延迟10ms */	
		}

		/* 判断是否是超时退出的循环 */
		if(timeout == 0)
		{
			st_CO2_Ctl.TryCount ++;
		}
		else
		{
			ModBusBufToPack(&st_ModBusPack, TempData);					/* 把数据赋值到PACK */
			st_CO2_Ctl.CO2Value = CalculaCO2SensorValue(st_ModBusPack.data, st_ModBusPack.datalen);
		}

	}
}

/**
  * @brief CO2传感器初始化
 */
void InitCO2Sensor(void)
{
	st_CO2_Ctl.timeout = 5000;								/* 5秒左右 */
	st_CO2_Ctl.TryCount = 5;								/* 尝试发送5次 */
	st_CO2_Ctl.Sensor_addr = CO2SensorAddr;					/* 地址赋值 */
	st_ModBusPack.address = st_CO2_Ctl.Sensor_addr;			/* ModBus设备地址赋值 */
	st_ModBusPack.function = FUNCODE;						/* 功能码 */
	OPENRS_485;												/* 打开RS485通信 */
}

/**
  * @brief 解析CO2数据
 */
u16 CalculaCO2SensorValue(u8* CO2Data, u32 len)
{
	u16 CO2Value = 0;
	if(len != 0)
	{
		CO2Value = ((CO2Data[0] << 8) | CO2Data[1]);
	}
	return CO2Value;
}
