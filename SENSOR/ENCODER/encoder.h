#ifndef __ENCODER_H
#define __ENCODER_H
#include "sys.h"
extern s32 motor_circle_A;// 
extern s32 motor_circle_B;
extern s32 motor_circle_C;
extern s32 motor_circle_D;
// 电机输出一圈编码器输出的脉冲数
#define CEPR 1440
#define GET_ENCODERA ((float)motor_circle_A * CEPR + TIM4->CNT)
#define GET_ENCODERB ((float)motor_circle_B * CEPR + TIM5->CNT)
#define GET_ENCODERC ((float)motor_circle_C * CEPR + TIM2->CNT)
#define GET_ENCODERD ((float)motor_circle_D * CEPR + TIM8->CNT)
void TIM4_Init(void);
void TIM5_Init(void);
void TIM2_Init(void);
void TIM8_Init(void);
#endif
