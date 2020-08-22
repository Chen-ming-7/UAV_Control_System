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
 \____/\_____/_/ /_/ /_/_/ /_/ / /\_____/\_____/\_____/_/   \_____\
                                                                                                                
* �������:2016.6.1
****************************************************************************/

#include "Usart1.h"
#include "stdio.h"
//����һ�����ջ�������һ�����ͻ�����
u8 RX_Buffer[50];
u8 TX_Buffer[50];
u8 USART_DMA_Flag=0;
//USART1  ��ʼ��  TX9   RX10
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void Usart_Init(u32 baud)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
	  USART_InitTypeDef   USART_InitStruct;
	  NVIC_InitTypeDef    NVIC_InitStruct;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,ENABLE);
	
	  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	  GPIO_Init(GPIOA,&GPIO_InitStruct);
	   
	  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	  GPIO_Init(GPIOA,&GPIO_InitStruct);
	  
	  NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	  NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	  NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	  NVIC_Init(&NVIC_InitStruct);

	  USART_InitStruct.USART_BaudRate=baud;
	  USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	  USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	  USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	  USART_InitStruct.USART_Parity=USART_Parity_No;
	  USART_InitStruct.USART_StopBits=USART_StopBits_1;
	  USART_Init(USART1,&USART_InitStruct);

		//�����ж�ʹ�ܺʹ����ж�����ѡ��
	  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	  USART_Cmd(USART1,ENABLE);
}
//�����жϺ�������λ�����͹���������֡��һ���ĸ�ʽ
void USART1_IRQHandler(void)
{ 
    
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
	     static u8 receive;
		 static u8 state=0;//��ʾ�ڼ����ֽ�
		 static u8 sum=0;
		 receive=USART1->DR;
		 if((receive==0xAA)&&(state==0))//���յ���һ���ֽ�
		 {
		      state=1;
			  RX_Buffer[0]=0xAA;
		 }
		 else if((receive==0xAF)&&(state==1))//���յ��ڶ����ֽ�
		 {
		      state=2;
			  RX_Buffer[1]=0xAF;
		 }
		 else if((receive<0xF1)&&(receive>0)&&(state==2))//���յ��������ֽ�
		 {
		      state=3;
			  RX_Buffer[2]=receive;
		 }
		 else if((receive<=50)&&(state==3))
		 {
		      state=4;
			  RX_Buffer[3]=receive;
			  sum=0;
		 }
		 else if(state==4)//��ʼ��ʽ��������
		 {  
		      RX_Buffer[4+sum]=receive;
			  if(sum==RX_Buffer[3])//����������ɣ�ֱ�ӽ��з�������
			  {   
				    Analyse_Data(sum+5);
				    sum=0;
				    state=0;
			  }
			  else
			  {
			    sum++;
			  }
		 }
        else 
		{
		    state=0;
		}			
	   USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}
void Analyse_Data(u8 Sum)
{
	 u8 i;
	 u8 temp=0;
     for(i=0;i<Sum-1;i++)
	 {
	   temp+=RX_Buffer[i];
	 }
	 if(temp!=RX_Buffer[Sum-1])return; // �ͼ��
	 if((RX_Buffer[0]!=0xAA)&&(RX_Buffer[1]!=0xAF))return ;//֡ͷ���
	 if(RX_Buffer[2]==0x01)
	 {
	        if(RX_Buffer[4]==0x01)//��������λ����У׼����
			{
			  ACC_Calibration_Flag=1;
			}
			if(RX_Buffer[4]==0x02)//��������λ����У׼����
			{
			  GYRO_Calibration_Flag=1;
			}
			if(RX_Buffer[4]==0x03)//
			{  
			    ACC_Calibration_Flag=1;
			    GYRO_Calibration_Flag=1;
			}
			if(RX_Buffer[4]==0x04)
			{
			    LED1=0;
			    MagFlag.calibratingM=1;
			}
			if(RX_Buffer[4]==0x05)//��ѹ��У׼
			{
			    BAR_Calibration_Flag=1;
			}
	 }
	 else if(RX_Buffer[2]==0x02)
	 {
	    if(RX_Buffer[4]==0x01)//��PID���ͻص��Ա�־
		{
			 Send_PID_Flag=1;
		}
	 }
	 else if(RX_Buffer[2]==0x10)//����PIDhost����computer���ͻ�����PID��ֵ
	 {
	     PID2_ROLL.P=(float)((short int)(RX_Buffer[4]<<8)|RX_Buffer[5])/1000;
		   PID2_ROLL.I=(float)((short int)(RX_Buffer[6]<<8)|RX_Buffer[7])/1000;
		   PID2_ROLL.D=(float)((short int)(RX_Buffer[8]<<8)|RX_Buffer[9])/1000;
		   PID2_PITCH.P=(float)((short int)(RX_Buffer[10]<<8)|RX_Buffer[11])/1000;
		   PID2_PITCH.I=(float)((short int)(RX_Buffer[12]<<8)|RX_Buffer[13])/1000;
		   PID2_PITCH.D=(float)((short int)(RX_Buffer[14]<<8)|RX_Buffer[15])/1000;
		   PID2_YAW.P=(float)((short int)(RX_Buffer[16]<<8)|RX_Buffer[17])/1000;
		   PID2_YAW.I=(float)((short int)(RX_Buffer[18]<<8)|RX_Buffer[19])/1000;//ע��������I���ǳ���100
		   PID2_YAW.D=(float)((short int)(RX_Buffer[20]<<8)|RX_Buffer[21])/1000;
		   //����Э�飬�������֮����Ҫ����ȷ��
		   Send_Check(RX_Buffer[2],temp);
		  // EE_Save_MPU6050_PID_Para();//�������ݵ�EEPROM
	 }
	 else if(RX_Buffer[2]==0x11)//����PIDhost����computer���ͻ�����PID��ֵ
	 {
	     PID1_ROLL.P=(float)((vs16)(RX_Buffer[4]<<8)|RX_Buffer[5])/1000;
		   PID1_ROLL.I=(float)((vs16)(RX_Buffer[6]<<8)|RX_Buffer[7])/1000;
		   PID1_ROLL.D=(float)((vs16)(RX_Buffer[8]<<8)|RX_Buffer[9])/1000;
		   PID1_PITCH.P=(float)((vs16)(RX_Buffer[10]<<8)|RX_Buffer[11])/1000;
		   PID1_PITCH.I=(float)((vs16)(RX_Buffer[12]<<8)|RX_Buffer[13])/1000;
		   PID1_PITCH.D=(float)((vs16)(RX_Buffer[14]<<8)|RX_Buffer[15])/1000;
		   PID1_YAW.P=(float)((vs16)(RX_Buffer[16]<<8)|RX_Buffer[17])/1000;
		   PID1_YAW.I=(float)((vs16)(RX_Buffer[18]<<8)|RX_Buffer[19])/1000;
		   PID1_YAW.D=(float)((vs16)(RX_Buffer[20]<<8)|RX_Buffer[21])/1000;
		   //����Э�飬�������֮����Ҫ����ȷ�ϣ�������λ�����Ƿ��ͳ�ʱ����ô���£���
		   Send_Check(RX_Buffer[2],temp);
		   //EE_Save_MPU6050_PID_Para();//�������ݵ�EEPROM
	 }
	 else if(RX_Buffer[2]==0x12)//PID3
	 {
		   Height_PID2.P=(float)((vs16)(RX_Buffer[4]<<8)|RX_Buffer[5])/1000;
		   Height_PID2.I=(float)((vs16)(RX_Buffer[6]<<8)|RX_Buffer[7])/1000;
		   Height_PID2.D=(float)((vs16)(RX_Buffer[8]<<8)|RX_Buffer[9])/1000;
		   Height_PID1.P=(float)((vs16)(RX_Buffer[10]<<8)|RX_Buffer[11])/1000;
		   Height_PID1.I=(float)((vs16)(RX_Buffer[12]<<8)|RX_Buffer[13])/1000;
		   Height_PID1.D=(float)((vs16)(RX_Buffer[14]<<8)|RX_Buffer[15])/1000;
		  // PID1_YAW.P=(float)((vs16)(RX_Buffer[16]<<8)|RX_Buffer[17])/1000;
		  // PID1_YAW.I=(float)((vs16)(RX_Buffer[18]<<8)|RX_Buffer[19])/1000;
		  // PID1_YAW.D=(float)((vs16)(RX_Buffer[20]<<8)|RX_Buffer[21])/1000; 
          Send_Check(RX_Buffer[2],temp);//ֻ���أ�������
	 }
//	  else if(RX_Buffer[2]==0x13)//PID4
//	 { 
//	     Send_Check(RX_Buffer[2],temp);
//	 }
//	  else if(RX_Buffer[2]==0x14)//PID5
//	 { 
//	     Send_Check(RX_Buffer[2],temp);
//	 }
//	  else if(RX_Buffer[2]==0x15)//PID6
//	 { 
//	     Send_Check(RX_Buffer[2],temp);
//	 }
}


	