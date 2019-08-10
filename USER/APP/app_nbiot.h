/** @copyright XunFang Communication Tech Limited. All rights reserved. 2019.
  * @file  app_nbiot.h
  * @author  walle
  * @version  V1.0
  * @date  20/05/2019
  * @brief  app_nbiot��ͷ�ļ�
  */
#ifndef __NB_IOT__H
#define __NB_IOT__H


/**
  * @brief  ͷ�ļ�
 */
#include "bsp.h"
#include "bsp_nbiot.h"


/**
  * @brief  NB����Э��
 */
typedef struct
{
    u8 		Starting_mark;                     	/* ��ʼ��־ */
    u16 	Message_number;                   	/* ��Ϣ���к� */
    u8 		Node_type;                 			/* �ڵ����� */
    u8 		Device_type;                        /* �豸����     */
    u8 		Command_code;						/* ������     */
    u8 		ack;                   				/* Ӧ���� */
    u8  	data_size;  						/* ���ݳ���     */
	u8		data[64];							/* ���� */
	u8 		Check_code;							/* У���� */
	u8 		End_mark;							/* ������־ */
}Device_packet_t, *P_Device_packet_t;


/**
  * @brief  NBģ�鴦������
 */
typedef enum 
{
	EN_NBPROCESS_INIT,		/* ���̳�ʼ�� */
	EN_NBPROCESS_OPEN,		/* ���̴� */
	EN_NBPROCESS_HANDLE,	/* ���̴��� */
	EN_NBPROCESS_WIITRX,	/* ���̵ȴ� */
	EN_NBPROCESS_IDLE,		/* ���̿��� */
}EN_NB_PROCESS;

/**
  * @brief  NBģ�������
 */
typedef enum
{
	EN_NBOPEN_IDLE,			/* ��״̬���� 			*/
	EN_NBOPEN_REBOOT,		/* ��״̬���� */
	EN_NBOPEN_NCFG,			/* ��״̬�ֶ����� */
	EN_NBOPEN_AT,			/* ��״̬AT���� 		*/
	EN_NBOPEN_ATE0,			/* ��״̬ATE0���� */
	EN_NBOPEN_CMEE,			/* ��״̬CMEE���� */
	EN_NBOPEN_CGSN,			/* ��״̬CGSN���� */
	EN_NBOPEN_CGMI,			/*  */
	EN_NBOPEN_CGMM,			/*  */
	EN_NBOPEN_CIMI,			/* ��״̬CIMI���� */
	EN_NBOPEN_CEREG1,		/* ��״̬ʹ������ע��CEREG���� */
	EN_NBOPEN_IMEI,			/* ��״̬��ȡIMEI����� */
	EN_NBOPEN_CFUN0,		/* ��״̬�ر�ģ�鹦�� */
	EN_NBOPEN_CFUN1,		/* ��״̬�ر�ģ�鹦�� */
	EN_NBOPEN_CFGIP,		/* ��״̬����IP */
	EN_NBOPEN_CPSMS0,		/* �ر�Ĭ�ϵ�˯������ */
	EN_NBOPEN_eDRX0,		/* �ر�eDRX */
	EN_NBOPEN_NNMI1,		/* ��״̬���Զ�������Ϣ */
	EN_NBOPEN_NSMI1,		/* ��״̬�򿪷��Ϳ�����Ϣ */
	EN_NBOPEN_CGATT1,		/* �����總�� */
	EN_NBOPEN_CGATTIF,		/* ��ѯ���總�� */
	EN_NBOPEN_NRB,			/* ģ������ */
	EN_NBOPEN_END,			/* ��״̬�������� */
	EN_NBOPEN_ERROR,		/* ��״̬���� */
}EN_NBOPEN_OPERATE;

/**
  * @brief  NBIOTģ������ṹ��
 */
typedef struct 
{
	u8* 					CurCMD;
	u8 						ExecuCount;			/* ����ִ�д��� 			  */
	u8						LastOperateState;	/* �ϴ�ִ�е�״̬ */
	u8						CurProcState;		/* ��ǰִ�еĴ���״̬ */
	u8						NextProcState;		/* �´�ִ�еĴ���״̬ */
	u8						CurOperateState;	/* �´�ִ�еĲ���״̬ */
	u8 						NextOperateState;	/* ��ǰִ�еĲ���״̬ */
	u32	  					WAIT_TIME;
	u8						NBTick_Flag;		/* NB_IOT��ʱ�ϱ���־ */
	u8	  					ERRORCODE;
	u8*						ExpectRcv;
	u8*						ErrorRcv;
}ST_NB_OPERAION, *PST_NB_OPERATION;


/**
  * @brief  NBIOTģ�������Ӧ������
 */
typedef enum
{
	NBIOT_ERROR_NORESPONSE 	= 0xF0,
	NBIOT_ERROR_TIMEOUT 	= 0xFF,


}EN_NBIOT_ERRORCODE;

/**
  * @brief  NBIOTģ�������Ӧ��
 */
typedef enum
{
	NBIOT_RESPONSE_SUCCESS 	= 0xA0,
	NBIOT_RESPONSE_FAIL 	= 0xA1,
	NBIOT_OPEN_SUCCESS,

}EN_NBIOT_RESPONSECODE;


void App_NBIOT_TASK(void);
u8 NBIOT_send_cmd(char* ucCMD, u16 waittime);
u8 NBIOT_MODULE_PROCESS(PST_NB_OPERATION pst_nb_operation);
u8 NBIOT_MODULE_OPEN(PST_NB_OPERATION pst_nb_operation);
static u8 NBIOT_MODULE_WAITRX(PST_NB_OPERATION pst_nb_operation);
static void SetNB_OperatStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState);
static void SetNB_ProceStrState(PST_NB_OPERATION pst_nb_operation, u8 curState, u8 nextState);
static u8 NBIOT_MODULE_INIT(PST_NB_OPERATION pst_nb_operation);
static void AnalysisCoapMsg(P_Device_packet_t p_device_packet, char* MSG, P_Device_packet_t ptx_device_packet);
static u8 NBIOT_MODULE_HANDLE(PST_NB_OPERATION pst_nb_operation);
void NBSystickTmr_callback(OS_TMR *ptmr, void *p_arg);
void GetNBMsgFrQue(P_Device_packet_t p_device_packet);
void SendNBMsgPack(P_Device_packet_t p_device_packet);
static unsigned long GetCoapMsgDataLen(char *coapMsg);
static void SplitCoapData(P_Device_packet_t p_device_packet, u8* ReceiveBuf, u32 DataLen, P_Device_packet_t ptx_device_packet);
void GropPackNBMsgSend(u8* Data, u32 DataLen, u8 ack);


#endif
