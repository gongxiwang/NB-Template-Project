/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		RF13P56M_Driver.c
  * @author		qyz
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		RF13P56M_Driver驱动函数。
  */ 


/**
  * @包含头文件
  */
#include "bsp.h"
#include "bsp_13p56m.h"
#include "stdio.h"
#include "string.h"

/**
  * @变量定义
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
  * @brief		PN532初始化函数
  * @details	
  * @param 	baud	串口波特率
  * @retval 	NULL	无
  */
void PN532_init(void)
{
    RF13P56M_Tim2_Init();
    PN532_WakeUp();
}

/**
  * @brief		tim2初始化函数
  * @details	定时器2初始化，配置10ms中断
  * @param 	NULL	无
  * @retval 	NULL	无
  */
static void RF13P56M_Tim2_Init(void)
{															
   	TIM_TimeBaseInitTypeDef  time;
    NVIC_InitTypeDef nvic; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM_DeInit(TIM2);

    time.TIM_Period = 10000;		 				/* 10ms 自动重装载寄存器周期的值(计数值) */
    											    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    time.TIM_Prescaler= (72 - 1);				    /* 时钟预分频数 72M/72 */
    time.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
    time.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
    TIM_TimeBaseInit(TIM2, &time);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);			/* 清除溢出中断标志 */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);		
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);												
    nvic.NVIC_IRQChannel = TIM2_IRQChannel;	  
    nvic.NVIC_IRQChannelPreemptionPriority =1;
    nvic.NVIC_IRQChannelSubPriority = 2;	
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	BSP_IntVectSet(BSP_INT_ID_TIM2, BSP_IntHandlerTIM2); 				//设置串口1的中断向量，必须要有的
	BSP_IntEn(BSP_INT_ID_TIM2);
}


/**
  * @brief		tim2中断函数
  * @details	
  * @param 	NULL	无
  * @retval 	NULL	无
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
		if(RF13P56M_rev_timeout == 25)  /*接收超时时间200ms*/
		{
			TIM_Cmd(TIM2, DISABLE);	   /*超过200ms没有接收到数据，认为接收完成*/
		 	RF13P56M_rev_finish = 1;
		}
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); 	  
	}
    OSIntExit();
}	

/**
  * @brief		PN532_WakeUp
  * @details	唤醒PN532模块，此条命令特殊：55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
  * @param 	NULL	无
  * @retval 	返回0为成功，返回1为失败
  */ 
static int PN532_WakeUp(void)
{
    u8 data[24];
    u8 i,time = 5;
    u8 CheckCode=0; /*数据校验码 */
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
        data[17]=0x03; /*包 长度 */
        data[18]=0xFD; /*包 长度 校验  0x100-data[3] */
        data[19]=0xD4; /*命令标识码*/
        data[20]=0x14; /*命令标识码 */
        data[21]=0x01;
        data[22]=0x17; /*数据 校验   0x100- */
        data[23]=0x00;
        
        SendData(data, 24);/*发送 length长度的数据data*/

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
            CleanBuffer(40);                /* 清除 串口接收缓冲区前30 个字节数据 */
            return 0;
        }
		delay_ms(50);
    }
    return -1;
}


/**
  * @brief		PN532_InListPassiveTarget
  * @details	寻卡
  * @param 	uid 存储卡号
  * @retval 	返回0为成功，返回1为失败
  */
int PN532_InListPassiveTarget(u8 *uid)
{
    u8 data[11];
    u8 i;
    u8 CheckCode=0; /*数据校验码	*/
    u8 temp=0;
	u8 chekdata[4] = {0};
    u16 wait_flag = 0;
    while(1)
    {   
        data[0]=0x00;
        data[1]=0x00;
        data[2]=0xFF;
        data[3]=0x04; /*包 长度	*/
        data[4]=0xFC; /*包 长度 校验  0x100-data[3]*/
        data[5]=0xD4; /*命令标识码 */
        data[6]=0x4A; /*命令标识码	*/
        data[7]=0x01;
        data[8]=0x00;
        data[9]=0xE1; /*数据 校验   0x100-*/
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
            if(memcmp(uid, chekdata, 4) != 0)/*检测是否寻到卡*/
            {  
                CleanBuffer(40);			 /*清除 串口接收缓冲区前40 个字节数据 */
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
  * @details	密码授权，验证KeyA 
  *             00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF UID1 UID2 UID3 UID4 2A 00
  *             00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF 94 8A 3B 0B 2A 00
  * @param 	block	块号
  * @param 	uid	卡号
  * @retval 	返回0为成功，返回1为失败
  */
int PN532_PsdVerifyKeyA(u8 block,u8 *uid)
{
    u8 data[22];
    u8 temp=0; 
    u8 i;
    u8 CheckCode=0; /*数据校验码*/
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x0F; /*包 长度 */
    data[4]=0xF1; /*包 长度 校验  0x100-data[3] */
    
    data[5]=0xD4; /*命令标识码 */
    data[6]=0x40; /*命令标识码 */
    
    data[7]=0x01;
    data[8]=0x60;
    data[9]=block; 
    
    data[10]=0xFF;                    /*KEY A 密码 FF FF FF FF FF FF*/
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

    data[20]=0x100-temp;                /*数据 校验   0x100- */
    
    data[21]=0x00;
    
    SendData(data,22);                  /*往USART2，发送 length长度的数据data */
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
         CleanBuffer(40);              /*清除 串口接收缓冲区前40 个字节数据*/
		 return 0; /*验证密钥成功*/
       }
	   
    } 
	return -1;	   /*验证密钥失败*/
}


/**
  * @brief		PN532_read
  * @details	block 读取扇区00 00 FF 05 FB D4 40 01 30 02 B9 00
  * @param 	block	块号
  * @param 	block_data	数据
  * @retval 	返回0为成功，返回1为失败
  */
int PN532_read(u8 block,u8 *block_data)
{
    u8 data[12];
    u8 temp=0; 
    u8 i;
    u8 CheckCode=0; /*数据校验码*/

    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x05; /*包 长度 */
    data[4]=0xFB; /*包 长度 校验  0x100-data[3]*/
    
    data[5]=0xD4; /*命令标识码 */
    data[6]=0x40; /*命令标识码  */
    
    data[7]=0x01;
    data[8]=0x30;
    data[9]=block; /*读16字节数据*/ 
    temp=0;
    for(i=5;i<10;i++)
    {
        temp+=data[i];
    }    
    data[10]=0x100-temp; /*数据校验	*/
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
        CleanBuffer(40);/*清除 串口接收缓冲区前40 个字节数据*/
        return 0;     /*读卡数据成功*/
    }
	else
        CleanBuffer(40);
	return -1;
}


/**
  * @brief		PN532_write
  * @details	默认往 02区写16个字节
  * @param 	block	00 00 FF 15 EB D4 40 01 A0 02 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F D1 00
  * @param 	block_data	数据
  * @retval 	返回0为成功，返回1为失败
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
    
    data[3]=0x15; /*包 长度 */
    data[4]=0xEB; /*包 长度 校验  0x100-data[3]*/
    
    data[5]=0xD4; /*命令标识码 */
    data[6]=0x40; /*命令标识码*/
    
    data[7]=0x01; /*读写大于6字节 就置1，看手册*/
    data[8]=0xA0; /*写*/
    data[9]=block; /*写16字节数据*/ 
    
    memcpy(&data[10],block_data,16);

    for(i=5;i<26;i++)
    {
        temp+=data[i];
    }
    data[26]=0x100-temp; /*数据 校验码*/
    data[27]=0x00;

    SendData(data, 28);
    while(!RF13P56M_rev_finish)
        delay_ms(10);
    
    ClearFlag();   
	     
    /*返回00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 00 EA 00 */
    temp=0;
    for(i = 11; i < 14; i ++)
    {
        temp += RF13P56M_rev_buff[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==0xEA)
    {
        CleanBuffer(40);/*清除 串口接收缓冲区前40 个字节数据	*/
        return 0;/*写卡数据成功*/  
    }
	else
		CleanBuffer(40);	
    return -1;
}

/**
  * @brief		串口2数据发送函数
  * @details	
  * @param Data	数据缓存
  * @param len	数据长度
  * @retval 	无
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
  * @details	清零前多少个字节的数据
  * @param 	num 字节数
  * @retval 	NULL 无
  */
static void CleanBuffer(u16 num)
{
	memset(RF13P56M_rev_buff,0,num);
}


/**
  * @brief		ClearFlag
  * @details	清除标志位
  * @param 	NULL 无
  * @retval 	NULL 无
  */
static void ClearFlag(void)
{
	RF13P56M_rev_finish = 0;
	RF13P56M_rev_buff_cnt = 0;
	RF13P56M_rev_timeout = 0;        
}

/**
  * @brief		delay_ms
  * @details	系统延时
  * @param 	uint32_t ms
  * @retval 	NULL 无
  */
void delay_ms(u32 ms)
{
	OSTimeDly(ms);
}

/**
  * @brief   13.56M串口接收数据
  * @param   packet 数据包
  * @retval  寻标签结果
 */
void RF3P56MRecvData(void)
{
	TIM_Cmd(TIM2, ENABLE);
	RF13P56M_rev_timeout = 0;
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	RF13P56M_rev_buff[RF13P56M_rev_buff_cnt % RF13P56M_BUFFSIZE] = USART_ReceiveData(USART1);
	RF13P56M_rev_buff_cnt++; 
}