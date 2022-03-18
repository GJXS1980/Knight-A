#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#include "pwm.h"
#define PWMA   TIM3->CCR1 // A6
#define PWMB   TIM3->CCR2 // A7
#define PWMC   TIM3->CCR3 // B0
#define PWMD   TIM3->CCR4 // B1

#define AIN1   PDout(10)
#define AIN2   PDout(11)
#define BIN1   PDout(12)
#define BIN2   PDout(13)
#define CIN1   PDout(14)
#define CIN2   PDout(15)
#define DIN1   PBout(14)
#define DIN2   PBout(15)
void Motor_Direction_Init(void);
void Set_Pwm(int motoA,int motoB,int motoC,int motoD);
#endif
