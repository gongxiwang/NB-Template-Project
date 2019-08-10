#include "bsp_dma.h"
#include "stm32f10x_dma.h"
#include "bsp.h"
#include "app_uart.h"
#include "stdio.h"
#include "string.h"
#include "app_adc.h"
#include "bsp_uart.h"
#include "app_beep.h"
/* ����DMA���͵����ڵ�Buffer ��С */
#define SENDBUFF_SIZE (64)
#define NBDMABUFF_SIZE 512

/* �������ADCת�������Ҳ��DMA��Ŀ���ַ,4ͨ����ÿͨ���ɼ�10�κ���ȡƽ���� */
volatile unsigned long ADCConvertedValue[10][4];

/* ������ŷ��͵�usart2������ */
static unsigned char uiSendbuf[NBDMABUFF_SIZE] = {0};
/* ������ŷ��͵�usart1������ */
static unsigned char uiSendbuf1[SENDBUFF_SIZE] = {0};
/* ������Ž��յ�usart2������ */
static unsigned char uiReceivebuf[NBDMABUFF_SIZE] = {0};
/* ������Ž��յ�usart������ */
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&(ADC2->DR);       /* �����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (unsigned int)&ADCConvertedValue;    /* �������ݵ�ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* �ò����涨������������Ϊ���ݴ����Ŀ�ĵػ�����Դ���˴�����Ϊ��Դ */
    DMA_InitStructure.DMA_BufferSize = 4*10;                                    /* ��ָ��DMAͨ����DMA����Ĵ�С,��λΪ���ݵ�λ������Ҳ����ADCConvertedValue�Ĵ�С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* �趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* �����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         /* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /* ������ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /* DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ��� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* ʹ��DMAͨ�����ڴ浽�ڴ洫�� */
    DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel1_BASE, &DMA_InitStructure);	/* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� */
	
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel1_BASE, ENABLE);                                             /* ����DMAͨ��һ */
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;				/* �����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiSendbuf;    					/* �������ݵ�ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          /* ������ڴ浽���� */
    DMA_InitStructure.DMA_BufferSize = NBDMABUFF_SIZE;                           /* �����С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* �趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* �����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* ������ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     /* DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ��� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* ʹ��DMAͨ�����ڴ浽�ڴ洫�� */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel7_BASE, &DMA_InitStructure);    /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel7_BASE, ENABLE);                                             /* ����DMAͨ��һ */
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;				/* �����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiSendbuf1;    					/* �������ݵ�ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          /* ������ڴ浽���� */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* �����С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* �趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* �����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* ������ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     /* DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ��� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* ʹ��DMAͨ�����ڴ浽�ڴ洫�� */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, &DMA_InitStructure);    /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� */
	DMA_ITConfig((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel4_BASE, ENABLE);                                             /* ����DMAͨ��һ */
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;				/* �����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uiReceivebuf;    				/* �������ݵ�ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* ������ڴ浽���� */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* �����С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* �趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* �����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* ������ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     /* DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ��� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* ʹ��DMAͨ�����ڴ浽�ڴ洫�� */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel6_BASE, &DMA_InitStructure);    /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel6_BASE, ENABLE);                                             /* ����DMAͨ��һ */
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;				/* �����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart1Receivebuf;    				/* �������ݵ�ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* ������ڴ浽���� */
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;                           /* �����С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* �趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* �����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         	/* ���ݿ��Ϊ16λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             	/* ������ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     /* DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ��� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* ʹ��DMAͨ�����ڴ浽�ڴ洫�� */
	DMA_Init((DMA_Channel_TypeDef *)DMA1_Channel5_BASE, &DMA_InitStructure);    /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� */
	DMA_Cmd((DMA_Channel_TypeDef *)DMA1_Channel5_BASE, ENABLE);                                             /* ����DMAͨ��һ */
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
	BSP_IntVectSet(BSP_INT_ID_DMA1_CH4, BSP_IntHandlerDMA1_CH4); 				//���ô���1���ж�����������Ҫ�е�
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
		/* ��佫Ҫ���͵����� */
		memcpy(uiSendbuf, buffer, strlen(buffer));
		/* ����DMA���� */
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		/* �ȴ�ͨ��7������� */
		while(DMA1_Channel7->CNDTR != 0);
		/* Test */
		debug_printf("DMA:%s\r\n", uiSendbuf);
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC7);	
		/* ����������� */
		memset(uiSendbuf, 0, NBDMABUFF_SIZE);
		/* ��������˵������ͨ����ģʽÿ�η��������Ҫ���¶�DMA���г�ʼ�� */
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
		/* ��佫Ҫ���͵����� */
		memcpy((char *)uiSendbuf, buffer, strlen(buffer));
		/* ����DMA���� */
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		/* �ȴ�ͨ��7������� */
		while(DMA1_Channel7->CNDTR != 0);
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC7);

		/* Test */
		debug_printf("DMA:%s\r\n", uiSendbuf);
		/* ����������� */
		memset(uiSendbuf, 0, NBDMABUFF_SIZE);
		/* ��������˵������ͨ����ģʽÿ�η��������Ҫ���¶�DMA���г�ʼ�� */
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
		memcpy((char*)uiSendbuf1, (char*)buffer, Size);		/* ��佫Ҫ���͵�����--�˴�ʹ��memcpy�Ϻ� */	
		USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	    /* ����DMA���� */		
		while(DMA1_Channel4->CNDTR != 0);				    /* �ȴ�ͨ��4������� */	
		OSTimeDlyHMSM(0,0,0,100);						    /* ʵ��֤����Ҫ�ӳ�10ms����ʹ���������Ҳ����� */
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC4);	
		memset(uiSendbuf1, 0, SENDBUFF_SIZE);			/* ����������� */		
		BSP_USART1DMASEND_Init();						/* ��������˵������ͨ����ģʽÿ�η��������Ҫ���¶�DMA���г�ʼ�� */
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
		/* ����DMA���� */
		USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);
		BSP_USART2RXDMA_Init();
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
		/* �ȴ�2S���� */
		OSTimeDlyHMSM(0,0,2,0);
		
		/* ��䵽buff���յ����� */
		memcpy((char *)buffer, (char *)uiReceivebuf, SENDBUFF_SIZE);
		debug_printf("NBiotDMA_Receive_Data��%s", uiReceivebuf);
		/* ����������� */
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
		/* ����DMA���� */
		USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
		BSP_USART1RXDMA_Init();
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		/* �ȴ�ͨ��7������� */
		//while(DMA1_Channel5->CNDTR != 0);
		OSTimeDlyHMSM(0,0,0,10);
		/* ��佫Ҫ���͵����� */
		strcpy((char *)buffer, (char *)Uart1Receivebuf);
		/* ����������� */
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
    	if(GETUSART_MODE() == EN_USART_RS485)	/* �жϴ���ģʽ��RS232����RS485 */
       		ENRS485_RX;							/* ��������жϺ��RS485�Ľ���ģʽ */
		//Beep_output_time(100);				/* ���� */
		OSTimeDlyHMSM(0,0,0,100);				/* ʹ�ܽ��պ���Ҫ�ӳ�10ms */
        DMA_ClearITPendingBit(DMA1_IT_GL4); 	/* ���ȫ���жϱ�־ */
    }
	OSIntExit(); 
}

