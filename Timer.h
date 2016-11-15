#ifndef __TIMER_H__
#define __TIMER_H__

    #define ENABLE_ALL_INTERRUPTS   EA = 1;
    #define DISABLE_ALL_INTERRUPTS  EA = 0;

    extern volatile u16 xdata FreqCnt;

    void Timer0_Init();
            
#endif
