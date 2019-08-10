/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		bsp_lora.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		LORA驱动函数
  */
#if 1
#include "bsp.h"
#include "stdio.h"
#include "string.h"
#include "bsp_lora.h"

/**
  * @brief		tim2初始化函数
  * @details	定时器2初始化，配置10ms中断
  * @param 	NULL	无
  * @retval 	NULL	无
  */
void LORA_Timer_Init(void)
{															
   	TIM_TimeBaseInitTypeDef  time;
    NVIC_InitTypeDef nvic; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	TIM_DeInit(TIM3);

    time.TIM_Period = 999;		 					/* 1s 自动重装载寄存器周期的值(计数值) */    											    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    time.TIM_Prescaler= (72 - 1);				    /* 时钟预分频数 72M/72 */
    time.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
    time.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
    TIM_TimeBaseInit(TIM3, &time);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);			/* 清除溢出中断标志 */
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);		
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);												
    nvic.NVIC_IRQChannel = TIM3_IRQChannel;	  
    nvic.NVIC_IRQChannelPreemptionPriority = 11;
    nvic.NVIC_IRQChannelSubPriority = 2;	
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	BSP_IntVectSet(BSP_INT_ID_TIM3, BSP_IntHandlerTIM3); 				//设置串口1的中断向量，必须要有的
	BSP_IntEn(BSP_INT_ID_TIM3);
}


/**
  * @brief tim2中断函数
  * @details	
  * @param 	NULL 无
  * @retval NULL 无
  */
#if 0
void BSP_IntHandlerTIM3(void)
{
	OS_CPU_SR  cpu_sr; 
	OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET ) 
	{
		TIM_Cmd(TIM3, DISABLE);	 
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update); 	  
	}
    OSIntExit();
}	
#endif

/**
  * @brief 将十六进制字符串转成数字
 */
u16 LORA_HexStrToNum(char* str)
{
	u16 i = 0;
	u8 tmp = 0;
	u8 tmp1 = 0;
	u16 len = strlen(str);
	if(len == 0)
	{
		return 0;
	}
	 
	for( i = 0 ; i < len; i ++)
	{
		if(str[i] >= '0' && str[i] <= '9')
	    {
			tmp1 = str[i] - '0';
	    }
	    else if(str[i] >= 'A' && str[i] <= 'F')
	    {
			tmp1 = str[i] - 'A' + 10; 
	    }
	    else if(str[i] >= 'a' && str[i] <= 'f')
	    {
			tmp1 = str[i] - 'a' + 10;
	    }
	    else
	    {
			tmp1 = 0;
	    }
	    
	    if((i % 2) == 0)
	    {
			tmp = tmp1; 
	    }
	    else
	    {
			tmp <<= 4;
			tmp += tmp1;
			str[i >> 1] = tmp;
	    }
	    
	}
	str[i >> 1] = 0;	  
	return (i >> 1);
}

/**
  * @brief 将字符型数字转成整型
 */
u32 LORA_Strtoul(const char* pStr,int base)
{
  return strtoul(pStr, 0, base);
}

/**
  * @brief 计算LoRa数据的长度
 */
u32 LORA_Datalen(char* pStr)
{
	u8 DataLen = 0;
	if(*pStr == 0x5A)/* 判断数据开头 */
	{
		while(*pStr++ != 0xCA)
		{
			DataLen ++;
		}
		return (DataLen + 1);
	}
	else
	{
		return DataLen;
	}
}




#endif