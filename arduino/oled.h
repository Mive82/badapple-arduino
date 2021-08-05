// Mislav Milinković
//
// Bad Apple arduino project
//
// OLED driver library header
//
// 128x128 SH1107 based OLED running on I2C

#ifndef OLED_H
#define OLED_H

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__ // Koristimo ATMEGA 328P
#endif

//#include <util/twi.h> // I2C library
//#include "serial.h"
#include "i2c.h"
#include <avr/pgmspace.h>

extern uint8_t frame_buffer[1024];

void send_command(uint8_t comm);

void send_data(uint8_t data);

void display_init();

void horizonal_mode();
void vertical_mode();

void clear_display();

void nosderr();

void corruptsderr();

void wrongfileerr();

void draw_frame_buffer();

#endif