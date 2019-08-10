/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2传感器应用函数
  */
#ifndef __APP_TEMPLG__H
#define __APP_TEMPLG__H

#define TEMPLGFUNCODE  			0x03		/* Templg传感器使用0x03--读取寄存器数据作为功能码 */
#define TemplgSensorAddr  		0x01		/* 定义Templg传感器地址 */
#define TmeplgSensorRecvLen		0x07		/* 定义Templg的长度 */

/**
  * @brief Templg的值类型
 */
typedef enum
{
	EN_TYPE_LIGHT,	/* 光照 */
	EN_TYPE_TEPHUM,	/* 温湿度 */
}EN_TEMPLG_TYPE;


/**
  * @brief Templg控制结构体
 */
typedef struct
{
	u8 Sensor_addr;	/* 传感器地址 */
	u8 StartAddr_H;	/* 起始地址低 */
	u8 StartAddr_L;	/* 起始地址高 */
	u8 DataAddr_H;	/* 数据地址低 */
	u8 DataAddr_L;	/* 数据地址高 */
	u8 TemplgType;	/* 读取的类型 */
	u16 TempValue;	/* 温度值 */
	u16 HumValue;	/* 湿度值 */
	u16 LightValue;	/* 光照值 */
	u32 timeout;	/* 超时时间 */
	u32 TryCount;	/* 尝试次数 */
}ST_TEMPLG_CTL, *PST_TEMPLG_CTL;


void ReadTemplgSensorValue(u8 ValueType);
void InitTemplgSensor(void);
u16 CalculaTemplgSensorValue(PST_TEMPLG_CTL pst_templgCtl, u8* TemplgData, u32 len);
#endif