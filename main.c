 /*****************************************************************************
 * File Name     : main.c
 * Author        : Mr_Liang
 * Date          : 2016/09/20
 * Description   : .C file function description
 * Version       : 3.1
 *
 * Record        :
 * 1.Date        : 2016/07/20
 *   Author      : Mr_Liang
 *   Modification: Created file
*****************************************************************************/

/*

修改了 "STARTUP.A51" 文件中的 XDATALEN 参数
使单片机上电初始化 xdata 区
根据 STC15W4K32S4 的数据手册，xdata的地址为 0H - EFFH
因此，  修改后的 XDATALEN 参数为 0xF00 十进制为 3840

ps: 如果不修改这个参数的话，每个存储在xdata的变量都需要在声明时初始化，不然数据可能是乱码。
    E.G. :
            unsigned char xdata i = 0;
            unsigned int  xdata Arr[10] = {0};

*/
#include <STC15.h>

#include "Config.h"
#include "12864.h"
#include "Delay.h"
#include "Timer.h"
#include "Uart.h"
#include "Menu.h"
#include "Key.h"
#include "LED.h"

#include "Message.h"

void IO_Init();

void main()
{    
    Timer0_Init();
    Uart_Init();
    IO_Init();
    Lcd_Init();

    ENABLE_ALL_INTERRUPTS;

    LED_Handle(LED_7, LED_BLINK_LONG);
    
    while(1)
    {
        // MenuGeneralSet();
        Menu_Service(pfCurrentFunc);
        // KeyService();
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
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

    UartSendDebugInfo("IO      Init_Successfully :)\r\n");
}