#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "key.h"
#include "dac.h"
#include "table_fft.h"
#include "stm32_dsp.h"
#include "math.h"
#include "STM32_WaveOutput.h"
#include "timer.h" 
 
 float Vref=0;
 long unsigned fftout[1024];
 long unsigned fftout2[1024];
 uint16_t X_MAG[1024];
 uint16_t X_MAG2[1024];
 u16 adcx1;
 u16 adcx2;

 int i=1;
 int j=0;
 int i_add1=0;  //�����ź�1  
 int i_add2=0;  //�����ź�2
 int i_noise=0; //�����ź�
 int rate;
 int Flag1=1;   //Ƶ�ʸ��ı�־
 int Flag2=1;   //Ƶ�ʸ��ı�־ 
 int Flag3=1;   //Ƶ�ʸ��ı�־

 int a=0;
 int b=0;
 int c=0;
 int start=1;   //�����ж�	 
 float temp;
 float temp2;
 long unsigned out[1024];
 long unsigned out2[1024];
 float max;      //�����źŷ�ֵ���ֵ
 float max1;     //�����źŷ�ֵmax
 float max2;     //�����źŷ�ֵmax2





 void dsp_asm_powerMag1(void)
{
  s16 lX,lY;
  u32 i;
  for(i=0;i<1024/2;i++)
  {
    lX  = (fftout[i] << 16) >> 16;
    lY  = (fftout[i] >> 16);
    {
    float X    = 1024 * ((float)lX) /32768;
    float Y    = 1024 * ((float)lY) /32768;
    float Mag = sqrt(X*X + Y*Y)/1024;
    X_MAG[i]    = (u32)(Mag * 65536);
    }
}
}
 void dsp_asm_powerMag2(void)
{
  s16 lX,lY;
  u32 i;
	  for(i=0;i<1024/2;i++)
  {
    lX  = (fftout2[i] << 16) >> 16;
    lY  = (fftout2[i] >> 16);
    {
    float X    = 1024 * ((float)lX) /32768;
    float Y    = 1024 * ((float)lY) /32768;
    float Mag = sqrt(X*X + Y*Y)/1024;
    X_MAG2[i]    = (u32)(Mag * 65536);
    }
  }

}


int main(void)
 { 

	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
 	LCD_Init();			   	//��ʼ��LCD 		
	KEY_Init();				//������ʼ��		 	
 	Adc_Init();		  		//ADC��ʼ��
	Dac1_Init();		 	//DACͨ��1��ʼ��	

	
	while(1)
	{




//�ɼ��ź�
		for(i=0;i<1024;i++)
		{
			adcx2=Get_Adc(ADC_Channel_3);
			temp2=10000*adcx2*3.3/4096-1.25*10000;
			out2[i]=((signed short)(temp2))<<16;
		}
		delay_ms(100);
		cr4_fft_1024_stm32(&fftout2,&out2, 1024);
		dsp_asm_powerMag2();

//���źŷ�ֵ�����ӦƵ��
		max=X_MAG2[1];
		for(i=1;i<1023;i++)
		{
			if((max<X_MAG2[i])&&(X_MAG2[i]>X_MAG2[i-1])&&(X_MAG2[i]>X_MAG2[i+1]))
			{
				max=X_MAG2[i];
				i_noise=i;
			}
		}	
					if(Flag1==1)
			{
				a=i_noise;
				Flag1=0;
			}
			
				if((i_noise>(a+1))||(i_noise<(a-1))||(start==1))
		{
				Vref=max/10000.0;
				rate=i_noise*2*44;
				Flag1=1;
		}
		else
		{
				if(Vref<(max/10000.0))
				Vref=max/10000.0;
	  }
		
	start=0;
	delay_ms(500);
		//���ԭ�ź� 
	SineWave_Init( SawToothWave ,rate ,ENABLE,SinWave ,rate  ,ENABLE);//PA4���Ϊ	10Hz�����ǲ���PA5���Ϊ10Hz�����Ҳ�����ֵ��Ϊ0~3.6V�����ʱΪ3.6��һ���ȶ���3.4V���ң�

	}
} 

