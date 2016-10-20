#include "Config.h"

#include "LED.h"

volatile u8 LedData[LED_NUM];
volatile u8 LedHandlePort;

/*****************************************************************************
 * Function      : LED_Handle
 * Description   : 操作LED的函数
 * Param         : u8 led_handle_port  
                   LED_STATUS status   
 * Returns       : 
 *
 * Record        :
 * 1.Date        : 2016/09/01
 *   Author      : Mr_Liang
 *   Modification: Created function
 *
 * 2.Date        : 2016/09/02
 *   Author      : Mr_Liang
 *   Modification: Create Version_3
*****************************************************************************/

//void LED_Handle(u8 led_handle_port, LED_STATUS status, )

void LED_Handle(u8 led_handle_port, LED_STATUS status)
{
#if  defined(LED_ENABLE)

    u8 xdata i;
    
/*
//-----------Version_1-------------
    if (status == LED_OFF) {
        tmp = LED_PORT;
        tmp |= led_handle_port;
        LED_PORT = tmp;
    }
    else if (status == LED_BLINK_SHORT) {
        LedHandlePort = led_handle_port;
        LedData = 20;                           // 0.2 second
    }
    else if (status == LED_BLINK_LONG) {
        LedHandlePort = led_handle_port;
        LedData = 100;                          // 1 second
    }
    else if (status == LED_ON) {
        tmp = LED_PORT;
        tmp &= ~led_handle_port;
        LED_PORT = tmp;
    }


//-----------Version_2-------------
    switch (status)
    {
        case LED_OFF :
            tmp = LED_PORT;
            tmp |= led_handle_port;
            LED_PORT = tmp;
                break;
                
        case LED_BLINK_SHORT :
            LedHandlePort = led_handle_port;
            LedData = 20;                           // 0.2 second
                break;
                
        case LED_BLINK_LONG :
            LedHandlePort = led_handle_port;
            LedData = 100;                          // 1 second
                break;
                
        case LED_ON :
            tmp = LED_PORT;
            tmp &= ~led_handle_port;
            LED_PORT = tmp;
                break;
                
        default:
                break;
    }
*/


//-----------Version_3-------------
    switch (status)
    {
        case LED_OFF :
            for (i = 0; i < LED_NUM; i ++) {       
                if (led_handle_port & 0x01) {
                    LedData[i] = 0;
                }
                led_handle_port >>= 1;
            }
                
                break;
                
        case LED_BLINK_SHORT :
            for (i = 0; i < LED_NUM; i ++) {       
                if (led_handle_port & 0x01) {
                    LedData[i] = 20;
                }
                led_handle_port >>= 1;
            }
                break;
                
        case LED_BLINK_LONG :
            for (i = 0; i < LED_NUM; i ++) {       
                if (led_handle_port & 0x01) {
                    LedData[i] = 100;
                }
                led_handle_port >>= 1;
            }
                break;
                
        case LED_ON :
            for (i = 0; i < LED_NUM; i ++) {       
                if (led_handle_port & 0x01) {
                    LedData[i] = 255;               // LedData 的值为 0xff 时LED常亮
                }
                led_handle_port >>= 1;
            }
                break;
                
        default:
                break;
    }
#endif // #if defined(LED_ENABLE) 
}

/*****************************************************************************
 * Function      : LED_Service
 * Description   : LED Service in Interrupt routine
 * Param         : void  
 * Returns       : 
 *
 * Record        :
 * 1.Date        : 2016/09/01
 *   Author      : Mr_Liang
 *   Modification: Created function
*****************************************************************************/
void LED_Service(void)
{
#if  defined(LED_ENABLE)

    u8 xdata i, tmp;
    
/*
    if (LedData > 0) {          
        LedData --;

        tmp = LED_PORT;
        tmp &= ~LedHandlePort;
        LED_PORT = tmp;
    }
    else {
        tmp = LED_PORT;
        tmp |= LedHandlePort;
        LED_PORT = tmp;
    }
*/

    for (i = 0; i < LED_NUM; i ++) {       
        if (LedData[i] > 0) {
            LedData[i] --;
            tmp &= ~(1 << i);
            if (LedData[i] == 254) {    // LedData 的值为 0xff 时LED常亮
                                        // 这里判断条件为254的原因是因为上面自减了1
                LedData[i] = 255;       // 将 LedData 重新赋值。防止减到0.
            }
        }
        else {
            tmp |= 1 << i;
        }
    }
    LED_PORT = tmp;

#endif /* #if defined(LED_ENABLE) */
}
