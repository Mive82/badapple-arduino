// Mislav Milinković
//
// Bad Apple arduino project
//
// SD Card interface library code
//
// Taken and modified from
// http://www.rjhcoding.com/avrc-sd-interface-1.php
// http://www.rjhcoding.com/avrc-sd-interface-2.php
// http://www.rjhcoding.com/avrc-sd-interface-3.php
// http://www.rjhcoding.com/avrc-sd-interface-4.php

#include "sd.h"

void SPI_init()
{
    // set CS, MOSI and SCK to output
    DDR_SPI |= (1 << CS) | (1 << MOSI) | (1 << SCK);

    // enable pull up resistor in MISO
    DDR_SPI |= (1 << MISO);

    // enable SPI, set as master, and clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPI2X) | (0 << SPR1) | (0 << SPR0);
}

uint8_t SPI_transfer(uint8_t data)
{
    // load data into register
    SPDR = data;

    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)))
        ;

    // return SPDR
    return SPDR;
}

void SD_powerUpSeq()
{
    // make sure card is deselected
    CS_DISABLE();

    // give SD card time to power up
    _delay_ms(1);

    // send 80 clock cycles to synchronize
    for (uint8_t i = 0; i < 10; i++)
        SPI_transfer(0xFF);
}

void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    // transmit command to sd card
    SPI_transfer(cmd | 0x40);

    // transmit argument
    SPI_transfer((uint8_t)(arg >> 24));
    SPI_transfer((uint8_t)(arg >> 16));
    SPI_transfer((uint8_t)(arg >> 8));
    SPI_transfer((uint8_t)(arg));

    // transmit crc
    SPI_transfer(crc | 0x01);
}

uint8_t SD_readRes1()
{
    uint8_t i = 0, res1;

    // keep polling until actual data received
    while ((res1 = SPI_transfer(0xFF)) == 0xFF)
    {
        i++;

        // if no data received for 8 bytes, break
        if (i > 8)
            break;
    }

    return res1;
}

void SD_readRes7(uint8_t *res)
{
    // read response 1 in R7
    res[0] = SD_readRes1();

    // if error reading R1, return
    if (res[0] > 1)
        return;

    // read remaining bytes
    res[1] = SPI_transfer(0xFF);
    res[2] = SPI_transfer(0xFF);
    res[3] = SPI_transfer(0xFF);
    res[4] = SPI_transfer(0xFF);
}

void SD_sendIfCond(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD8
    SD_command(CMD8, CMD8_ARG, CMD8_CRC);

    // read response
    SD_readRes7(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

uint8_t SD_goIdleState()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD0, CMD0_ARG, CMD0_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

void SD_readRes3_7(uint8_t *res)
{
    // read R1
    res[0] = SD_readRes1();

    // if error reading R1, return
    if (res[0] > 1)
        return;

    // read remaining bytes
    res[1] = SPI_transfer(0xFF);
    res[2] = SPI_transfer(0xFF);
    res[3] = SPI_transfer(0xFF);
    res[4] = SPI_transfer(0xFF);
}

void SD_readOCR(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD58
    SD_command(CMD58, CMD58_ARG, CMD58_CRC);

    // read response
    SD_readRes3_7(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

uint8_t SD_sendApp()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD55, CMD55_ARG, CMD55_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

uint8_t SD_sendOpCond()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

uint8_t SD_init()
{
    uint8_t res[5], cmdAttempts = 0;

    SD_powerUpSeq();

    // command card to idle
    while ((res[0] = SD_goIdleState()) != 0x01)
    {
        cmdAttempts++;
        if (cmdAttempts > 10)
            return SD_ERROR;
    }

    // send interface conditions
    SD_sendIfCond(res);
    if (res[0] != 0x01)
    {
        return SD_ERROR;
    }

    // check echo pattern
    if (res[4] != 0xAA)
    {
        return SD_ERROR;
    }

    // attempt to initialize card
    cmdAttempts = 0;
    do
    {
        if (cmdAttempts > 100)
            return SD_ERROR;

        // send app cmd
        res[0] = SD_sendApp();

        // if no error in response
        if (res[0] < 2)
        {
            res[0] = SD_sendOpCond();
        }

        // wait
        _delay_ms(10);

        cmdAttempts++;
    } while (res[0] != SD_READY);

    // read OCR
    SD_readOCR(res);

    // check card is ready
    if (!(res[1] & 0x80))
        return SD_ERROR;

    if (CCS_VAL(res[1]))
    {
        sd_type = SDHC;
    }
    else
    {
        sd_type = SD2;
    }

    return SD_SUCCESS;
}

/*******************************************************************************
 Read single 512 byte block
 token = 0xFE - Successful read
 token = 0x0X - Data error
 token = 0xFF - Timeout
*******************************************************************************/
uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1, read;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD17
    SD_command(CMD17, addr, CMD17_CRC);

    // read R1
    res1 = SD_readRes1();

    // if response received from card
    if (res1 != 0xFF)
    {
        // wait for a response token (timeout = 100ms)
        readAttempts = 0;
        while (++readAttempts != SD_MAX_READ_ATTEMPTS)
            if ((read = SPI_transfer(0xFF)) != 0xFF)
                break;

        if (read == 0xFE)
        {
            // read 512 byte block
            for (uint16_t i = 0; i < 512; i++)
                *buf++ = SPI_transfer(0xFF);

            // read 16-bit CRC
            SPI_transfer(0xFF);
            SPI_transfer(0xFF);
        }

        // set token to card response
        *token = read;
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

uint8_t SD_startMultiBlockRead(uint32_t addr)
{
    uint8_t res1;
    //uint16_t readAttempts;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD18
    SD_command(CMD18, addr, CMD18_CRC);

    // read R1
    res1 = SD_readRes1();

    if (res1 != 0xFF)
    {
        return SD_READY;
    }
    return SD_ERROR;
}

void SD_readMultipleBlocks(uint8_t *buf, uint8_t *token)
{
    //uint8_t res1;
    uint8_t read;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    // wait for a response token (timeout = 100ms)
    readAttempts = 0;
    while (++readAttempts != SD_MAX_READ_ATTEMPTS)
        if ((read = SPI_transfer(0xFF)) != 0xFF)
            break;

    // if response token is 0xFE
    if (read == 0xFE)
    {
        // read 512 byte block
        for (uint16_t i = 0; i < 512; i++)
            *buf++ = SPI_transfer(0xFF);

        // read 16-bit CRC
        SPI_transfer(0xFF);
        SPI_transfer(0xFF);
    }

    // wait for a response token (timeout = 100ms)
    readAttempts = 0;
    while (++readAttempts != SD_MAX_READ_ATTEMPTS)
        if ((read = SPI_transfer(0xFF)) != 0xFF)
            break;

    // if response token is 0xFE
    if (read == 0xFE)
    {
        // read 512 byte block
        for (uint16_t i = 0; i < 512; i++)
            *buf++ = SPI_transfer(0xFF);

        // read 16-bit CRC
        SPI_transfer(0xFF);
        SPI_transfer(0xFF);
    }

    // set token to card response
    *token = read;

    //return res1;
}

void SD_stopMultiBlockRead()
{
    SD_command(CMD12, 0x00, CMD18_CRC);
    SD_readRes1();
}