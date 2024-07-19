#include "delay.h"
#include "stm32f1xx_hal.h"	  
	
//��ʱnus
//nusΪҪ��ʱ��us��.	
//nus:0~190887435(���ֵ��2^32/fac_us@fac_us=168)
static uint8_t fac_us = 72;    //������ʱ��Ϊ72, ������1us��ticks��72��


void delay_init(uint8_t SYSCLK)
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);       //SysTickƵ��ΪHCLK
	fac_us=SYSCLK;
}								    

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;	//װ��ֵ	    	 
	ticks=nus*fac_us; //��Ҫ�Ľ����� 
	told=SysTick->VAL; //�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//�������ݼ�
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;	//ʱ�䳬��������ӳٵ�ʱ��ʱ�˳�.
		}  
	};
}

//ms��ʱ
void delay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) delay_us(1000);
}

//������ʱ��ʱ��
void SetTime(tTimeDelay *TimeDelay,uint32_t TimeInter)
{
	TimeDelay->TMStart = HAL_GetTick();
	
	TimeDelay->TMInter = TimeInter;
}

//�Ƚ���ʱ��ʱ���Ƿ񵽴�ﵽ�򷵻��棬���򷵻�0
uint8_t CompareTime(tTimeDelay *TimeDelay)
{
	return ((HAL_GetTick() - TimeDelay->TMStart) >= TimeDelay->TMInter) ;
}


