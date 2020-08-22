/***************************************************************************
* Copyright (c) 
* All rights reserved.
* �ļ����ƣ�
* �ļ���ʶ����
* ժ    Ҫ��
* ��ǰ�汾��
*                                     
      _____                                        __    
     / ____\                                      / /
    / /                                          / /      _    
   / /   _____  ________  ________  _____  _____/ /____  / \_______ 
  / /   / ___ \/ __  __ \/ __  __ \/ ___ \/ ___  / ___ \/ ___/ ___ \
 / /___/ /__/ / / / / / / / / / / / /__/ / /__/ / /__/ / /  / /____/
 \____/\_____/_/ /_/ /_/_/ /_/ /_/\_____/\_____/\_____/_/   \_____\
                                                                                                                
* �������:2016.6.1
****************************************************************************/
#include "TIM4.h"
#include "MPU6050.h"
#include "Attitude_Count.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include "Ultrasonic.h"
Angle Euler_Angle;
extern u8 Execute_Period2ms;//�ڻ�,MPU6050��RC
extern u8 Execute_Period5ms;//IMU���⻷PID5ms
extern u8 Execute_Period10ms;//��ѹ�ƺ͵�������10ms
extern u8 Execute_Period50ms;//��̬50ms�����������
extern u8 Execute_Period100ms;//����GPS100ms
extern u8 Execute_Period200ms;//��Ҫ����������200ms
extern u8 Execute_Period500ms;//��Ҫ����������500ms
extern u8 MPU6050_Buffer[14];
void TIM1_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  //��ʱ��TIM1�����ж�
  NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  TIM_TimeBaseStruct.TIM_Period = arr;
  TIM_TimeBaseStruct.TIM_Prescaler = psc;
  TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);
  
  TIM_ClearFlag(TIM1,TIM_FLAG_Update);
  //TIM_ARRPreloadConfig(TIM1, ENABLE); //Ĭ��
  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM1,ENABLE);
}
//ÿ��������1ms  72/72=1MHz;����1000������1ms����
//ע��߼���ʱ���ĸ����жϺ������ֺ���ͨ��ʱ����һ��
void TIM1_UP_IRQHandler(void)
{
    static u16 TIM1_Cnt=0;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{   
		TIM1_Cnt++;
		if(TIM1_Cnt%2==0)
		{
		  Execute_Period2ms=1;
		}
		if(TIM1_Cnt%5==0)
		{
		  Execute_Period5ms=1;
		}
		if(TIM1_Cnt%10==0)
		{
		  Execute_Period10ms=1;
		}
		if(TIM1_Cnt%50==0)
		{
		  Execute_Period50ms=1;
		}
		if(TIM1_Cnt%100==0)
		{
		  Execute_Period100ms=1;
		}
		if(TIM1_Cnt%200==0)
		{
		  Execute_Period200ms=1;
		}
		if(TIM1_Cnt%500==0)
		{
		  Execute_Period500ms=1;
		}
//		if(TIM1_Cnt%2==0)//ÿ2ms����һ��
//		{
//		  MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,14,MPU6050_Buffer);//��ȡ������������ÿ2msһ��
//		  MPU_Data_Analyse(&ACC,&GYRO);//ACC��GYRO����
//		  Filter_Acc(&ACC);//ACC�˲�  
//		  Filter_Gyro(&GYRO);//GYRO�˲������˲�
//		  PID_In_Control();//�ڻ�
//		}
//		if(TIM1_Cnt%5==0)//ÿ5ms����
//		{
//		  Calculate_Target();//����RC�����Ƕ� 
//		  AHRS_Update(&GYRO,&ACC,&MAG,&Euler_Angle);//������̬�ǣ���PITCHΪ90�ȵ�ʱ��ROLL��ͻȻ���170��ȣ������ص�����
//		  PID_Out_Control();//����PID���ƺ͵������
//		}
//		if(TIM1_Cnt%40==0)//40ms����һ�γ��������
//		{   
//			HMC5883lRead(&MAG);//��ȡ������
//		    BMP085_Routing();//ÿ40ms����һ����ѹ��
//		    Ultrasonic_Pulsing();
//		}
//		if(TIM1_Cnt%200==0)//200���뷢��һ�����ݵ���λ��
//		{  
//		  Send_Data_Flag1=1;
//		}
//		if(TIM1_Cnt%500==0)//
//		{
//		   QUAD_Armed(2);//ѭ��5�Σ�Ҳ���Ǳ���2����3S�ͱ任״̬
//		}
		TIM1_Cnt=TIM1_Cnt%1000;
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
     }
}


