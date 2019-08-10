#include "bsp_dma.h"
#include "stm32f10x_dma.h"
#include "bsp.h"
#include "app_uart.h"
#include "stdio.h"
#include "string.h"
#include "app_adc.h"
#include "bsp_uart.h"
#include "app_beep.h"
/* 定义DMA发送到串口的Buffer 大小 */
#define SENDBUFF_SIZE (64)
#define NBDMABUFF_SIZE 512

/* 用来存放ADC转换结果，也是DMA的目标地址,4通道，每通道采集10次后面取平均数 */
volatile unsigned long ADCConvertedValue[10][4];

/* 用来存放发送到usart2的数据 */
static unsigned char uiSendbuf[NBDMABUFF_SIZE] = {0};
/* 用来存放发送到usart1的数据 */
static unsigned char uiSendbuf1[SENDBUFF_SIZE] = {0};
/* 用来存放接收到usart2的数据 */
static unsigned char uiReceivebuf[NBDMABUFF_SIZE] = {0};
/* 用来存放接收到usart的数据 */
static unsigned char Uart1Receivebuf[SENDBUFF_SIZE] = {0};

/*
*********************************************************************************************************
*                                             BSP_ADCDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/        
static void BSP_ADCDMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit((DMA_Channel_TypeDef *)DMA1_Channel1_BASE);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&(ADC2->DR);       /* 外设地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (unsigned int)&ADCConvertedValue;    /* 传送数据地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* 该参数规定定了外设是作为数据传输的目的地还是来源，此处是作为来源 */
    DMA_InitStructure.DMA_BufferSize = 4*10;                                    /* 义指定DMA通道的DMA缓存的大小,单位为数据单位。这里也就是ADCConvertedValue的大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 设定外设地址寄存器递增与否,此处设为不变 Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 用来设定内存地址寄存器递增与否,此处设为递增，Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /* 数据宽度为16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         /* 数据宽度为16位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /* 工作在循环缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /* DMA通道拥有高优先级 分别4个等级 低、中、高、非常高 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 使能DMA通道的内存到内存传输 */
    DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel1_BASE, &DMA_InitStructure);	/* 根据DMA_InitStruct中指定的参数初始化DMA的通道 */
	
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel1_BASE, ENABLE);                                             /* 启动DMA通道一 */
}


/*
*********************************************************************************************************
*                                             BSP_USARTDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/        
static void BSP_USART2DMASEND_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit((DMA_Channel_TypeDef *)DMA1_Channel7_BASE);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;				/* 外设地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiSendbuf;    					/* 传送数据地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          /* 方向从内存到外设 */
    DMA_InitStructure.DMA_BufferSize = NBDMABUFF_SIZE;                           /* 传输大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 设定外设地址寄存器递增与否,此处设为不变 Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 用来设定内存地址寄存器递增与否,此处设为递增，Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* 工作在循环缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     /* DMA通道拥有高优先级 分别4个等级 低、中、高、非常高 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 使能DMA通道的内存到内存传输 */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel7_BASE, &DMA_InitStructure);    /* 根据DMA_InitStruct中指定的参数初始化DMA的通道 */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel7_BASE, ENABLE);                                             /* 启动DMA通道一 */
}

/*
*********************************************************************************************************
*                                             BSP_USART1DMASEND_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/        
static void BSP_USART1DMASEND_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit((DMA_Channel_TypeDef *)DMA1_Channel4_BASE);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;				/* 外设地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiSendbuf1;    					/* 传送数据地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          /* 方向从内存到外设 */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* 传输大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 设定外设地址寄存器递增与否,此处设为不变 Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 用来设定内存地址寄存器递增与否,此处设为递增，Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* 工作在循环缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     /* DMA通道拥有高优先级 分别4个等级 低、中、高、非常高 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 使能DMA通道的内存到内存传输 */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, &DMA_InitStructure);    /* 根据DMA_InitStruct中指定的参数初始化DMA的通道 */
	DMA_ITConfig((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, DMA_IT_TC,ENABLE);  //配置DMA发送完成后产生中断
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, ENABLE);                                             /* 启动DMA通道一 */
}


/*
*********************************************************************************************************
*                                             BSP_USARTDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/        
static void BSP_USART2RXDMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit((DMA_Channel_TypeDef *)DMA1_Channel6_BASE);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;				/* 外设地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiReceivebuf;    				/* 传送数据地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* 方向从内存到外设 */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* 传输大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 设定外设地址寄存器递增与否,此处设为不变 Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 用来设定内存地址寄存器递增与否,此处设为递增，Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* 工作在循环缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     /* DMA通道拥有高优先级 分别4个等级 低、中、高、非常高 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 使能DMA通道的内存到内存传输 */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel6_BASE, &DMA_InitStructure);    /* 根据DMA_InitStruct中指定的参数初始化DMA的通道 */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel6_BASE, ENABLE);                                             /* 启动DMA通道一 */
}

/*
*********************************************************************************************************
*                                             BSP_USARTDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/        
static void BSP_USART1RXDMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit((DMA_Channel_TypeDef *)DMA1_Channel5_BASE);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;				/* 外设地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart1Receivebuf;    				/* 传送数据地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* 方向从内存到外设 */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* 传输大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 设定外设地址寄存器递增与否,此处设为不变 Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 用来设定内存地址寄存器递增与否,此处设为递增，Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* 数据宽度为16位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* 工作在循环缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     /* DMA通道拥有高优先级 分别4个等级 低、中、高、非常高 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 使能DMA通道的内存到内存传输 */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel5_BASE, &DMA_InitStructure);    /* 根据DMA_InitStruct中指定的参数初始化DMA的通道 */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel5_BASE, ENABLE);                                             /* 启动DMA通道一 */
}


/*
*********************************************************************************************************
*                                             BSP_USARTDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void DMANVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);     
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQChannel;     
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
	NVIC_Init(&NVIC_InitStructure);
	BSP_IntVectSet(BSP_INT_ID_DMA1_CH4, BSP_IntHandlerDMA1_CH4); 				//设置串口1的中断向量，必须要有的
	BSP_IntEn(BSP_INT_ID_DMA1_CH4);
}

/*
*********************************************************************************************************
*                                             BSP_USARTDMA_Init()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_DMA_Init(void)
{
	//BSP_ADCDMA_Init();
	BSP_USART2DMASEND_Init();
	BSP_USART2RXDMA_Init();
	BSP_USART1RXDMA_Init();
	BSP_USART1DMASEND_Init();
	DMANVIC_Configuration();
}

/*
*********************************************************************************************************
*                                             NBiotDMA_Send_Data()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void NBiotDMA_Send_Data(char *buffer)
{
	if(buffer != NULL)
	{
		/* 填充将要发送的数据 */
		memcpy(uiSendbuf, buffer, strlen(buffer));
		/* 启动DMA发送 */
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		/* 等待通道7传输完成 */
		while(DMA1_Channel7->CNDTR != 0);
		/* Test */
		debug_printf("DMA:%s\r\n", uiSendbuf);
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC7);	
		/* 发送完成清零 */
		memset(uiSendbuf, 0, NBDMABUFF_SIZE);
		/* 依据网上说法：普通发送模式每次发送完成需要重新对DMA进行初始化 */
		BSP_USART2DMASEND_Init();
	}
}

/*
*********************************************************************************************************
*                                             NBiotDMA_Send_Data()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void LoRaDMA_Send_Data(char *buffer)
{
	if(buffer != NULL)
	{
		/* 填充将要发送的数据 */
		memcpy((char *)uiSendbuf, buffer, strlen(buffer));
		/* 启动DMA发送 */
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		/* 等待通道7传输完成 */
		while(DMA1_Channel7->CNDTR != 0);
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC7);

		/* Test */
		debug_printf("DMA:%s\r\n", uiSendbuf);
		/* 发送完成清零 */
		memset(uiSendbuf, 0, NBDMABUFF_SIZE);
		/* 依据网上说法：普通发送模式每次发送完成需要重新对DMA进行初始化 */
		BSP_USART2DMASEND_Init();
	}
}

/*
*********************************************************************************************************
*                                             NBiotDMA_Send_Data()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void USART1DMA_Send_Data(u8 *buffer, int Size)
{
	if(buffer != NULL)
	{	
		memcpy((char*)uiSendbuf1, (char*)buffer, Size);		/* 填充将要发送的数据--此处使用memcpy较好 */	
		USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	    /* 启动DMA发送 */		
		while(DMA1_Channel4->CNDTR != 0);				    /* 等待通道4传输完成 */	
		OSTimeDlyHMSM(0,0,0,100);						    /* 实测证明需要延迟10ms左右使发送完整且不乱码 */
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC4);	
		memset(uiSendbuf1, 0, SENDBUFF_SIZE);			/* 发送完成清零 */		
		BSP_USART1DMASEND_Init();						/* 依据网上说法：普通发送模式每次发送完成需要重新对DMA进行初始化 */
	}
}

/*
*********************************************************************************************************
*                                             NBiotDMA_Receive_Data()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void NBiotDMA_Receive_Data(u8 *buffer)
{
	if(buffer != NULL)
	{
		/* 启动DMA接收 */
		USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);
		BSP_USART2RXDMA_Init();
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
		/* 等待2S左右 */
		OSTimeDlyHMSM(0,0,2,0);
		
		/* 填充到buff接收的数据 */
		memcpy((char *)buffer, (char *)uiReceivebuf, SENDBUFF_SIZE);
		debug_printf("NBiotDMA_Receive_Data：%s", uiReceivebuf);
		/* 发送完成清零 */
		memset(uiReceivebuf, 0, SENDBUFF_SIZE);
	}
}

/*
*********************************************************************************************************
*                                             USART1DMA_Receive_Data()
*
* Description : Initialize the board's DMA
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART1DMA_Receive_Data(u8 *buffer)
{
	if(buffer != 0)
	{
		/* 启动DMA接收 */
		USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
		BSP_USART1RXDMA_Init();
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		/* 等待通道7传输完成 */
		//while(DMA1_Channel5->CNDTR != 0);
		OSTimeDlyHMSM(0,0,0,10);
		/* 填充将要发送的数据 */
		strcpy((char *)buffer, (char *)Uart1Receivebuf);
		/* 发送完成清零 */
		memset(Uart1Receivebuf, 0, SENDBUFF_SIZE);
	}
}

/*
*********************************************************************************************************
*                                           BSP_ADC_GetVoltStatus(void)
*
* Description : This function initializes the board's ADC
*
* Argument(s) : adc             ID of the ADC to probe.  For this board, the only legitimate value is 1.
*
* Return(s)   : The numerator of the binary fraction representing the result of the latest ADC conversion.
*               This value will be a 12-bit value between 0x0000 and 0x0FFF, inclusive.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_IntHandlerDMA1_CH4      (void)
{
	OS_CPU_SR  cpu_sr;
    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	if(DMA_GetITStatus(DMA1_IT_TC4))
    {
    	if(GETUSART_MODE() == EN_USART_RS485)	/* 判断串口模式是RS232还是RS485 */
       		ENRS485_RX;							/* 发送完成中断后打开RS485的接收模式 */
		//Beep_output_time(100);				/* 测试 */
		OSTimeDlyHMSM(0,0,0,100);				/* 使能接收后需要延迟10ms */
        DMA_ClearITPendingBit(DMA1_IT_GL4); 	/* 清除全部中断标志 */
    }
	OSIntExit(); 
}

