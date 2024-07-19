#include "car_task.h"
#include "inv_mpu.h"
#include "control.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
int  Encoder_left, Encoder_right;     //检测速度
int  Motor1, Motor2;                  //左右电机PWM值
int  Balance_Pwm,Velocity_Pwm,Turn_Pwm;        //PID计算的PWM值
float Movement = 0;                  //速度调节  
int  Contrl_Turn = 0;                //转向调节变量

/*************************************************************************************************************
*函数名:Task_200HZ()
*功能:运行主频为200Hz的任务
*形参:无
*返回值:无
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
*函数名:Task_100HZ()
*功能:运行主频为100Hz的任务
*形参:无
*返回值:无
**************************************************************************************************************/
void Car_Task_100Hz(void)
{
	Encoder_left = Read_Encoder(&htim2);
	Encoder_right = -Read_Encoder(&htim4);
	
	//1、确定直立环PWM
	
	Balance_Pwm = Vertical_Ring_PD(OutMpu.roll, OutMpu.gyro_x);
	
	//2、确定速度环PWM
	
	Velocity_Pwm = Vertical_speed_PI(Encoder_left,Encoder_right,OutMpu.pitch, Movement );
	
	//3、确定转向环PWM
	
	Turn_Pwm = Vertical_turn_PD(OutMpu.gyro_z,Contrl_Turn);
	
	
	//4、确定最终左右电机的PWM
	Motor1 = Balance_Pwm + Velocity_Pwm + Turn_Pwm;
	Motor2 = Balance_Pwm + Velocity_Pwm - Turn_Pwm;
	
	PWM_Limiting(&Motor1,&Motor2);		//限制电机过冲
	
	//5、设置电机
	//判断小车是否出现特殊状况
	if(Turn_off(OutMpu.roll)==0){
		Set_PWM(Motor1,Motor2);		//输出PWM控制电机
	}
}
