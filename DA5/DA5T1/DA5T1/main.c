#define F_CPU 8000000UL //XTAL = 8MHZ
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned int ADCvalue;
void init_328(void)
//subroutine to initialize all counter/adc used in the application
{
	ADMUX|=(1<<REFS0);	//use Vcc as ref (0.1uF cap  attached)
	ADCSRA|=(1<<ADSC)|(1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE); 
	//start conversion; enable adc; enable interrupts; select 128 prescaler;
	//auto trigger enable; ADC0 used
	ADCSRB=0;
	//adc in free running mode
	TCCR0A|=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00); //use timer0 in fast PWM mode
	//set OC0A on bottom and clear on up counting
	TCCR0B|=(1<<CS00); //prescaler of 1 selected
}

ISR (ADC_vect)
{
	ADCvalue=ADC;	//reads all 16-bits of ADCH:ADCL (0-1023 from pot)
	ADCvalue=ADCvalue/4; //divide ADC value by 4 to get 0-225 value
	OCR0A=ADCvalue; //set this value to the OCR0A register
	_delay_ms(10); //delay to allow OCR0A to hold
}

int main()
{
	DDRC = 0x00; //make PORTC input pins
	DDRD = 0xFF; //make PORTB output pin
	init_328(); //initialize 328 (ADC/timers)
	sei(); //enable global interrupts
	while(1)
	{
	};
	
	return 0;
}