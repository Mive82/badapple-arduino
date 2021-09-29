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
#include <string.h>

#include "serial.h"
#include "sdimg.h"
#include "i2c.h"
#include "oled.h"
#include "sd.h"

// Header, prvi dio prvih 512 bajtova
typedef struct _header header;
struct _header
{
    uint8_t h_width;
    uint8_t h_height;
    uint16_t h_frames;
    uint8_t h_header_text[7];
    uint8_t h_loop;
};

typedef struct _playbackinfo playbackinfo;
struct _playbackinfo
{
    uint8_t v_width;
    uint8_t v_height;
    uint16_t v_frames;
    uint8_t v_loop;
    uint8_t v_sd_type;
    uint8_t v_done;
    uint8_t v_fail_state;
};

uint8_t res[5], token;

void error_image(const uint8_t fail_state)
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

void check_sd(playbackinfo *videoInfo)
{
    videoInfo->v_fail_state = 0;
    if (SD_init(&(videoInfo->v_sd_type)) == SD_SUCCESS)
    {
        //uart_putstr("Success!\n");
    }
    else
    {
        //uart_putstr("No SD card\n");
        videoInfo->v_fail_state = 1;
    }
    return;
}

void read_header(playbackinfo *videoInfo)
{
    if (videoInfo->v_done)
    {
        return;
    }

    res[0] = SD_readSingleBlock(0, frame_buffer, &token); // read first 512 bytes

    // parse header
    if (SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
    {
        header v_header; // Inicijaliziraj header
        memcpy(&v_header, &frame_buffer, sizeof(header));

        uart_putstr("Looping: ");
        uart_putint(v_header.h_loop);
        uart_putstr("\nResolution: ");
        uart_putint(v_header.h_width);
        uart_putchar('x');
        uart_putint(v_header.h_height * 8);
        uart_putstr("\nFrames: ");
        uart_putint(v_header.h_frames);
        uart_putstr("\n\n");

        uint8_t header_text[] = {0x62, 0x61, 0x64, 0x75, 0x69, 0x6E, 0x6F}; // "baduino"

        // Invalid header string
        for (int i = 0; i < 7; i++)
        {
            if (v_header.h_header_text[i] != header_text[i])
            {
                videoInfo->v_fail_state = 3;
                return;
            }
        }

        // Invalid resolution
        if (v_header.h_width != 128 || v_header.h_height != 8)
        {
            videoInfo->v_fail_state = 3;
            return;
        }

        videoInfo->v_height = v_header.h_height;
        videoInfo->v_width = v_header.h_width;
        videoInfo->v_loop = v_header.h_loop;
        videoInfo->v_frames = v_header.h_frames;
    }
    else
    {
        //UART_puts("Error reading sector\n");
        videoInfo->v_fail_state = 2;
        return;
    }
}

void read_frames(playbackinfo *videoInfo)
{
    if (videoInfo->v_done)
    {
        return;
    }
    clear_display();
    do
    {
        // SD v2 uses byte offset (za ovo sam propisao krv)
        if (videoInfo->v_sd_type == SD2)
        {
            res[0] = SD_startMultiBlockRead(512);
        }
        // SDHC/XC uses block offset
        else
        {
            res[0] = SD_startMultiBlockRead(1);
        }

        // Reading block data directly into the frame buffer
        for (int i = 0; i < videoInfo->v_frames; i++)
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
                videoInfo->v_fail_state = 2;
                return;
            }
        }

        SD_stopMultiBlockRead();
    } while (videoInfo->v_loop);
}

int main()
{
    sei();

    i2c_init(0x3C); // hex 3C je adresa OLED-a
    display_init();
    horizonal_mode();
    clear_display();    // Nuliraj oled vram
    send_command(0xaf); // Upali oled
    _delay_ms(50);

    uart_init();
    // initialize SPI
    SPI_init();
    _delay_ms(10);

    playbackinfo videoInfo;
    memset(&videoInfo, 0, sizeof(playbackinfo));

    // hotswap yaaaaay
    while (1)
    {
        check_sd(&videoInfo);
        if (videoInfo.v_fail_state)
        {
            videoInfo.v_done = 0;
            error_image(videoInfo.v_fail_state);
            _delay_ms(100);
            continue;
        }
        read_header(&videoInfo);

        if (videoInfo.v_fail_state)
        {
            error_image(videoInfo.v_fail_state);
            _delay_ms(100);
            continue;
        }
        read_frames(&videoInfo);

        if (videoInfo.v_fail_state)
        {
            error_image(videoInfo.v_fail_state);
            _delay_ms(2000);
            continue;
        }
        else
        {
            videoInfo.v_done = 1;
        }
        _delay_ms(200);
    }

    return 0;
}
