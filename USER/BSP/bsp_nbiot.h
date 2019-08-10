#ifndef __BSP_NBIOT__H
#define __BSP_NBIOT__H

#include "app_nbiot.h"
#include "bsp.h"


/**
  * @brief ���������Ϣ����
 */
typedef enum
{
  MSG_NONE,
  MSG_INIT,
  MSG_IMSI,
  MSG_MODULE_INFO,
  MSG_MID,     //������ID
  MSG_MMODEL,  //�����ͺ�
  MSG_MREV,    //���Ұ汾��
  MSG_BAND,    //����Ƶ��
  MSG_IMEI,    //�ƶ��豸�����
  MSG_SIGN,    //�ź�ǿ��
  MSG_REG,
  MSG_UDP_CREATE,
  MSG_UDP_CLOSE,
  MSG_UDP_SEND,
  MSG_UDP_RECE,
  MSG_COAP,
  MSG_COAP_SEND,
  MSG_COAP_RECE,
  MSG_END
}msg_types_t;

/**
  * @brief undefine function
 */
typedef int (*NB_ReceCB)(msg_types_t, int, char*);

/**
  * @brief undefine function
 */
typedef int (*NB_Log)(char*, int);

/**
  * @brief NBIOT���
 */
typedef struct NB_Config *NB_Handle;

/**
  * @brief ��NBģ���������ǰ�ڵĳ�ʼ������
 */
typedef int (*NB_Open)(NB_Handle);

/**
  * @brief ��NBģ�������ռ�õ���Դ�����ͷ�
 */
typedef int (*NB_Close)(NB_Handle);

/**
  * @brief �Զ�������س�ʼ��ָ��
 */
typedef int (*NB_AutoInitModule)(NB_Handle);

/**
  * @brief ��ȡNBģ�����Ϣ�����磺Manufacturer ID, Device Module,Firmware Version,Freqenucy band
 */
typedef int (*NB_ModuleInfo)(NB_Handle);

/**
  * @brief ��ȡNBģ�������ע����Ϣ
 */
typedef int (*NB_NetRegisterInfo)(NB_Handle);

/**
  * @brief ��ȡNBģ��SIM������Ϣ
 */
typedef const char* (*NB_USIMInfo)(NB_Handle);

/**
  * @brief ��ȡNBģ���ź�ǿ��
 */
typedef int (*NB_NetSign)(NB_Handle);

/**
  * @brief ����NBģ��ͨ��������
 */
typedef int (*NB_DefPDP)(NB_Handle);

/**
  * @brief ʧ��NBģ����������Ϣ
 */
typedef int (*NB_DeactPDP)(NB_Handle);

/**
  * @brief ����UDP
 */
typedef int (*NB_CreateUdp)(NB_Handle);

/**
  * @brief �ر�UDP
 */
typedef int (*NB_CloseUdp)(NB_Handle);

/**
  * @brief ����UDP����
 */
typedef int (*NB_SendUdpData)(NB_Handle,int,char*);

/**
  * @brief CoAP����
 */
typedef int (*NB_CoAPServer)(NB_Handle, bool,char*);

/**
  * @brief CoAP������ʾ��ʽ����
 */
typedef int (*NB_CoAPSentIndication)(NB_Handle,int code);

/**
  * @brief CoAP�������ݷ�ʽ����
 */
typedef int (*NB_CoAPReceMode)(NB_Handle,int);

/**
  * @brief CoAPЭ����Ϣ����
 */
typedef int (*NB_CoAPSentMsg)(NB_Handle,int,char*);

/**
  * @brief NB Modle reset cmd
 */
typedef int (*NB_Reset)(NB_Handle);

/**
  * @brief NB ping function
 */
typedef int (*NB_Ping)(NB_Handle);

/**
  * @brief �жϽ��������ǲ���Udp
 */
typedef int (*NB_isUdpReceData)(NB_Handle, int, char*);

/**
  * @brief �жϽ��������Ƿ�ΪCoAP����
 */
typedef int (*NB_IsCoAPReceData)(NB_Handle,int,char*);

/**
  * @brief ����UDP����
 */
typedef int (*NB_ReceUdpData)(NB_Handle);

/**
  * @brief CoAPЭ����Ϣ����
 */
typedef int (*NB_CoAPReceMsg)(NB_Handle);

/**
  * @brief NB���߳�
 */
typedef int (*NB_MainThread)(NB_Handle);

/**
  * @brief ����NBIOTģ�����Ҫʵ�ֵĹ��ܺ���
 */
typedef struct
{
  NB_Open                 openFxn;
  NB_AutoInitModule       nbModuleInit;
  NB_ModuleInfo           getModuleInfo;
  NB_NetRegisterInfo      getModuleRegisterInfo;
  NB_USIMInfo             getUSIMinfo;
  NB_NetSign              getSign;
  NB_CreateUdp            createUdp;
  NB_CloseUdp             closeUdp;
  NB_SendUdpData          sendUdp;
  NB_CoAPServer           coapServer;
  NB_CoAPSentIndication   coapSentIndication;
  NB_CoAPReceMode         coapSetReceMode;
  NB_CoAPSentMsg          coapSentMsg;
  NB_MainThread           mainThread;
}NB_FxnTable;


/**
  * @brief NBIOTģ��Ĺ��ܽṹ����
 */
typedef struct NB_Config
{
  NB_FxnTable        *fxnTablePtr;	  /* NBģ����Ҫʵ�ֵĺ��� */
  void               *object;         /* NB uart����ָ�� */
  NB_ReceCB          AppReceCB;
  NB_Log             log;
}NB_Config;

u16 NB_HexStrToNum(char* str);
u32 NB_Strtoul(const char* pStr,int base);
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen);



#endif
