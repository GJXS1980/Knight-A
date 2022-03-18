#ifndef __CONFIG_H_
#define __CONFIG_H_

/**
 * 		电机型号: TETRIX@MAX TorqueNADO DC Motor Specifications
 * 		空载转速: 100rpm
 * 		编码器:		输出轴转1圈，获得1440个计数
 * 		电机驱动板:pwm最小有效脉宽200ns, pwm占空比范围 0.1%~100%(取pwm频率50kHz，)
 * 								电机1						电机2						电机3						电机4
 * 		PWM					PA6							PA7							PB0							PB1
 * 		正反转A			PD10						PD12						PD14						PB14
 * 		正反转B			PD11						PD13						PD15						PB15
 * 		编码器A			PB6							PA0							PA15						PC6
 * 		编码器B			PB7							PA1							PB3							PC7
 */

/**		测距
 * 								超声波1			超声波2			超声波3			超声波4			超声波5			超声波6
 * 		发送					PC13			 PC13				 PC13				PC13			 PC13			 	 PC13
 * 		接收					PE0				 PE1				 PE2				PE3			   PE4			   PE5
 */

/**		防跌落
 * 								红外反射光电管1    红外反射光电管2    红外反射光电管3    红外反射光电管4    红外反射光电管5    红外反射光电管6
 * 		接收					   PE6				       PE7				       PE8				       PE9			        PE10			        PE11
 */

/**		防撞边
 * 								1     2     3     4
 * 		接收				PD6		PD7		PD8		PD9
 */

#define MOTOR1
#define MOTOR2
#define MOTOR3

#ifdef MOTOR1
	#define PID1
	#define PWM1
	#define ENCODER1
#endif

#ifdef MOTOR2
	#define PID2
	#define PWM2
	#define ENCODER2
#endif

#ifdef MOTOR3
	#define PID3
	#define PWM3
	#define ENCODER3
#endif

#ifdef MOTOR4
	#define PID4
	#define PWM4
	#define ENCODER4
#endif

#ifdef PID1
	#define PID1_kp 5
	#define PID1_ki 0.3
	#define PID1_kd 0.2
#endif

#ifdef PID2
	#define PID2_kp 10
	#define PID2_ki 3
	#define PID2_kd 1.5
#endif

#ifdef PID3
	#define PID3_kp 10
	#define PID3_ki 3
	#define PID3_kd 1.5
#endif

#ifdef PID4
	#define PID4_kp 10
	#define PID4_ki 3
	#define PID4_kd 1.5
#endif

// 用于PWM的定时器的重装载值ARR和预分频值PSC
// PWM频率计算：84M / (PWM_ARR + 1) / (PWM_PSC + 1)
#define PWM_ARR (1000 - 1)
#define PWM_PSC (10 - 1)

#endif
