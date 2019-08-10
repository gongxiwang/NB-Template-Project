/** @copyright XunFang Communication Tech Limited. All rights reserved. 2013.
  * @file  App_RF13P56M.h
  * @author  www.xunfang.com
  * @version  V1.0.0
  * @date  08/01/2016
  * @brief  RFID13.56M
  */ 
#ifndef _app_13p56m_h_include_
#define _app_13p56m_h_include_

#include "app_device.h"
#include "app_nbiot.h"

int _cmd_13p56m_list_card(Device_packet_t *packet);
int _cmd_13p56m_read_card(Device_packet_t *packet);
int _cmd_13p56m_write_card(Device_packet_t *packet);
void RF3P56MRecvData(void);
#endif
