#include "car_task.h"
#include "inv_mpu.h"
#include "control.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
int  Encoder_left, Encoder_right;     //����ٶ�
int  Motor1, Motor2;                  //���ҵ��PWMֵ
int  Balance_Pwm,Velocity_Pwm,Turn_Pwm;        //PID�����PWMֵ
float Movement = 0;                  //�ٶȵ���  
int  Contrl_Turn = 0;                //ת����ڱ���

/*************************************************************************************************************
*������:Task_200HZ()
*����:������ƵΪ200Hz������
*�β�:��
*����ֵ:��
*************************************************************************************************************/
extern struct mpu6050_data OutMpu;
void Car_Task_200Hz(void)
{
	static struct mpu6050_data Last_Data;
	
	if(mpu_dmp_get_data()!=0)
		OutMpu = Last_Data;
	else
		Last_Data = OutMpu;
}

/**************************************************************************************************************
*������:Task_100HZ()
*����:������ƵΪ100Hz������
*�β�:��
*����ֵ:��
**************************************************************************************************************/
void Car_Task_100Hz(void)
{
	Encoder_left = Read_Encoder(&htim2);
	Encoder_right = -Read_Encoder(&htim4);
	
	//1��ȷ��ֱ����PWM
	
	Balance_Pwm = Vertical_Ring_PD(OutMpu.roll, OutMpu.gyro_x);
	
	//2��ȷ���ٶȻ�PWM
	
	Velocity_Pwm = Vertical_speed_PI(Encoder_left,Encoder_right,OutMpu.pitch, Movement );
	
	//3��ȷ��ת��PWM
	
	Turn_Pwm = Vertical_turn_PD(OutMpu.gyro_z,Contrl_Turn);
	
	
	//4��ȷ���������ҵ����PWM
	Motor1 = Balance_Pwm + Velocity_Pwm + Turn_Pwm;
	Motor2 = Balance_Pwm + Velocity_Pwm - Turn_Pwm;
	
	PWM_Limiting(&Motor1,&Motor2);		//���Ƶ������
	
	//5�����õ��
	//�ж�С���Ƿ��������״��
	if(Turn_off(OutMpu.roll)==0){
		Set_PWM(Motor1,Motor2);		//���PWM���Ƶ��
	}
}
