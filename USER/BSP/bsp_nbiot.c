/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		bsp_nbiot.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		NB驱动函数
  */

#include "bsp.h"
#include "stdio.h"
#include "string.h"

/**
  * @brief		tim2初始化函数
  * @details	定时器2初始化，配置10ms中断
  * @param 	NULL	无
  * @retval 	NULL	无
  */
void NBiot_Timer_Init(void)
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
  * @brief		tim2中断函数
  * @details	
  * @param 	NULL	无
  * @retval 	NULL	无
  */
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


/**
  * @brief 将十六进制字符串转成数字
 */
u16 NB_HexStrToNum(char* str)
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

//******************************************************************************
// fn : NB_Strtoul
//
// brief : 将字符型数字转成整型
//
// param : pStr -> 要转换内容地址
//         base -> 转换内容以多少进制进行转换（2-36）
//
// return : 整形数据
u32 NB_Strtoul(const char* pStr,int base)
{
  return strtoul(pStr, 0, base);
}

/***********************************************************************
* 功能：将一个十六进制字节串转换成ASCII码表示的十六进制字符串
* 输入参数：pHex----十六进制数字节串首地址
*        			pAscii---转换后ASCII码表示的十六进制字符串首地址
*        			nLen---要转换的十六进制数长度（字节数）
* 注：转换后的结果全部是大写ASCII码表示的十六进制数
*此部分百度的
************************************************************************/
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen)
{
	unsigned char Nibble[2];
    unsigned int i,j;
    for (i = 0; i < nLen; i++){
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (j = 0; j < 2; j++){
            if (Nibble[j] < 10){ 			
                Nibble[j] += 0x30;
            }
            else{
                if (Nibble[j] < 16)
                    Nibble[j] = Nibble[j] - 10 + 'A';
            }
            *pAscii++ = Nibble[j];
        }   
    }  		
}

