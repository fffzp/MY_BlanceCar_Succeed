#ifndef __CAR_TASK_H_
#define __CAR_TASK_H_


extern int  Encoder_left, Encoder_right;     //检测速度
extern int  Motor1, Motor2;                  //左右电机PWM值
extern int  Balance_Pwm,Velocity_Pwm,Turn_Pwm;        //PID计算的PWM值

void Car_Task_200Hz(void);
void Car_Task_100Hz(void);

#endif
