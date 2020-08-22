#include "Ultrasonic.h"


void Ultrasonic_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	       
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;					    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//��Ϊ�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(GPIOB, &GPIO_InitStructure);	        //��ʼ������GPIO 	
  GPIO_ResetBits(GPIOB,GPIO_Pin_2);
}

void Ultrasonic_Pulsing(void)
{ 
  GPIO_SetBits(GPIOB,GPIO_Pin_2);		  //��>10US�ĸߵ�ƽ
  delay_us(20);
  GPIO_ResetBits(GPIOB,GPIO_Pin_2);	
}
//ECHO����������PWM_Capture�ļ�����
