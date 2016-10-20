#include <STC15.h>

#include "Config.h"
#include "12864.h"
#include "Uart.h"
#include "Key.h"
#include "LED.h"
#include "Snake.h"

void Timer0_Init(void)      //100us @24.000MHz
{
    AUXR |= 0x80;   //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;   //���ö�ʱ��ģʽ
    TL0 = 0xA0;     //���ö�ʱ��ֵ
    TH0 = 0xF6;     //���ö�ʱ��ֵ
    TF0 = 0;        //���TF0��־
    ET0 = 1;
    TR0 = 1;        //��ʱ��0��ʼ��ʱ
}

void Timer0_ISR() interrupt 1   // 100us
{
    static u8  n500ms;
    static u16 TimerCnt;
    static bit b_1msFlag, b_10msFlag, b_100msFlag;
    
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
        LED_Service();
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