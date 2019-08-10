/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		led.h
  * @author		lwl
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		900M����ͷ�ļ���
  */ 

/**
  * @Multi-Include-Prevent Section
  */
#ifndef __UHF_READER_DRIVER_H__
#define __UHF_READER_DRIVER_H__
/**
  * @Include File Section
  * @����ͷ�ļ�
  */
#include "bsp.h"
#include "App_RF900M.h"


/**
  * @Macro Define Section
  * @�궨��
  */
#define UHF_BUF_NUM  1024                                               /*���������ݸ���*/  
#define UHF_SOF_DATA 0xA0                                              /*��ʼ����*/  
#define UHF_END_DATA 0x55                                               /*��������*/  
#define OFF_UHF_RX_DATA USART_ITConfig(USART2, USART_IT_RXNE, DISABLE)  /*�رս����ж�*/  
#define ON_UHF_RX_DATA  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE)   /*�򿪽����ж�*/  
#define OFF_UHF_TX_DATA USART_ITConfig(USART2, USART_IT_TXE, DISABLE)   /*�رշ����ж�*/  
#define ON_UHF_TX_DATA  USART_ITConfig(USART2, USART_IT_TXE, ENABLE)    /*�򿪷����ж�*/ 




#define RLM_SET_ANTENNA			0x74	/* ���ù������� */
#define RLM_GET_STATUS			0x63  	/*��ȡ�������Ӽ����״̬*/
#define RLM_GET_POWER			0x77  	/*��ѯ��д����ǰ�������*/
#define RLM_SET_POWER			0x76	/*���ö�д����Ƶ�������*/
#define RLM_GET_FRE 			0x79	/*��ѯ��д������Ƶ�ʷ�Χ*/
#define RLM_SET_FRE				0x78	/*���ö�д������Ƶ�ʷ�Χ*/		  
#define RLM_GET_VERSION			0x07	/*��ȡRLM��Ϣ*/
#define RLM_READ_UID 			0x0a    /*��ȡRLM-UID��Ϣ*/

#define RLM_INVENTORY			0x81	/*����ǩʶ��-ѭ��ģʽ*/
#define RLM_INVENTORY_ANTI		0x89	/*���ǩʶ��-ѭ��ģʽ*/
#define RLM_STOP_GET			0x12	/*ֹͣ����*/
#define RLM_READ_DATA			0x81	/*��ȡ��ǩ���ݣ�ָ��UII������*/
#define RLM_WRITE_DATA			0x82	/*д���ǩ����_���ֳ�ģʽ��ָ��UII��   ����  */
#define RLM_INVENTORY_SINGLE	0x80	/*����ǩʶ��-����ģʽ*/
#define RLM_BLOCK_WRITE_DATA	0x82    /*д���ǩ����_���ֳ�ģʽ��ָ��UII��*/


/*��Ӧ״̬*/
#define UHF_FLAG_OK				0X00		  /*��Ӧ����*/
#define UHF_FLAG_FAIL			0X01		  /*��ʱ����*/
#define UHF_FLAG_CMD			0X06		  /*�����ƥ��*/
#define UHF_FLAG_STATUS 		0X07		  /*״̬ʧ��*/

/**
  * @brief  Struct Define Section
  * @�ṹ�嶨��
  */
typedef struct
{
	u8  UHF_SOF;							/* ��ʼ��־ */
	u8  UHF_LEN;							/* ���ݳ��� */
	u8  UHF_ADR;							/* �豸��ַ */
	u8  UHF_CMD;							/* ������  	*/
	u8  UHF_PAYLOAD[UHF_BUF_NUM];			/* �������� */
	u8  UHF_CC;								/* У�� */
}_UHF_CARD_;
/**
  * @Global Varible Declare
  * @ȫ�ֱ�������
  */

/**
  * @Prototype Declare Section
  * @�ڲ�����ԭ������
  */
void UHF_init(void);/*��ʼ��*/
void UHFResive_Pasv(void);
void UHFSend_Pasv(_UHF_CARD_* Bdata);

u8 UhfWRdata_list(PST_UHF_Device pst_UHF_Dev)	;/*Ѱ��*/
u8 UhfWRdata_read(PST_UHF_Device pst_UHF_Dev, PST_READ_TAG pst_READ_TAG);/*����*/
u8 UhfWRdata_write(PST_UHF_Device pst_UHF_Dev, Device_packet_t *packet);/*д��*/
u8 UhfSelect_TAG(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev);

EN_UHF_STATUS UHF_Tx_PACK(PST_UHF_Device pst_UHF_device, const u8 *pdata, u8 data_size);
EN_UHF_STATUS WaitUHFDataReceive(PST_UHF_Device pst_UHF_Dev);

#endif

