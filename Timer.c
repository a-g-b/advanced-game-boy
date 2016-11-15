#include "Config.h"
#include "12864.h"
#include "Uart.h"
#include "Key.h"
#include "Game_Snake.h"

volatile u16 xdata FreqCnt = 0;

/*
void Timer0_Init(void)      //100us @24.000MHz
{
    AUXR |= 0x80;   //定时器时钟1T模式
    TMOD &= 0xF0;   //设置定时器模式
    TL0 = 0xA0;     //设置定时初值
    TH0 = 0xF6;     //设置定时初值
    TF0 = 0;        //清除TF0标志
    ET0 = 1;
    TR0 = 1;        //定时器0开始计时
}
*/

void Timer0_Init(void)       //100us@11.0592MHz
{
    AUXR |= 0x80;       //Timer clock is 1T mode
    TMOD &= 0xF0;       //Set timer work mode
    TMOD |= 0x01;       //Set timer work mode
    TL0 = 0xAE;     //Initial timer value
    TH0 = 0xFB;     //Initial timer value
    TF0 = 0;        //Clear TF0 flag
    ET0 = 1;
    TR0 = 1;        //Timer0 start run
}

void Timer0_ISR() interrupt 1   // 100us
{
    static u8  n500ms;
    static u16 TimerCnt;
    static bit b_1msFlag, b_10msFlag, b_100msFlag;

    TL0 = 0xAE;     //Initial timer value
    TH0 = 0xFB;     //Initial timer value
//    UartSendDebugInfo("Timer\r\n");
    TimerCnt++;
    if ((TimerCnt % 10) == 0)
    {
        b_1msFlag = 1;
    }
    if ((TimerCnt % 100) == 0)
    {
        b_10msFlag = 1;
    }
    if ((TimerCnt % 1000) == 0)
    {
        TimerCnt = 0;
        b_100msFlag = 1;
    }
    
    if (b_1msFlag)
    {
        ++FreqCnt;
        b_1msFlag = 0;
        Key_Scan();
    }
    if (b_10msFlag)
    {
        b_10msFlag = 0;
    }
    if (b_100msFlag)
    {
        b_100msFlag = 0;
        if (++n500ms == 5) {
            n500ms = 0;
//            ++SkScore;
            fNewFood= 1;
        }
    }
    
    // Buzzer Service
    if (BuzzerVoice > 0) {
        BuzzerVoice--;
        BUZZER_ON;
    } else BUZZER_OFF;

}
