/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_Light.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  �û�LED�ƽӿ�
  */ 
#include "app_cfg.h"
#include "app_led.h"
#include "app_uart.h"
#include "app_beep.h"
#include "bsp.h"
#include "bsp_led.h"



/**
  * @brief  LED��������
 */
void App_LED_TASK(void *pdata)
{
	debug_printf( "[%s] enter.\r\n", __func__ );
	while(1)
	{
		OSTimeDly(500);/* �л����� */
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		OSTimeDly(500);/* �л����� */
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
	}
}

