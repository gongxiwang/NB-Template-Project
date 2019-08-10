/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  bsp_uart.c
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  uart�����ļ�
  */ 

/* ͷ�ļ�---------------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "stm32f10x_usart.h"
#include <bsp.h>
#include "ringbuf.h"

/** 
  * @brief�����շ��ṹ��    
  */
static ST_USART_DATA  g_stUsart1Data = {0};
static ST_USART_DATA g_stUsart2Data = {0};
ST_PACK_DATA   g_stRecvData = {0};
static EN_USART_MODE USART_MODE = EN_USART_UNDEF;				/* ���ڹ���ģʽ */


  




/** 
  * @brief BC35������Ϣ�ṹ��    
  */
static ST_UART_INFO st_bc35_uart_info = 
{
	USART2, 				//���ں�
	GPIOA, 				    //����GPIO
	GPIO_Pin_2, 		    //��������
	GPIO_Pin_3, 		    //��������
	9600,					//������
	BSP_INT_ID_USART2,
	USART2_IRQChannel,	    //�ж�ͨ��
	BSP_IntHandlerUSART2,
};

/** 
  * @brief LORA������Ϣ�ṹ��    
  */
static ST_UART_INFO st_lora_uart_info = 
{
	USART2, 				//���ں�
	GPIOA, 				    //����GPIO
	GPIO_Pin_2, 		    //��������
	GPIO_Pin_3, 		    //��������
	115200,					//������
	BSP_INT_ID_USART2,
	USART2_IRQChannel,	    //�ж�ͨ��
	BSP_IntHandlerUSART2,
};

/** 
  * @brief USART3��Ϣ�ṹ��    
  */
static ST_UART_INFO st_uart3_info = 
{
	USART3, 				//���ں�
	GPIOC, 				    //����GPIO
	GPIO_Pin_10, 		    //��������
	GPIO_Pin_11, 		    //��������
	115200,					//������
	BSP_INT_ID_USART3,
	USART3_IRQChannel,	    //�ж�ͨ��
	BSP_IntHandlerUSART3,
};

/** 
  * @brief USART1��Ϣ�ṹ��    
  */
static ST_UART_INFO st_uart1_info = 
{
	USART1, 				//���ں�
	GPIOA, 				    //����GPIO
	GPIO_Pin_9, 		    //��������
	GPIO_Pin_10, 		    //��������
	115200,					//������
	BSP_INT_ID_USART1,
	USART1_IRQChannel,	    //�ж�ͨ��
	BSP_IntHandlerUSART1,
};

/**
  * @}
  */


/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
void SetUSART_MODE(EN_USART_MODE en_MODE)
{
	USART_MODE = en_MODE;	/* ģʽ��ֵ */
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
EN_USART_MODE GETUSART_MODE(void)
{
	return USART_MODE;	/* ģʽֵ���� */
}



/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
static void Stm32UsartGPIOConfigInit (PST_UART_INFO pst_uart_info)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;						/* GPIO���� */
	if(pst_uart_info->USARTx == USART3)
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	}
	GPIO_InitStructure.GPIO_Pin = pst_uart_info->GPIO_Pin_tx;	//�˿�tx					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			    //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//����2MHz
	GPIO_Init(pst_uart_info->GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = pst_uart_info->GPIO_Pin_rx;	//�˿�rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//����2MHz
	GPIO_Init(pst_uart_info->GPIOx, &GPIO_InitStructure);

	/* ��ʼ��USART1���ڵ�RS485��RS232���ƶ˿� */
	if(pst_uart_info->USARTx == USART1)
	{
		GPIO_InitStructure.GPIO_Pin = CTRL_485PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//���ģʽ
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����50MHz
		GPIO_Init(CTRL_485GPIOx, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = CTRL_232PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//���ģʽ
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����50MHz
		GPIO_Init(CTRL_232GPIOx, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = EN_RS485PINx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//���ģʽ
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����50MHz
		GPIO_Init(EN_RS485GPIOx, &GPIO_InitStructure);
		GPIO_SetBits(EN_RS485GPIOx, EN_RS485PINx);
		OPENRS_232;
		//GPIO_ResetBits(EN_RS485GPIOx, EN_RS485PINx);
	}
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
static void Stm32UsartIntCfgInit(PST_UART_INFO pst_uart_info)
{
  	static int Proiority = 0;
	NVIC_InitTypeDef 	NVIC_InitStructure;									//�ж����Ͳ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = pst_uart_info->NVIC_IRQChannel;	//�ж�ͨ��ΪUSART1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Proiority++;		//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Proiority++;			//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//�жϹ���ʹ��
	NVIC_Init(&NVIC_InitStructure);											//��ʼ���ж�����	
	USART_ITConfig(pst_uart_info->USARTx, USART_IT_RXNE, ENABLE);
#if 0
	if(pst_uart_info->USARTx == USART1)
	{
  		USART_ITConfig(pst_uart_info->USARTx, USART_IT_TC, ENABLE);				//��������ж�	
		USART_ITConfig(pst_uart_info->USARTx, USART_IT_IDLE, ENABLE);			//���߿����ж�
	}
#endif
	BSP_IntVectSet(pst_uart_info->ucBSP_INT_ID, pst_uart_info->INT_Service_name); 				//���ô��ڵ��ж�����������Ҫ�е�
	BSP_IntEn(pst_uart_info->ucBSP_INT_ID);
	USART_Cmd(pst_uart_info->USARTx, ENABLE);
	USART_ClearFlag(pst_uart_info->USARTx, USART_FLAG_TC); 
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
static void Stm32UsartQueueInit(void)
{
	/* ����1���г�ʼ�� */
	RingBufInit(&g_stUsart1Data.stUartBufRecv, g_stUsart1Data.aucBufRecv, sizeof (g_stUsart1Data.aucBufRecv));

	/* ����2���г�ʼ�� */
	RingBufInit(&g_stUsart2Data.stUartBufRecv, g_stUsart2Data.aucBufRecv, sizeof (g_stUsart2Data.aucBufRecv));

}


/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
static void Stm32UsartConfigInit(PST_UART_INFO pst_uart_info)
{
    Stm32UsartGPIOConfigInit(pst_uart_info);    	//�������ڵ�GPIO�˿ڳ�ʼ������
    Stm32UsartParamConfigInit(pst_uart_info);		//���ڲ�����Ϣ���ó�ʼ������
    Stm32UsartIntCfgInit(pst_uart_info);         	//�����ж����ó�ʼ������
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
	InitDebugPrintf();			/* ��ʼ��Debug��ӡ */
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
			unsigned char ucReceiveData = USART_ReceiveData(USARTx);	/* �������ݡ����� */
		}
		if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)		/* ��ȡ���ݼĴ���Ϊ�� */
			break;
	}
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * @brief	�ȴ����մ�������
  * @details	
  * @param 	
  * @retval 	
  */
u8 WaitUartDataReceive(u8* RecvBuffer, u8 ExpectLen)
{
	u32 ulReceiveLen = 0;									/* ����Ľ������ݳ��� */
	ulReceiveLen = Uart1RecvData((u8*)RecvBuffer);			/* ���մ���1������������ */
	if(ulReceiveLen >= ExpectLen)								/* �жϽ��յ������ݳ��� */
	{
		/* ��ӡ���� */
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
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
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
void BSP_IntHandlerUSART1(void)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();                         				/* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)		/* �����ж� */
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			/* �����Ӧ�ж�λ */
		UartDataReceiveStorage(USART1, &g_stUsart1Data);
		USART_SendData(USART3, USART_ReceiveData(USART1));		/* ���Բ��� */
	}
#if 0
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)		/* ���߿����ж� */
	{
        USART1->SR;												/* �ȶ�SR�����DR����������߿���λ */
        USART1->DR;	
	}
#endif
	OSIntExit();                                /* Tell uC/OS-II that we are leaving the ISR */
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
void BSP_IntHandlerUSART2(void)
{
	OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();                         				/* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)		/* �����ж� */
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);			/* �����Ӧ�ж�λ */
		//USART_SendData(USART3, USART_ReceiveData(USART2));		/* ���Բ��� */
		UartDataReceiveStorage(USART2, &g_stUsart2Data);	
		
	}
 
	OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR */
}

/** 
  * �������ܣ�
  * ����˵����
  * ����˵����
  * ���÷�Χ��
  * ���ʱ�䣺
  * �޸�ʱ�䣺
  * �޸�ԭ��
  */
void BSP_IntHandlerUSART3(void)
{
	OS_CPU_SR  cpu_sr;
    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)		/* �����ж� */
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			/* �����Ӧ�ж�λ */	
		//UartDataReceiveStorage(&g_stUsartData);
		//USART_SendData(USART3, USART_ReceiveData(USART3));		/* ����ָ����� */
		USART_SendData(USART1, USART_ReceiveData(USART3));	
		
	}
	OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR */     
}

