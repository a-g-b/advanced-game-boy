#ifndef __CONFIG_H__
#define __CONFIG_H__

    #include <STC12C5A60S2.H>
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <intrins.h>

    typedef signed char     s8;
    typedef signed int      s16;
    typedef signed long     s32;
    typedef unsigned char   u8;
    typedef unsigned int    u16;
    typedef unsigned long   u32;

    #define FALSE 0
    #define TRUE  1

    #define UART_ENABLE
    #define USE_UART_SEND_DEBUG_DATA
    #define USE_UART_SEND_DEBUG_INFO
//    #define USE_LCD_DRAW_GRAPH      // Lines, Rectengle, Circle  

//    sbit LED_1 = P0^0;
//    sbit LED_2 = P0^1;
//    sbit LED_3 = P0^2;
//    sbit LED_4 = P0^3;
//    sbit LED_5 = P0^4;33
//    sbit LED_6 = P0^5;
//    sbit LED_7 = P0^6;
//    sbit LED_8 = P0^7;

    sbit KEY1 = P1^1;   // up
    sbit KEY2 = P1^2;   // down
    sbit KEY3 = P1^3;   // left
    sbit KEY4 = P1^0;   // right
    sbit KEY5 = P3^3;   // enter
    sbit KEY6 = P3^2;   // esc

    sbit Buzzer = P1^5;

    #define BUZZER_ON   Buzzer = 0
    #define BUZZER_OFF  Buzzer = 1

#endif
