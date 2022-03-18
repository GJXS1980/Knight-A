#include "beep.h"

//有源蜂鸣器
void Beep_Init(void)
{
  // 4kHz
  u16 arr = 500 - 1;
  u16 psc = 84 - 1;
	// TIM11
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM11, ENABLE );  	

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource9, GPIO_AF_TIM11 );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频，比如时钟预分频数psc为 79，则驱动计数器的时钟(80M) CK_INT / (79+1)=1M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseStructure.TIM_Period = arr; // 输出PWM频率为freq = psc / arr
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分割并不是分频器的意思，a这是一个采样的频率设置，就像AD的采样一样，在作为定时器捕捉功能时，可以通过设置这个能屏蔽高频干扰
	TIM_TimeBaseInit( TIM11,&TIM_TimeBaseStructure );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse = arr/2;				//待预装载值
	TIM_OC1Init( TIM11, &TIM_OCInitStructure );
	TIM_OC1PreloadConfig( TIM11, TIM_OCPreload_Enable );
 
  TIM_ARRPreloadConfig( TIM11,ENABLE );
	TIM_ClearFlag( TIM11, TIM_FLAG_Update );//清中断标志位
	// TIM_Cmd( TIM11, ENABLE );
}

void Beep_On(void) {
  // TIM11->CCR1 = 250;
  TIM_Cmd( TIM11, ENABLE );
}

void Beep_Off(void) {
  // TIM11->CCR1 = 0;
  TIM_Cmd( TIM11, DISABLE );
}
