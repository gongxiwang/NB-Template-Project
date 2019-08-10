/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  Beep_Driver.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  ������������
  */ 
#include "bsp.h"
#include "bsp_beep.h"

/**
  * @brief ��������ʼ��
*/
void BSP_Beep_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    gpio.GPIO_Pin = GPIO_Pin_15;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;	
    gpio.GPIO_Speed = GPIO_Speed_50MHz;	
    GPIO_Init(GPIOB, &gpio);
    Beep_off();
}

/**
  * @brief �򿪷�����
*/
void Beep_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**
  * @brief �رշ�����
*/
void Beep_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}


