#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include "stdio.h"
#include "usart.h"
int fputc(int ch, FILE *f);
int __io_putchar(int ch);
int __io_getchar(void);

#endif