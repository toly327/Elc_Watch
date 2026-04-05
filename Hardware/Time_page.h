#ifndef __TIME_PAGE_H
#define __TIME_PAGE_H
#include "stm32f10x.h"
#include <string.h>

void Time_page(void);
void Time_page_io_init(void);
void Time_page_Timer(int8_t x,int8_t y);
void Time_page_small_Timer(u8 x,u8 y);
void Time_page_date(int8_t x,int8_t y);
void Time_page_week(int8_t x,int8_t y);
void Time_page_BAT(int8_t x,int8_t y);
void Time_page_AHT20(int8_t x,int8_t y);//ÏÔÊ¾ÎÂÊª¶È
void Time_page_Adjust(void);
void Time_page_figures(int8_t x,int8_t y,u8 num,u8 size);

void low_pass_filter(float inputX,float inputY,float inputx,float inputy,uint8_t i);

#endif
