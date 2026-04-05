#ifndef __YXMP3_H
#define __YXMP3_H
#include "stm32f10x.h"
#include <string.h>
#include "OLED_UI.h"  
#include "MyRTC.h"
#include "AD.h"

extern char Serial2_RxPacket[100];	//接收数据包数组
extern bool MP3_OK;
extern uint8_t MP3volume;//音量

void YXMP3_init(void);
void YXMP3_PWROFF(void);
void YXMP3_Homepage(void);
void MP3CMD(uint8_t CMD ,uint16_t data);
void MP3_Read_data(void);

#define   YX_NULL     0x00

//#define   YX_Audio+  	 0x01
//#define   YX_Audio-  	 0x02

//#define   YX_set_MP3_Audio     0x12  
#define   YX_begin_Audio     0x0D
#define   YX_stop_Audio     0x0E

#define   YX_set_folder_Audio     0x0F  
#define   YX_01_folder  0x0100  //(数据)+YX_01_folder
#define   YX_02_folder  0x0200  //(数据)+YX_02_folder

#define   YX_set_Audio_circulation    0x19   //01关闭，00开启
#define   YX_set_volume    0x06

#define   YX_read_folder     0x4E
#define   YX_01_quantity     0x01  



#endif
