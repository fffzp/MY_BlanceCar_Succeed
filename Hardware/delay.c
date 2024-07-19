#include "delay.h"
#include "stm32f1xx_hal.h"	  
	
//延时nus
//nus为要延时的us数.	
//nus:0~190887435(最大值即2^32/fac_us@fac_us=168)
static uint8_t fac_us = 72;    //这里主时钟为72, 所以在1us内ticks会72次


void delay_init(uint8_t SYSCLK)
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);       //SysTick频率为HCLK
	fac_us=SYSCLK;
}								    

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;	//装载值	    	 
	ticks=nus*fac_us; //需要的节拍数 
	told=SysTick->VAL; //刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//计数器递减
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;	//时间超过或等于延迟的时间时退出.
		}  
	};
}

//ms延时
void delay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) delay_us(1000);
}

//设置延时的时间
void SetTime(tTimeDelay *TimeDelay,uint32_t TimeInter)
{
	TimeDelay->TMStart = HAL_GetTick();
	
	TimeDelay->TMInter = TimeInter;
}

//比较延时的时间是否到达，达到则返回真，否则返回0
uint8_t CompareTime(tTimeDelay *TimeDelay)
{
	return ((HAL_GetTick() - TimeDelay->TMStart) >= TimeDelay->TMInter) ;
}


