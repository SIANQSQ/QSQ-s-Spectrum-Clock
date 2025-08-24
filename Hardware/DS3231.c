#include "stm32f10x.h"
#include "MyI2C.h"

uint8_t DS3231_Time[7];

#define DS3231_ADDRESS			0xD0

void DS3231_Init(void)
{
	MyI2C_Init();
}

void DS3231_WriteReg(uint8_t Address,uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(DS3231_ADDRESS|0x00);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Address);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t DS3231_ReadReg(uint8_t Address)
{
	uint8_t Data;
	MyI2C_Start();
	MyI2C_SendByte(DS3231_ADDRESS|0x00);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Address);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(DS3231_ADDRESS|0x01);
	MyI2C_ReceiveAck();
	Data=MyI2C_ReceiveByte();
	MyI2C_SendAck(0);
	MyI2C_Stop();
	return Data;
}

void DS3231_ReadTime(void)
{
	uint8_t Temp;
	Temp=DS3231_ReadReg(0x06);
	DS3231_Time[0]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x05);
	DS3231_Time[1]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x04);
	DS3231_Time[2]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x03);
	DS3231_Time[6]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x02);
	DS3231_Time[3]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x01);
	DS3231_Time[4]=Temp/16*10+Temp%16;
	Temp=DS3231_ReadReg(0x00);
	DS3231_Time[5]=Temp/16*10+Temp%16;
}

void DS3231_SetTime(void)
{
	DS3231_WriteReg(0x06,DS3231_Time[0]/10*16+DS3231_Time[0]%10); //年
	DS3231_WriteReg(0x05,DS3231_Time[1]/10*16+DS3231_Time[1]%10); //月
	DS3231_WriteReg(0x04,DS3231_Time[2]/10*16+DS3231_Time[2]%10); //日
	DS3231_WriteReg(0x03,DS3231_Time[6]/10*16+DS3231_Time[6]%10); //星期
	DS3231_WriteReg(0x02,DS3231_Time[3]/10*16+DS3231_Time[3]%10); //时
	DS3231_WriteReg(0x01,DS3231_Time[4]/10*16+DS3231_Time[4]%10); //分
	DS3231_WriteReg(0x00,DS3231_Time[5]/10*16+DS3231_Time[5]%10); //秒
}
