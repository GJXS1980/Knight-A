/**
 * @Description: commands of uart
 * @Author: Jim Long
 * @Date: 2019-01-26 09:13:53
 * @LastEditTime: 2019-03-26 18:58:37
 * @LastEditors: Please set LastEditors
 */
#ifndef __COMMAND_H
#define __COMMAND_H
#include "sys.h"

void Get_Command(u8* buf, u16 size);
extern float Motor_Rev_Setpoint1;
extern float Motor_Rev_Setpoint2;
extern float Motor_Rev_Setpoint3;
extern float Motor_Rev_Setpoint4;

extern float Motor_Turn_Duration1;
extern float Motor_Turn_Duration2;
extern float Motor_Turn_Duration3;
extern float Motor_Turn_Duration4;
#endif
