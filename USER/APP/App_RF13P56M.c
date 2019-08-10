/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_RF13P56M.c
  * @author  www.xunfang.com
  * @version  V1.0.0
  * @date  08/01/2016
  * @brief  RFID13.56M
  */ 
#define App_BSP_int_MODULE



#include "bsp_13p56m.h"
#include "App_RF13P56M.h"
#include "app_uart.h"
#include "App_Beep.h"
#include "string.h"
#include "app_device.h"
#include "app_nbiot.h"

static u8 is_operate_state = 0;
/**
  * @brief   13.56M寻标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_13p56m_list_card(Device_packet_t *packet)
{
    u8 card_uid[4] = {0};
    if(is_operate_state)
		return 0;
	is_operate_state = 1;
    if(!PN532_InListPassiveTarget(card_uid))
    {
        packet->ack = ACK_OK;
        packet->data_size = 4;
        memcpy(packet->data, card_uid, 4);
        debug_printf("list 13.56m card uid:");
        debug_print_buffer(card_uid,4);  
        Beep_output_time(30);
    }
    else
    {
        packet->ack = ACK_CARD_HANDLE_ERR;
        packet->data_size = 0;
    }
	is_operate_state = 0;
	GropPackNBMsgSend(packet->data, packet->data_size, 0x00);	
	return 0;
}


/**
  * @brief   13.56M读标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_13p56m_read_card(Device_packet_t *packet)
{
    u8 card_uid[4] = {0};
    u8 block_data[16] = {0};
    if(is_operate_state)
        return 0;
    is_operate_state = 1;
    if(!PN532_InListPassiveTarget(card_uid))/*寻卡*/
    {
        if(!PN532_PsdVerifyKeyA(packet->data[0],card_uid))/*验证密钥*/
        {
            if(!PN532_read(packet->data[0],block_data))/*读卡*/
            {
                packet->ack = ACK_OK;
                packet->data_size = 16;
                memcpy(packet->data,block_data,16);
                debug_printf("read 13.56m card uid and block_data:");
                debug_print_buffer(card_uid,4);
                debug_print_buffer(block_data,16);
                Beep_output_time(30);           
            }
            else
            {
                packet->ack = ACK_CARD_HANDLE_ERR;
                packet->data_size = 0;
            }
        }
        else
        {
            packet->ack = ACK_CARD_HANDLE_ERR;
            packet->data_size = 0;
        }
    }
    else
    {
        packet->ack = ACK_CARD_HANDLE_ERR;
        packet->data_size = 0;
    }    
    is_operate_state = 0;
	GropPackNBMsgSend(packet->data, packet->data_size, 0x00);
    return 0;
}


/**
  * @brief   13.56M写标签
  * @param   packet 数据包
  * @retval  寻标签结果
 */
int _cmd_13p56m_write_card(Device_packet_t *packet)
{
    u8 card_uid[4] = {0};
    if(is_operate_state)
        return 0;
    is_operate_state = 1;
    if(!PN532_InListPassiveTarget(card_uid))/*寻卡*/
    {
        if(!PN532_PsdVerifyKeyA(packet->data[0],card_uid))/*验证密钥*/
        {
            if(!PN532_write(packet->data[0],&packet->data[1]))/*写卡*/
            {
                packet->ack = ACK_OK;
                packet->data_size = 0;
                debug_printf("write 13.56m card uid and block_data:");
                debug_print_buffer(card_uid,4);
                debug_print_buffer(&packet->data[1],16);
                Beep_output_time(30);
            }
            else
            {
                packet->ack = ACK_CARD_HANDLE_ERR;
                packet->data_size = 0;
            }
        }
        else
        {
            packet->ack = ACK_CARD_HANDLE_ERR;
            packet->data_size = 0;
        }
    }
    else
    {
        packet->ack = ACK_CARD_HANDLE_ERR;
        packet->data_size = 0;
    }     
    is_operate_state = 0;
	GropPackNBMsgSend(packet->data, packet->data_size, 0x00);
    return 0;
}

