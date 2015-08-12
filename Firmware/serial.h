/*
 * SitUp - Serial port functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#ifndef INCLUDE_SERIAL
#define INCLUDE_SERIAL

#include "circular_buffer.h"
#include <avr/pgmspace.h>

#undef PSTR
#define PSTR(s) (__extension__({static const char __c[] __attribute__((section (".progmem.data"))) = (s); &__c[0];}))


class uart {
public:
    uart(USART_t* usart, const uint32_t baud);

    void sendChar(const char c);
    void sendString(const char *str);
    void sendStringPgm(const char *str);
    uint8_t sendStringNonBlocking(const char *str);
    uint8_t sendStringPgmNonBlocking(const char *str);


    void sendInt (const int8_t i);
    void sendInt (const int16_t i);
    void sendInt (const int32_t i);
    void sendInt (const uint8_t i);
    void sendInt (const uint16_t i);
    void sendInt (const uint32_t i);

    void sendHex (const uint8_t i);
    void sendHex (const uint16_t i);
    void sendHex (const uint32_t i);
    void sendHex (const int8_t i);
    void sendHex (const int16_t i);
    void sendHex (const int32_t i);

    char getChar();
    uint8_t dataAvailable();
    void flush();
    uint8_t setBaudRate(const uint32_t baud);


    void rxInterrupt()
    {
        rxBuffer.write(reg->DATA);
    }

    void txInterrupt()
    {
        if (txBuffer.isEmpty())
            reg->CTRLA = USART_RXCINTLVL_HI_gc | USART_DREINTLVL_OFF_gc;
        else
            reg->DATA = txBuffer.read();
    }

private:
    circularBuffer<char, 128> rxBuffer;
    circularBuffer<char, 64> txBuffer;

    USART_t * const reg;

};


#endif
