/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_adc.h
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  ADC任务处理
  */ 
#ifndef __APP_ADC__H
#define __APP_ADC__H
#include "bsp.h"
/**
  * @brief 土壤水分温度传感器 宏定义
 */
#define OFFSET                  	0
#define VMIN_C                    (590 + OFFSET)   /* 实际值 对应480mv -- 2400mv */
#define VMAX_C                    (2975 + OFFSET)

/**
  * @brief 水分量程范围
 */
#define RMIN_C                    	0 
#define RMAX_C                    	100

/**
  * @brief ADC采样电压和电流通道
 */
#define ADC_SAMPLE_VOLCHANNEL 0x01
#define ADC_SAMPLE_CURCHANNEL 0x02





void App_ADC_TASK(void);
u16 APP_GetCSF11(u8 channel);
#endif