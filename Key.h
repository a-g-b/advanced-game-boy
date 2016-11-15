#ifndef __KEY_H__
#define __KEY_H__

    #include <STC12C5A60S2.H>
    #include "Menu.h"

    #define BUZZER_SHORT_VOICE 1000
    #define KEY1_DELAY 20
    #define KEY2_DELAY 20
    #define KEY3_DELAY 20
    #define KEY4_DELAY 20
//    #define KEY_DELAY_LONG     400
//    #define KEY_CNST_ADD_DELAY 200

    typedef enum
    {
        KV_NULL,
        KV_UP,
        KV_DOWN,
        KV_LEFT,
        KV_RIGHT,
        KV_ENTER,
        KV_ESC
    }KV_ENUM;

    extern KV_ENUM  volatile xdata KeyValue;
    extern u16      volatile xdata BuzzerVoice;

    void Key_Scan();
//  void KeyService();

#endif
