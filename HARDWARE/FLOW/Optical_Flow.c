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
#include "Position_Control.h"
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )


Flow Optical_Flow;    //����һ���ṹ��
float     Exp_Roll;
float     Exp_Pitch;
u8        Flow_Buffer[25]; //����
short int Coordinate_X_Offset;//���붨��ģʽ����һ�̵�λ��ֵ
short int Coordinate_Y_Offset;//���붨��ģʽ��һ�̵�λ��ֵ
short int Coordinate_X = 0;   //��ǰλ��ֵ
short int Coordinate_Y = 0;   //��ǰλ��ֵ


PID PID1_POSITION_ROLL;//��
PID PID2_POSITION_ROLL;//��
PID PID1_POSITION_PITCH;
PID PID2_POSITION_PITCH;


void Optical_Flow_Read(void)
{
	
	//���㵱ǰ����
	Coordinate_X += Optical_Flow.flow_comp_m_x *0.1;//����mm
    Coordinate_Y += Optical_Flow.flow_comp_m_y *0.1;	
}
//��������

void Position_Control(void)
{    
	float Position_Roll_Error=0,Speed_Roll_Error;
    float Position_Pitch_Error=0,Speed_Pitch_Error;
	static float Last_Position_Roll_Error=0,Last_Position_Pitch_Error=0;//��һ��roll���
	static float Position_Roll_Speed=0,Position_Pitch_Speed=0;     //��ǰroll�ٶ�
	static float Last_Position_Roll_Speed=0,Last_Position_Pitch_Speed=0;//��һ��roll�ٶ�
	//Position_Loop
	Position_Roll_Error = (float)(Coordinate_Y_Offset-Coordinate_Y);//����
	Position_Pitch_Error= (float)(Coordinate_X_Offset-Coordinate_X);//ǰ��
	//Position_P
	PID1_POSITION_ROLL.POut  =PID1_POSITION_ROLL.P*Position_Roll_Error;
	PID1_POSITION_PITCH.POut =PID1_POSITION_PITCH.P*Position_Pitch_Error;
	//Position_I
	PID1_POSITION_ROLL.IOut +=PID1_POSITION_ROLL.I*Position_Roll_Error;
	PID1_POSITION_ROLL.IOut  =LIMIT(PID1_POSITION_ROLL.IOut,-2000,2000);
	PID1_POSITION_PITCH.IOut +=PID1_POSITION_PITCH.I*Position_Pitch_Error;
	PID1_POSITION_PITCH.IOut  =LIMIT(PID1_POSITION_Pitch.IOut,-2000,2000);
	//Position_D
	PID1_POSITION_ROLL.DOut  =PID1_POSITION_ROLL.D*(Position_Roll_Error-Last_Position_Roll_Error);
	PID1_POSITION_PITCH.DOut =PID1_POSITION_PITCH.D*(Position_Pitch_Error-Last_Position_Pitch_Error);
	Last_Position_Roll_Error =Position_Roll_Error;
	Last_Position_Pitch_Error=Position_Pitch_Error;
	//��������
	if(thr<100)
	{
	   PID1_POSITION_ROLL.IOut=0;
	   PID1_POSITION_PITCH.IOut=0;
	}
	//Position_Out
	PID1_POSITION_ROLL.Out=LIMIT((PID1_POSITION_ROLL.POut +PID1_POSITION_ROLL.IOut+PID1_POSITION_ROLL.DOut),-3000,3000);
    PID1_POSITION_PITCH.Out=LIMIT((PID1_POSITION_PITCH.POut+PID1_POSITION_PITCH.IOut +PID1_POSITION_ROLL.DOut),-3000,3000);
	
	
	//SpeeD_Loop
	Position_Roll_Speed = (float)(Optical_Flow.flow_comp_m_y)/1000.0f;//��λmm
	Position_Pitch_Speed= (float)(Optical_Flow.flow_comp_m_x)/1000.0f;
	//�����ٶ�-��ǰ�ٶȣ�ע�����������⡣?????????????????????????
	Speed_Roll_Error    = -(PID1_POSITION_ROLL.Out -Position_Roll_Speed)*0.01f;
	Speed_Pitch_Error   = -(PID1_POSITION_PITCH.Out-Position_Pitch_Speed)*0.01f;
	//Speed_P
	PID2_POSITION_ROLL.POut =PID2_POSITION_ROLL.P *Speed_Roll_Error ;
	PID2_POSITION_PITCH.POut=PID2_POSITION_PITCH.P*Speed_Pitch_Error;
	//Speed_I
	PID2_POSITION_ROLL.IOut +=PID2_POSITION_ROLL.I*Speed_Roll_Error;
	PID2_POSITION_ROLL.IOut  =LIMIT(PID2_POSITION_ROLL.IOut,-20,20);
	PID2_POSITION_PICTH.IOut+=PID2_POSITION_PICTH.I*Speed_Picth_Error;
	PID2_POSITION_PICTH.IOut =LIMIT(PID2_POSITION_PICTH.IOut,-20,20);
	//Speed_D
	PID2_POSITION_ROLL.DOut =PID2_POSITION_ROLL.D  *(Speed_Roll_Error-Last_Speed_Roll_Error);
	PID2_POSITION_PICTH.DOut=PID2_POSITION_PICTH.D*(Speed_Pitch_Error-Last_Speed_Pitch_Error);
	//Speed_Out
	PID2_POSITION_ROLL.Out   =LIMIT((PID2_POSITION_ROLL.POut +PID2_POSITION_ROLL.IOut+PID2_POSITION_ROLL.DOut),-30,30);
	PID2_POSITION_PICTH.Out  =LIMIT(PID2_POSITION_PITCH.POut+PID2_POSITION_PICTH.IOut+PID2_POSITION_PICTH.DOut),-30,30);
	Exp_Roll=PID2_POSITION_ROLL.Out;
	Exp_Pitch=PID2_POSITION_PICTH.Out;
	Last_Position_Roll_Speed = Position_Roll_Speed;
	Last_Position_Pitch_Speed= Position_Pitch_Speed;
} 
