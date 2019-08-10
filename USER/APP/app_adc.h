/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_adc.h
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  ADC������
  */ 
#ifndef __APP_ADC__H
#define __APP_ADC__H
#include "bsp.h"
/**
  * @brief ����ˮ���¶ȴ����� �궨��
 */
#define OFFSET                  	0
#define VMIN_C                    (590 + OFFSET)   /* ʵ��ֵ ��Ӧ480mv -- 2400mv */
#define VMAX_C                    (2975 + OFFSET)

/**
  * @brief ˮ�����̷�Χ
 */
#define RMIN_C                    	0 
#define RMAX_C                    	100

/**
  * @brief ADC������ѹ�͵���ͨ��
 */
#define ADC_SAMPLE_VOLCHANNEL 0x01
#define ADC_SAMPLE_CURCHANNEL 0x02





void App_ADC_TASK(void);
u16 APP_GetCSF11(u8 channel);
#endif