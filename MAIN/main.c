#include "stm32f4xx.h"
#include "config.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "timer.h"
#include "encoder.h"
#include "iwdg.h"
#include "urf02.h"
#include "adc.h"
#include "mpu9250.h"
#include "control.h"
#include "beep.h"
#include "hs485hb.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "command.h"
#include "wwdg.h"
#include "OLED.h"
#include "bmp.h"
#include "stop.h"
#include "rtc.h"
#include "TIM_6_7.h"
#include "string.h"

#define  Filtering_times 	20

volatile u8 UPDATE_FLAG = 1;
volatile u8 STOP_FLAG = 0;              												// 急停开关标志 
volatile u8 DROP_FLAG = 0;																// 防跌落标识
volatile u8 GUARD_DROP_ENABLE   = 0;     												// 是否开启防跌落
volatile u8 GUARD_ULTRA_ENABLE  = 0;         											//是否开启超声波探测
volatile u8 GUARD_MPU9250_ENBLE = 1;
unsigned char tbuf[50] = {0};

float pitch=0.0, roll=0.0, yaw=0.0; 													//欧拉角
volatile u32 count1 = 0;

static	RTC_TimeTypeDef RTC_TimeStruct;
static	RTC_DateTypeDef RTC_DateStruct;

volatile u8 date_up = 0;

float bat_vcc[Filtering_times] = {0};
volatile u8 mpu_outtime = 0;

void BOOT_UI(void)
{
	int i = 3;
	u8 x = 26,j = 0;

	OLED_Clear();	
	Beep_On();	
	while(i--)
	{	
		OLED_ShowPicture(0,0,128,8,BMP2); 
		delay_ms(300);
		OLED_ShowPicture(0,0,128,8,BMP1);
		if(i==1) 	Beep_Off();
		delay_ms(300);
		
	}

	for(i=7;;i--,j++)
	{
		OLED_Clear();	
		if(x*(j-7)>128)
		{
			break;
		}
		switch(i)
		{
			default:
			case 0:  	if(x*(j-7) <128) OLED_ShowChinese(x*(j-7),20,0,24);	
			case 1:  	if(x*(j-6) <128) OLED_ShowChinese(x*(j-6),20,1,24);		
			case 2:  	if(x*(j-5) <128) OLED_ShowChinese(x*(j-5),20,2,24);	
			case 3:		if(x*(j-4) <128) OLED_ShowChinese(x*(j-4),20,3,24);
			case 4:		if(x*(j-3) <128) OLED_ShowChinese(x*(j-3),20,4,24);
			case 5:		if(x*(j-2) <128) OLED_ShowChinese(x*(j-2),20,5,24);
			case 6:		if(x*(j-1) <128) OLED_ShowChinese(x*(j-1),20,6,24);
			case 7:		if(x*j     <128) OLED_ShowChinese(x*j ,20,7,24);
		}	
		OLED_Refresh();
		delay_ms(150);
	}	
	
	for(i=0; i<Filtering_times; i++)
	{
		bat_vcc[i] = bat_vol[0];
	}
}	

float Filtering_Func(void)
{
	int i = 0,j=0;
	int flag;
	float result;
		
	for(i=0; i<Filtering_times-1; i++)
	{
		flag = 1;
		for(j=0; j<Filtering_times-i-1;j++)
		{
			if(bat_vcc[j]>bat_vcc[j+1])
			{
				float temp = bat_vcc[j];
				bat_vcc[j] = bat_vcc[j+1];
				bat_vcc[j+1] = temp;
				flag = 0;
			}	
			
		}
		
		if(flag) break;
	}
	
	for(i=5;i<Filtering_times-5;i++)
	{
		result += bat_vcc[i];
	}

	return result/(Filtering_times-10);	
}	


void oled_ui(void)
{
	// 1 广州慧谷动力科技
	// 2 服务机器人教学平台
	// 3 POWER 电量显示
	// 4 菜单
	// 5 ROS教程
	
	static u8 flag = 1;
	static int count2 = 0;
	u8 i;
	double bat_temp = 0.0;
	static unsigned char buf[20] = {0};	
	static int bat_out = 0;
	static char date_buf[10] = {0};
	
	if(count2%10==0 || count2==0)
	{	
		bat_temp = Filtering_Func();
		bat_temp = bat_temp*3.3*(3.9f+1.0f)/4096; 
		bat_temp = (bat_temp - 12.0)/2.0;
		bat_out = bat_temp*100;
		sprintf((char *)buf,"%d%s",bat_out,"%");	
	}
	count2++;	
	OLED_Clear();
	for(i=0;i<7;i++)
	{
		OLED_ShowChinese((i)*17+4,2,i,16);
	}
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	if(date_up == 0 || date_up != RTC_DateStruct.RTC_WeekDay)
	{	
		date_up = RTC_DateStruct.RTC_WeekDay;
		
		switch(RTC_DateStruct.RTC_WeekDay)
		{
			case 1: 	strcpy(date_buf,"Mon");break;
			case 2:		strcpy(date_buf,"Tue");break;
			case 3: 	strcpy(date_buf,"Wed");break;
			case 4: 	strcpy(date_buf,"Thu");break;
			case 5: 	strcpy(date_buf,"Fri");break;
			case 6: 	strcpy(date_buf,"Sat");break;
			case 7: 	strcpy(date_buf,"Sun");break;	
		}	
	}
	
	sprintf((char *)tbuf,"20%02d-%02d-%02d %s",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,
											   RTC_DateStruct.RTC_Date,date_buf); 
	OLED_ShowString(0,54,tbuf,12);
	sprintf((char *)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
	OLED_ShowString(10,23,tbuf,24);
 
	if(bat_out<25&&flag)
	{	
		OLED_ShowString(95,54,"!",12);
	}

	if(bat_out>99)
	{	
		strcpy((char *)buf,"100%");
		OLED_ShowString(98,54,buf,12);
	}	
	else if(bat_out<0)
	{
		strcpy((char *)buf,"0%");
		OLED_ShowString(104,54,buf,12);
	}	
	else 
		OLED_ShowString(104,54,buf,12);

	OLED_Refresh();
	flag = !flag;
}

int main(void)
{
	int i = 0;
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );							// 设置中断优先级分组方式
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);					//选择外部时钟
	uart_init(115200); 																						//串口初始化
	delay_ms(10);
	Motor_Direction_Init();																				// 初始化电机驱动
	PWM_Init(1000-1,8-1); 																				//10.5kHz 与门，u2,u5,u8 	
	TIM4_Init();																									// 初始化编码器（双相）
	TIM2_Init();
	TIM8_Init();
	delay_ms(10);
	URF_Init();																									//超声波初始化	
	TIM1_Init();																								// 初始化主定时器,处理控制逻辑 内部含有红外线、防止碰撞、急停的定时器中断
	TIM1_IRQ_Config();
	TIM1_NVIC_Config();						
	ScramButton_init();
	Beep_Init();															
	IWDG_Init(4,312); 																					// 初始化独立看门狗0.5s
	TIM9_PWM_Init(40000-1,84-1);
	delay_ms(10);
	OLED_Init();
	BAT_DMA_Init();
	TIM_6_7_INIT(10000,8400);
	My_RTC_Init();
	delay_ms(100);
	MPU_Init();        																					// 初始化陀螺仪 mpu9250，具有延时函数的初始化要在内核启动后初始化						
	mpu_outtime = 10;
	while(mpu_dmp_init()&& mpu_outtime--)
	{ 
		delay_ms(100);
	}
	printf("306\r\n");
	BOOT_UI();	
	oled_ui();

	while(1) 
	{		
		if(g_usart1_event)
		{		
			Get_Command(USART_RX_BUF, USART_RX_STA&0X3FFF);							//滤除最后两位
			USART_RX_STA = 0;														//重新接收
			g_usart1_event = 0;
		}
		else
		{	
			if(GUARD_MPU9250_ENBLE)
			{	
				mpu_mpl_get_data(&pitch,&roll,&yaw);							
			}	
			
			if(GUARD_ULTRA_ENABLE)	
			{
				count1++;
				if(count1 == 200) Read_URF(1);										//读取超声波
				if(count1 >= 300) { Read_URF(2); count1=0; }
			}
			
			if(GUARD_DROP_ENABLE)
			{
				Get_DMA_distance();
			}			
		}
			

		bat_vcc[i%(Filtering_times - 1)] = bat_vol[0];
		if(UPDATE_FLAG)																			//1s刷新一次屏幕
		{
			UPDATE_FLAG = 0;
			oled_ui();
		}
		
		i++;
	}
}

