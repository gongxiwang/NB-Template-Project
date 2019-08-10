/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		App_RF900M.h
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app接口函数文件
  */
#ifndef _APP_RF900M_H
#define _APP_RF900M_H


#include "bsp.h"
#include "stdio.h"
#include "app_nbiot.h"
#include "app_device.h"

#define UHF_TX_BUFFER_MAX_SIZE			0xFF
#define UHF_RX_BUFFER_MAX_SIZE			0xFF
#define UHF_DATA_MAX_SIZE				0xFF
#define UHF_NAME_MAX_SIZE				0x0C
#define UHF_EPC_MAX_LENGTH				0x1E

/**
  * @brief UHF Proc State
 */
typedef enum
{
	EN_UHF_RESET,
	EN_UHF_SETADDR,
	EN_UHF_SETANTENNA,
	EN_UHF_SETPOWER,
	EN_UHF_LISTCARD,
	EN_UHF_READCARD,
	EN_UHF_WRITECAED,
	EN_UHF_WRITERCV,
}EN_UHF_PROC;

/**
  * @brief UHF天线
 */
typedef enum
{
	EN_UHF_ANTENNA_1 = 0x00,
	EN_UHF_ANTENNA_2,	
	EN_UHF_ANTENNA_3,
	EN_UHF_ANTENNA_4,
}EN_UHF_ANTENNA;

/**
  * @brief  UHFCMD 
 */
typedef enum
{
    UHF_CMD_RESET = 0x70,       /** 复位读卡器 */
    UHF_CMD_SET_UART_BAUDRATE, /** 设置串口通讯波特率 */
    UHF_CMD_GET_FW, /** 读取读写器固件版本 */
    UHF_CMD_SET_READER_ADDRESS, /** 设置读写器地址 */
    UHF_CMD_SET_WORK_ANTENNA, /** 设置读写器工作天线 */
    UHF_CMD_GET_WORK_ANTENNA, /** 查询当前天线工作天线 */
    UHF_CMD_SET_OUTPUT_POWER, /** 设置读写器射频输出功率 */
    UHF_CMD_GET_OUTPUT_POWER, /** 查询读写器当前输出功率 */
    UHF_CMD_SET_FREQUENCY, /** 设置读写器工作频率范围 */
    UHF_CMD_GET_FREQUENCY, /** 查询读写器工作频率范围 */
    UHF_CMD_SET_BEEPER_MODE, /** 设置蜂鸣器状态 */
    UHF_CMD_GET_READER_TEMP, /** 查询当前设备的工作温度 */
    UHF_CMD_SET_DRM_MODE, /** 设置DRM 状态 */
    UHF_CMD_GET_DRM_MODE, /** 查询DRM 状态 */
    
    UHF_CMD_READ_GPIO_VALUE = 0x60, /** 读取GPIO 电平 */
    UHF_CMD_WRITE_GPIO_VALUE, /** 设置GPIO 电平 */
    UHF_CMD_SET_ANT_DETECTOR, /** 设置天线连接检测器状态 */
    UHF_CMD_GET_ANT_DETECTOR, /** 读取天线连接检测器状态 */
    UHF_CMD_SET_TEMPORARY_POWER = 0x66, /** 设置读写器临时射频输出功率 */
    UHF_CMD_SET_READER_ID, /** 设置读写器识别码 */
    UHF_CMD_GET_READER_ID, /** 读取读写器识别码 */
    
    /**  */
    UHF_CMD_INVENTORY = 0x80, /** 盘存标签 */
    UHF_CMD_READ, /** 读标签 */
    UHF_CMD_WRITE, /** 写标签 */
    UHF_CMD_LOCK, /** 锁定标签 */
    UHF_CMD_KILL, /** 灭活标签 */
    UHF_CMD_SET_ACCESS_EPC_MATCH, /** 匹配ACCESS 操作的EPC 号 */
    UHF_CMD_GET_ACCESS_EPC_MATCH, /** 查询匹配的EPC 状态 */
    UHF_CMD_REAL_TIME_INVENTORY = 0x89, /** 盘存标签(实时上传标签数据) */
    UHF_CMD_FAST_SWITCH_ANT_INVENTORY, /** 快速轮询多个天线盘存标签 */
    UHF_CMD_CUSTOMIZED, /** 自定义session 和target 盘存 */
    UHF_CMD_SET_IMPINJ_FAST_TID, /** 设置Monza 标签快速读TID (设置不被保存至内部FLASH) */
    UHF_CMD_SAVE_IMPINJ_FAST_TID, /** 设置Monza 标签快速读TID (设置被保存至内部FLASH) */
    UHF_CMD_GET_IMPINJ_FAST_TID, /** 查询当前的快速TID 设置 */
    
    UHF_CMD_GET_INVENTORY_BUFFER = 0x90, /** 提取标签数据并删除缓存 */
    UHF_CMD_GET_AND_RESET_INVENTORY_BUFFER, /** 提取标签数据保留缓存备份 */
    UHF_CMD_GET_INVENTORY_BUFFER_TAG_COUNT, /** 查询缓存中已读标签个数 */
    UHF_CMD_RESET_INVENTORY_BUFFER, /** 清空标签数据缓存 */
}EN_UHF_CMD;

/**
  * @brief  UHFSTATUES
 */
typedef enum
{
    UHF_STATUS_SUCCESS = 0x10, /** 命令成功完成 */
    UHF_STATUS_FAIL, /** 命令执行失败 */
    UHF_STATUS_RESET_ERROR = 0x20, /** CPU 复位错误 */
    UHF_STATUS_CW_ON_ERROR, /** 打开CW 错误 */
    UHF_STATUS_ANTENNA_MISSING_ERROR, /** 天线未连接 */
    UHF_STATUS_WRITE_FLASH_ERROR, /** 写Flash 错误 */
    UHF_STATUS_READ_FLASH_ERROR, /** 读Flash 错误 */
    UHF_STATUS_SET_POWER_ERROR, /** 设置发射功率错误 */
    UHF_STATUS_TAG_INVENTORY_ERROR =0x31, /** 盘存标签错误 */
    UHF_STATUS_TAG_READ_ERROR, /** 读标签错误 */
    UHF_STATUS_TAG_WRITE_ERROR, /** 写标签错误 */
    UHF_STATUS_TAG_LOCK_ERROR, /** 锁定标签错误 */
    UHF_STATUS_TAG_KILL_ERROR, /** 灭活标签错误 */
    UHF_STATUS_NO_TAG_ERROR, /** 无可操作标签错误 */
    UHF_STATUS_INVENTORY_OK_ACCESS_FAIL, /** 成功盘存但访问失败 */
    UHF_STATUS_BUFFER_EMPTY_ERROR, /** 缓存为空 */
    UHF_STATUS_ACCESS_OR_PASSWORD_ERROR = 0x40, /** 访问标签错误或访问密码错误 */
    UHF_STATUS_PARAM_INVALID, /** 无效的参数 */
    UHF_STATUS_PARAM_INVALID_WORD_CNT_TOO_LONG, /** wordCnt 参数超过规定长度 */
    UHF_STATUS_PARAM_INVALID_MEMBANK_RANGE, /** MemBank 参数超出范围 */
    UHF_STATUS_PARAM_INVALID_LOCK_REGION_RANGE, /** Lock 数据区参数超出范围 */
    UHF_STATUS_PARAM_INVALID_LOCK_ACTION_RANGE, /** LockType 参数超出范围 */
    UHF_STATUS_PARAM_READER_ADDRESS_INVALID, /** 读写器地址无效 */
    UHF_STATUS_PARAM_INVALID_ANTENNA_ID_RANGE, /** Antenna_id 超出范围 */
    UHF_STATUS_PARAM_INVALID_OUTPUT_POWER_RANGE, /** 输出功率参数超出范围 */
    UHF_STATUS_PARAM_INVALID_FREQUENCY_REGION_RANGE, /** 射频规范区域参数超出范围 */
    UHF_STATUS_PARAM_INVALID_BAUDRATE_RANGE, /** 波特率参数超出范围 */
    UHF_STATUS_PARAM_BEEPER_MODE_RANGE, /** 蜂鸣器设置参数超出范围 */
    UHF_STATUS_PARAM_EPC_MATCH_LEN_TOO_LONG, /** EPC 匹配长度越界 */
    UHF_STATUS_PARAM_EPC_MATCH_LEN_ERROR, /** EPC 匹配长度错误 */
    UHF_STATUS_PARAM_INVALID_EPC_MATCH_MODE, /** EPC 匹配参数超出范围 */
    UHF_STATUS_PARAM_INVALID_FREQUENCY_RANGE, /** 频率范围设置参数错误 */
    UHF_STATUS_FAIL_GET_RN16_FROM_TAG, /** 无法接收标签的RN16 */
    UHF_STATUS_PARAM_INVALID_DRM_MODE, /** DRM 设置参数错误 */
    UHF_STATUS_PLL_LOCK_FAIL, /** PLL 不能锁定 */
    UHF_STATUS_RF_CHIP_FAIL_TO_RESPONSE, /** 射频芯片无响应 */
    UHF_STATUS_FAIL_TO_ACHIEVE_DESIRED_POWER, /** 输出达不到指定的输出功率 */
    UHF_STATUS_COPYRIGHT_AUTH_FAIL, /** 版权认证未通过 */
    UHF_STATUS_SPECTRUM_REGULATION_ERROR, /** 频谱规范设置错误 */
    UHF_STATUS_POWER_TOO_LOW, /** 输出功率过低 */
    UHF_STATUS_TIMEOUT_ERROR = 0xff,
}EN_UHF_STATUS;

/**
  * @brief  UHFREgion
 */
typedef enum
{
    UHF_MEMORY_RESERVE 	= 0x00,	/* 保留 */
    UHF_MEMORY_EPC 		= 0x01,	/* EPC */
    UHF_MEMORY_TID 		= 0x02,	/* TID */
    UHF_MEMORY_USER		= 0x03,	/* USER */   
}EN_UHF_REGION;

/**
  * @brief  UHF PACK
 */
typedef struct
{
	u8 address;
	EN_UHF_CMD cmd;
	u8 data_size;
	u8 data[UHF_DATA_MAX_SIZE];
}ST_UHF_PACK, *PST_UHF_PACK;

/**
  * @brief  UHF PACK
 */
typedef struct
{
	u8 epc_length;
	u8 epc_password[4];
	u8 epc[UHF_EPC_MAX_LENGTH]; 
} ST_UHF_EPC, *PST_UHF_EPC;

/**
  * @brief ReadTag
 */
typedef struct
{
	u16 TagCount;		/* 成功操作的标签总数 */
	u8  DataLen;		/* 成功操作的数据长度 */
	u8  Data[64];		/* PC + EPC + CRC + Data */
}ST_READ_TAG, *PST_READ_TAG;



/**
  * @brief  UHF Device
 */
typedef struct 
{
	ST_UHF_EPC		st_UHF_epc;					/* EPC */
	ST_UHF_PACK		st_UHF_pack;				/* UHF发送接收包 */
	u8				address;					/* 地址 */
	EN_UHF_CMD		cmd;						/* 命令 */
	u8*				ctlData;					/* 数据 */
	u32				Timeout;					/* 超时时间 */
}ST_UHF_Device, *PST_UHF_Device;

int _cmd_900m_list_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
int _cmd_900m_read_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
int _cmd_900m_write_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_SET_ANTENNA(PST_UHF_Device pst_UHF_device, u8 antenna, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_SET_POWER(PST_UHF_Device pst_UHF_device, u8 power_dbm, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_RESET(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_Init(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_SETADDR(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
#endif