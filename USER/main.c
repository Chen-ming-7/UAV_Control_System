/**************************************************************************************************************
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
 \____/\_____/_/ /_/ /_/_/ /_/ / /\_____/\_____/\_____/_/   \_____\
                                                                                                                
* �������:2016.6.1
**************************************************************************************************************/
#include "Includes.h"
#include "MPU6050.h"
#include "Attitude_Count.h"
#include "HMC5883L.h"
#include "Ultrasonic.h"
u8 Execute_Period2ms=0;//�ڻ�,MPU6050��RC
u8 Execute_Period5ms=0;//IMU���⻷PID5ms
u8 Execute_Period10ms=0;//��ѹ�ƺ͵�������10ms
u8 Execute_Period50ms=0;//��̬50ms�����������
u8 Execute_Period100ms=0;//����GPS100ms
u8 Execute_Period200ms=0;//��Ҫ����������200ms
u8 Execute_Period500ms=0;//��Ҫ����������500ms
extern u8 MPU6050_Buffer[14];
int main(void)
{
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();               //SYSTICKʱ��Ƶ�ʵ�ѡ��   
	Usart_Init(115200);         //���ڳ�ʼ����ͨ��USART1�������ݵ�host����computer ����������
  FLASH_Unlock();	          //Flash����
	EE_Init();                  //EEPROM��ʼ��
	LED_Init();                 //LED ��ʼ��
	DMA1_Init();                //DMA ��ʼ��,����USART��DMA����
  MPU_Init();                 //MPU6050��ʼ��������ģ��IIC���߳�ʼ��
	HMC5883L_Init();            //HMC5883��ʼ��
	BMP085_Init();              //BMP��ʼ��
	Ultrasonic_Init();          //��������ʼ��
	PWM_Capture_Init(0xFFFF,71);//1MHz.  PWM�����ʼ����TIM4
  Motor_Init(2499,71);        //2500,1MHz;2.5ms���һ��//�����ʼ��
	PID_Init();                 //PID�������ڳ�ʼ��
	while(1)
	{	
		if(Execute_Period2ms==1)
		{
		  Execute_Period2ms=0;
		  MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,14,MPU6050_Buffer);//��ȡ������������ÿ2msһ��
		  MPU_Data_Analyse(&ACC,&GYRO);//ACC��GYRO����
		  Filter_Acc(&ACC);  //ACC�˲�  
		  Filter_Gyro(&GYRO);//GYRO�˲������˲�
		  PID_In_Control();  //�ڻ�
		}
		if(Execute_Period5ms==1)
		{
		  Execute_Period5ms=0;
		  Calculate_Target();//����RC�����Ƕ� 
		  AHRS_Update(&GYRO,&ACC,&MAG,&Euler_Angle);//������̬�ǣ���PITCHΪ90�ȵ�ʱ��ROLL��ͻȻ���170��ȣ������ص�����
		  PID_Out_Control();//����PID���ƺ͵������
		}
		if(Execute_Period10ms==1)
		{
		   Execute_Period10ms=0;
		   HMC5883lRead(&MAG);  //��ȡ������
		   BMP085_Routing();    //ÿ40ms����һ����ѹ��
		}
		if(Execute_Period50ms==1)//����һ�γ���������
		{
		  //���������һ��ģʽ�ı亯����
		  Execute_Period50ms=0; 
		}
		if(Execute_Period100ms==1)//����������
		{
		   Execute_Period100ms=0;
		   Ultrasonic_Pulsing();
		}
		if(Execute_Period200ms==1)
		{
		   Execute_Period200ms=0;
		   Data_Send_Status();   // ŷ����
		   Data_Send_Sensor1();  // ����������
		   Data_Send_Sensor2();  // ���͸߶���Ϣ 
		}
		if(Execute_Period500ms==1)
		{
		   Execute_Period500ms=0;
		   if(Send_PID_Flag==1) //PID���е�PIDϵ��
		   {
		    Send_PID_Flag=0;    
			  Send_PID1_Parameters();
			  Send_PID2_Parameters();
			  Send_PID3_Parameters();
		   }
		   Data_Send_RCData(); //RCͨ����PWMֵ
		   QUAD_Armed(2);//ѭ��5�Σ�Ҳ���Ǳ���2����3S�ͱ任״̬
		}
   }
}
