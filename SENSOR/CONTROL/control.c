#include "control.h"

int myabs(int a)
{ 		   
  int temp;
  if(a<0)  temp=-a;  
  else temp=a;
  return temp;
}

void Set_Pwm(int motoA,int motoB,int motoC,int motoD)
{
  if(motoA>0)			AIN2=1,			AIN1=0;
  else 	          AIN2=0,			AIN1=1;
  PWMA=myabs(motoA)*1;
  if(motoB>0)	BIN1=0,			BIN2=1;
  else        BIN1=1,			BIN2=0;
  PWMB=myabs(motoB)*1.0;
  if(motoC>0)	CIN1=0,			CIN2=1;
  else        CIN1=1,			CIN2=0;
  PWMC=myabs(motoC)*1.0;
  if(motoD>0)	DIN1=0,			DIN2=1;
  else        DIN1=1,			DIN2=0;
  PWMD=myabs(motoD)*1.0;
}

void Motor_Direction_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

	// motor A
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  //GPIO_SetBits(GPIOD, GPIO_Pin_10);
  GPIO_ResetBits(GPIOD, GPIO_Pin_10|GPIO_Pin_11);

	// morot B
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//GPIO_SetBits(GPIOD, GPIO_Pin_12);
  GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12);

	// morot C
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//GPIO_SetBits(GPIOD, GPIO_Pin_14);
  GPIO_ResetBits(GPIOD, GPIO_Pin_14|GPIO_Pin_15);

	// morot D
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIO_SetBits(GPIOB, GPIO_Pin_14);
  GPIO_ResetBits(GPIOB, GPIO_Pin_14|GPIO_Pin_15);
}
