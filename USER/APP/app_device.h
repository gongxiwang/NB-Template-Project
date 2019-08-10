#ifndef APP_DEVICE_H
#define APP_DEVICE_H


#include "bsp.h"
#include "app_nbiot.h"


/**
  * @brief 节点类型
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
    NODE_LORA,					  /*8 LORA节点 */
    NODE_NULL = 0xFF              /*255 空节点*/
} Node_type_t;

/**
  * @brief 命令列表
 */
typedef enum
{
    DELETE_NODE = 0x00,             /*0 删除节点 */
    FIRMWARE_UPDATE,                /*1 固件更新*/
    WRITE_BIN_CHECK,                /*2 写BIN文件校验*/
    NODE_POWER_CONTROL,             /*3 节点电源控制*/
    QUERY_ONLINE_NODE,              /*4 查询在线设备*/
    NODE_ONLINE,                    /*5 设备上线*/
    NODE_LOSE,                      /*6 设备掉线*/
    BEEP_CONTROL,                   /*7 控制蜂鸣器*/
    LED_CONTROL,                    /*8 控制LED*/
    NODE_WHITE_LIST,                /*9 下发白名单*/
    DELETE_WHITE_LIST,              /*10 删除白名单 */
    CHANGE_CHANLE_PANID,            /*11 修改信道和panid*/

	/* WSN实验箱 */
    ISSUE_BLE_SCAN = 0x10,          /*16 下发蓝牙扫描*/
    BLE_CONNECTING,                 /*17 蓝牙连接扫描到的从机*/
    BLE_CONNECTED_NETWORK,          /*18 蓝牙节点连接上网络*/
    BLE_NODE_LOSS,                  /*19 蓝牙节点丢失*/
    UPLOAD_SENSOR_DATA,             /*20 上传传感器数据*/
    CONTROL_COMMAND,                /*21 下发控制命令*/
    CAN_DATA_PACKETS,               /*22 CAN数据包传输*/
    RS485_DATA_PACKETS,             /*23 485数据包传输*/

	/* RFID实验箱 */
    PRINTER_CONTROL = 0x30,         /*48 打印机控制*/
    SEARCH_CARD,                    /*49 寻卡命令*/
    READ_CARD,                      /*50 读卡命令*/
    WRITE_CARD,                     /*51 写卡命令*/
    NFC_P2P,                        /*52 NFC P2P交互*/
    GET_MAC_ADDR,                   /*53 获取ZigBeeMAC地址*/
    BT_NODE_HEART,                  /*54 蓝牙节点间心跳*/

	/* 物联网创新实训台 */
	AUTO_CONTROL = 0x40,			/*64 自动控制 */
	HAND_CONTROL,					/*65 手动控制     */
	THRESHOLD_CONTROL,				/*66 阈值控制 */
	MODE_CONTROL,					/*67 情景模式控制     */
	LCD_CONTROL,					/*68 LCD显示控制 */
	RESV0_CONTROL,					/*69 控制预留0     */

    /* 智能医疗平台 */
    START_MEASURE = 0x50,           /*80 开始测量      */
    STOP_MEASURE,                   /*81 停止测量     */

	
} Device_cmd_t;

/**
  * @brief 应答码列表
 */
typedef enum
{
    ACK_OK = 0x00,                /*命令操作成功*/
    ACK_NOT_ONLINE,               /*节点不在线*/
    ACK_NOT_REPLY,                /*节点无回复*/
    ACK_DELETE_NODE_FAIL,         /*删除节点失败*/
    ACK_NODE_TYPE_ERR,            /*设备类型错误*/
    ACK_INVALID_CMD,              /*无效命令*/
    ACK_UPDATE_FAIL,              /*固件更新失败应答*/
    ACK_CHECK_FAIL,               /*写BIN文件校验失败应答*/
    ACK_CARD_HANDLE_ERR,          /*寻卡超时/发送超时/充值超时/设置超时*/
    ACK_OTHER_ERR,                /*其他错误*/
    ACK_GET_MAC_ERR,              /*获取节点MAC地址失败*/
    ACK_NODE_WHITE_LIST,		  /* 下发白名单失败 */
    ACK_DELETE_WHITE_LIST,		  /* 删除白名单失败 */
} Device_ack_t;


/**
  * @brief  传感器节点编号
 */
typedef enum
{
    /*传感器类*/
    DEVICE_SENSOR_TEMP = 0x01,    /*1 温度传感器*/
    DEVICE_SENSOR_HUMI,           /*2 环境湿度传感器*/
    DEVICE_SENSOR_LIGHT,          /*3 光敏传感器*/
    DEVICE_SENSOR_HALL,           /*4 霍尔传感器*/
    DEVICE_SENSOR_CGAS,           /*5 煤气检测传感器*/
    DEVICE_SENSOR_VIBRA,          /*6 震动传感器*/
    DEVICE_SENSOR_GYROS,          /*7 三轴加速度传感器*/
    DEVICE_SENSOR_ULTRA,          /*8 超声波传感器*/
    DEVICE_SENSOR_PYRO,           /*9 热释电红外线传感器*/
    DEVICE_SENSOR_PRES,           /*10 压力传感器*/
    DEVICE_SENSOR_SOUND,          /*11 声音传感器*/
    DEVICE_SENSOR_CURR,           /*12 电流传感器*/
    DEVICE_SENSOR_PELE,           /*13 光电传感器*/
    DEVICE_SENSOR_PULSE,          /*14 脉搏传感器*/
    DEVICE_SENSOR_VIOCE,          /*15 语音识别传感器*/
    DEVICE_SENSOR_BOTYT,          /*16 红外测温传感器*/
    DEVICE_SENSOR_ECG,            /*17 心电传感器*/
    DEVICE_SENSOR_HR,             /*18 心率传感器*/
    DEVICE_SENSOR_BLDOX,          /*19 血氧传感器*/
    DEVICE_SENSOR_BLDPR,          /*20 血压传感器*/

	/* 工业传感器 */
    DEVICE_SENSOR_SLTMP,            /*21 土壤温湿度传感器 */
    DEVICE_SENSOR_TMPLG,            /*22 温湿度光照传感器 */
    DEVICE_SENSOR_CO2,            	/*23 二氧化碳传感器 */
    DEVICE_SENSOR_PM2P5,            /*24 PM2.5传感器 */
    DEVICE_SENSOR_GAS,            	/*25 可燃气体传感器 */
    DEVICE_SENSOR_WATMET,           /*26 水表传感器 */
    DEVICE_SENSOR_WINDSP,           /*27 风速传感器 */
    DEVICE_SENSOR_WINDDR,           /*28 风向传感器 */
    DEVICE_SENSOR_BODYTMP,          /*29 人体热释电传感器 */
	DEVICE_SENSOR_SCANGUN,          /*30 扫描枪传感器 */
	DEVICE_SENSOR_RESV0,          	/*31 预留0 */
	DEVICE_SENSOR_RESV1,          	/*32 预留1 */
	DEVICE_SENSOR_RESV2,          	/*33 预留2 */

	/*控制类*/
    DEVICE_IR_CONTROL = 0x30,     /*48 红外控制器*/
    DEVICE_DOOR,                  /*49 门闸*/
    DEVICE_LOCK,                  /*50 电子锁*/
    DEVICE_RELAY,                 /*51 继电器 */
    DEVICE_STEPPER_MOTOR,         /*52 步进电机*/
    DEVICE_PRINTER,               /*53 热敏打印机*/
    DEVICE_POWER_LCD,             /*54 LCD显示模块  */
    DEVICE_POWER_WARMLG,          /*55 加热灯*/
    DEVICE_POWER_SUNLG,           /*56 日光灯*/
    DEVICE_POWER_SNDLGLM,         /*57 声光报警器*/
    DEVICE_POWER_PUMP,        	  /*58 水泵*/
    DEVICE_POWER_FAN,             /*59 风扇*/
    DEVICE_POWER_CUTAN,           /*60 电动窗帘控制器*/
    DEVICE_POWER_TRALG,           /*61 红绿灯模块*/
	DEVICE_POWER_RESV3, 		  /*59 预留3*/
	DEVICE_POWER_RESV4, 		  /*60 预留4*/
	DEVICE_POWER_RESV5, 		  /*61 预留5*/

	/*RFID类*/
    DEVICE_RFID_125K = 0x50,      /*80 125K读头*/
    DEVICE_RFID_13P56M,           /*81 13.56M读头*/
    DEVICE_RFID_900M,             /*82 900M读头*/
    DEVICE_RFID_2P4G,             /*83 2.4G读头*/
    NDEVICE_RFID_MAG_READER,      /*84 磁条卡读头*/
    DEVICE_RFID_IC_READER,        /*85 IC卡读写器*/
    DEVICE_RFID_NFC,              /*86 NFC设备*/
    
    /*数据传输类*/
    DEVICE_433M = 0x70,           /*112 433M节点*/
    DEVICE_WIFI_CAMERA,           /*113 摄像头*/
    
    /*空设备类型*/
    DEVICE_NULL = 0xFF,           /*255 空设备类型*/
} Device_type_t;


/**
  * @brief  设备处理状态机
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
	EN_DEVPROC_IDLE,				/* 空闲 */
}EN_USORPROC_STATE;


/**
  * @brief 设备结构体
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

