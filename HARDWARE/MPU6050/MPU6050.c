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
#include "MPU6050.h"
#include "Attitude_Count.h"
#define 	MPU6050_MAX		32767
#define		MPU6050_MIN		-32768
u8 MPU6050_Buffer[14];
//У׼�ı�־
u8  ACC_Calibration_Flag;
u8  GYRO_Calibration_Flag;

//IMU��ʼ��
void MPU_Init(void)
{
	u8 res; 
	IIC_Init();//��ʼ��IIC����
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
    delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//����MPU6050 
	MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(1);					//���ٶȴ�����,��4g
	MPU_Set_Rate(50);						//���ò�����50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//�ر������ж�
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X82);	//INT���ŵ͵�ƽ��Ч//
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG); 
	if(res==MPU_ADDR)//����ID��ȷ
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
		MPU_Set_Rate(50);						//���ò�����Ϊ50Hz
 	}
}
//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}
//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}
//Acc��gyro����������Ϊָ�����ͣ�MPU���ݴ�����
void MPU_Data_Analyse(Sensor_Data*Acc,Sensor_Data*Gyro)
{
		short int acc_x; 
		short int acc_y;
		short int acc_z;
		short int gyr_x;
		short int gyr_y;
		short int gyr_z;
		  
		acc_x =((((short int)MPU6050_Buffer[0])	<< 8) | MPU6050_Buffer[1]) - ACC_OFFSET.X;
		acc_y =((((short int)MPU6050_Buffer[2])	<< 8) | MPU6050_Buffer[3]) - ACC_OFFSET.Y;
		acc_z =((((short int)MPU6050_Buffer[4])	<< 8) | MPU6050_Buffer[5]) - ACC_OFFSET.Z;
		
	    gyr_x =((((short int)MPU6050_Buffer[8])	 << 8) | MPU6050_Buffer[9]) - GYRO_OFFSET.X;
        gyr_y =((((short int)MPU6050_Buffer[10]) << 8) | MPU6050_Buffer[11])- GYRO_OFFSET.Y;
	    gyr_z =((((short int)MPU6050_Buffer[12]) << 8) | MPU6050_Buffer[13])- GYRO_OFFSET.Z;
	   //����Ϊʲô��ô������
//	    if(gyr_z>=-10&&gyr_z<=10)
//	    {
//		  gyr_z=0;
//	    }
		Acc->X=(acc_x > MPU6050_MAX ?MPU6050_MAX : acc_x);
		Acc->Y=(acc_y > MPU6050_MAX ?MPU6050_MAX : acc_y);
		Acc->Z=(acc_z > MPU6050_MAX ?MPU6050_MAX : acc_z);
		Acc->X=(acc_x < MPU6050_MIN ?MPU6050_MIN : acc_x);
		Acc->Y=(acc_y < MPU6050_MIN ?MPU6050_MIN : acc_y);
		Acc->Z=(acc_z < MPU6050_MIN ?MPU6050_MIN : acc_z);

		Gyro->X=(gyr_x >MPU6050_MAX ?MPU6050_MAX : gyr_x);
		Gyro->Y=(gyr_y >MPU6050_MAX ?MPU6050_MAX : gyr_y);
		Gyro->Z=(gyr_z >MPU6050_MAX ?MPU6050_MAX : gyr_z);
		Gyro->X=(gyr_x <MPU6050_MIN ?MPU6050_MIN : gyr_x);
		Gyro->Y=(gyr_y <MPU6050_MIN ?MPU6050_MIN : gyr_y);
		Gyro->Z=(gyr_z <MPU6050_MIN ?MPU6050_MIN : gyr_z);

		if(GYRO_Calibration_Flag)
		{ 
			static u8 gyr_time=0;
			static int tempgx=0;
			static int tempgy=0;
            static int tempgz=0;				
			if(gyr_time==0)
			{
			  GYRO_OFFSET.X=0;
			  GYRO_OFFSET.Y=0;
			  GYRO_OFFSET.Z=0;
			  tempgx=0;
			  tempgy=0;
			  tempgz=0;
              gyr_time++;
			  return;
			}	
            tempgx+=gyr_x;
			tempgy+=gyr_y;
			tempgz+=gyr_z;
			if(gyr_time==100)
			{
			    GYRO_OFFSET.X=tempgx/100;
				GYRO_OFFSET.Y=tempgy/100;
				GYRO_OFFSET.Z=tempgz/100;
				gyr_time=0;
				GYRO_Calibration_Flag=0;
				EE_Save_MPU6050_Gyro_Offset();
				return ;
			}
			gyr_time++;
		}
		if(ACC_Calibration_Flag)
		{
		      static  u8  acc_time=0;
			  static  int tempax=0;
			  static  int tempay=0;
			  static  int tempaz=0;
			  if(acc_time==0)
			  {
				ACC_OFFSET.X=0;
				ACC_OFFSET.Y=0;
				ACC_OFFSET.Z=0;
				tempax=0;
				tempay=0;
				tempaz=0;
				acc_time++;
				return;
			  }
				tempax+=acc_x;
				tempay+=acc_y;
				tempaz+=acc_z;
				if(acc_time==100)
				{
				     ACC_OFFSET.X=tempax/100;
					 ACC_OFFSET.Y=tempay/100;
					 ACC_OFFSET.Z=tempaz/100-8192;
					 acc_time=0;
					 ACC_Calibration_Flag=0;
					 EE_Save_MPU6050_Acc_Offset();
					 return ;
				}
				acc_time++;
		}
}
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
  addr=MPU_ADDR<<1;
  return IIC_Write_Len(addr,reg,len,buf);
}
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
  addr=MPU_ADDR<<1;
  return IIC_Read_Len(addr,reg,len,buf);
}
u8 MPU_Write_Byte(u8 reg,u8 data)
{
   return IIC_Write_Byte(0xD0,reg,data);
}
u8  MPU_Read_Byte(u8 reg)
{
   return IIC_Read_Byte(0xD0,reg);
}
