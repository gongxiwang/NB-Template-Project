/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_Beep.c
  * @author  netfire.liang
  * @version  V2.0.0
  * @date  08/01/2013
  * @brief  ����������
  */ 

#include "app_cfg.h"
#include "app_uart.h"
#include <bsp_beep.h>
#include "app_beep.h"
#include "stdio.h"
#include <stdarg.h>
#include "bsp.h"

/**
  * @brief ����������
 */
static OS_EVENT *Beep_Queue;

/**
  * @brief ���������г���
 */
#define beep_queue_size 3

/**
  * @brief ��������������
 */
static INT32U beep_queue[beep_queue_size];


/**
  * @brief ������������
  * @details ��
  * @param pdata ����
  * @retval ��
 */
void App_BEEP_TASK(void)
{
    INT8U err;
    INT32U beep_time;
    Beep_Queue = OSQCreate((void*)beep_queue, beep_queue_size);	/* ������Ϣ���� */
    assert_param(Beep_Queue);									/* �����кϷ��� */
	debug_printf( "[%s] enter!\r\n", __func__ );				/* debug */
	Beep_output_time(10);										/* ���� */
    while(1)
    {
		beep_time = (INT32U)OSQPend (Beep_Queue, 0, &err);
		Beep_on();
		OSTimeDly(beep_time);
		Beep_off();
		OSTimeDlyHMSM(0, 0, 0, 1);								/* ����������� */
    }
}

/**
  * @brief ���÷�������ʱ��
  * @details ��
  * @param time ʱ�䳤��
  * @retval ��
 */
void Beep_output_time(INT32U time)
{
	OSQPost(Beep_Queue, (void*)time);
}

/**
  * @brief �����������ʱ�����
  * @details ��
  * @param ��
  * @retval ��
 */
void Beep_clean(void)
{
    OSQFlush(Beep_Queue);
}

