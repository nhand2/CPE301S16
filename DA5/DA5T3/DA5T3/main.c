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
	TCCR1A|=(1<<COM1A1)|(1<<WGM11); //timer1 in fast pwm mode, OCR1A clear on up, set down
	TCCR1B|=(1<<WGM12)|(1<<WGM13)|(1<<CS11); //prescaler of 1, fast pwm, ICR1 as top
	ICR1=20000; //set to 20000 (50Mhz, T=20m)
}

ISR (ADC_vect)
//ADC interrupt subroutine
{
	ADCvalue=ADC;	//reads all 16-bits of ADCH:ADCL (0-1023 from pot)
	ADCvalue=(3*ADCvalue)+1150; //multiply value by 3 and add to 1150 to obtain servo duty
	OCR1A=ADCvalue; //set OCR1A as new value to create duty cycle
}

int main()
{
	DDRC = 0x00; //make PORTC input pins
	DDRB = 0xFF; //make PORTB output
	init_328(); //initizlize timer1 and adc
	sei(); //set global interrupts
	while(1)
	{
	};
	
	return 0;
}
