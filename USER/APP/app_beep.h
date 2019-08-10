/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_beep.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  beep接口
  */ 
#ifndef __APP_BEEP__H
#define __APP_BEEP__H


#include "bsp.h"


/**
  * @brief 函数声明
 */
void App_BEEP_TASK(void);
void Beep_output_time(INT32U time);
#endif