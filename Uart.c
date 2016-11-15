#include "Config.h"
#include "Uart.h"

/*----------------------------
���ڷ�������
----------------------------*/
static void UartSendData(unsigned char dat)
{
#if defined(UART_ENABLE)

    SBUF = dat;                     //���͵�ǰ����
    while (!TI);                    //�ȴ����ݷ������
    TI = 0;                         //������ͱ�־

#endif
}

/*----------------------------
���ڷ����ַ���
----------------------------*/

void UartSendString(unsigned char *str)
{
#if defined(UART_ENABLE)

    while(*str!='\0')
    {
        UartSendData(*str++);
    }

#endif
}

/*----------------------------
������ʮ��������ʽ�������������ascii
Ĭ�Ϸ���ǰ׺"0x" ��׺" "
----------------------------*/

void UartSendHexNum(u8 dat)     // Max Send Num : 0xFF
{
#if defined(UART_ENABLE)

    u8 xdata Hnum, Lnum;

    Hnum = dat / 16;
    Lnum = dat % 16;

    UartSendString("0x");   // ǰ׺"0x"

    if (Hnum < 10)
    {
        UartSendData((Hnum + 48));
    }
    else
    {
        UartSendData((Hnum + 65 - 10));
    }

    if (Lnum < 10)
    {
        UartSendData((Lnum + 48));
    }
    else
    {
        UartSendData((Lnum + 65 - 10));
    }

    UartSendString(" ");    // ��׺" "

#endif
}

/*----------------------------
������ʮ������ʽ�������������ascii
    ps : ��������Ǽ�λ���ͷ���λ��ASCII
        E.G. : �������Ϊ 100�� ������� '1' '0' '0' ' ' ��ASCII (���һλΪ���һ���ո�)
               �������Ϊ 15��  ������� '1' '5' ' ' ��ASCII (���һλΪ���һ���ո�)
----------------------------*/

void UartSendDecNum(u8 dat)     // Max Send Num : 255
{
#if defined(UART_ENABLE)

    u8 xdata digit[3];
    s8 xdata num;

    digit[2] = dat / 100;
    digit[1] = dat % 100 / 10;
    digit[0] = dat % 10;

    num = 2;
    if (digit[2] == 0)
    {
        num = 1;
        if (digit[1] == 0)
            num = 0;
    }

    for ( ; num >= 0; num --)
    {
        UartSendData((digit[num] + 48));
    }

    UartSendString(" ");

#endif
}

/*******************************************************
* �� �� ��: void UartSendArray(u16 size, bit mode, u8 * pArr)
* ��������: ������"ASCII"��ʽ����������������
* �������:
            size :  ������鳤��
            mode :  0 Ϊʮ������ʽ�����
                    1 Ϊʮ��������ʽ���
            *pArr : ָ�������ָ��
* �� �� ֵ: NULL
*******************************************************/
/*
void UartSendArray(u16 size, UART_OPT_MODE mode, u8 *pArr)
{
    #if defined(UART_ENABLE)

    while (size --)
    {
        if (mode == DEC)                // Output decimal
        {
            UartSendDecNum(*pArr ++);
        }
        else if (mode == HEX)           // Output hex
        {
            UartSendHexNum(*pArr ++);
        }
        else
        {
            return;
        }
    }

    #endif
}
*/

void UartSendFloat(float dat)
{
    u8 xdata str = 0;
    
    sprintf(&str, "%.03f", dat);
    UartSendString(&str);
    UartSendString(" ");
}

void UartSendDebugData(u8 dat, UART_OPT_MODE mode)
{
#if defined(USE_UART_SEND_DEBUG_DATA)

    if (mode == HEX)
    {
        UartSendHexNum(dat);
    }
    else if (mode == DEC)
    {
        UartSendDecNum(dat);
    }

#endif /* #if defined(USE_UART_SEND_DEBUG_DATA) */
}

void UartSendDebugInfo(u8 *str)
{
#if  defined(USE_UART_SEND_DEBUG_INFO)

    UartSendString(str);

#endif /* #if defined(USE_UART_SEND_DEBUG_INFO) */
}

/*----------------------------
��ʼ������
----------------------------*/
/*
void Uart_Init(void)        //9600bps@24.000MHz
{
#ifdef UART_ENABLE

    SCON = 0x50;        //8λ����,�ɱ䲨����
    AUXR |= 0x01;       //����1ѡ��ʱ��2Ϊ�����ʷ�����
    AUXR |= 0x04;       //��ʱ��2ʱ��ΪFosc,��1T
    T2L = 0x8F;     //�趨��ʱ��ֵ
    T2H = 0xFD;     //�趨��ʱ��ֵ
    AUXR |= 0x10;       //������ʱ��2

    UartSendString("TIMER_0 Init_Successfully :)\r\n");
    UartSendString("UART_1  Init_Successfully :)\r\n");

#endif
}
*/

/*
void Uart_Init(void)        //115200bps@24.000MHz
{
#if defined(UART_ENABLE)

    SCON = 0x50;        //8λ����,�ɱ䲨����
    AUXR |= 0x01;       //����1ѡ��ʱ��2Ϊ�����ʷ�����
    AUXR |= 0x04;       //��ʱ��2ʱ��ΪFosc,��1T
    T2L = 0xCC;         //�趨��ʱ��ֵ
    T2H = 0xFF;         //�趨��ʱ��ֵ
    AUXR |= 0x10;       //������ʱ��2

    UartSendDebugInfo("TIMER_0 Init_Successfully :)\r\n");
    UartSendDebugInfo("UART_1  Init_Successfully :)\r\n");

#endif
}
*/

/*
void Uart_Init(void)     //9600bps@11.0592MHz
{

    SCON = 0x50;        //8bit and variable baudrate
    AUXR |= 0x40;       //Timer1's clock is Fosc (1T)
    AUXR &= 0xFE;       //Use Timer1 as baudrate generator
    TMOD &= 0x0F;       //Set Timer1 as 16-bit auto reload mode
    TL1 = 0xE0;     //Initial timer value
    TH1 = 0xFE;     //Initial timer value
    ET1 = 0;        //Disable Timer1 interrupt
    TR1 = 1;        //Timer1 running
}
*/

void Uart_Init(void)     //9600bps@11.0592MHz
{
    #if defined(UART_ENABLE)
    
    PCON &= 0x7F;       //Baudrate no doubled
    SCON = 0x50;        //8bit and variable baudrate
    AUXR |= 0x40;       //Timer1's clock is Fosc (1T)
    AUXR &= 0xFE;       //Use Timer1 as baudrate generator
    TMOD &= 0x0F;       //Mask Timer1 mode bit
    TMOD |= 0x20;       //Set Timer1 as 8-bit auto reload mode
    TL1 = 0xDC;     //Initial timer value
    TH1 = 0xDC;     //Set reload value
    ET1 = 0;        //Disable Timer1 interrupt
    TR1 = 1;        //Timer1 running

    UartSendDebugInfo("TIMER_0 Init_Successfully :)\r\n");
    UartSendDebugInfo("UART_1  Init_Successfully :)\r\n");

#endif
}

