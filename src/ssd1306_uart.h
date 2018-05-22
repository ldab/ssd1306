/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 * @file ssd1306_uart.h Uart implementation for SSD1306 library
 * @detail Define UART_INTERRUPT_ENABLE before including this header, if you
 *         want interrupt-based UART support.
 *         Define SSD1306_UART_SUPPORTED before including this header, if you
 *         want to use uart module outside ssd1306 library.
 */

#ifndef _SSD1306_UART_H_
#define _SSD1306_UART_H_

// This check is required if you want to use ssd1306_uart module outside ssd1306_library
#ifndef SSD1306_UART_SUPPORTED
#include "ssd1306_hal/io.h"
#endif

#if defined(SSD1306_UART_SUPPORTED) && defined(CONFIG_AVR_UART_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define UART_BUFFER_RX  32  // :( Still need large buffer to process USART RX bytes

extern volatile uint8_t g_uart_put_ptr;

extern volatile uint8_t g_uart_buf[];

void uart_init_internal(uint32_t baud, uint8_t interrupt);

/**
 * @brief Initializes uart module
 *
 * Initializes uart module. Depending on UART_INTERRUPT_ENABLE define,
 * module will be initialized in interrupt mode or synchronouse mode.
 * @param baud baud rate for the uart module
 * @note only 115200, 57600, 38400 and 19200 are supported.
 */
static inline void uart_init(uint32_t baud)
{
#ifdef UART_INTERRUPT_ENABLE
    uart_init_internal(baud, 1);
#else
    uart_init_internal(baud, 0);
#endif
}

/**
 *
 */
void uart_send_byte(uint8_t c);

uint8_t uart_byte_available(void);

uint8_t uart_read_byte(void);

static inline void __uart_read_byte(void)
{
    g_uart_buf[g_uart_put_ptr] = UDR0;
    g_uart_put_ptr = (g_uart_put_ptr+1) & (UART_BUFFER_RX - 1);
}


#ifdef UART_INTERRUPT_ENABLE

ISR(USART_RX_vect, ISR_BLOCK)
{
    if (bit_is_clear(UCSR0A, FE0)) // Do not perform error checks for now
    {
        __uart_read_byte();
    }
    else
    {
        volatile unsigned char data __attribute__((unused)) = UDR0;
    }
}
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif

