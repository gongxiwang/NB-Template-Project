/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_adc.c
  * @author  walle
  * @version  V1.0.0
  * @date  10/05/2019
  * @brief  ADC驱动
  */ 
#ifndef __BSP_ADC__H
#define __BSP_ADC__H

#include "bsp.h"

/**
  * @brief 宏定义控制ADC采样电压和电流
 */
#define CTRL_ADC_VOL	GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define CTRL_ADC_CUR	GPIO_SetBits(GPIOB, GPIO_Pin_7)


void  BSP_ADCS_Init (void);
float BSP_ADC_GetVolt1Status(void);
void BSP_DMA_Init(void);

#endif
