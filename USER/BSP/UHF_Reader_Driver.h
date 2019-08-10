/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		led.h
  * @author		lwl
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		900M驱动头文件。
  */ 

/**
  * @Multi-Include-Prevent Section
  */
#ifndef __UHF_READER_DRIVER_H__
#define __UHF_READER_DRIVER_H__
/**
  * @Include File Section
  * @包含头文件
  */
#include "bsp.h"
#include "App_RF900M.h"


/**
  * @Macro Define Section
  * @宏定义
  */
#define UHF_BUF_NUM  1024                                               /*缓冲区数据个数*/  
#define UHF_SOF_DATA 0xA0                                              /*起始符号*/  
#define UHF_END_DATA 0x55                                               /*结束符号*/  
#define OFF_UHF_RX_DATA USART_ITConfig(USART2, USART_IT_RXNE, DISABLE)  /*关闭接收中断*/  
#define ON_UHF_RX_DATA  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE)   /*打开接收中断*/  
#define OFF_UHF_TX_DATA USART_ITConfig(USART2, USART_IT_TXE, DISABLE)   /*关闭发送中断*/  
#define ON_UHF_TX_DATA  USART_ITConfig(USART2, USART_IT_TXE, ENABLE)    /*打开发送中断*/ 




#define RLM_SET_ANTENNA			0x74	/* 设置工作天线 */
#define RLM_GET_STATUS			0x63  	/*读取天线连接检测器状态*/
#define RLM_GET_POWER			0x77  	/*查询读写器当前输出功率*/
#define RLM_SET_POWER			0x76	/*设置读写器射频输出功率*/
#define RLM_GET_FRE 			0x79	/*查询读写器工作频率范围*/
#define RLM_SET_FRE				0x78	/*设置读写器工作频率范围*/		  
#define RLM_GET_VERSION			0x07	/*读取RLM信息*/
#define RLM_READ_UID 			0x0a    /*读取RLM-UID信息*/

#define RLM_INVENTORY			0x81	/*单标签识别-循环模式*/
#define RLM_INVENTORY_ANTI		0x89	/*多标签识别-循环模式*/
#define RLM_STOP_GET			0x12	/*停止操作*/
#define RLM_READ_DATA			0x81	/*读取标签数据（指定UII）单步*/
#define RLM_WRITE_DATA			0x82	/*写入标签数据_单字长模式（指定UII）   单步  */
#define RLM_INVENTORY_SINGLE	0x80	/*单标签识别-单步模式*/
#define RLM_BLOCK_WRITE_DATA	0x82    /*写入标签数据_多字长模式（指定UII）*/


/*响应状态*/
#define UHF_FLAG_OK				0X00		  /*响应正常*/
#define UHF_FLAG_FAIL			0X01		  /*超时错误*/
#define UHF_FLAG_CMD			0X06		  /*命令比匹配*/
#define UHF_FLAG_STATUS 		0X07		  /*状态失败*/

/**
  * @brief  Struct Define Section
  * @结构体定义
  */
typedef struct
{
	u8  UHF_SOF;							/* 起始标志 */
	u8  UHF_LEN;							/* 数据长度 */
	u8  UHF_ADR;							/* 设备地址 */
	u8  UHF_CMD;							/* 命令码  	*/
	u8  UHF_PAYLOAD[UHF_BUF_NUM];			/* 数据内容 */
	u8  UHF_CC;								/* 校验 */
}_UHF_CARD_;
/**
  * @Global Varible Declare
  * @全局变量声明
  */

/**
  * @Prototype Declare Section
  * @内部函数原型声明
  */
void UHF_init(void);/*初始化*/
void UHFResive_Pasv(void);
void UHFSend_Pasv(_UHF_CARD_* Bdata);

u8 UhfWRdata_list(PST_UHF_Device pst_UHF_Dev)	;/*寻卡*/
u8 UhfWRdata_read(PST_UHF_Device pst_UHF_Dev, PST_READ_TAG pst_READ_TAG);/*读卡*/
u8 UhfWRdata_write(PST_UHF_Device pst_UHF_Dev, Device_packet_t *packet);/*写卡*/
u8 UhfSelect_TAG(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev);

EN_UHF_STATUS UHF_Tx_PACK(PST_UHF_Device pst_UHF_device, const u8 *pdata, u8 data_size);
EN_UHF_STATUS WaitUHFDataReceive(PST_UHF_Device pst_UHF_Dev);

#endif

