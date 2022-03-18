#include "wwdg.h"
#include "usart.h"

void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
 
	NVIC_InitTypeDef NVIC_InitStructure;
 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); 				//ʹ�ܴ��ڿ��Ź�ʱ��
	
	WWDG_SetPrescaler(fprer); 											//���÷�Ƶֵ
	WWDG_SetWindowValue(wr); 											//���ô���ֵ
	WWDG_Enable(tr);  											//�������Ź�
	
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  						//���ڿ��Ź��ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;  		//��ռ���ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;					//�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  						//ʹ�ܴ��ڿ��Ź�
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag();													//�����ǰ�����жϱ�־λ
	WWDG_EnableIT();														//������ǰ�����ж�
}



//���ڿ��Ź��жϷ������ 
void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(127); //���贰�ڿ��Ź�ֵ
	WWDG_ClearFlag();//�����ǰ�����жϱ�־λ
	
}
