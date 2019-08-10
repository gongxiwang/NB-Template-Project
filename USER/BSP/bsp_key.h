/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  InputKey_Driver.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  °´¼üµÄÇý¶¯
  */ 
#ifndef BSP_KEY_H
#define BSP_KEY_H




#define KEY_NUMBER 1

#define Key_Pin       GPIO_Pin_0
#define Key_Port      GPIOA
#define Key_ClockCmd  RCC_APB2Periph_GPIOA

void BSP_KEY_Init(void);
int InputKey_GetKey(int index);

#endif
