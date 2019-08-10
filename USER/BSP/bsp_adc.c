/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_adc.c
  * @author  walle
  * @version  V1.0.0
  * @date  10/05/2019
  * @brief  ADC驱动
  */ 
#include "bsp.h"
#include "bsp_dma.h"
#include "stm32f10x_dma.h"
#include "bsp_adc.h"
#include "app_uart.h"



/*
*********************************************************************************************************
*                                             BSP_ADCS_Init()
*
* Description : Initialize the board's ADC
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

void  BSP_ADCS_Init (void)
{
    ADC_InitTypeDef   adc_init;
    GPIO_InitTypeDef  gpio_init;
	ADC_DeInit(ADC1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
    gpio_init.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_init);

	gpio_init.GPIO_Pin  = GPIO_Pin_7;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);

    adc_init.ADC_Mode               = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode       = DISABLE;
    adc_init.ADC_ContinuousConvMode = DISABLE;						/* 使能连续转换 */
    adc_init.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc_init.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel       = 1;							/* 按顺序转换采样通道 */
    ADC_Init(ADC1, &adc_init);
#if 0
	/* 设置采样通道 */
    ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 2, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 3, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 4, ADC_SampleTime_13Cycles5);
#endif
	ADC_Cmd(ADC1, ENABLE);
   
	/* Enable ADC1 reset calibaration register */	 
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}






