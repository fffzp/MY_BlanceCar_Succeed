#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f1xx_hal.h"	  


typedef struct{
	uint32_t TMStart;
	uint32_t TMInter;
	
}tTimeDelay;


void SetTime(tTimeDelay *TimeType,uint32_t TimeInter);

uint8_t  CompareTime(tTimeDelay *TimeType);



void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
#endif

