#include "adc.h"
#include "delay.h"		 
#include "usart.h"

//volatile u16 bat_vol = 0;
volatile u16 bat_vol[2] = {0};
extern volatile u8 DROP_FLAG;

//***************************************方法1**********************************************

void  BAT_DMA_Init(void)
{    
	 DMA_InitTypeDef  DMA_InitStructure;
	
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);					//DMA2时钟使能 
			
	  while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}					//等待DMA可配置 
		
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  						//通道选择
	  DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&ADC1->DR);		//DMA外设地址
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&bat_vol;			//DMA 存储器0地址
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;				//存储器到外设模式
	  DMA_InitStructure.DMA_BufferSize = 2;												//数据传输量 
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设非增量模式
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//存储器增量模式
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						// 使用循环模式 
	  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//中等优先级
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;			//存储器突发单次传输
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//外设突发单次传输
	  DMA_Init(DMA2_Stream0, &DMA_InitStructure);							//初始化DMA Stream
		  
	  DMA_Cmd(DMA2_Stream0,ENABLE);	 
	
	  BAT_ADC_Init();		  
}


void BAT_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;											// 独立ADC模式
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;										// 时钟为fpclk x分频	84/4 = 21MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;				// 禁止DMA直接访问模式	
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 	// 采样时间间隔	
	ADC_CommonInit(&ADC_CommonInitStructure);


	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;											// ADC 分辨率
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 																// 禁止扫描模式，多通道采集才需要	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 													// 连续转换	
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//禁止外部边沿触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;				//外部触发通道，本例子使用软件触发，此值随便赋值即可
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;											//数据右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 2;         													//转换通道 个                           
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_112Cycles);  //TOUT = 113T + 11.5T  T = 1/21	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 2, ADC_SampleTime_480Cycles);
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);															// 使能DMA请求 after last transfer (Single-ADC mode)
	ADC_DMACmd(ADC1, ENABLE);																											// 使能ADC DM
	ADC_Cmd(ADC1, ENABLE);  																											// 使能ADC

	ADC_SoftwareStartConv(ADC1);	

}	

void Get_DMA_distance(void)
{
	
	double  an_bat = 0.0;
	an_bat = bat_vol[1];
	
	an_bat = an_bat*3.3/4096;					//参考电压3.3v 转换 
																		//带入距离转换公式 分段函数							
	if(an_bat < 2.7500f)
		DROP_FLAG = 1;
	else 
		DROP_FLAG = 0;

}

//***************************************方法2**********************************************
void  Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// PA2 电池电压, 对应ADC通道2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	//红外测距模拟量
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 4096
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
}				  

u16 Get_Adc(u8 ch)   
{
	  	
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	
  
	ADC_SoftwareStartConv(ADC1);		
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	return ADC_GetConversionValue(ADC1);	
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 

void Get_distance(u8 ch)
{
	double  an_bat = 0;

	an_bat = Get_Adc_Average(ch,1);
	an_bat = an_bat*3.3/4096;					//参考电压3.3v 转换 
												//带入距离转换公式 分段函数							
	//distance = ???; 
	if(an_bat < 2.700f)
		DROP_FLAG = 1;
	else 
		DROP_FLAG = 0;
	
//	printf("AN_bat:%.3f\r\n",an_bat);
	
} 




