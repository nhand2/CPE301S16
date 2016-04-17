#define F_CPU 8000000UL //XTAL = 8MHZ
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile unsigned int ADCvalue;

void delay(unsigned int d)
//delay subroutine
{
	unsigned n;
	for(n=d; n>0; n--) //as long as n>0, decrease value
		_delay_ms(1); //1 ms delay, thus n ms delay is used
}

void init_328(void)
//subroutine to initialize all counter/adc used in the application
{
	ADMUX|=(1<<REFS0);	//use Vcc as ref (0.1uF cap  attached)
	ADCSRA|=(1<<ADSC)|(1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE);
	//start conversion; enable adc; enable interrupts; select 128 prescaler;
	//auto trigger enable; ADC0 used
	ADCSRB=0; //adc in free running mode
}

ISR (ADC_vect)
{
	ADCvalue=ADC; //reads all 16-bits of ADCH:ADCL (0-1023 from pot)
	PORTB = 0x66; //output first step of stepper motor 
	delay(ADCvalue); //hold this step for the delay (the larger the slower the stepper)
	PORTB = 0xCC;  //output second step
	delay(ADCvalue); //hold for same value
	PORTB = 0x99; //output third step
	delay(ADCvalue); //hold for same value
	PORTB = 0x33; //output fourth step
	delay(ADCvalue); //stay for same value
}

int main()
{
	DDRC = 0x00; //make PORTC input pins
	DDRB = 0xFF; //make PORTB output pin
	init_328(); //initialize adc
	sei(); //enable global interrupts
	while(1)
	{
	};
	
	return 0;
}