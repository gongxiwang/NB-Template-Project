/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  app_watmet.h
  * @author  walle
  * @version  V1.0.0
  * @date  08/06/2019
  * @brief  水表传感器
  */ 
#ifndef __APP_WATMET__H
#define __APP_WATMET__H



/**
  * @brief  水表ModuleBUS协议组包
 */
typedef struct
{
	unsigned char  ucADDRCODE;		/* 地址码 */
    unsigned char  ucFUNCODE;		/* 功能码 */
	unsigned char  ucDataSize;		/* 数据长度 */
	unsigned char  ucStarAdd_H;		/* 起始地址_高 */
	unsigned char  ucStarAdd_L;		/* 起始地址_低 */
	unsigned char  ucRegData_H;		/* 寄存器数据_高 */
	unsigned char  ucRegData_L;		/* 寄存器数据_低 */
	unsigned char  ucByteNum;		/* 字节数 */
	unsigned char  ucWrittingData_H;/* 待写入数据_高 */
	unsigned char  ucWrittingData_L;/* 待写入数据_高 */
	unsigned char  ucCRC0;			/* CRC0校验 */
	unsigned char  ucCRC1;			/* CRC1校验 */
}ST_WATMET_PACK, *PST_WATMET_PACK;


/**
  * @brief  水表传感器ModuleBUS功能码
 */
typedef enum
{
	EN_FUNCODE_READREG = 0x03,	/* 读取寄存器 */
	EN_FUNCODE_WRTEREG = 0x10,	/* 写入寄存器 */
	EN_FUNCODE_CTLGATE = 0x05,	/* 控制阀门 */
	EN_FUNCODE_RDGATE  = 0x01,	/* 读取阀门 */
}EN_FUN_CODE;


/**
  * @brief  水表传感器开关阀门
 */
typedef enum
{
	EN_VALUE_ON = 0x00ff,	/* 打开阀门 */
	EN_VALUE_OFF = 0x0000, /* 关闭阀门 */
	EN_USED_WATER = 0x0002,/* 总用水量 */
}EN_WATMET_REG;



void Watmet_Init(void);
void Watmet_Read(void);
void CreatModBusPackWithSend(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 FuCode, u16 StartAdr, u16 RegData);
u16 CRC16_Modbus(u8*_pBuf, u16 _usLen);
void ModBusSendPack(u8* sendbuf, u8 len);
u8 WatmetRecvAnalysis(PST_WATMET_PACK pst_ModBusPack_Watmet, u8 *RecvData);
u8 WatmetRecv(PST_WATMET_PACK pst_ModBusPack_Watmet);


#endif
