#ifndef APP_DEVICE_H
#define APP_DEVICE_H


#include "bsp.h"
#include "app_nbiot.h"


/**
  * @brief �ڵ�����
 */
typedef enum
{
    NODE_ZIGBEE = 0x00,           /*0 zigbee*/
    NODE_RFID,                    /*1 RFID*/
    NODE_BT,               		  /*2 BT*/
    NODE_WIFI,                    /*3 wifi*/
    NODE_IPV6,                    /*4 ipv6*/
    NODE_CAN,                     /*5 CAN*/
    NODE_485,                     /*6 485*/
    NODE_NB,                      /*7 NB_IOT*/
    NODE_LORA,					  /*8 LORA�ڵ� */
    NODE_NULL = 0xFF              /*255 �սڵ�*/
} Node_type_t;

/**
  * @brief �����б�
 */
typedef enum
{
    DELETE_NODE = 0x00,             /*0 ɾ���ڵ� */
    FIRMWARE_UPDATE,                /*1 �̼�����*/
    WRITE_BIN_CHECK,                /*2 дBIN�ļ�У��*/
    NODE_POWER_CONTROL,             /*3 �ڵ��Դ����*/
    QUERY_ONLINE_NODE,              /*4 ��ѯ�����豸*/
    NODE_ONLINE,                    /*5 �豸����*/
    NODE_LOSE,                      /*6 �豸����*/
    BEEP_CONTROL,                   /*7 ���Ʒ�����*/
    LED_CONTROL,                    /*8 ����LED*/
    NODE_WHITE_LIST,                /*9 �·�������*/
    DELETE_WHITE_LIST,              /*10 ɾ�������� */
    CHANGE_CHANLE_PANID,            /*11 �޸��ŵ���panid*/

	/* WSNʵ���� */
    ISSUE_BLE_SCAN = 0x10,          /*16 �·�����ɨ��*/
    BLE_CONNECTING,                 /*17 ��������ɨ�赽�Ĵӻ�*/
    BLE_CONNECTED_NETWORK,          /*18 �����ڵ�����������*/
    BLE_NODE_LOSS,                  /*19 �����ڵ㶪ʧ*/
    UPLOAD_SENSOR_DATA,             /*20 �ϴ�����������*/
    CONTROL_COMMAND,                /*21 �·���������*/
    CAN_DATA_PACKETS,               /*22 CAN���ݰ�����*/
    RS485_DATA_PACKETS,             /*23 485���ݰ�����*/

	/* RFIDʵ���� */
    PRINTER_CONTROL = 0x30,         /*48 ��ӡ������*/
    SEARCH_CARD,                    /*49 Ѱ������*/
    READ_CARD,                      /*50 ��������*/
    WRITE_CARD,                     /*51 д������*/
    NFC_P2P,                        /*52 NFC P2P����*/
    GET_MAC_ADDR,                   /*53 ��ȡZigBeeMAC��ַ*/
    BT_NODE_HEART,                  /*54 �����ڵ������*/

	/* ����������ʵѵ̨ */
	AUTO_CONTROL = 0x40,			/*64 �Զ����� */
	HAND_CONTROL,					/*65 �ֶ�����     */
	THRESHOLD_CONTROL,				/*66 ��ֵ���� */
	MODE_CONTROL,					/*67 �龰ģʽ����     */
	LCD_CONTROL,					/*68 LCD��ʾ���� */
	RESV0_CONTROL,					/*69 ����Ԥ��0     */

    /* ����ҽ��ƽ̨ */
    START_MEASURE = 0x50,           /*80 ��ʼ����      */
    STOP_MEASURE,                   /*81 ֹͣ����     */

	
} Device_cmd_t;

/**
  * @brief Ӧ�����б�
 */
typedef enum
{
    ACK_OK = 0x00,                /*��������ɹ�*/
    ACK_NOT_ONLINE,               /*�ڵ㲻����*/
    ACK_NOT_REPLY,                /*�ڵ��޻ظ�*/
    ACK_DELETE_NODE_FAIL,         /*ɾ���ڵ�ʧ��*/
    ACK_NODE_TYPE_ERR,            /*�豸���ʹ���*/
    ACK_INVALID_CMD,              /*��Ч����*/
    ACK_UPDATE_FAIL,              /*�̼�����ʧ��Ӧ��*/
    ACK_CHECK_FAIL,               /*дBIN�ļ�У��ʧ��Ӧ��*/
    ACK_CARD_HANDLE_ERR,          /*Ѱ����ʱ/���ͳ�ʱ/��ֵ��ʱ/���ó�ʱ*/
    ACK_OTHER_ERR,                /*��������*/
    ACK_GET_MAC_ERR,              /*��ȡ�ڵ�MAC��ַʧ��*/
    ACK_NODE_WHITE_LIST,		  /* �·�������ʧ�� */
    ACK_DELETE_WHITE_LIST,		  /* ɾ��������ʧ�� */
} Device_ack_t;


/**
  * @brief  �������ڵ���
 */
typedef enum
{
    /*��������*/
    DEVICE_SENSOR_TEMP = 0x01,    /*1 �¶ȴ�����*/
    DEVICE_SENSOR_HUMI,           /*2 ����ʪ�ȴ�����*/
    DEVICE_SENSOR_LIGHT,          /*3 ����������*/
    DEVICE_SENSOR_HALL,           /*4 ����������*/
    DEVICE_SENSOR_CGAS,           /*5 ú����⴫����*/
    DEVICE_SENSOR_VIBRA,          /*6 �𶯴�����*/
    DEVICE_SENSOR_GYROS,          /*7 ������ٶȴ�����*/
    DEVICE_SENSOR_ULTRA,          /*8 ������������*/
    DEVICE_SENSOR_PYRO,           /*9 ���͵�����ߴ�����*/
    DEVICE_SENSOR_PRES,           /*10 ѹ��������*/
    DEVICE_SENSOR_SOUND,          /*11 ����������*/
    DEVICE_SENSOR_CURR,           /*12 ����������*/
    DEVICE_SENSOR_PELE,           /*13 ��紫����*/
    DEVICE_SENSOR_PULSE,          /*14 ����������*/
    DEVICE_SENSOR_VIOCE,          /*15 ����ʶ�𴫸���*/
    DEVICE_SENSOR_BOTYT,          /*16 ������´�����*/
    DEVICE_SENSOR_ECG,            /*17 �ĵ紫����*/
    DEVICE_SENSOR_HR,             /*18 ���ʴ�����*/
    DEVICE_SENSOR_BLDOX,          /*19 Ѫ��������*/
    DEVICE_SENSOR_BLDPR,          /*20 Ѫѹ������*/

	/* ��ҵ������ */
    DEVICE_SENSOR_SLTMP,            /*21 ������ʪ�ȴ����� */
    DEVICE_SENSOR_TMPLG,            /*22 ��ʪ�ȹ��մ����� */
    DEVICE_SENSOR_CO2,            	/*23 ������̼������ */
    DEVICE_SENSOR_PM2P5,            /*24 PM2.5������ */
    DEVICE_SENSOR_GAS,            	/*25 ��ȼ���崫���� */
    DEVICE_SENSOR_WATMET,           /*26 ˮ������ */
    DEVICE_SENSOR_WINDSP,           /*27 ���ٴ����� */
    DEVICE_SENSOR_WINDDR,           /*28 ���򴫸��� */
    DEVICE_SENSOR_BODYTMP,          /*29 �������͵紫���� */
	DEVICE_SENSOR_SCANGUN,          /*30 ɨ��ǹ������ */
	DEVICE_SENSOR_RESV0,          	/*31 Ԥ��0 */
	DEVICE_SENSOR_RESV1,          	/*32 Ԥ��1 */
	DEVICE_SENSOR_RESV2,          	/*33 Ԥ��2 */

	/*������*/
    DEVICE_IR_CONTROL = 0x30,     /*48 ���������*/
    DEVICE_DOOR,                  /*49 ��բ*/
    DEVICE_LOCK,                  /*50 ������*/
    DEVICE_RELAY,                 /*51 �̵��� */
    DEVICE_STEPPER_MOTOR,         /*52 �������*/
    DEVICE_PRINTER,               /*53 ������ӡ��*/
    DEVICE_POWER_LCD,             /*54 LCD��ʾģ��  */
    DEVICE_POWER_WARMLG,          /*55 ���ȵ�*/
    DEVICE_POWER_SUNLG,           /*56 �չ��*/
    DEVICE_POWER_SNDLGLM,         /*57 ���ⱨ����*/
    DEVICE_POWER_PUMP,        	  /*58 ˮ��*/
    DEVICE_POWER_FAN,             /*59 ����*/
    DEVICE_POWER_CUTAN,           /*60 �綯����������*/
    DEVICE_POWER_TRALG,           /*61 ���̵�ģ��*/
	DEVICE_POWER_RESV3, 		  /*59 Ԥ��3*/
	DEVICE_POWER_RESV4, 		  /*60 Ԥ��4*/
	DEVICE_POWER_RESV5, 		  /*61 Ԥ��5*/

	/*RFID��*/
    DEVICE_RFID_125K = 0x50,      /*80 125K��ͷ*/
    DEVICE_RFID_13P56M,           /*81 13.56M��ͷ*/
    DEVICE_RFID_900M,             /*82 900M��ͷ*/
    DEVICE_RFID_2P4G,             /*83 2.4G��ͷ*/
    NDEVICE_RFID_MAG_READER,      /*84 ��������ͷ*/
    DEVICE_RFID_IC_READER,        /*85 IC����д��*/
    DEVICE_RFID_NFC,              /*86 NFC�豸*/
    
    /*���ݴ�����*/
    DEVICE_433M = 0x70,           /*112 433M�ڵ�*/
    DEVICE_WIFI_CAMERA,           /*113 ����ͷ*/
    
    /*���豸����*/
    DEVICE_NULL = 0xFF,           /*255 ���豸����*/
} Device_type_t;


/**
  * @brief  �豸����״̬��
 */
typedef enum
{
	EN_DEVPROC_SLTMP,
	EN_DEVPROC_WATMT,
	EN_DEVPROC_13P56M,
	EN_DEVPROC_900M,
	EN_DEVPROC_RELAY,
	EN_DEVPROC_PUMP,
	EN_DEVPROC_WARMLG,
	EN_DEVPROC_ICRD,
	EN_DEVPROC_SCANGUN,
	EN_DEVPROC_IDLE,				/* ���� */
}EN_USORPROC_STATE;


/**
  * @brief �豸�ṹ��
 */
typedef struct
{
	u8	DeviceNum;
	u8	DeviceAct;
}ST_DEVICE, *PST_DEVICE;

void App_DEVICE_TASK(void *p_data);
u8 DeviceOperateProc(P_Device_packet_t p_device_packet);
void DevicePacket_post(Device_packet_t *packet);
void DoDevCtl(PST_DEVICE pst_dev);
void DevActionProc(PST_DEVICE pst_device);




#endif

