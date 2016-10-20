#include "Delay.h"
#include "Config.h"

void Delay1us()     //@24.000MHz
{
    u8 i;

    _nop_();
    _nop_();
    i = 2;
    while (--i);
}

void Delay1ms()     //@24.000MHz
{
    u8 i, j;

    i = 24;
    j = 85;
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