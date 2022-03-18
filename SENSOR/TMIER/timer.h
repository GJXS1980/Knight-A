#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void TIM1_Init(void);
void TIM1_IRQ_Config(void);
void TIM1_NVIC_Config(void);

extern u8 pe1_flag, pe2_flag, pe3_flag, th1_flag;

extern float ANGLE_Z;

extern float VelocityA_Kp;
extern float VelocityA_Ki;
extern float VelocityA_Kd;

extern float pre_encoderA;
extern float encoderA;
extern float VelocityA_tar;
extern float VelocityA;
extern float VelocityA_Er;
extern float VelocityA_tar;
extern float VelocityA_cur;
extern float VelocityA_Int;
extern float VelocityA_Er_pre;

extern s32 Turn_DurationA;

extern float VelocityB_Kp;
extern float VelocityB_Ki;
extern float VelocityB_Kd;

extern float pre_encoderB;
extern float encoderB;
extern float VelocityB_tar;
extern float VelocityB;
extern float VelocityB_Er;
extern float VelocityB_tar;
extern float VelocityB_cur;
extern float VelocityB_Int;
extern float VelocityB_Er_pre;

extern s32 Turn_DurationB;

extern float VelocityC_Kp;
extern float VelocityC_Ki;
extern float VelocityC_Kd;

extern float pre_encoderC;
extern float encoderC;
extern float VelocityC_tar;
extern float VelocityC;
extern float VelocityC_Er;
extern float VelocityC_tar;
extern float VelocityC_cur;
extern float VelocityC_Int;
extern float VelocityC_Er_pre;

extern s32 Turn_DurationC;

extern float VelocityD_Kp;
extern float VelocityD_Ki;
extern float VelocityD_Kd;

extern float pre_encoderD;
extern float encoderD;
extern float VelocityD_tar;
extern float VelocityD;
extern float VelocityD_Er;
extern float VelocityD_tar;
extern float VelocityD_cur;
extern float VelocityD_Int;
extern float VelocityD_Er_pre;

extern s32 Turn_DurationD;

#endif
