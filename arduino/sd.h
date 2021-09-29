// Mislav Milinković
//
// Bad Apple arduino project
//
// SD Card interface library header
//
// Taken and modified from
// http://www.rjhcoding.com/avrc-sd-interface-1.php
// http://www.rjhcoding.com/avrc-sd-interface-2.php
// http://www.rjhcoding.com/avrc-sd-interface-3.php
// http://www.rjhcoding.com/avrc-sd-interface-4.php

#ifndef SD_H
#define SD_H

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__ // Koristimo ATMEGA 328P
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

#define SD2 1
#define SDHC 2

// pin definitions
#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define CS PINB2
#define MOSI PINB3
#define MISO PINB4
#define SCK PINB5

// macros
#define CS_ENABLE() PORT_SPI &= ~(1 << CS)
#define CS_DISABLE() PORT_SPI |= (1 << CS)

#define UART_puts(X) uart_putstr(X)

// command definitions
#define CMD0 0
#define CMD0_ARG 0x00000000
#define CMD0_CRC 0x94

#define CMD8 8
#define CMD8_ARG 0x0000001AA
#define CMD8_CRC 0x86 //(1000011 << 1)

// ERROR definitions
#define PARAM_ERROR(X) X & 0b01000000
#define ADDR_ERROR(X) X & 0b00100000
#define ERASE_SEQ_ERROR(X) X & 0b00010000
#define CRC_ERROR(X) X & 0b00001000
#define ILLEGAL_CMD(X) X & 0b00000100
#define ERASE_RESET(X) X & 0b00000010
#define IN_IDLE(X) X & 0b00000001

#define CMD_VER(X) ((X >> 4) & 0xF0)
#define VOL_ACC(X) (X & 0x1F)

#define VOLTAGE_ACC_27_33 0b00000001
#define VOLTAGE_ACC_LOW 0b00000010
#define VOLTAGE_ACC_RES1 0b00000100
#define VOLTAGE_ACC_RES2 0b00001000

#define CMD58 58
#define CMD58_ARG 0x00000000
#define CMD58_CRC 0x00

#define POWER_UP_STATUS(X) X & 0x40
#define CCS_VAL(X) X & 0x40
#define VDD_2728(X) X & 0b10000000
#define VDD_2829(X) X & 0b00000001
#define VDD_2930(X) X & 0b00000010
#define VDD_3031(X) X & 0b00000100
#define VDD_3132(X) X & 0b00001000
#define VDD_3233(X) X & 0b00010000
#define VDD_3334(X) X & 0b00100000
#define VDD_3435(X) X & 0b01000000
#define VDD_3536(X) X & 0b10000000

#define CMD55 55
#define CMD55_ARG 0x00000000
#define CMD55_CRC 0x00

#define ACMD41 41
#define ACMD41_ARG 0x40000000
#define ACMD41_CRC 0x00

#define POWER_UP_STATUS(X) X & 0x40
#define CCS_VAL(X) X & 0x40
#define VDD_2728(X) X & 0b10000000
#define VDD_2829(X) X & 0b00000001
#define VDD_2930(X) X & 0b00000010
#define VDD_3031(X) X & 0b00000100
#define VDD_3132(X) X & 0b00001000
#define VDD_3233(X) X & 0b00010000
#define VDD_3334(X) X & 0b00100000
#define VDD_3435(X) X & 0b01000000
#define VDD_3536(X) X & 0b10000000

#define CMD55 55
#define CMD55_ARG 0x00000000
#define CMD55_CRC 0x00

#define ACMD41 41
#define ACMD41_ARG 0x40000000
#define ACMD41_CRC 0x00

#define SD_SUCCESS 0
#define SD_ERROR 1
#define SD_IN_IDLE_STATE 0x01
#define SD_READY 0x00

#define CMD12 12
#define CMD17 17
#define CMD18 18
#define CMD12_CRC 0x00
#define CMD17_CRC 0x00
#define CMD18_CRC 0x00
#define SD_MAX_READ_ATTEMPTS 1563
#define SD_R1_NO_ERROR(X) X < 0x02

// SPI functions
void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

// SD functions
void SD_powerUpSeq(void);
void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_readRes1(void);
uint8_t SD_goIdleState(void);

void SPI_init();

uint8_t SPI_transfer(uint8_t data);

void SD_readRes7(uint8_t *res);

void SD_sendIfCond(uint8_t *res);

void SD_readRes3_7(uint8_t *res);

void SD_readOCR(uint8_t *res);

uint8_t SD_sendApp();

uint8_t SD_sendOpCond();

uint8_t SD_init(uint8_t *sd_type);

uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token);

uint8_t SD_startMultiBlockRead(uint32_t addr);

void SD_readMultipleBlocks(uint8_t *buf, uint8_t *token);

void SD_stopMultiBlockRead();

#endif