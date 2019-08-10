#ifndef __BSP_BEEP__H
#define __BSP_BEEP__H

#include "bsp.h"


/**
  * @brief º¯ÊýÉùÃ÷
*/
void BSP_Beep_Init(void);
void Beep_on(void);
void Beep_off(void);
void Beep_output_time(INT32U time);
#endif