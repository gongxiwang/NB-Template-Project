/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2������Ӧ�ú���
  */
#ifndef __APP_TEMPLG__H
#define __APP_TEMPLG__H

#define TEMPLGFUNCODE  			0x03		/* Templg������ʹ��0x03--��ȡ�Ĵ���������Ϊ������ */
#define TemplgSensorAddr  		0x01		/* ����Templg��������ַ */
#define TmeplgSensorRecvLen		0x07		/* ����Templg�ĳ��� */

/**
  * @brief Templg��ֵ����
 */
typedef enum
{
	EN_TYPE_LIGHT,	/* ���� */
	EN_TYPE_TEPHUM,	/* ��ʪ�� */
}EN_TEMPLG_TYPE;


/**
  * @brief Templg���ƽṹ��
 */
typedef struct
{
	u8 Sensor_addr;	/* ��������ַ */
	u8 StartAddr_H;	/* ��ʼ��ַ�� */
	u8 StartAddr_L;	/* ��ʼ��ַ�� */
	u8 DataAddr_H;	/* ���ݵ�ַ�� */
	u8 DataAddr_L;	/* ���ݵ�ַ�� */
	u8 TemplgType;	/* ��ȡ������ */
	u16 TempValue;	/* �¶�ֵ */
	u16 HumValue;	/* ʪ��ֵ */
	u16 LightValue;	/* ����ֵ */
	u32 timeout;	/* ��ʱʱ�� */
	u32 TryCount;	/* ���Դ��� */
}ST_TEMPLG_CTL, *PST_TEMPLG_CTL;


void ReadTemplgSensorValue(u8 ValueType);
void InitTemplgSensor(void);
u16 CalculaTemplgSensorValue(PST_TEMPLG_CTL pst_templgCtl, u8* TemplgData, u32 len);
#endif