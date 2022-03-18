#include "pwm.h"

// A6,A7,B0,B1对应TIM3的通道1/2/3/4
// 使用TIM3输出PWM
// PWM频率freq_PWM=84MHz/arr/psc
// arr和psc 的取值范围
// 占空比通过TIM_OCInitStructure.TIM_Pulse 属性设置，可以调用库函数TIM_SetCompareX(TIMx, comp)，其中X是定时器通道号，comp是比较值(比arr小)，比如计数器从零向上计数，达到比较值时将将翻转电平
void PWM_Init( u16 arr, u16 psc )
{
	// TIM3
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );  	

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource7, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource0, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource1, GPIO_AF_TIM3 );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频，比如时钟预分频数psc为 79，则驱动计数器的时钟(80M) CK_INT / (79+1)=1M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseStructure.TIM_Period = arr; // 输出PWM频率为freq = psc / arr
	// TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分割并不是分频器的意思，a这是一个采样的频率设置，就像AD的采样一样，在作为定时器捕捉功能时，可以通过设置这个能屏蔽高频干扰
	TIM_TimeBaseInit( TIM3,&TIM_TimeBaseStructure );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init( TIM3, &TIM_OCInitStructure );
	TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable );
 
	TIM_ARRPreloadConfig( TIM3,ENABLE );
	TIM_ClearFlag( TIM3, TIM_FLAG_Update );//清中断标志位

	TIM_Cmd( TIM3, ENABLE );
}
