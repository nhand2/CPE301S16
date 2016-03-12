#include <avr/io.h>
#include <avr/interrupt.h>

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

ISR (INT0_vect)
//interrupt routine that toggles PC.4 every 5 rising clock edges
{
	PORTC^=(1<<PORTC4);
}

ISR (INT1_vect)
//interrupt routine that toggles PC.5 every 10 rising clock edges
{
	PORTC^=(1<<PORTC5);
}

int main(void)
{
	unsigned char toggle1=0;	//toggle counter for 5 rising clock edges
	unsigned char toggle2=0;	//toggle counter for 10 rising clock edges
	DDRB=0xFF;					//PORTB will be used to output the counter
	PORTB=0x00;					//Initialize the out to output LOW signal
	DDRC=0x31;					//PC.4 and 5 were used to output every 5 and 10 rising pulses due to availability on the board
	PORTC=0x00;					//Clears the PORTC to output LOW signal
	DDRD=0x0C;					//PD.4 is set to be used at T0 to take input from the clock generated
	
	TCNT0=0x00;					//counter 0 that will output to PORTB
	TCCR0A=0x00;				//Setting counter to normal mode operation
	TCCR0B=0x07;				//sets counter to accept external clock (PD4)
	
//This set of instructions sets the interrupt registers
	EICRA=0x0F;					//sets interrupts INT0 and INT1 to trigger on rising edge
	EIMSK=0x03;					//enables INT0 and INT1 to be used as outputs
	sei();						//enables all global interrupts
	
	while(1)
	//while loop that will execute forever	
	{
		sub_delay();			//Subroutine to cause a delay of 0.25s which will cause a 5s period for the clock
		PORTB=TCNT0;
		if ((PORTC&(0x01))==1)
		//checks if clock rising edge to increment toggle counters
		{
			++toggle1;
			++toggle2;
		}
		if (toggle1==5)
		//if toggle1=5, output 1 to PD.2 to trigger INT0
		{
			PORTD^=(1<<PORTD2);	//set PD.2
			toggle1=0;			//reset toggle1
			PORTD^=(1<<PORTD2);	//clear PD.2
		}
		if (toggle2==10)
		//if toggle2=10, output 1 to PD.3 to trigger INT0
		{
			PORTD^=(1<<PORTD3);	//set PD.3
			toggle2=0;			//reset toggle2
			PORTD^=(1<<PORTD3);	//clear PD.3
		}
	}
	return 1;
}

