#ifndef __RGB_H
#define __RGB_H


void RGB_Point(u8,u8,int);
void RGB_Point_AddClr(u8,u8,int);
void RGB_Alpha(u8,char,int);
void RGB_Num(u8,u8,int);
void RGB_Alpha_SetX(u8,char,int);
void RGB_Num_SetX(u8,u8,int);
void RGB_Reset(void);
void RGB_Column(u8,u8,int);
void RGB_Row(u8,u8,int);
void RGB_Column_Colors(u8,u8,int*);
void RGB_Row_Colors(u8,u8,int*);
void RGB_Column_AddClr(u8,u8,int);
void RGB_AllBreath(void);

#endif
