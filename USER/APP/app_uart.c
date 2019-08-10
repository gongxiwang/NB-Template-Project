/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		app_uart.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app接口函数文件
  */
#include "app_uart.h"
#include "bsp_uart.h"
#include "app_nbiot.h"
#include "stdio.h"
#include <stdarg.h>
#include "bsp.h"
#include "bsp_dma.h"
#include "app_device.h"
#include "App_RF900M.h"
#include "App_RF13P56M.h"
#include "app_watmet.h"

/**
  * @brief 内存块指针
 */
static OS_MEM *pmem_debug;
static debug_memory_struct USART_MEMORY[debug_memory_blk_number];


/**
  * @brief 打印数据接口初始化
  * @details 可变参数
  * @param  pstring 字符参数
  * @retval 无
 */
void InitDebugPrintf(void)
{
	u8 err;	
	/* 创建用于打印输出的内存 */
	pmem_debug = OSMemCreate(USART_MEMORY, 
							debug_memory_blk_number, 
							debug_memory_blk_size,
							&err);
}


/**
  * @brief 打印数据接口
  * @details 可变参数
  * @param  pstring 字符参数
  * @retval 无
 */
void debug_printf(const char *pstring, ...)
{
	debug_memory_struct *p;
    unsigned char err;
    va_list args;
    
    p = OSMemGet(pmem_debug, &err);
    if(!p) 
        return;
    va_start(args, pstring);
    p->size = vsnprintf(p->buffer, debug_memory_blk_size, pstring, args);
    va_end(args);
    if(!p->size)
    {
        OSMemPut(pmem_debug, p);
        return;
    }
    debug_send_data((unsigned char*)p->buffer, p->size);
    OSMemPut(pmem_debug, p);
    return;
}

/**
  * @brief 发送串口数据
  * @details 可变参数
  * @param  pstring 字符参数
  * @retval 无
 */
static void debug_send_data(unsigned char* data, unsigned char len)
{
    unsigned char i;
	/* 每次发送前清除发送标志位就能够发送第一个字节了 */
	USART_ClearFlag(USART3, USART_FLAG_TC); 
    for(i = 0; i < len; i ++)
    {
        USART_SendData(USART3, data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

/**
  * @brief HEX转字符串
  * @details 可变参数
  * @param  pstring 字符参数
  * @retval 无
 */
static u8 hex_to_string(u8 hex, u8 *pbuffer)
{
	u8 temp;
	temp = (hex & 0xf0) >> 4;
	if(temp <= 9)
		*pbuffer++ = temp + '0';
	else
		*pbuffer++ = temp - 10 + 'A';
	temp = (hex & 0x0f) >> 0;
	if(temp <= 9)
		*pbuffer++ = temp + '0';
	else
		*pbuffer++ = temp  - 10 + 'A';
	*pbuffer++ = ' ';
	return 3;
}

/**
  * @brief 发送串口buffer数据
  * @details 可变参数
  * @param  pstring 字符参数
  * @retval 无
 */
void debug_print_buffer(const u8 *buffer, u8 size)
{
    debug_memory_struct *p;
    u8 err;
    int i;

    if(!size)
        return;
    
    p = OSMemGet(pmem_debug, &err);
    if(!p) 
        return;
    
    p->size = 0;
    for(i = 0; i < size; ++i)
    {
        if(buffer_size - p->size < 6)
        {
            /* not enough memory to fill the buffer, fix the EOF */
            p->buffer[p->size] = '\0';
            p->buffer[p->size - 1] = '\n';
            p->buffer[p->size - 2] = '\r';
            p->buffer[p->size - 3] = '.';
            p->buffer[p->size - 4] = '.';
            p->buffer[p->size - 5] = '.';
            break;  
        }
        p->size += hex_to_string(buffer[i], (u8 *)&p->buffer[p->size]);
    }
    /* all buffer filled to memory */
    if(i == size)
    {
        p->buffer[p->size++] = '\r';
        p->buffer[p->size++] = '\n';
        p->buffer[p->size++] = '\0';
    }
    
    if(!p->size)
    {
        OSMemPut(pmem_debug, p);
        return;
    }
    printf("%s", p->buffer);
    debug_send_data((u8*)p->buffer, p->size);
    OSMemPut(pmem_debug, p);
}

