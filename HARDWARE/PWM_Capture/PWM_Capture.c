/***************************************************************************
* Copyright (c) 
* All rights reserved.
* �ļ����ƣ�
* �ļ���ʶ����
* ժ    Ҫ��
* ��ǰ�汾��
*     _____                                        __    
     / ____\                                      / /
    / /                                          / /      _    
   / /   _____  ________  ________  _____  _____/ /____  / \_______ 
  / /   / ___ \/ __  __ \/ __  __ \/ ___ \/ ___  / ___ \/ ___/ ___ \
 / /___/ /__/ / / / / / / / / / / / /__/ / /__/ / /__/ / /  / /____/
 \____/\_____/_/ /_/ /_/_/ /_/ / /\_____/\_____/\_____/_/   \_____\
                                                                                                                
* �������:2016.6.1
****************************************************************************/

#include "PWM_Capture.h"
//����һ���ṹ��
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
u8  PWM_Capture1_Sta=0;
u8  PWM_Capture2_Sta=0;
u8  PWM_Capture3_Sta=0;
u8  PWM_Capture4_Sta=0;
u8  PWM_Capture5_Sta=0;
u8  PWM_Capture6_Sta=0;

u16 PWM_Capture1_Val1=0;
u16 PWM_Capture1_Val2=0;
u16 PWM_Capture2_Val1=0;
u16 PWM_Capture2_Val2=0;
u16 PWM_Capture3_Val1=0;
u16 PWM_Capture3_Val2=0;
u16 PWM_Capture4_Val1=0;
u16 PWM_Capture4_Val2=0;
u16 PWM_Capture5_Val1=0;
u16 PWM_Capture5_Val2=0;
u16 PWM_Capture6_Val1=0;
u16 PWM_Capture6_Val2=0;

u16 Ultra_Height[3];

short int ROLL;
short int PITCH;
short int THROTTLE;
short int YAW;
short int AUX1;
extern u8 ultra_start_f;
extern u8 height_start_f;
void PWM_Capture_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
  TIM_ICInitTypeDef        TIM_ICInitStruct;
  GPIO_InitTypeDef         GPIO_InitStruct;
  NVIC_InitTypeDef         NVIC_InitStruct;
  //����TIM4��GPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //ʹ��AFIO���ܵ�ʱ��
	
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_0|PWM_IN_PIN1|PWM_IN_PIN2 |PWM_IN_PIN3|PWM_IN_PIN4;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD; //�������벶��ߵ�ƽ
  GPIO_Init(GPIOB,&GPIO_InitStruct);
  //��ʼ������״̬��ȫ���õ�
  GPIO_ResetBits(GPIOB,PWM_IN_PIN1);
  GPIO_ResetBits(GPIOB,PWM_IN_PIN2);
  GPIO_ResetBits(GPIOB,PWM_IN_PIN3);
  GPIO_ResetBits(GPIOB,PWM_IN_PIN4);
  GPIO_ResetBits(GPIOB,GPIO_Pin_0);//��ӦTIM3��B0���ź�TIM3��CH3ͨ��
  GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	
  //TIM4��������ã�TIM_BASE����
  TIM_DeInit(TIM4);
  TIM_DeInit(TIM3);
  
  TIM_TimeBaseStruct.TIM_Period=arr;
  TIM_TimeBaseStruct.TIM_Prescaler=psc;
  TIM_TimeBaseStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseStruct.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStruct);
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);
  //TIM_ICChannel����
  
  TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;//IC1
  TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
  TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;//ӳ�䵽TI1
  TIM_ICInitStruct.TIM_ICFilter=0x00;//���˲�
  TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;//����Ƶ
  TIM_ICInit(TIM4,&TIM_ICInitStruct);
  
  TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
  TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
  TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;
  TIM_ICInitStruct.TIM_ICFilter=0x00;
  TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
  TIM_ICInit(TIM4,&TIM_ICInitStruct);
  
  TIM_ICInitStruct.TIM_Channel=TIM_Channel_3;
  TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
  TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;
  TIM_ICInitStruct.TIM_ICFilter=0x00;
  TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
  TIM_ICInit(TIM4,&TIM_ICInitStruct);
  TIM_ICInit(TIM3,&TIM_ICInitStruct);
  
   TIM_ICInitStruct.TIM_Channel=TIM_Channel_4;
   TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
   TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;
   TIM_ICInitStruct.TIM_ICFilter=0x00;
   TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
   TIM_ICInit(TIM4,&TIM_ICInitStruct);
   TIM_ICInit(TIM3,&TIM_ICInitStruct);
   
   //��ʼ���ж�IRQ  1 NVIC�ּ�����ʹ��  2 TIM4ͨ���жϿ�����TIM4ʹ��
   NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
   NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStruct);
   NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
   NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStruct);
   
   TIM_ARRPreloadConfig(TIM4, ENABLE); 
	 TIM_ARRPreloadConfig(TIM3, ENABLE); 
   TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);
	 TIM_ITConfig(TIM4,TIM_IT_CC2,ENABLE);
	 TIM_ITConfig(TIM4,TIM_IT_CC3,ENABLE);
	 TIM_ITConfig(TIM4,TIM_IT_CC4,ENABLE);
	
	 TIM_ITConfig(TIM3,TIM_IT_CC3,ENABLE);
	 TIM_ITConfig(TIM3,TIM_IT_CC4,ENABLE);
	
   TIM_Cmd(TIM4,ENABLE);
	 TIM_Cmd(TIM3,ENABLE);
} 
extern float ultra_distance,ultra_distance_old,ultra_delta;
void TIM3_IRQHandler(void)
{
  //����õ����ǲ���3�ж�
  if(TIM_GetITStatus(TIM3,TIM_IT_CC3)!=RESET)
  {  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture5_Sta)
		 {    
				 PWM_Capture5_Val2=TIM_GetCapture3(TIM3);
			     PWM_Capture5_Sta=0;
			     Ultra_Height[2]=Ultra_Height[1];
			     Ultra_Height[1]=Ultra_Height[0];
			    if(PWM_Capture5_Val2>PWM_Capture5_Val1)
			    {
				      Ultra_Height[0]=PWM_Capture5_Val2-PWM_Capture5_Val1;
			    }
			    else 
			    {  
				     Ultra_Height[0]=65535-PWM_Capture5_Val1+PWM_Capture5_Val2;
			    }
			    ultra_distance=LIMIT(((float)(Ultra_Height[2]+Ultra_Height[1]+Ultra_Height[0])/3*0.17f),0,2500);//��ȷ��mm
				ultra_delta=ultra_distance-ultra_distance_old;
				ultra_distance_old=ultra_distance;
				if(ultra_distance>2500)//���Ƹ߶���1500mm
				{
				  ultra_distance=2500;
				}
				ALT_CSB=ultra_distance/10;//����cm
				ultra_start_f=1;//�߶����ݸ�����ɱ�־
			    TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising);
		 }			 
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			  //���񵽵�һ��������
			  PWM_Capture5_Sta=1;
			  PWM_Capture5_Val1=TIM_GetCapture3(TIM3);
			  //��Ϊ�½��ز���
			  TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		 } 
		 TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);
	}
  //����õ����ǲ���4�ж�
  if(TIM_GetITStatus(TIM3,TIM_IT_CC4)!=RESET)
  {  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture6_Sta)
		 {
		      PWM_Capture6_Val2=TIM_GetCapture4(TIM3);
			  PWM_Capture6_Sta=0;
			 if(PWM_Capture6_Val2>PWM_Capture6_Val1)
			 {
			   AUX1=PWM_Capture6_Val2-PWM_Capture6_Val1;
			 }
			 else 
			 {
			   AUX1=65535-PWM_Capture6_Val1+PWM_Capture6_Val2;
			 }
			 if(AUX1<1300)//1129Ĭ�ϵ�ģʽ�ֶ�ģʽ
			 {
			   Flight_Mode=0;
			   height_start_f=0;//û�н��붨��ģʽ��־
			 }
			 else if(AUX1>1300&&AUX1<1500)//1450����ģʽ
			 {
			   Flight_Mode=1;
			 }
			 else if(AUX1>1500&&AUX1<2100)//1650�ڶ��ߵĻ����Ͻ��ж���
			 {
			   Flight_Mode=2;
			 }
			 TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Rising);
		 }
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			 //���񵽵�һ��������
			 PWM_Capture6_Sta=1;
			 PWM_Capture6_Val1=TIM_GetCapture4(TIM3);
			 //��Ϊ�½��ز���
			 TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		 }
         TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);		 
	}
}
void TIM4_IRQHandler(void)
{  
  //����õ����ǲ���1�ж�
  if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
  {  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture1_Sta)
		 {     
		     PWM_Capture1_Val2=TIM_GetCapture1(TIM4);
			 PWM_Capture1_Sta=0;
			 if(PWM_Capture1_Val2>PWM_Capture1_Val1)
			 {
				 PITCH=PWM_Capture1_Val2-PWM_Capture1_Val1;
			 }
			 else 
			 {
			    PITCH=65535-PWM_Capture1_Val1+PWM_Capture1_Val2;
			 }
			 TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			 //���񵽵�һ��������
			 PWM_Capture1_Sta=1;
			 PWM_Capture1_Val1=TIM_GetCapture1(TIM4);
			 //��Ϊ�½��ز���
			 TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
  }
  //����õ����ǲ���2�ж�
  if(TIM_GetITStatus(TIM4,TIM_IT_CC2)!=RESET)
  {  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture2_Sta)
		 {
		      PWM_Capture2_Val2=TIM_GetCapture2(TIM4);
			  PWM_Capture2_Sta=0;
			  if(PWM_Capture2_Val2>PWM_Capture2_Val1)
			  {
					ROLL=PWM_Capture2_Val2-PWM_Capture2_Val1;
			  }
				else 
				{
				  ROLL=65535-PWM_Capture2_Val1+PWM_Capture2_Val2;
				}
			  TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			 //���񵽵�һ��������
			 PWM_Capture2_Sta=1;
			 PWM_Capture2_Val1=TIM_GetCapture2(TIM4);
			 //��Ϊ�½��ز���
			 TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC2);
	}
	
	//����õ����ǲ���3�ж�
  if(TIM_GetITStatus(TIM4,TIM_IT_CC3)!=RESET)
	{  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture3_Sta)
		 {
		    PWM_Capture3_Val2=TIM_GetCapture3(TIM4);
			  PWM_Capture3_Sta=0;
			  if(PWM_Capture3_Val2>PWM_Capture3_Val1)
				{
					THROTTLE=PWM_Capture3_Val2-PWM_Capture3_Val1;
				}
				else 
				{
				  THROTTLE=65535-PWM_Capture3_Val1+PWM_Capture3_Val2;
				}
			  TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			 //���񵽵�һ��������
			 PWM_Capture3_Sta=1;
			 PWM_Capture3_Val1=TIM_GetCapture3(TIM4);
			 //��Ϊ�½��ز���
			 TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 } 
		 TIM_ClearITPendingBit(TIM4,TIM_IT_CC3);
	}
	//����õ����ǲ���4�ж�
  if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{  
		 //�Ѿ����񵽸ߵ�ƽ
		 if(PWM_Capture4_Sta)
		 {
		      PWM_Capture4_Val2=TIM_GetCapture4(TIM4);
			  PWM_Capture4_Sta=0;
			  if(PWM_Capture4_Val2>PWM_Capture4_Val1)
			  {
				   YAW=PWM_Capture4_Val2-PWM_Capture4_Val1;
			  }
			  else 
			  {
				   YAW=65535-PWM_Capture4_Val1+PWM_Capture4_Val2;
			  }
			  TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); 
		 }
		 else//��һ�β��񵽸ߵ�ƽ
		 {
			 //���񵽵�һ��������
			 PWM_Capture4_Sta=1;
			 PWM_Capture4_Val1=TIM_GetCapture4(TIM4);
			 //��Ϊ�½��ز���
			 TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);
		 }
         TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);		 
	}
}

