#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "i2c_control.h"
#include "MPU6050.h"
#include "usart.h"
#include "ESP8266.h"

char buffer[6] = "0"; //buffer to hold ASCII angle
int gyro_xout = 0; //initialize gyro_xout
int GYRO_XOUT_OFFSET = 0; //initialize GYRO offset
int GYRO_XOUT_OFFSET_SUM = 0; //initialize sum of GYRO offset


int angle = 0; //initialize angle
int gyro_rate = 0; //initialize the gyro_rate

int gyro_angle = 0; //initialize gyro_angle



void get_values(void)
{
	i2c_start(MPU6050_WRITE); //set the address of MPU6050 write register
	i2c_write(GYRO_YOUT_H); //set pointer to GYRO_XOUT_H register
	i2c_stop();
	
	i2c_start(MPU6050_READ); //set the address to READ register
	gyro_xout = ((uint8_t) i2c_read_ack())<<8; //read the xout value and acknowledge data received (continue SLAVE to MASTER transmission)
	gyro_xout |= i2c_read_nack(); //read xout_lower bits without acknowledge (stops SLAVE to MASTER transmission)
	i2c_stop(); //stop transmission
}

void calc_values(void)
//This function calculates the angle of IMU. The angle is calculated by taking the gyro's x value and
//converting from degrees/second to degrees. The gyro contains a good amount of DRIFT from outside noise,
//the accelerometer is used to attempt to offset this noise
{
	gyro_rate = (gyro_xout - GYRO_XOUT_OFFSET)/1.0131; //the gyro's rate is calculated
	gyro_angle += gyro_rate/1000; //the angle is added to the rate divided by sample rate to get the angle of the IMU
	itoa(gyro_angle, buffer, 10);
}

void gyro_calibrate(void)
//This function calibrates the GYROSCOPE by getting the value of the gyro's x-axis and adding the
//values. The values are then averaged and the OFFSET of the gyroscope is obtained
{
	for (int i=0; i<1000; i++) //loop for 1000 iterations
	{
		get_values(); //get the GYRO values by calling get_values
		
		GYRO_XOUT_OFFSET_SUM += gyro_xout; //add next gyro_xout value to offset sum
	}
	GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET_SUM/1000; //average the OFFSET to get the official offset
}

void MPU6050_init(void)
//This function initialize the MPU6050 by sending various commands to the IMU
{
	i2c_start(MPU6050_WRITE); //sets WRITE address of device
	i2c_write(PWR_MGMT_1); //sets address of power management 1
	i2c_write(0x00); //turns on the device
	i2c_stop(); //stop I2C connection
	
	i2c_start(MPU6050_WRITE);
	i2c_write(CONFIG); //set address to point to CONFIG register
	i2c_write(0x01); //write 0x01 to the register ()
	i2c_stop();
	
	i2c_start(MPU6050_WRITE);
	i2c_write(GYRO_CONFIG); //set address to point to GYRO_CONFIG register
	i2c_write(0x10); //write 0x10 to register (1000 degrees/sec range)
	i2c_stop();
	
	i2c_start(MPU6050_WRITE);
	i2c_write(ACCEL_CONFIG); //configures the accelerometer
	i2c_write(0x00); //write 0x00 to register (+/- 2g range)
	i2c_stop();
	
	i2c_start(MPU6050_WRITE);
	i2c_write(INT_ENABLE); //set pointer to address of INTERRUPTS ENABLE
	i2c_write(0x00); //write 0x00 (no interrupts)
	i2c_stop();
	
	i2c_start(MPU6050_WRITE);
	i2c_write(SIGNAL_PATH_RESET); //set pointer to address of
	i2c_write(0x00); //write 0x00 (no reset)
	i2c_stop();
}

int main()
{
	PINC = 0xFF; //set pull up resistors to ensure proper data
	
	i2c_init(); //initialize all I2C/TWI interaface registers/module
	usart_init(); //initialize usart module on ATmega328p
	MPU6050_init(); //call to initialize the MPU6050
	gyro_calibrate(); //call to calibrate the gyro to zero it at a given position

	char *SSID = "Mah_New_Internet"; //AP SSID
	uint8_t connected = 0; //variable to check if connected
	char *HTTP_POST; //character array/string to hold the generic "HTTP POST" command
	char cmd[256]; //buffer to hold the "POST" command, variable depending on the size and variable to be sent
	char cipsend[32]; //buffer to hold the cipsend command, which is variable according to # of bytes sending
	uint8_t err; //variable to hold error status
	
	
	while (1)
	{
		get_values(); //get the values from the gyro and accel
		calc_values(); //call function to calculate the values of the accel and gyro
		if (gyro_angle > 200|| gyro_angle < -400)
		{
			ESP8266_cmd("AT+RST\r\n", 1); //AT+ command to reset the ESP8266
			while (ESP8266_rec("WIFI GOT IP", NULL)); //wait here until WIFI is initialized correctly
			
			ESP8266_cmd("ATE0\r\n", 1); //disable ECHO
			ESP8266_cmd("AT+CWJAP?\r\n", 0); //check connected AP
			if (ESP8266_rec(SSID, NULL) == 0) //checks if AP is correct and set "conencted" to 1
			connected = 1;
			
			if (connected) //enter if connected to AP
			{
				ESP8266_cmd("AT+CIPMODE=0\r\n", 1); //set CIPMODE to 0
				ESP8266_cmd("AT+CIPMUX=0\r\n", 1); //set CIPMUX to 0, single port connection
				ESP8266_cmd("AT+CIPSTART=\"TCP\",\"50.23.124.68\",80\r\n", 0); //connect to Ubidots server using CIPSTART
				err = ESP8266_rec("CONNECT", NULL);	 //store results of command to err variable

				if(!err) //if the connected, enter
				{
					//the following line sets the POST command to the ESP8266 to SEND data to the server
					HTTP_POST="POST /api/v1.6/variables/572c0ff97625420e3376b14c/values HTTP/1.1\r\nX-Auth-Token: QUAq0Xx3zPAPxpxDb6JWzL8v62TJtc\r\nHost: things.ubidots.com\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{\"value\":%s}";
					snprintf(cmd, sizeof(cmd),HTTP_POST, strlen(buffer)+10, buffer); //prints the string in to a string that is variable in size but cannot exceed max sizeof(cmd)
					cmd[sizeof(cmd)-1] = '\0'; //null terminate the string
					
					snprintf(cipsend, sizeof(cipsend), "AT+CIPSEND=%d\r\n", strlen(cmd)); //prints the string with variable byte size
					cipsend[sizeof(cipsend)-1] = '\0'; //null terminate the string
					
					ESP8266_cmd(cipsend, 0); //call the send command of ESP8266
					ESP8266_rec(">", NULL); //check to see if '>' is recieved
					ESP8266_cmd(cmd, 0); //send the command printed to the cmd buffer
					err = ESP8266_rec("SEND OK", NULL); //check if "SEND OK" was issued from the ESP8266
					ESP8266_cmd("AT+CIPCLOSE\r\n", 0); //terminate the connection once done
					ESP8266_rec("OK", "ERROR"); //check the result of closing the serve connection
				}
				gyro_calibrate();
				gyro_angle = 0;
			}
		}*/
	}
	return 0;
}
