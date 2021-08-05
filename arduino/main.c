// Mislav Milinković
//
// Bad Apple arduino project "baduino"
//
// Projekt iz kolegija Ugrađeni računalni sustavi
// "Bad Apple arduino" name pending
//
// Kod za Arduino:
//      - SD & OLED driver
//      - Video decoder

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__ // Koristimo ATMEGA 328P
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "sdimg.h"
#include "i2c.h"
#include "oled.h"
#include "sd.h"

volatile uint8_t fail_state = 0;
volatile uint16_t n_frames = 0;
volatile uint16_t data_ptr = 0;
volatile uint8_t sd_type = 0;
volatile uint8_t done = 0;
volatile uint8_t loop = 0;

uint8_t res[5], token;
uint8_t header_text[] = {0x62, 0x61, 0x64, 0x75, 0x69, 0x6E, 0x6F}; // "baduino"

void error_image()
{
    if (fail_state == 0)
    {
        return;
    }
    if (fail_state == 1) // No SD card
    {
        nosderr();
    }
    if (fail_state == 2) // Corrupt file
    {
        corruptsderr();
    }
    if (fail_state == 3) // Wrong file
    {
        wrongfileerr();
    }
}

void read_header()
{
    if (done)
    {
        return;
    }
    res[0] = SD_readSingleBlock(0, frame_buffer, &token); // read first 512 bytes
    // parse header
    if (SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
    {
        // "baduino" in hex from offset 4
        for (int i = 0; i < 7; i++)
        {

            if (frame_buffer[i + 4] != header_text[i])
            {
                //uart_putstr("Invalid file format\n");
                fail_state = 3;
                return;
            }
        }

        // 0xF00D on the last 2 bytes for shits and giggles
        if (frame_buffer[510] != 0xF0 || frame_buffer[511] != 0x0D)
        {
            //uart_putstr("Invalid file format\n");
            fail_state = 3;
            return;
        }

        // first 4 bytes - width in pixels, height in pages, no. of frames (2 bytes)
        data_ptr++;
        if (frame_buffer[0] != 128 || frame_buffer[1] != 8)
        {
            //uart_putstr("Invalid resolution\n");
            fail_state = 3;
            return;
        }
        n_frames = 0 | (frame_buffer[2] << 8);
        n_frames |= frame_buffer[3];
        loop = frame_buffer[11];
        uart_putstr("Looping: ");
        uart_putint(loop);
        uart_putstr("\nResolution: ");
        uart_putint(frame_buffer[0]);
        uart_putchar('x');
        uart_putint(frame_buffer[1] * 8);
        uart_putstr("\nFrames: ");
        uart_putint(n_frames);
        uart_putstr("\n\n");
    }
    else
    {
        //UART_puts("Error reading sector\n");
        fail_state = 2;
        return;
    }
}

void read_frames()
{
    if (done)
    {
        return;
    }
    clear_display();
    do
    {
        // SD v2 uses byte offset (za ovo sam propisao krv)
        if (sd_type == SD2)
        {
            res[0] = SD_startMultiBlockRead(512);
        }
        // SDHC/XC uses block offset
        else
        {
            res[0] = SD_startMultiBlockRead(1);
        }

        // Reading block data directly into the frame buffer
        for (int i = 0; i < n_frames; i++)
        {
            //res[0] = SD_readMultipleBlocks(frame_buffer, &token);
            SD_readMultipleBlocks(frame_buffer, &token);
            //if (SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
            if ((token == 0xFE))
            {
                draw_frame_buffer(); // Drawing the frame buffer from RAM
            }
            else
            {
                UART_puts("Error multireading sector\n");
                fail_state = 2;
                return;
            }
        }

        SD_stopMultiBlockRead();
    } while (loop);
}

void check_sd()
{
    fail_state = 0;
    if (SD_init() == SD_SUCCESS)
    {
        //uart_putstr("Success!\n");
    }
    else
    {
        //uart_putstr("No SD card\n");
        fail_state = 1;
        return;
    }
}

int main()
{
    fail_state = 0;
    sei();

    i2c_init(0x3C); // hex 3C je adresa OLED-a
    display_init();
    horizonal_mode();
    clear_display();
    send_command(0xaf);
    _delay_ms(50);

    uart_init();
    // initialize SPI
    SPI_init();
    _delay_ms(10);

    // hotswap yaaaaay
    while (1)
    {
        check_sd();
        if (fail_state)
        {
            done = 0;
            error_image();
            _delay_ms(100);
            continue;
        }
        read_header();

        if (fail_state)
        {
            error_image();
            _delay_ms(100);
            continue;
        }
        read_frames();

        if (fail_state)
        {
            error_image();
            _delay_ms(2000);
            continue;
        }
        else
        {
            done = 1;
        }
        _delay_ms(200);
    }

    return 0;
}
