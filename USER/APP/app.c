/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app.c
  * @author  walle
  * @version  V1.0
  * @date  20/05/2019
  * @brief  app.c
  */

#include <includes.h>
#include "app_uart.h"
#include "app_led.h"
#include "app_beep.h"
#include "app_adc.h"
#include "app_nbiot.h"
#include "app_device.h"
#include "app_lora.h"


/**
  * @brief ��������Ķ�ջ��С
*/
static  OS_STK          App_TaskStartStk[APP_TASK_START_STK_SIZE];		/* ��ʼ�����ջ��С */
static  OS_STK          App_TaskLedStk[APP_TASK_LED_STK_SIZE];			/* LED�����ջ��С */
static  OS_STK          App_TaskAdcStk[APP_TASK_ADC_STK_SIZE];			/* ADC�����ջ��С */
static  OS_STK          App_TaskBeepStk[APP_TASK_BEEP_STK_SIZE];		/* BEEP��ջ��С */
static  OS_STK			App_TaskNbiotStk[APP_TASK_NBIOT_STK_SIZE];		/* NBIOT��ջ��С */
static  OS_STK          App_TaskDeviceStk[APP_TASK_DEVICE_STK_SIZE];	/* DEVICE�����ջ��С */
static  OS_STK			App_TaskLoraStk[APP_TASK_LORA_STK_SIZE];		/* NBIOT��ջ��С */



/**
  * @brief ��������
*/
static  void  App_TaskCreate             (void);
static  void  App_TaskStart              (void *p_arg);


/**
  * @brief	main()����
  * @details ��
  * @retval    ��
  */




int  main (void)
{
    CPU_INT08U  os_err;
    BSP_IntDisAll();                                            /* Disable all ints until we are ready to accept them.  */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel".         */

    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */						 
							 (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 11)
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */
  
    return (0);
}

/**
  * @brief	����ʼ
  * @details ��������
  * @retval    ��
  */
static  void  App_TaskStart (void *p_arg)
{
    (void)p_arg;
    BSP_Init();                                                 /* BSP�����ʼ��*/
	debug_printf("BSP Peripheral Init Over!\r\n");				/* debug */
    OS_CPU_SysTickInit();                                       /* ��ʼ��ϵͳʱ��.*/
    App_TaskCreate();                                           /* ��������.*/
	debug_printf("App Task Create Over!\r\n");					/* debug */
    OSTaskDel(APP_TASK_START_PRIO);                             /* ɾ���������� */   
}

/**
  * @brief	���񴴽�
  * @details �ڲ��������񴴽�
  * @retval    ��
  */
static  void  App_TaskCreate (void)
{
    CPU_INT08U  os_err;
	/* ����LED���� */
    os_err = OSTaskCreateExt((void (*)(void *)) App_LED_TASK,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskLedStk[APP_TASK_LED_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_LED_PRIO,
                             (INT16U          ) APP_TASK_LED_PRIO,
                             (OS_STK        * )&App_TaskLedStk[0],
                             (INT32U          ) APP_TASK_LED_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_LED_PRIO, "LED Task", &os_err);
#endif

	/* ����ADC���� */
    os_err = OSTaskCreateExt((void (*)(void *)) App_ADC_TASK,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskAdcStk[APP_TASK_ADC_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_ADC_PRIO,
                             (INT16U          ) APP_TASK_ADC_PRIO,
                             (OS_STK        * )&App_TaskAdcStk[0],
                             (INT32U          ) APP_TASK_ADC_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_ADC_PRIO, "ADC TASK", &os_err);
#endif
	
	/* ����BEEP���� */
    os_err = OSTaskCreateExt((void (*)(void *)) App_BEEP_TASK,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskBeepStk[APP_TASK_BEEP_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_BEEP_PRIO,
                             (INT16U          ) APP_TASK_BEEP_PRIO,
                             (OS_STK        * )&App_TaskBeepStk[0],
                             (INT32U          ) APP_TASK_BEEP_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_BEEP_PRIO, "BEEP TASK", &os_err);
#endif

	/* �����豸���� */
	os_err = OSTaskCreateExt((void (*)(void *)) App_DEVICE_TASK,
								 (void			* ) 0,
								 (OS_STK		* )&App_TaskDeviceStk[APP_TASK_DEVICE_STK_SIZE - 1],
								 (INT8U 		  ) APP_TASK_DEVICE_PRIO,
								 (INT16U		  ) APP_TASK_DEVICE_PRIO,
								 (OS_STK		* )&App_TaskDeviceStk[0],
								 (INT32U		  ) APP_TASK_DEVICE_STK_SIZE,
								 (void			* ) 0,
								 (INT16U		  )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	
#if (OS_TASK_NAME_SIZE >= 9)
		OSTaskNameSet(APP_TASK_DEVICE_PRIO, "DEVICE TASK", &os_err);
#endif

#if defined (NBIOT_MODULE)	/* ���������NBIOTģ�� */
	/* ����NBIOT���� */
	os_err = OSTaskCreateExt((void (*)(void *)) App_NBIOT_TASK,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskNbiotStk[APP_TASK_NBIOT_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_NBIOT_PRIO,
                             (INT16U          ) APP_TASK_NBIOT_PRIO,
                             (OS_STK        * )&App_TaskNbiotStk[0],
                             (INT32U          ) APP_TASK_NBIOT_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_NBIOT_PRIO, "NBIOT TASK", &os_err);
#endif
	
#endif	

#if defined (LORA_MODULE)		/* ���������LORAģ�� */
	/* ����LORA���� */
	os_err = OSTaskCreateExt((void (*)(void *)) App_LORA_TASK,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskLoraStk[APP_TASK_LORA_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_LORA_PRIO,
                             (INT16U          ) APP_TASK_LORA_PRIO,
                             (OS_STK        * )&App_TaskNbiotStk[0],
                             (INT32U          ) APP_TASK_LORA_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_LORA_PRIO, "LORA TASK", &os_err);
#endif
#endif

}
