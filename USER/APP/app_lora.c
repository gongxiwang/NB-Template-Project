/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_nbiot.c
  * @author  walle
  * @version  V2.0.0
  * @date  08/05/2019
  * @brief  用户app_nbiot
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


#define LORA_CMDEXECUTE_COUNT 	0x03                       	/* BC-35命令执行次数 */

#define ENABLE_LORA_Tick		0x01						/* 使能NBIOT固定时间发送 */
#define DISABLE_LORA_Tick   	0x00


/* LORA操作结构体 */
static ST_LORA_OPERAION st_lora_operation = {0};
static Device_packet_t LORADataPacket = {0};				/* LORA */
static Device_packet_t LORATxdaPacket = {0};

/**
  * @brief LORA模块设置结构体执行操作状态
 */
static void SetLORA_OperatStrState(PST_LORA_OPERATION pst_lora_operation, u8 curState, u8 nextState)
{
	pst_lora_operation->LastOperateState = pst_lora_operation->CurOperateState;
	pst_lora_operation->CurOperateState = curState;
	pst_lora_operation->NextOperateState = nextState;
	debug_printf("%s:SEND\r\n", pst_lora_operation->CurCMD);
}

/**
  * @brief LORA模块设置结构体执行处理状态
 */
static void SetLORA_ProceStrState(PST_LORA_OPERATION pst_lora_operation, u8 curState, u8 nextState)
{
	pst_lora_operation->CurProcState =  curState;
	pst_lora_operation->NextProcState = nextState;
}

/**
  * @brief LORA操作初始化
 */
static u8 LORA_MODULE_INIT(PST_LORA_OPERATION pst_lora_operation)
{
	pst_lora_operation->CurProcState = EN_LORAPROCESS_OPEN;		/* 打开状态 */
	pst_lora_operation->CurOperateState = EN_LORAOPEN_ADD;		/* 首先打开loraAT测试命令 */
	pst_lora_operation->WAIT_TIME = 1000;						/* 延迟时间 */
	pst_lora_operation->ExecuCount = 1;
    return 0;
}

/**
  * @brief LORA操作打开
 */
static u8 LORA_MODULE_OPEN(PST_LORA_OPERATION pst_lora_operation)
{
	if(pst_lora_operation != NULL)
	{
		/* AT命令检测 */
		if(EN_LORAOPEN_ADD == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "+++";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd(pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_REBOOT);
		}
		/* LORA模块重启 */
		else if(EN_LORAOPEN_REBOOT == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+SRS\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_CHECKREBOOT);	
		}
		/* LORA模块检测重启是否成功 */
		else if(EN_LORAOPEN_CHECKREBOOT == pst_lora_operation->CurOperateState)
		{
			OSTimeDlyHMSM(0, 0, 3, 0); /* 延迟等待模块重启成功 */ 	
			pst_lora_operation->CurCMD = "+++";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_MOD);	
		}
		/* 设置工作模式 */
		else if(EN_LORAOPEN_MOD == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+MOD=0\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_CLA);
		}
		/* 设置设备C类 */
		else if(EN_LORAOPEN_CLA == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+CLA=C\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_JON);
		}
		/* 启动加网 */
		else if(EN_LORAOPEN_JON == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+JON\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_RJN);
		}
		/* 重新启动加网 */
		else if(EN_LORAOPEN_RJN == pst_lora_operation->CurOperateState)
		{
			pst_lora_operation->CurCMD = "AT+RJN\r\n";
			pst_lora_operation->ExpectRcv = "OK";
			OSTimeDlyHMSM(0, 0, 3, 0); 						/* 延迟等待模块重启成功 */ 	
			LORA_send_cmd((char *)pst_lora_operation->CurCMD, pst_lora_operation->WAIT_TIME);
			SetLORA_OperatStrState(pst_lora_operation, EN_NBOPEN_IDLE, EN_LORAOPEN_END);
		}
		/* 结束状态 */
		else if(EN_LORAOPEN_END == pst_lora_operation->CurOperateState)
		{
			debug_printf("EN_LORAOPEN_END is END!!!\r\n");
			SetLORA_OperatStrState(pst_lora_operation, EN_LORAPROCESS_HANDLE, EN_LORAOPEN_IDLE);
			SetLORA_ProceStrState(pst_lora_operation, EN_LORAPROCESS_HANDLE, EN_LORAPROCESS_IDLE);
			pst_lora_operation->LORATick_Flag = ENABLE_LORA_Tick;		/* 使能心跳发送 */
		}
		/* 空闲状态 */
		else if(EN_LORAOPEN_IDLE == pst_lora_operation->CurOperateState)
		{
			/* 跳出状态循环，LORA模块的回应 */
			LORA_MODULE_WAITRX(pst_lora_operation);
			return 0;
		}
		/* 空闲等待接收LORA模块回应 */
		else if(EN_LORAOPEN_ERROR == pst_lora_operation->CurOperateState)
		{
			/* TODO：错误状态下发送错误码到串口 */
			return 0;
		}
	}
    return 0;
}

/**
  * @brief LORA模块处理
 */
static u8 LORAIOT_MODULE_HANDLE(PST_LORA_OPERATION pst_lora_operation)
{
	char Tempbuf[300] = {0};						/* 缓冲接收buf */
	if(LORAUartRecvData((u8 *)Tempbuf) > 0)			/* 一直检查平台接收 */
	{
		/* 判断接收的是否是数据 */
		if(strstr(Tempbuf, "+RCV") != NULL)
		{
			/* Test:分析接收的数据并显示 */
			debug_printf("LocalRecvMessage:%s\r\n", (char *)Tempbuf);
			AnalysisLoRaMsg(&LORADataPacket, Tempbuf, &LORATxdaPacket);				/* 分析接收的COAP数据 */
		}	
	}
	return 0;
}


/**
  * @brief LORA软件定时发回调函数送
 */
void LORASystickTmr_callback(OS_TMR *ptmr, void *p_arg)
{
	if(ENABLE_LORA_Tick == st_lora_operation.LORATick_Flag)		/* LORA心跳发送 */
	{
		LORA_send_cmd("AT+TXH=10,5A271008FF360000A3CA\r\n", 0);/* 节点心跳命令直接发送 */
	}
}

/**
  * @brief LORA处理状态函数
 */
static u8 LORA_MODULE_PROCESS(PST_LORA_OPERATION pst_lora_operation)
{
	if(pst_lora_operation != NULL)
	{
		if(EN_LORAPROCESS_IDLE == pst_lora_operation->CurProcState)			/* 模块空闲 */
		{
			/* TODO SOMETHINGS */
		}
		else if(EN_LORAPROCESS_OPEN == pst_lora_operation->CurProcState)	/* LORA模块打开流程 */
		{	
		  	LORA_MODULE_OPEN(pst_lora_operation);
		}
		else if(EN_LORAPROCESS_HANDLE == pst_lora_operation->CurProcState)	/* LORA模块处理流程 */
		{
			LORAIOT_MODULE_HANDLE(pst_lora_operation);							/* TODO SOMETHINGS */
		}
	}
    return 0;
}

/**
  * @brief  LORA任务
 */
void App_LORA_TASK(void)
{
	u8 err;
	OS_TMR  *LORASystickTmr; 												/* 软件定时器 */ 
    LORASystickTmr = OSTmrCreate(0, 300, OS_TMR_OPT_PERIODIC, 
								(OS_TMR_CALLBACK)LORASystickTmr_callback, 	/* 定时器回调函数 */
								0, "LORASystickTmr", &err);					/* 500ms执行一次 */
    OSTmrStart(LORASystickTmr, &err);										/* 启动软件定时器 */
	LORA_MODULE_INIT(&st_lora_operation);									/* NB_IOT模块初始化 */
	debug_printf("[%s] enter.\r\n", __func__ );
	while(1)
	{
		LORA_MODULE_PROCESS(&st_lora_operation);							/* NB_IOT模块处理 */
		OSTimeDlyHMSM(0, 0, 0, 1);  										/* 调用延时函数释放CPU控制权 */  
	}
}

/**
  * @brief 向LoRa发送命令
 */
u8 LORA_send_cmd(char* ucCMD, u16 waittime)
{
	//LoRaDMA_Send_Data(ucCMD);
	Module_SendMessage(strlen(ucCMD), (char *)ucCMD);
	OSTimeDlyHMSM(0, 0, 0, waittime);
	
    return 0;
}

/**
  * @brief LORA模块等待接收消息
 */
static u8 LORA_MODULE_WAITRX(PST_LORA_OPERATION pst_lora_operation)
{
	static u32 TimeCount = 0;
	char lpbuf[300] = {0};			/* 临时接收的数据存放位置 */
	if(++ TimeCount <= 60000)		/* 这里LORA模块官方给出的数据是30s超时时间 */
	{
		if(LORAUartRecvData((u8 *)lpbuf) > 0)
		{
			debug_printf("RecvDebug:%s\r\n", (char *)lpbuf);/* 打印接收到的消息 */
			/* 判断接收到的数据中是否有期望接收到的数据 */
			if(strstr(lpbuf, (char *)pst_lora_operation->ExpectRcv) != NULL)
			{
				pst_lora_operation->CurOperateState = pst_lora_operation->NextOperateState;
				pst_lora_operation->ExecuCount = 0;										/* 每次命令执行成功，则命令执行次数清零 */
				debug_printf("%s:success\r\n", pst_lora_operation->CurCMD);
				debug_printf("Enter The Next Operate State\r\n");
				TimeCount = 0;
				return LORA_RESPONSE_SUCCESS;
			}
				
			/* 如果接收到错误消息 */
			else if(strstr(lpbuf, "ERROR") != NULL)
			{
				pst_lora_operation->CurOperateState = pst_lora_operation->LastOperateState;
				debug_printf("!!!Attempt excute:%s -->count=%d!!!\r\n", pst_lora_operation->CurCMD, pst_lora_operation->ExecuCount);	/* debug */
				if(++pst_lora_operation->ExecuCount >= 3)
				{
					pst_lora_operation->ExecuCount = 0;										/* 尝试次数清零 */
					pst_lora_operation->CurOperateState = EN_LORAOPEN_ERROR;
					TimeCount = 0;
					debug_printf("%s:No Response!!!\r\n", pst_lora_operation->CurCMD);		/* debug */
					return LORA_ERROR_NORESPONSE; 											/* 尝试次数过多无响应 */
				}
			}
		}
	}
	else		/* 如果在规定时间内没有收到数据 */
	{
		TimeCount = 0;
		pst_lora_operation->CurOperateState = pst_lora_operation->LastOperateState;			/* 尝试重新执行一次 */
		debug_printf("!!!Attempt excute:%s -->count=%d!!!\r\n", pst_lora_operation->CurCMD, pst_lora_operation->ExecuCount);	/* debug */
		if(++ pst_lora_operation->ExecuCount >= 4)											
		{
			pst_lora_operation->CurOperateState = EN_LORAOPEN_ERROR;
			pst_lora_operation->ExecuCount = 0;			/* 尝试次数清零 */
			TimeCount = 0;
			debug_printf("%s:Timeout!!!\r\n", pst_lora_operation->CurCMD);	/* debug */
			return LORA_ERROR_TIMEOUT; 					/* 返回超时 */
		}
	}
	return -1;
}


/**
  * @brief 分析平台发送的数据并显示
 */
static void AnalysisLoRaMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet)
{
	char* TempMsg = NULL;
	u32 LoRaDataLen = 0;
	if(MSG != NULL)
	{
		TempMsg = strchr(MSG, ',');							/* 把之后的地址赋值给TempMsg */
		TempMsg ++;
	}
	LoRaDataLen = LORA_Datalen(TempMsg);
	debug_printf("LoRadataLen = %d\n", LoRaDataLen);		/* 测试打印长度 */
	if(LoRaDataLen == 0)									/* 如果接收的数据长度是等于0 */
	{
		debug_printf("CS Server Send Data is not HEX!!!!!!\r\n");
	}
	debug_printf("TempMsg = %s\n", TempMsg);				/* 测试打印长度 */
	/* Debug_Test */
	for(int i = 0; i < LoRaDataLen; i ++) 
	{
		debug_printf("MSG[%d] = %x\r\n", i, TempMsg[i]);		
	}
	if(TempMsg)
	{
		SplitLORAData(p_device_packet, (u8 *)TempMsg, LoRaDataLen, ptx_device_packet);	/* 拆分数据到数据包 */
		if(p_device_packet != NULL)														
		{
			debug_printf("LORA Pack Post\r\n");
			DevicePacket_post(p_device_packet);											/* 发送设备控制数据到设备管理任务 */
		}																			
	}
}

/**
  * @brief 数据组包
 */
void GropPackLORAMsgSend(u8* Data, u32 DataLen)
{
	
	LORATxdaPacket.data_size = DataLen;					/* 赋值长度 */
	//LORATxdaPacket.data = Data;
	memcpy(LORATxdaPacket.data, Data, DataLen);
	SendLORAMsgPack(&LORATxdaPacket); 					/* 数据包发送 */
}

/**
  * @brief 发送数据到平台
 */
void SendLORAMsgPack(P_Device_packet_t p_device_packet)
{
	u8 sendbuf[32] = {0};				/* LORA Data发送缓冲区 */
	u8 TxNBiotLen = 0;					/* 总的NB消息长度 */
	char NBIOTSendBuf[64] = {0};		/* 总的NB发送消息缓冲区--AT+NMGS=... */
	char NBDataBuf[255] = {0};			/* LORA发送数据缓冲区--保存发送数据用 */
	memset(NBIOTSendBuf, 0, 64);
	memset(NBDataBuf, 0, 255);
	if(p_device_packet != NULL)			/* 打包赋值 */
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
		sprintf(NBIOTSendBuf,"AT+TXH=32,%s\r\n", NBDataBuf);				/* 默认端口32 */	
		debug_printf("Back Show!!!:%s", NBIOTSendBuf);						/* debug */
		OSTimeDlyHMSM(0, 0, 0, 1);											/* 延迟5ms */
		LORA_send_cmd(NBIOTSendBuf, 0);									/* 启动数据发送 */		
	}
}

/**
  * @brief 解包平台数据
 */
static void SplitLORAData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet)
{
	p_device_packet->Starting_mark 	= 	ReceiveBuf[0];			/* 起始标志 */
	p_device_packet->Message_number =	ReceiveBuf[1] << 8 | ReceiveBuf[2];	/* 消息序列号占两个字节 */
	p_device_packet->Node_type 		= 	ReceiveBuf[3];
	p_device_packet->Device_type 	= 	ReceiveBuf[4];
	p_device_packet->Command_code 	= 	ReceiveBuf[5];
	p_device_packet->ack 			=	ReceiveBuf[6];
	p_device_packet->data_size 		= 	ReceiveBuf[7];
	//p_device_packet->data 			=	(u8*)&ReceiveBuf[8];
	memcpy(p_device_packet->data, &ReceiveBuf[8], p_device_packet->data_size);
	p_device_packet->Check_code 	= 	ReceiveBuf[DataLen - 2];
	p_device_packet->End_mark 		=	ReceiveBuf[DataLen - 1];					/* 协议中结束标志是0xCA */

	/* 发送固定数据赋值 */
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
	for(int i = 0; i < 16; i ++)						/* 协议规定写入16字节数据 */		
	{			
		debug_printf("packet->data[%d] = %02X\r\n", 12 + i, p_device_packet->data[12 + i]);
	}
	debug_printf("\r\n");
	/*  测试接收到的数据 */
	//GropPackLORAMsgSend(p_device_packet->data, p_device_packet->data_size);	/* debug--回显LORA数据测试 */
}
