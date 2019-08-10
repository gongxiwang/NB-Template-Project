/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_modbus.c
  * @author  walle
  * @version  V1.0.0
  * @date  08/07/2019
  * @brief  modbus����
  */ 
#include "bsp_modbus.h"
#include "bsp_uart.h"
#include "bsp_dma.h"


/**
  * @brief CRC0У��
 */
static const u8 s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

/**
  * @brief CRC1У��
 */
const u8 s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/**
  * @brief ModBus����buffer��pack
*/
EN_MODBUS_RESULT ModBusBufToPack(PST_ModBusPack pst_ModBusPack, u8* modbus_RxBuffer)
{
	pst_ModBusPack->address = modbus_RxBuffer[0];		/* ��ַ��ֵ */
	pst_ModBusPack->function = modbus_RxBuffer[1];		/* �����븳ֵ */
	pst_ModBusPack->datalen = modbus_RxBuffer[2];		/* ���ݳ��ȸ�ֵ */
	if(pst_ModBusPack->datalen)
	{
		memcpy(pst_ModBusPack->data, &modbus_RxBuffer[3], pst_ModBusPack->datalen);/* ���ݸ�ֵ */
	}
	return MR_OK;	
}

/**
  * @brief ���һ֡���ݣ������� 
*/
void ModBusTxPack(u8* modbus_TxBuffer, PST_ModBusPack pst_ModBusPack)
{
	u16 ModBusCRC = 0;									/* CRC16У���� */
	modbus_TxBuffer[0] = pst_ModBusPack->address;		/* ��ַ��ֵ */
	modbus_TxBuffer[1] = pst_ModBusPack->function;		/* �����븳ֵ */
	modbus_TxBuffer[2] = pst_ModBusPack->datalen;		/* ���ݳ��ȸ�ֵ */
	if(pst_ModBusPack->datalen)
	{
		memcpy(&modbus_TxBuffer[3], pst_ModBusPack->data, pst_ModBusPack->datalen);	/* ���ݸ�ֵ */
	}
	/* CRC16У�� */
	ModBusCRC = CRC16_Modbus(modbus_TxBuffer, pst_ModBusPack->datalen + 3);			/* ����CRCУ���� */
	/* ����У�� */
	modbus_TxBuffer[pst_ModBusPack->datalen + 3] = (ModBusCRC & 0x0F) << 8;			/* CRC_L */
	modbus_TxBuffer[pst_ModBusPack->datalen + 4] = (ModBusCRC & 0xF0) >> 8;			/* CRC_H */
	/* ִ�з��� */
	ModBusSendPack(modbus_TxBuffer, 0x08);						/* �������ݰ� */
}

/**
  * @brief Modbus����
 */
void ModBusSendPack(u8* sendbuf, u8 len)
{
	ENRS485_TX;							/* ����RS485����ģʽ */
	OSTimeDlyHMSM(0, 0, 0, 10);			/* ����RS485������Ҫ��ʱ10ms���� */
	USART1DMA_Send_Data(sendbuf, 0x08);
}

/*
*********************************************************************************************************
*	�� �� ��: CRC16_Modbus
*	����˵��: ����CRC�� ����ModbusЭ�顣
*	��    ��: _pBuf : ����У�������
*			  _usLen : ���ݳ���
*	�� �� ֵ: 16λ����ֵ�� ����Modbus ���˽�����ֽ��ȴ��ͣ����ֽں��͡�
*
*   ���п��ܵ�CRCֵ����Ԥװ���������鵱�У������㱨������ʱ���Լ򵥵��������ɣ�
*   һ�����������16λCRC�������256�����ܵĸ�λ�ֽڣ���һ�����麬�е�λ�ֽڵ�ֵ��
*   ������������CRC�ķ�ʽ�ṩ�˱ȶԱ��Ļ�������ÿһ�����ַ��������µ�CRC����ķ�����
*
*  ע�⣺�˳����ڲ�ִ�и�/��CRC�ֽڵĽ������˺������ص����Ѿ�����������CRCֵ��Ҳ����˵���ú����ķ���ֵ����ֱ�ӷ���
*        �ڱ������ڷ��ͣ�
*********************************************************************************************************
*/
u16 CRC16_Modbus(u8*_pBuf, u16 _usLen)
{
	u8 ucCRCHi = 0xFF; 	/* ��CRC�ֽڳ�ʼ�� */
	u8 ucCRCLo = 0xFF; 	/* ��CRC �ֽڳ�ʼ�� */
	u16 usIndex;  		/* CRCѭ���е����� */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* ����CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((u16)ucCRCHi << 8 | ucCRCLo);
}

