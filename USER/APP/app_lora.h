/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_lora.h
  * @author  walle
  * @version  V1.0
  * @date  20/05/2019
  * @brief  app_lora��ͷ�ļ�
  */
#ifndef __APP_LORA__H
#define __APP_LORA__H

/**
  * @brief  ͷ�ļ�
 */
#include "bsp.h"
#include "bsp_nbiot.h"

/**
  * @brief  LORAģ�鴦������
 */
typedef enum 
{
	EN_LORAPROCESS_INIT,		/* ���̳�ʼ�� */
	EN_LORAPROCESS_OPEN,		/* ���̴� */
	EN_LORAPROCESS_HANDLE,		/* ���̴��� */
	EN_LORAPROCESS_WIITRX,		/* ���̵ȴ� */
	EN_LORAPROCESS_IDLE,		/* ���̿��� */
}EN_LORA_PROCESS;

/**
  * @brief  LORAģ�������
 */
typedef enum
{
	EN_LORAOPEN_IDLE,			/* ��״̬���� 			*/
	EN_LORAOPEN_ADD,			/* ��״̬AT���� 		*/	
	EN_LORAOPEN_REBOOT,			/* ��״̬���� */
	EN_LORAOPEN_CHECKREBOOT,	/* ��״̬�����ɹ���� */
	EN_LORAOPEN_MOD,			/* ��״̬���ù���ģʽ */
	EN_LORAOPEN_CLA,			/* ��״̬����CLASS�� 		*/
	EN_LORAOPEN_JON,			/* ��״̬�������� */
	EN_LORAOPEN_RJN,			/* ��״̬������������ */
	EN_LORAOPEN_END,			/* ��״̬���� */
	EN_LORAOPEN_ERROR,			/* �򿪴��� */
}EN_LORAOPEN_OPERATE;

/**
  * @brief  LORAģ������ṹ��
 */
typedef struct 
{
	char*					CurCMD;
	u8 						ExecuCount;			/* ����ִ�д��� */
	u8						LastOperateState;	/* �ϴ�ִ�е�״̬ */
	u8						CurProcState;		/* ��ǰִ�еĴ���״̬ */
	u8						NextProcState;		/* �´�ִ�еĴ���״̬ */
	u8						CurOperateState;	/* �´�ִ�еĲ���״̬ */
	u8 						NextOperateState;	/* ��ǰִ�еĲ���״̬ */
	u32	  					WAIT_TIME;
	u8						LORATick_Flag;		/* LORA��ʱ�ϱ���־ */
	u8	  					ERRORCODE;
	u8*						ExpectRcv;
	u8*						ErrorRcv;
}ST_LORA_OPERAION, *PST_LORA_OPERATION;


/**
  * @brief  LORAģ�������Ӧ������
 */
typedef enum
{
	LORA_ERROR_NORESPONSE 	= 0xF0,
	LORA_ERROR_TIMEOUT 	= 0xFF,


}EN_LORA_ERRORCODE;

/**
  * @brief  LORAģ�������Ӧ��
 */
typedef enum
{
	LORA_RESPONSE_SUCCESS 	= 0xA0,
	LORA_RESPONSE_FAIL 	= 0xA1,
	LORA_OPEN_SUCCESS,
}EN_LORA_RESPONSECODE;


void App_LORA_TASK(void);
u8 LORA_send_cmd(char* ucCMD, u16 waittime);
static u8 LORA_MODULE_WAITRX(PST_LORA_OPERATION pst_lora_operation);
static void SplitLORAData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet);
void SendLORAMsgPack(P_Device_packet_t p_device_packet);
static void AnalysisLoRaMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet);
#endif
