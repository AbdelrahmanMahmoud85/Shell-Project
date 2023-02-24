#include "UART.h"
void Uart_init(long BaudRate)

{
	unsigned char UBRR = (((F_CPU)/16/BaudRate)-1);
	UBRRH =(UBRR>>8);          // MSB of UBRR
	UBRRL = UBRR;              // LSB of UBRR
	UCSRB |= (1<<RXEN)|(1<<RXCIE);    // Enabling RX
	UCSRC |= (1<<UCSZ0)|(1<<UCSZ1)|(1<<URSEL); // 8-bit mode
}

char UART_recieve_byte(void){
	while((UCSRA & (1<<RXC)) == 0){ // if not ready to receive, infinite loop

	}
	return UDR;
}

void UART_recieve_string(char*ptr){
	for(int i=0; i<8; i++){
		ptr[i] = UART_recieve_byte();
	}
}
