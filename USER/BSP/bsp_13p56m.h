/** Copyright(c) 2015, XunFang Tech Limited. All rights reserved.
  * @file		PN532.h
  * @author		qyz
  * @version	V1.0.0
  * @date		07/21/2015
  * @brief		PN532驱动头文件。
  */ 

/**
  * @Multi-Include-Prevent Section
  */


#ifndef __RF_13P56M_DRIVER_H__
#define __RF_13P56M_DRIVER_H__ 
/**
  * @Include File Section
  * @包含头文件
  */
#include "bsp.h"

/**
  * @Macro Define Section
  * @宏定义
  */
#define Default_Baud           115200
#define RF13P56M_BUFFSIZE		   200


				   
/**
  * @Prototype Declare Section
  * @内部函数原型声明
  */
static void RF13P56M_Tim2_Init(void);                      /*13.56M模块初始化*/
int PN532_InListPassiveTarget(u8 *uid);     /*寻卡*/
int PN532_PsdVerifyKeyA(u8 block,u8 *uid);  /*密码授权，验证KeyA*/
int PN532_read(u8 block,u8 *block_data);    /*读16个字节 */
int PN532_write(u8 block,u8 *block_data);   /*写16个字节的第一个字节*/
void delay_ms(u32 ms);

#endif /* __RF_13P56M_DRIVER_H__ */

