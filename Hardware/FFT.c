#include "stm32f10x.h"
#include "math.h"
extern uint16_t AudioSample[256];
extern uint32_t NPT,Fs;
extern float PI2;
extern long lBufInArray[256], lBufOutArray[128],lBufMagArray[128];
void GetPowerMag()
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        lX  = (lBufOutArray[i] << 16) >> 16;
        lY  = (lBufOutArray[i] >> 16);
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}
void InitBufInArray()
{
    unsigned short i;
    for(i=0; i<NPT; i++)
    {
        lBufInArray[i] = ((signed short)(AudioSample[i] - 1400)) << 16;
    }
}


