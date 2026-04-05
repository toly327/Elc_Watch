#include "Bootloader.h"
  


float low_pass_filter_TX(float input)
{float ALPHA=0.03;static float outputw = 0; outputw = ALPHA * input + (1 - ALPHA) * outputw; return outputw;}  
float low_pass_filter_TY(float input)
{float ALPHA=0.03;static float outputw = 0; outputw = ALPHA * input + (1 - ALPHA) * outputw; return outputw;} 
float low_pass_filter_x(float input)
{float ALPHA=0.03;static float outputw = 2; outputw = ALPHA * input + (1 - ALPHA) * outputw; return outputw;} 
float low_pass_filter_y(float input)
{float ALPHA=0.01;static float outputw = 0; outputw = ALPHA * input + (1 - ALPHA) * outputw; return outputw;} 

float low_pass_filter_LOGG(float input)
{float ALPHA=0.02;static float outputw = 0; outputw = ALPHA * input + (1 - ALPHA) * outputw; return outputw;} 

void PASS_Aton(void)
{	
	float temp1,temp2,temp3;
	bool PS1=true,PS2=true;
	
//	for(u8 i;i<100;i++)
//	{	
//		
//		if(PS1)
//		{OLED_DrawRectangle(0,0,2,64,OLED_UNFILLED);OLED_Update();}
//		else
//		{OLED_Clear();OLED_Update();}
//			
//		delay_ms(50);
//	
//		PS1=!PS1;
//	}
	
	PS1=true;
	
	OLED_Clear();OLED_Update();
//	
//	OLED_DrawRectangle(0,0,2,64,OLED_UNFILLED);
//	OLED_Update();
//	delay_ms(100);
//	
//	OLED_Clear();
//	
//	delay_ms(100);	
//	
//	OLED_DrawRectangle(0,0,2,64,OLED_UNFILLED);
//	OLED_Update();
//	delay_ms(300);	
	
	while(temp1<63.8)
	{
		OLED_Clear();
		temp1=low_pass_filter_y(64);
		delay_ms(1);
		OLED_DrawRectangle(low_pass_filter_TX(0),low_pass_filter_TY(0),low_pass_filter_x(2),temp1,OLED_UNFILLED);
		OLED_Update();
	}
	
	while(PS1 || temp2>0.3)
	{	
		OLED_Clear();
		temp1=low_pass_filter_x(96);
		temp3=low_pass_filter_TX(15);
		OLED_DrawRectangle(temp3,low_pass_filter_TY(8),temp1,low_pass_filter_y(48),OLED_UNFILLED);


		
		if(PS1)
		{if(temp1)
			{	temp2=low_pass_filter_LOGG(80);	
				OLED_DrawRectangle(22+80-temp2,16,temp2,16,OLED_FILLED);
				if(temp2>79)PS1=false;
			}		
		}
		else
		{	OLED_ShowString(22,16,"ZHAO-Watch",OLED_8X16_HALF);
			temp2=low_pass_filter_LOGG(0);	
			OLED_DrawRectangle(22,16,temp2,16,OLED_FILLED);	
		}	

		OLED_ClearArea(temp3+temp1,0,128,64);

		OLED_Update();
	}
	
	PS1=true;
	
	while(PS1 || temp2>0.3)
	{	
		OLED_Clear();
		temp1=low_pass_filter_x(96);	
		OLED_DrawRectangle(low_pass_filter_TX(15),low_pass_filter_TY(8),temp1,low_pass_filter_y(48),OLED_UNFILLED);
		OLED_ShowString(22,16,"ZHAO-Watch",OLED_8X16_HALF);
		if(PS1)
		{	temp2=low_pass_filter_LOGG(42);	
			OLED_DrawRectangle(22+42-temp2,34,temp2,8,OLED_FILLED);
			if(temp2>41)PS1=false;	
		}
		else
		{	OLED_ShowString(22,34,"MH-V1.2",OLED_6X8_HALF);
			temp2=low_pass_filter_LOGG(0);	
			OLED_DrawRectangle(22,34,temp2,8,OLED_FILLED);	
		}			
		OLED_Update();
	}	
	
	PS1=true;
	
	while(1)
	{	
		OLED_Clear();	
		OLED_DrawRectangle(low_pass_filter_TX(15),low_pass_filter_TY(8),low_pass_filter_x(96),low_pass_filter_y(48),OLED_UNFILLED);
		OLED_ShowString(22,16,"ZHAO-Watch",OLED_8X16_HALF);
		OLED_ShowString(22,34,"MH-V1.2",OLED_6X8_HALF);
		
		if(PS1)
		{	
			temp1=low_pass_filter_LOGG(96+13);	
			OLED_DrawLine(15,45,temp1,45);
			if(temp1>96+12.8){PS1=false;	temp2=96+12;}
		}
		else
		{
			OLED_DrawLine(15,45,temp1,45);
			temp2*=0.99;
			if(temp2>0.1)OLED_ShowString(17,46,"Initializing",OLED_6X8_HALF);
			else OLED_ShowString(17,46,"OK!",OLED_6X8_HALF);
			OLED_ReverseArea(15,46,temp1-15-temp2,8);
			if(temp2<0.01)break;
		}
	
		OLED_Update();
	}	
	
	PS1=true;
	
	OLED_ReverseArea(low_pass_filter_TX(15),low_pass_filter_TY(8),low_pass_filter_x(96),low_pass_filter_y(48));
	OLED_Update();
	delay_ms(100);
	
	OLED_Clear();
	OLED_DrawRectangle(low_pass_filter_TX(15),low_pass_filter_TY(8),low_pass_filter_x(96),low_pass_filter_y(48),OLED_UNFILLED);
	OLED_Update();
	delay_ms(100);
	
	OLED_Clear();
	OLED_DrawRectangle(low_pass_filter_TX(15),low_pass_filter_TY(8),low_pass_filter_x(96),low_pass_filter_y(48),OLED_FILLED);
	OLED_Update();
	delay_ms(100);
	
	OLED_Clear();
	OLED_Update();
	delay_ms(100);
	
	while(low_pass_filter_LOGG(0)>0.1);
	
	temp1=0.05;
	
	temp3=0;
	
	u8 p=0,c=0;
	int16_t GyrX=1;
	
	while(1)
	{	
		OLED_Clear();
	
		if(temp2>63.7)
		{
			OLED_DrawRectangle(128-30,4,26,6,OLED_UNFILLED);
			if(PS2){temp1*=1.05;OLED_DrawRectangle(128-30+temp1,4,5,6,OLED_FILLED);if(temp1>22)PS2=!PS2;}else{temp1*=0.95;OLED_DrawRectangle(128-30+temp1,4,5,6,OLED_FILLED);if(temp1<0.1)PS2=!PS2;	}
		
			for(u8 i=0;i<2;i++)
			{ for(u8 j=0;j<4;j++)
				{					
					OLED_DrawRectangle(104+i*9,15+j*9,7,7,OLED_UNFILLED);			
				}	
			}
			for(u8 i=0;i<2;i++)
			{ if(p>=c)break;		
				for(u8 j=0;j<4;j++)
					{					
						if(p>=c)break;
						OLED_DrawRectangle(104+i*9,15+j*9,7,7,OLED_FILLED);
						p++;
					}
							
			}
			p=0;
		}
		else temp2=low_pass_filter_LOGG(64);
		
		OLED_DrawLine(64-temp2,0,64+temp2,0);	
		OLED_DrawLine(64-temp2,63,64+temp2,63);	
		
		if(c==0)
		{	
			
			OLED_ShowString(2, 63-12, "AHT20_Init...", OLED_7X12_HALF);
			delay_ms(2);
			temp3+=1;
			if(temp3>100){AHT20_Init();c=1;temp3=0;}
			
		}
		else if(c==1)
		{	
			
			OLED_ShowString(2, 63-12, "MyRTC_Init...", OLED_7X12_HALF);
			delay_ms(1);
		    temp3+=1;
			if(temp3>100){MyRTC_Init();c=2;temp3=0;}
		}
		else if(c==2)
		{	
			
			OLED_ShowString(2, 63-12, "MPU6050_Init...", OLED_7X12_HALF);
			delay_ms(3);		

			temp3+=1;
			if(temp3>100){MPU6050_Init();delay_ms(30);MPU6050_Get_GXData(&GyrX);if(GyrX==0)while(1);MPU6050_RES();c=3;temp3=0;}
			
			
			
		}
		else if(c==3)
		{	
			
			
			OLED_ShowString(2, 63-12, "ADC_Init...", OLED_7X12_HALF);
			delay_ms(1);		
			temp3+=1;
			if(temp3>100){AD_Init();c=4;temp3=0;}
			
		}
		else if(c==4)
		{	
			
			
			
			OLED_ShowString(2, 63-12, "DATA_Loading...", OLED_7X12_HALF);
			delay_ms(3);	
			
			temp3+=1;
			if(temp3>100){Load_Para();c=5;temp3=0;}
			
		}
		else if(c==5)
		{	
			temp3+=1;
			if(temp3>100){c=6;temp3=0;}
			
		}
		else if(c==6)
		{	
			temp3+=1;
			if(temp3>100){c=7;temp3=0;}
			
		}
		else if(c==7)
		{	
			temp3+=1;
			if(temp3>100){c=8;temp3=0;}
			
		}
		else if(c==8)
		{	
			temp3+=1;
			if(temp3>100){c=9;temp3=0;break;}
			
		}

		
					
//		OLED_ShowNum(20,20,temp2,7,OLED_6X8_HALF);
		OLED_Update();		
	}
	
	OLED_UI_FadeoutAllArea();
	
//	uint8_t u=128;
//	while(u)
//	{	
//		u--;
//		OLED_ClearArea(127-u,0,1,64);
//		delay_ms(5);
//		OLED_Update();	
//	}
	
//	
//		OLED_Write_CMD(0xAD);	//设置充电泵
//		OLED_Write_CMD(0x8A);    //0x8A关闭充电泵

//		OLED_Write_CMD(0xAE);	//0xAE关闭显示

//				/*使能WKUP引脚*/
//			PWR_WakeUpPinCmd(ENABLE);	 //使能位于PA0的WKUP引脚，WKUP引脚上升沿唤醒待机模式
//			PWR_EnterSTANDBYMode();		//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
//		
		
	

	OLED_Update();
	
	Delay_ms(1000);
}


