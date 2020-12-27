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
int U_Rmax;//输入电阻最大电压
int U_Rmin;//输入电阻最小电压
int U_out1;//输出电阻闭合电压
int U_out1min;
int U_out2;//输出电阻开路电压
int U_out2min;
int Uout=0;
float Uin_PP=0;
int temp=0;
float Udds=2050;
float Ri=23000;
float Rin=0;//输入电阻
float Ro=33000;
float Rout=0;//输出电阻
int change=0;
int key_i=0;
float Y[31]={45,45,45,41,40,40,39,39,39,39,39,39,39,39,39,39,45,45,48,53,54,64,79,85,99,111,134,140,171,189,204};
float gain=0;
int f[13]={142,143,144,145,146,147,148,149,150,151,152,153,154};
//电阻故障状态
//0为正常normal   1为开路off  2为短路short
int R1_state=0;
int R2_state=0;
int R3_state=0;
int R4_state=0;

//电容故障状态
//0为正常normal   1为开路off  2为两倍twice
int C1_state=0;
int C2_state=0;
int C3_state=0;
int t=0;
//故障显示
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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
 	LCD_Init();			   	//初始化LCD 		
	KEY_Init();				//按键初始化		 	
 	Adc_Init();		  		//ADC初始化
	Dac1_Init();		 	//DAC通道1初始化	
	     

	while(1)
	{
		///////////////////////////////////////////////////////////////
		//按键控制模式切换 （1KHZ正弦波和扫频）
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
		
		
		
		
		//继电器控制//
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
		
		//自动测量与扫频模式切换
		////////////////////////////////////////////////////////////
	
		//////mode：0 自动测量 输入1KHZ正弦波
		if(mode==0)
	 {
		 
		++t;
		if(t==13)
			t=0;
		
		 //求输入电阻
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
			Uin_PP=(float)(Udds-(U_Rmax-U_Rmin)*1000*3.3/4096*0.95);//URin的VPP
			Rin=Udds/Uin_PP*Ri-Ri;
			//////////////////////////////////////////////////////
			/////////////////////////////////////////////////////
			
			
			
		//求输出电阻

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
			
			//求1Khz增益
			gain=(float)((U_out2-U_out2min)/21);
			gain=20*log10(gain);
			
			
			//输入电阻以及输出电阻显示
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
			LCD_ShowxNum(10,40,Rin*0.95,10,16,0);//显示输入电阻
			LCD_ShowxNum(10,90,Rout,10,16,0);//显示输出电阻
			LCD_ShowxNum(110,290,gain,10,16,0);//显示输出增益
			fault();
			POINT_COLOR=BLUE;
			LCD_ShowString(130,40,30,10,16,"Ohms");	
			LCD_ShowString(130,90,30,10,16,"Ohms");	
			LCD_ShowString(210,290,30,10,16,"dB");	
			delay_ms(100);
		}
	 
		
		
		///////////	///////////	///////////	///////////	///////////	///////////
		//mode ：1 扫频模式 绘频幅曲线图
		
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
			LCD_ShowxNum(65,290,f[t],10,16,0);//显示输出增益			
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




