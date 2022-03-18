#include "command.h"
#include "encoder.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>
#include "mpu9250.h"
#include "urf02.h"
#include "adc.h"
#include "hs485hb.h"
#include "inv_mpu.h"
#include "delay.h"
#include "rtc.h"


extern volatile u8 STOP_FLAG;
extern volatile u8 DROP_FLAG;
extern volatile u8 GUARD_DROP_ENABLE;
extern volatile u8 GUARD_ULTRA_ENABLE;
extern volatile u8 GUARD_MPU9250_ENBLE;
extern float pitch,roll,yaw; 	//欧拉角
extern volatile u8 date_up;
extern volatile u8 mpu_outtime;
/**
 * @description: 与上位机通信
 * @param {u8*} buf  接收字符
 * @param {u16} size 字符长度 
 * @return: void
 **/
void Get_Command(u8* buf, u16 size) 
{
  u16 i,j;
  float encoder_A = GET_ENCODERA, encoder_B = 0.00, encoder_C = GET_ENCODERC, encoder_D = GET_ENCODERD;
	
//	printf("Debug DATA:%s size:%d\r\n",buf,size);	
	
  if(buf[0]=='A'&&buf[1]=='L'&&buf[2]=='L' && size==3)
  {
	 printf("%f\t%f\t%f\t%d\t%d\t%d\t%d\t%d\r\n", yaw,URF_DIST[0], URF_DIST[1],DROP_FLAG,(s32)encoder_A,  (s32)encoder_B, (s32)encoder_C, (s32)encoder_D); 
  } 
  // 读取电机编码器,最后一个数值是急停
  else if (buf[0] == 'e' && size == 1) 
  {
    printf("%d\t%d\t%d\t%d\t%d\r\n", (s32)encoder_A,  (s32)encoder_B, (s32)encoder_C, (s32)encoder_D, (u8)STOP_FLAG);
  }
  // 读取9轴姿态传感器
  else if (buf[0] == 'g' && size == 1) 
  {
	printf("%f\t%f\t%f\r\n",roll,pitch ,yaw); 	//欧拉角;
  }
  //查询电源电量 满电为12.6v
  else if (buf[0] == 'b' && size == 1) 
  {

	  double bat_out = 0;
	  
	  bat_out = bat_vol[0];
	  	
	  bat_out = bat_out*3.3*(3.9f+1.0f)/4096; 
	  
	 if((bat_out - 12.0) > 2.0)		//将总电压分为4096份，占其中的多少份（尺子原理）
		printf("1.000\r\n");
	 else
		printf("%.3f%\r\n",(bat_out - 12.0)/2.0);

	    
  }
   // 设置电机速度
  else if (buf[0] == 'm') 
  {
    char motor_rev[4][32];
    u32 motor_rev_len[4];
    char duration[32];
    memset(motor_rev[0], 0, sizeof motor_rev[0]);
    memset(motor_rev[1], 0, sizeof motor_rev[1]);
    memset(motor_rev[2], 0, sizeof motor_rev[2]);
    memset(motor_rev[3], 0, sizeof motor_rev[3]);
    memset(duration, 0, sizeof duration);
    motor_rev_len[0] = 0;
    motor_rev_len[1] = 0;
    motor_rev_len[2] = 0;
    motor_rev_len[3] = 0;
    u32 duration_len = 0;
    u8 n = 0;
    for(u16 i = 1; i < size; i++)
    {
      if (buf[i] == '(') {
        for(u16 j = i + 1; j < size; j++)
        {
          if (buf[j] == ')') {
            if (n>3) {
              duration_len = j-i-1;
            }
            else
            {
              motor_rev_len[n] = j-i-1;
            }
            i = j;
            n++;
            break;
          }
          if (n>3) {
            duration[j-i-1] = buf[j];
          }
          else
          {
            motor_rev[n][j-i-1] = buf[j];
          }
        }
      }
    }
    if ((motor_rev_len[0] > 0) && (motor_rev_len[1] > 0) && (motor_rev_len[2] > 0) && duration_len>0) 
	{
     // 设置电机目标转速及持续时间
 //   VelocityA_tar = atof(motor_rev[0])*31.88f/60.0f;  		// 1440 * 1.5 / 100 = 21.6  //VelocityA_tar 的单位是,每秒多少转
      VelocityA_tar = atof(motor_rev[0])*(1440/60);   
	  VelocityB_tar = atof(motor_rev[1])*(1440/60); 
	  VelocityC_tar = atof(motor_rev[2])*(1440/60);				//*31.88f/60.0f;
      VelocityD_tar = atof(motor_rev[3])*(1440/60);				//*31.88f/60.0f;
      Turn_DurationA = atoi(duration);
      Turn_DurationB = atoi(duration);
      Turn_DurationC = atoi(duration);
      Turn_DurationD = atoi(duration);
      printf("OK\r\n");
    }
  }
  // 编码器清零
  else if (buf[0] == 'r' && size == 1)
  {
    motor_circle_A = 0;
    motor_circle_C = 0;
    motor_circle_D = 0;
    TIM4->CNT = 0;
    TIM5->CNT = 0;
    TIM2->CNT = 0;
    TIM8->CNT = 0;

    pre_encoderA = 0;
    encoderA = 0;
    VelocityA = 0;
    VelocityA_Er_pre = 0;
    VelocityA_Er = 0;
    VelocityA_cur = 0;
    VelocityA_Int = 0;

    pre_encoderC = 0;
    encoderC = 0;
    VelocityC = 0;
    VelocityC_Er_pre = 0;
    VelocityC_Er = 0;
    VelocityC_cur = 0;
    VelocityC_Int = 0;

    pre_encoderD = 0;
    encoderD = 0;
    VelocityD = 0;
    VelocityD_Er_pre = 0;
    VelocityD_Er = 0;
    VelocityD_cur = 0;
    VelocityD_Int = 0;
    			
    printf("OK\r\n");
  }
   // 读取超声波,防跌落，防碰撞
  else if (buf[0] == 'u' && size == 1) 
  {
    printf("%.3f\t%.3f\t%d\r\n", URF_DIST[0], URF_DIST[1],DROP_FLAG);
  }

// 设置pid
  else if (buf[0] == 'p') 
  {
	 u8 motor_no_len = 0;
    u32 pid_p_len = 0;
    u32 pid_i_len = 0;
    u32 pid_d_len = 0; 
    char motor_no[8];
    char pid_p[32];
    char pid_i[32];
    char pid_d[32];
    memset(motor_no, 0, sizeof motor_no);
    memset(pid_p, 0, sizeof pid_p);
    memset(pid_i, 0, sizeof pid_i);
    memset(pid_d, 0, sizeof pid_d);
    
    for(i = 1; i < size; i++)
    {
      if (buf[i] == '(') 
	  {
        for(j = i + 1; j < size; j++)
        {
          if (buf[j] == ')') 
		  {
            if (motor_no_len == 0) 
		    {
              motor_no_len = j-i-1;
            }
            else if (pid_p_len == 0) 
			{
              pid_p_len = j-i-1;
            }
            else if (pid_i_len == 0)
            {
              pid_i_len = j-i-1;
            }
            else if (pid_d_len == 0)
            {
              pid_d_len = j-i-1;
            }
            i = j;
            break;
          }
          if (motor_no_len == 0) 
		  {
            motor_no[j-i-1] = buf[j];
          }
          else if (pid_p_len == 0) 
		  {
            pid_p[j-i-1] = buf[j];
          }
          else if (pid_i_len == 0)
          {
            pid_i[j-i-1] = buf[j];
          }
          else if (pid_d_len == 0)
          {
            pid_d[j-i-1] = buf[j];
          }
        }
      }
    }
    if ((motor_no_len > 0) && (pid_p_len > 0) && (pid_i_len > 0) && (pid_d_len > 0)) 
	{
      switch (atoi(motor_no))
      {
        case 1:
          VelocityA_Kp = atof(pid_p);
          VelocityA_Ki = atof(pid_i);
          VelocityA_Kd = atof(pid_d);
          break;
        case 2:
          VelocityC_Kp = atof(pid_p);
          VelocityC_Ki = atof(pid_i);
          VelocityC_Kd = atof(pid_d);
          break;
        case 3:
          VelocityD_Kp = atof(pid_p);
          VelocityD_Ki = atof(pid_i);
          VelocityD_Kd = atof(pid_d);
          break;
        default:
          break;
      }
      printf("OK\r\n");
    }
  }
  //选择开启或关闭防触边、防跌落和超声波传感器
  else if (buf[0] == 'G'&& size == 7)
  {
    char guard_drop[8]={0};
	char guard_ultrasonic[8]={0};	
  
    u8 guard_drop_len = 0;
    u8 guard_ultra_len = 0; 

	  
    for(i = 1; i < size; i++)
    {
      if (buf[i] == '(') 
	 {
        for(j = i + 1; j < size; j++)
        {
			if (buf[j] == ')') 
			{
				if (guard_drop_len == 0) 
				{
					guard_drop_len = j-i-1;
				}
				else if(guard_ultra_len == 0)
				{
					guard_ultra_len = j-i-1;
				}
				i = j;
				break;
			}
			if (guard_drop_len == 0) 
			{
				guard_drop[j-i-1] = buf[j];
			}
			else if(guard_ultra_len == 0)
			{
				guard_ultrasonic[j-i-1]=buf[j];
			}
        }
      }
    }
	
    if (( guard_ultra_len> 0) && (guard_drop_len > 0)) 
	{
      if (atoi(guard_drop) == 1)
      {
        GUARD_DROP_ENABLE = 1;
      }
      else
      {
        GUARD_DROP_ENABLE = 0;
		DROP_FLAG = 0;  
      }
        
	  if(atoi(guard_ultrasonic) == 1)
	  {
		  GUARD_ULTRA_ENABLE=1;
	  }
	  else
	 {
		 GUARD_ULTRA_ENABLE=0;
		 URF_DIST[0]=0;
		 URF_DIST[1]=0;
     } 
	 
      printf("OK\r\n");
    }
  }
  else if(buf[0]=='c'&& size ==1)		//软件复位
  {
		NVIC_SystemReset();		
  }
  else if(buf[0]=='H')					//舵机
  {
	  int Angle = 0; 
	  char Angle_BUFF[20] = {0};  
 
	  if(buf[1] =='(' && buf [size-1] == ')' && size>3)
	  {	  
		  for(i=2; i<size-1; i++)
		  {		  
			Angle_BUFF[i-2] = buf[i];	
		  }  
		  Angle = atoi(Angle_BUFF);
		  
		  Set_HS485HB_Rotation(Angle);
		  printf("OK\r\n");   
	  }
  }
  else if(buf[0]=='M'&&buf[1]=='P'&&buf[2]=='U' && size==6)
  {
	if(buf[size-2] == '1')
	{
		GUARD_MPU9250_ENBLE = 1;
		mpu_outtime = 10;
		while(mpu_dmp_init() && mpu_outtime--)
		{ 
				delay_ms(10);
		}
	}
	else 
	{
		GUARD_MPU9250_ENBLE = 0;
		pitch = 0,roll = 0,yaw = 0; 	//欧拉角	
	}
	
	 printf("OK\r\n"); 
  }
  else if(buf[0]=='T' && size == 13)
  {
	int i ; 
	char time_buf[10] = {0};
	u8 year,month,date, week,hour,min,sec;
	
	for(i=1;i<size;i++)
	{	
		if(i%2)											//基数重新接收
		{
			memset(time_buf,0,sizeof(time_buf));
			time_buf[0] = buf[i];
		}
		else 
			time_buf[1] = buf[i];
		
		if(i%2 == 0)											//偶数接收完成
		{
			switch(i)
			{
				case 2: 	year = atoi(time_buf);break;
				case 4: 	month = atoi(time_buf);break;
				case 6: 	date = atoi(time_buf);break;
				case 8: 	week = atoi(time_buf)+1;break;
				case 10: 	hour = atoi(time_buf);break;
				case 12: 	min = atoi(time_buf);break;
				default:	break;
			}	
		}	
	}
	 
	sec = 0;											//秒未使用，默认0
	 
    RTC_Set_Date(year,month,date,week);

	if(hour>12)
		RTC_Set_Time(hour,min,sec,RTC_H12_PM);				//下午
	else
		RTC_Set_Time(hour,min,sec,RTC_H12_AM);				//早上
  
	//printf("year:%d month:%d date:%d week:%d \n",year,month,date,week);
	
	date_up = 0;
	
	printf("OK\r\n"); 
  }
  
}


