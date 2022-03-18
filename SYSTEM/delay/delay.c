#include "delay.h"
#include "sys.h"

//使用外部8MHZ晶振时钟   168/8 = 21 MHZ
void delay_ms(u32 t)
{
	u32 temp = 0;
	while(t--)
	{	
		SysTick->CTRL = 0;
		SysTick->LOAD = 21000 - 1;
		SysTick->VAL = 0;
		
		SysTick->CTRL = 1;
		do
		{
			temp = SysTick->CTRL ;
		}
		while(temp&0x01 && !(temp&(1<<16)));		//temp&0x01 防止定时器关闭，死循环
		
		SysTick->CTRL = 0;
	}
	
}	


void delay_us(u32 t)
{
	u32 temp = 0;
	
	SysTick->CTRL = 0;
	SysTick->LOAD = 21*t-1;
	SysTick->VAL = 0;
	
	SysTick->CTRL = 1;
	do
	{
		temp = SysTick->CTRL ;
	}
	while(temp&0x01 && !(temp&(1<<16)));		//temp&0x01 防止定时器关闭 CTRL的第16位等于1时溢出
	
	SysTick->CTRL = 0;

}






