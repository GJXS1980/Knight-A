#include "encoder.h"
#include "usart.h"
#include "urf02.h"
// stm32f407中定时器1、2、3、4、5、8提供编码器接口模式
// stm32提供了单相计数（只能测速度）和双相计数模式（可测速度&方向），双相模式可以更好地消除毛刺干扰，一般使用双相模式

// === 选用的电机及其编码器参数 ===
// TETRIX MAX TorqueNADO DC Motor Specifications
// Encoder Voltage : 3.3/5 volts DC
// Encoder Type : Hall effect
// Cycles per Revolution (motor shaft) : 6
// Cycles per Revolution (output shaft) : 360
// Countable Events per Revolution (motor shaft) : 24
// Countable Events per Revolution (output shaft) : 1,440 [电机输出轴转1周AB相编码器输出1440个脉冲]

// TIM4 : PB6, PB7
// TIM5 : PA0, PA1
// TIM2 : PA15, PB3
// TIM8 : PC6, PC7

// EC1: CH B
// EC2: CH A

s32 motor_circle_A = 0, motor_circle_B=0, motor_circle_C = 0, motor_circle_D = 0;

// 配置更新中断
void TIM4_IRQ_Config( void ) {
  TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );
}
// 配置定时器中断优先级
void TIM4_NVIC_Config( void )
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 副优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );
}

void TIM4_Init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

  // 使能TIM4通道1和通道2的IO口的时钟
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );

  // 配置引脚PB6和PB7复用为TIM4的通道1和通道2输出
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource6,GPIO_AF_TIM4 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource7,GPIO_AF_TIM4 );

	// 配置GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

  // 使能TIM4时钟
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );

  // 定时器设置
  TIM_TimeBaseInitStructure.TIM_Period = CEPR - 1; // 重装载值
  TIM_TimeBaseInitStructure.TIM_Prescaler = 0; // 预分频
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟不分频
  TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStructure );// 初始化TIM4

  TIM_EncoderInterfaceConfig( TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising ); // AB相编码器模式

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; // 输入通道，由于设置了AB相编码器模式，会占用通道1和通道2两个通道的IO口
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // TI与IC的映射关系
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 不分频
  TIM_ICInitStructure.TIM_ICFilter = 10; // 滤波器值，过滤编码器的毛刺信号
  TIM_ICInit( TIM4, &TIM_ICInitStructure );

  // 清中断标志位
  TIM_ClearFlag( TIM4, TIM_FLAG_Update );
  // 中断配置
  TIM4_NVIC_Config();
  TIM4_IRQ_Config();
 
  TIM_Cmd( TIM4, ENABLE ); // 使能定时器

  // 计数器清零
  TIM_SetCounter( TIM4, 0 );
}

void TIM4_IRQHandler( void )
{
  if( TIM_GetITStatus( TIM4, TIM_IT_Update ) != RESET )
  {
	 //编码器脉冲
    if((TIM4->CR1>>4 & 0x01)==0)		// 计数器向上
      motor_circle_A++;		
    else if((TIM4->CR1>>4 & 0x01)==1)	//计数器向下
      motor_circle_A--;  	
  }
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

void TIM5_IRQ_Config(void) {
  TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
}

void TIM5_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM5_Init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 

  TIM_TimeBaseInitStructure.TIM_Period = CEPR - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler=0;
  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

  TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
  TIM_ClearFlag(TIM5, TIM_FLAG_Update);

  TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 10;
  TIM_ICInit(TIM5, &TIM_ICInitStructure);

  // 清中断标志位
  TIM_ClearFlag( TIM5, TIM_FLAG_Update );
  // 中断配置
  TIM5_NVIC_Config();
  TIM5_IRQ_Config();
 
  TIM_Cmd( TIM5, ENABLE ); // 使能定时器

  // 计数器清零
  TIM_SetCounter( TIM5, 0 );
}

void TIM5_IRQHandler(void)
{
  if( TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET )
  {
    if((TIM5->CR1>>4 & 0x01)==0)
      motor_circle_B++;
    else if((TIM5->CR1>>4 & 0x01)==1)
      motor_circle_B--;
  }
  TIM_ClearITPendingBit(TIM5,TIM_IT_Update); 
}

void TIM2_IRQ_Config(void) {
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

void TIM2_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
  
  TIM_TimeBaseInitStructure.TIM_Period = CEPR - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler=0;
  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);

  TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 10;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  // 清中断标志位
  TIM_ClearFlag( TIM2, TIM_FLAG_Update );
  // 中断配置
  TIM2_NVIC_Config();
  TIM2_IRQ_Config();
 
  TIM_Cmd( TIM2, ENABLE ); // 使能定时器

  // 计数器清零
  TIM_SetCounter( TIM2, 0 );
}

void TIM2_IRQHandler(void)
{
  if( TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET )
  {
    if((TIM2->CR1>>4 & 0x01)==0)
      motor_circle_C++;
    else if((TIM2->CR1>>4 & 0x01)==1)
      motor_circle_C--;
  }
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
}

void TIM8_IRQ_Config(void) {
  TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);
}

void TIM8_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM8_Init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE); 
  
  TIM_TimeBaseInitStructure.TIM_Period = CEPR - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler=0;
  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

  TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);
  TIM_ClearFlag(TIM8, TIM_FLAG_Update);

  TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 10;
  TIM_ICInit(TIM8, &TIM_ICInitStructure);

  // 清中断标志位
  TIM_ClearFlag( TIM8, TIM_FLAG_Update );
  // 中断配置
  TIM8_NVIC_Config();
  TIM8_IRQ_Config();
 
  TIM_Cmd( TIM8, ENABLE ); // 使能定时器

  // 计数器清零
  TIM_SetCounter( TIM8, 0 ); 
}

void TIM8_UP_TIM13_IRQHandler(void)
{
  if( TIM_GetITStatus(TIM8,TIM_IT_Update) != RESET )
  {
    if((TIM8->CR1>>4 & 0x01)==0)
      motor_circle_D++;
    else if((TIM8->CR1>>4 & 0x01)==1)
      motor_circle_D--;
    TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
  }
  
  if ( TIM_GetITStatus( TIM13, TIM_IT_Update ) != RESET ) 
  {
    over_time[1] = 1;
    TIM_ClearITPendingBit( TIM13, TIM_IT_Update );
  }
}
