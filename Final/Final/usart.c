#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include "usart.h"

void usart_init(void)
//This function intializes the USART module on the ATmega328p
{
	UCSR0B |= (1<<TXEN0)|(1<<RXEN0); //enable transmit only
	UCSR0C |= ((1<<USBS0)|(3<<UCSZ00));	//removed (1<<UMSEL00) because it sets in synchronous mode, we are using asynchronous
	UBRR0H = 0x00; //high value of baud rate
	UBRR0L = 0x19; //baud rate of 19200
}

void usart_send(unsigned char data)
//This function takes a character datatype and loads it in to the register UDR0 for tramission
{
	while (!(UCSR0A & (1<<UDRE0))); //wait here until buffer is empty
	UDR0 = data; //put DATA into buffer
}

void usart_sends(char *s)
//This function takes in a string/character array and sends it one byte at a time to transmit a string
{
	while(*s) //while there are still characters in the string
	{
		usart_send(*s); //call usart_send to send a single byte
		s++; //advance the string position
	}
}

unsigned int usart_receive(void)
//This function receives data from the RX0 pin and assigns that byte to the data buffer
{
	unsigned char data=0; //clear data of previous information
	while (!(UCSR0A & (1<<RXC0))); //wait until receive is complete
	data=UDR0; //assign data to UDR0
	
	return data; //return the byte of data
}