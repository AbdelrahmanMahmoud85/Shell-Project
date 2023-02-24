#include "LCD.h"

void LCD_Send_Command(unsigned char x){
	LCD_Data_Port = x;
	LCD_Control_Port &= ~(1<<RS); //rs = 0
	LCD_Control_Port &= ~(1<<RW); //write operation(rw=0)
	LCD_Control_Port |= (1<<EN);  //enable = 1
	_delay_ms(10);
	LCD_Control_Port &= ~(1<<EN); // enable = 0
	_delay_ms(10);
}

void LCD_Config(void){
	LCD_Control_Dir |= (0x07); // lcd command port is output
	LCD_Data_Dir = 0xFF;  // lcd data port as output
	_delay_ms(100); // delay until the lcd powers on

	LCD_Send_Command (0x38);  // lcd in 8-bit mode
	LCD_Send_Command (0x0C);  // display cursor is off
	LCD_Send_Command (0x06);  // auto increment cursor
	LCD_Send_Command (0x01);  // clears display on lcd
	LCD_Send_Command (0x80);  // cursor at the starting point
	
}

void LCD_write_char(unsigned char y){
	LCD_Data_Port = y;
	LCD_Control_Port |= (1<<RS);  //rs = 1 (data)
	LCD_Control_Port &= ~(1<<RW); //write operation
	LCD_Control_Port |= (1<<EN);  //enable = 1
	_delay_us(1);
	LCD_Control_Port &= ~(1<<EN); // enable = 0;
	_delay_ms(1);
}

void LCD_write_String(char *p){
	int i;
	for(i=0; p[i] != '\0';i++){ //loop that receives a char to pointer to string
		LCD_write_char(p[i]);
	}
}

void LCD_Clear(void){
	LCD_Send_Command (0x01); //clear display
	LCD_Send_Command (0x80); //cursor reset
}

