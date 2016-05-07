#include <avr/io.h>
#include "usart.h"
#include <string.h>
#include <stdlib.h>

static char reply[64];
static uint8_t reply_pos; //pointer for reply from ESP8266

uint8_t ESP8266_rec(const char* k_reply, const char* n_reply)
//This function works to receive a reply from the the ESP8266
//Two strings are accepted, okay and not_okay. Okay will send 0, meaning success
//Not okay will send 1, which means failed
{
	reply_pos = 0; //position of the reply
	for (;;) //similiar to while loop
	{
		uint16_t rx = usart_receive(); //save receive in rx
		if ((rx>>8) == 0) //if rx shifted 8 bits right == 0 do this
		{
			reply[reply_pos++] = rx & 0xFF; //determine what character is in rx
			reply[reply_pos] = '\0'; //NULL terminator
			if (reply_pos == sizeof(reply)-1) //reset position if end of string
			{
				reply_pos=0;
			}
			if (k_reply && strstr(reply, k_reply)) //if okay matched, return 0
			return 0;
			if (n_reply && strstr(reply, n_reply)) //if not okay matched, return 1
			{
				return 1;
			}
		}
	}
}

uint8_t ESP8266_cmd (char* cmd, uint8_t wait_ok)
//This function takes in a command for the ESP8266 and will wait until an "OK" is received
{
	usart_sends(cmd); //send the command via USART
	
	if(wait_ok) //if wait_ok is 1, we expect and "OK" reply
	{
		return ESP8266_rec("OK\r\n", NULL); //return if OK was received
	}
	
	return 0;
}