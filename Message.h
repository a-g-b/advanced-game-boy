#ifndef __MESSAGE_H
#define __MESSAGE_H

#if defined(LED_ENABLE)

    #pragma message("'LED_ENABLE' is defined")

#endif /* LED_ENABLE */

#if defined(USE_LCD_DRAW_GRAPH)

    #pragma message("'USE_LCD_DRAW_GRAPH' is defined")
    
#endif /* USE_LCD_DRAW_GRAPH */

#if defined(UART_ENABLE)

    #pragma message("'UART_ENABLE' is defined")

#endif /* USE_UART_DEBUG */

#if  defined(USE_UART_SEND_DEBUG_DATA)

    #pragma message("'USE_UART_SEND_DEBUG_DATA' is defined")

#endif /* #if defined(USE_UART_DEBUG) */

#if  defined(USE_UART_SEND_DEBUG_INFO)

    #pragma message("'USE_UART_SEND_DEBUG_INFO' is defined")

#endif /* #if defined(USE_UART_SEND_DEBUG_INFO) */

#endif /* __OPT_MESSAGE_H */
