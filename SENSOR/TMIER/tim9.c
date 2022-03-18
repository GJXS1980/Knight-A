#include "stm32f4xx.h"
#include "tim9.h"
#include "iwdg.h"
#include "usart.h"

void TIM9_INIT(void)
{
	
	u32 arr = 10000- 1;
	u32 psc = 8400 - 1;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//分频系数为8400-1	8400 0000/8400 = 10000HZ		1/10000 = 0.0001S
	TIM_TimeBaseStructure.TIM_Period = arr;						//预装载值为1000-1  	10000*0.00001S = 1S
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM9,TIM_IT_Update);
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);					//溢出更新
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ARRPreloadConfig(TIM9,ENABLE);							//ARPE使能 
	TIM_Cmd(TIM9, ENABLE);  									//使能TIM9 
}


void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
	{			
		UPDATE_FLAG = 1;
		printf("hello word\r\n");
		TIM_ClearFlag(TIM9,TIM_IT_Update);
	}
	
}


