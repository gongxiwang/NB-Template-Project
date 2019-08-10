#ifndef __APP_UART__H
#define __APP_UART__H
#include "app_cfg.h"
#include "bsp.h"
#include "app_device.h"
#include "app_RF900m.h"

#define DATA_ADDRESS_BALANCE 0x00
#define DATA_ADDRESS_POINT   0x04

/**
  * @brief ���ڷ��ͻ������ĳ���
 */
#define buffer_size 1024
/**
  * @brief �����ڴ�����
 */
#define debug_memory_blk_number 1
/**
  * @brief �����ڴ���С
 */
#define debug_memory_blk_size sizeof(debug_memory_struct)

/**
  * @brief ���ڻ������Ľṹ��
 */
typedef struct 
{
    /** @brief Ҫ���͵Ļ����� */
    char buffer[buffer_size];
    /** @brief Ҫ���͵����ݳ��� */
    int size;
}debug_memory_struct;

/**
  * @brief �����豸�ṹ��---900Mɨ��ǹ��
 */
typedef struct
{
	u8	DeviceNum;
	u8	DeviceAct;
}ST_DEV_UAT, *PST_DEV_UAT;

void  App_USART_TASK (void *p_data);
static void debug_send_data(unsigned char* data, unsigned char len);
void debug_printf(const char *pstring, ...);
void debug_print_buffer(const u8 *buffer, u8 size);
void InitDebugPrintf(void);


#endif
