#include "Uart.h"
#include "usart.h"

#define TEST_UART huart3

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&TEST_UART, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&TEST_UART, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

int __io_getchar(void)
{
    uint8_t ch;
    HAL_UART_Receive(&TEST_UART, &ch, 1, HAL_MAX_DELAY);
    return ch;
}