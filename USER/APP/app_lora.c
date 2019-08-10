/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_nbiot.c
  * @author  walle
  * @version  V2.0.0
  * @date  08/05/2019
  * @brief  �û�app_nbiot
  */ 
#include "app_cfg.h"
#include "app_nbiot.h"
#include "app_uart.h"
#include "bsp_uart.h"
#include "bsp.h"
#include "bsp_dma.h"
#include "app_device.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_lora.h"
#include "bsp_lora.h"


#define LORA_CMDEXECUTE_COUNT 	0x03                       	/* BC-35����ִ�д��� */

#define ENABLE_LORA_Tick		0x01						/* ʹ��NBIOT�̶�ʱ�䷢�� */
#define DISABLE_LORA_Tick   	0x00


/* LORA�����ṹ�� */
static ST_LORA_OPERAION st_lora_operation = {0};
static Device_packet_t LORADataPacket = {0};				/* LORA */
static Device_packet_t LORATxdaPacket = {0};

/**
  * @brief LORAģ�����ýṹ��ִ�в���״̬
 */
static void SetLORA_OperatStrState(PST_LORA_OPERATION pst_lora_operation, u8 curState, u8 nextState)
{
	pst_lora_operation->LastOperateState = pst_lora_operation->CurOperateState;
	pst_lora_operation->CurOperateState = curState;
	pst_lora_operation->NextOperateState = nextState;
	debug_printf("%s:SEND\r\n", pst_lora_operation->CurCMD);
}

/**
  * @brief LORAģ�����ýṹ��ִ�д���״̬
 */
static void SetLORA_ProceStrState(PST_LORA_OPERATION pst_lora_operation, u8 curState, u8 nextState)
{
	pst_lora_operation->CurProcState =  curState;
	pst_lora_operation->NextProcState = nextState;
}

/**
  * @brief LORA������ʼ��
 */
static u8 LORA_MODULE_INIT(PST_LORA_OPERATION pst_lora_operation)
{
	pst_lora_operation->CurProcState = EN_LORAPROCESS_OPEN;		/* ��״̬ */
	pst_lora_operation->CurOperateState = EN_LORAOPEN_ADD;		/* ���ȴ�loraAT�������� */
	pst_lora_operation->WAIT_TIME = 1000;						/* �ӳ�ʱ�� */
	pst_lora_operation->ExecuCount = 1;
    return 0;
}

/**
  * @brief LORA������
 */
static u8 LORA_MODULE_OPEN(PST_LORA_OPERATION pst_lora_operation)
{
	if(pst_lora_operation != NULL)
	{
		/* AT������ */
		if(EN_LORAOPEN_ADD == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "+++";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd(pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_REBOOT);
		}
		/* LORAģ������ */
		else if(EN_LORAOPEN_REBOOT == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+SRS\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_CHECKREBOOT);	
		}
		/* LORAģ���������Ƿ�ɹ� */
		else if(EN_LORAOPEN_CHECKREBOOT == pst_lora_operation->CurOperateState)
		{
			OSTimeDlyHMSM(0, 0, 3, 0); /* �ӳٵȴ�ģ�������ɹ� */ 	
			pst_lora_operation->CurCMD = "+++";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_MOD);	
		}
		/* ���ù���ģʽ */
		else if(EN_LORAOPEN_MOD == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+MOD=0\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_CLA);
		}
		/* �����豸C�� */
		else if(EN_LORAOPEN_CLA == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+CLA=C\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_JON);
		}
		/* �������� */
		else if(EN_LORAOPEN_JON == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+JON\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_RJN);
		}
		/* ������������ */
		else if(EN_LORAOPEN_RJN == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+RJN\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			OSTimeDlyHMSM(0, 0, 3, 0); 						/* �ӳٵȴ�ģ�������ɹ� */ 	
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_END);
		}
		/* ����״̬ */
		else if(EN_LORAOPEN_END == pst_lora_operation->CurOperateState)
		{
			debug_printf("EN_LORAOPEN_END is END!!!\r\n");
			SetLORA_OperatStrState(pst_lora_operation, EN_LORAPROCESS_HANDLE, EN_LORAOPEN_IDLE);
			SetLORA_ProceStrState(pst_lora_operation, EN_LORAPROCESS_HANDLE, EN_LORAPROCESS_IDLE);
			pst_lora_operation->LORATick_Flag = ENABLE_LORA_Tick;		/* ʹ���������� */
		}
		/* ����״̬ */
		else if(EN_LORAOPEN_IDLE == pst_lora_operation->CurOperateState)
		{
			/* ����״̬ѭ����LORAģ��Ļ�Ӧ */
			LORA_MODULE_WAITRX(pst_lora_operation);
			return 0;
		}
		/* ���еȴ�����LORAģ���Ӧ */
		else if(EN_LORAOPEN_ERROR == pst_lora_operation->CurOperateState)
		{
			/* TODO������״̬�·��ʹ����뵽���� */
			return 0;
		}
	}
    return 0;
}

/**
  * @brief LORAģ�鴦��
 */
static u8 LORAIOT_MODULE_HANDLE(PST_LORA_OPERATION pst_lora_operation)
{
	char Tempbuf[300] = {0};						/* �������buf */
	if(LORAUartRecvData((u8 *)Tempbuf) > 0)			/* һֱ���ƽ̨���� */
	{
		/* �жϽ��յ��Ƿ������� */
		if(strstr(Tempbuf, "+RCV") != NULL)
		{
			/* Test:�������յ����ݲ���ʾ */
			debug_printf("LocalRecvMessage:%s\r\n", (char *)Tempbuf);
			AnalysisLoRaMsg(&LORADataPacket, Tempbuf, &LORATxdaPacket);				/* �������յ�COAP���� */
		}	
	}
	return 0;
}


/**
  * @brief LORA�����ʱ���ص�������
 */
void LORASystickTmr_callback(OS_TMR *ptmr, void *p_arg)
{
	if(ENABLE_LORA_Tick == st_lora_operation.LORATick_Flag)		/* LORA�������� */
	{
		LORA_send_cmd("AT+TXH=10,5A271008FF360000A3CA\r\n", 0);/* �ڵ���������ֱ�ӷ��� */
	}
}

/**
  * @brief LORA����״̬����
 */
static u8 LORA_MODULE_PROCESS(PST_LORA_OPERATION pst_lora_operation)
{
	if(pst_lora_operation != NULL)
	{
		if(EN_LORAPROCESS_IDLE == pst_lora_operation->CurProcState)			/* ģ����� */
		{
			/* TODO SOMETHINGS */
		}
		else if(EN_LORAPROCESS_OPEN == pst_lora_operation->CurProcState)	/* LORAģ������� */
		{	
		  	LORA_MODULE_OPEN(pst_lora_operation);
		}
		else if(EN_LORAPROCESS_HANDLE == pst_lora_operation->CurProcState)	/* LORAģ�鴦������ */
		{
			LORAIOT_MODULE_HANDLE(pst_lora_operation);							/* TODO SOMETHINGS */
		}
	}
    return 0;
}

/**
  * @brief  LORA����
 */
void App_LORA_TASK(void)
{
	u8 err;
	OS_TMR  *LORASystickTmr; 												/* �����ʱ�� */ 
    LORASystickTmr = OSTmrCreate(0, 300, OS_TMR_OPT_PERIODIC, 
								(OS_TMR_CALLBACK)LORASystickTmr_callback, 	/* ��ʱ���ص����� */
								0, "LORASystickTmr", &err);					/* 500msִ��һ�� */
    OSTmrStart(LORASystickTmr, &err);										/* ���������ʱ�� */
	LORA_MODULE_INIT(&st_lora_operation);									/* NB_IOTģ���ʼ�� */
	debug_printf("[%s] enter.\r\n", __func__ );
	while(1)
	{
		LORA_MODULE_PROCESS(&st_lora_operation);							/* NB_IOTģ�鴦�� */
		OSTimeDlyHMSM(0, 0, 0, 1);  										/* ������ʱ�����ͷ�CPU����Ȩ */  
	}
}

/**
  * @brief ��LoRa��������
 */
u8 LORA_send_cmd(char* ucCMD, u16 waittime)
{
	//LoRaDMA_Send_Data(ucCMD);
	Module_SendMessage(strlen(ucCMD), (char *)ucCMD);
	OSTimeDlyHMSM(0, 0, 0, waittime);
	
    return 0;
}

/**
  * @brief LORAģ��ȴ�������Ϣ
 */
static u8 LORA_MODULE_WAITRX(PST_LORA_OPERATION pst_lora_operation)
{
	static u32 TimeCount = 0;
	char lpbuf[300] = {0};			/* ��ʱ���յ����ݴ��λ�� */
	if(++ TimeCount <= 60000)		/* ����LORAģ��ٷ�������������30s��ʱʱ�� */
	{
		if(LORAUartRecvData((u8 *)lpbuf) > 0)
		{
			debug_printf("RecvDebug:%s\r\n", (char *)lpbuf);/* ��ӡ���յ�����Ϣ */
			/* �жϽ��յ����������Ƿ����������յ������� */
			if(strstr(lpbuf, (char *)pst_lora_operation->ExpectRcv) != NULL)
			{
				pst_lora_operation->CurOperateState = pst_lora_operation->NextOperateState;
				pst_lora_operation->ExecuCount = 0;										/* ÿ������ִ�гɹ���������ִ�д������� */
				debug_printf("%s:success\r\n", pst_lora_operation->CurCMD);
				debug_printf("Enter The Next Operate State\r\n");
				TimeCount = 0;
				return LORA_RESPONSE_SUCCESS;
			}
				
			/* ������յ�������Ϣ */
			else if(strstr(lpbuf, "ERROR") != NULL)
			{
				pst_lora_operation->CurOperateState = pst_lora_operation->LastOperateState;
				debug_printf("!!!Attempt excute:%s -->count=%d!!!\r\n", pst_lora_operation->CurCMD, pst_lora_operation->ExecuCount);	/* debug */
				if(++pst_lora_operation->ExecuCount >= 3)
				{
					pst_lora_operation->ExecuCount = 0;										/* ���Դ������� */
					pst_lora_operation->CurOperateState = EN_LORAOPEN_ERROR;
					TimeCount = 0;
					debug_printf("%s:No Response!!!\r\n", pst_lora_operation->CurCMD);		/* debug */
					return LORA_ERROR_NORESPONSE; 											/* ���Դ�����������Ӧ */
				}
			}
		}
	}
	else		/* ����ڹ涨ʱ����û���յ����� */
	{
		TimeCount = 0;
		pst_lora_operation->CurOperateState = pst_lora_operation->LastOperateState;			/* ��������ִ��һ�� */
		debug_printf("!!!Attempt excute:%s -->count=%d!!!\r\n", pst_lora_operation->CurCMD, pst_lora_operation->ExecuCount);	/* debug */
		if(++ pst_lora_operation->ExecuCount >= 4)											
		{
			pst_lora_operation->CurOperateState = EN_LORAOPEN_ERROR;
			pst_lora_operation->ExecuCount = 0;			/* ���Դ������� */
			TimeCount = 0;
			debug_printf("%s:Timeout!!!\r\n", pst_lora_operation->CurCMD);	/* debug */
			return LORA_ERROR_TIMEOUT; 					/* ���س�ʱ */
		}
	}
	return -1;
}


/**
  * @brief ����ƽ̨���͵����ݲ���ʾ
 */
static void AnalysisLoRaMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet)
{
	char* TempMsg = NULL;
	u32 LoRaDataLen = 0;
	if(MSG != NULL)
	{
		TempMsg = strchr(MSG, ',');							/* ��֮��ĵ�ַ��ֵ��TempMsg */
		TempMsg ++;
	}
	LoRaDataLen = LORA_Datalen(TempMsg);
	debug_printf("LoRadataLen = %d\n", LoRaDataLen);		/* ���Դ�ӡ���� */
	if(LoRaDataLen == 0)									/* ������յ����ݳ����ǵ���0 */
	{
		debug_printf("CS Server Send Data is not HEX!!!!!!\r\n");
	}
	debug_printf("TempMsg = %s\n", TempMsg);				/* ���Դ�ӡ���� */
	/* Debug_Test */
	for(int i = 0; i < LoRaDataLen; i ++) 
	{
		debug_printf("MSG[%d] = %x\r\n", i, TempMsg[i]);		
	}
	if(TempMsg)
	{
		SplitLORAData(p_device_packet, (u8 *)TempMsg, LoRaDataLen, ptx_device_packet);	/* ������ݵ����ݰ� */
		if(p_device_packet != NULL)														
		{
			debug_printf("LORA Pack Post\r\n");
			DevicePacket_post(p_device_packet);											/* �����豸�������ݵ��豸�������� */
		}																			
	}
}

/**
  * @brief �������
 */
void GropPackLORAMsgSend(u8* Data, u32 DataLen)
{
	
	LORATxdaPacket.data_size = DataLen;					/* ��ֵ���� */
	//LORATxdaPacket.data = Data;
	memcpy(LORATxdaPacket.data, Data, DataLen);
	SendLORAMsgPack(&LORATxdaPacket); 					/* ���ݰ����� */
}

/**
  * @brief �������ݵ�ƽ̨
 */
void SendLORAMsgPack(P_Device_packet_t p_device_packet)
{
	u8 sendbuf[32] = {0};				/* LORA Data���ͻ����� */
	u8 TxNBiotLen = 0;					/* �ܵ�NB��Ϣ���� */
	char NBIOTSendBuf[64] = {0};		/* �ܵ�NB������Ϣ������--AT+NMGS=... */
	char NBDataBuf[255] = {0};			/* LORA�������ݻ�����--���淢�������� */
	memset(NBIOTSendBuf, 0, 64);
	memset(NBDataBuf, 0, 255);
	if(p_device_packet != NULL)			/* �����ֵ */
	{	
		sendbuf[0] = p_device_packet->Starting_mark;
		sendbuf[1] = (p_device_packet->Message_number & 0xFF00) >> 8;
		sendbuf[2] = p_device_packet->Message_number & 0x00FF;
		sendbuf[3] = p_device_packet->Node_type;
		sendbuf[4] = p_device_packet->Device_type;
		sendbuf[5] = p_device_packet->Command_code;
		sendbuf[6] = p_device_packet->ack;
		sendbuf[7] = p_device_packet->data_size;
		memcpy(&sendbuf[8], p_device_packet->data, p_device_packet->data_size);
		sendbuf[8 + p_device_packet->data_size] = p_device_packet->Check_code;
		sendbuf[9 + p_device_packet->data_size] = p_device_packet->End_mark;
		TxNBiotLen = p_device_packet->data_size + 10;
		HexToAscii(sendbuf, (u8 *)NBDataBuf, TxNBiotLen);
		sprintf(NBIOTSendBuf,"AT+TXH=32,%s\r\n", NBDataBuf);				/* Ĭ�϶˿�32 */	
		debug_printf("Back Show!!!:%s", NBIOTSendBuf);						/* debug */
		OSTimeDlyHMSM(0, 0, 0, 1);											/* �ӳ�5ms */
		LORA_send_cmd(NBIOTSendBuf, 0);									/* �������ݷ��� */		
	}
}

/**
  * @brief ���ƽ̨����
 */
static void SplitLORAData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet)
{
	p_device_packet->Starting_mark 	= 	ReceiveBuf[0];			/* ��ʼ��־ */
	p_device_packet->Message_number =	ReceiveBuf[1] << 8 | ReceiveBuf[2];	/* ��Ϣ���к�ռ�����ֽ� */
	p_device_packet->Node_type 		= 	ReceiveBuf[3];
	p_device_packet->Device_type 	= 	ReceiveBuf[4];
	p_device_packet->Command_code 	= 	ReceiveBuf[5];
	p_device_packet->ack 			=	ReceiveBuf[6];
	p_device_packet->data_size 		= 	ReceiveBuf[7];
	//p_device_packet->data 			=	(u8*)&ReceiveBuf[8];
	memcpy(p_device_packet->data, &ReceiveBuf[8], p_device_packet->data_size);
	p_device_packet->Check_code 	= 	ReceiveBuf[DataLen - 2];
	p_device_packet->End_mark 		=	ReceiveBuf[DataLen - 1];					/* Э���н�����־��0xCA */

	/* ���͹̶����ݸ�ֵ */
	ptx_device_packet->Starting_mark 	= p_device_packet->Starting_mark;
	ptx_device_packet->Message_number 	= p_device_packet->Message_number;
	ptx_device_packet->Node_type 		= p_device_packet->Node_type;
	ptx_device_packet->Device_type 		= p_device_packet->Device_type;
	ptx_device_packet->Command_code 	= p_device_packet->Command_code;
	ptx_device_packet->ack 				= p_device_packet->ack;
	ptx_device_packet->Check_code 		= p_device_packet->Check_code ;
	ptx_device_packet->End_mark 		= p_device_packet->End_mark;
	
	for(int i = 0; i < p_device_packet->data_size; i ++)
	{
		debug_printf("%02X ", p_device_packet->data[i]);
	}
	debug_printf("\r\n");
	for(int i = 0; i < 16; i ++)						/* Э��涨д��16�ֽ����� */		
	{			
		debug_printf("packet->data[%d] = %02X\r\n", 12 + i, p_device_packet->data[12 + i]);
	}
	debug_printf("\r\n");
	/*  ���Խ��յ������� */
	//GropPackLORAMsgSend(p_device_packet->data, p_device_packet->data_size);	/* debug--����LORA���ݲ��� */
}
