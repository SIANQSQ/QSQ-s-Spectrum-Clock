#ifndef __DS3231_H
#define __DS3231_H
#include "stm32f10x.h"

extern uint8_t DS3231_Time[];

void DS3231_Init(void);
void DS3231_WriteReg(uint8_t Address,uint8_t Data);
uint8_t DS3231_ReadReg(uint8_t Address);
void DS3231_ReadTime(void);
void DS3231_SetTime(void);

#endif
