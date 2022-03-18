#ifndef _STOP_H
#define _STOP_H
#include "sys.h"

#define STOP GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)

#define STOP_LED_H  GPIO_SetBits(GPIOE,GPIO_Pin_13)
#define STOP_LED_L  GPIO_ResetBits(GPIOE,GPIO_Pin_13)

#define RUN_LED_H  GPIO_SetBits(GPIOD,GPIO_Pin_7)
#define RUN_LED_L  GPIO_ResetBits(GPIOD,GPIO_Pin_7)


void ScramButton_init(void);

#endif
