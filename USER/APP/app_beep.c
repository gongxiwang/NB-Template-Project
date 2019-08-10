/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_Beep.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  蜂鸣器程序
  */ 

#include "app_cfg.h"
#include "app_uart.h"
#include <bsp_beep.h>
#include "app_beep.h"
#include "stdio.h"
#include <stdarg.h>
#include "bsp.h"

/**
  * @brief 蜂鸣器队列
 */
static OS_EVENT *Beep_Queue;

/**
  * @brief 蜂鸣器队列长度
 */
#define beep_queue_size 3

/**
  * @brief 蜂鸣器队列数据
 */
static INT32U beep_queue[beep_queue_size];


/**
  * @brief 蜂鸣器主任务
  * @details 无
  * @param pdata 无用
  * @retval 无
 */
void App_BEEP_TASK(void)
{
    INT8U err;
    INT32U beep_time;
    Beep_Queue = OSQCreate((void*)beep_queue, beep_queue_size);	/* 创建消息队列 */
    assert_param(Beep_Queue);									/* 检查队列合法性 */
	debug_printf( "[%s] enter!\r\n", __func__ );				/* debug */
	Beep_output_time(10);										/* 测试 */
    while(1)
    {
		beep_time = (INT32U)OSQPend (Beep_Queue, 0, &err);
		Beep_on();
		OSTimeDly(beep_time);
		Beep_off();
		OSTimeDlyHMSM(0, 0, 0, 1);								/* 用于任务调度 */
    }
}

/**
  * @brief 设置蜂鸣器的时间
  * @details 无
  * @param time 时间长度
  * @retval 无
 */
void Beep_output_time(INT32U time)
{
	OSQPost(Beep_Queue, (void*)time);
}

/**
  * @brief 清除蜂鸣器的时间队列
  * @details 无
  * @param 无
  * @retval 无
 */
void Beep_clean(void)
{
    OSQFlush(Beep_Queue);
}

