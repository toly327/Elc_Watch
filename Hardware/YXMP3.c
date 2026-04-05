#include "YXMP3.h"
#include "Time_page.h"

bool MP3_OK = false;
bool MP3_Play = false;
bool MP3_Circulation = false;
uint8_t MP3volume = 10;//音量
uint16_t MP3MAXfilesnum=300;//文件数
uint16_t MP3atsnum=1,MP3UPatsnum=0;//当前播放




char Serial2_RxPacket[100];	//定义接收数据包数组
uint8_t Serial2_RxFlag;	   //定义接收数据包标志位

extern OLED_ChangePoint OLED_UI_PageStartPoint;

void MP3CMD(uint8_t CMD ,uint16_t data);
void MP3_Read_data(void);

void YXMP3_init(void)
{	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//开启USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_ResetBits(GPIOB, GPIO_Pin_6);	

	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;		   //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;           	  //模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		     //奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	    //停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长，选择8位
	USART_Init(USART2, &USART_InitStructure);				  //将结构体变量交给USART_Init，配置USART2
	
	/*中断输出配置*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		   //开启串口接收数据的中断
	

	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					       //定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		      //选择配置NVIC的USART2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    	 //指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		   //指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							  //将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART2, ENABLE);							   //使能USART2，串口开始运行
	
}

void YXMP3_PWROFF(void)
{		
	USART_Cmd(USART2, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //开启GPIOB的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	MP3_OK = false;	
	MP3_Play = false;
	MP3MAXfilesnum=300;
	MP3UPatsnum=0;
}

void YXMP3_DDD1(void)
{
	MP3CMD(YX_set_volume,MP3volume);
	Delay_ms(30);
	if(MP3UPatsnum!=MP3atsnum)
	{MP3CMD(YX_set_folder_Audio,YX_01_folder+MP3atsnum);MP3UPatsnum=MP3atsnum;}
	else MP3CMD(YX_begin_Audio,YX_NULL);
	Delay_ms(100);
	if(MP3_Circulation)MP3CMD(YX_set_Audio_circulation,00);
}

void YXMP3_DDD2(int8_t X,int8_t Y,bool i)
{	
	static int8_t b;
	if(i){if(b<64)b++;
	else b=0;}	
	OLED_DrawRectangle( X,Y,64-12,6 ,OLED_UNFILLED);
	OLED_DrawRectangle( X-12+b,Y,12,6 ,OLED_FILLED);
	OLED_ClearArea( X-12,Y,12,6);
	OLED_ClearArea( X-12+64,Y,12,6);
}


void YXMP3_Homepage(void)
{
	int8_t p,a;
	uint16_t c=0;
	OLED_UI_FadeoutAllArea();
	OLED_Brightness_DODO();
	
	YXMP3_init();
	
	low_pass_filter(80,10,38,38,2);
	
	a= -30;
	
	while(1)
	{	
	   OLED_Clear();	
		if(a>0)
		{			
			if(MP3MAXfilesnum==300)
			{	
				OLED_ShowString(32+2, 10+40-8-2-8-2, "MP3", OLED_6X8_HALF);	
				OLED_ShowString(32+2, 10+40-8-2, "loading...", OLED_6X8_HALF);	
				c++;
				if(c>3600){OLED_ClearArea(32+2,10+40-8-2,60,8);OLED_ShowString(32+2, 10+40-8-2, "ERROR!!!", OLED_6X8_HALF);OLED_Update();Delay_ms(5000);YXMP3_PWROFF();OLED_UI_FadeoutAllArea();OLED_UI_PageStartPoint.CurrentPoint.X = 127;return;}
			}
			else
			{	
				OLED_ShowString(32+2, 10+40-8-2-8-2, "Files", OLED_6X8_HALF);	
				OLED_ShowString(32+2, 10+40-8-2, "Num:", OLED_6X8_HALF);
				OLED_ShowNum(32+2+24,10+40-8-2,MP3MAXfilesnum,3,OLED_6X8_HALF);
				a++;
				if(a>120)break;
			}
		
			YXMP3_DDD2(32+6,10+4,true);
		}
		else a++;
		
	    low_pass_filter(32,10,64,40,0);
		
		MP3_Read_data();
		

	   OLED_Brightness_UPUP();	
	   OLED_Update();
	   Delay_ms(15);
	}
	
	low_pass_filter(32,10,64,40,1);
	OLED_Update();
	Delay_ms(100);
	
	OLED_Clear();
	OLED_DrawRectangle(32,10,64,40,OLED_UNFILLED);
	OLED_Update();
	Delay_ms(100);
	
	OLED_Clear();
	OLED_DrawRectangle(32,10,64,40,OLED_FILLED);
	OLED_Update();
	Delay_ms(100);
	
	OLED_Clear();
	OLED_Update();
	Delay_ms(100);
	
	
	
	uint16_t mun = 0,OFFmun=1;
	float tttx=128;
	extern bool hold;
	
	
	while(1)
	{
		
		
		
		
		OLED_Clear();
		
		MP3_Read_data();
		
		if(tttx>0.3)tttx*=0.95;
		else tttx=0;
		
		Time_page_BAT(90-tttx/2,0);
		Time_page_small_Timer(0+tttx,0);	

		OLED_ShowNum(128-24-10+tttx,32-4,MP3atsnum,3,OLED_8X16_HALF);
		OLED_DrawRectangle(128-24-10+tttx-2,32-2-2,24+4,16+1 ,OLED_UNFILLED);
		
		OLED_ShowString(128-18-6,32+16+1+tttx/3,"/",OLED_6X8_HALF);
		OLED_ShowNum(128-18,32+16+1+tttx/3,MP3MAXfilesnum,3,OLED_6X8_HALF);
		
		OLED_ShowNum(0-tttx/4,64-12,MP3volume,2,OLED_7X12_HALF);
		OLED_ShowString(15-tttx/2,64-8,"/30",OLED_6X8_HALF);

		YXMP3_DDD2(12,32+tttx/4,MP3_Play);
		
		
		if(OFFmun==0)OLED_Power(1);
		else if(OFFmun==500)OLED_Power(0);
		
		if(OFFmun<=500 && hold==false)OFFmun++;

	   OLED_Brightness_UPUP();	
	   OLED_Update();
		
	   Delay_ms(15);
	   p = menu_Roll_event();   
	   if( p == 1){ 
		   MP3atsnum++;
		   if(MP3atsnum>MP3MAXfilesnum)MP3atsnum=1;	   
			YXMP3_DDD1();
		   MP3_Play=true;
		   OFFmun=0;
	   }	
       else if( p == -1){
		   MP3atsnum--;
		   if(MP3atsnum==0)MP3atsnum=MP3MAXfilesnum; 
			YXMP3_DDD1();
		   MP3_Play=true;
		   OFFmun=0;		   
	   }	
		p=menu_Enter_event();
		if(p == 1)
		{	
			MP3_Play=!MP3_Play;
			if(MP3_Play)
			{	
				YXMP3_DDD1();
			}
			else MP3CMD(YX_stop_Audio,YX_NULL);	
			 OFFmun=0;
		}
		else if(p == 2){
			 OFFmun=0;
			OLED_Power(1);
			while(1)
			{
				MP3_Read_data();	
				
			   OLED_ClearArea(32,16,64,32);
				OLED_DrawRectangle(32,16,64,32,OLED_UNFILLED);
				
			    OLED_ShowMixString(3+32+2,16+2,"退出",OLED_8X8_FULL,OLED_6X8_HALF);
				
			    OLED_ShowMixString(3+32+2,16+2+8+2,"Vol:",OLED_8X8_FULL,OLED_6X8_HALF);
				OLED_ShowNum(3+32+2+24,16+2+8+2,MP3volume,2,OLED_6X8_HALF);
				
			    if(MP3_Circulation)OLED_ShowString(3+32+2,16+2+8+2+8+2,"<->",OLED_6X8_HALF);
				else OLED_ShowString(3+32+2,16+2+8+2+8+2," ->",OLED_6X8_HALF);
				
				switch(mun)
				{ 
					case 0:OLED_ShowString(3+32+2+30,16+2,"-",OLED_6X8_HALF);break;
					case 1:OLED_ShowString(3+32+2+30+8+2,16+2+8+2,"-",OLED_6X8_HALF);break;
					case 2:OLED_ShowString(3+32+2+30,16+2+8+2+8+2,"-",OLED_6X8_HALF);break;
				}
				
			   OLED_Update();
				
			   Delay_ms(20);
			   p = menu_Roll_event();   
			   if( p == 1){
					switch(mun)
					{ 
						case 0:	OLED_Power(1);YXMP3_PWROFF();OLED_UI_FadeoutAllArea();OLED_UI_PageStartPoint.CurrentPoint.X = 127;Delay_ms(300);return;	
						case 1:if(MP3volume>0)MP3volume--;	MP3CMD(YX_set_volume,MP3volume);Delay_ms(30);break;
						case 2:MP3_Circulation=!MP3_Circulation;MP3CMD(YX_set_Audio_circulation,(int)!MP3_Circulation);break;
					}
				   
			   }	
			   else if( p == -1){
					switch(mun)
					{ 
						case 0:	OLED_Power(1);YXMP3_PWROFF();OLED_UI_FadeoutAllArea();OLED_UI_PageStartPoint.CurrentPoint.X = 127;Delay_ms(300);return;							
						case 1:if(MP3volume<30)MP3volume++;	MP3CMD(YX_set_volume,MP3volume);Delay_ms(30);break;					
						case 2:MP3_Circulation=!MP3_Circulation;MP3CMD(YX_set_Audio_circulation,(int)!MP3_Circulation);break;
					}
			   }	
				p=menu_Enter_event();
				if(p == 1){	
					mun++;
				   if(mun>2)mun=0;

				}
				else if(p == 2){
			
					break;
				}	
		
			}	
			
			
		}	
	}
}



/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

void MP3CMD(uint8_t CMD ,uint16_t data)
{
	uint8_t Send_buf[8];
	
    Send_buf[0] = 0x7e;    //头
    Send_buf[1] = 0xff;    //保留字节 
    Send_buf[2] = 0x06;    //长度
    Send_buf[3] = CMD;     //控制指令
    Send_buf[4] = 0x00;   //是否需要反馈
    Send_buf[5] = (uint8_t)(data >> 8);//data
    Send_buf[6] = (uint8_t)(data);     //data
    Send_buf[7] = 0xef;    //尾
	
	for (u8 i = 0; i < 8; i ++)		//遍历数组
	{
		Serial2_SendByte(Send_buf[i]);	  //依次调用Serial_SendByte发送每个字节数据
	}
}

uint8_t ReadLN=0;

void MP3_Read_data(void)
{		
	if (Serial2_RxFlag == 1) //如果接收到数据包
	{	
		if (Serial2_RxPacket[3] == 0x3F && Serial2_RxPacket[6]==0x02){ MP3_OK = true;ReadLN=1;}//确认是否初始化完毕
		if (Serial2_RxPacket[3] == 0x4E){ MP3MAXfilesnum =  Serial2_RxPacket[6];ReadLN=0;}//读取文件数
		if (Serial2_RxPacket[3] == 0x3D && MP3_Circulation==false){ MP3_Play =  false;}//MP3播放完
		
		Serial2_RxFlag = 0;//处理完成，接收数据包标志位清零
	}
	
	
	if(ReadLN==1)MP3CMD(YX_read_folder,YX_01_quantity);
	
}

/**
  * 函    数：USART2中断函数
  * 参    数：无
  * 返 回 值：无
  */
void USART2_IRQHandler(void)
{
	static uint8_t RxState = 0;		 //定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket = 0;	//定义表示当前接收数据位置的静态变量	

	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)	 //判断是否是USART的接收事件触发的中断
	{
		uint8_t RxData = USART_ReceiveData(USART2);		   //读取数据寄存器，存放在接收的数据变量				
		/*当前状态为0，接收数据包包头或配对验证*/
		if (RxState == 0)
		{
			if (RxData == 0x7E && Serial2_RxFlag == 0) //如果数据确实是包头，并且上一个数据包已处理完毕
			{
				pRxPacket = 0;		   //数据包的位置归零
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;
				
				RxState = 1;			//置下一个状态
				
			}

		}
		/*当前状态为1，接收数据包第一个数据*/
		else if (RxState == 1)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 2;		              //置下一个状态

		}
				else if (RxState == 2)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 3;		              //置下一个状态

		}

				else if (RxState == 3)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 4;		              //置下一个状态

		}

				else if (RxState == 4)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 5;		              //置下一个状态

		}
				else if (RxState == 5)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 6;		              //置下一个状态

		}

				else if (RxState == 6)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 7;		              //置下一个状态

		}

				else if (RxState == 7)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 8;		              //置下一个状态

		}

				else if (RxState == 8)
		{
				Serial2_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
				pRxPacket ++;			           //数据包的位置自增
    			RxState = 9;		              //置下一个状态

		}

				else if (RxState == 9)
		{
				Serial2_RxPacket[pRxPacket] = RxData;	  //将数据存入数据包数组的指定位置
			    pRxPacket ++;			                 //数据包的位置自增
//				Serial2_RxPacket[pRxPacket] = '\0';		//将收到的字符数据包添加一个字符串结束标志
				Serial2_RxFlag = 1;		               //接收数据包标志位置1，成功接收一个数据包
				RxState = 0;			              //状态归0
		}	

		
			
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);		//清除标志位
	}
}
