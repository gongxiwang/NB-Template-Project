/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		UHF_reader_Driver.c
  * @author		qyz
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		UHF_Reader����
  */
/**
  * @brief Multi-Include-Prevent Section
  */
/**
  * @brief Include File Section
  * @brief ����ͷ�ļ�
  */
#include "UHF_Reader_Driver.h"
#include "bsp.h"
#include "stdio.h"
#include "string.h"
#include "App_RF900M.h"
#include "bsp_uart.h"
#include "bsp_dma.h"
#include "app_uart.h"


/**
  * @brief  check_sum 
 */
static u8 check_sum(const u8 *pbuffer, u8 size)
{
    u8 sum = 0;
    for(u8 i = 0; i < size; ++i)
    {
        sum += *pbuffer++;
    }
    sum = ~sum + 1;
    
    return sum;
}

/**
  * @brief  packet_to_buffer 
 */
static u8 packet_to_buffer(const PST_UHF_PACK packet, u8 *pbuffer)
{
    u8 *p = pbuffer;
    
    *p = 0xA0;
    p++;
    
    *p = packet->data_size + 3;
    p++;
    
    *p = packet->address;
    p++;
    
    *p = packet->cmd;
    p++;
    
    if(packet->data_size)
    {
        memcpy(p, packet->data, packet->data_size);
        p += packet->data_size;
    }
    
    *p = check_sum(pbuffer, p - pbuffer);
    p++;
    
    return (p - pbuffer);
}

/**
  * @brief  buffer_to_packet 
 */
static u8 buffer_to_packet(const u8 *pbuffer, PST_UHF_PACK packet)
{
    const u8 *p = pbuffer;
    u8 len;
    u8 sum;
    
    if(*p != 0xA0)
        return 0;
    p++;
    
    len = *p++;
    packet->data_size = len - 3;
    
    if(len < 3)
        return 0;
    
    packet->address = *p++;
    packet->cmd = (EN_UHF_CMD)*p++;
    
    if(packet->data_size)
    {
        memcpy(packet->data, p, packet->data_size);
        p += packet->data_size;
    }
    
    sum = check_sum(pbuffer, len + 1);
    
    if(sum != *p)
        return 0;
    
    p++;
    
    return (p - pbuffer);    
}

/**
  * @brief  UHF_Tx_PACK 
 */
EN_UHF_STATUS UHF_Tx_PACK(PST_UHF_Device pst_UHF_device, const u8 *pdata, u8 data_size)
{
	u8 tx_buffer[UHF_TX_BUFFER_MAX_SIZE] = {0};
	u8 tx_valid_size;
	u8 ret = UHF_STATUS_FAIL;

	pst_UHF_device->st_UHF_pack.address = pst_UHF_device->address;
	pst_UHF_device->st_UHF_pack.cmd = pst_UHF_device->cmd;
	pst_UHF_device->st_UHF_pack.data_size = data_size;
	
	for(u8 i = 0; i < data_size; ++ i)
	{
		pst_UHF_device->st_UHF_pack.data[i] = pdata[i];							/* ���ݰ����ݸ�ֵ */
	}
	tx_valid_size = packet_to_buffer(&pst_UHF_device->st_UHF_pack, tx_buffer);	/* ���ݰ�ת�������� */

	if(tx_valid_size > 0)
	{
		/*** debug��ӡ���͵�UHF900���� ***/
		debug_printf("\r\nUHF Send Data:");
		for(int i = 0; i < tx_valid_size; i ++)
		{
			debug_printf("%02X ", tx_buffer[i]);
		}
		debug_printf("\r\n");
		/********************************/

		/* ���͵�900M�豸 */
		USART1DMA_Send_Data(tx_buffer, tx_valid_size);	
		ret = UHF_STATUS_SUCCESS;
	}
	return (ret > 0 ? UHF_STATUS_SUCCESS : UHF_STATUS_FAIL);
}

/**
  * @brief		UhfWRdata_list()
  * @details	Ѱ��ǩ
  * @param 	UHF_Uid �洢��
  * @retval 	0 ʧ�� N�ɹ� ����EPC�ų���
  */
u8 UhfWRdata_list(PST_UHF_Device pst_UHF_Dev)	
{			
	u8 UHF_Repeat = 0x01;					/* ����Э��Repeat�ֶ�����0xFF--�̴�������ǩ���� */
	EN_UHF_STATUS ret = UHF_STATUS_FAIL;
	u32 timecount = pst_UHF_Dev->Timeout;
	/* �����̴�����-Ѱ��-���ݳ�����0x01 */
	UHF_Tx_PACK(pst_UHF_Dev, &UHF_Repeat, 0x01);
#if 1
	while(timecount --)						/* �ȴ�Ѱ����Ӧ 6*800 = 4.8S */
	{
		if(WaitUHFDataReceive(pst_UHF_Dev) == UHF_STATUS_SUCCESS)
		{
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 
	}
	/* EPC���ݸ�ֵ */
	pst_UHF_Dev->st_UHF_epc.epc_length = (pst_UHF_Dev->st_UHF_pack.data[1] >> 3) *2;
	memcpy(pst_UHF_Dev->st_UHF_epc.epc, &pst_UHF_Dev->st_UHF_pack.data[3], pst_UHF_Dev->st_UHF_epc.epc_length);
	return (pst_UHF_Dev->st_UHF_pack.data_size - 4);		/* ����EPC���ݳ��� */
#endif
}

/**
  * @brief		WaitUHFDataReceive()
  * @details	Ѱ��ǩ
  * @param 	UHF_Uid �洢��
  * @retval 	
  */
EN_UHF_STATUS WaitUHFDataReceive(PST_UHF_Device pst_UHF_Dev)
{
	u8 UsartSensorData[256] = {0};							/* ���������մ����������� */
	u32 ulReceiveLen = 0;									/* ����Ľ������ݳ��� */
	ulReceiveLen = Uart1RecvData((u8*)UsartSensorData);		/* ���մ���1������������ */
	if(ulReceiveLen >= 0x01)
	{
		/* ��ӡ���� */
		debug_printf("UHF900M Recv:");
		for(int i = 0; i < ulReceiveLen; i ++)
		{
			debug_printf("%02X ", UsartSensorData[i]);
		}
		debug_printf("\r\n");
		
		/* ֻ��Ѱ����������д��ʱ�����ݷ��� */
		if((pst_UHF_Dev->cmd == UHF_CMD_REAL_TIME_INVENTORY) || (pst_UHF_Dev->cmd == UHF_CMD_READ)
			|| (pst_UHF_Dev->cmd == UHF_CMD_WRITE))
		{
			debug_printf("Buffer to Packet Starting ......\r\n");
			buffer_to_packet(UsartSensorData, &pst_UHF_Dev->st_UHF_pack);/* �ѽ��յ���������PACK */
		}
		return UHF_STATUS_SUCCESS;
	}
	return 0;
}

/**
  * @brief		UhfWRdata_read()
  * @details	ѡ����ǩ
  * @param 	bolck_data	���ݴ洢��
  * @retval 	0 ʧ�� 1 �ɹ�
  */
u8 UhfSelect_TAG(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev)	
{
	u8 UHF_ReadDataRegion[64] = {0x00, 0x0C};
	u32 timecount = pst_UHF_Dev->Timeout;
	pst_UHF_Dev->cmd = UHF_CMD_SET_ACCESS_EPC_MATCH;/* ѡ����ǩ���� */
	EN_UHF_STATUS ret = UHF_STATUS_FAIL;
	for(int i = 0; i < 12; i ++)
	{
		UHF_ReadDataRegion[2 + i] = packet->data[i]; /* epc��ֵ */
	}
	/* �����̴�����-Ѱ��-���ݳ��� */
	UHF_Tx_PACK(pst_UHF_Dev, UHF_ReadDataRegion, 0x0E);
#if 1
	debug_printf("Waitting Select Tag Data");
	while(timecount --)						/* �ȴ�Ѱ����Ӧ5S */
	{
		debug_printf("...");
		if(WaitUHFDataReceive(pst_UHF_Dev) == UHF_STATUS_SUCCESS)
		{
			debug_printf("\r\n");/* ��ӡ���� */
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 
	}
	debug_printf("\r\n");	/* ��ӡ���� */

	if(timecount == 0)		/* �ж��Ƿ�ʱ�˳� */
	{
		return ACK_CARD_HANDLE_ERR;
	}
#endif

	return 0;					/* ���ض�ȡ�������ݳ��� */
}



/**
  * @brief		UhfWRdata_read()
  * @details	����ǩ����
  * @param 	bolck_data	���ݴ洢��
  * @retval 	0 ʧ�� 1 �ɹ�
  */
u8 UhfWRdata_read(PST_UHF_Device pst_UHF_Dev, PST_READ_TAG pst_READ_TAG)	
{
	u8 UHF_ReadDataRegion[3] = {0x03, 0x00, 0x08};
	u32 timecount = pst_UHF_Dev->Timeout;
	EN_UHF_STATUS ret = UHF_STATUS_FAIL;
	/* �����̴�����-Ѱ��-���ݳ�����0x01 */
	UHF_Tx_PACK(pst_UHF_Dev, UHF_ReadDataRegion, 0x03);
	debug_printf("Start Recving UHF Data...");
	while(timecount --)						/* �ȴ�Ѱ����Ӧ5S */
	{
		debug_printf("...");
		if(WaitUHFDataReceive(pst_UHF_Dev) == UHF_STATUS_SUCCESS)
		{
			debug_printf("\r\n");/* ��ӡ���� */
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 
	}
	debug_printf("\r\n");	/* ��ӡ���� */
	if(timecount == 0)		/* �ж��Ƿ�ʱ�˳� */
	{
		return ACK_CARD_HANDLE_ERR;
	}
	/* ��ȡ���ı�ǩ���ݸ�ֵ*/
	debug_printf("Read Card Data:");
	for(int i = 0; i < pst_UHF_Dev->st_UHF_pack.data_size; i ++)
	{
		debug_printf("%02X ", pst_UHF_Dev->st_UHF_pack.data[i]);
	}
	debug_printf("\n");
	pst_READ_TAG->DataLen = (pst_UHF_Dev->st_UHF_pack.data[2] - 0x10);								/* ��ǩ���ݳ��ȸ�ֵ--PC+EPC+DATA */
	memcpy(pst_READ_TAG->Data, &pst_UHF_Dev->st_UHF_pack.data[19], pst_READ_TAG->DataLen);		/* ��ǩ���ݸ�ֵ */

	/* ��ȡ���ı�ǩ���ݸ�ֵ*/
	debug_printf("ANISIS Card Data:");
	for(int i = 0; i < pst_READ_TAG->DataLen; i ++)
	{
		debug_printf("%02X ", pst_READ_TAG->Data[i]);
	}
	debug_printf("\n");
	
	return (pst_READ_TAG->DataLen);					/* ���ض�ȡ�������ݳ��� */
}

/**
  * @brief		UhfWRdata_write()
  * @details	д��ǩ����
  * @param 	bolck_data	���ݴ洢��
  * @retval 	0 ʧ�� 1 �ɹ�
  */
u8 UhfWRdata_write(PST_UHF_Device pst_UHF_Dev, Device_packet_t *packet)	
{
	u8 UHF_WriteData[64] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0X08};
	memcpy(pst_UHF_Dev->st_UHF_epc.epc, packet->data, 12);/* epc��ֵ */
	debug_printf("Read Write Data:");
	for(int i = 0; i < 16; i ++)						/* Э��涨д��16�ֽ����� */		
	{
		UHF_WriteData[7 + i] = packet->data[12 + i];	/* ��ֵ��Ҫд������� */
		debug_printf("%02X ", packet->data[12 + i]);
	}
	debug_printf("\r\n");
	EN_UHF_STATUS ret = UHF_STATUS_FAIL;
	u32 timecount = pst_UHF_Dev->Timeout;
	/* �����̴�����-д������ */
	UHF_Tx_PACK(pst_UHF_Dev, UHF_WriteData, 0x17);
	debug_printf("Wait Write Card return...");
	while(timecount --)						/* �ȴ�Ѱ����Ӧ 6*800 = 4.8S */
	{
		debug_printf("...");
		if(WaitUHFDataReceive(pst_UHF_Dev) == UHF_STATUS_SUCCESS)
		{
			debug_printf("\r\n");/* ��ӡ���� */
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 1); 
	}
	debug_printf("\r\n");
	if(timecount == 0)		/* �ж��Ƿ�ʱ�˳� */
	{
		return ACK_CARD_HANDLE_ERR;
	}

	return pst_UHF_Dev->st_UHF_epc.epc_length;
}

