#ifndef _SPI_H
#define _SPI_H
#include "sys.h"

//-----------------OLED端口定义----------------  
//CS 片选引脚
#define  	OLED_CS_H()   		GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define  	OLED_CS_L()   		GPIO_ResetBits(GPIOE,GPIO_Pin_0)
//DC 数据和指令 使能
#define 	OLED_DC_H()			GPIO_SetBits(GPIOE,GPIO_Pin_1)
#define 	OLED_DC_L()			GPIO_ResetBits(GPIOE,GPIO_Pin_1)
//RES 复位
#define 	OLED_RESET_H() 		GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define 	OLED_RESET_L()		GPIO_ResetBits(GPIOE,GPIO_Pin_2)

//数据传输
#define 	SPI_MOSI_H()		GPIO_SetBits(GPIOE,GPIO_Pin_3)
#define 	SPI_MOSI_L()		GPIO_ResetBits(GPIOE,GPIO_Pin_3)

//时钟
#define 	SPI_CLK_H()			GPIO_SetBits(GPIOE,GPIO_Pin_4)
#define 	SPI_CLK_L()			GPIO_ResetBits(GPIOE,GPIO_Pin_4)



void MySPI_Init(void);
void OLED_WR_Byte(u8 data,u8 cmd);

#endif

