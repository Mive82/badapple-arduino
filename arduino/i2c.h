// Mislav Milinković
//
// Bad Apple arduino project
//
// I2C library header

#ifndef I2C_H
#define I2C_H

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__ // Koristimo ATMEGA 328P
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <util/twi.h> // I2C library

extern volatile uint8_t i2c_address;
extern volatile uint8_t received;

void i2c_init(uint8_t add);

void i2c_stop();

void i2c_start(uint8_t rw);

void i2c_write(uint8_t data);

void send_byte(uint8_t byte);

#endif