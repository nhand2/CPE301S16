#include <avr/io.h>

void sub_delay()
//delay subroutine that creates a 50% duty cycle clock w/ period of 5 seconds
{
	TCNT1=63583;				//sets counter to 63583, which takes 0.25 s to overflow
	TCCR1A=0x00;				//normal more operation
	TCCR1B=0x05;				//prescaler of 1024
	while((TIFR1&0x01)==0);		//loops until TOV1 is set
	TCCR1B=0x00;				//stops the timer
	TIFR1|=(1<<TOV1);			//clear TOV1 flag
	PORTC=PORTC ^(0x01);		//xor PORTC, PC.0 specifically to toggle a clock
}



int main(void)
{
	DDRB=0xFF;					//PORTB will be used to output the counter
	PORTB=0x00;					//Initialize the out to output LOW signal
	DDRC=0x31;					//PC.4 and 5 were used to output every 5 and 10 rising pulses due to availability on the board
	PORTC=0x00;					//Clears the PORTC to output LOW signal
	DDRD=0x00;					//PD.4 is set to be used at T0 to take input from the clock generated
	
	TCNT0=0x00;					//counter 0 that will output to PORTB
	TCCR0A=0x00;				//Setting counter to normal mode operation
	TCCR0B=0x07;				//sets counter to accept external clock (PD4)
	
	while(1)
	//while loop that will execute forever
	{
		sub_delay();			//Subroutine to cause a delay of 0.25s which will cause a 5s period for the clock
		PORTB=TCNT0;			//output counter/timer0 to PORTB (8 bit leds)
	}
	return 1;
}

