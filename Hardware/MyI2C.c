#include "stm32f10x.h"
#include "Delay.h"

#define MyI2C_SCL(x)		GPIO_WriteBit(GPIOB,GPIO_Pin_8,(BitAction)(x));	\
									eDelay()
#define MyI2C_SDA(x)		GPIO_WriteBit(GPIOB,GPIO_Pin_9,(BitAction)(x))
#define MyI2C_SDA_Read()	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

void MyI2C_Init(void)
{
	static uint8_t InitFlag=0;
	if(InitFlag==0)
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_OD;
		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin= GPIO_Pin_8|GPIO_Pin_9;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		MyI2C_SDA(1);
		MyI2C_SCL(1);
		InitFlag=1;
	}
}

void MyI2C_Start(void)
{
	MyI2C_SDA(1);
	MyI2C_SCL(1);
	MyI2C_SDA(0);
	MyI2C_SCL(0);
}

void MyI2C_Stop(void)
{
	MyI2C_SDA(0);
	MyI2C_SCL(1);
	MyI2C_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		MyI2C_SDA(Byte&(0x80>>i));
		MyI2C_SCL(1);
		MyI2C_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i,Byte=0x00;
	for(i=0;i<8;i++)
	{
		MyI2C_SCL(1);
		if(MyI2C_SDA_Read())
		{
			Byte|=(0x80>>i);
		}
		MyI2C_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t Ack)
{
	if(Ack)
	{
		MyI2C_SDA(0);
	}
	else
	{
		MyI2C_SDA(1);
	}
	MyI2C_SCL(1);
	MyI2C_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t Ack;
	MyI2C_SCL(1);
	if(MyI2C_SDA_Read()==0)
	{
		Ack=1;
	}
	else
	{
		Ack=0;
	}
	MyI2C_SCL(0);
	return Ack;
}
