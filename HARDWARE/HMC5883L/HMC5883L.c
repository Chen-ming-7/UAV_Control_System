/******************* (C) COPYRIGHT 2016  ***************************
 * �ļ���   ��hmc5883.c
 * ����     ��hmc5883����
 * ʵ��ƽ̨ ��
 * ��汾   ��ST3.5.0
 *
 *  
*********************************************************************************/
#include "HMC5883L.h"
#define Gyro_G  0.0609756f   //���ٶȵ���   1�� = 0.0174533 ����
//��ǰ�ų������ֵ����Сֵ
_Flag    MagFlag;
int16_t  HMC58X3_limit[6]={0};
int16_t mag[3];
/*====================================================================================================*
**���� : Init_HMC5883L
**���� : ָ�����ʼ��
**���� : None
**ݔ�� : ״̬
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
void HMC5883L_Init(void)
{
	u8 ack; 
	ack = IIC_Read_Byte(MAG_ADDRESS,0x0A);//��ȡID��
	if (!ack)
	{
	  MagFlag.MagExist=0;
	  return ;
	}
	MagFlag.MagExist=1;
	// leave test mode
	IIC_Write_Byte(MAG_ADDRESS, HMC58X3_R_CONFA, 0x78);   // 0x70Configuration Register A  -- 0 11 100 00  num samples: 8 ; output rate: 75Hz ; normal measurement mode
	IIC_Write_Byte(MAG_ADDRESS, HMC58X3_R_CONFB, 0x20);   //0x20 Configuration Register B  -- 001 00000    configuration gain 1.33Ga
	IIC_Write_Byte(MAG_ADDRESS, HMC58X3_R_MODE, 0x00);    // Mode register             -- 000000 00    continuous Conversion Mode
	delay_ms(100);
}
	
/*====================================================================================================*/
/*====================================================================================================*
**���� : hmc5883lRead
**���� : ��ȡ�ش�����
**���� : None
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/

void HMC5883lRead(Sensor_Data*magData)
{
	u8 buf[6];
	static int An[3]={0,0,0};
	//��ȡ�Ĵ�������
	IIC_Read_Len(MAG_ADDRESS, MAG_DATA_REGISTER,6,buf);
	//ʮλ����˲�
	An[0]-=An[0]/10;
	An[0]+=(int16_t)buf[0] << 8 | buf[1];//X��
	mag[0] =An[0]/10;
	
	An[1]-=An[1]/10;
	An[1]+=(int16_t)buf[4]<< 8  | buf[5];//Y��
	mag[1] =An[1]/10;
	
	An[2]-=An[2]/10;
	An[2]+=(int16_t)buf[2] << 8 | buf[3];//Z��
	mag[2] =An[2]/10;
	if(MagFlag.calibratingM!=0)
	{
		MagFlag.MagIssue=0;
		Mag_Calibration(mag);//����У׼,����������ֵ����Сֵ
	}
	else 
	{
	  magData->X = mag[0] -(HMC58X3_limit[3] +HMC58X3_limit[0])/2;//(���ֵ+��Сֵ)/2
      magData->Y = mag[1] -(HMC58X3_limit[4] +HMC58X3_limit[1])/2;
      magData->Z = mag[2] -(HMC58X3_limit[5] +HMC58X3_limit[2])/2;
	}
}
/*====================================================================================================*/
/*====================================================================================================*
**���� : Mag_Calibration
**���� : �ش�У׼
**���� : None
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
void Mag_Calibration(int16_t *array)
{
	u8 cy,num;
	static u8  clen_MagFlag=1; 
	static float x,y,z; 
	//У׼֮ǰ�Ȱ�֮ǰ��������
	if(clen_MagFlag)
	{
		clen_MagFlag = 0;
		x=y=z=0;
		for(cy=0;cy<6;cy++)
			HMC58X3_limit[cy]=0;
	}
	// ��ʼ�ɼ�Ѱ���������������Сֵ
	for(cy=0;cy<3;cy++)//0,1,2�洢��Сֵ3,4,5�洢���ֵ
	{
		if(HMC58X3_limit[cy]>array[cy]) 
		{
			HMC58X3_limit[cy]= array[cy];//����С
		}
		else if(HMC58X3_limit[cy+3]<array[cy]) 
		{
			HMC58X3_limit[cy+3]=array[cy];//�����
		}
	}
	//��������жϽ��еش�У׼�Ķ������ü��ٶȼ��ж��Ƿ�ֱ�������������ж��Ƿ�ת����360��
	if(MagFlag.calibratingM == 1 && (absu16(ACC.Z) > 5000))  
	{
	    z+=GYRO.Z * Gyro_G * 0.002f;
		if(absFloat(z)>=360)  
		{
			MagFlag.calibratingM = 2;
			LED1=1;
		}
	}
	if(MagFlag.calibratingM == 2 && (absu16(ACC.X) > 5000))   
	{   
	    x += GYRO.X * Gyro_G * 0.002f;
		if(absFloat(x)>360)  
		{
			MagFlag.calibratingM = 3;
			LED1=0;
		}
	}
	if(MagFlag.calibratingM == 3 && (absu16(ACC.Y) > 5000))   
	{
	    y += GYRO.Y * Gyro_G * 0.002f;
		if(absFloat(y)>360)  
		{   
		    //���������ֵ��ƫС˵���ش������⣬ͣ�õش�  
			for(cy=0;cy<6;cy++)	
			{   
			  //���������ֵ����-20��20֮��
			  if(absu16(HMC58X3_limit[cy])<20)num++;
			}
			if(num>2)MagFlag.MagIssue = 1;//������
			LED1=1;
			clen_MagFlag = 1;
			MagFlag.calibratingM = 0;
			EE_Save_MAG_OFFSET();
		}
	}	
}
