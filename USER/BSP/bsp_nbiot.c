/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		bsp_nbiot.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		NB��������
  */

#include "bsp.h"
#include "stdio.h"
#include "string.h"

/**
  * @brief		tim2��ʼ������
  * @details	��ʱ��2��ʼ��������10ms�ж�
  * @param 	NULL	��
  * @retval 	NULL	��
  */
void NBiot_Timer_Init(void)
{															
   	TIM_TimeBaseInitTypeDef  time;
    NVIC_InitTypeDef nvic; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	TIM_DeInit(TIM3);

    time.TIM_Period = 999;		 					/* 1s �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */    											    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    time.TIM_Prescaler= (72 - 1);				    /* ʱ��Ԥ��Ƶ�� 72M/72 */
    time.TIM_ClockDivision=TIM_CKD_DIV1; 			/* ������Ƶ */
    time.TIM_CounterMode=TIM_CounterMode_Up; 		/* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM3, &time);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);			/* �������жϱ�־ */
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);		
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);												
    nvic.NVIC_IRQChannel = TIM3_IRQChannel;	  
    nvic.NVIC_IRQChannelPreemptionPriority = 11;
    nvic.NVIC_IRQChannelSubPriority = 2;	
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	BSP_IntVectSet(BSP_INT_ID_TIM3, BSP_IntHandlerTIM3); 				//���ô���1���ж�����������Ҫ�е�
	BSP_IntEn(BSP_INT_ID_TIM3);
}


/**
  * @brief		tim2�жϺ���
  * @details	
  * @param 	NULL	��
  * @retval 	NULL	��
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
  * @brief ��ʮ�������ַ���ת������
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
// brief : ���ַ�������ת������
//
// param : pStr -> Ҫת�����ݵ�ַ
//         base -> ת�������Զ��ٽ��ƽ���ת����2-36��
//
// return : ��������
u32 NB_Strtoul(const char* pStr,int base)
{
  return strtoul(pStr, 0, base);
}

/***********************************************************************
* ���ܣ���һ��ʮ�������ֽڴ�ת����ASCII���ʾ��ʮ�������ַ���
* ���������pHex----ʮ���������ֽڴ��׵�ַ
*        			pAscii---ת����ASCII���ʾ��ʮ�������ַ����׵�ַ
*        			nLen---Ҫת����ʮ�����������ȣ��ֽ�����
* ע��ת����Ľ��ȫ���Ǵ�дASCII���ʾ��ʮ��������
*�˲��ְٶȵ�
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

