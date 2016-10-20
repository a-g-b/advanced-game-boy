#ifndef __UART_H__
#define __UART_H__

    typedef enum{
        DEC,
        HEX,
    } UART_OPT_MODE;

    void Uart_Init();
    void UartSendString(u8 *str);
    void UartSendDecNum(u8 dat);
    void UartSendHexNum(u8 dat);
    void UartSendArray(u16 size, UART_OPT_MODE mode, u8 *pArr);
    void UartSendDebugData(u8 dat, UART_OPT_MODE mode);
    void UartSendDebugInfo(u8 *str);
        
#endif
