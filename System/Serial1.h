#ifndef __SERIAL1_H
#define __SERIAL1_H

#include <stdio.h>

extern char Seria1_RxPacket[];
extern uint8_t Serial_RxFlag;
extern uint8_t p0, p1,we;

void Serial1_Init(void);
void Serial1_Power_OFF(void);

//void Seria1_SendByte(uint8_t Byte);
//void Seria1_SendArray(uint8_t *Array, uint16_t Length);
//void Seria1_SendString(char *String);



#endif
