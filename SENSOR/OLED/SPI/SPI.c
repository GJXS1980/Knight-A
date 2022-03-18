#include "SPI.h"

#define mode0 0
#define mode1 0
#define mode2 0
#define mode3 1


void MySPI_Init(void)	
{	
	GPIO_InitTypeDef  GPIO_InitStructrue;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

// PD4 spi_clk
// PD5 spi_mosi
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;			
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructrue.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructrue.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE,&GPIO_InitStructrue);
	

//模拟模式0
#if mode0     	
	SPI_CLK_L();		//CPOL==0 CPHA==0	
	OLED_CS_H();
#endif
	
//模拟模式1	
#if mode1 
	SPI_CLK_L();		//CPOL==0 CPHA==1	
	OLED_CS_H();
#endif
	
//模拟模式2
#if mode2 
	SPI_CLK_H();		//CPOL==1 CPHA==0	
	OLED_CS_H();
#endif

//模拟模式3	
#if mode3
	SPI_CLK_H();		//CPOL==1 CPHA==1	
	OLED_CS_H();
#endif

}

//模拟四种模式
void OLED_WR_Byte(u8 data,u8 cmd)
{
	int32_t i;
	
	if(cmd)
		OLED_DC_H();		//发送数据
	else 
		OLED_DC_L();		//发送指令
	
	OLED_CS_L();			//拉低片选引脚发送数据

#if mode0
	
	for(i=7;i>=0;i--)
	{			  
		SPI_CLK_L();
		
		if(data&(1<<i))
		   SPI_MOSI_H();
		else 
		   SPI_MOSI_L();
		
		SPI_CLK_H(); 
	}
	
	SPI_CLK_L();
	
#endif	

#if mode1
	
	SPI_MOSI_L();
	for(i=7;i>=0;i--)
	{			  		
		SPI_CLK_H();
		
		if(data&(1<<i))
		   SPI_MOSI_H();
		else 
		   SPI_MOSI_L();
		
		SPI_CLK_L(); 
	}
	
	SPI_CLK_H();	

//	SPI_CLK_H();   ???
	
#endif	
	
#if mode2
	
	for(i=7;i>=0;i--)
	{			  		
		SPI_CLK_H();
		
		if(data&(1<<i))
		   SPI_MOSI_H();
		else 
		   SPI_MOSI_L();
		
		SPI_CLK_L(); 
	}
	
	SPI_CLK_H();
	
#endif	

#if mode3

	SPI_CLK_H();	//高字节先发送
	for(i=7;i>=0;i--)
	{			  
		SPI_CLK_L();
		
		if(data&(1<<i))
		   SPI_MOSI_H();
		else 
		   SPI_MOSI_L();
		
		SPI_CLK_H(); 
	}
		
#endif	
	
	OLED_CS_H();
	OLED_DC_H();

}



