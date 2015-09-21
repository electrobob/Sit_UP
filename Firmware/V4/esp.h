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


#ifndef ESP_H_
#define ESP_H_

#include <avr/pgmspace.h>

#define ESP_ENABLE IOPORT_CREATE_PIN(PORTC, 7)
#define ESP_RESET IOPORT_CREATE_PIN(PORTC, 5)
#define ESP_GPIO_0 IOPORT_CREATE_PIN(PORTC, 4)
#define ESP_GPIO_2 IOPORT_CREATE_PIN(PORTC, 6)

void esp_dump(void);
void esp_clear_usart_buff(void);
void esp_power_on(void);
void esp_power_off(void);
void esp_cpy(void);

uint8_t get_IP(void);
void upload_electrobob_data(void);
uint8_t connectWiFi(uint16_t wifi_conn_attempts);
void esp_init(void);

#endif /* ESP_H_ */