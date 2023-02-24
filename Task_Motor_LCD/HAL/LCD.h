#ifndef LCD_H_
#define LCD_H_

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define LCD_Data_Dir DDRA // data bits to portA
#define LCD_Control_Dir DDRB //control bits to portB
#define LCD_Data_Port PORTA // data state portA
#define LCD_Control_Port PORTB // control state portB

#define RS PORTB2  //register select on pinb2
#define RW PORTB1  //read/write on pinb1
#define EN PORTB0  //enable on pinb0

void LCD_Send_Command(unsigned char x);
void LCD_Config(void);
void LCD_write_char(unsigned char x);
void LCD_Clear(void);
void LCD_write_String(char *p);




#endif /* LCD_H_ */
