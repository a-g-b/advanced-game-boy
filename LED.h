/*****************************************************************************
 * File Name     : LED.h
 * Author        : Mr_Liang
 * Date          : 2016/08/30
 * Description   : LED.c header file
 * Version       : 1.0
 *
 * Record        :
 * 1.Date        : 2016/08/30
 *   Author      : Mr_Liang
 *   Modification: Created file
*****************************************************************************/

#ifndef __LED_H__
#define __LED_H__

#include "Config.h"

#define LED_NUM 8

enum
{
    LED_1 = 0x01,
    LED_2 = 0x02,
    LED_3 = 0x04,
    LED_4 = 0x08,
    LED_5 = 0x10,
    LED_6 = 0x20,
    LED_7 = 0x40,
    LED_8 = 0x80,
};

#define LED_PORT P0

//    #define LED_1 0x01
//    #define LED_2 0x02
//    #define LED_3 0x04
//    #define LED_4 0x08
//    #define LED_5 0x10
//    #define LED_6 0x20
//    #define LED_7 0x40
//    #define LED_8 0x80


//----Only fit on V1 V2 V3----
typedef enum
{
    LED_OFF,
    LED_BLINK_SHORT,
    LED_BLINK_LONG,
    LED_ON,
} LED_STATUS;


/*
    typedef enum
    {
        LED_OFF,
        LED_BLINK,
        LED_ON,
    }LED_STATUS;
*/

extern volatile u8 LedData[LED_NUM];
extern volatile u8 LedHandlePort;

extern void LED_Handle(u8 led_handle_port, LED_STATUS status);
extern void LED_Service(void);

#endif /* __LED_H__ */
