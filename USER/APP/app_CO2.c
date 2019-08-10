/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_CO2.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		CO2������Ӧ�ú���
  */
#include "bsp.h"
#include "app_CO2.h"
#include "bsp_modbus.h"
#include "bsp_uart.h"


/***************************************************
�Ĵ�����ַ ���� ���� ��Χ������˵��
0000H  ʪ��ֵ ֻ�� 0~1000
0001H  �¶�ֵ ֻ�� -400~1000
0002H  CO2Ũ��ֵ ֻ�� 0~5000
0030H  �¶����ޱ���ֵ ��д -400~1000
0031H  �¶����ޱ���ֵ ��д -400~1000
0032H  �¶ȱ����ز�ֵ ��д 0~1000
0033H  �¶�У׼ֵ ��д -400~1000
0034H  �¶ȼ̵���ʹ�� ��д 1Ϊʹ��0Ϊʧ��
0035H  ʪ�����ޱ���ֵ ��д 0~1000
0036H  ʪ�����ޱ���ֵ ��д 0~1000
0037H  ʪ�ȱ����ز�ֵ ��д 0~1000
0038H  ʪ��У׼ֵ ��д -400~1000
0039H  ʪ�ȼ̵���ʹ�� ��д 1Ϊʹ��0Ϊʧ��
***************************************************/


static ST_CO2_CTL st_CO2_Ctl = {0};			/* CO2���ƽṹ�� 		*/
static ST_ModBusPack st_ModBusPack = {0};	/* ModBus�ṹ�� 		*/

/**
  * @brief ��ȡ������CO2ֵ
 */
void ReadCO2SensorValue(void)
{
	u8 TempData[16] = {0};			/* ���ݻ��� */
	u32 timeout = st_CO2_Ctl.timeout;
	if(st_CO2_Ctl.TryCount != 0)
	{
		/************************************************************
		��ȡ�豸��ַ 0x01 �� CO2 ֵ
		��ַ�� ������ 		��ʼ��ַ 		���ݳ��� 		У�����λ У�����λ
		0x01 	0x03 	0x00 0x02 	0x00 0x01 	0x25 		0xCA
		*************************************************************/

		st_ModBusPack.data[0] = 0x00;
		st_ModBusPack.data[1] = 0x02;
		st_ModBusPack.data[2] = 0x00;
		st_ModBusPack.data[3] = 0x01;
		
		ModBusTxPack(TempData, &st_ModBusPack);							/* ������� */

		/* �ȴ��������� */
		while(timeout --)/* 5S */
		{
			if(WaitUartDataReceive(TempData, CO2SensorRecvLen) == 0x00)	/* �ȴ��������� */
			{
				break;
			}
			OSTimeDlyHMSM(0, 0, 0, 1); 									/* ��Ҫ�ӳ�10ms */	
		}

		/* �ж��Ƿ��ǳ�ʱ�˳���ѭ�� */
		if(timeout == 0)
		{
			st_CO2_Ctl.TryCount ++;
		}
		else
		{
			ModBusBufToPack(&st_ModBusPack, TempData);					/* �����ݸ�ֵ��PACK */
			st_CO2_Ctl.CO2Value = CalculaCO2SensorValue(st_ModBusPack.data, st_ModBusPack.datalen);
		}

	}
}

/**
  * @brief CO2��������ʼ��
 */
void InitCO2Sensor(void)
{
	st_CO2_Ctl.timeout = 5000;								/* 5������ */
	st_CO2_Ctl.TryCount = 5;								/* ���Է���5�� */
	st_CO2_Ctl.Sensor_addr = CO2SensorAddr;					/* ��ַ��ֵ */
	st_ModBusPack.address = st_CO2_Ctl.Sensor_addr;			/* ModBus�豸��ַ��ֵ */
	st_ModBusPack.function = FUNCODE;						/* ������ */
	OPENRS_485;												/* ��RS485ͨ�� */
}

/**
  * @brief ����CO2����
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
