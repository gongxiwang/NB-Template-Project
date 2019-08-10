/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  app_watmet.h
  * @author  walle
  * @version  V1.0.0
  * @date  08/06/2019
  * @brief  ˮ������
  */ 
#ifndef __APP_WATMET__H
#define __APP_WATMET__H



/**
  * @brief  ˮ��ModuleBUSЭ�����
 */
typedef struct
{
	unsigned char  ucADDRCODE;		/* ��ַ�� */
    unsigned char  ucFUNCODE;		/* ������ */
	unsigned char  ucDataSize;		/* ���ݳ��� */
	unsigned char  ucStarAdd_H;		/* ��ʼ��ַ_�� */
	unsigned char  ucStarAdd_L;		/* ��ʼ��ַ_�� */
	unsigned char  ucRegData_H;		/* �Ĵ�������_�� */
	unsigned char  ucRegData_L;		/* �Ĵ�������_�� */
	unsigned char  ucByteNum;		/* �ֽ��� */
	unsigned char  ucWrittingData_H;/* ��д������_�� */
	unsigned char  ucWrittingData_L;/* ��д������_�� */
	unsigned char  ucCRC0;			/* CRC0У�� */
	unsigned char  ucCRC1;			/* CRC1У�� */
}ST_WATMET_PACK, *PST_WATMET_PACK;


/**
  * @brief  ˮ������ModuleBUS������
 */
typedef enum
{
	EN_FUNCODE_READREG = 0x03,	/* ��ȡ�Ĵ��� */
	EN_FUNCODE_WRTEREG = 0x10,	/* д��Ĵ��� */
	EN_FUNCODE_CTLGATE = 0x05,	/* ���Ʒ��� */
	EN_FUNCODE_RDGATE  = 0x01,	/* ��ȡ���� */
}EN_FUN_CODE;


/**
  * @brief  ˮ���������ط���
 */
typedef enum
{
	EN_VALUE_ON = 0x00ff,	/* �򿪷��� */
	EN_VALUE_OFF = 0x0000, /* �رշ��� */
	EN_USED_WATER = 0x0002,/* ����ˮ�� */
}EN_WATMET_REG;



void Watmet_Init(void);
void Watmet_Read(void);
void CreatModBusPackWithSend(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 FuCode, u16 StartAdr, u16 RegData);
u16 CRC16_Modbus(u8*_pBuf, u16 _usLen);
void ModBusSendPack(u8* sendbuf, u8 len);
u8 WatmetRecvAnalysis(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 *RecvData);
u8 WatmetRecv(PST_WATMET_PACK pst_ModBusPack_Watmet);


#endif
