/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  Beep_Driver.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  蜂鸣器的驱动
  */ 
#include "bsp.h"
#include "bsp_beep.h"

/**
  * @brief 蜂鸣器初始化
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
  * @brief 打开蜂鸣器
*/
void Beep_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**
  * @brief 关闭蜂鸣器
*/
void Beep_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}


