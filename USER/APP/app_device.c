/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		app_device.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		�������ӿں���
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

#define SCANGUNDATASIZE 	0x06						/* ɨ��ǹ�����ݳ��� */
#define WATMETDATASIZE 		0x08						/* ˮ�����ݳ��� */
#define ICREADERSIZE		0x0B						/* 13.56M--ICreader���ݳ���     */

/**
  * @brief  Device�ṹ��:
 */
static ST_DEVICE en_device = {0};  						/* �������豸 */
/**
  * @brief  ST_UHF900_Device 
 */
static ST_UHF_Device st_UHF_device = {0};
extern ST_PACK_DATA   g_stRecvData;

/**
  * @brief  �������ڴ����� 
 */
#define Device_memory_blk_number 2


/**
  * @brief  ���г��� 
 */
#define queue_number Device_memory_blk_number

/**
  * @brief  �����ڴ���Ϊ������ 
 */
static Device_packet_t DEVICE_MEMORY[Device_memory_blk_number];

/**
  * @brief  �����ڴ�ָ�� 
 */
static Device_packet_t *pqueue_storge[queue_number];
/**
  * @brief  ����ͷ 
 */
static OS_EVENT *Device_queue = 0;
/**
  * @brief  �ڴ��ͷ 
 */
static OS_MEM *Device_mem = 0;

/**
  * @brief  ÿ���ڴ��Ĵ�С 
 */
#define Device_memory_blk_size sizeof(Device_packet_t)


/**
  * @brief App_DEVICE_TASK
  * @details �豸��������
  * @param void *p_data
  * @retval NULL
 */
void App_DEVICE_TASK(void *p_data) 
{
	u8 err;														
	Device_queue = OSQCreate((void *)pqueue_storge, queue_number);
	assert_param(Device_queue);						/* �����кϷ��� */
	Device_mem = OSMemCreate(DEVICE_MEMORY, Device_memory_blk_number, Device_memory_blk_size, &err);
	debug_printf( "[%s] enter.\r\n", __func__ );	/* debug */
	while(1)
	{
		DoDevCtl(&en_device);						/* ���������ݴ��� */
		DevActionProc(&en_device);
		OSTimeDlyHMSM(0, 0, 0, 1); 					/* �л����� */		
	}	
}

/**
  * @brief ִ�п������ݴ���--�������ա����Ƶ�����Ƶ�
  * @details ��
  * @param CtlData
  * @retval ��
 */
void DoDevCtl(PST_DEVICE pst_dev)
{
	u8 err;
	u8 buffer_trash[200] = {0};
	Device_packet_t *packet = OSQPend(Device_queue, 100, &err);	/* �����ݶ����н������� */
	if(DEVICE_SENSOR_SCANGUN == packet->Device_type)			/* �ж�ʲô�豸---ɨ��ǹ */
	{
		/* 485���� */
		OPENRS_485;
		ENRS485_RX; 											/* ʹ��RS485���� */
		OSTimeDlyHMSM(0, 0, 0, 10); 							/* ʹ�ܽ��պ���Ҫ�ӳ�10ms */
		en_device.DeviceNum = DEVICE_SENSOR_SCANGUN;
		debug_printf("Scan Gun online!\r\n");
	}
	
	else if(DEVICE_SENSOR_WATMET == packet->Device_type)		/* ˮ���豸 */
	{
		Watmet_Init();											/* ��ʼ��ˮ�� */
		Watmet_Read();											/* ��ѯ��ˮ�� */
		en_device.DeviceNum = DEVICE_SENSOR_WATMET;
		debug_printf("Water meter online!\r\n");
	}
	
#if 0	
	else if(DEVICE_RFID_IC_READER == packet->Device_type)		/* IC����д��---��ʱ���� */
	{
		en_device.DeviceNum = DEVICE_RFID_IC_READER;			/* �豸���ΪIC����д�� */
		debug_printf("IC Reader online!\r\n");
	}
#endif

	else if(DEVICE_RFID_900M == packet->Device_type)			/* 900M�豸 */
	{
		OPENRS_232; 											/* ��RS232 */
		OSTimeDlyHMSM(0, 0, 0, 10);								/* �ӳ�10ms */ 	
		/* ʹ�ܽ��պ���Ҫ�ӳ�10ms */	
		UHF_900M_Init(&st_UHF_device, &en_device);				/* 900M��ʼ�� */
		debug_printf("RFID 900M online!\r\n");
		switch(packet->Command_code)							/* �ж�900M���� */
		{		
			case SEARCH_CARD:/* Ѱ900m��ǩ */
				debug_printf("UHF900M_CMD:SEARCH CARD!!!\r\n");
				_cmd_900m_list_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = SEARCH_CARD;
				break;
			case READ_CARD:/* ��ȡ900M��ǩ���� */
				debug_printf("UHF900M_CMD:READ CARD!!!\r\n");
				_cmd_900m_read_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = READ_CARD;
				break;
			case WRITE_CARD:/* д���ݵ�900M��ǩ */
				debug_printf("UHF900M_CMD:WRITE CARD!!!\r\n");
				_cmd_900m_write_card(packet, &st_UHF_device, &en_device);
				en_device.DeviceAct = WRITE_CARD;
				break;			   
			default:
				break;
		}
	}
#if 0	
	else if(DEVICE_RFID_13P56M == packet->Device_type)			/* ��ʱ���� */
	{
		OPENRS_232; 											/* ��RS232 */
		OSTimeDlyHMSM(0, 0, 0, 10); 							/* ʹ�ܽ��պ���Ҫ�ӳ�10ms */	
		en_device.DeviceNum = DEVICE_RFID_13P56M;				/* ����ִ�е��豸Ϊ13.56M */
		debug_printf("RFID 13.56M online!\r\n");

		switch(packet->Command_code)							/* �ж�13.56M���� */
		{		
			case SEARCH_CARD:/* Ѱ13.56M��ǩ */
				debug_printf("SEARCH CARD\r\n");
				_cmd_13p56m_list_card(packet);
				break;
			case READ_CARD:/* ��ȡ13.56M��ǩ���� */
				debug_printf("READ CARD\r\n");
				_cmd_13p56m_read_card(packet);
				break;
			case WRITE_CARD:/* д���ݵ�13.56M��ǩ */
				debug_printf("WRITE CARD\r\n");
				_cmd_13p56m_write_card(packet);
				break;
			default:
				break;
		}

	}
#endif	
	else if(DEVICE_POWER_PUMP == packet->Device_type)		/* �̵�������ˮ�� */
	{
		en_device.DeviceNum = DEVICE_POWER_PUMP;			
	}
	
	else if(DEVICE_POWER_WARMLG == packet->Device_type) 	/* �̵������Ƽ��ȵ� */
	{
		en_device.DeviceNum = DEVICE_POWER_WARMLG;			
	}
	
	else if(DEVICE_SENSOR_CO2 == packet->Device_type) 		/* CO2������ */
	{
		InitCO2Sensor();
		ReadCO2SensorValue();
		en_device.DeviceNum = DEVICE_SENSOR_CO2;			
	}	
	
	else if(DEVICE_SENSOR_TMPLG == packet->Device_type) 	/* ��ʪ�ȹ���3��һ */
	{
		InitTemplgSensor();
		ReadTemplgSensorValue(EN_TYPE_LIGHT);
		ReadTemplgSensorValue(EN_TYPE_TEPHUM);
		en_device.DeviceNum = DEVICE_SENSOR_TMPLG;		
	}
	
	else if(DEVICE_DOOR == packet->Device_type) 			/* �̵���������բ */
	{
		en_device.DeviceNum = DEVICE_DOOR;					
	}

	else if(DEVICE_POWER_TRALG == packet->Device_type)		/* ���̵�ģ�� */
	{
		en_device.DeviceNum = DEVICE_POWER_TRALG;	
	}
	
	OSMemPut(Device_mem, packet); 							/* �ͷ�Deviceʹ�õ��ڴ�� */
}

/**
  * @brief   ��Ϣ���ݽӿ�
  * @details ����һ�����ݰ�
  * @param  packet ���յ������ݰ�
 */
void DevActionProc(PST_DEVICE pst_device)
{
	u8 UsartSensorData[256] = {0};								/* ���������մ����������� */
	u32 ulReceiveLen = 0;										/* ����Ľ������ݳ��� */
	if(DEVICE_SENSOR_SCANGUN == pst_device->DeviceNum) 			/* ɨ��ǹ */
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* ���մ���1������������ */
		if(ulReceiveLen >= SCANGUNDATASIZE) 					/* �жϽ��յ����ݳ��� */
		{			
			GropPackNBMsgSend(UsartSensorData, ulReceiveLen, 0x00);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	}
#if 0	
	else if(DEVICE_RFID_13P56M == pst_device->DeviceNum) 		/* ɨ��ǹ */
	{
		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* ���մ���1������������ */
		if(ulReceiveLen >= 0) 						/* �жϽ��յ����ݳ��� */
		{
			GropPackNBMsgSend(UsartSensorData, ulReceiveLen, 0x00);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	}

	else if(DEVICE_RFID_900M == pst_device->DeviceNum) 			/* UHF900M */
	{

		ulReceiveLen = Uart1RecvData((u8*)UsartSensorData); 	/* ���մ���1������������ */
		if(ulReceiveLen > 0) 									/* �жϽ��յ����ݳ��� */
		{
			debug_printf("Recv UHF Data!!!\r\n");
			/* ��ӡ���� */
			for(int i = 0; i < ulReceiveLen; i ++)
			{
				debug_printf("UHF Recv Data[%d] = %02X\n", i, UsartSensorData[i]);
			}
			//GropPackNBMsgSend(UsartSensorData, ulReceiveLen);
			pst_device->DeviceNum = DEVICE_NULL;
		}
	
		
		if(WaitUHFDataReceive(&st_UHF_device) == UHF_STATUS_SUCCESS)
		{	
			/* EPC���ݸ�ֵ */
			st_UHF_device.st_UHF_epc.epc_length = (st_UHF_device.st_UHF_pack.data[1] >> 3) * 2;
			memcpy(st_UHF_device.st_UHF_epc.epc, &st_UHF_device.st_UHF_pack.data[3], st_UHF_device.st_UHF_epc.epc_length);
			//return (&st_UHF_device->st_UHF_pack.data_size - 4);		/* ����EPC���ݳ��� */
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
  * @brief   ��Ϣ���ݽӿ�
  * @details ����һ�����ݰ�
  * @param  packet ���յ������ݰ�
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















