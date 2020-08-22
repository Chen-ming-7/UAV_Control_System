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
#include "Attitude_Count.h"
#include "MPU6050.h"
#define RtA     57.324841f   //���ȵ��Ƕ�
#define AtR     0.0174533f   //�Ƕȵ�����
#define ACC_G   0.0011963f   //���ٶȵ�G
#define Gyro_G  0.0609756f   //���ٶȵ���   1�� = 0.0174533 ����
#define Gyro_Gr 0.0010642f   //���ٶȱ仡��

Sensor_Data  Acc1;
Sensor_Data  Gyro1;
Sensor_Data  Acc2;
Sensor_Data  Gyro2;
Sensor_Data  MAG;
Sensor_Data  ACC_AVG;
Sensor_Data  ACC;
Sensor_Data  GYRO;
Sensor_Data  GYRO_OFFSET;
Sensor_Data  ACC_OFFSET;
Angle Euler_Angle;
/* �������������  ��С����˳��� */
/* ��ʾ���������  ����ٰ�΢�ּ� */
/* �����𵴺�Ƶ��  ��������Ҫ�Ŵ� */
/* ����Ư���ƴ���  ����������С�� */
/* ����ƫ��ظ���  ����ʱ�����½� */
/* ���߲������ڳ�  ����ʱ���ټӳ� */
/* ������Ƶ�ʿ�  �Ȱ�΢�ֽ����� */
/* �������������  ΢��ʱ��Ӧ�ӳ� */
/* ��������������  ǰ�ߺ���ı�һ */
/* һ�����������  ������������� */
#define Kp     0.9f//����Ư��
#define Ki     0.0008f//������̬����ƫ��
#define halfT  0.0025f//����������ô����
float q0=1;
float q1=0;
float q2=0;
float q3=0;
float exInt=0;
float eyInt=0;
float ezInt=0;
float hx,hy,mag_yaw;
float vx,vy,vz;
void AHRS_Update(Sensor_Data*Gyro,Sensor_Data*Acc,Sensor_Data*Mag,Angle*euler_angle)
{
	    // float halfT,T;
	    // static u32   Last_Time=0;
	    float ex,ey,ez;
	    float norm,sin_roll,sin_pitch,cos_roll,cos_pitch;
	    float ax=Acc->X,ay=Acc->Y,az=Acc->Z;
	    float gx=Gyro->X,gy=Gyro->Y,gz=Gyro->Z;
	    // �����������Լ����ظ�������
       float q0q0 = q0*q0;
       float q0q1 = q0*q1;
       float q0q2 = q0*q2;
       float q0q3 = q0*q3;
       float q1q1 = q1*q1;
        float q1q2 = q1*q2;
        float q1q3 = q1*q3;
        float q2q2 = q2*q2;
        float q2q3 = q2*q3;
        float q3q3 = q3*q3;
	   //T=((float)(Micros()-Last_Time))/1000000.0f;//��λ������
	   //halfT=T/2.0f;
		//Printf("halfT:%.6lf",halfT);
		//����ÿ��
		gx*=Gyro_Gr;
		gy*=Gyro_Gr;
		gz*=Gyro_Gr;
		if(ax*ay*az==0)
		{
		  return;
		}
		//���ٶȵ�λ��,aΪ�������������ڷ������ķ���
		norm=sqrt(ax*ax+ay*ay+az*az);
		
		ax=ax/norm;
		ay=ay/norm;
		az=az/norm;
		
		
		vx = 2*(q1q3-q0q2);
		vy = 2*(q0q1 + q2q3);
		vz = 1-2*(q1q1 + q2q2);
		
		
		//������
		ex = (ay*vz - az*vy);
		ey = (az*vx - ax*vz);
		ez = (ax*vy - ay*vx);
		
		//ͨ��PI�����������		
		exInt = exInt + VariableParameter(ex)*ex*Ki;
		eyInt = eyInt + VariableParameter(ey)*ey*Ki;
		ezInt = ezInt + VariableParameter(ez)*ez*Ki;
		
    gx = gx + Kp*VariableParameter(ex)*ex+exInt;
		gy = gy + Kp*VariableParameter(ey)*ey+eyInt;
		gz = gz + Kp*VariableParameter(ez)*ez+ezInt;
		
		
		
		q0 = q0 + (-q1*gx - q2*gy - q3*gz)* halfT;
		q1 = q1 + (q0*gx + q2*gz - q3*gy) * halfT;
		q2 = q2 + (q0*gy - q1*gz + q3*gx) * halfT;
		q3 = q3 + (q0*gz + q1*gy - q2*gx) * halfT;
		
		//��Ԫ����λ����ֻ�е�λ������Ԫ�����ܱ�ʾ��ת����
		norm = sqrt(q0q0 + q1q1 + q2q2 + q3q3);
		q0 = q0 / norm;
		q1 = q1 / norm;
		q2 = q2 / norm;
		q3 = q3 / norm;
		
		
	    if((MagFlag.MagExist==1)&&(MagFlag.MagIssue==0))//�����ƴ��� ���Ҵ�����������ֵ���� ��ͨ�������ƹ���YAW
	    {   //����ŷ���ǵ����Ǻ���
			 sin_roll  = sin(atan2(2*q2q3 + 2*q0q1, -2*q1q1 - 2*q2q2 + 1));
			 sin_pitch = sin(asin(-2* q1q3 + 2*q0q2));
			 cos_roll  = cos(atan2(2*q2q3 + 2*q0q1, -2*q1q1 - 2*q2q2 + 1));
			 cos_pitch = cos(asin(-2* q1q3 + 2*q0q2));
			 //����hx��hy
			 hx=Mag->X*cos_pitch+Mag->Y*sin_pitch*sin_roll-Mag->Z*cos_roll*sin_pitch;
			 hy=Mag->Y*cos_roll+Mag->Z*sin_roll;
			 mag_yaw=-atan2(hy,hx)*RtA;//����Ϊ��ô�Ǹ�����	
		//	 euler_angle->yaw=mag_yaw;
        //   �����ǻ��ֵõ�YAW
			 euler_angle->yaw+=Gyro->Z*Gyro_G*0.005f;//�Ƕ�ÿ��
		   if((mag_yaw>90 && euler_angle->yaw<-90)||(mag_yaw<-90 && euler_angle->yaw>90)) 
			 {
				 euler_angle->yaw = -euler_angle->yaw * 0.988f + mag_yaw * 0.012f;
			 }
			 else
			 {				 
			     euler_angle->yaw = euler_angle->yaw * 0.988f + mag_yaw * 0.012f;
			 }		 
	     }
		else//���������������ڻ�����ֵ������
	    {		
		  euler_angle->yaw+=Gyro->Z*Gyro_G*0.005f;
		  //euler_angle->yaw=atan2(2 * (q0q1 + q2q3), -2*q2q2-2*q3q3+1)*RtA;//yaw
		}//����pitch������roll��
		euler_angle->pitch = safe_asin(-2* q1q3 + 2*q0q2)*RtA;//�����и�����������
	    euler_angle->roll  = atan2(2*q2q3 + 2*q0q1, -2*q1q1 - 2*q2q2 + 1)*RtA;
}
//******************************************************************************
// ��չ�ķ����Һ������������ֵ�ķ�Χ��-1~1��������������֣�isnan�����жϣ��򷵻�0
//******************************************************************************
float safe_asin(float v)
{
	if (isnan(v)) return 0.0f;

	if (v >= 1.0f) return 3.1415926/2;

	if (v <= -1.0f)return -3.1415926/2;
	
	return asin(v);
}
float Num_To_Dps(short int Num)//ת���ɶ�
{
     float temp;
	 temp=(float)Num*Gyro_G;
	 return temp;
}
/*====================================================================================================*/
/*====================================================================================================*
** ��������: IIR_I_Filter
** ��������: IIRֱ��I���˲���
** ��    ��: InData Ϊ��ǰ����
**           *x     ����δ�˲�������
**           *y     �����˲��������
**           *b     ����ϵ��b
**           *a     ����ϵ��a
**           nb     ����*b�ĳ���
**           na     ����*a�ĳ���
**           LpfFactor
** ��    ��: OutData         
** ˵    ��: ��
** ����ԭ��: y(n) =  b0*x(n) + b1*x(n-1) + b2*x(n-2) -
                    a1*y(n-1) - a2*y(n-2)
					
#define  IIR_ORDER     4    //ʹ��IIR�˲����Ľ���
double   b_IIR[IIR_ORDER+1] ={0.0008f, 0.0032f, 0.0048f, 0.0032f, 0.0008f};//ϵ��b
double   a_IIR[IIR_ORDER+1] ={1.0000f, -3.0176f, 3.5072f,-1.8476f,0.3708f};//ϵ��a

===================================================================================================*/
#define  IIR_ORDER     4    //ʹ��IIR�˲����Ľ���
double   b_IIR[IIR_ORDER+1] ={0.0008f, 0.0032f, 0.0048f, 0.0032f, 0.0008f};//ϵ��b
double   a_IIR[IIR_ORDER+1] ={1.0000f, -3.0176f, 3.5072f,-1.8476f,0.3708f};//ϵ��a
double   InPut_IIR[3][IIR_ORDER+1] = {0};

double   OutPut_IIR[3][IIR_ORDER+1] = {0};double IIR_I_Filter(double InData, double *x, double *y, double *b, short nb, double *a, short na)
{
  double z1,z2;
  short i;
  double OutData;
  
  for(i=nb-1; i>0; i--)
  {
    x[i]=x[i-1];
  }
  
  x[0] = InData;
  
  for(z1=0,i=0; i<nb; i++)
  {
    z1 += x[i]*b[i];
  }
  
  for(i=na-1; i>0; i--)
  {
    y[i]=y[i-1];
  }
  
  for(z2=0,i=1; i<na; i++)
  {
    z2 += y[i]*a[i];
  }
  
  y[0] = z1 - z2; 
  OutData = y[0];
    
  return OutData;
}
/*	
	Q:����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
	R:����������R���󣬶�̬��Ӧ�����������ȶ��Ա��	
*/
#define KALMAN_Q        0.02
#define KALMAN_R        6.0000
/*           ����������������ٶȽ����˲�����           */
double KalmanFilter(double ResrcData,double ProcessNiose_Q,double MeasureNoise_R)
{
   double R = MeasureNoise_R;
   double Q = ProcessNiose_Q;
   static double x_last;
   double x_mid = x_last;
   double x_now;
   static double p_last;
   double p_mid;
   double p_now;
   double kg;        

   x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
   p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
   kg=p_mid/(p_mid+R); //kgΪkalman filter��RΪ����
   x_now=x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ
                
   p_now=(1-kg)*p_mid;//����ֵ��Ӧ��covariance       
   p_last = p_now; //����covarianceֵ
   x_last = x_now; //����ϵͳ״ֵ̬
   return x_now;                
 }
/***********************************************
  * @brief  �ɱ���������Ӧ����
  * @param  None
  * @retval None
************************************************/
float VariableParameter(float error)
{
	float  result = 0;
	
	if(error < 0)
	{
	   error = -error;
	}
    if(error >0.8f)
	{
	   error = 0.8f;
	}
	result = 1 - 1.28 * error;
	if(result < 0)
	{
	   result = 0;
	}
	return result;
}
short int File_Buf[6][10];//
void Filter_Acc(Sensor_Data*Acc)
{
    // ���ٶȼ�IIR�˲�  ��ͨ�˲������������� //�����˲�Ч�������Ǻܺã��������绬��ƽ���˲�
	Acc->X = IIR_I_Filter(Acc->X, InPut_IIR[0], OutPut_IIR[0], b_IIR, IIR_ORDER+1, a_IIR, IIR_ORDER+1);
	Acc->Y = IIR_I_Filter(Acc->Y, InPut_IIR[1], OutPut_IIR[1], b_IIR, IIR_ORDER+1, a_IIR, IIR_ORDER+1);
	Acc->Z = IIR_I_Filter(Acc->Z, InPut_IIR[2], OutPut_IIR[2], b_IIR, IIR_ORDER+1, a_IIR, IIR_ORDER+1);
    //Acc->X=KalmanFilter(Acc->X,KALMAN_Q,KALMAN_R);//���ܼ�����������ʱ����
	//Acc->Y=KalmanFilter(Acc->Y,KALMAN_Q,KALMAN_R);
	//Acc->Z=KalmanFilter(Acc->Z,KALMAN_Q,KALMAN_R);
//	static int num = 0;
//	u8 i=0;
//	float sum1=0,sum2=0,sum3=0;
//	File_Buf[3][num] =Acc->X;
//	File_Buf[4][num] =Acc->Y;
//	File_Buf[5][num] =Acc->Z;
//	
//	for(i=0;i<10;i++)
//	{
//         sum1 += File_Buf[3][i];
//		 sum2 += File_Buf[4][i];
//		 sum3 += File_Buf[5][i];
//    }
//	Acc->X= sum1 / 10;
//	Acc->Y= sum2 / 10;
//	Acc->Z= sum3 / 10;
//	num = (num + 1) % 10;
}

void Filter_Gyro(Sensor_Data*Gyro)
{
    static int num = 0;
	u8 i=0;
	float sum1=0,sum2=0,sum3=0;
	File_Buf[0][num] =Gyro->X;
	File_Buf[1][num] =Gyro->Y;
	File_Buf[2][num] =Gyro->Z;
	
	for(i=0;i<10;i++)
	{
         sum1 += File_Buf[0][i];
		 sum2 += File_Buf[1][i];
		 sum3 += File_Buf[2][i];
    }
	Gyro->X= sum1 / 10;
	Gyro->Y= sum2 / 10;
	Gyro->Z= sum3 / 10;
	num = (num + 1) % 10;
}

