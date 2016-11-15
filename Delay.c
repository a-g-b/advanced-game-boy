#include "Delay.h"
#include "Config.h"

void Delay1us()     //@11.0592MHz
{
    _nop_();
}

void Delay1ms()     //@11.0592MHz
{
    unsigned char i, j;

    _nop_();
    i = 11;
    j = 190;
    do
    {
        while (--j);
    } while (--i);
}


void Delay_Xus(u16 i) 
{
    while(--i) {
        Delay1us();
    }
}

void Delay_Xms(u16 i) 
{
    while(--i) {
        Delay1ms();
    }
}
