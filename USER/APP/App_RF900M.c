/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		App_RF900M.c
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app�ӿں����ļ�
  */
#include "App_RF900M.h"
#include "UHF_reader_Driver.h"
#include "string.h"
#include "bsp_dma.h"
#include "app_nbiot.h"
#include "app_device.h"
#include "app_uart.h"
#include "app_beep.h"

#define UHF900M_ADDR 0x01		/* ����900M���豸��ַ */


/**
  * @brief   900MѰ��ǩ
  * @param   packet ���ݰ�
  * @retval  Ѱ��ǩ���
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
	/* ����EPC���ݵ�NBIOTƽ̨ */;
	GropPackNBMsgSend(pst_UHF_Dev->st_UHF_epc.epc, epc_len, 0x00);
#endif
	return 0;
}

/**
  * @brief   900M����ǩ
  * @param   packet ���ݰ�
  * @retval  Ѱ��ǩ���
 */
int _cmd_900m_read_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device)
{
	u8 block_data[16] = {0};
	u8 ret = 0;
	ST_READ_TAG st_READ_TAG = {0};							/* �������ݱ���ṹ�� */
	pst_UHF_Dev->cmd = UHF_CMD_READ;						/* ����ǩ���� */
	ret = UhfWRdata_read(pst_UHF_Dev, &st_READ_TAG);		/* ���� */
	/* ���Ͷ������ݵ�NBIOTƽ̨ */;
	GropPackNBMsgSend(st_READ_TAG.Data, st_READ_TAG.DataLen, ret);
	return 0;
}

/**
  * @brief   900Mд��ǩ
  * @param   packet ���ݰ�
  * @retval  Ѱ��ǩ���
 */
int _cmd_900m_write_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device)
{
	UhfSelect_TAG(packet, pst_UHF_Dev);				/* ѡ����ǩ */
	pst_UHF_Dev->cmd = UHF_CMD_WRITE;				/* д��ǩ���� */
    if(UhfWRdata_write(pst_UHF_Dev, packet))		/*д��*/
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
	/* ����д�����ݵ�NBIOTƽ̨ */;
	GropPackNBMsgSend(pst_UHF_Dev->st_UHF_epc.epc , pst_UHF_Dev->st_UHF_epc.epc_length, 0x00);
    return 0;
}

/**
  * @brief   900M��ʼ������
  * @param   pst_UHF_device
  * @retval  ����״̬
 */
EN_UHF_STATUS UHF_900M_Init(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	pst_UHF_device->Timeout = 5000;									/* ���ó�ʱʱ�� */
	pst_device->DeviceNum = DEVICE_RFID_900M;
	UHF_900M_RESET(pst_UHF_device, pst_device);						/* ����900Mģ�� */
	UHF_900M_SETADDR(pst_UHF_device, pst_device);					/* ����900Mģ�鹤����ַ */
	UHF_SET_ANTENNA(pst_UHF_device, EN_UHF_ANTENNA_1, pst_device);	/* ���ù������� */
	UHF_SET_POWER(pst_UHF_device, 30, pst_device);					/* ���ù������� */
	OSTimeDlyHMSM(0, 0, 1, 0); 										/* �ӳٵȴ�1sģ��������� */
	debug_printf("UHF 900M Init Over!!!\r\n");
	return UHF_STATUS_SUCCESS;
}

/**
  * @brief   900M���õ�ַ
  * @param   pst_UHF_device
  * @retval  ����״̬
 */
EN_UHF_STATUS UHF_900M_SETADDR(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_SUCCESS;
	debug_printf("UHF900M Set Addr Start!!!\r\n");		/* debug printf */
	u8 UHF_ADDR = UHF900M_ADDR;
	u32 timecount = pst_UHF_device->Timeout;
	pst_UHF_device->address = UHF_ADDR;					/* �������ö�д��ַ */	
	pst_UHF_device->cmd = UHF_CMD_SET_READER_ADDRESS;	/* �������ö�д��ַ */	
	UHF_Tx_PACK(pst_UHF_device, &UHF_ADDR, 0x01);
	debug_printf("UHF900M Set Addr Over!!!\r\n");			/* debug printf */
	pst_device->DeviceAct = EN_UHF_SETADDR;					/* ����״̬Ϊ��ַ���� */
	while(timecount --)						/* �ȴ�Ѱ����Ӧ 6*800 = 4.8S */
	{
		if(WaitUHFDataReceive(pst_UHF_device) == UHF_STATUS_SUCCESS)
		{
			break;/* ��������whileѭ�� */
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 							/* �л������ֹ���� */
	}
	return ret;
}

/**
  * @brief   900M��������
  * @param   pst_UHF_device
  * @retval  ����״̬
 */
EN_UHF_STATUS UHF_900M_RESET(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device)
{
	EN_UHF_STATUS ret = UHF_STATUS_SUCCESS;
	u8 UHF_ADDR = 0xFF;
	pst_UHF_device->address = UHF_ADDR;					/* �������ö�д��ַ */	
	debug_printf("UHF900M Reset Start!!!\r\n");			/* debug printf */
	pst_UHF_device->cmd = UHF_CMD_RESET;
	UHF_Tx_PACK(pst_UHF_device, &UHF_ADDR, 0x00);		/* �������ݰ���UHFģ�� */
	OSTimeDlyHMSM(0, 0, 1, 0); 							/* �ӳٵȴ�1sģ��������� */
	
	debug_printf("UHF900M Reset Over!!!\r\n");			/* debug printf */
	pst_device->DeviceAct = EN_UHF_RESET;				/* ����״̬Ϊ���� */
	return ret;
}

/**
  * @brief   900M�������ߺ�
  * @param   pst_UHF_device
  * @retval  �������߽��
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
	pst_device->DeviceAct = EN_UHF_SETANTENNA;				/* ����״̬Ϊ�������� */
	while(timecount --)						/* �ȴ�Ѱ����Ӧ 6*800 = 4.8S */
	{
		WaitUHFDataReceive(pst_UHF_device);
	}
	return ret;
}

/**
  * @brief   900M���ù���
  * @param   pst_UHF_device
  * @retval  �������߽��
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
	while(timecount --)						/* �ȴ�Ѱ����Ӧ 6*800 = 4.8S */
	{
		WaitUHFDataReceive(pst_UHF_device);
	}
	pst_device->DeviceAct = EN_UHF_SETPOWER;				/* ����״̬Ϊ�������� */
	return ret;
}

