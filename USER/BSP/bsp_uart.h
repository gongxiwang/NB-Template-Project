/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_uart.h
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  uart����ͷ�ļ�
  */ 
#ifndef _USART_BUF_H_
#define _USART_BUF_H_

/* ͷ�ļ�---------------------------------------------------------------------------*/

#include "stm32f10x_usart.h"
#include "includes.h"
#include "ringbuf.h"


/* �궨�� --------------------------------------------------------------------------*/

#define PACK_CMD_DATA_LEN   0x08        	//�������������


/** 
  * @brief 485/232���ƶ˿�
  */
#define CTRL_485GPIOx GPIOB
#define CTRL_232GPIOx GPIOB
#define CTRL_485PINx  GPIO_Pin_6
#define CTRL_232PINx  GPIO_Pin_5
#define EN_RS485GPIOx GPIOA
#define EN_RS485PINx  GPIO_Pin_8  


/** 
  * @brief ��ǰ����ͨ�ŷ�ʽ
  */
typedef enum
{
	EN_USART_RS485,	/* RS485ͨ�ŷ�ʽ */
	EN_USART_RS232,	/* RS232ͨ�ŷ�ʽ */
	EN_USART_UART1,	/* USARTͨ�ŷ�ʽ */
	EN_USART_UNDEF,	/* δ���� */
}EN_USART_MODE;

/** 
  * @brief 485/232/USART1�򿪺���
  */
#define OPENRS_485  	{GPIO_SetBits(CTRL_485GPIOx, CTRL_485PINx);\
						GPIO_ResetBits(CTRL_232GPIOx, CTRL_232PINx);\
						SetUSART_MODE(EN_USART_RS485);}

#define OPENRS_232   	{GPIO_ResetBits(CTRL_485GPIOx, CTRL_485PINx);\
						GPIO_SetBits(CTRL_232GPIOx, CTRL_232PINx);\
						SetUSART_MODE(EN_USART_RS232);}

#define OPENUSAT1		{GPIO_ResetBits(CTRL_485GPIOx, CTRL_485PINx);\
						GPIO_ResetBits(CTRL_232GPIOx, CTRL_232PINx);\
						SetUSART_MODE(EN_USART_UART1);}	

/** 
  * @brief ����RS485���շ���ʹ��
  */
#define ENRS485_TX		GPIO_SetBits(EN_RS485GPIOx, EN_RS485PINx)
#define ENRS485_RX		GPIO_ResetBits(EN_RS485GPIOx, EN_RS485PINx)





/** 
  * @brief UART������Ϣ�ṹ��    
  */
typedef struct
{
	USART_TypeDef* USARTx;
	GPIO_TypeDef* GPIOx;
	unsigned long GPIO_Pin_tx;
	unsigned long GPIO_Pin_rx;
	unsigned long ulBaudRate;
	unsigned char ucBSP_INT_ID;
	unsigned char NVIC_IRQChannel;
	void (*INT_Service_name)(void);
}ST_UART_INFO, *PST_UART_INFO;

/** 
  * @brief    
  */
typedef enum
{
    EN_STATE_PACK_HEAD      = 0x01,     //��ȡ��ͷ״̬
    EN_STATE_PACK_DATA      = 0x02,     //��ȡ������״̬
    EN_STATE_PACK_DISCARD   = 0x03,     //����״̬״̬   
}EN_RECV_STATE;

/** 
  * @brief �������ݰ��ṹ��    
  */
typedef struct
{
	unsigned char  ucADDRCODE;		/* ��ַ�� */
    unsigned char  ucFUNCODE;		/* ������ */
	unsigned char  ucByteNum;		/* �ֽ��� */
	unsigned char  ucStarAdd_H;		/* ��ʼ��ַ_�� */
	unsigned char  ucStarAdd_L;		/* ��ʼ��ַ_�� */
	unsigned char  ucRegData_H;		/* �Ĵ�������_�� */
	unsigned char  ucRegData_L;		/* �Ĵ�������_�� */
	unsigned char  ucCRC0;			/* CRC0У�� */
	unsigned char  ucCRC1;			/* CRC1У�� */
}ST_PACK_DATA,*PST_PACK_DATA;


/**
  * @}���崮�ڷ��ͺͽ���ѭ��������нṹ��
  */
typedef struct
{
    unsigned char   aucBufRecv[256];
    tRingBufObject  stUartBufRecv;
}ST_USART_DATA, *PST_USART_DATA;



/* ��������-----------------------------------------------------------------------*/

PST_UART_INFO ReturnUartInfo(void);
unsigned long Stm32UartSendData(unsigned char *pucDataBuf, unsigned long const ulDataLen);
unsigned long Stm32UartRecvData(unsigned char* lpBuf);
int Module_SendMessage(int len, const void* src);
PST_PACK_DATA GetStm32UartRecvData(void);
void BSP_UART_Init(void);
void Stm32UsartSendData(USART_TypeDef *USARTx,unsigned char *pucDataAddr,unsigned short usDataLen);
unsigned long NBUartRecvData(unsigned char* lpBuf);
unsigned long Uart1RecvData(unsigned char* lpBuf);
unsigned long LORAUartRecvData(unsigned char* lpBuf);
void SetUSART_MODE(EN_USART_MODE en_MODE);
EN_USART_MODE GETUSART_MODE(void);
u8 WaitUartDataReceive(u8* RecvBuffer, u8 ExpectLen);

#endif

