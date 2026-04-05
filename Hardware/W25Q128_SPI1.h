#ifndef __W25Q128_SPI1_H
#define __W25Q128_SPI1_H
#include "Delay.h"  
#include "W25Q128_SPI1_Ins.h"

void W25Q128_Init(void);
void W25Q128_ReadID(uint8_t *MID, uint16_t *DID);
void W25Q128_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count);
void W25Q128_unlimited_Program(uint32_t Address, uint8_t* DataArray,uint16_t Count);
void W25Q128_SectorErase(uint32_t Address);
void W25Q128_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);

void W25Q128_WaitBusy(void);
void W25Q128_WriteEnable(void);
void MySPI_Start(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
void MySPI_Stop(void);


#endif
