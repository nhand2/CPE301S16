#define F_CPU 8000000UL
#include <avr/io.h> //standard AVR header
#include <util/delay.h> //delay header
#include <avr/interrupt.h>

#define LCD_DPRT PORTD //LCD DATA PORT
#define LCD_DDDR DDRD //LCD DATA DDR
#define LCD_DPIN PIND //LCD DATA PIN
#define LCD_CPRT PORTB //LCD COMMANDS PORT
#define LCD_CDDR DDRB //LCD COMMANDS DDR
#define LCD_CPIN PINB //LCD COMMANDS PIN
#define LCD_RS 0 //LCD RS
#define LCD_RW 1 //LCD RW
#define LCD_EN 2 //LCD EN

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

ISR (ADC_vect)
{
	char *temperature="00"; //temperature string array
	char temp; //temp place holder
	ADCvalue=ADC; //reads all 16-bits of ADCH:ADCL
	ADCvalue=(ADCvalue)*(500.0/1024.0); //convert the read ADCvalue to temperature
	//500.0=>(Vref * 100)=>(5V * 100)
	//divide by 1024, the max for the ADC values (0-1024)
	temp=(ADCvalue/10); //divide by 10 to get the 10's place digit
	temperature[0]=temp+48; //add ascii '0' to display on LCD
	temp=(ADCvalue%10); //modulo by 10 to get 1's place digit
	temperature[1]=temp+ 48; //add ascii '0' to get display on LCD
	lcd_gotoxy(5,2); //move to cell 5 on second half of lcd
	lcd_print(temperature); //print the converted temperature string array
	_delay_ms(1000); //wait 1 seconds before retreiving data again
}


void lcdCommanda (unsigned char cmnd)
{
	LCD_DPRT = cmnd; //send cmnd to data port
	LCD_CPRT &= ~(1<<LCD_RS); //RS = 0 for command
	LCD_CPRT &= ~(1<<LCD_RW); //RW = 0 for write
	LCD_CPRT |= (1<<LCD_EN); //EN = 1 for H-to-L pulse
	_delay_ms(150); //wait to make enable wide
	LCD_CPRT &= ~(1<<LCD_EN); //EN = 0 for H-to_L pulse
	_delay_ms(100); //wait to make enable wide
}

void lcdData(unsigned char data)
{
	LCD_DPRT = data; //send data to data port
	LCD_CPRT |= (1<<LCD_RS); //RS = 1 for data
	LCD_CPRT &= ~(1<<LCD_RW); //RW = 0 for write
	LCD_CPRT |= (1<<LCD_EN); //EN = 1 for H-to-L pulse
	_delay_ms(150); //wait to make enable wide
	LCD_CPRT &= ~(1<<LCD_EN); //EN = 0 for H-to_L pulse
	_delay_ms(100); //wait to make enable wide
}

void lcd_init()
{
	LCD_DDDR = 0xFF;
	LCD_CDDR = 0xFF;
	LCD_CPRT &=~(1<<LCD_EN); //LCD_EN = 0
	_delay_ms(2000); //wait for init
	lcdCommanda(0x38); //inti. LCD 2 line, 5x7
	lcdCommanda(0x0E); //display on, cursor on
	lcdCommanda(0x01); //clear LCD
	_delay_ms(2000); //wait
	lcdCommanda(0x06); //shift cursor right
}
//****************************************************************************
void lcd_gotoxy(unsigned char x, unsigned char y)
//choose which position on the LED to display message
//our module is 16x1, this y=>x, but on the second half
{
	unsigned char firstCharAdr[] = {0x80, 0xC0, 0x94, 0xD4};
	lcdCommanda(firstCharAdr[y-1] + x -1);
	_delay_ms(100);
}

void lcd_print(char * str)
//prints the string array, one cell at a time (ascii)
{
	unsigned char i = 0;
	while (str[i]!=0)
	{
		lcdData(str[i]);
		i++;
	}
}

int main(void)
{
	char *degree;
	
	lcd_init();	//initialize the lcd module
	lcd_gotoxy(1,1); //display first 8 characters of "temperature"
	lcd_print("Temperat");
	lcd_gotoxy(1,2); //display last 4 characters of "temperature:"
	lcd_print("ure:");
	lcd_gotoxy(7,2); //display degress symbol 2nd half, second to last cell
	degree[0]=223; //set to ASCII degree symbol
	lcd_print(degree);
	lcd_gotoxy(8,2); //display F for farenheit
	lcd_print("F");
	init_328();	//initialize the adc
	sei(); //enable interrupts
	while(1);

return 0;
}