/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_Light.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  用户LED灯接口
  */ 
#include "app_cfg.h"
#include "app_led.h"
#include "app_uart.h"
#include "app_beep.h"
#include "bsp.h"
#include "bsp_led.h"



/**
  * @brief  LED灯主任务
 */
void App_LED_TASK(void *pdata)
{
	debug_printf( "[%s] enter.\r\n", __func__ );
	while(1)
	{
		OSTimeDly(500);/* 切换任务 */
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		OSTimeDly(500);/* 切换任务 */
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
	}
}

