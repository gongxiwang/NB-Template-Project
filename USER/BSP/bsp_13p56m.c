/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		RF13P56M_Driver.c
  * @author		qyz
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		RF13P56M_Driver����������
  */ 


/**
  * @����ͷ�ļ�
  */
#include "bsp.h"
#include "bsp_13p56m.h"
#include "stdio.h"
#include "string.h"

/**
  * @��������
  */


static u8 RF13P56M_rev_buff[RF13P56M_BUFFSIZE];
static u16 RF13P56M_rev_buff_cnt = 0;
static u16 RF13P56M_rev_timeout;
static u8 RF13P56M_rev_finish = 0;


static int PN532_WakeUp(void);
static void CleanBuffer(u16 num);
static void ClearFlag(void);
static void SendData(u8 *data,u8 len);

/**
  * @brief		PN532��ʼ������
  * @details	
  * @param 	baud	���ڲ�����
  * @retval 	NULL	��
  */
void PN532_init(void)
{
    RF13P56M_Tim2_Init();
    PN532_WakeUp();
}

/**
  * @brief		tim2��ʼ������
  * @details	��ʱ��2��ʼ��������10ms�ж�
  * @param 	NULL	��
  * @retval 	NULL	��
  */
static void RF13P56M_Tim2_Init(void)
{															
   	TIM_TimeBaseInitTypeDef  time;
    NVIC_InitTypeDef nvic; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM_DeInit(TIM2);

    time.TIM_Period = 10000;		 				/* 10ms �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    											    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    time.TIM_Prescaler= (72 - 1);				    /* ʱ��Ԥ��Ƶ�� 72M/72 */
    time.TIM_ClockDivision=TIM_CKD_DIV1; 			/* ������Ƶ */
    time.TIM_CounterMode=TIM_CounterMode_Up; 		/* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM2, &time);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);			/* �������жϱ�־ */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);		
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);												
    nvic.NVIC_IRQChannel = TIM2_IRQChannel;	  
    nvic.NVIC_IRQChannelPreemptionPriority =1;
    nvic.NVIC_IRQChannelSubPriority = 2;	
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	BSP_IntVectSet(BSP_INT_ID_TIM2, BSP_IntHandlerTIM2); 				//���ô���1���ж�����������Ҫ�е�
	BSP_IntEn(BSP_INT_ID_TIM2);
}


/**
  * @brief		tim2�жϺ���
  * @details	
  * @param 	NULL	��
  * @retval 	NULL	��
  */
void BSP_IntHandlerTIM2(void)
{
	OS_CPU_SR  cpu_sr; 
	OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	if( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		if(RF13P56M_rev_buff_cnt != 0)
			RF13P56M_rev_timeout++;
		if(RF13P56M_rev_timeout == 25)  /*���ճ�ʱʱ��200ms*/
		{
			TIM_Cmd(TIM2, DISABLE);	   /*����200msû�н��յ����ݣ���Ϊ�������*/
		 	RF13P56M_rev_finish = 1;
		}
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); 	  
	}
    OSIntExit();
}	

/**
  * @brief		PN532_WakeUp
  * @details	����PN532ģ�飬�����������⣺55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
  * @param 	NULL	��
  * @retval 	����0Ϊ�ɹ�������1Ϊʧ��
  */ 
static int PN532_WakeUp(void)
{
    u8 data[24];
    u8 i,time = 5;
    u8 CheckCode=0; /*����У���� */
    u8 temp=0;
    
    while(time--)
    { 
        data[0]=0x55;
        data[1]=0x55;
        data[2]=0x00;
        data[3]=0x00;
        data[4]=0x00;
        data[5]=0x00;
        data[6]=0x00;
        data[7]=0x00;
        data[8]=0x00;
        data[9]=0x00;
        data[10]=0x00;
        data[11]=0x00;
        data[12]=0x00;
        data[13]=0x00;
        data[14]=0x00;
        data[15]=0x00;       
        data[16]=0xFF;
        data[17]=0x03; /*�� ���� */
        data[18]=0xFD; /*�� ���� У��  0x100-data[3] */
        data[19]=0xD4; /*�����ʶ��*/
        data[20]=0x14; /*�����ʶ�� */
        data[21]=0x01;
        data[22]=0x17; /*���� У��   0x100- */
        data[23]=0x00;
        
        SendData(data, 24);/*���� length���ȵ�����data*/

        while(!RF13P56M_rev_finish)
            delay_ms(10);
        
        ClearFlag();      

        for(i = 11;i < 13; i ++)
        {
            temp += RF13P56M_rev_buff[i];
        }
        CheckCode = 0x100 - temp;
        if(CheckCode == RF13P56M_rev_buff[13])
        {
            CheckCode = 0x100 - temp;
            CleanBuffer(40);                /* ��� ���ڽ��ջ�����ǰ30 ���ֽ����� */
            return 0;
        }
		delay_ms(50);
    }
    return -1;
}


/**
  * @brief		PN532_InListPassiveTarget
  * @details	Ѱ��
  * @param 	uid �洢����
  * @retval 	����0Ϊ�ɹ�������1Ϊʧ��
  */
int PN532_InListPassiveTarget(u8 *uid)
{
    u8 data[11];
    u8 i;
    u8 CheckCode=0; /*����У����	*/
    u8 temp=0;
	u8 chekdata[4] = {0};
    u16 wait_flag = 0;
    while(1)
    {   
        data[0]=0x00;
        data[1]=0x00;
        data[2]=0xFF;
        data[3]=0x04; /*�� ����	*/
        data[4]=0xFC; /*�� ���� У��  0x100-data[3]*/
        data[5]=0xD4; /*�����ʶ�� */
        data[6]=0x4A; /*�����ʶ��	*/
        data[7]=0x01;
        data[8]=0x00;
        data[9]=0xE1; /*���� У��   0x100-*/
        data[10]=0x00;
        SendData(data,11);
        while(!RF13P56M_rev_finish)
            delay_ms(10);
        
        ClearFlag();       		  
        temp=0;
     
        for(i=11; i < 23; i ++)
        {
            temp += RF13P56M_rev_buff[i];
        }
        CheckCode = 0x100 - temp;
        if(CheckCode == RF13P56M_rev_buff[23])
        {
            memcpy(uid, &RF13P56M_rev_buff[19], 4);
            if(memcmp(uid, chekdata, 4) != 0)/*����Ƿ�Ѱ����*/
            {  
                CleanBuffer(40);			 /*��� ���ڽ��ջ�����ǰ40 ���ֽ����� */
               return 0;
            }
            else
            {
                wait_flag ++;
                delay_ms(50);
                if(wait_flag == 50)
                {
                    wait_flag = 0;                    
                    break;
                }
            }
         }            
		 delay_ms(50);
    }
    return -1;
}


/**
  * @brief		PN532_PsdVerifyKeyA
  * @details	������Ȩ����֤KeyA 
  *             00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF UID1 UID2 UID3 UID4 2A 00
  *             00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF 94 8A 3B 0B 2A 00
  * @param 	block	���
  * @param 	uid	����
  * @retval 	����0Ϊ�ɹ�������1Ϊʧ��
  */
int PN532_PsdVerifyKeyA(u8 block,u8 *uid)
{
    u8 data[22];
    u8 temp=0; 
    u8 i;
    u8 CheckCode=0; /*����У����*/
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x0F; /*�� ���� */
    data[4]=0xF1; /*�� ���� У��  0x100-data[3] */
    
    data[5]=0xD4; /*�����ʶ�� */
    data[6]=0x40; /*�����ʶ�� */
    
    data[7]=0x01;
    data[8]=0x60;
    data[9]=block; 
    
    data[10]=0xFF;                    /*KEY A ���� FF FF FF FF FF FF*/
    data[11]=0xFF;
    data[12]=0xFF;
    data[13]=0xFF;
    data[14]=0xFF;
    data[15]=0xFF;
    memcpy(&data[16],uid,4);
    
    for(i=5;i<20;i++)
    {
        temp+=data[i];
    }

    data[20]=0x100-temp;                /*���� У��   0x100- */
    
    data[21]=0x00;
    
    SendData(data,22);                  /*��USART2������ length���ȵ�����data */
    while(!RF13P56M_rev_finish)
        delay_ms(10);
    
    ClearFlag();       
	  
    temp=0;
    for(i = 11; i < 14; i ++)
    {
        temp += RF13P56M_rev_buff[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode == RF13P56M_rev_buff[14])
    {
       if(RF13P56M_rev_buff[13]==0x00)
       {
         CleanBuffer(40);              /*��� ���ڽ��ջ�����ǰ40 ���ֽ�����*/
		 return 0; /*��֤��Կ�ɹ�*/
       }
	   
    } 
	return -1;	   /*��֤��Կʧ��*/
}


/**
  * @brief		PN532_read
  * @details	block ��ȡ����00 00 FF 05 FB D4 40 01 30 02 B9 00
  * @param 	block	���
  * @param 	block_data	����
  * @retval 	����0Ϊ�ɹ�������1Ϊʧ��
  */
int PN532_read(u8 block,u8 *block_data)
{
    u8 data[12];
    u8 temp=0; 
    u8 i;
    u8 CheckCode=0; /*����У����*/

    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x05; /*�� ���� */
    data[4]=0xFB; /*�� ���� У��  0x100-data[3]*/
    
    data[5]=0xD4; /*�����ʶ�� */
    data[6]=0x40; /*�����ʶ��  */
    
    data[7]=0x01;
    data[8]=0x30;
    data[9]=block; /*��16�ֽ�����*/ 
    temp=0;
    for(i=5;i<10;i++)
    {
        temp+=data[i];
    }    
    data[10]=0x100-temp; /*����У��	*/
    data[11]=0x00;

    SendData(data,12);
    while(! RF13P56M_rev_finish)
        delay_ms(10);
    
    ClearFlag();      
	  
    temp=0;
    for(i = 11; i < 30; i ++)
    {
        temp += RF13P56M_rev_buff[i];
    }
    CheckCode = 0x100 - temp;
    if(CheckCode == RF13P56M_rev_buff[30])
    {
        memcpy(block_data, &RF13P56M_rev_buff[14],16);
        CleanBuffer(40);/*��� ���ڽ��ջ�����ǰ40 ���ֽ�����*/
        return 0;     /*�������ݳɹ�*/
    }
	else
        CleanBuffer(40);
	return -1;
}


/**
  * @brief		PN532_write
  * @details	Ĭ���� 02��д16���ֽ�
  * @param 	block	00 00 FF 15 EB D4 40 01 A0 02 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F D1 00
  * @param 	block_data	����
  * @retval 	����0Ϊ�ɹ�������1Ϊʧ��
  */
int PN532_write(u8 block,u8 *block_data)
{
    u8 data[28];
    u8 temp=0;
    u8 i;
    u8 CheckCode=0;
    
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x15; /*�� ���� */
    data[4]=0xEB; /*�� ���� У��  0x100-data[3]*/
    
    data[5]=0xD4; /*�����ʶ�� */
    data[6]=0x40; /*�����ʶ��*/
    
    data[7]=0x01; /*��д����6�ֽ� ����1�����ֲ�*/
    data[8]=0xA0; /*д*/
    data[9]=block; /*д16�ֽ�����*/ 
    
    memcpy(&data[10],block_data,16);

    for(i=5;i<26;i++)
    {
        temp+=data[i];
    }
    data[26]=0x100-temp; /*���� У����*/
    data[27]=0x00;

    SendData(data, 28);
    while(!RF13P56M_rev_finish)
        delay_ms(10);
    
    ClearFlag();   
	     
    /*����00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 00 EA 00 */
    temp=0;
    for(i = 11; i < 14; i ++)
    {
        temp += RF13P56M_rev_buff[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==0xEA)
    {
        CleanBuffer(40);/*��� ���ڽ��ջ�����ǰ40 ���ֽ�����	*/
        return 0;/*д�����ݳɹ�*/  
    }
	else
		CleanBuffer(40);	
    return -1;
}

/**
  * @brief		����2���ݷ��ͺ���
  * @details	
  * @param Data	���ݻ���
  * @param len	���ݳ���
  * @retval 	��
  */
static void SendData(u8 *data,u8 len)
{
	u16 i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART1, data[i]);
		/* Loop until the end of transmission */
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}


/**
  * @brief		CleanBuffer
  * @details	����ǰ���ٸ��ֽڵ�����
  * @param 	num �ֽ���
  * @retval 	NULL ��
  */
static void CleanBuffer(u16 num)
{
	memset(RF13P56M_rev_buff,0,num);
}


/**
  * @brief		ClearFlag
  * @details	�����־λ
  * @param 	NULL ��
  * @retval 	NULL ��
  */
static void ClearFlag(void)
{
	RF13P56M_rev_finish = 0;
	RF13P56M_rev_buff_cnt = 0;
	RF13P56M_rev_timeout = 0;        
}

/**
  * @brief		delay_ms
  * @details	ϵͳ��ʱ
  * @param 	uint32_t ms
  * @retval 	NULL ��
  */
void delay_ms(u32 ms)
{
	OSTimeDly(ms);
}

/**
  * @brief   13.56M���ڽ�������
  * @param   packet ���ݰ�
  * @retval  Ѱ��ǩ���
 */
void RF3P56MRecvData(void)
{
	TIM_Cmd(TIM2, ENABLE);
	RF13P56M_rev_timeout = 0;
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	RF13P56M_rev_buff[RF13P56M_rev_buff_cnt % RF13P56M_BUFFSIZE] = USART_ReceiveData(USART1);
	RF13P56M_rev_buff_cnt++; 
}