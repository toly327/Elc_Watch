#include "video_app.h"

extern uint8_t OLED_DisplayBuf[8][128];
bool VIDEOOKK=true;
//因为不同视频播放逻辑基本一样，只是偏移地址计算不一样，所以写了个通用的一起用
void SHOW_VIDEO(uint32_t Address ,uint16_t mpg)//总帧量 , 读取数据起始地址
{	
		int8_t p;		
	    p0=0,p1=0;
	
		OLED_UI_FadeoutAllArea();

		
		uint8_t temp[3];   //文件头(3字节)
		uint32_t mydata=0; //总数据量(不包括文件头)
	    uint32_t datasite=3; //帧数据偏移地址

	
		OLED_Clear();
		OLED_Update(); 
//		OLED_WriteCommand(0x81);	//设置对比度
//		OLED_WriteCommand(0xFF);	//0x00~0xFF
	
		W25Q128_ReadData( Address , temp , 3 );
	
		if(temp[0]!=0xAA) //检测数据头是否存在
		{	
			OLED_ShowMixString(0,20,"无视频",OLED_12X12_FULL,OLED_7X12_HALF);
			OLED_Update(); 
			
			Delay_ms(500);
//			GUODUsn=1;
//			OLED_transition(0);
			OLED_UI_FadeoutAllArea();

			return;
		}
		
		mydata= (temp[2]<< 8) |  temp[1];  //获取总帧数量
		
		OLED_ShowMixString(0,0,"帧数:25FPS",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_ShowMixString(0,13,"帧量:",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_ShowNum(38+12,15,mydata,6,OLED_6X8);
		
		mydata *= 1024; 				 //计算总数据量
		
		OLED_ShowMixString(0,26,"数据量(Byte):",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_ShowNum(0,40,mydata,8,OLED_6X8);
		
		
		YXMP3_init();
				
		OLED_ShowMixString(92,52,"读取中",OLED_12X12_FULL,OLED_7X12_HALF);		//这里实际上是为了掩盖mp3音乐的延迟
		
		OLED_Update(); 
		
		uint8_t temm=0;
					
		while(!MP3_OK)
		{MP3_Read_data();Delay_ms(2000);temm++; if(temm>2){OLED_ShowMixString(0,52,"MP3-ERROR",OLED_12X12_FULL,OLED_7X12_HALF);OLED_Update();Delay_ms(1000);break;}}
		temm=0;
		
		MP3CMD(YX_set_volume,MP3volume);
		
		OLED_UI_FadeoutAllArea();
		
		MP3CMD(YX_set_folder_Audio,mpg);
		
		
		Delay_ms(2000);				//音乐播放延迟，根据实际微调
//		Delay_ms(2000);
//		Delay_ms(2000);
		
	
//	 uint16_t temm=15;

	while(1)
	{
		if(VIDEOOKK)
		{
			VIDEOOKK=false;
			W25Q128_ReadData( Address+datasite , OLED_DisplayBuf[0] , 1024 );//读取一帧到显存
			datasite+=1024;       //地址偏移到下一帧
		}
		
//		Delay_ms(temm);		//帧间隔，根据实际微调
		
		OLED_Update();  		
//        OLED_transition(1);
		
		
//		p=menu_Roll_event(); //微调帧间隔
//		if (p==1){			
//			temm++;
//		}	
//		else if(p == -1){
//		    temm--;
//		}
		
			
		p=menu_Enter_event();  //手动退出|停止
		if (p==1){
		
			do
			{ 
				OLED_Update();  		
				p=menu_Enter_event(); 
				
				if(p == 2){
			
//					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
//					{	
////						MP3_starts(0);
////						Serial2_Power_OFF();
//					
//					}
//					GUODUsn=1;
//					OLED_transition(0);
					YXMP3_PWROFF();
					OLED_UI_FadeoutAllArea();
					Delay_ms(300);
					return;	
				}
				
			
			}
			while(p !=1 ); 
			
			
		}
		else if(p == 2){
			
//			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
//			{	
////				MP3_starts(0);
////				Serial2_Power_OFF();
//			
//			}
//			GUODUsn=1;
//			OLED_transition(0);
			YXMP3_PWROFF();
			OLED_UI_FadeoutAllArea();
			Delay_ms(300);
			return;	
		}
		
		
		if(datasite > mydata) //是否播放完毕
		{	
//			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
//			{	
//				
////				MP3_starts(0);				
////				Serial2_Power_OFF();
//			
//			}
			OLED_Clear();
			OLED_ShowMixString(0,20,"播放完毕",OLED_12X12_FULL,OLED_7X12_HALF);
			OLED_Update(); 
			Delay_ms(1000);
//			GUODUsn=1;
//			OLED_transition(0);
			YXMP3_PWROFF();
			OLED_UI_FadeoutAllArea();
			return;
		}
	}

	
}

uint32_t VIDEO_SITE(uint32_t Address) //跟据输入的视频地址计算下一个视频地址
{
		uint8_t temp[3];   //文件头(3字节)
	    uint32_t mydata=0; //总数据量(不包括文件头)
		W25Q128_ReadData( Address , temp , 3 );

			mydata= (temp[2]<< 8) |  temp[1];  //获取总帧数量	
			mydata*=1024;			
			Address += (mydata+3) ;
		
	return Address;	
}


void VIDEO_1(void)
{
//		uint32_t Address=163840;//视频文件数据起始地址 0x28000	
		uint32_t Address=0;
		SHOW_VIDEO(Address,0x0201); //顺便播放tf卡里02文件夹的01歌曲
}

void VIDEO_2(void)
{			
		SHOW_VIDEO( VIDEO_SITE(0) ,0x0202);
}


void VIDEO_3(void)
{				
		SHOW_VIDEO( (VIDEO_SITE( VIDEO_SITE(0) ))  ,0x0203);
}


//同上，写了个通用的一起用
void WRITE_VIDEO(uint32_t Address)//数据起始地址
{	
	
		int8_t p;
	
		OLED_UI_FadeoutAllArea();
	
		TIM_Cmd(TIM4, DISABLE);

		Serial1_Init();
	
		
	    p0=0,p1=0 ,we=0;
		
		uint8_t temp[3]={0xAA,0,0};   //文件头(3字节)
	    uint32_t datasite=3;    //帧数据偏移地址
	
	
//		OLED_WriteCommand(0x81);	//设置对比度
//		OLED_WriteCommand(0xFF);	//0x00~0xFF
	
		OLED_Clear();
		OLED_ShowMixString(0,20,"开放烧录中",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_Update(); 
	
		

	while(1)
	{
		if(we)//如果有数据
		{	
			W25Q128_unlimited_Program( Address+datasite , OLED_DisplayBuf[0] , 1024 );//烧录一帧到显存
			datasite+=1024;       //地址偏移到下一帧
			we=0;
		}
		
		
		p=menu_Enter_event();
		if (p){
		
			if( (datasite-3) != 0 ) //如果有数据
			{	
				temp[1]= ((datasite-3)/1024) & 0xFF;    
				temp[2]= ( ( (datasite-3) /1024 ) >> 8 ) & 0xFF;  //已存帧数
				
				W25Q128_unlimited_Program( Address , temp , 3 );	//写入已存头数据
				
				OLED_Clear();
				OLED_ShowMixString(0,20,"视频烧录完",OLED_12X12_FULL,OLED_7X12_HALF);
				OLED_Update(); 
				Serial1_Power_OFF();
										TIM_Cmd(TIM4, ENABLE);
				Delay_ms(500);
//				GUODUsn=1;
//				OLED_transition(0);
				OLED_UI_FadeoutAllArea();
				return;	
			}
			else
			{
				OLED_Clear();
				OLED_ShowMixString(0,20,"未检测到烧录数据流",OLED_12X12_FULL,OLED_7X12_HALF);
				OLED_Update(); 
				Serial1_Power_OFF();
										TIM_Cmd(TIM4, ENABLE);
				Delay_ms(500);
//				GUODUsn=1;
//				OLED_transition(0);
				OLED_UI_FadeoutAllArea();
				return;	
			}
				
		}	
		
	}
}

void VIDEO_write_1(void)
{
//		uint32_t Address=163840;//数据起始地址 0x28000	
		WRITE_VIDEO(0);	
}

void VIDEO_write_2(void)
{	
		WRITE_VIDEO( VIDEO_SITE(0) );	
}


void VIDEO_write_3(void)
{		
	    WRITE_VIDEO( VIDEO_SITE(VIDEO_SITE(0)) );	
}										

void VIDEO_erase(void)
{	
	int8_t p,y=-1;	

	OLED_UI_FadeoutAllArea();
	
	while(1)
	{	
		OLED_Clear();	
		OLED_ShowMixString(0,2,"是否擦除?",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_ShowMixString(0,2+13,"Are you sure?",OLED_12X12_FULL,OLED_7X12_HALF);
		
		if(y==-1) OLED_ShowMixString(0,2+13+20,"否 NO",OLED_12X12_FULL,OLED_7X12_HALF);
		else OLED_ShowMixString(0,2+13+20,"是 YES",OLED_12X12_FULL,OLED_7X12_HALF);
		
		OLED_Update();
		
	
		p=menu_Roll_event(); //微调帧间隔
		if (p==1){	
				y=1;
		}	
		else if(p == -1){
			y=-1;	
		}
		p=menu_Enter_event();
		if (p){	
			if(y==-1){OLED_UI_FadeoutAllArea();Delay_ms(300);return;}
			else break;			
		}

	}
	
	
	
//		GUODUsn=1;
//		uint32_t data = 163840; 
		OLED_Clear();
		OLED_ShowMixString(0,20,"擦除中",OLED_12X12_FULL,OLED_7X12_HALF);
		OLED_Update();
	
		W25Q128_WaitBusy();							//等待忙
		W25Q128_WriteEnable();						//写使能
		
		MySPI_Start();								//SPI起始
		MySPI_SwapByte(W25Q128_CHIP_ERASE);	   //交换发送扇区擦除的指令
		MySPI_Stop();								//SPI终止
	
	   
		W25Q128_WaitBusy();                         //等待忙		
//		OLED_WriteCommand(0x81);	//设置对比度
//		OLED_WriteCommand(0xFF);	//0x00~0xFF
		
			OLED_Clear();
			OLED_ShowMixString(0,20,"擦除完毕",OLED_12X12_FULL,OLED_7X12_HALF);
			OLED_Update();
			
			OLED_Update(); 
			Delay_ms(1000);
//			GUODUsn=1;
//			OLED_transition(0);
			OLED_UI_FadeoutAllArea();
			return;	
}




	
	

