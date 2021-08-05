// Mislav Milinković
//
// Bad Apple arduino project
//
// OLED driver library code
//
// 128x128 SH1107 based OLED running on I2C

#include "oled.h"

uint8_t frame_buffer[1024];

extern const unsigned char no_sd_img[2048] PROGMEM;
extern const unsigned char corrupt_file_img[2048] PROGMEM;
extern const unsigned char wrong_file_img[2048] PROGMEM;

void send_command(uint8_t comm)
{
    i2c_start(0);
    i2c_write(0x80);
    i2c_write(comm);
    i2c_stop();
}

void send_data(uint8_t data)
{
    i2c_start(0);
    i2c_write(0x40);
    i2c_write(data);
    i2c_stop();
}

void display_init()
{
    send_command(0xae); // Display OFF
    send_command(0xd5); // Set Display Clock
    send_command(0x50);
    send_command(0x20); // Set Row adress
    send_command(0x81); // Set Contrast Control
    send_command(0x80);
    send_command(0xa0); // Segment remap
    //send_command(0xa4); // Set Entire display on
    send_command(0xa6); // Normal display
    send_command(0xad); // Set external VCC
    send_command(0x80);
    send_command(0xc0); // Set common scan direction
    send_command(0xd9); // Set Phase length
    send_command(0x1f);
    send_command(0xdb); // set Vcomh voltage
    send_command(0x27);
    //send_command(0xaf); // Display ON
}

void horizonal_mode()
{
    send_command(0xA0);
    send_command(0xC8);
}

void vertical_mode()
{
    send_command(0xA0);
    send_command(0xC0);
}

void clear_display()
{
    for (uint8_t i = 0; i < 16; i++)
    {
        send_command(0xb0 + i);
        send_command(0x0);
        send_command(0x10);
        i2c_start(0);
        i2c_write(0x40);
        for (uint8_t j = 0; j < 128; j++)
        {
            i2c_write(0x00);
        }
        i2c_stop();
    }
}

void nosderr()
{
    send_command(0x00);
    send_command(0x10);
    uint8_t temp;
    for (uint8_t i = 0; i < 16; i++)
    {
        send_command(0xb0 + i);
        i2c_start(0);
        i2c_write(0x40);
        for (uint8_t j = 0; j < 128; j++)
        {
            temp = pgm_read_byte_near(no_sd_img + i * 128 + j);
            i2c_write(temp);
        }
        i2c_stop();
    }
}

void corruptsderr()
{
    send_command(0x00);
    send_command(0x10);
    uint8_t temp;
    for (uint8_t i = 0; i < 16; i++)
    {
        send_command(0xb0 + i);
        i2c_start(0);
        i2c_write(0x40);
        for (uint8_t j = 0; j < 128; j++)
        {
            temp = pgm_read_byte_near(corrupt_file_img + i * 128 + j);
            i2c_write(temp);
        }
        i2c_stop();
    }
}

void wrongfileerr()
{
    send_command(0x00);
    send_command(0x10);
    uint8_t temp;
    for (uint8_t i = 0; i < 16; i++)
    {
        send_command(0xb0 + i);
        i2c_start(0);
        i2c_write(0x40);
        for (uint8_t j = 0; j < 128; j++)
        {
            temp = pgm_read_byte_near(wrong_file_img + i * 128 + j);
            i2c_write(temp);
        }
        i2c_stop();
    }
}

void draw_frame_buffer()
{
    send_command(0x00);
    send_command(0x10);
    for (uint8_t i = 0; i < 8; i++)
    {
        send_command(0xb0 + i + 4);
        i2c_start(0);
        i2c_write(0x40);
        for (uint8_t j = 0; j < 128; j++)
        {
            i2c_write(frame_buffer[i * 128 + j]);
        }
        i2c_stop();
    }
}
