/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  bsp_uart.c
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  uart驱动文件
  */ 

/* 头文件---------------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "stm32f10x_usart.h"
#include <bsp.h>
#include "ringbuf.h"

/** 
  * @brief串口收发结构体    
  */
static ST_USART_DATA  g_stUsart1Data = {0};
static ST_USART_DATA g_stUsart2Data = {0};
ST_PACK_DATA   g_stRecvData = {0};
static EN_USART_MODE USART_MODE = EN_USART_UNDEF;				/* 串口工作模式 */


  




/** 
  * @brief BC35串口信息结构体    
  */
static ST_UART_INFO st_bc35_uart_info = 
{
	USART2, 				//串口号
	GPIOA, 				    //串口GPIO
	GPIO_Pin_2, 		    //发送引脚
	GPIO_Pin_3, 		    //接收引脚
	9600,					//波特率
	BSP_INT_ID_USART2,
	USART2_IRQChannel,	    //中断通道
	BSP_IntHandlerUSART2,
};

/** 
  * @brief LORA串口信息结构体    
  */
static ST_UART_INFO st_lora_uart_info = 
{
	USART2, 				//串口号
	GPIOA, 				    //串口GPIO
	GPIO_Pin_2, 		    //发送引脚
	GPIO_Pin_3, 		    //接收引脚
	115200,					//波特率
	BSP_INT_ID_USART2,
	USART2_IRQChannel,	    //中断通道
	BSP_IntHandlerUSART2,
};

/** 
  * @brief USART3信息结构体    
  */
static ST_UART_INFO st_uart3_info = 
{
	USART3, 				//串口号
	GPIOC, 				    //串口GPIO
	GPIO_Pin_10, 		    //发送引脚
	GPIO_Pin_11, 		    //接收引脚
	115200,					//波特率
	BSP_INT_ID_USART3,
	USART3_IRQChannel,	    //中断通道
	BSP_IntHandlerUSART3,
};

/** 
  * @brief USART1信息结构体    
  */
static ST_UART_INFO st_uart1_info = 
{
	USART1, 				//串口号
	GPIOA, 				    //串口GPIO
	GPIO_Pin_9, 		    //发送引脚
	GPIO_Pin_10, 		    //接收引脚
	115200,					//波特率
	BSP_INT_ID_USART1,
	USART1_IRQChannel,	    //中断通道
	BSP_IntHandlerUSART1,
};

/**
  * @}
  */


/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void SetUSART_MODE(EN_USART_MODE en_MODE)
{
	USART_MODE = en_MODE;	/* 模式赋值 */
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
EN_USART_MODE GETUSART_MODE(void)
{
	return USART_MODE;	/* 模式值返回 */
}



/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void Stm32UsartGPIOConfigInit (PST_UART_INFO pst_uart_info)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;						/* GPIO参数 */
	if(pst_uart_info->USARTx == USART3)
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	}
	GPIO_InitStructure.GPIO_Pin = pst_uart_info->GPIO_Pin_tx;	//端口tx					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			    //复用推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速率2MHz
	GPIO_Init(pst_uart_info->GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = pst_uart_info->GPIO_Pin_rx;	//端口rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速率2MHz
	GPIO_Init(pst_uart_info->GPIOx, &GPIO_InitStructure);

	/* 初始化USART1所在的RS485和RS232控制端口 */
	if(pst_uart_info->USARTx == USART1)
	{
		GPIO_InitStructure.GPIO_Pin = CTRL_485PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//输出模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速率50MHz
		GPIO_Init(CTRL_485GPIOx, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = CTRL_232PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//输出模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速率50MHz
		GPIO_Init(CTRL_232GPIOx, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = EN_RS485PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//输出模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速率50MHz
		GPIO_Init(EN_RS485GPIOx, &GPIO_InitStructure);
		GPIO_SetBits(EN_RS485GPIOx, EN_RS485PINx);
		OPENRS_232;
		//GPIO_ResetBits(EN_RS485GPIOx, EN_RS485PINx);
	}
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void Stm32UsartParamConfigInit(PST_UART_INFO pst_uart_info)
{
    USART_InitTypeDef	USART_InitStructure;			
    USART_InitStructure.USART_BaudRate = pst_uart_info->ulBaudRate;			
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_InitStructure.USART_Parity = USART_Parity_No;		
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_Init(pst_uart_info->USARTx, &USART_InitStructure);
}


/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void Stm32UsartIntCfgInit(PST_UART_INFO pst_uart_info)
{
  	static int Proiority = 0;
	NVIC_InitTypeDef 	NVIC_InitStructure;									//中断类型参数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = pst_uart_info->NVIC_IRQChannel;	//中断通道为USART1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Proiority++;		//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Proiority++;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//中断功能使能
	NVIC_Init(&NVIC_InitStructure);											//初始化中断配置	
	USART_ITConfig(pst_uart_info->USARTx, USART_IT_RXNE, ENABLE);
#if 0
	if(pst_uart_info->USARTx == USART1)
	{
  		USART_ITConfig(pst_uart_info->USARTx, USART_IT_TC, ENABLE);				//发送完成中断	
		USART_ITConfig(pst_uart_info->USARTx, USART_IT_IDLE, ENABLE);			//总线空闲中断
	}
#endif
	BSP_IntVectSet(pst_uart_info->ucBSP_INT_ID, pst_uart_info->INT_Service_name); 				//设置串口的中断向量，必须要有的
	BSP_IntEn(pst_uart_info->ucBSP_INT_ID);
	USART_Cmd(pst_uart_info->USARTx, ENABLE);
	USART_ClearFlag(pst_uart_info->USARTx, USART_FLAG_TC); 
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void Stm32UsartQueueInit(void)
{
	/* 串口1队列初始化 */
	RingBufInit(&g_stUsart1Data.stUartBufRecv, g_stUsart1Data.aucBufRecv, sizeof (g_stUsart1Data.aucBufRecv));

	/* 串口2队列初始化 */
	RingBufInit(&g_stUsart2Data.stUartBufRecv, g_stUsart2Data.aucBufRecv, sizeof (g_stUsart2Data.aucBufRecv));

}


/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void Stm32UsartConfigInit(PST_UART_INFO pst_uart_info)
{
    Stm32UsartGPIOConfigInit(pst_uart_info);    	//串口所在的GPIO端口初始化函数
    Stm32UsartParamConfigInit(pst_uart_info);		//串口参数信息配置初始化函数
    Stm32UsartIntCfgInit(pst_uart_info);         	//串口中断配置初始化函数
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void BSP_UART_Init(void)
{
#if defined (NBIOT_MODULE)
  	Stm32UsartConfigInit(&st_bc35_uart_info);
#else
  	Stm32UsartConfigInit(&st_lora_uart_info);
#endif
	Stm32UsartConfigInit(&st_uart1_info);
	Stm32UsartConfigInit(&st_uart3_info);
	Stm32UsartQueueInit();
//	OPENRS_232;
	InitDebugPrintf();			/* 初始化Debug打印 */
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
static void UartDataReceiveStorage(USART_TypeDef* USARTx, PST_USART_DATA pstUsartData)
{        
    for(;;)
	{
		if(RingBufFree(&pstUsartData->stUartBufRecv) > 0)
		{
            RingBufWriteOne(&pstUsartData->stUartBufRecv, USART_ReceiveData(USARTx));
		}
		else
		{	
			unsigned char ucReceiveData = USART_ReceiveData(USARTx);	/* 接收数据、抛弃 */
		}
		if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)		/* 读取数据寄存器为空 */
			break;
	}
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void Stm32UsartSendData(USART_TypeDef *USARTx,unsigned char *pucDataAddr,unsigned short usDataLen)
{
	unsigned short usi = 0;
	for(usi = 0; usi < usDataLen; usi ++)
	{
		while(RESET == USART_GetFlagStatus(USARTx, USART_FLAG_TXE))
			;
		USART_SendData(USARTx, pucDataAddr[usi]);
	}
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
int Module_SendMessage(int len, const void* src)
{
	if ((len > 0) && (0!=src))
	{
		Stm32UsartSendData(USART2, (unsigned char*)src, len & 0xFFFF);
		return 0x01;
	}
	return 0;
}


/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
PST_PACK_DATA GetStm32UartRecvData(void) 
{
	unsigned long ulRecvLen = Stm32UartRecvData((unsigned char *)&g_stRecvData);
	if(ulRecvLen > 0)
	{
		return &g_stRecvData;
	}
	return 0;
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
unsigned long Stm32UartRecvData(unsigned char* lpBuf)
{
    unsigned long ulRingBufUsedLen = RingBufUsed (&g_stUsart1Data.stUartBufRecv);
    if(ulRingBufUsedLen >= 0x08)
    {
    	
		if(PACK_CMD_DATA_LEN == ulRingBufUsedLen)
		{
			RingBufRead (&g_stUsart1Data.stUartBufRecv, lpBuf, PACK_CMD_DATA_LEN);
			return PACK_CMD_DATA_LEN;
		}
		else
		{
			//RingBufRead (&g_stUsart1Data.stUartBufRecv, lpBuf, ulRingBufUsedLen);
			return ulRingBufUsedLen;
		}
		//debug_printf("%d\r\n", ulRingBufUsedLen);
		//ulRingBufUsedLen = 0;
    }
    return 0;  
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
unsigned long Uart1RecvData(unsigned char* lpBuf)
{
	static unsigned long ulBufReadTime = 0;
    unsigned long ulRingBufUsedLen = RingBufUsed (&g_stUsart1Data.stUartBufRecv);
    if(ulRingBufUsedLen >= 0x01)
    {
    	if(++ulBufReadTime >= 200)
		{
			RingBufRead(&g_stUsart1Data.stUartBufRecv, lpBuf, ulRingBufUsedLen);
			ulBufReadTime = 0;
			return ulRingBufUsedLen;
		}
	}
	return 0;
}
/**
  * @brief	等待接收串口数据
  * @details	
  * @param 	
  * @retval 	
  */
u8 WaitUartDataReceive(u8* RecvBuffer, u8 ExpectLen)
{
	u32 ulReceiveLen = 0;									/* 保存的接收数据长度 */
	ulReceiveLen = Uart1RecvData((u8*)RecvBuffer);			/* 接收串口1传过来的数据 */
	if(ulReceiveLen >= ExpectLen)								/* 判断接收到的数据长度 */
	{
		/* 打印测试 */
		debug_printf("Sensor Recv:");
		for(int i = 0; i < ulReceiveLen; i ++)
		{
			debug_printf("%02X ", RecvBuffer[i]);
		}
		debug_printf("\r\n");
		return 0x00;
	}
	return 0x01;
}


/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
unsigned long NBUartRecvData(unsigned char* lpBuf)
{
	static unsigned long ulBufReadTime = 0;
    unsigned long ulRingBufUsedLen = RingBufUsed (&g_stUsart2Data.stUartBufRecv);
    if(ulRingBufUsedLen >= 0x01)
    {
    	if(++ulBufReadTime >= 2000)
		{
			RingBufRead (&g_stUsart2Data.stUartBufRecv, lpBuf, ulRingBufUsedLen);
			ulBufReadTime = 0;
			return ulRingBufUsedLen;
		}
    }
    return 0;  
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
unsigned long LORAUartRecvData(unsigned char* lpBuf)
{
	static unsigned long ulBufReadTime = 0;
    unsigned long ulRingBufUsedLen = RingBufUsed (&g_stUsart2Data.stUartBufRecv);
    if(ulRingBufUsedLen >= 0x01)
    {
    	if(++ulBufReadTime >= 2000)
		{
			RingBufRead (&g_stUsart2Data.stUartBufRecv, lpBuf, ulRingBufUsedLen);
			ulBufReadTime = 0;
			return ulRingBufUsedLen;
		}
    }
    return 0;  
}
/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void BSP_IntHandlerUSART1(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         				/* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)		/* 接收中断 */
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			/* 清除对应中断位 */
		UartDataReceiveStorage(USART1, &g_stUsart1Data);
		USART_SendData(USART3, USART_ReceiveData(USART1));		/* 回显测试 */
	}
#if 0
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)		/* 总线空闲中断 */
	{
        USART1->SR;												/* 先读SR，后读DR可以清楚总线空闲位 */
        USART1->DR;	
	}
#endif
	OSIntExit();                                /* Tell uC/OS-II that we are leaving the ISR */
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void BSP_IntHandlerUSART2(void)
{
	OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();                         				/* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)		/* 接收中断 */
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);			/* 清除对应中断位 */
		//USART_SendData(USART3, USART_ReceiveData(USART2));		/* 回显测试 */
		UartDataReceiveStorage(USART2, &g_stUsart2Data);	
		
	}
 
	OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR */
}

/** 
  * 函数功能：
  * 参数说明：
  * 返回说明：
  * 调用范围：
  * 完成时间：
  * 修改时间：
  * 修改原因：
  */
void BSP_IntHandlerUSART3(void)
{
	OS_CPU_SR  cpu_sr;
    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)		/* 接收中断 */
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			/* 清除对应中断位 */	
		//UartDataReceiveStorage(&g_stUsartData);
		//USART_SendData(USART3, USART_ReceiveData(USART3));		/* 发送指令回显 */
		USART_SendData(USART1, USART_ReceiveData(USART3));	
		
	}
	OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR */     
}

