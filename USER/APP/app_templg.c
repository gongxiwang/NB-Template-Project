/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2������Ӧ�ú���
  */
#include "bsp.h"
#include "app_templg.h"
#include "bsp_modbus.h"
#include "bsp_uart.h"


/***************************************************
�Ĵ�����ַ PLC����̬��ַ ���� ����
0000 H 40001 ʪ�� ֻ��
0001 H 40002 �¶� ֻ��
0002 H 40003 ���ն�
��ֻ��0~200000Lux
���ã���λ1Lux��
ֻ��
0003 H 40004
0006 H 40007 ���ն�
(0~65535��λ1Lux
0~200000��λ��Lux)
ֻ��
***************************************************/


static ST_TEMPLG_CTL st_Templg_Ctl = {0};			/* CO2���ƽṹ�� 		*/
static ST_ModBusPack st_ModBusPack = {0};			/* ModBus�ṹ�� 		*/

/**
  * @brief ��ȡ������CO2ֵ
 */
void ReadTemplgSensorValue(u8 ValueType)
{
	u8 TempData[16] = {0};					/* ���ݻ��� */
	u32 timeout = st_Templg_Ctl.timeout;	/* �ȴ���ʱʱ�� */
	if(EN_TYPE_LIGHT == ValueType)			/* ���� */
	{
		/************************************************************
		��ѯ֡
		��ַ�� ������ ��ʼ��ַ ���ݳ��� У�����λ У�����λ
		0x01 0x03 0x00 0x06 0x00 0x01 0x64 0x0B
		*************************************************************/
		st_Templg_Ctl.TemplgType = EN_TYPE_LIGHT;
		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x06;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x01;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* ������� */

		/* �ȴ��������� */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, TmeplgSensorRecvLen) == 0x00)	/* �ȴ��������� */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* ��Ҫ�ӳ�10ms */	
		}

	}
	else if(EN_TYPE_TEPHUM == ValueType)	/* ��ʪ�� */
	{
		/************************************************************
		 ��ȡ�豸��ַ 0x01 ����ʪ��ֵ
		��ѯ֡
		��ַ�� ������ ��ʼ��ַ ���ݳ��� У�����λ У�����λ
		0x01 0x03 0x00 0x00 0x00 0x02 0xC4 0x0B
		*************************************************************/
		st_Templg_Ctl.TemplgType = EN_TYPE_TEPHUM;
		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x00;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x02;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* ������� */

		/* �ȴ��������� */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, TmeplgSensorRecvLen) == 0x00)	/* �ȴ��������� */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* ��Ҫ�ӳ�10ms */	
		}
	}

	/* �ж��Ƿ��ǳ�ʱ�˳���ѭ�� */
	if(timeout == 0)
	{
		st_Templg_Ctl.TryCount ++;
	}
	else
	{
		ModBusBufToPack(&st_ModBusPack, TempData);					/* �����ݸ�ֵ��PACK */
		CalculaTemplgSensorValue(&st_Templg_Ctl, st_ModBusPack.data, st_ModBusPack.datalen);/* �ó�����һ��ֵ */
	}

	
}

/**
  * @brief CO2��������ʼ��
 */
void InitTemplgSensor(void)
{
	st_Templg_Ctl.timeout = 5000;								/* 5������ */
	st_Templg_Ctl.TryCount = 5;								/* ���Է���5�� */
	st_Templg_Ctl.Sensor_addr = TemplgSensorAddr;					/* ��ַ��ֵ */
	st_ModBusPack.address = st_Templg_Ctl.Sensor_addr;			/* ModBus�豸��ַ��ֵ */
	st_ModBusPack.function = TEMPLGFUNCODE;						/* ������ */
	OPENRS_485;												/* ��RS485ͨ�� */
}

/**
  * @brief ����TemplgSensorValue
 */
u16 CalculaTemplgSensorValue(PST_TEMPLG_CTL pst_templgCtl, u8* TemplgData, u32 len) 
{
	if(len > 0)
	{
		switch(pst_templgCtl->TemplgType)
		{
			case EN_TYPE_LIGHT:				/* ���� */
				pst_templgCtl->LightValue = ((TemplgData[0] << 8) | TemplgData[1]);
				break;
			case EN_TYPE_TEPHUM:			/* ��ʪ�� */
				pst_templgCtl->HumValue = ((TemplgData[0] << 8) | TemplgData[1]);
				pst_templgCtl->TempValue = ((TemplgData[2] << 8) | TemplgData[3]);
				break;
			default:
				break;
		}
	}
	return 0;
}
