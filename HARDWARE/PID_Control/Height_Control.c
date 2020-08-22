#include "Height_Control.h"
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define ABS(x) ( (x)>0?(x):-(x) )
extern float   Thr_Weight;
u8 ultra_start_f =0;
u8 height_start_f=0;
extern float vx,vy,vz;
float wz_speed,wz_speed_old;
float Actual_Acc;//
float ultra_speed,ultra_distance,ultra_distance_old,ultra_delta,ultra_ctrl_out,ultra_dis_lpf;
//�߶ȿ��ƺ���
PID Height_PID1;//�߶��⻷
PID Height_PID2;//�߶��ڻ�
//�߶ȿ��ƺ���
void Height_Control(float T,float thr)
{   
	static u8 cnt=0;
	//��Լ��ٶ�
	if(height_start_f==0)//��һ�ν��붨�ߺ���
	{
	   Actual_Acc=0; 
	}
	Actual_Acc += ( 1 / ( 1 + 1 / ( 20 *3.14f *T ) ) ) *( (vz *ACC.Z + vx *ACC.X + vy*ACC.Y - 8192 ) - Actual_Acc);
	cnt++;
	cnt = cnt%10;
	if(cnt == 0)//20ms����һ��
	{
	   Ultra_In_Control(0.02f,thr,0.4f*ultra_ctrl_out,ultra_speed);
	}
	if(ultra_start_f == 1)//�����������һ�θ���
	{
	   Ultra_Out_Control(0.1f,thr);//����������100ms
	   ultra_start_f = 0;//���±�־���
	}
}

//�߶ȿ����⻷
void Ultra_Out_Control(float T,float thr)
{     
	  static float exp_height,height_err_old;
	  float        exp_speed,ultra_sp_tmp,ultra_dis_tmp,height_err;
	  //ע����������ٶȺ͸߶��й�ϵ��һ����ɣ����߶�2m��
	 if(height_start_f==0)//��һ�ν��붨��ģʽ�����߶ȵ��ڵ�ǰ�߶�,��ػ��ֶ�Ҫ����
	 {
	   exp_height=ultra_distance;
	   Height_PID1.IOut=0;
	   height_err_old=0;
	   ultra_dis_lpf=0;
	   ultra_speed=0;
	   height_start_f=1;
	 }
	 //������1400��1600֮�������ٶ�Ϊ0
	 exp_speed=LIMIT((300*my_deathzoom_2(thr-500,100)/200.0f),-300,300);
	 if(exp_height>2000)//�����߶Ȳ��ܳ���2m
	 { 
		exp_height=500;
		if(exp_speed>0)
		{
		  exp_speed=0;
		}		
	 }
	 else if(exp_height<200)//��С�����߶�20cm
	 {
		  exp_height=200;//20cm
		  if(exp_speed<0)
		  {
			exp_speed=0;
		  }
	 }
    exp_height+=exp_speed*T;//��������߶�1.5m
	if(thr<100)//�������С��100�������߶ȿ�ʼ�½�(һ������)
	{  
	  //�߶��𽥼�С
	  exp_height += ( 1 / ( 1 + 1 / ( 0.2f *3.14f *T ) ) ) *( -exp_height);
//	  if(exp_height<=200)//20cm
//	  {
//	    exp_height=0;
//	  }
	}
	ultra_sp_tmp = Moving_Median(0,5,ultra_delta/T); //ultra_delta/T;
    //���ٶȽ��е�ͨ�˲�
    if( ABS(ultra_sp_tmp) < 100)
    {
        ultra_speed += ( 1 / ( 1 + 1 / ( 4 *3.14f *T ) ) ) * ( (float)(ultra_sp_tmp) - ultra_speed );
    }
    else
    {
        ultra_speed += ( 1 / ( 1 + 1 / ( 1.0f *3.14f *T ) ) ) * ( (float)(ultra_sp_tmp) - ultra_speed );
    }
    //�����������ľ�����л����˲�
	ultra_dis_tmp = Moving_Median(1,5,ultra_distance);
    //�Գ�������õľ�����е�ͨ�˲�
    if(ABS(ultra_dis_tmp - ultra_dis_lpf) < 100)
    {
        ultra_dis_lpf += ( 1 / ( 1 + 1 / ( 4.0f *3.14f *T ) ) ) *(ultra_dis_tmp - ultra_dis_lpf) ;
    }
    else if( ABS(ultra_dis_tmp - ultra_dis_lpf) < 200 )
    {

        ultra_dis_lpf += ( 1 / ( 1 + 1 / ( 2.2f *3.14f *T ) ) ) *(ultra_dis_tmp- ultra_dis_lpf) ;
    }
    else if( ABS(ultra_dis_tmp - ultra_dis_lpf) < 400 )
    {
        ultra_dis_lpf += ( 1 / ( 1 + 1 / ( 1.2f *3.14f *T ) ) ) *(ultra_dis_tmp- ultra_dis_lpf) ;
    }
    else
    {
        ultra_dis_lpf += ( 1 / ( 1 + 1 / ( 0.6f *3.14f *T ) ) ) *(ultra_dis_tmp- ultra_dis_lpf) ;
    }
	//�����߶��������Ÿ����ģ����������߶ȼ�ȥ��ͨ�˲�֮��ĸ߶�=�߶����
	height_err =Height_PID1.P*(exp_height - ultra_dis_lpf);
	//������
	Height_PID1.IOut += Height_PID1.I *height_err *T;
	//�������Ʒ���300*����Ȩ��
	Height_PID1.IOut = LIMIT(Height_PID1.IOut,-Thr_Weight *300,Thr_Weight *300);
	//����ȫ΢��
	Height_PID1.DOut = Height_PID1.D *(0.6f *(-wz_speed*T) + 0.4f *(height_err - height_err_old) );
	Height_PID1.Out  = LIMIT(( height_err + Height_PID1.IOut + Height_PID1.DOut),-500,500);
    //�������Ʒ���
	ultra_ctrl_out=Height_PID1.Out;
	height_err_old = height_err;
}

//����������ģʽ�ڻ�
//����ģʽ�ڻ��ٶȿ��ƣ�����T��THR��0-1000��������Z���ٶȺ�ʵ��Z���ٶ�   �ٶȿ��ƻ�������Ϊ20ms
void Ultra_In_Control(float T,float thr,float exp_z_speed,float h_speed)
{
	static float thr_lpf;
	float height_thr,Actual_Acc_mms2,speed_err;
	static float hc_acc_i,wz_speed_0,wz_speed_1;
    //�߶��������Ʒ���
	if(height_start_f==1)//��һ�ν����������
	{
	   hc_acc_i=0;
	   wz_speed_0=0;
	   wz_speed_1=0;
	   thr_lpf=thr;     //���Կ��ǽ���ǰ����ֱֵ�Ӹ�ֵ��ֹ�л�������ģʽ��ʱ�����
	  Height_PID2.IOut=0;
	}
	//Ϊ�˷�ֹ�ոռ��붨�ߵ�ʱ��ֱ�ӽ��䣬
	height_thr = LIMIT( 2 * thr , 0, 600 );//�߶�������0-600//0-700
    //�߶����ŵ�ͨ�˲�
	thr_lpf += ( 1 / ( 1 + 1 / ( 0.5f *3.14f *T ) ) ) *( height_thr - thr_lpf );
	//wz_speed�ĸ��������⣿������������������������������������
	//���ٶ����ݺͳ��������ݽ����ںϡ���������ʼ
	Actual_Acc_mms2 = (Actual_Acc/8192.0f) *10000 + hc_acc_i;//9.8m/S^2=10m/S^2=10000mm/S^2
	wz_speed_0 += my_deathzoom((Actual_Acc_mms2) ,100) *T;//if acc>100 ��(acc-100)*T   else if acc<-100,��(acc+100)*T
	//���ٶ�������
	hc_acc_i += 0.4f *T *((wz_speed - wz_speed_old)/T - Actual_Acc_mms2 );//���ٶ����=�Ӽ��ٶ� ����ki=0.4*T
	//���ٶ�������Ʒ���
	hc_acc_i = LIMIT( hc_acc_i, -500, 500 );
	//�Գ�������õ��ٶȽ��е�ͨ�˲�
	wz_speed_0 += ( 1 / ( 1 + 1 / (0.1f *3.14f *T ) ) ) *(h_speed - wz_speed_0) ;
	wz_speed_1 = wz_speed_0;//�����˲���ĳ�������õ��ٶȵ�λmm/s
	if(ABS(wz_speed_1)<50)//50mm/s �ٶ�Ҳ��С�˰�
	{
		wz_speed_1 = 0;
	}
	wz_speed_old = wz_speed;
	wz_speed = wz_speed_1;  //�����˲���ĳ�������õ��ٶȵ�����һ�ε��ٶ�
	//���ٶȼ����ݺͳ��������ݽ����ںϽ���
	speed_err = Height_PID2.P *( exp_z_speed - wz_speed );
	Height_PID2.DOut= 0.002f/T *10*Height_PID2.D * (-Actual_Acc_mms2)*T;//(Height_PID2_v.err - Height_PID2_v.err_old);
	//Height_PID2_v.err_i += Height_PID2.ki *Height_PID2_v.err *T;
	Height_PID2.IOut+= Height_PID2.I *Height_PID2.P*(exp_z_speed - h_speed)*T;
    //����Ȩ���ڣ�0~1��
	Height_PID2.IOut = LIMIT(Height_PID2.IOut,-Thr_Weight *300,Thr_Weight *300);
	Height_PID2.Out = thr_lpf + Thr_Weight *LIMIT((Height_PID2.P *exp_z_speed + speed_err + Height_PID2.DOut+ Height_PID2.IOut),-300,300); 
}

float my_deathzoom(float x,float zoom)
{
    float t;
    if(x>0)
    {
        t = x - zoom;
        if(t<0)
        {
            t = 0;
        }
    }
    else
    {
        t = x + zoom;
        if(t>0)
        {
            t = 0;
        }
    }
    return (t);
}


//������������
float my_deathzoom_2(float x,float zoom)
{
    float t;

    if( x> -zoom && x < zoom )
    {
        t = 0;
    }
    else
    {
        t = x;
    }
    return (t);
}

//�����˲�����
#define MED_WIDTH_NUM 11
#define MED_FIL_ITEM  2
#define MED_FIL_ITEM_int  2
float med_filter_tmp[MED_FIL_ITEM][MED_WIDTH_NUM ];
int med_filter_tmp_int[MED_FIL_ITEM_int][MED_WIDTH_NUM ];
u8 med_fil_cnt[MED_FIL_ITEM];
float Moving_Median(u8 item,u8 width_num,float in)
{
    u8 i,j;
    float t;
    float tmp[MED_WIDTH_NUM];

    if(item >= MED_FIL_ITEM || width_num >= MED_WIDTH_NUM )
        return 0;
    else
    {
        if( ++med_fil_cnt[item] >= width_num )
            med_fil_cnt[item] = 0;
        med_filter_tmp[item][ med_fil_cnt[item] ] = in;
        for(i=0; i<width_num; i++)
            tmp[i] = med_filter_tmp[item][i];
        for(i=0; i<width_num-1; i++)
        {
            for(j=0; j<(width_num-1-i); j++)
            {
                if(tmp[j] > tmp[j+1])
                {
                    t = tmp[j];
                    tmp[j] = tmp[j+1];
                    tmp[j+1] = t;
                }
            }
        }
        return ( tmp[(u16)width_num/2] );
    }
}
