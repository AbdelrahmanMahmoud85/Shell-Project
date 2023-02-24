#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void Uart_init(long BaudRate);
char UART_recieve_byte(void);
void UART_recieve_string( char *ptr);

#endif /* UART_H_ */
