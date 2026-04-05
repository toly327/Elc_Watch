#include "Time_page.h"
#include "stm32f10x.h"               
#include "OLED_UI.h"     
#include "MyRTC.h"
#include "AD.h"
#include "AHT20.h" 

extern OLED_ChangePoint OLED_UI_PageStartPoint;

//定义电压与电池容量关系的数组
const float battery_voltage[31]={ 6.6, 4.35,  4.18, 4.15,4.12,4.10, 4.08, 4.05, 4.03, 3.97, 3.93, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.74, 3.73, 3.72, 3.71, 3.69, 3.66, 3.65, 3.64, 3.63, 3.60, 3.55, 3.50, 3.40};
const uint8_t battery_level[31]={ 255,  100,   100,   98,  96,  93,   90,   87,   85,   80,   75,   70,   65,   60,   55,   50,   45,   42,   35,   30,   25,   20,   17,   14,   11,    8,    6,    5,    4,    2,  0};

float VIN = 3.31;            //STM32电源脚电压，这个也要校准，可以用万用表测量降压芯片的输出3.3v或者直接测量单片机对屏幕的RES脚
float show_BAT = 100;         //上次电池电量
float BAT_volt_CAL = 2.000;//电池电压校准系数  
	
//获取电池电量并显示在指定位置
void Time_page_BAT(int8_t x,int8_t y)
{
	uint16_t ADValue;	
	static uint8_t ADVx=0;
	static float UP_BAT = 100;
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0 ) //如果在充电
	{			
		if(ADVx>3  && ADVx<33){OLED_DrawLine(x+21, y+2, x+21, y+6);}
		if(ADVx>6  && ADVx<36){OLED_DrawLine(x+22, y+2, x+22, y+6);}
		if(ADVx>9  && ADVx<39){OLED_DrawLine(x+23, y+2, x+23, y+6);}
		if(ADVx>12 && ADVx<42){OLED_DrawLine(x+24, y+2, x+24, y+6);}
		if(ADVx>15 && ADVx<45){OLED_DrawLine(x+25, y+2, x+25, y+6);}
		if(ADVx>18 && ADVx<48){OLED_DrawLine(x+26, y+2, x+26, y+6);}
		if(ADVx>21 && ADVx<51){OLED_DrawLine(x+27, y+2, x+27, y+6);}
		if(ADVx>24 && ADVx<54){OLED_DrawLine(x+28, y+2, x+28, y+6);}
		if(ADVx>27 && ADVx<57){OLED_DrawLine(x+29, y+2, x+29, y+6);}
		if(ADVx>30 && ADVx<60){OLED_DrawLine(x+30, y+2, x+30, y+6);}
		                      //一个小动画//
		ADVx++;
		if(ADVx>60)ADVx=0;	
		UP_BAT=100;
	}
	else 
	{
		ADVx=0;
		
		ADValue = AD_GetValue(ADC_Channel_8);
		
		float voltage = ((float)ADValue * VIN ) / 4095 * BAT_volt_CAL ;//得到电池电压
		
//		OLED_ShowFloatNum(x+6, y+1,voltage,2,2,OLED_6X8_HALF);
		float BAT;
		
		for(uint8_t i=0;i<31;i++)//对应数组有31档
		{
			if((voltage<battery_voltage[i]) & (voltage>=battery_voltage[i+1]))
			{
				//线性插值得到电池电量
			   BAT = battery_level[i+1]+( (voltage-battery_voltage[i+1]) * ( (battery_level[i]-battery_level[i+1])/(battery_voltage[i]-battery_voltage[i+1]) ) );
				//此时voltage为电池电量 100 到 0
			   break;
			}
		}
		
		UP_BAT = 0.1 * BAT + (1 - 0.1) * UP_BAT;
		
		
	    if(UP_BAT < show_BAT) //防止电量上下抖动
		{
			show_BAT=UP_BAT;
		}
		else if(UP_BAT - show_BAT> 7)//如果电量增加
		{
			show_BAT=UP_BAT;
		}	
	}
	
	
	
	OLED_DrawRectangle(x+20, y+1, 12, 7, OLED_UNFILLED);//画电池图案
	OLED_DrawLine(x+32, y+3, x+32, y+5);               //画电池图案

	
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==1 )//如果没在充电
	{
	
		if(show_BAT>9)OLED_DrawLine(x+21, y+2, x+21, y+6);
		if(show_BAT>19)OLED_DrawLine(x+22, y+2, x+22, y+6);
		if(show_BAT>29)OLED_DrawLine(x+23, y+2, x+23, y+6);
		if(show_BAT>39)OLED_DrawLine(x+24, y+2, x+24, y+6);
		if(show_BAT>49)OLED_DrawLine(x+25, y+2, x+25, y+6);
		if(show_BAT>59)OLED_DrawLine(x+26, y+2, x+26, y+6);
		if(show_BAT>69)OLED_DrawLine(x+27, y+2, x+27, y+6);
		if(show_BAT>79)OLED_DrawLine(x+28, y+2, x+28, y+6);
		if(show_BAT>89)OLED_DrawLine(x+29, y+2, x+29, y+6);//根据电池电量来绘制格子
	
	
		if(show_BAT >= 100 || GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)==0)//3位数//或充电完成
		{
			OLED_DrawLine(x+30, y+2, x+30, y+6);
			OLED_ShowNum(x, y+1, 100, 3, OLED_6X8_HALF);			
		}
		else OLED_ShowNum(x+6, y+1, show_BAT, 2, OLED_6X8_HALF);	
    }
	else//当充电时不显示电量，因为不准
	{
		OLED_ShowString(x+6, y+1, ">>", OLED_6X8_HALF);
	}
	
}


//在指定位置显示表盘数字
void Time_page_figures(int8_t x,int8_t y,u8 i,u8 p)
{	
	if(p)
	{
		switch(i)
		{ 
			case 1:OLED_ShowImage(x, y, 13, 30,t1);break;
			case 2:OLED_ShowImage(x, y, 13, 30,t2);break;
			case 3:OLED_ShowImage(x, y, 13, 30,t3);break;
			case 4:OLED_ShowImage(x, y, 13, 30,t4);break;
			case 5:OLED_ShowImage(x, y, 13, 30,t5);break;
			case 6:OLED_ShowImage(x, y, 13, 30,t6);break;
			case 7:OLED_ShowImage(x, y, 13, 30,t7);break;
			case 8:OLED_ShowImage(x, y, 13, 30,t8);break;
			case 9:OLED_ShowImage(x, y, 13, 30,t9);break;
			case 0:OLED_ShowImage(x, y, 13, 30,t0);break;
		 }
	}
	else
	{
		switch(i)
		{ 
			case 1:OLED_ShowImage(x, y, 9, 21,i1);break;
			case 2:OLED_ShowImage(x, y, 9, 21,i2);break;
			case 3:OLED_ShowImage(x, y, 9, 21,i3);break;
			case 4:OLED_ShowImage(x, y, 9, 21,i4);break;
			case 5:OLED_ShowImage(x, y, 9, 21,i5);break;
			case 6:OLED_ShowImage(x, y, 9, 21,i6);break;
			case 7:OLED_ShowImage(x, y, 9, 21,i7);break;
			case 8:OLED_ShowImage(x, y, 9, 21,i8);break;
			case 9:OLED_ShowImage(x, y, 9, 21,i9);break;
			case 0:OLED_ShowImage(x, y, 9, 21,i0);break;
		 }
	}
}



void Time_page_Timer(int8_t x,int8_t y)//获取时间并在指定位置显示
{

		MyRTC_ReadTime();
		uint16_t i=MyRTC_Time[3];
		uint16_t p= i ;
	
	    if(i  > 12) 
		{
			i = i-12 ;
			p = i ;
			
			OLED_ShowString(x+109, y+21, "PM", OLED_6X8_HALF);
		} 
		else OLED_ShowString(x+109, y+21, "AM", OLED_6X8_HALF);
		
	
		i= i%10;
		Time_page_figures(x+19,y+0,i,1);
		i=(p-i)/10;
		Time_page_figures(x+0,y+0,i,1);
		
		OLED_DrawRectangle(x+37, y+6, 2, 4, OLED_UNFILLED);
		OLED_DrawRectangle(x+37, y+19, 2, 4, OLED_UNFILLED);
		
		i=MyRTC_Time[4];
		p=i;
		i= i%10;
		Time_page_figures(x+63,y+0,i,1);
		i=(p-i)/10;
		Time_page_figures(x+44,y+0,i,1);
		
		i=MyRTC_Time[5];
		p=i;
		i= i%10;
		Time_page_figures(x+95,y+8,i,0);
		i=(p-i)/10;
		Time_page_figures(x+81,y+8,i,0);   
}

void Time_page_date(int8_t x,int8_t y)
{
		OLED_ShowNum(x, y, MyRTC_Time[0], 4, OLED_6X8_HALF);
		OLED_ShowNum(x+26, y, MyRTC_Time[1], 2, OLED_6X8_HALF);
	    OLED_ShowNum(x+40, y, MyRTC_Time[2], 2, OLED_6X8_HALF);
}

void Time_page_week(int8_t x,int8_t y)
{	
	
		uint16_t week = MyRTC_Time[6];
	
		OLED_ShowString(x, y, "week:", OLED_6X8_HALF);
		switch(week)
		{ 
			case 1:OLED_ShowString(x+30, y, "1", OLED_6X8_HALF);break;
			case 2:OLED_ShowString(x+30, y, "2", OLED_6X8_HALF);break;
			case 3:OLED_ShowString(x+30, y, "3", OLED_6X8_HALF);break;
			case 4:OLED_ShowString(x+30, y, "4", OLED_6X8_HALF);break;
			case 5:OLED_ShowString(x+30, y, "5", OLED_6X8_HALF);break;
			case 6:OLED_ShowString(x+30, y, "6", OLED_6X8_HALF);break;
			case 0:OLED_ShowString(x+30, y, "R", OLED_6X8_HALF);break;
		 }
	
}

void Time_page_AHT20(int8_t x,int8_t y)//显示温湿度
{
	int Numtemp1,Numtemp2;
	static uint16_t del=400;
	if(del>300)
	{
		AHT20_Get_data();//获取温湿度信息
		del=0;
	}
	else del++;
	
		// 获取个位数字  湿度
		Numtemp2 = AHT20humidity % 10;  
		// 获取除个位外的其他数字  
		Numtemp1 = AHT20humidity / 10;  
		OLED_ShowNum(x,y,Numtemp1,2,OLED_6X8_HALF);
		OLED_DrawPoint(x+12+1,y+6);
		OLED_DrawPoint(x+12+1,y+7);
		OLED_DrawPoint(x+12+2,y+6);
		OLED_DrawPoint(x+12+2,y+7);
		OLED_ShowNum(x+12+3,y,Numtemp2,1,OLED_6X8_HALF);
		OLED_ShowString(x+18+3,y,"%",OLED_6X8_HALF);
		

		// 获取个位数字  温度
		Numtemp2 = AHT20temperature % 10;  
		// 获取除个位外的其他数字  
		Numtemp1 = AHT20temperature / 10; 		
		OLED_ShowNum(x+30+2,y,Numtemp1,2,OLED_6X8_HALF);
		OLED_DrawPoint(x+12+1+30+2,y+6);
		OLED_DrawPoint(x+12+1+30+2,y+7);
		OLED_DrawPoint(x+12+2+30+2,y+6);
		OLED_DrawPoint(x+12+2+30+2,y+7);
		OLED_ShowNum(x+12+3+30+2,y,Numtemp2,1,OLED_6X8_HALF);
		OLED_DrawPoint(x+18+3+30+1+2,y);
		OLED_ShowString(x+18+3+30+2+2,y,"C",OLED_6X8_HALF);			
}


// 4低通滤波器指示器函数
void low_pass_filter(float inputX,float inputY,float inputx,float inputy,uint8_t i)
{  
   #define ALPHA 0.2
	
	static float Time_pageoutX = 0; // 存储上一个输出值 
	static float Time_pageoutY = 0;
	static float Time_pageoutx = 0; 
	static float Time_pageouty = 0; 


	//OLED_ReverseArea(outX,outY,outx,outy);
	 
    // 计算输出值  
	if(i!=2)
	{
		Time_pageoutX = ALPHA * (float)inputX + (1 - ALPHA) * Time_pageoutX;  
		Time_pageoutY = ALPHA * (float)inputY + (1 - ALPHA) * Time_pageoutY; 
		Time_pageoutx = ALPHA * (float)inputx + (1 - ALPHA) * Time_pageoutx; 
		Time_pageouty = ALPHA * (float)inputy + (1 - ALPHA) * Time_pageouty; 
		
		
		if(i)OLED_ReverseArea(Time_pageoutX,Time_pageoutY,Time_pageoutx+1,Time_pageouty+1);
		else OLED_DrawRectangle2(Time_pageoutX,Time_pageoutY,Time_pageoutx+1,Time_pageouty+1);
	}
	else
	{	
		Time_pageoutX = inputX;
		Time_pageoutY = inputY;
		Time_pageoutx = inputx;
	    Time_pageouty = inputy;
	}
}  

void Time_page_Adjust(void)
{
	
	
	OLED_UI_FadeoutAllArea();
	OLED_Brightness_DODO();
	
	low_pass_filter(0,0,0,0,2);
	
	for(u8 i=0;i<7;i++){MyRTC_Time2[i]=MyRTC_Time[i];}
	
	int8_t p;
	uint8_t i=0;
	uint16_t t=0;
	while(1)
	{
		OLED_Clear();
		OLED_ShowNum(6, 5, MyRTC_Time2[0], 4, OLED_7X12_HALF);
		OLED_ShowNum(6+3+7*4, 5, MyRTC_Time2[1], 2, OLED_7X12_HALF);
	    OLED_ShowNum(6+3+7*4+3+7*2, 5, MyRTC_Time2[2], 2, OLED_7X12_HALF);
		OLED_ShowNum(6, 30, MyRTC_Time2[3], 2, OLED_7X12_HALF);
		OLED_ShowNum(6+3+7*2, 30, MyRTC_Time2[4], 2, OLED_7X12_HALF);
	    OLED_ShowNum(6+3+7*2+3+7*2, 30, MyRTC_Time2[5], 2, OLED_7X12_HALF);
		
			switch(i)
			{ 
				case 0:low_pass_filter(5, 5-1, 7*4+2, 12+2,1);break;
				case 1:low_pass_filter(6+3+7*4, 5-1, 7*2+2, 12+2,1);break;
				case 2:low_pass_filter(6+3+7*4+3+7*2, 5-1, 7*2+2, 12+2,1);break;
				case 3:low_pass_filter(5, 30-1, 7*2+2, 12+2,1);break;
				case 4:low_pass_filter(6+3+7*2, 30-1, 7*2+2, 12+2,1);break;
				case 5:low_pass_filter(6+3+7*2+3+7*2, 30-1, 7*2+2, 12+2,1);break;
			}
			
		Delay_ms(20);
	    OLED_Brightness_UPUP();		
		OLED_Update();	
	
		p = menu_Roll_event();   
	   if( p == 1){ 
		   
			   t = MyRTC_Time2[i];
			   t++;
			   MyRTC_Time2[i] = t;
		   
	   }	 
       else if( p == -1){
		   
			   t = MyRTC_Time2[i];
			   t--;
			   if(t>10000)t=0;
			   MyRTC_Time2[i] = t;
		   
	   }	
		p=menu_Enter_event();
		if(p==1)
		{                      
			i++;
			if(i>=6)
			{		
				MyRTC_SetTime();
				
				OLED_UI_FadeoutAllArea();
				OLED_UI_PageStartPoint.CurrentPoint.X = 127;
				delay_ms(300);
				
				return;
			}
			

		}
		
	}
	
}


void Time_page_small_Timer(u8 x,u8 y)//获取时间并在指定位置显示
{
//	static uint8_t yans=45;
//	
//	if(yans>30)
//	{
//		yans=0;
//		MyRTC_ReadTime();
		uint16_t i=MyRTC_Time[3];
	
	    if(i  > 12) 
		{
			i = i-12 ;
			
			OLED_ShowString(x+42, y, "PM", OLED_6X8_HALF);
		} 
		else OLED_ShowString(x+42, y, "AM", OLED_6X8_HALF);
		
			
		OLED_ShowNum(x, y, i, 2, OLED_6X8_HALF);
			
		i=MyRTC_Time[4];
		
		OLED_ShowNum(x+14, y, i, 2, OLED_6X8_HALF);
		
		i=MyRTC_Time[5];
		
		OLED_ShowNum(x+28, y, i, 2, OLED_6X8_HALF);
//	}	
//	else yans++;
	
}


void Time_page_io_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;//电池充电检测脚
	GPIO_Init(GPIOB,&GPIO_InitStructure);   
	
		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;     //电池检测电路控制到地
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);	
}

float Time_page_PXXXX=64;
void Time_page(void)
{
	Time_page_PXXXX*=0.95;
		   Time_page_BAT(89+Time_page_PXXXX/2,0);
		   Time_page_Timer(5-Time_page_PXXXX,18);
		   Time_page_date(0-Time_page_PXXXX/2,56);
		   Time_page_week(90+Time_page_PXXXX/2,57);
		   Time_page_AHT20(0-Time_page_PXXXX/3,0);
	
}


	
	//获取时间并在指定位置显示
