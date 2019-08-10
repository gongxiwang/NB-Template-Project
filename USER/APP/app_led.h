/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_Light.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  用户LED灯接口
  */ 
#ifndef __APP_LED__H
#define __APP_LED__H
#include "bsp.h"

/* Modes */
#define LED_MODE_OFF     0x00
#define LED_MODE_ON      0x01
#define LED_MODE_BLINK   0x02
#define LED_MODE_FLASH   0x04
#define LED_MODE_TOGGLE  0x08
#define LED_MODE_IDLE    0x10

/* Defaults */
#define LED_DEFAULT_MAX_LEDS      3
#define LED_DEFAULT_DUTY_CYCLE    5
#define LED_DEFAULT_FLASH_COUNT   50
#define LED_DEFAULT_FLASH_TIME    1000


/* LEDS - The LED number is the same as the bit position */
#define LED_BLUE    0x01
#define LED_YELLOW  0x02
#define LED_RED     0x04
#define LED_ALL   (LED_BLUE|LED_YELLOW|LED_RED)


/* LED control structure */
typedef struct {
	u8 device;
	u8 status;
	u8 mode;	/* Operation mode */
	u8 number_blinks;	/* Blink cycles left */
	u8 percent;	/* On cycle percentage */
	u8 period;	/* On/off cycle time (msec) */
	u8 next;	/* Time for next change */
} Led_control_t;

/* LED control structure */
typedef struct
{
	Led_control_t frame[LED_DEFAULT_MAX_LEDS];
} Led_status_t;



/**
  * @brief  函数声明
 */
void App_LED_TASK(void *pdata);
void app_led_blink (u8 leds, u8 numBlinks, u8 percent, u16 period);
void _led_set_status (u8 leds, u8 mode);
static void update_led_mode_blink(Led_control_t *led);
static void update_led_mode_on_off(Led_control_t *led, u8 on_off);
void app_led_blink (u8 leds, u8 number_blinks, u8 percent, u16 period);
void _led_set_status (u8 leds, u8 mode);

#endif
