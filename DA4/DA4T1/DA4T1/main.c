#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

void wait(unsigned int us)
//subroutine to cause a time delay relative to the frequency
{
	unsigned char n;
	n=us/2;
	while (n!=0)
	{
		n--;
		_delay_ms(1);
	}
}

int main()
{
	DDRB=0xFF;		//set PORTB as output
	DDRD=0xFF;		//set PORTD as output
	TCCR0A=0x21;		//Set OCR0B as I/O
	TCCR0B=0x09;		//Set OCR0A as TOP, prescaler=1
	TCCR1A=0x23;		//Set OCR1B as I/O, OCR1A as top
	TCCR1B=0x11;		//Set OCR1A as top, prescaler=1
	TCCR2A=0x21;		//Set OCR2B as I/O, OCR2A as top
	TCCR2B=0x09;		//Prescaler=1
	
	while (1)
	//loop forever
	{
		for (unsigned int a=25; a<=230; a+=25)
		//for loop to control 10%-90% frequency for green led
		{
			OCR0A=a;	//sets OCR0A as the top value, thus changing freqeuncy
			for(unsigned int b=25; b<=230; b+=25)
			//for loop to control 10%-90% frequency for blue led
			{
				OCR1A=b;
				OCR1AH=b>>8;	//sets OCR1A as the top value, thus changing frequency
				for (unsigned int c=25; c<=230; c+=25)
				//for loop to control 10%-90% frequency for red led
				{
					OCR2A=c; //sets OCR2A as the top value, thus changing frequency
					for(unsigned int i=0; i<=a; i+=1)
					//change duty cycle of green led at frequency a
					{
						OCR0B=i; //sets OCR0B of timer2 for phase correct pwm
						for(unsigned char j=0; j<=b; j+=1)
						//change duty cycle of blue led at frequency b
						{
							OCR1B=j; //sets OCR2B of timer1 for phase correct pwm
							for(unsigned char k=0; k<=c; k+=1)
							//change duty cycle of red led at frequency c
							{
								OCR2B=k; //sets OCR2B of timer2 for phase correct pwm
								wait(c); //delay to allow user to see colors/changes
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
