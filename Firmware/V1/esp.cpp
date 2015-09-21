/*
 * SitUp - ESP8266 WiFi functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#include "esp.h"
#include "serial.h"
#include <avr/io.h>
#include <util/delay.h>
#include <asf.h>
#include <stdlib.h>
#include <string.h>
#include "secret.h"

uart esp_serial(&USARTC0, 115200);
ISR (USARTC0_RXC_vect) {esp_serial.rxInterrupt();}
ISR (USARTC0_DRE_vect) {esp_serial.txInterrupt();}

extern uart debug;

char esp_rx_buf[256];
uint16_t esp_rx_buf_ptr = 0;
char ip_string[16];
char cmd[1024];
char csv_data_output[256];
uint16_t global_wifi_attempts = 0;

//Enable the ESP
void esp_power_on(void) {
	ioport_set_pin_level(ESP_ENABLE, true); //enable the ESP
}

//Disable the ESP
void esp_power_off(void) {
	ioport_set_pin_level(ESP_ENABLE, false); //enable the ESP
}

void esp_init(void) {
	ioport_set_pin_dir(ESP_ENABLE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(ESP_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ESP_RESET, true); //dont reset the esp
	esp_power_off();
}

//dumps the data in the esp buffer to the debug serial
void esp_dump(void) {
	debug.sendString("\r ESP RX:");
	while (esp_serial.dataAvailable()) {
		debug.sendChar(esp_serial.getChar());
	}
	debug.sendString("\r");
}

void esp_clear_usart_buff(void) {
	while (esp_serial.dataAvailable())
		esp_serial.getChar();
}

//copy data from esp to the working buffer, make sure do to this after waiting for a while
void esp_cpy(void) {
	uint16_t esp_rx_buf_ptr = 0;
	while (esp_serial.dataAvailable()) {
		esp_rx_buf[esp_rx_buf_ptr] = esp_serial.getChar();
		esp_rx_buf_ptr++;
	}
	esp_rx_buf[esp_rx_buf_ptr] = '\0'; //make sure to null terminate string
	//debug.sendString("\rESP RX buff: ");
	//debug.sendString(esp_rx_buf);
}

uint8_t get_IP(void) {//should implement a zero time delay, wait for something like 4 x . in received, then check for ip
	//dump the data
	esp_serial.flush();
	esp_clear_usart_buff();
	debug.sendString("\n\rget ip\n\r");
	//check if IP is acquired
	esp_serial.sendString("AT+CIFSR\r\n"); //so have to check IP assigned
	_delay_ms(2000); //wait for the rest of the data to come in.
	esp_cpy();
	char *find_ptr = strstr(esp_rx_buf, IP_BASE_EXP);
	if (find_ptr != NULL) { //simple check, look for 192.168 in ip
		debug.sendStringPgm(PSTR(" OK\r\n"));
		//strcpy()
		for (uint8_t i = 0; i < 15; i++) {
			ip_string[i] = *find_ptr;
			find_ptr++;
		}
		ip_string[15] = '\0';
		return 1;
	} else
		return 0;
}

//upload the data to emoncms
void upload_electrobob_data(void) {
	esp_serial.flush();
	debug.sendString("\rAttempt to upload led_logger_data\r");
	cmd[0] = '\0';
	strcat(cmd, "AT+CIPSTART=\"TCP\",\"");
	strcat(cmd, ELECTROBOB_IP);
	strcat(cmd, "\",80\r\n");
	esp_serial.sendString(cmd);
	//wait for connection
	while (esp_serial.dataAvailable() < 3)
		;
	_delay_ms(1000);

	//construct command
	cmd[0] = '\0';
	strcat(cmd, "GET ");
	strcat(cmd, API_LINK);
	strcat(cmd, csv_data_output);
	strcat(cmd, "&apikey=");
	strcat(cmd, API_KEY);
	strcat(cmd, " HTTP/1.0\r\nHost: electrobob.com\r\n\r\n\r\n");

	debug.sendString("\n\r command string: ");
	debug.sendString(cmd);
	//send command and size
	esp_serial.sendStringPgm(PSTR("AT+CIPSEND="));
	esp_serial.sendInt(strlen(cmd));
	esp_serial.sendString("\r\n");

	//wait for prompt
	while (esp_serial.dataAvailable() < 2)
		;
	_delay_ms(100);
	esp_serial.sendString(cmd);

	//wait for data
	while (esp_serial.dataAvailable() < 3)
		;
	_delay_ms(1000);
}

//connect to wifi with maximum attempts number,  infinite for 0.
//returns 1 for succesful conection, 0 for failed
//resets device before attempting wifi
uint8_t connectWiFi(uint16_t wifi_conn_attempts) {
	uint8_t attempts = 1;
	while ((attempts <= wifi_conn_attempts) || (wifi_conn_attempts == 0)) {

		debug.sendStringPgm(PSTR("\n\rConnect to wifi attempt "));
		debug.sendInt(attempts);
		attempts++;
		global_wifi_attempts++;
		esp_clear_usart_buff();
		esp_serial.flush();

		esp_clear_usart_buff();
		esp_serial.sendString("AT+CWMODE=1\r\n");
		while (esp_serial.dataAvailable() < 5)
			;
		_delay_ms(1000);

		esp_clear_usart_buff();
		esp_serial.flush();
		esp_serial.sendString("AT+CWJAP=\"");
		esp_serial.sendString(SSID);
		esp_serial.sendString("\",\"");
		esp_serial.sendString(SSID_PASS);
		esp_serial.sendString("\"\r\n");
		//SOB returns OK even if wifi does not exist, cannot use that for monitoring
		//esp_wait_for_data("OK", 1000);
		while (esp_serial.dataAvailable() < 5)
			;
		_delay_ms(1000);
		_delay_ms(5000);
		//check if IP is aquired
		if (get_IP()) {
			debug.sendStringPgm(PSTR(" OK\r\n"));
			debug.sendString("\r\nIP: ");
			debug.sendString(ip_string);
			return 1;
		} else {
			debug.sendStringPgm(PSTR(" Failed! Buffer dump: "));
			debug.sendStringPgm(esp_rx_buf);
			_delay_ms(5000); //wait 5 seconds before retry
		}
	}
	return 0;
}
