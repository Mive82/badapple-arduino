// Bad Apple arduino project
//
// UART debug code
//
// *Kod kopiran sa vježbi*

#ifndef F_CPU
#define F_CPU 16000000
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__ // Koristimo ATMEGA 328P
#endif

#define BAUD 9600

#include <stdio.h>

#include <avr/io.h>

#include <avr/sfr_defs.h>

#include <stdlib.h>

#include <util/setbaud.h>
#include "serial.h"

void uart_init()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UCSR0B = _BV(TXEN0);
}

void uart_putchar(char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

void uart_putstr(char *data)
{
	while (*data)
		uart_putchar(*data++);
}

void uart_putint(int32_t n)
{
	char str[10];
	itoa(n, str, 10);
	uart_putstr(str);
	//uart_putchar('\n');
}