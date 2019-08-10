/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		App_RF900M.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app接口函数文件
  */
#include "App_RF900M.h"
#include "UHF_reader_Driver.h"
#include "string.h"
#include "bsp_dma.h"
#include "app_nbiot.h"
#include "app_device.h"
#include "app_uart.h"
#include "app_beep.h"

#define UHF900M_ADDR 0x01		/* 定义900M的设备地址 */


/**
  * @brief   900M寻标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_900m_list_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device)
{
	u8 epc_len;
	pst_UHF_Dev->cmd = UHF_CMD_REAL_TIME_INVENTORY;
	epc_len = UhfWRdata_list(pst_UHF_Dev);

#if 1
	if(epc_len)
	{
		packet->ack = ACK_OK;
		packet->data_size = epc_len;
		memcpy(packet->data, pst_UHF_Dev->st_UHF_epc.epc, epc_len);
		debug_printf("list 900m card epc:");
		for(int i = 0; i < epc_len; i ++)
		{
			debug_printf("%02X ", pst_UHF_Dev->st_UHF_epc.epc[i]);
		}
		debug_printf("\n");
		Beep_output_time(30);
	}
	else
	{
		packet->ack = ACK_CARD_HANDLE_ERR;
		packet->data_size = 0;
	}
	/* 发送EPC数据到NBIOT平台 */;
	GropPackNBMsgSend(pst_UHF_Dev->st_UHF_epc.epc, epc_len, 0x00);
#endif
	return 0;
}

/**
  * @brief   900M读标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_900m_read_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device)
{
	u8 block_data[16] = {0};
	u8 ret = 0;
	ST_READ_TAG st_READ_TAG = {0};							/* 读卡数据保存结构体 */
	pst_UHF_Dev->cmd = UHF_CMD_READ;						/* 读标签命令 */
	ret = UhfWRdata_read(pst_UHF_Dev, &st_READ_TAG);		/* 读卡 */
	/* 发送读卡数据到NBIOT平台 */;
	GropPackNBMsgSend(st_READ_TAG.Data, st_READ_TAG.DataLen, ret);
	return 0;
}

/**
  * @brief   900M写标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_900m_write_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device)
{
	UhfSelect_TAG(packet, pst_UHF_Dev);				/* 选定标签 */
	pst_UHF_Dev->cmd = UHF_CMD_WRITE;				/* 写标签命令 */
    if(UhfWRdata_write(pst_UHF_Dev, packet))		/*写卡*/
    {
        packet->ack = ACK_OK;
        packet->data_size = 0;
        debug_printf("write 900m card block_data:");
        Beep_output_time(30);
    }
    else
    {
        packet->ack = ACK_CARD_HANDLE_ERR;
        packet->data_size = 0;
    }
	/* 发送写卡数据到NBIOT平台 */;
	GropPackNBMsgSend(pst_UHF_Dev->st_UHF_epc.epc , pst_UHF_Dev->st_UHF_epc.epc_length, 0x00);
    return 0;
}

/**
  * @brief   900M初始化命令
  * @param   pst_UHF_device
  * @retval  返回状态
 */
EN_UHF_STATUS UHF_900M_Init(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	pst_UHF_device->Timeout = 5000;									/* 设置超时时间 */
	pst_device->DeviceNum = DEVICE_RFID_900M;
	UHF_900M_RESET(pst_UHF_device, pst_device);						/* 重启900M模块 */
	UHF_900M_SETADDR(pst_UHF_device, pst_device);					/* 设置900M模块工作地址 */
	UHF_SET_ANTENNA(pst_UHF_device, EN_UHF_ANTENNA_1, pst_device);	/* 设置工作天线 */
	UHF_SET_POWER(pst_UHF_device, 30, pst_device);					/* 设置工作功率 */
	OSTimeDlyHMSM(0, 0, 1, 0); 										/* 延迟等待1s模块重启完成 */
	debug_printf("UHF 900M Init Over!!!\r\n");
	return UHF_STATUS_SUCCESS;
}

/**
  * @brief   900M设置地址
  * @param   pst_UHF_device
  * @retval  返回状态
 */
EN_UHF_STATUS UHF_900M_SETADDR(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_SUCCESS;
	debug_printf("UHF900M Set Addr Start!!!\r\n");		/* debug printf */
	u8 UHF_ADDR = UHF900M_ADDR;
	u32 timecount = pst_UHF_device->Timeout;
	pst_UHF_device->address = UHF_ADDR;					/* 首先设置读写地址 */	
	pst_UHF_device->cmd = UHF_CMD_SET_READER_ADDRESS;	/* 首先设置读写地址 */	
	UHF_Tx_PACK(pst_UHF_device, &UHF_ADDR, 0x01);
	debug_printf("UHF900M Set Addr Over!!!\r\n");			/* debug printf */
	pst_device->DeviceAct = EN_UHF_SETADDR;					/* 处理状态为地址设置 */
	while(timecount --)						/* 等待寻卡回应 6*800 = 4.8S */
	{
		if(WaitUHFDataReceive(pst_UHF_device) == UHF_STATUS_SUCCESS)
		{
			break;/* 跳出整个while循环 */
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 							/* 切换任务防止跑死 */
	}
	return ret;
}

/**
  * @brief   900M重启命令
  * @param   pst_UHF_device
  * @retval  返回状态
 */
EN_UHF_STATUS UHF_900M_RESET(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_SUCCESS;
	u8 UHF_ADDR = 0xFF;
	pst_UHF_device->address = UHF_ADDR;					/* 首先设置读写地址 */	
	debug_printf("UHF900M Reset Start!!!\r\n");			/* debug printf */
	pst_UHF_device->cmd = UHF_CMD_RESET;
	UHF_Tx_PACK(pst_UHF_device, &UHF_ADDR, 0x00);		/* 发送数据包到UHF模块 */
	OSTimeDlyHMSM(0, 0, 1, 0); 							/* 延迟等待1s模块重启完成 */
	
	debug_printf("UHF900M Reset Over!!!\r\n");			/* debug printf */
	pst_device->DeviceAct = EN_UHF_RESET;				/* 处理状态为重启 */
	return ret;
}

/**
  * @brief   900M设置天线号
  * @param   pst_UHF_device
  * @retval  设置天线结果
 */
EN_UHF_STATUS UHF_SET_ANTENNA(PST_UHF_Device pst_UHF_device, u8 antenna, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_SUCCESS;
	debug_printf("UHF900M SET ANTENNA Start!!!\r\n");			/* debug printf */
	pst_UHF_device->cmd = UHF_CMD_SET_WORK_ANTENNA;
	u32 timecount = pst_UHF_device->Timeout;
	if(antenna > 3)
		return UHF_STATUS_FAIL;
	UHF_Tx_PACK(pst_UHF_device, &antenna, 0x01);
	debug_printf("UHF900M SET ANTENNA Over!!!\r\n");		/* debug printf */
	pst_device->DeviceAct = EN_UHF_SETANTENNA;				/* 处理状态为设置天线 */
	while(timecount --)						/* 等待寻卡回应 6*800 = 4.8S */
	{
		WaitUHFDataReceive(pst_UHF_device);
	}
	return ret;
}

/**
  * @brief   900M设置功率
  * @param   pst_UHF_device
  * @retval  设置天线结果
 */
EN_UHF_STATUS UHF_SET_POWER(PST_UHF_Device pst_UHF_device, u8 power_dbm, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_FAIL;
	debug_printf("UHF900M SET POWER Start!!!\r\n");	/* debug printf */
	u32 timecount = pst_UHF_device->Timeout;
	if(power_dbm> 33)
		power_dbm = 33;
	UHF_Tx_PACK(pst_UHF_device, &power_dbm, 0x01);
	debug_printf("UHF900M SET POWER Over!!!\r\n");		/* debug printf */
	while(timecount --)						/* 等待寻卡回应 6*800 = 4.8S */
	{
		WaitUHFDataReceive(pst_UHF_device);
	}
	pst_device->DeviceAct = EN_UHF_SETPOWER;				/* 处理状态为功率设置 */
	return ret;
}

