#include "hs485hb.h"

void TIM9_PWM_Init(u32 arr,u32 psc)			//周期为40ms  可控范围为0 ~ 2.5ms
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM9,&TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM9,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM9,ENABLE);
	
	TIM_ClearFlag( TIM9, TIM_FLAG_Update );
		
	TIM_Cmd(TIM9,ENABLE);
	
	TIM9->CCR2 = 2266; //初始位置60度

}	


void Set_HS485HB_Rotation(float rotation) 
{
  if (rotation>180)
  {
    rotation = 180;
  }
  else if (rotation<0)
  {
    rotation = 0 ;
  }
  
    TIM9->CCR2 = (u16)(rotation * 3800/180 + 1000);
}



