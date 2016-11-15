#include <STC12C5A60S2.H>

#include "Config.h"
#include "12864.h"
#include "Delay.h"
#include "Timer.h"
#include "Uart.h"
#include "Menu.h"
#include "Key.h"

void IO_Init();

void main()
{
    Delay_Xms(200);
    IO_Init();
    Timer0_Init();
    Uart_Init();
    Lcd_Init();

    BACK_LED = 0;

    ENABLE_ALL_INTERRUPTS;

    while (1)
    {
        Menu_Service(pfCurrentFunc);
        UartSendDebugInfo("while\r\n");
    }
}

void IO_Init()
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;

    UartSendDebugInfo("IO      Init_Successfully :)\r\n");
}