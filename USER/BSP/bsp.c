/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <bsp.h>
#include "bsp_uart.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_beep.h"
#include "bsp_key.h"

/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    RCC_DeInit();										/*��ֹ�жϡ�����PLL������HSE����HSE����LSE*/
    RCC_HSEConfig(RCC_HSE_ON);							/* �ⲿʱ�Ӵ� */
    RCC_WaitForHSEStartUp();							/* �ȴ�ʱ������ */
    
    RCC_HCLKConfig(RCC_SYSCLK_Div1);					/* HCLK ��AHB����ʱ�ӣ���ϵͳʱ��SYSCLK ��Ƶ�õ���һ�㲻��Ƶ������ϵͳʱ��72 */
    RCC_PCLK2Config(RCC_HCLK_Div1);						/* APB2ʱ�Ӳ���Ƶ72mhz */
    RCC_PCLK1Config(RCC_HCLK_Div2);						/* APB1 ����Ƶ36mhz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);					/* ADC ����Ƶ12mhz */
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/* 8*9 = 72(MHZ) */
    RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
        ;
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while (RCC_GetSYSCLKSource() != 0x08) {
        ;
    }
	BspInitClk();												/* ��ʼ��ʱ��. */
    BSP_LED_Init();                                             /* ��ʼ��LED. */
	BSP_UART_Init();    										/* ��ʼ������. */
    BSP_ADCS_Init(); 											/* ��ʼ��ADC. */
	BSP_Beep_Init();											/* ��ʼ��������. */
	BSP_DMA_Init();												/* ��ʼ��DMA. */
    BSP_KEY_Init();                                             /* ��ʼ���û����� */
}

/**
  * @brief  BspInitClkʱ�ӳ�ʼ��
 */
static void BspInitClk(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | 
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1, ENABLE);	/* ʹ��GPIO��ʱ�� */
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;


    freq = BSP_CPU_ClkFreq();
    return (freq);
}

