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

#include "DMA_Config.h"
//DMA ͨ�����ú���
u8 Flag_Uart_Send=0;
extern u8 TX_Buffer[50];
void DMA1_Init(void)
{
   DMA_InitTypeDef  DMA_InitStruct;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	//ʹ��DMA����
   DMA_DeInit(DMA1_Channel4);//��DMAͨ����������Ϊȱʡֵ  
   DMA_InitStruct.DMA_PeripheralBaseAddr=(u32)&USART1->DR;
   DMA_InitStruct.DMA_MemoryBaseAddr=(u32)TX_Buffer;
   DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralDST;
   DMA_InitStruct.DMA_BufferSize=50;
   DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
   DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
   DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
   DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
   DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
   DMA_InitStruct.DMA_Priority=DMA_Priority_Medium;
   DMA_InitStruct.DMA_M2M=DMA_M2M_Disable; 
   DMA_Init(DMA1_Channel4,&DMA_InitStruct);
   //DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
   // USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);	
}
//ÿ����һ��DMA���䶼Ҫ��������len,ʹ��DMAͨ��,��ַ����һֱ����TX_Buffer
void DMA_USART_Send(u16 len)
{    
	 USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//DMA����һ��������
	 DMA_Cmd(DMA1_Channel4,DISABLE);//�ر���Ӧͨ��
	 DMA_SetCurrDataCounter(DMA1_Channel4,len);//��������DMA�������Ĵ�С
	 DMA_Cmd(DMA1_Channel4,ENABLE);//������Ӧ��DMAͨ��
	 while(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=SET);//�ȴ��������
	 DMA_ClearFlag(DMA1_FLAG_TC4);//�����־λ
}

