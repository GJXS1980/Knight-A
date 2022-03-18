#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 

//extern volatile u16 bat_vol;
extern volatile u16 bat_vol[2];

void BAT_DMA_Init(void);
void BAT_ADC_Init(void);
void Get_DMA_distance(void);

void Adc_Init(void);
u16  Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
void Get_distance(u8 ch);

#endif 
