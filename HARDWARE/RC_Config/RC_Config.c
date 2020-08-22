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
#include "RC_Config.h"
u8   QUAD_OK=0;
u8   Flight_Mode=0;//ģʽ����
 u16 THR_MAX=1500;
 u16 THR_MIN=1500;
 u16 YAW_MAX=1500;
 u16 YAW_MIN=1500;
 u16 ROLL_MAX=1500;
 u16 ROLL_MIN=1500;
 u16 PITCH_MAX=1500;
 u16 PITCH_MIN=1500;
 u16 Half_Roll=0;
 u16 Half_Pitch=0;
 u16 Half_Yaw=0;
//�������������OR����,,�Ǵ����3��
void QUAD_Armed(u32 time)
{
  static u32 Keep_Cnt=0;
	if(ARMED_STATE)//�������ͨ����С��ƫ��ͨ������־�Ž���
	{
	   Keep_Cnt++;
		 if(Keep_Cnt>time)
		 {
		      QUAD_OK=1;//�����ɹ�
			  Keep_Cnt=0;
			  LED0=0;		 
		 }
	}
	else if(UN_ARMED_STATE)//����ͨ����С��ƫ��ͨ����С��־������
	{
	   Keep_Cnt++;
		 if(Keep_Cnt>time)
		 {
		     QUAD_OK=0;//�����ɹ�
			 Keep_Cnt=0;			 
			 LED0=1;
		 }
	}
	else 
	{
	   Keep_Cnt=0;
	}
}
//RC���̵��ڣ���10S��Ĭ��,����ǵ�һ��ʹ��һ��Ҫ����У׼�ģ�Ȼ�󱣴浽EEPROM
void RC_Adjust(void)
{  
	 u16 i;
	 LED1=0;
	 LED0=1;
	 delay_ms(100);
	 LED1=1;
	 LED0=1;
	 for(i=0;i<500;i++)
	 { 
		 //�����������2500������С��900����ô��
	     if(THROTTLE>THR_MAX&&THROTTLE<2500)    THR_MAX=THROTTLE;
		 if(THROTTLE<THR_MIN&&THROTTLE>900)     THR_MIN=THROTTLE;
		 if(YAW>YAW_MAX&&YAW<2500)              YAW_MAX=YAW;
		 if(YAW<YAW_MIN&&YAW>900)               YAW_MIN=YAW;
		 if(ROLL>ROLL_MAX&&ROLL<2500)           ROLL_MAX=ROLL;
		 if(ROLL<ROLL_MIN&&ROLL>900)      		ROLL_MIN=ROLL;
		 if(PITCH>PITCH_MAX&&PITCH<2500)        PITCH_MAX=PITCH;
		 if(PITCH<PITCH_MIN&&PITCH>900)         PITCH_MIN=PITCH;
		 delay_ms(20);
	 }
	 LED1=0;
	 LED0=1;
	 delay_ms(100);
	 LED1=1;
	 LED0=1;
	 EE_Save_RC_MAX_MIN();
}

