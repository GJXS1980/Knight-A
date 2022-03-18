#include "timer.h"
#include "encoder.h"
#include "usart.h"
#include "pwm.h"
#include "command.h"
#include <stdlib.h>
#include "iwdg.h"
#include "control.h"
#include "stop.h"
#include "usart.h"
#define ARR 10000-1
#define PSC 168-1

// 采集频率 10ms
// freq = psc / arr
#define FREQ ( 168000000 / (ARR) / (PSC) )

extern u8 STOP_FLAG;

float VelocityA_Kp = 1.0201f, VelocityA_Ki = 0.00014f, VelocityA_Kd = 0.00205f; // pid参数
//float VelocityA_Kp = 70.0f, VelocityA_Ki = 5.0f, VelocityA_Kd = 0.0f; // pid参数
float pre_encoderA = 0;    // 上一帧编码器
float encoderA = 0;        // 编码器
float VelocityA = 0;       // 输出
float VelocityA_Er_pre = 0;// 上一帧误差
float VelocityA_Er = 0;    // 误差
float VelocityA_tar = 0;   // 目标值
float VelocityA_cur = 0;   // 当前值
float VelocityA_Int = 0;   // 积分

s32 Turn_DurationA = 0;    //持续时间

float VelocityB_Kp =  1.0201f, VelocityB_Ki =0.00014f, VelocityB_Kd = 0.00205f; 
float pre_encoderB = 0; 
float encoderB = 0; 
float VelocityB = 0; 
float VelocityB_Er_pre = 0; 
float VelocityB_Er = 0; 
float VelocityB_tar = 0; 
float VelocityB_cur = 0; 
float VelocityB_Int = 0; 

s32 Turn_DurationB = 0;

float VelocityC_Kp =  1.0201f, VelocityC_Ki = 0.00014f, VelocityC_Kd = 0.00205f; 
float pre_encoderC = 0; 
float encoderC = 0; 
float VelocityC = 0; 
float VelocityC_Er_pre = 0; 
float VelocityC_Er = 0; 
float VelocityC_tar = 0; 
float VelocityC_cur = 0; 
float VelocityC_Int = 0; 

s32 Turn_DurationC = 0;

float VelocityD_Kp =  1.0201f, VelocityD_Ki = 0.00014f, VelocityD_Kd = 0.00205f; 
float pre_encoderD = 0; 
float encoderD = 0; 
float VelocityD = 0; 
float VelocityD_Er_pre = 0; 
float VelocityD_Er = 0; 
float VelocityD_tar = 0; 
float VelocityD_cur = 0; 
float VelocityD_Int = 0; 

s32 Turn_DurationD = 0;

void TIM1_IRQ_Config(void) 
{
	TIM_ITConfig( TIM1, TIM_IT_Update, ENABLE );
}

// 中断优先级配置
void TIM1_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}

// freq_PWM=168MHz/ARR/Period
void TIM1_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = PSC; // 时钟预分频数为 168-1，则驱动计数器的时钟(168M) CK_INT / (168)=1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseStructure.TIM_Period = ARR; // freq = psc / arr
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // 时钟不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 重复计数设置 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM1, TIM_FLAG_Update); // 清中断标志位
	TIM_ARRPreloadConfig( TIM1,ENABLE );
	TIM_Cmd( TIM1, ENABLE );
}

// 中断服务程序
void TIM1_UP_TIM10_IRQHandler (void)
{
	if ( TIM_GetITStatus( TIM1, TIM_IT_Update ) != RESET ) 
	{

	  IWDG_Feed();						//定时器中断喂狗

		// 急停
		if (STOP) 
		{
			STOP_FLAG = 1;
			STOP_LED_L;
			RUN_LED_H;
		}
		else
		{
			STOP_FLAG = 0;
			STOP_LED_H;
			RUN_LED_L;
		}

		// motorA
		encoderA = GET_ENCODERA; // 读取编码器值
		VelocityA_cur = encoderA - pre_encoderA;// 当前转速
		pre_encoderA = encoderA; // 保存编码器值
		VelocityA_Er = VelocityA_tar - VelocityA_cur;
		VelocityA_Int += VelocityA_Er;
		if(VelocityA_Int>10000)  	VelocityA_Int=10000; // 限幅
		if(VelocityA_Int<-10000)  	VelocityA_Int=-10000; 
		VelocityA = VelocityA_Er*VelocityA_Kp+VelocityA_Int*VelocityA_Ki+(VelocityA_Er-VelocityA_Er_pre)*VelocityA_Kd;
		VelocityA_Er_pre = VelocityA_Er;

		if(VelocityA>999)VelocityA = 999;
		else if(VelocityA<-999)VelocityA = -999;

		// motorB
		encoderB = GET_ENCODERB; 
		VelocityB_cur = encoderB - pre_encoderB;
		pre_encoderB = encoderB;
		VelocityB_Er = VelocityB_tar - VelocityB_cur;
		VelocityB_Int += VelocityB_Er;
		if(VelocityB_Int>10000)  	VelocityB_Int=10000;
		if(VelocityB_Int<-10000)  	VelocityB_Int=-10000; 
		VelocityB = VelocityB_Er*VelocityB_Kp+VelocityB_Int*VelocityB_Ki+(VelocityB_Er-VelocityB_Er_pre)*VelocityB_Kd;
		VelocityB_Er_pre = VelocityB_Er;

		if(VelocityB>999)VelocityB = 999;
		else if(VelocityB<-999)VelocityB = -999;

		// motorC
		encoderC = GET_ENCODERC; 
		VelocityC_cur = encoderC - pre_encoderC;
		pre_encoderC = encoderC;
		VelocityC_Er = VelocityC_tar - VelocityC_cur;
		VelocityC_Int += VelocityC_Er;
		if(VelocityC_Int>10000)  	VelocityC_Int=10000;
		if(VelocityC_Int<-10000)  	VelocityC_Int=-10000;
		VelocityC = VelocityC_Er*VelocityC_Kp+VelocityC_Int*VelocityC_Ki+(VelocityC_Er-VelocityC_Er_pre)*VelocityC_Kd;
		VelocityC_Er_pre = VelocityC_Er;

		if(VelocityC>999)VelocityC = 999;
		else if(VelocityC<-999)VelocityC = -999;
		
		// motorD
		encoderD = GET_ENCODERD; 
		VelocityD_cur = encoderD - pre_encoderD;
		pre_encoderD = encoderD;
		VelocityD_Er = VelocityD_tar - VelocityD_cur;
		VelocityD_Int += VelocityD_Er;
		if(VelocityD_Int>10000)  	VelocityD_Int=10000;
		if(VelocityD_Int<-10000)  	VelocityD_Int=-10000;
		VelocityD = VelocityD_Er*VelocityD_Kp+VelocityD_Int*VelocityD_Ki+(VelocityD_Er-VelocityD_Er_pre)*VelocityD_Kd;
		VelocityD_Er_pre = VelocityD_Er;

		if(VelocityD>999)VelocityD = 999;
		else if(VelocityD<-999)VelocityD = -999;

		if (Turn_DurationA>0) Turn_DurationA--;
		else 
		{	VelocityA_tar = 0;
			VelocityA = 0;
		}
		if (Turn_DurationB>0) Turn_DurationB--;
		else 
		{	
			VelocityB_tar = 0;
			VelocityB = 0;
		}
		if (Turn_DurationC>0) Turn_DurationC--;
		else 
		{
			VelocityC_tar = 0;
			VelocityC = 0;
		}
		if (Turn_DurationD>0) Turn_DurationD--;
		else 
		{	
			VelocityD_tar = 0;
			VelocityD = 0;
		}	

		if (STOP_FLAG ) 
		{
			Set_Pwm(0, 0, 0, 0);

			VelocityA_tar = 0;
			VelocityA = 0;
			VelocityA_Er_pre = 0;
			VelocityA_Er = 0;
			VelocityA_cur = 0;
			VelocityA_Int = 0;

			VelocityB_tar = 0;
			VelocityB = 0;
			VelocityB_Er_pre = 0;
			VelocityB_Er = 0;
			VelocityB_cur = 0;
			VelocityB_Int = 0;

			VelocityC_tar = 0;
			VelocityC = 0;
			VelocityC_Er_pre = 0;
			VelocityC_Er = 0;
			VelocityC_cur = 0;
			VelocityC_Int = 0;
			
			VelocityD_tar = 0;
			VelocityD = 0;
			VelocityD_Er_pre = 0;
			VelocityD_Er = 0;
			VelocityD_cur = 0;
			VelocityD_Int = 0;
		}
		else
		{
			Set_Pwm(VelocityA, VelocityB, VelocityC, VelocityD);
			
//			printf("tar:%f\r\n",VelocityA_tar);
//			printf("cur:%f\r\n",VelocityA_cur);
//			printf("PWM:%f\r\n",VelocityA);
//			printf("\r\n");
		}
		
    TIM_ClearITPendingBit( TIM1, TIM_FLAG_Update );
	}
}










