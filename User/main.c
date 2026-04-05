#include "OLED_UI.h"
#include "OLED_UI_MenuData.h"
//#include "Bootloader.h"
#include "delay.h"
#include "OLED.h"
#include "MyRTC.h"
#include "AD.h"
#include "AHT20.h" 
#include "stm32_power.h" 
#include "MPU6050.h"
#include "YXMP3.h"
#include "W25Q128_SPI1.h"


//项目赶着结项，先不整理程序了，乱就乱吧 (



extern MenuPage*  CurrentMenuPage;

void STM32_rouse(void);
int16_t GyroX,UPGyroX=0;
bool hold = false;

int main(){
	//初始化OLED显示屏
	
	OLED_Init();
	
	MyRTC_Init();	
	
	delay_init();
	
	Key_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);      // 开启AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //重映射配置,用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试

	
	//本来有一段开机动画的，但是空间不太够就先不要了
//	/*备份寄存器访问使能*/
//	PWR_BackupAccessCmd(ENABLE);							//使用PWR开启对备份寄存器的访问
//	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)			//通过写入备份寄存器的标志位，判断是否是第一次配置
//	{	PASS_Aton(); }
//	MyRTC_Init();
	
	YXMP3_PWROFF();
	
	//开机自检+动画
	
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //配置NVIC为分组2
	 
	W25Q128_Init();
	
	AHT20_Init();
	
	Time_page_io_init();
	
	STM32_rouse();                     //唤醒休眠所用的中断配置
	
	AD_Init();				       //AD转换器初始化
	
	Load_Para();
	//20ms的定时中断,用于OLED_UI的中断函数和其他功能
	Timer_Init();
	//按键初始化	
	//设置当前页面的结构体指针
	CurrentMenuPage = &MainMenuPage;	//设置当前页面的结构体指针
	//初始化菜单页面参数
	CurrentMenuPageInit();
	uint8_t p;
	uint16_t oodd=100;	
	OLED_Brightness_DODO();
	
	extern bool RAWA;
	extern float Time_page_PXXXX;
	while(oodd)
	{
		OLED_Clear();
		Time_page();	
		OLED_Brightness_UPUP();
		oodd--;
		OLED_Update();
	}	
	while(1){ 
	//清屏
	OLED_Clear();
		

	
	Time_page();	
	p=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
		
	if(oodd>800)
	{p=1;oodd=0;}
	else oodd++;
	if(hold)oodd=0;
	
	if(RAWA)//落腕熄屏
	{
		MPU6050_Get_GXData(&GyroX);
		if( GyroX < -4000 )
		{
			p = 1;
			oodd=0;
		}
	}		
	if( p == 1)
	{
		
		OLED_UI_FadeoutAllArea();
		Time_page_PXXXX=64;
		POWER_OFF=1;
		Power_OFF();
		OLED_Brightness_DODO();
		oodd=100;
		while(oodd)
		{
			OLED_Clear();
			Time_page();	
			OLED_Brightness_UPUP();
			oodd--;
			delay_ms(5);
			OLED_Update();
		}	
	}	

		

	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
	{
		oodd=0;
		OLED_UI_EN=true;
		OLED_UI_FadeoutAllArea();
		Time_page_PXXXX=64;
		OLED_UI_MainLoop();
		
		delay_ms(100);
		OLED_Brightness_DODO();
	}	
	OLED_Brightness_UPUP();	
	delay_ms(5);
//	//刷屏
	OLED_Update();
	}
}

//中断处理函数
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)	   //外部中断0号线触发的中断，摇杆下拨
	{
		POWER_OFF=0;	
		EXTI_ClearITPendingBit(EXTI_Line0);		//清除中断标志位																								
	}
}



//中断函数
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		OLED_UI_InterruptHandler();		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}


extern uint8_t Pick_up;//拿起检测标志位
extern int16_t RAWAtri;//抬腕触发值
extern int16_t RAWAtrigger;//实际抬腕触发值
uint8_t Addtrigger=0;//手表不动累加值

void EXTI9_5_IRQHandler(void)
{	
	
	if (EXTI_GetITStatus(EXTI_Line7) == SET)		//外部中断7号线触发的中断，MPU6050
	{		
		SystemInit();
	    MPU6050_Get_GXData(&GyroX);//读取x轴陀螺仪数据
		if(GyroX>RAWAtrigger)//如果大于触发值
		{ 	
			POWER_OFF = 0; //取消待机
			Addtrigger= 0;
			RAWAtrigger = RAWAtri*70;//恢复触发值
		}
		else if(Pick_up)
		{
			if(UPGyroX-5<GyroX && GyroX<UPGyroX+5)//如果手表没有动
			{ 
				if(Addtrigger<80)
				{	
					Addtrigger++;//累加80次，实际大约5秒
				}
				else RAWAtrigger=UPGyroX+5;//变为运动触发模式，一但检测到有点运动，马上唤醒
			}
			else {Addtrigger = 0;}
			UPGyroX=GyroX;
			
	    }
		EXTI_ClearITPendingBit(EXTI_Line7);		//清除中断标志位																								
	}

}


//用于唤醒的外部中断，统一在这里配置
void STM32_rouse(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA0在Key_Init();里面已经配置过了，所以直接从PA7开始	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;	//MPU6050用
	GPIO_Init(GPIOB,&GPIO_InitStructure);
		
	/*AFIO选择中断引脚*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);//将外部中断的0号线映射到GPIOA
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);//将外部中断的7号线映射到GPIOB
	
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;				     //定义结构体变量
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			    //指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   //指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//指定外部中断线为上升沿触发

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;			//选择配置外部中断
	EXTI_Init(&EXTI_InitStructure);					   //将结构体变量交给EXTI_Init，配置EXTI外设
	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //指定外部中断线为下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;			    //选择配置外部中断
	EXTI_Init(&EXTI_InitStructure);						   //将结构体变量交给EXTI_Init，配置EXTI外设
	
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;				       //定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	    //指定NVIC线路的响应优先级为2
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	  //选择配置NVIC的0线
	NVIC_Init(&NVIC_InitStructure);						 //配置NVIC
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//指定NVIC线路的抢占优先级为
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	    //指定NVIC线路的响应优先级为
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  
	NVIC_Init(&NVIC_InitStructure);		
}

