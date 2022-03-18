#ifndef _SPI_H
#define _SPI_H
#include "sys.h"

//-----------------OLED�˿ڶ���----------------  
//CS Ƭѡ����
#define  	OLED_CS_H()   		GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define  	OLED_CS_L()   		GPIO_ResetBits(GPIOE,GPIO_Pin_0)
//DC ���ݺ�ָ�� ʹ��
#define 	OLED_DC_H()			GPIO_SetBits(GPIOE,GPIO_Pin_1)
#define 	OLED_DC_L()			GPIO_ResetBits(GPIOE,GPIO_Pin_1)
//RES ��λ
#define 	OLED_RESET_H() 		GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define 	OLED_RESET_L()		GPIO_ResetBits(GPIOE,GPIO_Pin_2)

//���ݴ���
#define 	SPI_MOSI_H()		GPIO_SetBits(GPIOE,GPIO_Pin_3)
#define 	SPI_MOSI_L()		GPIO_ResetBits(GPIOE,GPIO_Pin_3)

//ʱ��
#define 	SPI_CLK_H()			GPIO_SetBits(GPIOE,GPIO_Pin_4)
#define 	SPI_CLK_L()			GPIO_ResetBits(GPIOE,GPIO_Pin_4)



void MySPI_Init(void);
void OLED_WR_Byte(u8 data,u8 cmd);

#endif

