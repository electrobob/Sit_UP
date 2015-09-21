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


#include <avr/io.h>
#include <stdlib.h>

#include "serial.h"




uart::uart(USART_t* usart, const uint32_t baud) : reg(usart)
{
    //Make usart's tx pin output (supports xmega a4 series)
    if (usart == &USARTC0)
    {
        PORTC.OUTSET = PIN3_bm;
        PORTC.DIRSET = PIN3_bm;
    }

    else if (usart == &USARTD0)
    {
        PORTD.OUTSET = PIN3_bm;
        PORTD.DIRSET = PIN3_bm;
    }


    setBaudRate(baud);

	// Set USART Format to 8bit, no parity, 1 stop bit
	reg->CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;

	// Enable both RX and TX.
	reg->CTRLB = USART_RXEN_bm | USART_TXEN_bm;

	// Eanble rx interrupt and disable tx, becouse we don't send data yet
    reg->CTRLA = USART_RXCINTLVL_HI_gc | USART_DREINTLVL_OFF_gc;

}

void uart::flush()
{
    while(!txBuffer.isEmpty());
}


void uart::sendChar(const char c)
{
    while(txBuffer.isFull());
	txBuffer.write(c);

	//Enable interrupt for sending data
	reg->CTRLA = USART_RXCINTLVL_HI_gc | USART_DREINTLVL_HI_gc;
}

void uart::sendString(const char *str)
{
	while (*str)
        sendChar(*str++);

}

void uart::sendStringPgm(const char *str)
{
    while (pgm_read_byte(str))
        sendChar(pgm_read_byte(str++));

}

uint8_t uart::sendStringNonBlocking(const char *str)
{
	uint8_t charsSent = 0;

	while (*str)
    {
        if(txBuffer.isFull()) return charsSent;
        txBuffer.write(*str++);
        charsSent++;
        reg->CTRLA = USART_RXCINTLVL_HI_gc | USART_DREINTLVL_HI_gc;
    }

    return charsSent;
}

uint8_t uart::sendStringPgmNonBlocking(const char *str)
{
	uint8_t charsSent = 0;

	while (pgm_read_byte(str))
    {
        if(txBuffer.isFull()) return charsSent;
        txBuffer.write(pgm_read_byte(str++));
        charsSent++;
        reg->CTRLA = USART_RXCINTLVL_HI_gc | USART_DREINTLVL_HI_gc;
    }

    return charsSent;
}


void uart::sendInt(const int32_t i)
{
    char itoa_tmp[12];
    ltoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendInt(const int16_t i)
{
    char itoa_tmp[7];
    itoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendInt(const int8_t i)
{
    char itoa_tmp[5];
    itoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendInt(const uint32_t i)
{
    char itoa_tmp[11];
    ltoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendInt(const uint16_t i)
{
    char itoa_tmp[6];
    itoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendInt(const uint8_t i)
{
    char itoa_tmp[4];
    itoa (i, itoa_tmp, 10);
    sendString(itoa_tmp);
}

void uart::sendHex(const uint32_t i)
{
    sendHex((uint16_t)(i >> 16));
    sendHex((uint16_t) i);
}

void uart::sendHex(const uint16_t i)
{
    sendHex((uint8_t)(i >> 8));
    sendHex((uint8_t) i);
}

void uart::sendHex(const uint8_t i)
{
    char h = (i >> 4);
    if(h > 9) h += 55;
    else h += 48;
    sendChar(h);

    char l = (i & 0x0F);
    if(l > 9) l += 55;
    else l += 48;
    sendChar(l);
}

void uart::sendHex(const int32_t i)
{
    sendHex((uint32_t) i);
}

void uart::sendHex(const int16_t i)
{
    sendHex((uint16_t) i);
}

void uart::sendHex(const int8_t i)
{
    sendHex((uint8_t) i);
}

uint8_t uart::dataAvailable()
{
	return rxBuffer.size();
}

char uart::getChar()
{
	return rxBuffer.read();
}

uint8_t uart::setBaudRate(const uint32_t baud)
{
    uint32_t div1k;
    uint8_t bscale = 0;
    uint16_t bsel;

    if (baud > (F_CPU/16)) return 0;

    div1k = ((F_CPU*64) / baud) - 1024;
    while ((div1k < 2096640) && (bscale < 7))
    {
        bscale++;
        div1k <<= 1;
    }

    bsel = div1k >> 10;

    reg->BAUDCTRLA = bsel&0xff;
    reg->BAUDCTRLB = ((16-bscale) << 4 ) | (bsel >> 8);

    return 1;
}


