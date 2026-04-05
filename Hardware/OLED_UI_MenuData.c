#include "OLED_UI_MenuData.h"
#include "OLED_UI.h"

/*此文件用于存放菜单数据。实际上菜单数据可以存放在任何地方，存放于此处是为了规范与代码模块化*/

// ColorMode 是一个在OLED_UI当中定义的bool类型变量，用于控制OLED显示的颜色模式， DARKMODE 为深色模式， LIGHTMOOD 为浅色模式。这里将其引出是为了创建单选框菜单项。
extern bool ColorMode;
extern bool OLED_UI_ShowFps;
// OLED_UI_Brightness 是一个在OLED_UI当中定义的int16_t类型变量，用于控制OLED显示的亮度。这里将其引出是为了创建调整亮度的滑动条窗口，范围0-255。
extern int16_t OLED_UI_Brightness;
float testfloatnum = 0.5;
int16_t testintnum = 1;
#define SPEED 5

extern OLED_ChangePoint OLED_UI_PageStartPoint;

//*******************手电筒***************************************//

int8_t mubz=50;//手电筒默认亮度
void shoudt(void)
{	
	int8_t p;
	uint16_t temp;
	
	OLED_UI_FadeoutAllArea();
	OLED_Brightness_DODO();
	
	OLED_Clear();	   
	OLED_Update();
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseSrtructure;        //TIM时间基数初始化结构体
	TIM_OCInitTypeDef	TIM_OCInitStructure;                //TIM输出比较功能结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //使能GPIOA外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);    //重映射需开启AFIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);    //使能TIM3时钟 
	
	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);
	
	/* 端口（GPIO)设置 */
	GPIO_InitTypeDef  GPIO_InitStructure;                   //GPIO初始化结构体
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                //PA7=T3 Ch2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;        //速度 50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;          //复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);                  //初始化 GPIOA	
	
	/* TIM时间基数初始化 */
	TIM_TimeBaseSrtructure.TIM_Period= 100 - 1;           //计数器 TIMx_CNT计数 从0累加到ARR次后溢出，设置自动重装载值
	TIM_TimeBaseSrtructure.TIM_Prescaler= 72 - 1;        //设置预分频系数 PSC
	TIM_TimeBaseSrtructure.TIM_ClockDivision= 0;                       //采用时钟分割，见库函数。TIM_Clock_Division_CKD定义，查手册 00:Tdts=Tck_int:01:Tdts=2*Tck_int(2分频）
	TIM_TimeBaseSrtructure.TIM_CounterMode=TIM_CounterMode_Up;        //TIM 向上计数模式溢出
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseSrtructure);					  //初始化TIM3的时钟

	/* TIM外设初始化 */
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;     			        //选择定时器模式：TIM 脉冲宽度调制模式1,模式1为比较低输出。
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;           //输出极性：TIM输出比较极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = mubz ;                            //设置占空比,即CCR
	
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
		
	/* 初始化CH1 */
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);                  //CH2 预装载使能
	TIM_ARRPreloadConfig(TIM2,ENABLE);                            //使能 TIMx在ARR上的预装载寄存器

	TIM_Cmd(TIM2, ENABLE);                                    //使能 TIM3	
	

	float GP=0 , GP_target;                 //条条长度，目标条条长度
	
	while(1)
	{	
	   OLED_Clear();
		
	   OLED_ShowNum(19,30,mubz,3,OLED_8X16_HALF);
	   OLED_ShowString(45,30,"%",OLED_8X16_HALF);	
		
		GP_target = mubz * 0.9 ;
		if(GP < GP_target)
		{ 
			if( GP_target - GP > 25 ) GP += 5 ;
			else if( GP_target - GP > 7 ) GP += 2.4 ;
			else if( GP_target - GP > 0 ) GP += 0.5 ;
		}
		if(GP > GP_target)
		{ 	
			if( GP - GP_target > 25 ) GP -= 5 ;
			else if( GP - GP_target > 7 ) GP -= 2.4 ;
			else if( GP - GP_target > 0 ) GP -= 0.5 ;
		}
		OLED_DrawRectangle(17, 50, 94 , 10, OLED_UNFILLED);
		OLED_DrawRectangle(19, 52, GP , 6, OLED_FILLED);
		
	   OLED_Brightness_UPUP();	
	   OLED_Update();
		
	   Delay_ms(10);
		
	   p = menu_Roll_event();   
	   if( p == 1){ 
		   
		   mubz++;
		   if(mubz>100)mubz=100;
		   TIM_SetCompare4(TIM2, (uint8_t)mubz);//写CRR
		   
		   if(mubz>90)Delay_ms(10);
		   
	   }	
       else if( p == -1){ 
			mubz--;
		    if(mubz<0)mubz=0;
		    TIM_SetCompare4(TIM2, (uint8_t)mubz);//写CRR
		   
		   if(mubz<20)Delay_ms(60);
	   }	
	   
		p=Key_GetEnterStatus();
		if(p == 0){
			
			OLED_UI_FadeoutAllArea();
		
//			ResetEnterFlag();
			
			TIM_SetCompare4(TIM2, 0);//写CRR
					
			temp = (0xAE<<8) | mubz;
			
			/*备份寄存器访问使能*/
			PWR_BackupAccessCmd(ENABLE);
			BKP_WriteBackupRegister(BKP_DR2, temp);//在备份寄存器写入自己规定的标志位，用于判断手电筒亮度是不是第一次执行配置		
			
			OLED_UI_PageStartPoint.CurrentPoint.X = 127;		
			
			Delay_ms(300);
			
			TIM_Cmd(TIM2, DISABLE);//TIM失能
			
			
			
			return;	
		}	
	}
}

//*******************秒表***************************************//
uint8_t timedll[4];
	uint8_t timedlll1[4];
		uint8_t timedlll2[4];
			uint8_t timedlll3[4];

bool timedllKK = false;
void miaobiaotimedllKKO(void)
{	
	for(u8 i=0;i<4;i++)
	{	
		timedll[i]=0;
		timedlll1[i]=0;
		timedlll2[i]=0;
		timedlll3[i]=0;
	}
}

void miaobiao(void)
{	
	int8_t p;	

	OLED_UI_FadeoutAllArea();
	OLED_Brightness_DODO();


	float GP=127;                 //条条长度，目标条条长度
	
	while(1)
	{	
	   OLED_Clear();
		
		GP*=0.9;
		
		OLED_Printf(2+10+GP,2,OLED_8X16_HALF,"%02d:%02d:%02d:%02d",timedll[3],timedll[2],timedll[1],timedll[0]);
		OLED_Printf(2+3+10-GP,2+16+2,OLED_7X12_HALF,"%02d:%02d:%02d:%02d",timedlll1[3],timedlll1[2],timedlll1[1],timedlll1[0]);
		OLED_Printf(2+3+10+GP,2+16+2+12+2,OLED_7X12_HALF,"%02d:%02d:%02d:%02d",timedlll2[3],timedlll2[2],timedlll2[1],timedlll2[0]);
		OLED_Printf(2+3+10-GP,2+16+2+12+2+12+2,OLED_7X12_HALF,"%02d:%02d:%02d:%02d",timedlll3[3],timedlll3[2],timedlll3[1],timedlll3[0]);
	
	   OLED_Brightness_UPUP();	
	   OLED_Update();
		
	   Delay_ms(15);
		
	   p = menu_Roll_event();   
	   if( p == 1){ 		   
			for(u8 i=0;i<4;i++)
			{	
				timedlll3[i]=timedlll2[i];
				timedlll2[i]=timedlll1[i];
				timedlll1[i]=timedll[i];
			}
			Delay_ms(100);
	   }	
       else if( p == -1){ 		   
		    timedllKK = false;
			miaobiaotimedllKKO();   
	   }	
	   
		p=menu_Enter_event();
		if(p == 1)
		{
			timedllKK=!timedllKK;
		}
		
		if(p == 2)
		{
			timedllKK = false;
			miaobiaotimedllKKO();
			OLED_UI_FadeoutAllArea();
			OLED_UI_PageStartPoint.CurrentPoint.X = 127;
			Delay_ms(300);
			return;	
		}
	}
}




//*******************翻腕亮屏***************************************//


bool RAWA=0;//抬腕唤醒启用标志位
bool Pick_up=1;//拿起检测标志位
int16_t RAWAtri=110;//抬腕触发值
int16_t RAWAtrigger;//实际抬腕触发值

void Raise_Wake(void)
{
		if(RAWA)
		{
			MPU6050_CYCLe_Init();
		}
		else
		{
			MPU6050_RES();
			Pick_up=0;
			RAWAtrigger=RAWAtri*70;
			PWR_BackupAccessCmd(ENABLE);
			BKP_WriteBackupRegister(BKP_DR4, (0xAC<<8)|RAWAtri);//将触发值写入BKP寄存器
		}
	
}

void Raise_Wake_trigger(void)
{
	uint16_t temp;
	RAWAtrigger=RAWAtri*70;//将抬腕触发值对应到实际触发值，满值17850
	/*备份寄存器访问使能*/
	PWR_BackupAccessCmd(ENABLE);
	temp = (0xAC<<8) | RAWAtri;
	BKP_WriteBackupRegister(BKP_DR4, temp);//将触发值写入BKP寄存器
}



//*******************Game_Of_Life_Play***************************************//

extern uint8_t OLED_DisplayBuf[8][128];		//拿来吧你

uint8_t Nodes_Cache[8][128];				//计算缓存

void Update_Display(void)					//上传屏幕
{
	memcpy(OLED_DisplayBuf, Nodes_Cache, 1024);
}

void Point_life(uint8_t X, uint8_t Y)		//写点生
{
	Nodes_Cache[(Y/8)][X] |= (0x01 << (Y%8));
}

void Point_death(uint8_t X, uint8_t Y)		//写点死
{
	Nodes_Cache[(Y/8)][X] &= ~(0x01 << (Y%8));
}

uint8_t CountPointRound(uint8_t X, uint8_t Y)		//统计点周围细胞数量
{
	return ( 
	OLED_GetPoint(X-1, Y-1) + 	OLED_GetPoint(X  , Y-1) + 	OLED_GetPoint(X+1, Y-1) + 
	OLED_GetPoint(X-1, Y  ) + 								OLED_GetPoint(X+1, Y  ) + 
	OLED_GetPoint(X-1, Y+1) + 	OLED_GetPoint(X  , Y+1) + 	OLED_GetPoint(X+1, Y+1)
	);
}

void Game_Of_Life_Turn(void)		//刷新游戏回合
{
	uint8_t x, y;
	uint8_t Count;
	
	for(y = 0; y < 64; y ++)
	{
		for(x = 0; x < 128; x ++)
		{
			Count = CountPointRound(x, y);
			if(OLED_GetPoint(x, y))
			{
				if(Count < 2 || Count > 3)
				{
					Point_death(x, y);
				}
			}
			else
			{
				if(Count == 3)
				{
					Point_life(x, y);
				}
			}
		}
	}	
	Update_Display();
}


void Game_Of_Life_Play(void)		//游戏开始
{
	OLED_UI_FadeoutAllArea();
	OLED_Brightness_DODO();

	uint8_t x1 = 8, x2 = 16, y1 = 32, y2 = 32;
	int8_t shift = -1;
	
	
	OLED_Clear();
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//遍历8页
	{
		for (i = 0; i < 128; i ++)			//遍历128列
		{
			Nodes_Cache[j][i] = 0x00;	//将显存数组数据全部清零
		}
	}

	int8_t p;
	while(1)
	{
		Game_Of_Life_Turn();
		
		if(shift > 0) {y2 += menu_Roll_event()*8;}
		else {x2 += menu_Roll_event()*8;}
		x2 %= 128;
		y2 %= 64;
		OLED_DrawLine(x1, y1, x2, y2);
		if((x2 - x1) > 1) {x1 += (x2 - x1) / 8 + 1;}
		else if((x2 - x1) < -1) {x1 += (x2 - x1) / 8 - 1;}
		else {x1 = x2;}
		if((y2 - y1) > 1) {y1 += (y2 - y1) / 2 +1;}
		else if((y2 - y1) < -1) {y1 += (y2 - y1) / 2 - 1;}
		else{y1 = y2;}
		
		OLED_Brightness_UPUP();
		
		OLED_Update();

		//Delay_ms(100);
		
		p=menu_Enter_event();
		if(p==1) {shift = -shift;}	
		else if ( p==2 ){
			OLED_UI_FadeoutAllArea();
			OLED_UI_PageStartPoint.CurrentPoint.X = 127;
			Delay_ms(300);
			return;
		}	
	}
}




//关于窗口的结构体
MenuWindow SetBrightnessWindow = {
	.General_Width = 80,								//窗口宽度
	.General_Height = 28, 							//窗口高度
	.Text_String = "翻腕速度",					//窗口标题
	.Text_FontSize = OLED_UI_FONT_12,				//字高
	.Text_FontSideDistance = 4,							//字体距离左侧的距离
	.Text_FontTopDistance = 3,							//字体距离顶部的距离
	.General_WindowType = WINDOW_ROUNDRECTANGLE, 	//窗口类型
	.General_ContinueTime = 4.0,						//窗口持续时间

	.Prob_Data_Int = &RAWAtri,				//显示的变量地址
	.Prob_DataStep = 5,								//步长
	.Prob_MinData = 5,									//最小值
	.Prob_MaxData = 255, 								//最大值
	.Prob_BottomDistance = 3,							//底部间距
	.Prob_LineHeight = 8,								//进度条高度
	.Prob_SideDistance = 4,								//边距
};


/**
 * @brief 创建窗口
 */
void BrightnessWindow(void){
	OLED_UI_CreateWindow(&SetBrightnessWindow);
	Raise_Wake_trigger();
}


//主LOGO移动的结构体
OLED_ChangePoint LogoMove;
//主LOGO文字移动的结构体
OLED_ChangePoint LogoTextMove;
//welcome文字移动的结构体
OLED_ChangePoint WelcomeTextMove;

extern OLED_ChangePoint OLED_UI_PageStartPoint ;



/**
 * @brief 返回表盘
 */
void ReturnTimepageWindow(void){
	OLED_UI_EN=false;
	OLED_UI_FadeoutAllArea();
}




//设置菜单项的辅助显示函数
void SettingAuxFunc(void){
			
//	//在规定位置显示LOGO
//	if(fabs(OLED_UI_PageStartPoint.CurrentPoint.X - OLED_UI_PageStartPoint.TargetPoint.X) < 4){
//		
//		if(tp)TY-=0.5;
//		else TY+=0.5;
//		
		LogoMove.TargetPoint.X = 0;
		LogoMove.TargetPoint.Y = 0;
//		
//		if(TY>64)tp=1;
//		else if(TY<0)tp=0;
//		
//	}
	
	//将LOGOTEXT移动到屏幕右侧看不见的地方
	LogoTextMove.TargetPoint.X = 129;
	LogoTextMove.TargetPoint.Y = 0;
	//将Welcome文字移动到屏幕底部看不见的地方
	WelcomeTextMove.TargetPoint.X = 128;
	WelcomeTextMove.TargetPoint.Y = 0;
	
	ChangePoint(&LogoMove);
	OLED_ShowImageArea(LogoMove.CurrentPoint.X,LogoMove.CurrentPoint.Y,32,64,0,0,128,128,OLED_UI_SettingsLogo);
	
//	ChangePoint(&LogoTextMove);
//	OLED_ShowImageArea(LogoTextMove.CurrentPoint.X,LogoTextMove.CurrentPoint.Y,26,64,0,0,128,128,OLED_UI_LOGOTEXT64);
//	ChangePoint(&WelcomeTextMove);
//	OLED_ShowImageArea(WelcomeTextMove.CurrentPoint.X,WelcomeTextMove.CurrentPoint.Y,16,64,0,0,128,128,OLED_UI_LOGOGithub);
}


//主菜单的辅助显示函数
void MainAuxFunc(void){
	//不显示
	LogoMove.TargetPoint.X = -200;
	LogoMove.TargetPoint.Y = 0;
	LogoMove.CurrentPoint.X = -200;
	LogoMove.CurrentPoint.Y = 0;

	LogoTextMove.TargetPoint.X = 129;
	LogoTextMove.TargetPoint.Y = 0;
	LogoTextMove.CurrentPoint.X = 129;
	LogoTextMove.CurrentPoint.Y = 0;
	
	WelcomeTextMove.TargetPoint.X = 128;
	WelcomeTextMove.TargetPoint.Y = 0;
	WelcomeTextMove.CurrentPoint.X = 128;
	WelcomeTextMove.CurrentPoint.Y = 0;
}

//主菜单的菜单项
MenuItem MainMenuItems[] = {

	{.General_item_text = "返回表盘",.General_callback = ReturnTimepageWindow,.General_SubMenuPage =NULL,.Tiles_Icon = Image_BIPAN},
	{.General_item_text = "电压表",.General_callback = voltage_mt,.General_SubMenuPage = NULL,.Tiles_Icon = Image_ADDV},
	{.General_item_text = "示波器",.General_callback = voltage_waveform,.General_SubMenuPage = NULL,.Tiles_Icon = Image_ADDV2},
	{.General_item_text = "[MUSIC]",.General_callback = YXMP3_Homepage,.General_SubMenuPage = NULL,.Tiles_Icon = Image_MUSIC},
	{.General_item_text = "[VIDEO]",.General_callback = NULL,.General_SubMenuPage = &VIDEOMenuPage,.Tiles_Icon = Image_VIDEO},
	{.General_item_text = "[LED]",.General_callback = shoudt,.General_SubMenuPage = NULL,.Tiles_Icon = Image_flashlight},
	{.General_item_text = "秒表",.General_callback = miaobiao,.General_SubMenuPage = NULL,.Tiles_Icon = Image_second},
//	{.General_item_text = "收款码",.General_callback = NULL,.General_SubMenuPage = NULL,.Tiles_Icon = Image_QRcode},
//	{.General_item_text = "温湿度",.General_callback = NULL,.General_SubMenuPage = NULL,.Tiles_Icon = Image_humiture},
	{.General_item_text = " [GAME]",.General_callback = Game_Of_Life_Play,.General_SubMenuPage = NULL,.Tiles_Icon = Image_GAME},
//	{.General_item_text = "计算器",.General_callback = Calculator,.General_SubMenuPage = NULL,.Tiles_Icon = Image_calc},
////	{.General_item_text = "Night",.General_callback = NULL,.General_SubMenuPage = NULL,.Tiles_Icon = Image_night},
////	{.General_item_text = "More",.General_callback = NULL,.General_SubMenuPage = &MoreMenuPage,.Tiles_Icon = Image_more},
	{.General_item_text = "SETTINTS",.General_callback = NULL,.General_SubMenuPage = &SettingsMenuPage,.Tiles_Icon = Image_setings},
	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/

};
extern bool hold;
//设置菜单项内容数组
MenuItem SettingsMenuItems[] = {
	{.General_item_text = "黑暗模式",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = &ColorMode},
	{.General_item_text = "翻腕亮屏",.General_callback = Raise_Wake,.General_SubMenuPage = NULL,.List_BoolRadioBox = &RAWA},
	{.General_item_text = "翻腕速度",.General_callback = BrightnessWindow,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "屏幕Hold",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = &hold},
	{.General_item_text = "时间调整",.General_callback = Time_page_Adjust,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "显示帧率",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = &OLED_UI_ShowFps},
	{.General_item_text = "此设备",.General_callback = NULL,.General_SubMenuPage = &AboutThisDeviceMenuPage,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},

	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/
};

MenuItem AboutThisDeviceMenuItems[] = {
	{.General_item_text = "-[MCU:]",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " STM32F103C8T6",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " CLK:72Mhz",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " RAM:163833Bit",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " ROM:524267Bit",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "-[Screen:]",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " SSD1306 128x64 OLED",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " MPU-6050",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " AHT20",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "-[Periph:]",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " W25Q128",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " YX6100-16s",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = ">关于该WATCH",.General_callback = NULL,.General_SubMenuPage = &AboutOLED_UIMenuPage,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},

	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/
};

MenuItem AboutOLED_UIMenuItems[] = {
	{.General_item_text = "-[Author:]",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " bili @时光久泽",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " OLED_UI框架:",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " bili @上nm网课呢",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = " Ver:3.1",.General_callback = NULL,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},

	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/
};

MenuItem VIDEOWRITEMenuItems[] = {
	{.General_item_text = "[返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "烧录视频1",.General_callback = VIDEO_write_1,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "烧录视频2",.General_callback = VIDEO_write_2,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "烧录视频3",.General_callback = VIDEO_write_3,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回返回返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回返回返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回返回返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "擦除所有视频",.General_callback = VIDEO_erase,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "[返回返回返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/
};


MenuItem VIDEOMenuItems[] = {
	{.General_item_text = "[返回]",.General_callback = OLED_UI_Back,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "播放视频1",.General_callback = VIDEO_1,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "播放视频2",.General_callback = VIDEO_2,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "播放视频3",.General_callback = VIDEO_3,.General_SubMenuPage = NULL,.List_BoolRadioBox = NULL},
	{.General_item_text = "烧录视频",.General_callback = NULL,.General_SubMenuPage = &VIDEOWRITEMenuPage,.List_BoolRadioBox = NULL},
	{.General_item_text = NULL},/*最后一项的General_item_text置为NULL，表示该项为分割线*/
};







MenuPage MainMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_TILES,  		 //菜单类型为磁贴类型
	.General_CursorStyle = NOT_SHOW,			 //光标类型
	.General_FontSize = OLED_UI_FONT_16,			//字高
	.General_ParentMenuPage = NULL,				//由于这是根菜单，所以父菜单为NULL
	.General_LineSpace = 10,						//磁贴间距 单位：像素（对于磁贴类型菜单，此值表示每个磁贴之间的间距，对于列表类型菜单，此值表示行间距）
	.General_MoveStyle = UNLINEAR,				//移动方式
	.General_MovingSpeed = 3,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = MainAuxFunc,		 //显示辅助函数
	.General_MenuItems = MainMenuItems,			//菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.Tiles_ScreenHeight = 64,					//屏幕高度
	.Tiles_ScreenWidth = 128,						//屏幕宽度
	.Tiles_TileWidth = 32,						 //磁贴宽度
	.Tiles_TileHeight = 32,						 //磁贴高度
};


MenuPage SettingsMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_LIST,  		 //菜单类型为列表类型
	.General_CursorStyle = REVERSE_ROUNDRECTANGLE,	 //光标类型为线型
	.General_FontSize = OLED_UI_FONT_12,			//字高
	.General_ParentMenuPage = &MainMenuPage,		 //父菜单为主菜单
	.General_LineSpace = 4,						//行间距 单位：像素
	.General_MoveStyle = UNLINEAR,				//移动方式为非线性曲线动画
	.General_MovingSpeed = SPEED,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = SettingAuxFunc,		 //显示辅助函数
	.General_MenuItems = SettingsMenuItems,		 //菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.List_MenuArea = {32, 0, 95, 64},			 //列表显示区域
	.List_IfDrawFrame = false,					 //是否显示边框
	.List_IfDrawLinePerfix = true,				 //是否显示行前缀
	.List_StartPointX = 4,                        //列表起始点X坐标
	.List_StartPointY = 2,                        //列表起始点Y坐标
};

MenuPage AboutThisDeviceMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_LIST,  		 //菜单类型为列表类型
	.General_CursorStyle = REVERSE_ROUNDRECTANGLE,	 //光标类型为圆角矩形
	.General_FontSize = OLED_UI_FONT_12,			//字高
	.General_ParentMenuPage = &SettingsMenuPage,		 //父菜单为主菜单
	.General_LineSpace = 4,						//行间距 单位：像素
	.General_MoveStyle = UNLINEAR,				//移动方式为非线性曲线动画
	.General_MovingSpeed = SPEED,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = NULL,		 //显示辅助函数
	.General_MenuItems = AboutThisDeviceMenuItems,		 //菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.List_MenuArea = {0, 0, 128, 64},			 //列表显示区域
	.List_IfDrawFrame = false,					 //是否显示边框
	.List_IfDrawLinePerfix = false,				 //是否显示行前缀
	.List_StartPointX = 4,                        //列表起始点X坐标
	.List_StartPointY = 2,                        //列表起始点Y坐标

};

MenuPage AboutOLED_UIMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_LIST,  		 //菜单类型为列表类型
	.General_CursorStyle = REVERSE_BLOCK,	 //光标类型为圆角矩形
	.General_FontSize = OLED_UI_FONT_12,			//字高
	.General_ParentMenuPage = &SettingsMenuPage,		 //父菜单为主菜单
	.General_LineSpace = 4,						//行间距 单位：像素
	.General_MoveStyle = UNLINEAR,				//移动方式为非线性曲线动画
	.General_MovingSpeed = SPEED,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = NULL,		 //显示辅助函数
	.General_MenuItems = AboutOLED_UIMenuItems,		 //菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.List_MenuArea = {0, 0, 128, 64},			 //列表显示区域
	.List_IfDrawFrame = false,					 //是否显示边框
	.List_IfDrawLinePerfix = false,				 //是否显示行前缀
	.List_StartPointX = 4,                        //列表起始点X坐标
	.List_StartPointY = 2,                        //列表起始点Y坐标

};



MenuPage VIDEOMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_LIST,  		 //菜单类型为列表类型
	.General_CursorStyle = REVERSE_ROUNDRECTANGLE,	 //光标类型为圆角矩形
	.General_FontSize = OLED_UI_FONT_12,			//字高
	.General_ParentMenuPage = &MainMenuPage,		 //父菜单为主菜单
	.General_LineSpace = 4,						//行间距 单位：像素
	.General_MoveStyle = UNLINEAR,				//移动方式为非线性曲线动画
	.General_MovingSpeed = SPEED,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = NULL,		 //显示辅助函数
	.General_MenuItems = VIDEOMenuItems,		 //菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.List_MenuArea = {0, 0, 128, 64},			 //列表显示区域
	.List_IfDrawFrame = false,					 //是否显示边框
	.List_IfDrawLinePerfix = true,				 //是否显示行前缀
	.List_StartPointX = 4,                        //列表起始点X坐标
	.List_StartPointY = 2,                        //列表起始点Y坐标
};


MenuPage VIDEOWRITEMenuPage = {
	//通用属性，必填
	.General_MenuType = MENU_TYPE_LIST,  		 //菜单类型为列表类型
	.General_CursorStyle = REVERSE_ROUNDRECTANGLE,	 //光标类型为圆角矩形
	.General_FontSize = OLED_UI_FONT_12,			//字高
	.General_ParentMenuPage = &VIDEOMenuPage,		 //父菜单为主菜单
	.General_LineSpace = 4,						//行间距 单位：像素
	.General_MoveStyle = UNLINEAR,				//移动方式为非线性曲线动画
	.General_MovingSpeed = SPEED,					//动画移动速度(此值根据实际效果调整)
	.General_ShowAuxiliaryFunction = NULL,		 //显示辅助函数
	.General_MenuItems = VIDEOWRITEMenuItems,		 //菜单项内容数组

	//特殊属性，根据.General_MenuType的类型选择
	.List_MenuArea = {0, 0, 128, 64},			 //列表显示区域
	.List_IfDrawFrame = false,					 //是否显示边框
	.List_IfDrawLinePerfix = true,				 //是否显示行前缀
	.List_StartPointX = 4,                        //列表起始点X坐标
	.List_StartPointY = 2,                        //列表起始点Y坐标
};
