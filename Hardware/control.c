#include "stm32f1xx_hal.h"
#include "control.h"
#include "math.h"
#include "stdlib.h"

extern TIM_HandleTypeDef htim1;			//��ʱ��1 PWM
extern float Movement;                  //�ٶȵ���  
extern int  Contrl_Turn;                //ת����ڱ���

//PID���ڲ���
struct pid_arg PID = {
	.Balance_Kp= 425,  
	.Balance_Kd= 2.0,
	
	.Velocity_Kp= -57,  
	.Velocity_Ki= -0.28,
	
	.Turn_Kp = 15,
	.Turn_Kd = 0.15,
};


/**************************************************************************************************************
*������:Read_Encoder()
*����:��ȡ������ֵ(����С����ǰǰ�����ٶ�)
*�β�:(u8 TIMX):xΪ������1����2
*����ֵ:��
*************************************************************************************************************/
int Read_Encoder(TIM_HandleTypeDef *htim)
{
	int temp;
	temp = (short)__HAL_TIM_GetCounter(htim);
	__HAL_TIM_SetCounter(htim,0);
	return temp;
}

/**************************************************************************************************************
*������:Set_PWM()
*����:���PWM���Ƶ��
*�βΣ�(int motor1):���1��Ӧ��PWMֵ/(int motor2):���2��Ӧ��PWMֵ
*����ֵ:��
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
*������:PWM_Limiting()
*����:PWM�޷�����
*�β�:��
*����ֵ:��
***************************************************************************************************************/
void PWM_Limiting(int *motor1,int *motor2)
{
	if(*motor1 < PWM_MIN) *motor1 = PWM_MIN;
	if(*motor1 > PWM_MAX) *motor1 = PWM_MAX;
	if(*motor2 < PWM_MIN) *motor2 = PWM_MIN;
	if(*motor2 > PWM_MAX) *motor2 = PWM_MAX;	
}

/**************************************************************************************************************
*������:Turn_off()
*����:�رյ��
*�β�:(const float Angle):x��Ƕ�ֵ
*����ֵ:1:С����ǰ����ֹͣ״̬/0:С����ǰ��������״̬
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
*������:Vertical_Ring_PD()
*����:ֱ����PD����
*�β�:(float Angle):x��ĽǶ�/(float Gyro):x��Ľ��ٶ�
*����ֵ:����PIDת��֮���PWMֵ
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
*������:Vertical_speed_PI()
*���ܣ��ٶȻ�PI����
*�β�:(int encoder_left):���ֱ�����ֵ/(int encoder_right):���������ֵ�ֵ/(float Angle):x��Ƕ�ֵ
*����ֵ:
**************************************************************************************************************/
extern uint8_t stop;
int Vertical_speed_PI(int encoder_left,int encoder_right,float Angle,float Movement )
{
	static float Velocity,Encoder_Least,Encoder;
	static float Encoder_Integral;
	Encoder_Least =(encoder_left+encoder_right)-Movement;   //��ȡ�����ٶ�ƫ��=�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩
	Encoder *= 0.8f;								 //һ�׵�ͨ�˲��� ���ϴε��ٶ�ռ85%
	Encoder += Encoder_Least*0.2f;                   //һ�׵�ͨ�˲����� ���ε��ٶ�ռ15% 
	
	Encoder_Integral +=Encoder;                      //���ֳ�λ�� ����ʱ�䣺10ms
//	Encoder_Integral=Encoder_Integral-Movement; 

	if(Encoder_Integral>10000)  	Encoder_Integral=10000;           //�����޷�
	if(Encoder_Integral<-10000)	  Encoder_Integral=-10000;            //�����޷�

	Velocity=Encoder*PID.Velocity_Kp+Encoder_Integral*PID.Velocity_Ki;      //�ٶȿ���

	if(Turn_off(Angle)==1)   Encoder_Integral=0;            //����رպ��������
	return Velocity;
}
/**************************************************************************************************************
*������:Vertical_turn_PD()
*����:ת��PD
*�β�:��  gyro_z����������ֵ,�Ƕ�ֵ
*����ֵ:��
***************************************************************************************************************/
int Vertical_turn_PD(float gyro_z,int Target_turn)
{
	float Turn;     
	Turn=Target_turn*PID.Turn_Kp-gyro_z*PID.Turn_Kd;
	return Turn;
}

