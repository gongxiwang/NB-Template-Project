/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		app_uart.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app�ӿں����ļ�
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
  * @brief �ڴ��ָ��
 */
static OS_MEM *pmem_debug;
static debug_memory_struct USART_MEMORY[debug_memory_blk_number];


/**
  * @brief ��ӡ���ݽӿڳ�ʼ��
  * @details �ɱ����
  * @param  pstring �ַ�����
  * @retval ��
 */
void InitDebugPrintf(void)
{
	u8 err;	
	/* �������ڴ�ӡ������ڴ� */
	pmem_debug = OSMemCreate(USART_MEMORY, 
							debug_memory_blk_number, 
							debug_memory_blk_size,
							&err);
}


/**
  * @brief ��ӡ���ݽӿ�
  * @details �ɱ����
  * @param  pstring �ַ�����
  * @retval ��
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
  * @brief ���ʹ�������
  * @details �ɱ����
  * @param  pstring �ַ�����
  * @retval ��
 */
static void debug_send_data(unsigned char* data, unsigned char len)
{
    unsigned char i;
	/* ÿ�η���ǰ������ͱ�־λ���ܹ����͵�һ���ֽ��� */
	USART_ClearFlag(USART3, USART_FLAG_TC); 
    for(i = 0; i < len; i ++)
    {
        USART_SendData(USART3, data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

/**
  * @brief HEXת�ַ���
  * @details �ɱ����
  * @param  pstring �ַ�����
  * @retval ��
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
  * @brief ���ʹ���buffer����
  * @details �ɱ����
  * @param  pstring �ַ�����
  * @retval ��
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

