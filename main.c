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

�޸��� "STARTUP.A51" �ļ��е� XDATALEN ����
ʹ��Ƭ���ϵ��ʼ�� xdata ��
���� STC15W4K32S4 �������ֲᣬxdata�ĵ�ַΪ 0H - EFFH
��ˣ�  �޸ĺ�� XDATALEN ����Ϊ 0xF00 ʮ����Ϊ 3840

ps: ������޸���������Ļ���ÿ���洢��xdata�ı�������Ҫ������ʱ��ʼ������Ȼ���ݿ��������롣
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