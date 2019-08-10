/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_modbus.h
  * @author  walle
  * @version  V1.0.0
  * @date  08/07/2019
  * @brief  modbus����
  */ 
#ifndef __BSP_MODBUS__H
#define __BSP_MODBUS__H

#include "bsp.h"

#define M_MAX_FRAME_LENGTH	256		/* ���֡����Ϊ261�ֽ� ��ַ+����+֡��+���ݳ���+����+CRC(CRC16��2�ֽ�) */
#define M_MIN_FRAME_LENGTH	5		/* ��С֡����Ϊ5�ֽ� ��ַ+����+֡��+���ݳ���+CRC8/XOR/SUM */
#define FRAM_DATA_LEN		16		/* ֡���ݳ��� */



/**
  * @brief ���ؽ�����������Ͷ���
*/
typedef enum
{
	MR_OK=0,						/* ���� */
	MR_FRAME_FORMAT_ERR = 1,		/* ֡��ʽ���� */	 
	MR_FRAME_CHECK_ERR = 2,			/* У��ֵ��λ */ 
	MR_MEMORY_ERR = 3,				/* �ڴ���� */ 
}EN_MODBUS_RESULT;


/**
  * @brief MODBUF֡�ṹ��
*/
typedef struct
{ 
	u8 address;						/* �豸��ַ��0���㲥��ַ��1~255���豸��ַ */
	u8 function;					/* ֡���ܣ�0~255 */
	u8 count;						/* ֡��� */
	u8 datalen;						/* ��Ч���ݳ��� */
	u8 data[FRAM_DATA_LEN];			/* ���ݴ洢�� */
	u16 chkval;						/* У��ֵ  */
}ST_ModBusPack, *PST_ModBusPack;



/**
  * @brief ��������
*/
u16 CRC16_Modbus(u8*_pBuf, u16 _usLen);
void ModBusSendPack(u8* sendbuf, u8 len);
void ModBusTxPack(u8* modbus_TxBuffer, PST_ModBusPack pst_ModBusPack);
EN_MODBUS_RESULT ModBusBufToPack(PST_ModBusPack pst_ModBusPack, u8* modbus_RxBuffer);
#endif
