
#include <avr/io.h>

#include "../HAL/LCD.h"
#include "../MCAL/UART.h"
#include "util/delay.h"
#include <avr/interrupt.h>
unsigned char tick = 0;  // Timer2 1 second flag
unsigned char check = 0; // Input flag

typedef struct vehiclestate{
char dcmotor_direction, stepper_angle, stepper_direction,dcmotor_speed;
}vehiclestate;

unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;


struct vehiclestate v;

char arr[8]; // receiving array from uart
char dcm[4]; // stores the first 3 characters of the received array
char stp[3]; // stores the fifth and sixth character of the received array

void inputread(){
	LCD_Clear();
		if(arr[7]!= 'E' || arr[0] > '1' || arr[0]<'0'|| (arr[3] != 'F' && arr[3] != 'B') || arr[4] > '4' || (arr[4] == '4' && arr[5] > '5')|| (arr[6] != 'R' && arr[6] != 'L') ){
			LCD_write_String("Wrong Input!");
			_delay_ms(500);
			LCD_Clear();// if wrong input, print error and ignore(return)
			return;}
		for(int i = 0; i<3; i++){ // stores the first 3 characters of the received array
			dcm[i] = arr[i];
		}
		dcm[3] = '\0'; // converting the array into a string
		for(int i = 0; i<2; i++){ // stores the fifth and sixth character of the received array
					stp[i] = arr[i+4];
				}
				stp[2] = '\0'; // converting the array into a string
		v.dcmotor_speed = atoi(dcm); //converting the array into an integer
		v.dcmotor_direction = arr[3];
		v.stepper_angle = atoi(stp); //converting the array into an integer
		v.stepper_direction = arr[6];LCD_Clear();}


void PWM_Timer0_Init()
{
	TCNT0 = 0; //start counting from zero
	DDRB  = DDRB | (1<<PB3); // pin 3 in port 3 is output
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01) | (1<<CS00); //Fast PWM mode - Clear on compare
	// Pre-scaler = 64 for frequency of approximately 500 Hz
}

ISR(USART_RXC_vect){
	LCD_Clear();
	LCD_write_String("Please enter correct input");
	UART_recieve_string(arr);
	check = 1; // set the input flag
}
ISR(TIMER2_COMP_vect)
{
	tick++;

	if(tick == 31) // if 1 second has passed
	{
		seconds++;
		if(seconds == 60){
			seconds = 0;
			minutes++;
			if(minutes==60){
				hours++;
				minutes =0;
			if(hours > 12){
				hours=0;
			}
			}
		}
		tick = 0;}
}
void Timer2_Init_CTC_Mode()
{
	TCNT2 = 0;
	OCR2  = 250; // interrupt at 250 ms
	TIMSK |= (1<<OCIE2); // timer2 interrupt flag on
	TCCR2 = (1<<FOC2) | (1<<WGM21) | (1<<CS22) | (1<<CS21)| (1<<CS20); // non-PWM mode - CTC mode - Pre-scale = 1024
}

void printstatus(void){
	LCD_Clear();
	LCD_write_String("  Speed: ");
	LCD_write_String(dcm);
	LCD_write_String("% ");
	LCD_write_char(v.dcmotor_direction);
	LCD_write_String("  Direction:");
	if(v.stepper_direction == 'L'){
		LCD_write_String(" -");
	}
	LCD_write_String(stp);

}

void printtime(){
	char s[3]; char m[3]; char h[3];
	itoa(seconds,s,10); itoa(minutes,m,10); itoa(hours,h,10); // converting time characters into arrays
	s[2] = '\0'; m[2]= '\0'; h[2]= '\0'; // converting arrays to strings
	LCD_Send_Command(0xC0); //start from the second line
		LCD_write_String("  Time: ");
		LCD_write_String(h);
		LCD_write_String(":");
		LCD_write_String(m);
	    LCD_write_String(":");
	    LCD_write_String(s);
	    if(hours == 12){
	    LCD_write_String("  PM");
	    }
	    else{
	    	LCD_write_String("  AM");
	    }
}
int main()
{
	sei(); // setting global interrupt flag on
	Timer2_Init_CTC_Mode();
	LCD_Config();
	Uart_init(9600);
	DDRC |= 0x3F; //Pins from 0 to 5 are output
	PORTC &= 0xC0; // Reset pins from 0 to 5
	PWM_Timer0_Init();
	LCD_write_String("  Welcome to the Main Menu");
	unsigned char motor_steps[9] = {6,2,3,1,9,8,12,4,6}; // half step stepper motor
	unsigned char i = 0; // counter for controlling the stepper motor
	while(1)
	{   if(check){ // if input is read from uart
		inputread(); // check if input is right then store, otherwise ignore
		OCR0 = v.dcmotor_speed * 255/100; // set the motor's speed percentage
		printstatus(); // print the status of the lcd
		check = 0; // clear the input flag
	}
	printtime(); //continuously print the time
		if(v.dcmotor_direction == 'B')
		{PORTC &= (~(1<<PC0));
		 PORTC |= (1<<PC1);} // 0x10 indicates backward motion(cw)

		if(v.dcmotor_direction == 'F')
				{PORTC |= (1<<PC0);
				PORTC &= (~(1<<PC1));} //0x01 indicates forward motion

		if(v.stepper_direction == 'R' && v.stepper_angle > 0){
			v.stepper_angle--; //decrement until the angle is 0
			PORTC = ((motor_steps[i+1] & 0x0F))<<2 | (PORTC & 0xC3); //take the steps from the array and shift left by 2 then OR with a properly set PORTC
			_delay_ms(20); // stepper motor delay
			if(i!=7){
		         	i++;}
			else{
				i = 0;} //reset the counter
		}
		if(v.stepper_direction == 'L' && v.stepper_angle > 0){
			v.stepper_angle--;
			PORTC = ((motor_steps[7-i] & 0x0F)<<2) | (PORTC & 0xC3);
			_delay_ms(20);
			if(i!=7){
					i++;
								}
		    else{
					i = 0;}}

	}
}
