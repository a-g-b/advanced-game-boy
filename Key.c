#include <STC15.h>

#include "Config.h"
#include "Key.h"

KV_ENUM volatile xdata KeyValue;
u16     volatile xdata BuzzerVoice;

/*
void KeyService()
{
    switch (KeyValue)
    {
    case 1:
        // KeyValue = 0;
        BuzzerVoice = BUZZER_SHORT_VOICE;
            break;
    case 2:
        // KeyValue = 0;
        BuzzerVoice = BUZZER_SHORT_VOICE;
            break;
    case 3:
        // KeyValue = 0;
        BuzzerVoice = BUZZER_SHORT_VOICE;
            break;
    case 4:
        // KeyValue = 0;
        BuzzerVoice = BUZZER_SHORT_VOICE;
            break;
    default:

            break;
    }
}
*/

/*
KV_ENUM Key_GetKV(void)
{
    switch (KeyValue)
    {
        case KV_UP:                   // 向上
            return KV_UP;
                break;

        case KV_DOWN:                   // 向下
            return KV_DOWN;
                break;

        case KV_LEFT:
            return KV_LEFT;
                break;

        case KV_RIGHT:
            return KV_RIGHT;
                break;

        case KV_ENTER:                  // 确认
            return KV_ENTER;
                break;

        case KV_ESC:                    // 退出
            return KV_ESC;
                break;
        default:
            return KV_NULL;                                                                                                                                                                                                                                                                                                                                                                                    
                break;    
    }
}
*/

void Key_Scan() {
    static u8   KeyCnt1, KeyCnt2, KeyCnt3, KeyCnt4;
    static bit  KeyLock1, KeyLock2, KeyLock3, KeyLock4;

    if (KEY1 == 1) {
        KeyLock1 = 0;
        KeyCnt1 = 0;
    }
    else if (KeyLock1 == 0) {
        KeyCnt1++;
        if(KeyCnt1 > KEY1_DELAY) {
            KeyCnt1 = 0;
            KeyLock1 = 1;
            KeyValue = KV_UP;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    if (KEY2 == 1) {
        KeyLock2 = 0;
        KeyCnt2 = 0;
    }
    else if (KeyLock2 == 0) {
        KeyCnt2++;
        if(KeyCnt2 > KEY2_DELAY) {
            KeyCnt2 = 0;
            KeyLock2 = 1;
            KeyValue = KV_DOWN;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    if (KEY3 == 1) {
        KeyLock3 = 0;
        KeyCnt3 = 0;
    }
    else if (KeyLock3 == 0) {
        KeyCnt3++;
        if (KeyCnt3 > KEY3_DELAY) {
            KeyCnt3 = 0;
            KeyLock3 = 1;
            KeyValue = KV_ENTER;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    if (KEY4 == 1) {
        KeyLock4 = 0;
        KeyCnt4 = 0;
    }
    else if (KeyLock4 == 0) {
        KeyCnt4++;
        if (KeyCnt4 > KEY4_DELAY) {
            KeyCnt4 = 0;
            KeyLock4 = 1;
            KeyValue = KV_ESC;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
}

/*
void Key_Scan() 
{
    static u16 xdata KeyCnt1, KeyCnt2, KeyCnt3, KeyCnt4;
    static u16 xdata KeyCnstAddCnt1, KeyCnstAddCnt2;//, KeyCnstAddCnt3, KeyCnstAddCnt4;
    static bit       KeyLock1, KeyLock2, KeyLock3, KeyLock4;
    
    if(KEY1 == 1) {
        KeyLock1 = 0;
        KeyCnt1 = 0;
    }
    else if(KeyLock1 == 0) {
        KeyCnt1++;
        if(KeyCnt1 > KEY1_DELAY) {
            KeyCnt1 = 0;
            KeyLock1 = 1;
            KeyValue = KV_UP;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    else if(KeyCnt1 < KEY_DELAY_LONG) {
        KeyCnt1++;
    }
    else {
        KeyCnstAddCnt1++;
        if(KeyCnstAddCnt1 > KEY_CNST_ADD_DELAY) {
            KeyCnstAddCnt1= 0;
            KeyValue = KV_UP;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }

    if(KEY2 == 1) {
        KeyLock2 = 0;
        KeyCnt2 = 0;
    }
    else if(KeyLock2 == 0) {
        KeyCnt2++;
        if(KeyCnt2 > KEY2_DELAY) {
            KeyCnt2 = 0;
            KeyLock2 = 1;
            KeyValue = KV_DOWN;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    else if(KeyCnt2 < KEY_DELAY_LONG) {
        KeyCnt2++;
    }
    else {
        KeyCnstAddCnt2++;
        if(KeyCnstAddCnt2 > KEY_CNST_ADD_DELAY) {
            KeyCnstAddCnt2 = 0;
            KeyValue = KV_DOWN;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }

    if (KEY3 == 1) {
        KeyLock3 = 0;
        KeyCnt3 = 0;
    }
    else if (KeyLock3 == 0) {
        KeyCnt3++;
        if (KeyCnt3 > KEY3_DELAY) {
            KeyCnt3 = 0;
            KeyLock3 = 1;
            KeyValue = KV_ENTER;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }
    if (KEY4 == 1) {
        KeyLock4 = 0;
        KeyCnt4 = 0;
    }
    else if (KeyLock4 == 0) {
        KeyCnt4++;
        if (KeyCnt4 > KEY4_DELAY) {
            KeyCnt4 = 0;
            KeyLock4 = 1;
            KeyValue = KV_ESC;
            BuzzerVoice = BUZZER_SHORT_VOICE;
        }
    }

    if(KEY3 == 1) {
        KeyLock3 = 0;
        KeyCnt3 = 0;
    }
    else if(KeyLock3 == 0) {
        KeyCnt3++;
        if(KeyCnt3 > KEY3_DELAY) {
            KeyCnt3 = 0;
            KeyLock3 = 1;
            KeyValue = KV_ENTER;
        }
    }
    else if(KeyCnt3 < KEY_DELAY_LONG) {
        KeyCnt3++;
    }
    else {
        KeyCnstAddCnt3++;
        if(KeyCnstAddCnt3 > KEY_CNST_ADD_DELAY) {
            KeyCnstAddCnt3 = 0;
            KeyValue = KV_ENTER;
        }
    }

    if(KEY4 == 1) {
        KeyLock4 = 0;
        KeyCnt4 = 0;
    }
    else if(KeyLock4 == 0) {
        KeyCnt4++;
        if(KeyCnt4 > KEY4_DELAY) {
            KeyCnt4 = 0;
            KeyLock4 = 1;
            KeyValue = KV_ESC;
        }
    }
    else if(KeyCnt4 < KEY_DELAY_LONG) {
        KeyCnt4++;
    }
    else {
        KeyCnstAddCnt4++;
        if(KeyCnstAddCnt4 > KEY_CNST_ADD_DELAY) {
            KeyCnstAddCnt4 = 0;
            KeyValue = KV_ESC;
        }
    }
}
*/
