/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_LCD_EN                              DEF_DISABLED
#define  APP_OS_PROBE_EN                         DEF_DISABLED
#define  APP_PROBE_COM_EN                        DEF_DISABLED

/*
*********************************************************************************************************
*                                            BSP CONFIGURATION
*********************************************************************************************************
*/

#define  BSP_CFG_LED_SPI2_EN                     DEF_ENABLED    /* Enable/disable LEDs on SPI port.                     */
#define  BSP_CFG_LED_PIOC_EN                     DEF_ENABLED    /* Enable/disable PIOC LEDs.                            */

/*
*********************************************************************************************************
*                                            任务优先级
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                               3	/* 任务开始 */
#define  APP_TASK_LED_PRIO                                 4	/* LED任务 */
#define  APP_TASK_USART_PRIO							   5	/* 串口任务 */
#define  APP_TASK_BEEP_PRIO							   	   6	/* 蜂鸣器任务 */
#define  APP_TASK_ADC_PRIO								   7	/* ADC任务 */
#define  APP_TASK_NBIOT_PRIO							   8	/* NBIOT模块任务 */
#define  APP_TASK_DEVICE_PRIO						   	   9	/* 设备处理任务 */
#define  APP_TASK_LORA_PRIO							   		10	/* NBIOT模块任务 */



#define  OS_PROBE_TASK_PRIO              (OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            任务堆栈大小
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         64
#define  APP_TASK_KBD_STK_SIZE                           128
#define  APP_TASK_USER_IF_STK_SIZE                       256
#define  OS_PROBE_TASK_STK_SIZE                          128



#define	 APP_TASK_LED_STK_SIZE       						64
#define	 APP_TASK_USART_STK_SIZE          					128
#define	 APP_TASK_BEEP_STK_SIZE        						128
#define	 APP_TASK_ADC_STK_SIZE          					128
#define  APP_TASK_NBIOT_STK_SIZE							512
#define  APP_TASK_LORA_STK_SIZE								512
#define  APP_TASK_DEVICE_STK_SIZE							128

/*
*********************************************************************************************************
*                                                 LCD
*********************************************************************************************************
*/

#define  DISP_BUS_WIDTH                                    4    /* LCD controller is accessed with a 4 bit bus.         */

/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

/*
*********************************************************************************************************
*                                                 PROBE
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                                     0    /* Task will be created for uC/Probe OS Plug-In.        */
#define  OS_PROBE_TMR_32_BITS                              0    /* uC/Probe OS Plugin timer is a 16-bit timer.          */
#define  OS_PROBE_TIMER_SEL                                2    /* Select timer 2.                                      */
#define  OS_PROBE_HOOKS_EN                                 0    /* Hooks to update OS_TCB profiling members included.   */
#define  OS_PROBE_USE_FP                                   0


#endif
