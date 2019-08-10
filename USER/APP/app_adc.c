/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_adc.c
  * @author  walle
  * @version  V1.0.0
  * @date  28/05/2019
  * @brief  ADC������
  */ 

#include "app_adc.h"
#include "app_uart.h"
#include "app_beep.h"
#include "app_cfg.h"
#include "bsp.h"
#include "bsp_adc.h"

/**
  * @brief ȫ�־�̬ADC�洢ת��ֵ
 */
static vu16 ADC1ConvertedValue;


/**
  * @brief ADC_TASK������
 */
void App_ADC_TASK(void)
{
	u16 curADCSampleValue = 0;
	debug_printf( "[%s] enter.\r\n", __func__ );	/* debug */
	CTRL_ADC_CUR;/* �򿪵������ */
	while(1)
	{

#if 0
		curADCSampleValue = APP_GetCSF11(ADC_SAMPLE_CURCHANNEL);
		
		debug_printf("curADCSampleValue = %d\r\n", curADCSampleValue);
#endif
	OSTimeDlyHMSM(0, 0, 0, 1);					/* ϵͳ�����л� */
	}
}


/*****************************************************************
*    Function Name           :   Megre
*
*    Parameters              :   u16 *P    Array to be sorted
*                                u16 *Q    Sort Results
*                                low       Left out of the starting unit
*                                mid       Right out of the starting unit
*                                high      the unit behind the right sub-column
*    Description             :   �ú���Ϊ�鲢������ڲ��㷨,�����㷨
*                                ������ο� Ƕ��ʽϵͳ�������еĳ����㷨
*                                �ܺ��� ��  �������պ����ѧ������
*
*    Returns                 :
*
*    Global Variable         :
*    File Static Variable    :
*    Function Static Variable:
*----------------------------------------------------------------
*    Revision History
*    No.    Date        Revised by        Item        Description
*
*****************************************************************/

static void Megre(u16 *P,u16 *Q,u16 low,u16 mid, u16 high)
{
    u16 i,j,k;

    i = low; 
    j = mid;
    k = low;

    while((i<mid)&&(j<high)) 
    {
        if(P[i]<=P[j])
        {
            Q[k++] = P[i++];
        }
        else 
        {
            Q[k++] = P[j++];
        }
    }

    while(i<mid)
    {
        Q[k++] = P[i++];
    }

    while(j<high) 
    {
        Q[k++] = P[j++];
    }
}

/*****************************************************************
*    Function Name           :   MergePass
*
*    Parameters              :
*
*    Description             :   �ú���Ϊ�鲢������в��㷨,�����㷨
*                                ������ο� Ƕ��ʽϵͳ�������еĳ����㷨
*                                �ܺ��� ��  �������պ����ѧ������
*    Returns                 :
*
*    Global Variable         :
*    File Static Variable    :
*    Function Static Variable:
*----------------------------------------------------------------
*    Revision History
*    No.    Date        Revised by        Item        Description
*
*****************************************************************/
static void MegrePass(u16 *M, u16 *N, u16 length,u16 sortLen)
{
    u16 i=0,j;

    while(i+2*length-1 < sortLen) 
    {
        Megre(M,N,i,i+length,i+2*length);
        i+=2*length;
    }

    if(i+length<sortLen)
    {
        Megre(M,N,i,i+length,sortLen);
    }
    else
    {
        for(j=i;j<sortLen;j++)
        {
            N[j] = M[j];
        }
    }
}

/*****************************************************************
*    Function Name           :   MegreSort
*
*    Parameters              :   u16 *P        sorted array  
*                                u16 *Q        Auxiliary array
*                                u16 sortlen   need sort number
*    Description             :   �ú���Ϊ�鲢������в��㷨,�����㷨
*                                ������ο� Ƕ��ʽϵͳ�������еĳ����㷨
*                                �ܺ��� ��  �������պ����ѧ������
*    Returns                 :
*
*    Global Variable         :
*    File Static Variable    :
*    Function Static Variable:
*----------------------------------------------------------------
*    Revision History
*    No.    Date        Revised by        Item        Description
*
*****************************************************************/
static void MegreSort(u16 *P,u16 *Q,u16 sortLen)
{
    u16 length =1;

    while(length < sortLen) 
    {
        MegrePass(P,Q,length,sortLen);
        length *=2;
        MegrePass(Q,P,length,sortLen);
        length *=2;
    }
}

/*****************************************************************
*    Function Name           :   ADC1_Sample
*
*    Parameters              :
*
*    Description             :
*
*    Returns                 :
*
*    Global Variable         :
*    File Static Variable    :
*    Function Static Variable:
*----------------------------------------------------------------
*    Revision History
*    No.    Date        Revised by        Item        Description
*
*****************************************************************/
u16 ADC1_Sample(u8 channel)
{
	u8 i;
	/*  �������ɼ�����  */
	u16 ADCArry[30];
	/*  �����������ʱ����  */
	u16 tmpArry[30];

	u16 ADCValue;
	/*  */
	u32 sum=0;

    for(i = 0; i < 30; i ++)
    {
        /* ADC1 regular channels configuration */ 
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
        /* Start ADC1 Software Conversion */ 
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        /*  �ȴ�ת��  */
        while(!(ADC1->SR&0x00000002));
        //while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
        ADC1ConvertedValue = ADC1->DR & 0x0000FFFF;
		ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
        /* DISABLE ADC1  */ 
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);
        /*-----------------------------------------------------------------------*/
        ADCArry[i] = ADC1ConvertedValue ;
    }
	//debug_printf("ADC1ConvertedValue = %f\r\n", (float)ADCArry[5]);
    /* ���� */
    MegreSort(ADCArry, tmpArry, 30);

	/*  ʹ��ǰ����  */
    sum = 0;
	/* ȡ�м�ֵ��� */
    for(i = 5; i < 25; i ++)
    {
        sum += ADCArry[i];
    }
    ADCValue = (u16)(sum/20);
    return ADCValue;
}


/*****************************************************************
*    Function Name           :  APP_GetCSF11
*
*    Parameters              :  
*                                
*    Description             :	��ȡ����
*
*    Returns                 :
*
*    Global Variable         :
*    File Static Variable    :
*    Function Static Variable:
*----------------------------------------------------------------
*    Revision History
*    No.    Date        Revised by        Item        Description
*
*****************************************************************/
u16 APP_GetCSF11(u8 channel)
{
	u16 adc_w,adc_t;
    float result_w,result_t;
	u16 value;

	switch(channel)
	{
		case ADC_SAMPLE_CURCHANNEL:
			adc_w = ADC1_Sample(ADC_Channel_10);
			adc_t = ADC1_Sample(ADC_Channel_11);
			debug_printf("adc_w = %d, adc_t = %d\r\n", adc_w, adc_t);
		break;
		case ADC_SAMPLE_VOLCHANNEL:
			adc_w = ADC1_Sample(ADC_Channel_12);
			adc_t = ADC1_Sample(ADC_Channel_13);
			debug_printf("adc_w = %d, adc_t = %d\r\n", adc_w, adc_t);
		break;
		case 3:
		break;
		default:
		break;
	}

	if(adc_w < VMIN_C) 
		adc_w = VMIN_C;
	if(adc_w > VMAX_C) 
		adc_w = VMAX_C;

	if(adc_t < VMIN_C) 
		adc_t = VMIN_C;
	if(adc_t > VMAX_C) 
		adc_t = VMAX_C;

	/* ����ˮ�ֺ��� */
	result_w = (float)(adc_w - VMIN_C)*(RMAX_C - RMIN_C)/(float)(VMAX_C - VMIN_C) + RMIN_C;
	debug_printf("result_w = %d\r\n", result_w);

	/* �����¶� */
	result_t = ((float)(adc_t - VMIN_C)*(RMAX_C - RMIN_C)/(float)(VMAX_C - VMIN_C) + RMIN_C)/2; 
	debug_printf("result_t = %d\r\n", result_t);

	/* ����ֵ--�����¶Ⱥ�ˮ�ֵ�ֵ */
	value = (((u16)result_w & 0xFF) << 8)|((u16)result_t & 0xFF);

	return value;
}








