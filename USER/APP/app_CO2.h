/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2������Ӧ�ú���
  */
#ifndef __APP_CO2__H
#define __APP_CO2__H

#define FUNCODE  			0x03					/* CO2������ʹ��0x03--��ȡ�Ĵ���������Ϊ������ */
#define CO2SensorAddr  		0x01					/* ����CO2��������ַ */
#define CO2SensorRecvLen	0x07


/**
  * @brief CO2���ƽṹ��
 */
typedef struct
{
	u8 Sensor_addr;	/* ��������ַ */
	u8 StartAddr_H;	/* ��ʼ��ַ�� */
	u8 StartAddr_L;	/* ��ʼ��ַ�� */
	u8 DataAddr_H;	/* ���ݵ�ַ�� */
	u8 DataAddr_L;	/* ���ݵ�ַ�� */
	u16 CO2Value;	/* CO2���� */
	u32 timeout;	/* ��ʱʱ�� */
	u32 TryCount;	/* ���Դ��� */
}ST_CO2_CTL, *PST_CO2_CTL;


void ReadCO2SensorValue(void);
void InitCO2Sensor(void);
u16 CalculaCO2SensorValue(u8* CO2Data, u32 len);
#endif