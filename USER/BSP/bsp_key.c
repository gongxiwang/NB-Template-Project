/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  InputKey_Driver.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  按键的驱动
  */ 
#include "bsp_key.h"
#include "bsp.h"

/**
  * @brief 按键初始化
 */
void BSP_KEY_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(Key_ClockCmd, ENABLE);
    
    gpio.GPIO_Pin = Key_Pin;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_Init(Key_Port, &gpio);
}

/**
  * @brief 获得按键状态
 */
int InputKey_GetKey(int index)
{
    switch(index)
    {
      case 0:
         return !GPIO_ReadInputDataBit(Key_Port, Key_Pin);
         break;
      default:
          assert_param(0);
          break;
    }   
    return 0;
}
