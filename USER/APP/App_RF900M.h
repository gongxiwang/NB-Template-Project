/** Copyright(c) 2019, XunFang Tech Limited. All rights reserved.
  * @file		App_RF900M.h
  * @author		walle
  * @version	V1.0.0
  * @date		31/5/2019
  * @brief		app�ӿں����ļ�
  */
#ifndef _APP_RF900M_H
#define _APP_RF900M_H


#include "bsp.h"
#include "stdio.h"
#include "app_nbiot.h"
#include "app_device.h"

#define UHF_TX_BUFFER_MAX_SIZE			0xFF
#define UHF_RX_BUFFER_MAX_SIZE			0xFF
#define UHF_DATA_MAX_SIZE				0xFF
#define UHF_NAME_MAX_SIZE				0x0C
#define UHF_EPC_MAX_LENGTH				0x1E

/**
  * @brief UHF Proc State
 */
typedef enum
{
	EN_UHF_RESET,
	EN_UHF_SETADDR,
	EN_UHF_SETANTENNA,
	EN_UHF_SETPOWER,
	EN_UHF_LISTCARD,
	EN_UHF_READCARD,
	EN_UHF_WRITECAED,
	EN_UHF_WRITERCV,
}EN_UHF_PROC;

/**
  * @brief UHF����
 */
typedef enum
{
	EN_UHF_ANTENNA_1 = 0x00,
	EN_UHF_ANTENNA_2,	
	EN_UHF_ANTENNA_3,
	EN_UHF_ANTENNA_4,
}EN_UHF_ANTENNA;

/**
  * @brief  UHFCMD 
 */
typedef enum
{
    UHF_CMD_RESET = 0x70,       /** ��λ������ */
    UHF_CMD_SET_UART_BAUDRATE, /** ���ô���ͨѶ������ */
    UHF_CMD_GET_FW, /** ��ȡ��д���̼��汾 */
    UHF_CMD_SET_READER_ADDRESS, /** ���ö�д����ַ */
    UHF_CMD_SET_WORK_ANTENNA, /** ���ö�д���������� */
    UHF_CMD_GET_WORK_ANTENNA, /** ��ѯ��ǰ���߹������� */
    UHF_CMD_SET_OUTPUT_POWER, /** ���ö�д����Ƶ������� */
    UHF_CMD_GET_OUTPUT_POWER, /** ��ѯ��д����ǰ������� */
    UHF_CMD_SET_FREQUENCY, /** ���ö�д������Ƶ�ʷ�Χ */
    UHF_CMD_GET_FREQUENCY, /** ��ѯ��д������Ƶ�ʷ�Χ */
    UHF_CMD_SET_BEEPER_MODE, /** ���÷�����״̬ */
    UHF_CMD_GET_READER_TEMP, /** ��ѯ��ǰ�豸�Ĺ����¶� */
    UHF_CMD_SET_DRM_MODE, /** ����DRM ״̬ */
    UHF_CMD_GET_DRM_MODE, /** ��ѯDRM ״̬ */
    
    UHF_CMD_READ_GPIO_VALUE = 0x60, /** ��ȡGPIO ��ƽ */
    UHF_CMD_WRITE_GPIO_VALUE, /** ����GPIO ��ƽ */
    UHF_CMD_SET_ANT_DETECTOR, /** �����������Ӽ����״̬ */
    UHF_CMD_GET_ANT_DETECTOR, /** ��ȡ�������Ӽ����״̬ */
    UHF_CMD_SET_TEMPORARY_POWER = 0x66, /** ���ö�д����ʱ��Ƶ������� */
    UHF_CMD_SET_READER_ID, /** ���ö�д��ʶ���� */
    UHF_CMD_GET_READER_ID, /** ��ȡ��д��ʶ���� */
    
    /**  */
    UHF_CMD_INVENTORY = 0x80, /** �̴��ǩ */
    UHF_CMD_READ, /** ����ǩ */
    UHF_CMD_WRITE, /** д��ǩ */
    UHF_CMD_LOCK, /** ������ǩ */
    UHF_CMD_KILL, /** ����ǩ */
    UHF_CMD_SET_ACCESS_EPC_MATCH, /** ƥ��ACCESS ������EPC �� */
    UHF_CMD_GET_ACCESS_EPC_MATCH, /** ��ѯƥ���EPC ״̬ */
    UHF_CMD_REAL_TIME_INVENTORY = 0x89, /** �̴��ǩ(ʵʱ�ϴ���ǩ����) */
    UHF_CMD_FAST_SWITCH_ANT_INVENTORY, /** ������ѯ��������̴��ǩ */
    UHF_CMD_CUSTOMIZED, /** �Զ���session ��target �̴� */
    UHF_CMD_SET_IMPINJ_FAST_TID, /** ����Monza ��ǩ���ٶ�TID (���ò����������ڲ�FLASH) */
    UHF_CMD_SAVE_IMPINJ_FAST_TID, /** ����Monza ��ǩ���ٶ�TID (���ñ��������ڲ�FLASH) */
    UHF_CMD_GET_IMPINJ_FAST_TID, /** ��ѯ��ǰ�Ŀ���TID ���� */
    
    UHF_CMD_GET_INVENTORY_BUFFER = 0x90, /** ��ȡ��ǩ���ݲ�ɾ������ */
    UHF_CMD_GET_AND_RESET_INVENTORY_BUFFER, /** ��ȡ��ǩ���ݱ������汸�� */
    UHF_CMD_GET_INVENTORY_BUFFER_TAG_COUNT, /** ��ѯ�������Ѷ���ǩ���� */
    UHF_CMD_RESET_INVENTORY_BUFFER, /** ��ձ�ǩ���ݻ��� */
}EN_UHF_CMD;

/**
  * @brief  UHFSTATUES
 */
typedef enum
{
    UHF_STATUS_SUCCESS = 0x10, /** ����ɹ���� */
    UHF_STATUS_FAIL, /** ����ִ��ʧ�� */
    UHF_STATUS_RESET_ERROR = 0x20, /** CPU ��λ���� */
    UHF_STATUS_CW_ON_ERROR, /** ��CW ���� */
    UHF_STATUS_ANTENNA_MISSING_ERROR, /** ����δ���� */
    UHF_STATUS_WRITE_FLASH_ERROR, /** дFlash ���� */
    UHF_STATUS_READ_FLASH_ERROR, /** ��Flash ���� */
    UHF_STATUS_SET_POWER_ERROR, /** ���÷��书�ʴ��� */
    UHF_STATUS_TAG_INVENTORY_ERROR =0x31, /** �̴��ǩ���� */
    UHF_STATUS_TAG_READ_ERROR, /** ����ǩ���� */
    UHF_STATUS_TAG_WRITE_ERROR, /** д��ǩ���� */
    UHF_STATUS_TAG_LOCK_ERROR, /** ������ǩ���� */
    UHF_STATUS_TAG_KILL_ERROR, /** ����ǩ���� */
    UHF_STATUS_NO_TAG_ERROR, /** �޿ɲ�����ǩ���� */
    UHF_STATUS_INVENTORY_OK_ACCESS_FAIL, /** �ɹ��̴浫����ʧ�� */
    UHF_STATUS_BUFFER_EMPTY_ERROR, /** ����Ϊ�� */
    UHF_STATUS_ACCESS_OR_PASSWORD_ERROR = 0x40, /** ���ʱ�ǩ��������������� */
    UHF_STATUS_PARAM_INVALID, /** ��Ч�Ĳ��� */
    UHF_STATUS_PARAM_INVALID_WORD_CNT_TOO_LONG, /** wordCnt ���������涨���� */
    UHF_STATUS_PARAM_INVALID_MEMBANK_RANGE, /** MemBank ����������Χ */
    UHF_STATUS_PARAM_INVALID_LOCK_REGION_RANGE, /** Lock ����������������Χ */
    UHF_STATUS_PARAM_INVALID_LOCK_ACTION_RANGE, /** LockType ����������Χ */
    UHF_STATUS_PARAM_READER_ADDRESS_INVALID, /** ��д����ַ��Ч */
    UHF_STATUS_PARAM_INVALID_ANTENNA_ID_RANGE, /** Antenna_id ������Χ */
    UHF_STATUS_PARAM_INVALID_OUTPUT_POWER_RANGE, /** ������ʲ���������Χ */
    UHF_STATUS_PARAM_INVALID_FREQUENCY_REGION_RANGE, /** ��Ƶ�淶�������������Χ */
    UHF_STATUS_PARAM_INVALID_BAUDRATE_RANGE, /** �����ʲ���������Χ */
    UHF_STATUS_PARAM_BEEPER_MODE_RANGE, /** ���������ò���������Χ */
    UHF_STATUS_PARAM_EPC_MATCH_LEN_TOO_LONG, /** EPC ƥ�䳤��Խ�� */
    UHF_STATUS_PARAM_EPC_MATCH_LEN_ERROR, /** EPC ƥ�䳤�ȴ��� */
    UHF_STATUS_PARAM_INVALID_EPC_MATCH_MODE, /** EPC ƥ�����������Χ */
    UHF_STATUS_PARAM_INVALID_FREQUENCY_RANGE, /** Ƶ�ʷ�Χ���ò������� */
    UHF_STATUS_FAIL_GET_RN16_FROM_TAG, /** �޷����ձ�ǩ��RN16 */
    UHF_STATUS_PARAM_INVALID_DRM_MODE, /** DRM ���ò������� */
    UHF_STATUS_PLL_LOCK_FAIL, /** PLL �������� */
    UHF_STATUS_RF_CHIP_FAIL_TO_RESPONSE, /** ��ƵоƬ����Ӧ */
    UHF_STATUS_FAIL_TO_ACHIEVE_DESIRED_POWER, /** ����ﲻ��ָ����������� */
    UHF_STATUS_COPYRIGHT_AUTH_FAIL, /** ��Ȩ��֤δͨ�� */
    UHF_STATUS_SPECTRUM_REGULATION_ERROR, /** Ƶ�׹淶���ô��� */
    UHF_STATUS_POWER_TOO_LOW, /** ������ʹ��� */
    UHF_STATUS_TIMEOUT_ERROR = 0xff,
}EN_UHF_STATUS;

/**
  * @brief  UHFREgion
 */
typedef enum
{
    UHF_MEMORY_RESERVE 	= 0x00,	/* ���� */
    UHF_MEMORY_EPC 		= 0x01,	/* EPC */
    UHF_MEMORY_TID 		= 0x02,	/* TID */
    UHF_MEMORY_USER		= 0x03,	/* USER */   
}EN_UHF_REGION;

/**
  * @brief  UHF PACK
 */
typedef struct
{
	u8 address;
	EN_UHF_CMD cmd;
	u8 data_size;
	u8 data[UHF_DATA_MAX_SIZE];
}ST_UHF_PACK, *PST_UHF_PACK;

/**
  * @brief  UHF PACK
 */
typedef struct
{
	u8 epc_length;
	u8 epc_password[4];
	u8 epc[UHF_EPC_MAX_LENGTH]; 
} ST_UHF_EPC, *PST_UHF_EPC;

/**
  * @brief ReadTag
 */
typedef struct
{
	u16 TagCount;		/* �ɹ������ı�ǩ���� */
	u8  DataLen;		/* �ɹ����������ݳ��� */
	u8  Data[64];		/* PC + EPC + CRC + Data */
}ST_READ_TAG, *PST_READ_TAG;



/**
  * @brief  UHF Device
 */
typedef struct 
{
	ST_UHF_EPC		st_UHF_epc;					/* EPC */
	ST_UHF_PACK		st_UHF_pack;				/* UHF���ͽ��հ� */
	u8				address;					/* ��ַ */
	EN_UHF_CMD		cmd;						/* ���� */
	u8*				ctlData;					/* ���� */
	u32				Timeout;					/* ��ʱʱ�� */
}ST_UHF_Device, *PST_UHF_Device;

int _cmd_900m_list_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
int _cmd_900m_read_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
int _cmd_900m_write_card(Device_packet_t *packet, PST_UHF_Device pst_UHF_Dev, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_SET_ANTENNA(PST_UHF_Device pst_UHF_device, u8 antenna, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_SET_POWER(PST_UHF_Device pst_UHF_device, u8 power_dbm, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_RESET(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_Init(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
EN_UHF_STATUS UHF_900M_SETADDR(PST_UHF_Device pst_UHF_device, PST_DEVICE pst_device);
#endif