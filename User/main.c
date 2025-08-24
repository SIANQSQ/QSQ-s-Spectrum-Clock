/*
	程序名	：	QSQ's Spectrum
	硬件平台：	STM32F103C8T6最小系统板
	引脚定义：	PA0	->	WS2812B DATA
				PA1	->	MAX4466 OUT
				PA9	->	USART TX
				PA10->	USART RX
				PB1	->	Spectrum Key
				PB2	->	Light Key
				PB3	->	Clock Key
				PB4	->	Clock Show Content Key
				PB8	->  IIC DS3231 SCL
				PB9	->  IIC DS3231 SDA
*/
#include "stm32f10x.h"
#include "main.h"
#include "RCC.h"
#include "Delay.h"
#include "IO.h"
#include "TIM3.h"
#include "WS2812B.h"
#include "Key.h"
#include "OLED.h"
#include "AD.h"
#include "RGB.h"
#include "FFT.h"
#include "stm32_dsp.h"
#include <stdlib.h>
#include <stdio.h>
#include "Serial.h"
#include "DS1302.h"  //DS1302
#include "DS3231.h"  //DS3231
#include "string.h"

#define GREEN 0x080000
float M_PI=3.14159;  //pi
float PI2 = 6.283185;//2*pi
uint32_t NPT = 256, Fs = 9984;//采样频率为方便计算，选用一个接近10000且，Fs/NTP为整数的值（39）
long lBufInArray[256] = {0}, lBufOutArray[128],lBufMagArray[128];

uint8_t KeyNum,MODE=1;

uint8_t UpdateFlag=1;
uint8_t WhiteLight_Brightness;
uint8_t ColorLight_Mode,ColorLight_Flag;
uint16_t ColorLight_Time;


extern uint8_t Serial_RxData;		//串口数据
extern uint8_t Serial_RxFlag;
int StartM=0;
void SetTimeArray()
{
	DS3231_Time[0]=25;    //年-月-日-时-分-秒-星期
	DS3231_Time[1]=2;
	DS3231_Time[2]=10;
	DS3231_Time[3]=21;
	DS3231_Time[4]=44;
	DS3231_Time[5]=0;
	DS3231_Time[6]=7;
}
	


void Init(void)
{
	RCC_Init();
	Key_Init();
	WS2812B_Init();
	Sample_Init();
	OLED_Init();
    USART1_Init();
	//DS1302_GPIO_Init();
	//DS1302_Init();
	DS3231_Init();
	//SetTimeArray();
	//DS3231_SetTime();
}

int freshpoint = 0; //顶端下落点延时计数变量
uint8_t fall_pot_y[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; //顶端下落点纵坐标数组
int16_t y = 0; //Spectrum模式fft计数
int mode=23;//模式变量
int changemode=1;//按下改变模式按钮
int ClockShowContentMode = 1;//时钟模式下显示： 1->时分  2->年  3->月日
int modeclr = 0xffffff; //显示切换模式的颜色
u8 InEXTI=0;  //已经进中断的标志位，防止连续按键多次切换模式
u8 FFTInNeed = 0; //是否需要进行fft运算

/*模式函数声明*/
void OFFMode(void);
void StartUp(void);
//频谱灯模式
void SpectrumMode1(void);
void SpectrumMode2(void);
void SpectrumMode3(void);
void SpectrumMode4(void);
void SpectrumMode5(void);
void SpectrumMode6(void);
void SpectrumMode7(void);
void SpectrumMode8(void);
//照明模式（用处不大，只有蓝牙连接时输入命令能进入）
void LightMode1(void);
void LightMode2(void);
void LightMode3(void);
void LightMode4(void);
void LightMode5(void);
//时钟模式
void ClockMode1(void);
void ClockMode2(void);
void ClockMode3(void);
void ClockMode4(void);
void ClockMode5(void);


int main(void)
{
	Init();
	while (1)
	{
		if(!StartM&&mode!=0)StartUp();
		switch(mode)
		{
			case 0: {OFFMode();break;}
			case 1: {SpectrumMode1();break;}
			case 2: {SpectrumMode2();break;}
			case 3: {SpectrumMode3();break;}
			case 4: {SpectrumMode4();break;}
			case 5: {SpectrumMode5();break;}
			case 6: {SpectrumMode6();break;}
			case 7: {SpectrumMode7();break;}
			case 8: {SpectrumMode8();break;}
			case 11: {LightMode1();break;}
			case 12: {LightMode2();break;}
			case 13: {LightMode3();break;}
			case 14: {LightMode4();break;}
			case 15: {LightMode5();break;}
			case 21: {ClockMode1();break;}
			case 22: {ClockMode2();break;}
			case 23: {ClockMode3();break;}
			case 24: {ClockMode4();break;}
			case 25: {ClockMode5();break;}
		}
		/*测试AD
		int a=AudioSample[0]/1000;
		int b=(AudioSample[0]%1000)/100;
		int c=(AudioSample[0]%100)/10;
		int d=AudioSample[0]%10;
		RGB_Num(1,a,modeclr);
		RGB_Num(2,b,modeclr);
		RGB_Num(3,c,modeclr);
		RGB_Num(4,d,modeclr);
		Delay_ms(50);*/
	    WS2812B_UpdateBuf();
		RGB_Reset();

	}
}



void ShowMode(char mode,int num,int color)
{
	int dl=100;
	while(dl)
	{
	RGB_Alpha(2,mode,color);
	RGB_Num(3,num,color);
	WS2812B_UpdateBuf();	
	RGB_Reset();
	changemode=0;
	dl--;
	}
	
	InEXTI=0;
}
void SpectrumMode1(void)
{
    if(changemode) ShowMode('S',1,modeclr);	
	for(uint16_t i=1; i<=16; i++)        //绘制128个列像素的每一点幅值，从1开始是为了舍弃谐波中的第一个直流分量
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;    //OLED显示屏左上角坐标为（0，0），右下角为（127， 63）、lBufMagArray[x]/10是为了量化显示结果
			//y*=2;
			if(i==1)y/=3;
			if(y >8 )          //如果y超出了屏幕，则按最大值绘制
			{
				y = 8;
			}
			
			RGB_Column(i,y,0x400005);//开始画线，前两个参数为线的顶端，后两个为低端位置，低端y轴固定为63
			/*下落的点*/
			RGB_Point(i, fall_pot_y[i-1],0x005f00);//绘制一个像素点
			if(fall_pot_y[i-1] <= y)            //像素点不断刷新，并由频谱线举起，（行坐标越小，代表越高）
			{
				if(y+2>8) y=6;
				fall_pot_y[i-1] = y+2;
                				
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%20==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		freshpoint++;
		InEXTI=0;
}


int clr_G2R[8]={0x4f0000,0x4f2800,0x2f2800,0x3f5800,0x3f7800,0x0f5000,0x0ff000,0x00ff00};
void SpectrumMode2(void)
{
	if(changemode) ShowMode('S',2,modeclr);
	for(uint16_t i=1; i<=16; i++)       
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;   
			//y/=4;
			if(i==1)y/=3;
			if(y >8 )          
			{
				y = 8;
			}
			RGB_Column_Colors(i,y,clr_G2R);
			/*下落的点*/
			RGB_Point(i, fall_pot_y[i-1],0x005f00);
			if(fall_pot_y[i-1] <= y)          
			{
				if(y+2>8) y=6;
				fall_pot_y[i-1] = y+2;
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%20==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		freshpoint++;
}

void SpectrumMode3(void)
{
	if(changemode) ShowMode('S',3,modeclr);	
	for(uint16_t i=1; i<=16; i++)
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;   
			//y/=4;
			if(i==1)y/=3;
			if(y >8 )          
			{
				y = 8;
			}
			RGB_Column_Colors(i,y,clr_G2R);
		}
}
int clr_cdif[16]={0x004b82,0x2b8ae2,0x6951e1,0x901eff,0xbf00ff,0xfa009a,0xcd3232,0xffad2f,0xddea32,0x86b80b,0xd7ff00,0x95ff00,0x45ff00,0x208000,0x109000,0x00ff00};
void SpectrumMode4(void)
{
	if(changemode) ShowMode('S',4,modeclr);	
	for(uint16_t i=1; i<=16; i++)     
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;    
			//y/=4;
			if(i==1)y/=3;
			if(y >8 )         
			{
				y = 8;
			}
			RGB_Column(i,y,clr_cdif[i-1]);
		}
}

void SpectrumMode5(void)
{
	if(changemode) ShowMode('S',5,modeclr);	
	for(uint16_t i=1; i<=16; i++)      
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;    
			//y/=4;
			if(i==1)y/=3;
			if(y >8 )         
			{
				y = 8;
			}
			RGB_Column(i,y,clr_cdif[i-1]);
			/*下落的点*/
			RGB_Point(i, fall_pot_y[i-1],clr_cdif[i-1]);
			if(fall_pot_y[i-1] <= y)            //像素点不断刷新，并由频谱线举起，（行坐标越小，代表越高）
			{
				if(y+2>8) y=6;
				fall_pot_y[i-1] = y+2;
                				
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%20==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		freshpoint++;
}

void SpectrumMode6(void)
{
	if(changemode) ShowMode('S',6,modeclr);	
	for(uint16_t i=1; i<=8; i++)        
		{
			y = 0;
			for(int t=(i-1)*16;t<i*16;t++) y += (lBufMagArray[t] / 10)*8/48; 
			y/=2;
			if(i==1)y/=3;
			if(y >8 )         
			{
				y = 8;
			}
			RGB_Column(2*i,y,clr_cdif[i-1]);
			RGB_Column(2*i-1,y,clr_cdif[i-1]);
			RGB_Point(2*i, fall_pot_y[i-1],clr_cdif[i-1]);
			RGB_Point(2*i-1, fall_pot_y[i-1],clr_cdif[i-1]);
			if(fall_pot_y[i-1] <= y)            
			{
				if(y+2>8) y=6;
				fall_pot_y[i-1] = y+2;
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%20==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		freshpoint++;
}


void SpectrumMode7(void)
{
	if(changemode) ShowMode('S',7,modeclr);	
	for(uint16_t i=1; i<=16; i++)      
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*7/48;   
			//y/=3.5;
			if(i==1)y/=3;
			if(y >7 )         
			{
				y = 7;
			}
			
			RGB_Column(i,y,clr_cdif[i-1]);
			//RGB_Row_Colors(8,floor(1.97*log(abs(AudioSample[0]-2000)+1)+1),clr_cdif);   //对数映射
			/*下落的点*/
			RGB_Point(i, fall_pot_y[i-1],clr_cdif[i-1]);
			if(fall_pot_y[i-1] <= y)            //像素点不断刷新，并由频谱线举起，（行坐标越小，代表越高）
			{
				if(y+2>7) y=5;
				fall_pot_y[i-1] = y+2;
                				
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%20==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		//顶行声音大小
		int lv=abs(AudioSample[0]-1400)*16/1600;
		if(lv>2)lv*=2;
		if(lv>16)lv=16;
		RGB_Row_Colors(8,lv,clr_cdif);
		
		freshpoint++;
}

/*频谱灯模式8*/
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} CLRBIT;
CLRBIT S8_CLR[16];
int S8_color_counter=0,S8_DIR=0;
void SpectrumMode8(void)
{
	if(changemode) ShowMode('S',8,modeclr);	
	if(S8_color_counter>10000)S8_DIR=-1;
	if(S8_color_counter<1)S8_DIR=1;
	S8_color_counter+=S8_DIR;
	float phase_step = 2 * M_PI / 16;
    float phase_base = (float)S8_color_counter*18 / 1000.0;
    
    for (int i = 0; i < 16; i++) {
        float phase = phase_base + i * phase_step;
        S8_CLR[i].r = (uint8_t)((sin(phase) + 1) * 127);
        S8_CLR[i].g = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
        S8_CLR[i].b = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
    }
	for(uint16_t i=1; i<=16; i++)        //绘制128个列像素的每一点幅值，从1开始是为了舍弃谐波中的第一个直流分量
		{
			y = 0;
			for(int t=(i-1)*8;t<i*8;t++) y += (lBufMagArray[t] / 10)*8/48;   
			//y/=4;
			if(i==1)y/=3;
			if(y >8 )          //如果y超出了屏幕，则按最大值绘制
			{
				y = 8;
			}
			int SendClr=(((S8_CLR[i-1].b))|((S8_CLR[i-1].r)<<8)|((S8_CLR[i-1].g)<<16));
			RGB_Column(i,y,SendClr);
			/*下落的点*/
			RGB_Point(i, fall_pot_y[i-1],SendClr);//绘制一个像素点
			if(fall_pot_y[i-1] <= y)            //像素点不断刷新，并由频谱线举起，（行坐标越小，代表越高）
			{
				if(y+2>8) y=6;
				fall_pot_y[i-1] = y+2;
                              				
			}
			else
			{
				if (fall_pot_y[i-1]>=2 && freshpoint%10==0) 
				{
					fall_pot_y[i-1]--;
					freshpoint = 0;
				}					
			}
		}
		freshpoint++;
}

void LightMode1(void)
{
	if(changemode) ShowMode('L',1,modeclr);	
	WS2812B_SetBuf(0xffffff);
}
void LightMode2(void)
{
	if(changemode) ShowMode('L',2,modeclr);	
	WS2812B_SetBuf(0x888888);
}
void LightMode3(void)
{
	if(changemode) ShowMode('L',3,modeclr);	
	WS2812B_SetBuf(0x101010);
}
void LightMode4(void)
{
	if(changemode) ShowMode('L',4,modeclr);	
    RGB_AllBreath();
}

int L5_color_counter=0,L5_DIR,L5_CLR;
void LightMode5(void)
{
	if(changemode) ShowMode('L',5,modeclr);	
    float phase = (float)L5_color_counter * 5 / 1000.0; // 归一化到0-1范围
    uint8_t red = (uint8_t)((sin(phase) + 1) * 127);
    uint8_t green = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
    uint8_t blue = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
	if(L5_color_counter>10000)L5_DIR=-1;
	if(L5_color_counter<1)L5_DIR=1;
	L5_color_counter+=L5_DIR;
	L5_CLR=(((blue))|((red)<<8)|((green)<<16));
	WS2812B_SetBuf(L5_CLR);
}


int lastmin=0;
int m1,m2,s1,s2,h1,h2,y1,y2,y3,y4,mh1,mh2,d1,d2;//mh->月

void FlushTime()
{
	//DS1302_read_realTime();
	DS3231_ReadTime();
	{
		s1=DS3231_Time[5]/10;
		s2=DS3231_Time[5]%10;
		m1=DS3231_Time[4]/10;
		m2=DS3231_Time[4]%10;
		h1=DS3231_Time[3]/10;
		h2=DS3231_Time[3]%10;
		y1=2;
		y2=0;
		y3=DS3231_Time[0]/10;
		y4=DS3231_Time[0]%10;
		mh1=DS3231_Time[1]/10;
		mh2=DS3231_Time[1]%10;
		d1=DS3231_Time[2]/10;
		d2=DS3231_Time[2]%10;
		//lastmin=TimeData.minute;
	}
}

int ChangeTimeFlag=0,ChangeTimeDelay=300,ChangeTimeFinishFlag=1,InChangeTimeFlag=0;
int ChangePos=1;//改变的位置
int ChangeSparkDelay=10;
void ChangeTime_FlushTime()
{
	s1=DS3231_Time[5]/10;
	s2=DS3231_Time[5]%10;
	m1=DS3231_Time[4]/10;
	m2=DS3231_Time[4]%10;
	h1=DS3231_Time[3]/10;
	h2=DS3231_Time[3]%10;
	y1=2;
	y2=0;
	y3=DS3231_Time[0]/10;
	y4=DS3231_Time[0]%10;
	mh1=DS3231_Time[1]/10;
	mh2=DS3231_Time[1]%10;
	d1=DS3231_Time[2]/10;
	d2=DS3231_Time[2]%10;
}
void ChangeTime_ShowTime(int ChangePos,int clr)
{
	switch(ChangePos)
	{
		case 1://改变小时
		{
			if(ChangeSparkDelay<50)
			{
				RGB_Num(1,h1,clr);
				RGB_Num(2,h2,clr);
				RGB_Num(3,m1,clr);
				RGB_Num(4,m2,clr);
			}
			else if(ChangeSparkDelay<100)
			{
				RGB_Num(3,m1,clr);
				RGB_Num(4,m2,clr);
			}
			else ChangeSparkDelay=0;
			ChangeSparkDelay++;
			break;
		}
		case 2://改变分钟
		{
			if(ChangeSparkDelay<50)
			{
				RGB_Num(1,h1,clr);
				RGB_Num(2,h2,clr);
				RGB_Num(3,m1,clr);
				RGB_Num(4,m2,clr);
			}
			else if(ChangeSparkDelay<100)
			{
				RGB_Num(1,h1,clr);
				RGB_Num(2,h2,clr);
			}
			else ChangeSparkDelay=0;
			ChangeSparkDelay++;
			break;
		}
		case 3://改变年
		{
			if(ChangeSparkDelay<50)
			{
				RGB_Num(1,y1,clr);
				RGB_Num(2,y2,clr);
				RGB_Num(3,y3,clr);
				RGB_Num(4,y4,clr);
			}
			else if(ChangeSparkDelay<100)
			{
				;
			}
			else ChangeSparkDelay=0;
			ChangeSparkDelay++;
			break;
		}
		case 4://改变月
		{
			if(ChangeSparkDelay<50)
			{
				RGB_Num(1,mh1,clr);
				RGB_Num(2,mh2,clr);
				RGB_Num(3,d1,clr);
				RGB_Num(4,d2,clr);
			}
			else if(ChangeSparkDelay<100)
			{
				RGB_Num(3,d1,clr);
				RGB_Num(4,d2,clr);
			}
			else ChangeSparkDelay=0;
			ChangeSparkDelay++;
			break;
		}
		case 5://改变日
		{
			if(ChangeSparkDelay<50)
			{
				RGB_Num(1,mh1,clr);
				RGB_Num(2,mh2,clr);
				RGB_Num(3,d1,clr);
				RGB_Num(4,d2,clr);
			}
			else if(ChangeSparkDelay<100)
			{
				RGB_Num(1,mh1,clr);
				RGB_Num(2,mh2,clr);
			}
			else ChangeSparkDelay=0;
			ChangeSparkDelay++;
			break;
		}
	}
	InEXTI=0;
}
void ChangeTime()
{
	ChangeTimeFinishFlag=0;
	InChangeTimeFlag=1;
	while(ChangeTimeFinishFlag==0)
	{
		ChangeTime_FlushTime();
		ChangeTime_ShowTime(ChangePos,0x222222);
		WS2812B_UpdateBuf();
		RGB_Reset();
		
	}
}
void ShowTime(int showmode,int clr)
{
	switch(showmode)
	{
		case 1:
		{
			RGB_Num(1,h1,clr);
			RGB_Num(2,h2,clr);
			RGB_Num(3,m1,clr);
			RGB_Num(4,m2,clr);
			break;
		}
		case 2:
		{
			RGB_Num(1,y1,clr);
			RGB_Num(2,y2,clr);
			RGB_Num(3,y3,clr);
			RGB_Num(4,y4,clr);
			break;
		}
		case 3:
		{
			RGB_Num(1,mh1,clr);
			RGB_Num(2,mh2,clr);
			RGB_Num(3,d1,clr);
			RGB_Num(4,d2,clr);
			break;
		}
	}
	if(ChangeTimeFlag==1){ChangeTimeDelay--;ChangeTimeFinishFlag=0;}
	if(ChangeTimeDelay<0 && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_RESET) //4按键还是低电平，进入调时模式
	    ChangeTime();
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET) ChangeTimeFinishFlag=1; //4按键还是高电平，普通切换时钟模式显示内容，把ChangeTimeFinishFlag置为1
	if(ChangeTimeFinishFlag==1){ChangeTimeFlag=0;ChangeTimeDelay=300;}
	InEXTI=0;//下一次显示时间了，表明已经退出中断
}
void ClockMode1(void)
{
	if(changemode) ShowMode('C',1,modeclr);
	FlushTime();
	ShowTime(ClockShowContentMode,0xffffff);
}
//Clock模式颜色渐变全局变量，phase,color_counter,CL2_CLR,CL2_DIR均为公用
int color_counter=0,CL2_CLR,CL2_DIR;
float phase;

void ClockMode2(void)
{
	if(changemode) ShowMode('C',2,modeclr);
	phase = (float)color_counter / 1000.0; // 归一化到0-1范围
    uint8_t red = (uint8_t)((sin(phase) + 1) * 127);
    uint8_t green = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
    uint8_t blue = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
	if(color_counter>10000)CL2_DIR=-1;
	if(color_counter<1)CL2_DIR=1;
	color_counter+=CL2_DIR;
	CL2_CLR=(((blue))|((red)<<8)|((green)<<16));
	FlushTime();
	ShowTime(ClockShowContentMode,CL2_CLR);
}

void ClockMode3(void)
{
	if(changemode) ShowMode('C',3,modeclr);
	phase = (float)color_counter / 1000.0; // 归一化到0-1范围
    uint8_t red = (uint8_t)((sin(phase) + 1) * 127);
    uint8_t green = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
    uint8_t blue = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
	if(color_counter>10000)CL2_DIR=-1;
	if(color_counter<1)CL2_DIR=1;
	color_counter+=CL2_DIR;
	CL2_CLR=(((blue))|((red)<<8)|((green)<<16));
	FlushTime();
	ShowTime(ClockShowContentMode,CL2_CLR);
	RGB_Row(1,DS3231_Time[5]*17/60,CL2_CLR);
}

void ClockMode4(void)
{
	if(changemode) {ShowMode('C',4,modeclr);FFTInNeed = 1;}
	phase = (float)color_counter*18 / 1000.0; // 归一化到0-1范围
    uint8_t red = (uint8_t)((sin(phase) + 1) * 127);
    uint8_t green = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
    uint8_t blue = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
	if(color_counter>10000)CL2_DIR=-1;
	if(color_counter<1)CL2_DIR=1;
	color_counter+=CL2_DIR;
	CL2_CLR=(((blue))|((red)<<8)|((green)<<16));
	FlushTime();
	ShowTime(ClockShowContentMode,CL2_CLR);
	int lv=abs(AudioSample[0]-1400)*16/1600;
	if(lv>2)lv*=2;
	if(lv>16)lv=16;
	RGB_Row_Colors(8,lv,clr_cdif);
}

void ClockMode5(void)
{
	if(changemode) {ShowMode('C',5,modeclr);FFTInNeed = 1;}
	SpectrumMode3();
	phase = (float)color_counter*18 / 1000.0; // 归一化到0-1范围
    uint8_t red = (uint8_t)((sin(phase) + 1) * 127);
    uint8_t green = (uint8_t)((sin(phase + 2 * M_PI / 3) + 1) * 127);
    uint8_t blue = (uint8_t)((sin(phase + 4 * M_PI / 3) + 1) * 127);
	if(color_counter>10000)CL2_DIR=-1;
	if(color_counter<1)CL2_DIR=1;
	color_counter+=CL2_DIR;
	CL2_CLR=(((blue))|((red)<<8)|((green)<<16));
	FlushTime();
	ShowTime(ClockShowContentMode,CL2_CLR);

}

void OFFMode()
{
	StartM=0;
	if(changemode) 
	{
		int dl=100;
		while(dl)
		{
			RGB_Num_SetX(3,0,modeclr);
			RGB_Alpha_SetX(7,'F',modeclr);
			RGB_Alpha_SetX(11,'F',modeclr);
			WS2812B_UpdateBuf();	
			RGB_Reset();
			changemode=0;
			dl--;
		}
		InEXTI=0;
	}
}

/*中断处理函数*/
void  DMA1_Channel1_IRQHandler(void)  //DMA转运完成
{
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
    {
		if(FFTInNeed)
		{
        InitBufInArray();           //将数据256个数据均左移16位
        cr4_fft_256_stm32(lBufOutArray, lBufInArray, NPT);//进行FFT变换
        GetPowerMag();                                      //计算各次谐波幅值
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);                 //清除完成中断标志位
	}
}

void EXTI1_IRQHandler(void)   //按下频谱模式按键（PB1）
{  
	EXTI_ClearITPendingBit(EXTI_Line1);
    if(InEXTI==0)
	{
		InEXTI=1;
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET) {
			FFTInNeed = 1;
			if(InChangeTimeFlag==1)
			{
				ChangePos++;
				if(ChangePos>5)ChangePos=1;
			}
			else
			{
			changemode=1;
			if(mode>10)mode = 1;
			else
			{
            mode++;
			if(mode==NumOfSpectrumMode+1) mode = 1;
			}
		}
        }
	}
}

void EXTI2_IRQHandler(void) //按下灯光模式按键（PB2，与boot1是一个引脚）
{
	EXTI_ClearITPendingBit(EXTI_Line2);
    if(InEXTI==0)
	{
		InEXTI=1;
		eDelay();//Delay_ms(20);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == Bit_RESET) {
			
				changemode=1;
				FFTInNeed = 0;
				if(mode>20 || mode<=10)mode = 11;
				else
				{
					mode++;
					if(mode==NumOfLightMode+11) mode = 11;
				}
			}
        }
	}


void EXTI3_IRQHandler(void) //按下时钟模式按键（PB3）
{
	EXTI_ClearITPendingBit(EXTI_Line3);
    if(InEXTI==0)
	{
		InEXTI=1;
		eDelay();//Delay_ms(20);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == Bit_RESET) {
			if(InChangeTimeFlag==1)
			{
				switch(ChangePos)
				{
					case 1:{DS3231_Time[3]++;if(DS3231_Time[3]>23) DS3231_Time[3]=0;break;}
					case 2:{DS3231_Time[4]++;if(DS3231_Time[4]>59) DS3231_Time[4]=0;break;}
					case 3:{DS3231_Time[0]++;if(DS3231_Time[0]>50) DS3231_Time[0]=20;break;}
					case 4:{DS3231_Time[1]++;if(DS3231_Time[1]>12) DS3231_Time[1]=1;break;}
					case 5:{DS3231_Time[2]++;if(DS3231_Time[2]>31) DS3231_Time[2]=1;break;}
				}
			}
			else
			{
            changemode=1;
			ClockShowContentMode = 1;
            if(mode>30 || mode<=20)mode = 21;
			else
			{
				mode++;
				if(mode==NumOfClockMode+21) mode = 21;
			}
			}
        }
	}
}

void EXTI4_IRQHandler(void) //按下显示内容切换按键（PB4）
{
	EXTI_ClearITPendingBit(EXTI_Line4);
	if(mode>20 && mode<30)  //只在时钟模式下生效
	{
		if(InEXTI==0)
		{
			InEXTI=1;
			eDelay();//Delay_ms(20);
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_RESET)
			{
				if(InChangeTimeFlag==1)  //在更改时间模式下再按下切换显示模式按键，表示设定时间完毕，切出此模式
				{
					ChangeTimeFinishFlag=1;
					InChangeTimeFlag=0;
					DS3231_Time[5]=0;//新时间把秒设置为0
					DS3231_SetTime();   //设置新时间
				}
				else
				{
				ClockShowContentMode++;
				if(ClockShowContentMode==4) ClockShowContentMode = 1;
				ChangeTimeFlag=1;
				}
			}
	}}
	else {if(mode!=0){changemode=1;mode=0;FFTInNeed = 0;}}
}
int STClr=0xff0000;
void StartUp(void)
{
	RGB_Point(1,3,STClr);
	RGB_Point(1,4,STClr);
	RGB_Point(1,5,STClr);
	RGB_Point(1,6,STClr);
	//RGB_Point(1,7,STClr);
	RGB_Point(2,2,STClr);
	RGB_Point(2,7,STClr);
	RGB_Point(3,2,STClr);
	RGB_Point(3,3,STClr);
	//RGB_Point(3,7,STClr);
	RGB_Point(4,2,STClr);
	RGB_Point(4,7,STClr);
	RGB_Point(5,1,STClr);
	RGB_Point(5,3,STClr);
	RGB_Point(5,4,STClr);
	RGB_Point(5,5,STClr);
	RGB_Point(5,6,STClr);
	RGB_Point(3,7,STClr);
	
	RGB_Point(7,2,STClr);
	RGB_Point(7,6,STClr);
	RGB_Point(8,2,STClr);
	RGB_Point(8,5,STClr);
	RGB_Point(8,7,STClr);
	RGB_Point(9,2,STClr);
	RGB_Point(9,4,STClr);
	RGB_Point(9,7,STClr);
	RGB_Point(10,3,STClr);
	RGB_Point(10,7,STClr);
	
	RGB_Point(12,3,STClr);
	RGB_Point(12,4,STClr);
	RGB_Point(12,5,STClr);
	RGB_Point(12,6,STClr);
	//RGB_Point(12,7,STClr);
	RGB_Point(13,2,STClr);
	RGB_Point(13,7,STClr);
	RGB_Point(14,2,STClr);
	RGB_Point(14,3,STClr);
	RGB_Point(14,7,STClr);
	RGB_Point(15,2,STClr);
	RGB_Point(15,7,STClr);
	RGB_Point(16,1,STClr);
	RGB_Point(16,3,STClr);
	RGB_Point(16,4,STClr);
	RGB_Point(16,5,STClr);
	RGB_Point(16,6,STClr);
	//RGB_Point(16,7,STClr);
	
	for(int i=1;i<=16;i++)
	{
		RGB_Point(i,8,0x00074f);
		WS2812B_UpdateBuf();
		int k=300000;
	    while(k)k--;
	}
	
	
	StartM=1;
	
}

//ds1302时钟代码
/*
int main(void) {
    uint8_t sec, min, hour, day, month, week, year;
    DS1302_Init();

    while (1) {
        DS1302_Read_Time(&sec, &min, &hour, &day, &month, &week, &year);
        // 在这里可以添加代码将时间数据显示到LCD等设备上
    }
}

*/
