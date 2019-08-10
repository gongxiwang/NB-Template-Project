/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  app_led.c
  * @author walle
  * @version  V2.0.0
  * @date  08/01/2019
  * @brief  �û�LED�ƽӿ�
  */ 

#include "bsp_led.h"
#include "bsp.h"


/**
  * @brief LED�ƿ��ƺ��������ĸ��Ƶ���
  * @param index LED�Ƶ����
  * @param on 1:����0:Ϩ��
 */

void  BSP_LED_Init (void)
{
    GPIO_InitTypeDef  gpio_init;
    gpio_init.GPIO_Pin   = GPIO_Pin_0;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
}

/**
  * @brief LED�ƿ��ƺ��������ĸ��Ƶ���
  * @param index LED�Ƶ����
  * @param on 1:����0:Ϩ��
 */
void Light_control(int index, int on)
{
	switch(index)
	{
		case 0:
		GPIO_WriteBit(LED_BLUE_GPIOx, LED_BLUE_PINx, (on ? Bit_SET : Bit_RESET));
		break;
		case 1:
		GPIO_WriteBit(LED_RED_GPIOx, LED_RED_PINx, (on ? Bit_SET : Bit_RESET));
		break;
		case 2:
		GPIO_WriteBit(LED_RED_GPIOx, LED_RED_PINx, (on ? Bit_SET : Bit_RESET));
		break;
		default:
		assert_param(0);
	}
}

/**
  * @brief ���еĵ�ȫ���ر�
  * @param NULL ��
  * @retval NULL ��
 */
void Light_all_off(void)
{
	int index;
	for(index = 0; index < LIGHT_NUMBER; ++index)
	{
		Light_control(index, EN_LED_OFF);
	}
}

/**
  * @brief �������еĵ�
  * @param NULL ��
  * @retval NULL ��
 */
void Light_all_on(void)
{
	int index;
	for(index = 0; index < LIGHT_NUMBER; ++index)
	{
		Light_control(index, EN_LED_ON);
	}
}

