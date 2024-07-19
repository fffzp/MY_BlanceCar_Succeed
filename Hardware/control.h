#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "stm32f1xx_hal.h"

#define Mechanical_balance -1.2	//机械0点
#define PWM_MAX 7200			//PWM限幅值			//7200
#define PWM_MIN -7200

#define AIN1(PinState)    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, (GPIO_PinState)PinState)
#define AIN2(PinState)    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, (GPIO_PinState)PinState)

#define BIN1(PinState)    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, (GPIO_PinState)PinState)
#define BIN2(PinState)    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, (GPIO_PinState)PinState)


struct pid_arg{
	float Balance_Kp;
	float Balance_Ki;
	float Balance_Kd;
	
	float Velocity_Kp;
	float Velocity_Ki;
	float Velocity_Kd;
	
	float  Turn_Kp;
	float  Turn_Ki;
	float  Turn_Kd;
};
extern struct pid_arg PID;

void Set_PWM(int motor1,int motor2);
void PWM_Limiting(int *motor1,int *motor2);
uint8_t Turn_off(const float angle);
int Read_Encoder(TIM_HandleTypeDef *htim);
int	Vertical_Ring_PD(float Angle,float Gyro);
int Vertical_speed_PI(int encoder_left,int encoder_right,float Angle,float Movement );
int Vertical_turn_PD(float gyro_z,int Target_turn);


#endif
