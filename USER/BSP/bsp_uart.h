/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_uart.h
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  uart驱动头文件
  */ 
#ifndef _USART_BUF_H_
#define _USART_BUF_H_

/* 头文件---------------------------------------------------------------------------*/

#include "stm32f10x_usart.h"
#include "includes.h"
#include "ringbuf.h"


/* 宏定义 --------------------------------------------------------------------------*/

#define PACK_CMD_DATA_LEN   0x08        	//定义命令包长度


/** 
  * @brief 485/232控制端口
  */
#define CTRL_485GPIOx GPIOB
#define CTRL_232GPIOx GPIOB
#define CTRL_485PINx  GPIO_Pin_6
#define CTRL_232PINx  GPIO_Pin_5
#define EN_RS485GPIOx GPIOA
#define EN_RS485PINx  GPIO_Pin_8  


/** 
  * @brief 当前串口通信方式
  */
typedef enum
{
	EN_USART_RS485,	/* RS485通信方式 */
	EN_USART_RS232,	/* RS232通信方式 */
	EN_USART_UART1,	/* USART通信方式 */
	EN_USART_UNDEF,	/* 未定义 */
}EN_USART_MODE;

/** 
  * @brief 485/232/USART1打开函数
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
  * @brief 设置RS485接收发送使能
  */
#define ENRS485_TX		GPIO_SetBits(EN_RS485GPIOx, EN_RS485PINx)
#define ENRS485_RX		GPIO_ResetBits(EN_RS485GPIOx, EN_RS485PINx)





/** 
  * @brief UART串口信息结构体    
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
    EN_STATE_PACK_HEAD      = 0x01,     //获取包头状态
    EN_STATE_PACK_DATA      = 0x02,     //获取包数据状态
    EN_STATE_PACK_DISCARD   = 0x03,     //丢弃状态状态   
}EN_RECV_STATE;

/** 
  * @brief 接收数据包结构体    
  */
typedef struct
{
	unsigned char  ucADDRCODE;		/* 地址码 */
    unsigned char  ucFUNCODE;		/* 功能码 */
	unsigned char  ucByteNum;		/* 字节数 */
	unsigned char  ucStarAdd_H;		/* 起始地址_高 */
	unsigned char  ucStarAdd_L;		/* 起始地址_低 */
	unsigned char  ucRegData_H;		/* 寄存器数据_高 */
	unsigned char  ucRegData_L;		/* 寄存器数据_低 */
	unsigned char  ucCRC0;			/* CRC0校验 */
	unsigned char  ucCRC1;			/* CRC1校验 */
}ST_PACK_DATA,*PST_PACK_DATA;


/**
  * @}定义串口发送和接收循环缓冲队列结构体
  */
typedef struct
{
    unsigned char   aucBufRecv[256];
    tRingBufObject  stUartBufRecv;
}ST_USART_DATA, *PST_USART_DATA;



/* 函数声明-----------------------------------------------------------------------*/

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

