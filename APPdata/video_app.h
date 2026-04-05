#ifndef __VIDEO_APP_H
#define __VIDEO_APP_H

#include "stm32f10x.h"               
#include "Serial1.h"
#include "Delay.h"
#include "OLED_UI.h"
#include "W25Q128_SPI1.h"


void VIDEO_1(void);
void VIDEO_write_1(void);

void VIDEO_2(void);
void VIDEO_write_2(void);

void VIDEO_3(void);
void VIDEO_write_3(void);

void VIDEO_erase(void);


#endif

