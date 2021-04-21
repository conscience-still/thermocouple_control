#ifndef _TIMER_H
#define _TIMER_H
#include "main.h"
	
extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 

void TIM3_Init(uint16_t arr,uint16_t psc);
uint32_t getSystemTime(void);
#endif

