#ifndef __CAR_TASK_H_
#define __CAR_TASK_H_


extern int  Encoder_left, Encoder_right;     //����ٶ�
extern int  Motor1, Motor2;                  //���ҵ��PWMֵ
extern int  Balance_Pwm,Velocity_Pwm,Turn_Pwm;        //PID�����PWMֵ

void Car_Task_200Hz(void);
void Car_Task_100Hz(void);

#endif
