#ifndef __APP_UART__H
#define __APP_UART__H
#include "app_cfg.h"
#include "bsp.h"
#include "app_device.h"
#include "app_RF900m.h"

#define DATA_ADDRESS_BALANCE 0x00
#define DATA_ADDRESS_POINT   0x04

/**
  * @brief 串口发送缓冲区的长度
 */
#define buffer_size 1024
/**
  * @brief 串口内存块个数
 */
#define debug_memory_blk_number 1
/**
  * @brief 串口内存块大小
 */
#define debug_memory_blk_size sizeof(debug_memory_struct)

/**
  * @brief 串口缓冲区的结构体
 */
typedef struct 
{
    /** @brief 要发送的缓冲区 */
    char buffer[buffer_size];
    /** @brief 要发送的数据长度 */
    int size;
}debug_memory_struct;

/**
  * @brief 串口设备结构体---900M扫码枪等
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
