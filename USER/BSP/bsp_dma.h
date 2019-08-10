#ifndef __BSP_DMA__H
#define __BSP_DMA__H

#include "bsp.h"

void BSP_DMA_Init(void);
void UsartDMA_Send_Data(char *buffer, unsigned short size);
void NBiotDMA_Send_Data(char *buffer);
static void BSP_USART2RXDMA_Init(void);
void NBiotDMA_Receive_Data(u8 *buffer);
static void BSP_USART1DMASEND_Init(void);
void USART1DMA_Send_Data(u8 *buffer, int Size);
static void DMANVIC_Configuration(void);
void USART1DMA_Receive_Data(u8 *buffer);
void LoRaDMA_Send_Data(char *buffer);
#endif