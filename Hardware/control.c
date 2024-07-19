#include "stm32f1xx_hal.h"
#include "control.h"
#include "math.h"
#include "stdlib.h"

extern TIM_HandleTypeDef htim1;			//定时器1 PWM
extern float Movement;                  //速度调节  
extern int  Contrl_Turn;                //转向调节变量

//PID调节参数
struct pid_arg PID = {
	.Balance_Kp= 425,  
	.Balance_Kd= 2.0,
	
	.Velocity_Kp= -57,  
	.Velocity_Ki= -0.28,
	
	.Turn_Kp = 15,
	.Turn_Kd = 0.15,
};


/**************************************************************************************************************
*函数名:Read_Encoder()
*功能:读取编码器值(当作小车当前前进的速度)
*形参:(u8 TIMX):x为编码器1或者2
*返回值:无
*************************************************************************************************************/
int Read_Encoder(TIM_HandleTypeDef *htim)
{
	int temp;
	temp = (short)__HAL_TIM_GetCounter(htim);
	__HAL_TIM_SetCounter(htim,0);
	return temp;
}

/**************************************************************************************************************
*函数名:Set_PWM()
*功能:输出PWM控制电机
*形参；(int motor1):电机1对应的PWM值/(int motor2):电机2对应的PWM值
*返回值:无
*************************************************************************************************************/
void Set_PWM(int motor1,int motor2)
{
	if(motor1<0){
		AIN1(1);			AIN2(0);
	}else{
		AIN1(0),			AIN2(1);
	}
	
	if(motor2<0){
		BIN1(1);			BIN2(0);
	}else{
		BIN1(0),			BIN2(1);
	}
	
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,abs(motor1));
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,abs(motor2));
}

/**************************************************************************************************************
*函数名:PWM_Limiting()
*功能:PWM限幅函数
*形参:无
*返回值:无
***************************************************************************************************************/
void PWM_Limiting(int *motor1,int *motor2)
{
	if(*motor1 < PWM_MIN) *motor1 = PWM_MIN;
	if(*motor1 > PWM_MAX) *motor1 = PWM_MAX;
	if(*motor2 < PWM_MIN) *motor2 = PWM_MIN;
	if(*motor2 > PWM_MAX) *motor2 = PWM_MAX;	
}

/**************************************************************************************************************
*函数名:Turn_off()
*功能:关闭电机
*形参:(const float Angle):x轴角度值
*返回值:1:小车当前处于停止状态/0:小车当前处于正常状态
***************************************************************************************************************/
uint8_t Turn_off(const float angle)
{
	uint8_t temp;
	if(fabs(angle) > 30){
		temp = 1;
		AIN2(0),			AIN1(0);
		BIN1(0),			BIN2(0);
	}
	else{
		temp = 0;
	}
	return temp;
}


/**************************************************************************************************************
*函数名:Vertical_Ring_PD()
*功能:直立环PD控制
*形参:(float Angle):x轴的角度/(float Gyro):x轴的角速度
*返回值:经过PID转换之后的PWM值
**************************************************************************************************************/

int	Vertical_Ring_PD(float Angle,float Gyro)
{
	float Bias;
	int balance;
	
	Bias=Angle-Mechanical_balance;
	balance=PID.Balance_Kp*Bias+ Gyro*PID.Balance_Kd;
	
	return balance;
}
/**************************************************************************************************************
*函数名:Vertical_speed_PI()
*功能；速度环PI控制
*形参:(int encoder_left):左轮编码器值/(int encoder_right):编码器右轮的值/(float Angle):x轴角度值
*返回值:
**************************************************************************************************************/
extern uint8_t stop;
int Vertical_speed_PI(int encoder_left,int encoder_right,float Angle,float Movement )
{
	static float Velocity,Encoder_Least,Encoder;
	static float Encoder_Integral;
	Encoder_Least =(encoder_left+encoder_right)-Movement;   //获取最新速度偏差=测量速度（左右编码器之和）-目标速度（此处为零）
	Encoder *= 0.8f;								 //一阶低通滤波器 ，上次的速度占85%
	Encoder += Encoder_Least*0.2f;                   //一阶低通滤波器， 本次的速度占15% 
	
	Encoder_Integral +=Encoder;                      //积分出位移 积分时间：10ms
//	Encoder_Integral=Encoder_Integral-Movement; 

	if(Encoder_Integral>10000)  	Encoder_Integral=10000;           //积分限幅
	if(Encoder_Integral<-10000)	  Encoder_Integral=-10000;            //积分限幅

	Velocity=Encoder*PID.Velocity_Kp+Encoder_Integral*PID.Velocity_Ki;      //速度控制

	if(Turn_off(Angle)==1)   Encoder_Integral=0;            //电机关闭后清除积分
	return Velocity;
}
/**************************************************************************************************************
*函数名:Vertical_turn_PD()
*功能:转向环PD
*形参:无  gyro_z轴陀螺仪数值,角度值
*返回值:无
***************************************************************************************************************/
int Vertical_turn_PD(float gyro_z,int Target_turn)
{
	float Turn;     
	Turn=Target_turn*PID.Turn_Kp-gyro_z*PID.Turn_Kd;
	return Turn;
}

