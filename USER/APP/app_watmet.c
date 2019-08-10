/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  app_watmet.c
  * @author  walle
  * @version  V1.0.0
  * @date  08/06/2019
  * @brief  ˮ�������ӿ�
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


#define READSTAR_ADDR 0x0000									/* ���Ĵ�����ʼ��ַ */
#define GATESTAR_ADDR 0x0001									/* ���ط��ſ��Ƶ���ʼ��ַ */
#define WATMET_ADDRESS 0x69 									/* �궨��ˮ��ĵ�ַ */
static ST_WATMET_PACK st_ModuleBusPack_Watmet = {0};			/* ˮ��ModuleBusPackWatmetЭ�����ݽṹ�� */


/**
  * @brief  ˮ���ʼ��
 */
void Watmet_Init(void)
{
	u8 WatMetRcv = 0;
	OPENRS_485;
	/* �򿪷��� */
	CreatModBusPackWithSend(&st_ModuleBusPack_Watmet, EN_FUNCODE_CTLGATE, GATESTAR_ADDR, EN_VALUE_OFF);
	WatMetRcv = WatmetRecv(&st_ModuleBusPack_Watmet);
	if(WatMetRcv == EN_VALUE_ON)
	{
		debug_printf("Watmet Init Success\r\n");
	}
}

/**
  * @brief  ˮ����ˮ��
 */
void Watmet_Read(void)
{
	u8 WatMetRcv = 0;
	u8 WatmetData[4] = {0};
	/* ����ˮ�� */
	CreatModBusPackWithSend(&st_ModuleBusPack_Watmet, EN_FUNCODE_READREG, READSTAR_ADDR, EN_USED_WATER);
	WatMetRcv = WatmetRecv(&st_ModuleBusPack_Watmet);
	if(WatMetRcv == 0x04)
	{
		WatmetData[0] = st_ModuleBusPack_Watmet.ucStarAdd_H;
		WatmetData[1] = st_ModuleBusPack_Watmet.ucStarAdd_L;
		WatmetData[2] = st_ModuleBusPack_Watmet.ucRegData_H;
		WatmetData[3] = st_ModuleBusPack_Watmet.ucRegData_L;
	}
	/* ����д�����ݵ�NBIOTƽ̨ */;
	GropPackNBMsgSend(WatmetData, WatMetRcv, 0x00);
}

/**
  * @brief  ˮ���Ӧ
 */
u8 WatmetRecv(PST_WATMET_PACK pst_ModBusPack_Watmet)
{
	static u8 Timeout = 200;	
	u8 UsartSensorData[100] = {0};								/* ���������մ����������� */
	u32 ulReceiveLen = 0;										/* ����Ľ������ݳ��� */
	while(Timeout --)
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData);		/* ���մ���1������������ */
		if(ulReceiveLen > 0)	/* ������յ����� */
		{
			return WatmetRecvAnalysis(pst_ModBusPack_Watmet, UsartSensorData);
		}
	}
	return -1;
}

/**
  * @brief  ˮ��������ݽ���
 */
u8 WatmetRecvAnalysis(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 *RecvData)
{
	pst_ModBusPack_Watmet->ucADDRCODE = RecvData[0];
	pst_ModBusPack_Watmet->ucFUNCODE = RecvData[1];
	if(pst_ModBusPack_Watmet->ucFUNCODE == EN_FUNCODE_CTLGATE)/* ������ص��ǿ��Ʒ������� */
	{
		pst_ModBusPack_Watmet->ucStarAdd_H = RecvData[2];
		pst_ModBusPack_Watmet->ucStarAdd_L = RecvData[3];
		pst_ModBusPack_Watmet->ucRegData_H = RecvData[4];
		pst_ModBusPack_Watmet->ucRegData_L = RecvData[5];
		return pst_ModBusPack_Watmet->ucRegData_L;			/* ���� */
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
  * @brief ���
 */
void CreatModBusPackWithSend(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 FuCode, u16 StartAdr, u16 RegData)
{
	u8 ModBusSendBuf[0x08] = {0};
	u16 ModBusCRC = 0;
	pst_ModBusPack_Watmet->ucADDRCODE 	= WATMET_ADDRESS;			/* ��ַ���ò���ˮ��ʹ��0x69 */
	pst_ModBusPack_Watmet->ucFUNCODE 	= FuCode;					/* ������ */
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

	ModBusCRC = CRC16_Modbus(ModBusSendBuf, 0x06);				/* ����CRCУ���� */
	ModBusSendBuf[6] = ModBusCRC >> 8;
	ModBusSendBuf[7] = ModBusCRC;

	ModBusSendPack(ModBusSendBuf, 0x08);						/* �������ݰ� */
}


