//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��д��:  Commodore 
//��������:2016/4/6
//�汾:    V1.0
//��Ȩ���У�����ؾ���
//All rights reserved									  
///////////////////////////////////////////////////////////////////////////////////

#ifndef _PID_CONTROL_H
#define _PID_CONTROL_H
#include "Includes.h"
void Fly_Prepare(void);
void PID_Init(void);
void PID_In_Control(void);
void PID_Out_Control(void);
void Calculate_Target(void);
#endif

