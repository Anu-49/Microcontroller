/* 
 * File:   uart.h
 */

#ifndef UART_H
#define	UART_H

#define FOSC                20000000

//FUNCTION TO INITIALIZE UART BLOCK
void init_uart(unsigned long baud);
//FUNCTION TO SEND CHARACTER
void putchar(unsigned char data);
//FUNCTION TO SEND STRING
void puts(const char *s);

#endif	/* UART_H */