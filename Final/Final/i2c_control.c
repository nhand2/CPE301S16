#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/twi.h>
#include "i2c_control.h"

#define F_SCL 10000UL //CLOCK for TWI
#define PRESCALAR 1 //prescalar
#define TWBAUD (((F_CPU/F_SCL)-16)/2) //set the baud rate of the TWI

void i2c_init(void)
//This function initialize the TWI interface
{
	TWSR = 0; //set status register to 0
	TWBR = (uint8_t) TWBAUD; //set TWBR with calculated baud rate
}

uint8_t i2c_start(uint8_t addr)
{
	uint8_t status; //variable to hold status
	
	//TWCR = 0; //resets the control register
	//set TWI interrupts, set start bit, enable 2-wire
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);	
	//wait for end of trasmit
	//2-wire interrupt = 0
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) != TW_START)
	//if TWSR is not the same as start, error
		return 1;
	
	//load slave address	
	TWDR = addr;
	//start trasmit
	TWCR = (1<<TWINT) | (1<<TWEN);
	//wait for trasmit to finish
	while (!(TWCR & (1<<TWINT)));
	
	//check for acknowledge bit
	status = TW_STATUS & 0xF8;
	if ((status != TW_MT_SLA_ACK) && (status != TW_MR_SLA_ACK))
	//if status is not acknowledge bit, send an error
		return 1;
		
	return 0;
}

uint8_t i2c_write(uint8_t data)
{
	TWDR = data;  //load data into TWDR
	TWCR = (1<<TWINT) | (1<<TWEN); //start transmit
	while (!(TWCR & (1<<TWINT))); //wait for transmit to finish
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
	//if master transmit is not acknowledge, send error
		return 1;
	return 0;
}

uint8_t i2c_read_ack(void)
{
	//start 2-wire and acknowledge data after finished
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	//wait for transmit to finish
	while (!(TWCR & (1<<TWINT)));
	
	return TWDR; //return data received
}

uint8_t i2c_read_nack(void)
{
	//start receive w/o acknowledge bit
	TWCR = (1<<TWINT) | (1<<TWEN);
	//wait for transmit end
	while (!(TWCR & (1<<TWINT)));
	//return received data
	return TWDR;
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //stop trasmit
	while(TWCR & (1<<TWSTO));
}