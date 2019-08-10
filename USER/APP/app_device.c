/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_device.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		传感器接口函数
  */

#include "bsp.h"
#include "app_adc.h"
#include "app_uart.h"
#include "bsp_uart.h"
#include "app_modbus.h"
#include "app_device.h"
#include "app_nbiot.h"
#include "stdio.h"
#include <stdarg.h>
#include "bsp.h"
#include "bsp_dma.h"
#include "app_device.h"
#include "App_RF900M.h"
#include "App_RF13P56M.h"
#include "app_watmet.h"
#include "app_CO2.h"
#include "app_templg.h"

#define SCANGUNDATASIZE 	0x06						/* 扫码枪的数据长度 */
#define WATMETDATASIZE 		0x08						/* 水表数据长度 */
#define ICREADERSIZE		0x0B						/* 13.56M--ICreader数据长度     */

/**
  * @brief  Device结构体:
 */
static ST_DEVICE en_device = {0};  						/* 传感器设备 */
/**
  * @brief  ST_UHF900_Device 
 */
static ST_UHF_Device st_UHF_device = {0};
extern ST_PACK_DATA   g_stRecvData;

/**
  * @brief  缓冲区内存块个数 
 */
#define Device_memory_blk_number 2


/**
  * @brief  队列长度 
 */
#define queue_number Device_memory_blk_number

/**
  * @brief  申请内存作为缓冲区 
 */
static Device_packet_t DEVICE_MEMORY[Device_memory_blk_number];

/**
  * @brief  队列内存指针 
 */
static Device_packet_t *pqueue_storge[queue_number];
/**
  * @brief  队列头 
 */
static OS_EVENT *Device_queue = 0;
/**
  * @brief  内存块头 
 */
static OS_MEM *Device_mem = 0;

/**
  * @brief  每个内存块的大小 
 */
#define Device_memory_blk_size sizeof(Device_packet_t)


/**
  * @brief App_DEVICE_TASK
  * @details 设备处理任务
  * @param void *p_data
  * @retval NULL
 */
void App_DEVICE_TASK(void *p_data) 
{
	u8 err;														
	Device_queue = OSQCreate((void *)pqueue_storge, queue_number);
	assert_param(Device_queue);						/* 检查队列合法性 */
	Device_mem = OSMemCreate(DEVICE_MEMORY, Device_memory_blk_number, Device_memory_blk_size, &err);
	debug_printf( "[%s] enter.\r\n", __func__ );	/* debug */
	while(1)
	{
		DoDevCtl(&en_device);						/* 传感器数据处理 */
		DevActionProc(&en_device);
		OSTimeDlyHMSM(0, 0, 0, 1); 					/* 切换任务 */		
	}	
}

/**
  * @brief 执行控制数据处理--单纯接收、控制电机、灯等
  * @details 无
  * @param CtlData
  * @retval 无
 */
void DoDevCtl(PST_DEVICE pst_dev)
{
	u8 err;
	u8 buffer_trash[200] = {0};
	Device_packet_t *packet = OSQPend(Device_queue, 100, &err);	/* 从数据队列中接收数据 */
	if(DEVICE_SENSOR_SCANGUN == packet->Device_type)			/* 判断什么设备---扫码枪 */
	{
		/* 485测试 */
		OPENRS_485;
		ENRS485_RX; 											/* 使能RS485接收 */
		OSTimeDlyHMSM(0, 0, 0, 10); 							/* 使能接收后需要延迟10ms */
		en_device.DeviceNum = DEVICE_SENSOR_SCANGUN;
		debug_printf("Scan Gun online!\r\n");
	}
	
	else if(DEVICE_SENSOR_WATMET == packet->Device_type)		/* 水表设备 */
	{
		Watmet_Init();											/* 初始化水表 */
		Watmet_Read();											/* 查询用水量 */
		en_device.DeviceNum = DEVICE_SENSOR_WATMET;
		debug_printf("Water meter online!\r\n");
	}
	
#if 0	
	else if(DEVICE_RFID_IC_READER == packet->Device_type)		/* IC卡读写器---暂时不用 */
	{
		en_device.DeviceNum = DEVICE_RFID_IC_READER;			/* 设备编号为IC卡读写器 */
		debug_printf("IC Reader online!\r\n");
	}
#endif

	else if(DEVICE_RFID_900M == packet->Device_type)			/* 900M设备 */
	{
		OPENRS_232; 											/* 打开RS232 */
		OSTimeDlyHMSM(0, 0, 0, 10);								/* 延迟10ms */ 	
		/* 使能接收后需要延迟10ms */	
		UHF_900M_Init(&st_UHF_device, &en_device);				/* 900M初始化 */
		debug_printf("RFID 900M online!\r\n");
		switch(packet->Command_code)							/* 判断900M命令 */
		{		
			case SEARCH_CARD:/* 寻900m标签 */
				debug_printf("UHF900M_CMD:SEARCH CARD!!!\r\n");
				_cmd_900m_list_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = SEARCH_CARD;
				break;
			case READ_CARD:/* 读取900M标签数据 */
				debug_printf("UHF900M_CMD:READ CARD!!!\r\n");
				_cmd_900m_read_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = READ_CARD;
				break;
			case WRITE_CARD:/* 写数据到900M标签 */
				debug_printf("UHF900M_CMD:WRITE CARD!!!\r\n");
				_cmd_900m_write_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = WRITE_CARD;
				break;			   
			default:
				break;
		}
	}
#if 0	
	else if(DEVICE_RFID_13P56M == packet->Device_type)			/* 暂时不用 */
	{
		OPENRS_232; 											/* 打开RS232 */
		OSTimeDlyHMSM(0, 0, 0, 10); 							/* 使能接收后需要延迟10ms */	
		en_device.DeviceNum = DEVICE_RFID_13P56M;				/* 设置执行的设备为13.56M */
		debug_printf("RFID 13.56M online!\r\n");

		switch(packet->Command_code)							/* 判断13.56M命令 */
		{		
			case SEARCH_CARD:/* 寻13.56M标签 */
				debug_printf("SEARCH CARD\r\n");
				_cmd_13p56m_list_card(packet);
				break;
			case READ_CARD:/* 读取13.56M标签数据 */
				debug_printf("READ CARD\r\n");
				_cmd_13p56m_read_card(packet);
				break;
			case WRITE_CARD:/* 写数据到13.56M标签 */
				debug_printf("WRITE CARD\r\n");
				_cmd_13p56m_write_card(packet);
				break;
			default:
				break;
		}

	}
#endif	
	else if(DEVICE_POWER_PUMP == packet->Device_type)		/* 继电器控制水泵 */
	{
		en_device.DeviceNum = DEVICE_POWER_PUMP;			
	}
	
	else if(DEVICE_POWER_WARMLG == packet->Device_type) 	/* 继电器控制加热灯 */
	{
		en_device.DeviceNum = DEVICE_POWER_WARMLG;			
	}
	
	else if(DEVICE_SENSOR_CO2 == packet->Device_type) 		/* CO2传感器 */
	{
		InitCO2Sensor();
		ReadCO2SensorValue();
		en_device.DeviceNum = DEVICE_SENSOR_CO2;			
	}	
	
	else if(DEVICE_SENSOR_TMPLG == packet->Device_type) 	/* 温湿度光照3合一 */
	{
		InitTemplgSensor();
		ReadTemplgSensorValue(EN_TYPE_LIGHT);
		ReadTemplgSensorValue(EN_TYPE_TEPHUM);
		en_device.DeviceNum = DEVICE_SENSOR_TMPLG;		
	}
	
	else if(DEVICE_DOOR == packet->Device_type) 			/* 继电器控制门闸 */
	{
		en_device.DeviceNum = DEVICE_DOOR;					
	}

	else if(DEVICE_POWER_TRALG == packet->Device_type)		/* 红绿灯模块 */
	{
		en_device.DeviceNum = DEVICE_POWER_TRALG;	
	}
	
	OSMemPut(Device_mem, packet); 							/* 释放Device使用的内存块 */
}

/**
  * @brief   信息数据接口
  * @details 接收一个数据包
  * @param  packet 接收到的数据包
 */
void DevActionProc(PST_DEVICE pst_device)
{
	u8 UsartSensorData[256] = {0};								/* 传感器接收串口数据数组 */
	u32 ulReceiveLen = 0;										/* 保存的接收数据长度 */
	if(DEVICE_SENSOR_SCANGUN == pst_device->DeviceNum) 			/* 扫码枪 */
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* 接收串口1传过来的数据 */
		if(ulReceiveLen >= SCANGUNDATASIZE) 					/* 判断接收的数据长度 */
		{			
			GropPackNBMsgSend(UsartSensorData, ulReceiveLen, 0x00);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	}
#if 0	
	else if(DEVICE_RFID_13P56M == pst_device->DeviceNum) 		/* 扫码枪 */
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* 接收串口1传过来的数据 */
		if(ulReceiveLen >= 0) 						/* 判断接收的数据长度 */
		{
			GropPackNBMsgSend(UsartSensorData, ulReceiveLen, 0x00);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	}

	else if(DEVICE_RFID_900M == pst_device->DeviceNum) 			/* UHF900M */
	{

		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* 接收串口1传过来的数据 */
		if(ulReceiveLen > 0) 									/* 判断接收的数据长度 */
		{
			debug_printf("Recv UHF Data!!!\r\n");
			/* 打印测试 */
			for(int i = 0; i < ulReceiveLen; i ++)
			{
				debug_printf("UHF Recv Data[%d] = %02X\n", i, UsartSensorData[i]);
			}
			//GropPackNBMsgSend(UsartSensorData, ulReceiveLen);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	
		
		if(WaitUHFDataReceive(&st_UHF_device) == UHF_STATUS_SUCCESS)
		{	
			/* EPC数据赋值 */
			st_UHF_device.st_UHF_epc.epc_length = (st_UHF_device.st_UHF_pack.data[1] >> 3) * 2;
			memcpy(st_UHF_device.st_UHF_epc.epc, &st_UHF_device.st_UHF_pack.data[3], st_UHF_device.st_UHF_epc.epc_length);
			//return (&st_UHF_device->st_UHF_pack.data_size - 4);		/* 返回EPC数据长度 */
			debug_printf("EPC = %d, ", st_UHF_device.st_UHF_epc.epc_length);
			for(int i = 0; i < st_UHF_device.st_UHF_epc.epc_length; i ++)
			{
				debug_printf("%02X ", st_UHF_device.st_UHF_epc.epc[i]);
			}
			debug_printf("\r\n");
			pst_device->DeviceNum = DEVICE_NULL;
		}		
	}
#endif
}

/**
  * @brief   信息数据接口
  * @details 接收一个数据包
  * @param  packet 接收到的数据包
  * @retval 
 */
void DevicePacket_post(Device_packet_t *packet)
{
    u8 err;    
    Device_packet_t *new = OSMemGet(Device_mem, &err);
    if(!new)
        return;
    memcpy(new, packet, sizeof(Device_packet_t));
    OSQPost(Device_queue, (void *)new);
}















