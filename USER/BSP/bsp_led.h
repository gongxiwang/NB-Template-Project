/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  bsp_led.h
  * @author  walle
  * @version  V2.0.0
  * @date  08/01/2019
  * @brief  用户LED灯接口
  */ 

#ifndef __BSP_LED__H
#define __BSP_LED__H

/**
  * @brief LED接口宏定义
 */
#define LED_BLUE_GPIOx GPIOB
#define LED_RED_GPIOx GPIOB
#define LED_YELLOW_GPIOx GPIOB
#define LED_GREEN_GPIOx GPIOB

#define LED_BLUE_PINx GPIO_Pin_0
#define LED_RED_PINx GPIO_Pin_0
#define LED_YELLOW_PINx GPIO_Pin_0
#define LED_GREEN_PINx GPIO_Pin_0

/* LIGHT的数量 */
#define LIGHT_NUMBER 3

/**
  * @brief LED ON\OFF
 */
 typedef enum
{
	EN_LED_OFF = 0x00,/* LED OFF */
	EN_LED_ON = 0x01,/* LED_ON */
}EN_LED_SWITCH;


/**
  * @brief 函数声明
 */
void  BSP_LED_Init (void);
void BSP_LED_ON(void);
void BSP_LED_OFF(void);

void Light_control(int index, int on);
#endif
