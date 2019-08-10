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

#define SENDCOAPIP        "AT+NCDP=49.4.85.232,5683\r\n"   	/* 华为-oceanconnect-COAP连接地址 */
#define NBIOT_CMDEXECUTE_COUNT 	0x03                       	/* BC-35命令执行次数 */

#define ENABLE_NBIOT_Tick		0x01						/* 使能NBIOT固定时间发送 */
#define DISABLE_NBIOT_Tick   	0x00

static Device_packet_t NBiotDataPacket = {0};				/* NBIOT数据包 */
static Device_packet_t NBiotTxdaPacket = {0};
u8 commandAT[255];     // 存放指令，包含ip地址和端口和长度
u8 HTTPTCPData[255];   //存放发送信息就是最后发送的命令里面的数据



/* AT Command Description Implementation Status */
  
/* 3GPP Commands (27.007)*/
//------------|-----------------------------------------------------| 
// AT+CGMI    | Request Manufacturer Identification B350 or later   
//------------|-----------------------------------------------------|
// AT+CGMM    | Request Manufacturer Model B350 or later            
//------------|-----------------------------------------------------|
// AT+CGMR    | Request Manufacturer Revision B350 or later         
//------------|-----------------------------------------------------|
// AT+CGSN    | Request Product Serial Number B350 or later         
//------------|-----------------------------------------------------|
// AT+CEREG   | EPS Network Registration Status B350 or later       
//------------|-----------------------------------------------------|
// AT+CSCON   | Signalling Connection Status B350 or later
//------------|-----------------------------------------------------|
// AT+CLAC    | List Available Commands B350 or later
//------------|-----------------------------------------------------|
// AT+CSQ     | Get Signal Strength Indicator B350 or later
//------------|-----------------------------------------------------|
// AT+CGPADDR | Show PDP Addresses B350 or later
//------------|-----------------------------------------------------|
// AT+COPS    | PLMN Selection B350 or later
//------------|-----------------------------------------------------|
// AT+CGATT   | PS Attach or Detach B350 or later
//------------|-----------------------------------------------------|
// AT+CGACT   | Activate or Deactivate PDP Context B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CIMI    | Request International Mobile Subscriber Identity B350 or later
//------------|-----------------------------------------------------|
// AT+CGDCONT | Define a PDP Context B350 or later
//------------|-----------------------------------------------------|
// AT+CFUN    | Set Phone Functionality B350 or later
//------------|-----------------------------------------------------|
// AT+CMEE    | Report Mobile Termination Error B600 or later
//------------|-----------------------------------------------------|
// AT+CCLK    | Return Current Date & Time B656 or later
//------------|-----------------------------------------------------|
// AT+CPSMS   | Power Saving Mode Setting B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CEDRXS  | eDRX Setting B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CEER    | Extended Error Report B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CEDRXRDP| eDRX Read Dynamic Parameters B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CTZR    | Time Zone Reporting B657SP1 or later
//------------|-----------------------------------------------------|

/*   ETSI Commands* (127.005)  <Under development> */
//------------|-----------------------------------------------------|
// AT+CSMS    | Select Message Service B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CNMA    | New Message Acknowledgement to ME/TA B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CSCA    | Service Centre Address B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CMGS    | Send SMS Message B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CMGC    | Send SMS Command B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CSODCP  | Sending of Originating Data via the Control Plane B657SP1 or later
//------------|-----------------------------------------------------|
// AT+CRTDCP  | Reporting of Terminating Data via the Control Plane
//------------|-----------------------------------------------------|

/*   General Commands  */
//--------------|-----------------------------------------------------|
// AT+NMGS      | Send a Message B350 or later
//--------------|-----------------------------------------------------|
// AT+NMGR      | Get a Message B350 or later
//--------------|-----------------------------------------------------|
// AT+NNMI      | New Message Indications B350 or later
//--------------|-----------------------------------------------------|
// AT+NSMI      | Sent message Indications B350 or later
//--------------|-----------------------------------------------------|
// AT+NQMGR     | Query Messages Received B350 or later
//--------------|-----------------------------------------------------|
// AT+NQMGS     | Query Messages Sent B350 or later
//--------------|-----------------------------------------------------|
// AT+NMSTATUS  | Message Registration Status B657SP1 or later
//--------------|-----------------------------------------------------|
// AT+NRB       | Reboot B350 or later
//--------------|-----------------------------------------------------|
// AT+NCDP      | Configure and Query CDP Server Settings B350 or later
//--------------|-----------------------------------------------------|
// AT+NUESTATS  | Query UE Statistics B350 or later
//--------------|-----------------------------------------------------|
// AT+NEARFCN   | Specify Search Frequencies B350 or later
//--------------|-----------------------------------------------------|
// AT+NSOCR     | Create a Socket B350 or later
//--------------|-----------------------------------------------------|
// AT+NSOST     | SendTo Command (UDP Only) B350 or later
//--------------|-----------------------------------------------------|
// AT+NSOSTF    | SendTo Command with Flags (UDP Only) B656 or later
//--------------|-----------------------------------------------------|
// AT+NSORF     | Receive Command (UDP only) B350 or later
//--------------|-----------------------------------------------------|
// AT+NSOCL     | Close a Socket B350 or later
//--------------|-----------------------------------------------------|
// +NSONMI      | Socket Message Arrived Indicator (Response Only) B350 or later
//--------------|-----------------------------------------------------|
// AT+NPING     | Test IP Network Connectivity to a Remote Host B350 or later
//--------------|-----------------------------------------------------|
// AT+NBAND     | Set Supported Bands B600 or later
//--------------|-----------------------------------------------------|
// AT+NLOGLEVEL | Set Debug Logging Level B600 or later
//--------------|-----------------------------------------------------|
// AT+NCONFIG   | Configure UE Behaviour B650 or later
//--------------|-----------------------------------------------------|
// AT+NATSPEED  | Configure UART Port Baud Rate B656 or later
//--------------|-----------------------------------------------------|
// AT+NCCID     | Card Identification B657SP1 or later
//--------------|-----------------------------------------------------|
// AT+NFWUPD    | Firmware Update via UART B657SP1 or later
//--------------|-----------------------------------------------------|
// AT+NRDCTRL   | Control Radio Configurations B657SP1 or later
//--------------|-----------------------------------------------------|
// AT+NCHIPINFO | Read System Information B657SP1 or later
//--------------|-----------------------------------------------------|
  
/*  Temporary Commands     */
//--------------|-----------------------------------------------------|
// AT+NTSETID   | Set ID B350 or later
//--------------|-----------------------------------------------------|

/* NB模块操作结构体 */
static ST_NB_OPERAION st_nb_operation = {0};

/**
  * @brief  NBIOT任务
 */
void App_NBIOT_TASK(void)
{
	u8 err;
	OS_TMR  *NBSystickTmr; 												/* 软件定时器 */ 
    NBSystickTmr = OSTmrCreate(0, 500, OS_TMR_OPT_PERIODIC, 
								(OS_TMR_CALLBACK)NBSystickTmr_callback, /* 定时器回调函数 */
								0, "NBSystickTmr", &err);				/* 500ms执行一次 */
    OSTmrStart(NBSystickTmr, &err);										/* 启动软件定时器 */
	NBIOT_MODULE_INIT(&st_nb_operation);								/* NB_IOT模块初始化 */
	debug_printf("[%s] enter.\r\n", __func__ );
	while(1)
	{
		//NBIOT_MODULE_PROCESS(&st_nb_operation);							/* NB_IOT模块处理 */
		//OSTimeDlyHMSM(0, 0, 0, 1);  									/* 调用延时函数释放CPU控制权 */ 
		
		NBIOT_send_cmd("+++", 0);
		OSTimeDlyHMSM(0, 0, 5, 0);  		
	}
}


/**
  * @brief 向NB发送命令
 */
u8 NBIOT_send_cmd(char* ucCMD, u16 waittime)
{
	NBiotDMA_Send_Data((char *)ucCMD);
	OSTimeDlyHMSM(0, 0, 0, waittime);
    return 0;
}

/**
  * @brief NB处理状态函数
 */
u8 NBIOT_MODULE_PROCESS(PST_NB_OPERATION pst_nb_operation)
{
	if(pst_nb_operation != NULL)
	{
		if(EN_NBPROCESS_IDLE == pst_nb_operation->CurProcState)			/* 模块空闲 */
		{
			/* TODO SOMETHINGS */
		}
		else if(EN_NBPROCESS_OPEN == pst_nb_operation->CurProcState)	/* NB_IOT模块打开流程 */
		{
			NBIOT_MODULE_OPEN(pst_nb_operation);
		}
		else if(EN_NBPROCESS_HANDLE == pst_nb_operation->CurProcState)	/* NB_IOT模块处理流程 */
		{
			NBIOT_MODULE_HANDLE(pst_nb_operation);						/* TODO SOMETHINGS */
		}
	}
    return 0;
}

/**
  * @brief NB操作初始化
 */
static u8 NBIOT_MODULE_INIT(PST_NB_OPERATION pst_nb_operation)
{
	pst_nb_operation->CurProcState = EN_NBPROCESS_OPEN;		/* 打开状态 */
	pst_nb_operation->CurOperateState = EN_NBOPEN_NCFG;		/* 首先重启模块 */
	pst_nb_operation->WAIT_TIME = 1000;						/* 延迟时间 */
    return 0;
}

/**
  * @brief NB操作打开
 */
u8 NBIOT_MODULE_OPEN(PST_NB_OPERATION pst_nb_operation)
{
	if(pst_nb_operation != NULL)
	{
		/* 设置手动入网 */
		if(EN_NBOPEN_NCFG == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+NCONFIG=AUTOCONNECT,FALSE\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CFGIP);
		}
		/* 配置华为OC平台IP地址 */
		else if(EN_NBOPEN_CFGIP == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = SENDCOAPIP;
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_REBOOT);	
		}
		/* 重启模块 */
		else if(EN_NBOPEN_REBOOT == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+NRB\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CFUN1);
		}
#if 0
		/* AT测试指令 */
		else if(EN_NBOPEN_AT == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CMEE);
		}
		/* 禁用报告错误使用ERROR代替 */
		else if(EN_NBOPEN_CMEE == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CMEE=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CGSN);	
		}
		/* 获取设备IMEI码 */
		else if(EN_NBOPEN_CGSN == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CGSN=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CGMI);
		}
		/* 获取设备CGMI */
		else if(EN_NBOPEN_CGMI == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CGMI\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CGMM);
		}
		/* CIMI测试指令 */
		else if(EN_NBOPEN_CGMM == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CGMM\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CFUN1);
		}

#if 0
		/* 配置IP前需要关闭模块功能 */
		else if(EN_NBOPEN_CFUN0 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CFUN=0\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd(pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CFGIP);
		}
#endif

#endif
		/* 打开最大模块功能 */
		else if(EN_NBOPEN_CFUN1 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CFUN=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CIMI);
		}
		/* CIMI测试指令 */
		else if(EN_NBOPEN_CIMI == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CIMI\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_eDRX0);
		}
		/* 关闭eDRX */
		else if(EN_NBOPEN_eDRX0 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CEDRXS=0,5\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CPSMS0);
		}	
		/* 关闭默认睡眠设置 */
		else if(EN_NBOPEN_CPSMS0 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CPSMS=0\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_NNMI1);
		}
		/* 自动接收消息 */
		else if(EN_NBOPEN_NNMI1 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+NNMI=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_NSMI1);
		}
		/* 打开发送开关 */
		else if(EN_NBOPEN_NSMI1 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+NSMI=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CGATT1);
		}
		/* 打开网络附着 */
		else if(EN_NBOPEN_CGATT1 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CGATT=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_CEREG1);
		}
		/* 使能网络注册 */
		else if(EN_NBOPEN_CEREG1 == pst_nb_operation->CurOperateState)
		{
			pst_nb_operation->CurCMD = "AT+CEREG=1\r\n";
			pst_nb_operation->ExpectRcv = "OK";
			NBIOT_send_cmd((char *)pst_nb_operation->CurCMD, pst_nb_operation->WAIT_TIME);
			SetNB_OperatStrState(pst_nb_operation, EN_NBOPEN_IDLE, EN_NBOPEN_END);
		}
		/* 结束状态 */
		else if(EN_NBOPEN_END == pst_nb_operation->CurOperateState)
		{
			/* 设置NB操作结构体处理状态 */
			debug_printf("EN_NBOPEN_END is END\r\n");
			SetNB_ProceStrState(pst_nb_operation, EN_NBPROCESS_HANDLE, EN_NBPROCESS_IDLE);
			pst_nb_operation->NBTick_Flag = ENABLE_NBIOT_Tick;		/* 使能定时发送 */
		}
		/* 空闲等待接收NB-IOT模块回应 */
		else if(EN_NBOPEN_IDLE == pst_nb_operation->CurOperateState)
		{
			/* 跳出状态循环，等待接收NB_IOT模块的回应 */
			NBIOT_MODULE_WAITRX(pst_nb_operation);
			return 0;
		}
		/* 空闲等待接收NB-IOT模块回应 */
		else if(EN_NBOPEN_ERROR == pst_nb_operation->CurOperateState)
		{
			/* TODO：错误状态下发送错误码到串口 */
			
			return 0;
		}
	}
    return 0;
}

/**
  * @brief NB模块处理
 */
static u8 NBIOT_MODULE_HANDLE(PST_NB_OPERATION pst_nb_operation)
{
	char Tempbuf[300] = {0};						/* 缓冲接收buf */
	if(NBUartRecvData((u8 *)Tempbuf) > 0)			/* 一直检查平台接收 */
	{
		/* 判断接收的是否是数据 */
		if(strstr(Tempbuf, "+NNMI") != NULL)
		{
			/* Test:分析接收的数据并显示 */
			debug_printf("%s\r\n", Tempbuf);
			AnalysisCoapMsg(&NBiotDataPacket, Tempbuf, &NBiotTxdaPacket);				/* 分析接收的COAP数据 */
		}	
	}
	return 0;
}

/**
  * @brief NB模块等待接收消息
 */
static u8 NBIOT_MODULE_WAITRX(PST_NB_OPERATION pst_nb_operation)
{
	static u32 TimeCount = 0;
	char lpbuf[300] = {0};			/* 临时接收的数据存放位置 */
	if(++ TimeCount <= 300000)		/* BC35模块官方给出的数据是300s超时时间 */
	{
		if(NBUartRecvData((u8 *)lpbuf) > 0)
		{
			/* 判断接收到的数据中是否有期望接收到的数据 */
			if(strstr(lpbuf, (char *)pst_nb_operation->ExpectRcv) != NULL)
			{
				pst_nb_operation->CurOperateState = pst_nb_operation->NextOperateState;
				pst_nb_operation->ExecuCount = 0;										/* 每次命令执行成功，则命令执行次数清零 */
				debug_printf("%s:success\r\n", pst_nb_operation->CurCMD);
				debug_printf("Enter The Next Operate State\r\n");
				TimeCount = 0;
				return NBIOT_RESPONSE_SUCCESS;
			}
				
			/* 如果接收到错误消息 */
			else if(strstr(lpbuf, "ERROR") != NULL)
			{
				pst_nb_operation->CurOperateState = pst_nb_operation->LastOperateState;
				debug_printf("!!!Attempt excute:%s-->count=%d!!!\r\n", pst_nb_operation->CurCMD, pst_nb_operation->ExecuCount);	/* debug */
				if(++pst_nb_operation->ExecuCount >= 3)
				{
					pst_nb_operation->ExecuCount = 0;										/* 尝试次数清零 */
					pst_nb_operation->CurOperateState = EN_NBOPEN_ERROR;
					TimeCount = 0;
					debug_printf("%s:No Response!!!\r\n", pst_nb_operation->CurCMD);		/* debug */
					return NBIOT_ERROR_NORESPONSE; 											/* 尝试次数过多无响应 */
				}
			}

			/* 判断接收的是否是数据 */
			if(strstr(lpbuf, "+NNMI") != NULL)
			{
				/* Test:分析接收的数据并显示 */
				debug_printf("%s\r\n", lpbuf);
				AnalysisCoapMsg(&NBiotDataPacket, lpbuf, &NBiotTxdaPacket);
			}
		}
	}
	else		/* 如果在规定时间内没有收到数据 */
	{
		pst_nb_operation->CurOperateState = pst_nb_operation->LastOperateState;			/* 尝试重新执行一次 */
		debug_printf("!!!Attempt excute:%s-->count=%d!!!\r\n", pst_nb_operation->CurCMD, pst_nb_operation->ExecuCount);	/* debug */
		if(++pst_nb_operation->ExecuCount >= 3)											
		{
			pst_nb_operation->ExecuCount = 0;			/* 尝试次数清零 */
			TimeCount = 0;
			debug_printf("%s:Timeout!!!\r\n", pst_nb_operation->CurCMD);	/* debug */
			return NBIOT_ERROR_TIMEOUT; 				/* 返回超时 */
		}
	}
	return -1;
}

/**
  * @brief NB模块设置结构体执行操作状态
 */
static void SetNB_OperatStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState)
{
	pst_nb_operation->LastOperateState = pst_nb_operation->CurOperateState;
	pst_nb_operation->CurOperateState = curState;
	pst_nb_operation->NextOperateState = nextState;
	debug_printf("%s:send!!!\r\n", pst_nb_operation->CurCMD);
}

/**
  * @brief NB模块设置结构体执行处理状态
 */
static void SetNB_ProceStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState)
{
	pst_nb_operation->CurProcState =  curState;
	pst_nb_operation->NextProcState = nextState;
}

/**
  * @brief 分析平台发送的数据并显示
 */
static void AnalysisCoapMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet)
{
	char* TempMsg = NULL;
	u32 NBdataLen = GetCoapMsgDataLen(MSG);				/* 获取平台发送了多少长度的数据 */
	debug_printf("NBdataLen = %d\n", NBdataLen);		/* 测试打印长度 */
	TempMsg = strchr(&MSG[0], '+');
	if(TempMsg)											/* 截取逗号以后的数据地址 */
	{
		TempMsg = strchr(&MSG[7], ','); 
	}
	else
	{
		TempMsg = strchr(&MSG[0], ',');  
	}
	
	if(TempMsg)
	{
		TempMsg++;													
		NB_HexStrToNum(TempMsg);														/* 把十六进制转换为数字 */
		SplitCoapData(p_device_packet, (u8 *)TempMsg, NBdataLen, ptx_device_packet);	/* 拆分数据到数据包 */
		/* Test */
		for(int i = 0; i < NBdataLen; i ++)
		{
			debug_printf("MSG[%d] = %d\r\n", i, TempMsg[i]);		
		}
		if(p_device_packet != NULL)														
		{
			debug_printf("NBiot Pack Post\r\n");
			DevicePacket_post(p_device_packet);											/* 发送设备控制数据到设备管理任务 */
		}																			
	}
}


/**
  * @brief NB-IOT软件定时发回调函数送
 */
void NBSystickTmr_callback(OS_TMR *ptmr, void *p_arg)
{
	if(ENABLE_NBIOT_Tick == st_nb_operation.NBTick_Flag)		/* NBIOT定时发送 */
	{
		NBIOT_send_cmd("AT+NMGS=10,5A03E807FF360000A3CA\r\n", 0);/* 节点心跳命令直接发送 */
	}
}

/**
  * @brief 数据组包
 */
void GropPackNBMsgSend(u8* Data, u32 DataLen, u8 ack)
{
	NBiotTxdaPacket.ack = ack;
	NBiotTxdaPacket.data_size = DataLen;					/* 赋值长度 */
	//NBiotTxdaPacket.data = Data;
	memcpy(NBiotTxdaPacket.data, Data, DataLen);
	SendNBMsgPack(&NBiotTxdaPacket); 						/* 数据包发送 */
}

/**
  * @brief 发送数据到平台
 */
void SendNBMsgPack(P_Device_packet_t p_device_packet)
{
	u8 sendbuf[32] = {0};				/* NBIOT Data发送缓冲区 */
	u8 TxNBiotLen = 0;					/* 总的NB消息长度 */
	char NBIOTSendBuf[64] = {0};		/* 总的NB发送消息缓冲区--AT+NMGS=... */
	char NBDataBuf[255] = {0};			/* NBIOT发送数据缓冲区--保存发送数据用 */
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
		sprintf(NBIOTSendBuf,"AT+NMGS=%d,%s\r\n", TxNBiotLen, NBDataBuf);		
		debug_printf("Back Show!!!:%s", NBIOTSendBuf);							/* debug */
		OSTimeDlyHMSM(0, 0, 0, 1);											/* 延迟5ms */
		NBIOT_send_cmd(NBIOTSendBuf, 0);									/* 启动数据发送 */		
	}
}

/**
  * @brief 解包平台数据
 */
static void SplitCoapData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet)
{
	p_device_packet->Starting_mark 	= 	ReceiveBuf[0];						/* 起始标志 */
	p_device_packet->Message_number =	ReceiveBuf[1] << 8 | ReceiveBuf[2];	/* 消息序列号占两个字节 */
	p_device_packet->Node_type 		= 	ReceiveBuf[3];
	p_device_packet->Device_type 	= 	ReceiveBuf[4];
	p_device_packet->Command_code 	= 	ReceiveBuf[5];
	p_device_packet->ack 			=	ReceiveBuf[6];
	p_device_packet->data_size 		= 	ReceiveBuf[7];
	//p_device_packet->data 			=	(u8*)&ReceiveBuf[DataLen - 2 - p_device_packet->data_size];
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

	/*  测试接收到的数据 */
	GropPackNBMsgSend(p_device_packet->data, p_device_packet->data_size, 0x00);	/* debug--回显NBIOT数据测试 */
	
}

/**
  * @brief 获取coap msg数据长度
 */
static unsigned long GetCoapMsgDataLen(char *coapMsg)
{
	char* TempCoapMsg = NULL;
	u32 CoapDataLen = 0;
	char RSCoapData[32] = {0};
	if(coapMsg != NULL)
	{
		strncpy(RSCoapData, coapMsg, 11);				/* 把coap数据前八个字符复制到RSCoapData中--包含\0 */
		TempCoapMsg = strchr(&RSCoapData[5], ':');		/* 把：之后的地址赋值给TempCoapMsg */
		TempCoapMsg ++;
		CoapDataLen = NB_Strtoul(TempCoapMsg, 10);		/* 将字符型数字转换位十进制数值型数字--长度 */
	}
	return CoapDataLen;
}

