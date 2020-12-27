#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "key.h"
#include "dac.h"
#include "timer.h"
#include "math.h"

int mode=0;
int i=0;
int j=0;
int U_Rmax;//�����������ѹ
int U_Rmin;//���������С��ѹ
int U_out1;//�������պϵ�ѹ
int U_out1min;
int U_out2;//������迪·��ѹ
int U_out2min;
int Uout=0;
float Uin_PP=0;
int temp=0;
float Udds=2050;
float Ri=23000;
float Rin=0;//�������
float Ro=33000;
float Rout=0;//�������
int change=0;
int key_i=0;
float Y[31]={45,45,45,41,40,40,39,39,39,39,39,39,39,39,39,39,45,45,48,53,54,64,79,85,99,111,134,140,171,189,204};
float gain=0;
int f[13]={142,143,144,145,146,147,148,149,150,151,152,153,154};
//�������״̬
//0Ϊ����normal   1Ϊ��·off  2Ϊ��·short
int R1_state=0;
int R2_state=0;
int R3_state=0;
int R4_state=0;

//���ݹ���״̬
//0Ϊ����normal   1Ϊ��·off  2Ϊ����twice
int C1_state=0;
int C2_state=0;
int C3_state=0;
int t=0;
//������ʾ
void fault(void)
{
	if(R1_state==0)
		LCD_ShowString(60,130,150,10,16,"normal");
	else if(R1_state==1)
		LCD_ShowString(60,130,150,10,16,"off");
	else 
		LCD_ShowString(60,130,150,10,16,"short");
	
	
	if(R2_state==0)
		LCD_ShowString(160,130,150,10,16,"normal");
	else if(R2_state==1)
		LCD_ShowString(160,130,150,10,16,"off");
	else 
		LCD_ShowString(160,130,150,10,16,"short");
	
	if(R3_state==0)
		LCD_ShowString(60,170,150,10,16,"normal");
	else if(R3_state==1)
		LCD_ShowString(60,170,150,10,16,"off");
	else 
		LCD_ShowString(60,170,150,10,16,"short");
	
	if(R4_state==0)
		LCD_ShowString(160,170,150,10,16,"normal");
	else if(R4_state==1)
		LCD_ShowString(160,170,150,10,16,"off");
	else 
		LCD_ShowString(160,170,150,10,16,"short");
	
	if(C1_state==0)
		LCD_ShowString(100,200,150,10,16,"normal");
	else if(C1_state==1)
		LCD_ShowString(100,200,150,10,16,"off");
	else 
		LCD_ShowString(100,200,150,10,16,"twice");
	
	if(C2_state==0)
		LCD_ShowString(100,230,150,10,16,"normal");
	else if(C2_state==1)
		LCD_ShowString(100,230,150,10,16,"off");
	else 
		LCD_ShowString(100,230,150,10,16,"twice");
	
	if(C3_state==0)
		LCD_ShowString(100,260,150,10,16,"normal");
	else if(C3_state==1)
		LCD_ShowString(100,260,150,10,16,"off");
	else 
		LCD_ShowString(100,260,150,10,16,"twice");

	
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
		///////////////////////////////////////////////////////////////
		//��������ģʽ�л� ��1KHZ���Ҳ���ɨƵ��
		///////////////////////////////////////////////////////////////

		if(KEY1==0)
		{
			mode=0;
			LCD_Clear(WHITE);
		}
		if(KEY0==0)
		{
			j=0;
			mode=1;
			LCD_Clear(WHITE);
		}
		///////////////////////////////////////////////////////////////	
		
		
		
		
		//�̵�������//
		/////////////////////////////////////////////////////////////
//		
//		if(change==0)
//		{
//			DAC_SetChannel1Data(DAC_Align_12b_R, 4000);
//			change=1;
//		}
//		else
//		{
//		 DAC_SetChannel1Data(DAC_Align_12b_R, 0);
//			change=0;
//		}
//		
		/////////////////////////////////////////////////////////////
		
		//�Զ�������ɨƵģʽ�л�
		////////////////////////////////////////////////////////////
	
		//////mode��0 �Զ����� ����1KHZ���Ҳ�
		if(mode==0)
	 {
		 
		++t;
		if(t==13)
			t=0;
		
		 //���������
			U_Rmax=Get_Adc(ADC_Channel_2);
		  	U_Rmin=Get_Adc(ADC_Channel_2);
			for(i=0;i<5000;i++)
			{
				temp=Get_Adc(ADC_Channel_2);
				if(temp<U_Rmin)
					U_Rmin=temp;
				if(temp>U_Rmax)
					U_Rmax=temp;
			}
			Uin_PP=(float)(Udds-(U_Rmax-U_Rmin)*1000*3.3/4096*0.95);//URin��VPP
			Rin=Udds/Uin_PP*Ri-Ri;
			//////////////////////////////////////////////////////
			/////////////////////////////////////////////////////
			
			
			
		//���������

			DAC_SetChannel1Data(DAC_Align_12b_R, 4000);
			U_out1=Get_Adc(ADC_Channel_3);
			U_out1min=Get_Adc(ADC_Channel_3);
			for(i=0;i<5000;i++)
			{
				temp=Get_Adc(ADC_Channel_3);
				if(temp>U_out1)
					U_out1=temp;
				if(temp<U_out1min)
					U_out1min=temp;
			}
			delay_ms(100);			

			DAC_SetChannel1Data(DAC_Align_12b_R, 0);
			U_out2=Get_Adc(ADC_Channel_3);
			U_out2min=Get_Adc(ADC_Channel_3);
			for(i=0;i<5000;i++)
			{
				temp=Get_Adc(ADC_Channel_3);
				if(temp>U_out2)
					U_out2=temp;
				if(temp<U_out2min)
					U_out2min=temp;
			}		
			Uout=(U_out2-U_out2min)-(U_out1-U_out1min);
			Rout=(float)(3300*(Uout+160)*Ro/(U_out1-U_out1min-160)/4096);
			delay_ms(1000);		
			
			//��1Khz����
			gain=(float)((U_out2-U_out2min)/21);
			gain=20*log10(gain);
			
			
			//��������Լ����������ʾ
			POINT_COLOR=BLACK;
			LCD_ShowString(50,20,150,10,16,"Input  Resistance:");	
			LCD_ShowString(50,70,150,10,16,"Output Resistance:");	
			LCD_ShowString(30,130,150,10,16,"R1:");	
			LCD_ShowString(130,130,150,10,16,"R2:");	
			LCD_ShowString(30,170,150,10,16,"R3:");	
			LCD_ShowString(130,170,150,10,16,"R4:");	
			LCD_ShowString(30,200,150,10,16,"C1:");
			LCD_ShowString(30,230,150,10,16,"C2:");
			LCD_ShowString(30,260,150,10,16,"C3:");
			LCD_ShowString(30,290,150,10,16,"1KHz Gain:");
			POINT_COLOR=RED;
			LCD_ShowxNum(10,40,Rin*0.95,10,16,0);//��ʾ�������
			LCD_ShowxNum(10,90,Rout,10,16,0);//��ʾ�������
			LCD_ShowxNum(110,290,gain,10,16,0);//��ʾ�������
			fault();
			POINT_COLOR=BLUE;
			LCD_ShowString(130,40,30,10,16,"Ohms");	
			LCD_ShowString(130,90,30,10,16,"Ohms");	
			LCD_ShowString(210,290,30,10,16,"dB");	
			delay_ms(100);
		}
	 
		
		
		///////////	///////////	///////////	///////////	///////////	///////////
		//mode ��1 ɨƵģʽ ��Ƶ������ͼ
		
		///////////	///////////	///////////	///////////	///////////	///////////
		
		if(mode==1)
		{ 
			DAC_SetChannel1Data(DAC_Align_12b_R, 0);
			U_out2=Get_Adc(ADC_Channel_3);
			U_out2min=Get_Adc(ADC_Channel_3);
			for(i=0;i<5000;i++)
			{
				temp=Get_Adc(ADC_Channel_3);
				if(temp>U_out2)
					U_out2=temp;
				if(temp<U_out2min)
					U_out2min=temp;
			}		
			gain=(float)((U_out2-U_out2min)/21);
			gain=20*log10(gain);

			POINT_COLOR=BLACK;
			LCD_DrawRectangle(25, 10, 235, 250);	
			LCD_ShowString(5,10,200,16,16,"dB");	
			LCD_ShowString(0,40,30,10,16,"150");	
			LCD_ShowString(0,80,30,10,16,"100");	
			LCD_ShowString(0,140,30,10,16,"50");				
			LCD_ShowString(25-2,260,200,16,16,"1");
			LCD_ShowString(95-2,260,200,16,16,"10");
			LCD_ShowString(165-2,260,200,16,16,"100");
			LCD_ShowString(210-5,260,200,16,16,"1000");
			LCD_ShowString(208,280,200,16,16,"KHz");
		  LCD_ShowString(30,290,200,16,16,"FH:");
			LCD_ShowxNum(65,290,f[t],10,16,0);//��ʾ�������			
		if(j==0)
		{
			for(i=0;i<=30;i++)
			{
				LCD_DrawLine(25+i*7,Y[i],25+(i+1)*7,Y[i+1]);
			}
			j++;
		}
		delay_ms(10);
	}
	}	 
	
  	
}	
		///////////	///////////	///////////	///////////	///////////	///////////




