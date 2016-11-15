#include "Config.h"
#include "Uart.h"

/*----------------------------
串口发送数据
----------------------------*/
static void UartSendData(unsigned char dat)
{
#if defined(UART_ENABLE)

    SBUF = dat;                     //发送当前数据
    while (!TI);                    //等待数据发送完成
    TI = 0;                         //清除发送标志

#endif
}

/*----------------------------
串口发送字符串
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
串口以十六进制形式发送输入参数的ascii
默认发送前缀"0x" 后缀" "
----------------------------*/

void UartSendHexNum(u8 dat)     // Max Send Num : 0xFF
{
#if defined(UART_ENABLE)

    u8 xdata Hnum, Lnum;

    Hnum = dat / 16;
    Lnum = dat % 16;

    UartSendString("0x");   // 前缀"0x"

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

    UartSendString(" ");    // 后缀" "

#endif
}

/*----------------------------
串口以十进制形式发送输入参数的ascii
    ps : 输入参数是几位数就发几位的ASCII
        E.G. : 输入参数为 100， 串口输出 '1' '0' '0' ' ' 的ASCII (最后一位为输出一个空格)
               输入参数为 15，  串口输出 '1' '5' ' ' 的ASCII (最后一位为输出一个空格)
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
* 函 数 名: void UartSendArray(u16 size, bit mode, u8 * pArr)
* 功能描述: 串口以"ASCII"形式输出输入数组的数据
* 输入参数:
            size :  输出数组长度
            mode :  0 为十进制形式输出，
                    1 为十六进制形式输出
            *pArr : 指向数组的指针
* 返 回 值: NULL
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
初始化串口
----------------------------*/
/*
void Uart_Init(void)        //9600bps@24.000MHz
{
#ifdef UART_ENABLE

    SCON = 0x50;        //8位数据,可变波特率
    AUXR |= 0x01;       //串口1选择定时器2为波特率发生器
    AUXR |= 0x04;       //定时器2时钟为Fosc,即1T
    T2L = 0x8F;     //设定定时初值
    T2H = 0xFD;     //设定定时初值
    AUXR |= 0x10;       //启动定时器2

    UartSendString("TIMER_0 Init_Successfully :)\r\n");
    UartSendString("UART_1  Init_Successfully :)\r\n");

#endif
}
*/

/*
void Uart_Init(void)        //115200bps@24.000MHz
{
#if defined(UART_ENABLE)

    SCON = 0x50;        //8位数据,可变波特率
    AUXR |= 0x01;       //串口1选择定时器2为波特率发生器
    AUXR |= 0x04;       //定时器2时钟为Fosc,即1T
    T2L = 0xCC;         //设定定时初值
    T2H = 0xFF;         //设定定时初值
    AUXR |= 0x10;       //启动定时器2

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

