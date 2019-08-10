#ifndef __BSP_LORA__H
#define __BSP_LORA__H

#include "app_nbiot.h"
#include "bsp.h"
#if 1


u16 LORA_HexStrToNum(char* str);
u32 LORA_Strtoul(const char* pStr,int base);
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen);
u32 LORA_Datalen(char* pStr);


#endif

#endif
