/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2传感器应用函数
  */
#ifndef __APP_CO2__H
#define __APP_CO2__H

#define FUNCODE  			0x03					/* CO2传感器使用0x03--读取寄存器数据作为功能码 */
#define CO2SensorAddr  		0x01					/* 定义CO2传感器地址 */
#define CO2SensorRecvLen	0x07


/**
  * @brief CO2控制结构体
 */
typedef struct
{
	u8 Sensor_addr;	/* 传感器地址 */
	u8 StartAddr_H;	/* 起始地址低 */
	u8 StartAddr_L;	/* 起始地址高 */
	u8 DataAddr_H;	/* 数据地址低 */
	u8 DataAddr_L;	/* 数据地址高 */
	u16 CO2Value;	/* CO2含量 */
	u32 timeout;	/* 超时时间 */
	u32 TryCount;	/* 尝试次数 */
}ST_CO2_CTL, *PST_CO2_CTL;


void ReadCO2SensorValue(void);
void InitCO2Sensor(void);
u16 CalculaCO2SensorValue(u8* CO2Data, u32 len);
#endif