#include "stm32f10x.h"
#include "WS2812B.h"
#include "RCC.h"
#include "Delay.h"
#include "IO.h"
#include "TIM3.h"
#include <stdio.h>
#include <stdlib.h>

int COORD[8][16]={   //??????
{  0,  1,  2,  3,  4,  5,  6,  7, 64, 65, 66, 67, 68, 69, 70, 71},
{ 15, 14, 13, 12, 11, 10,  9,  8, 79, 78, 77, 76, 75, 74, 73, 72},
{ 16, 17, 18, 19, 20, 21, 22, 23, 80, 81, 82, 83, 84, 85, 86, 87},
{ 31, 30, 29, 28, 27, 26, 25, 24, 95, 94, 93, 92, 91, 90, 89, 88},
{ 32, 33, 34, 35, 36, 37, 38, 39, 96, 97, 98, 99,100,101,102,103},
{ 47, 46, 45, 44, 43, 42, 41, 40,111,110,109,108,107,106,105,104},
{ 48, 49, 50, 51, 52, 53, 54, 55,112,113,114,115,116,117,118,119},
{ 63, 62, 61, 60, 59, 58, 57, 56,127,126,125,124,123,122,121,120}
};

void RGB_Point(u8 x,u8 y,int color)
{
	WS2812B_Buf[COORD[y-1][x-1]]=color;
}

void RGB_Point_AddClr(u8 x,u8 y,int addcolor)
{
	WS2812B_Buf[COORD[y-1][x-1]]+=addcolor;
}

void RGB_Reset()
{
	for(int i =0;i<8;i++)
	{
		for(int j =0;j<16;j++)
		    	WS2812B_Buf[COORD[i][j]]=0x000000;
	}
}
void RGB_Alpha(u8 pos,char alpha,int color)
{
	int px = 0;
	if(pos==1) px = 0;
	if(pos==2) px = 4;
	if(pos==3) px = 9;
	if(pos==4) px = 13;
	switch(alpha)
	{
		case 'A':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'B':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'C':
		{
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);

			break;
		}
		case 'D':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'E':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,5,color);
			break;
		}
		case 'F':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,5,color);
			break;
		}
		case 'H':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'I':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(2+px,6,color);
			RGB_Point(2+px,4,color);
			break;
		}
		case 'J':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(2+px,3,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'K':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(2+px,6,color);
			RGB_Point(2+px,4,color);
			break;
		}
		case 'L':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,3,color);
			break;
		}
		case 'O':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,4,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,6,color);
			RGB_Point(2+px,3,color);
			break;
		}
		case 'P':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(1+px,3,color);
			break;
		}
		case 'R':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,4,color);
			RGB_Point(3+px,3,color);
			break;
		}
		case 'S':
		{
			RGB_Point(3+px,4,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(3+px,7,color);
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			break;
		}
		case 'T':
		{
			RGB_Point(2+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,6,color);
			RGB_Point(2+px,3,color);
			RGB_Point(2+px,4,color);
			RGB_Point(3+px,7,color);
			break;
		}
		case 'U':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,4,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,3,color);
			break;
		}
		case 'X':
		{
			RGB_Point(1+px,4,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,3,color);
			RGB_Point(3+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,3,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 'Y':
		{
			RGB_Point(2+px,4,color);
			RGB_Point(2+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(3+px,7,color);
			break;
		}
		case 'Z':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			break;
		}
	}
	
}

void RGB_Alpha_SetX(u8 px,char alpha,int color)
{
	switch(alpha)
	{
		case 'F':
		{
			RGB_Point(1+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,5,color);;
			break;
		}
	}
}



void RGB_Num(u8 pos,u8 num,int color)
{
	int px = 0;
	if(pos==1) px = 0;
	if(pos==2) px = 4;
	if(pos==3) px = 9;
	if(pos==4) px = 13;
	switch(num)
	{
		case 0:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 1:
		{
			RGB_Point(2+px,3,color);
			RGB_Point(2+px,4,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,6,color);
			RGB_Point(2+px,7,color);
			break;
		}
		case 2:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			break;
		}
		case 3:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 4:
		{
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 5:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 6:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 7:
		{
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 8:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		case 9:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,5,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
			break;
		}
		
	}
}

void RGB_Num_SetX(u8 px,u8 num,int color)
{
	switch(num)
	{
		case 0:
		{
			RGB_Point(1+px,3,color);
			RGB_Point(2+px,3,color);
			RGB_Point(3+px,3,color);
			RGB_Point(1+px,4,color);
			RGB_Point(1+px,5,color);
			RGB_Point(1+px,6,color);
			RGB_Point(1+px,7,color);
			RGB_Point(2+px,7,color);
			RGB_Point(3+px,7,color);
			RGB_Point(3+px,6,color);
			RGB_Point(3+px,5,color);
			RGB_Point(3+px,4,color);
		}
	}
}
	
void RGB_Num_Ydisp(u8 pos,u8 num,int color,int Ydisp)
{
	int px = 0;
	if(pos==1) px = 0;
	if(pos==2) px = 4;
	if(pos==3) px = 9;
	if(pos==4) px = 13;
	switch(num)
	{
		case 0:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,4+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 1:
		{
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(2+px,4+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,6+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			break;
		}
		case 2:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,4+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			break;
		}
		case 3:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 4:
		{
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 5:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 6:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,4+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 7:
		{
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 8:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,4+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		case 9:
		{
			RGB_Point(1+px,3+Ydisp,color);
			RGB_Point(2+px,3+Ydisp,color);
			RGB_Point(3+px,3+Ydisp,color);
			RGB_Point(1+px,5+Ydisp,color);
			RGB_Point(1+px,6+Ydisp,color);
			RGB_Point(1+px,7+Ydisp,color);
			RGB_Point(2+px,5+Ydisp,color);
			RGB_Point(2+px,7+Ydisp,color);
			RGB_Point(3+px,7+Ydisp,color);
			RGB_Point(3+px,6+Ydisp,color);
			RGB_Point(3+px,5+Ydisp,color);
			RGB_Point(3+px,4+Ydisp,color);
			break;
		}
		
	}
}

void RGB_Column(u8 col,u8 num,int color)
{
	for(int i=1;i<=num;i++)
	{
		RGB_Point(col,i,color);
	}
}

void RGB_Column_AddClr(u8 col,u8 num,int color)
{
	for(int i=1;i<=num;i++)
	{
		RGB_Point_AddClr(col,i,color);
	}
}

void RGB_Column_Colors(u8 col,u8 num,int *color)
{
	for(int i=1;i<=num;i++)
	{
		RGB_Point(col,i,color[i-1]);
	}
}

void RGB_Row(u8 row,u8 num,int color)
{
	for(int i=1;i<=num;i++)
	{
		RGB_Point(i,row,color);
	}
}

void RGB_Row_Colors(u8 row,u8 num,int *color)
{
	for(int i=1;i<=num;i++)
	{
		RGB_Point(i,row,color[i-1]);
	}
}

uint8_t RGB_AllBreath_i,RGB_AllBreath_Color;
void RGB_AllBreath(void)
{
	

	if(RGB_AllBreath_i==0)WS2812B_SetBuf((RGB_AllBreath_Color));
	if(RGB_AllBreath_i==1)WS2812B_SetBuf((255-RGB_AllBreath_Color));
	if(RGB_AllBreath_i==2)WS2812B_SetBuf((RGB_AllBreath_Color)<<8);
	if(RGB_AllBreath_i==3)WS2812B_SetBuf((255-RGB_AllBreath_Color)<<8);
	if(RGB_AllBreath_i==4)WS2812B_SetBuf((RGB_AllBreath_Color)<<16);
	if(RGB_AllBreath_i==5)WS2812B_SetBuf((255-RGB_AllBreath_Color)<<16);
	if(RGB_AllBreath_i==6)WS2812B_SetBuf((RGB_AllBreath_Color)|((RGB_AllBreath_Color)<<8));
	if(RGB_AllBreath_i==7)WS2812B_SetBuf((255-RGB_AllBreath_Color)|((255-RGB_AllBreath_Color)<<8));
	if(RGB_AllBreath_i==8)WS2812B_SetBuf((RGB_AllBreath_Color)|((RGB_AllBreath_Color)<<16));
	if(RGB_AllBreath_i==9)WS2812B_SetBuf((255-RGB_AllBreath_Color)|((255-RGB_AllBreath_Color)<<16));
	if(RGB_AllBreath_i==10)WS2812B_SetBuf(((RGB_AllBreath_Color)<<8)|((RGB_AllBreath_Color)<<16));
	if(RGB_AllBreath_i==11)WS2812B_SetBuf(((255-RGB_AllBreath_Color)<<8)|((255-RGB_AllBreath_Color)<<16));
	if(RGB_AllBreath_i==12)WS2812B_SetBuf(((RGB_AllBreath_Color))|((RGB_AllBreath_Color)<<8)|((RGB_AllBreath_Color)<<16));
	if(RGB_AllBreath_i==13)WS2812B_SetBuf(((255-RGB_AllBreath_Color))|((255-RGB_AllBreath_Color)<<8)|((255-RGB_AllBreath_Color)<<16));

	RGB_AllBreath_Color++;
	if(RGB_AllBreath_Color==0)
	{
		RGB_AllBreath_i++;
		RGB_AllBreath_i%=14;
	}
}

