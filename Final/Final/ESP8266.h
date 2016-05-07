#include<avr/io.h>

uint8_t ESP8266_cmd (const char* cmd, uint8_t wait_ok);
uint8_t ESP8266_rec(const char* k_reply, const char* n_reply);