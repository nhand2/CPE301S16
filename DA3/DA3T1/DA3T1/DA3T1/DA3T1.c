#define F_CPU 8000000UL
#include <avr/io.h> //standard AVR header
#include <util/delay.h> //delay header
#include <avr/interrupt.h>

void usart_init (void)
{
	UCSR0B = (1<<TXEN0); //enable transmitt only
	UCSR0C = ((1<<UCSZ01)|(1<<UCSZ00));		//removed (1<<UMSEL00) because it sets in synchronous mode, we are using asynchronous
	UBRR0H = 0x00; //high value of baud rate
	UBRR0L = 0x33; //baud rate of 9600
}


volatile unsigned int ADCvalue; //adc value buffer
void init_328(void)
//subroutine to initialize all counter/adc used in the application
{
	ADMUX|=(1<<REFS0);	//use Vcc as ref (0.1uF cap  attached)
	ADCSRA|=(1<<ADSC)|(1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE);
	//start conversion; enable adc; enable interrupts; select 128 prescaler;
	//auto trigger enable; ADC0 used
	ADCSRB=0;
	//adc in free running mode
}

void usart_send(unsigned char ch)
{
	while (!(UCSR0A & (1<<UDRE0))); //wait here until the buffer done writing
	UDR0=ch; //set ch to UDR0 buffer to send to terminal
}


ISR (ADC_vect)
{
	char temperature[2] = "00"; //temperature string array
	char temp; //temp place holder
	char header[13]= "Temperature: "; //header for temperature
	ADCvalue=ADC; //reads all 16-bits of ADCH:ADCL
	ADCvalue=(ADCvalue)*(500.0/1024.0); //convert the read ADCvalue to temperature
	//500.0=>(Vref * 100)=>(5V * 100)
	//divide by 1024, the max for the ADC values (0-1024)
	temp=(ADCvalue/10); //divide by 10 to get the 10's place digit
	temperature[0]=temp+48; //add ascii '0' to display on LCD
	temp=(ADCvalue%10); //modulo by 10 to get 1's place digit
	temperature[1]=temp+ 48; //add ascii '0' to get display on LCD
	for (int i; i<13; i++)
		usart_send(header[i]); //loop to send the header "Temperature: "
	usart_send(temperature[0]); //sends 10s space of temperatue
	usart_send(temperature[1]); //sends 1 space of temperature
	usart_send('°'); //sends degree sign
	usart_send('F'); //sends F
	usart_send('\n'); //send line feed
	_delay_ms(1000); //wait 1 seconds before retreiving data again
}

int main(void)
{
	usart_init(); //initialize usart
	init_328();	//initialize the adc
	sei(); //enable interrupts
	while(1);

return 0;
}