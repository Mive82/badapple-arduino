// Mislav Milinković
//
// Bad Apple RPi project
//
// Raspberry Pi header za interfaceanje sa OLED ekranom putem I2C
//
// 128x128 OLED sa SH1107 kontrolerom

#ifndef I2C_H
#define I2C_H

#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

int i2c_file;
//uint8_t buffer[16][129];

void startI2C(uint8_t address)
{
    i2c_file = open("/dev/i2c-1", O_RDWR);
    if (i2c_file < 0)
    {
        perror("Failed to open I2C.\n");
        return;
    }
    if (ioctl(i2c_file, I2C_SLAVE, address) < 0)
    {
        perror("Failed to connect to slave.\n");
        return;
    }
}

void send_command(uint8_t comm)
{
    uint8_t data[2] = {0x80, comm};
    if (write(i2c_file, data, 2) != 1)
    {
        //perror("Failed to send command.\n");
        return;
    }
}

void send_data(uint8_t comm)
{
    uint8_t data[2] = {0x40, comm};
    if (write(i2c_file, data, 2) != 1)
    {
        //perror("Failed to send data.\n");
        return;
    }
}

void horizontal_mode()
{
    send_command(0xA0);
    send_command(0xC8);
}

void vertical_mode()
{
    send_command(0xA0);
    send_command(0xC0);
}

void init_display()
{
    send_command(0xae); // Display OFF
    send_command(0xd5); // Set Display Clock
    send_command(0x50);
    send_command(0x20); // Set Row adress
    send_command(0x81); // Set Contrast Control
    send_command(0x80);
    send_command(0xa0); // Segment remap
    //send_command(0xa4); // Display ON
    send_command(0xa6); // Normal display
    send_command(0xad); // Set external VCC
    send_command(0x80);
    send_command(0xc0); // Set common scan direction
    send_command(0xd9); // Set Phase length
    send_command(0x1f);
    send_command(0xdb); // set Vcomh voltage
    send_command(0x27);
    //send_command(0xaf);
    send_command(0xb0);
    send_command(0x00);
    send_command(0x11);
}

void clear_screen()
{
    horizontal_mode();
    uint8_t buffer[16][129];
    for (int i = 0; i < 16; i++)
    {
        buffer[i][0] = 0x40;
        for (int j = 1; j < 129; j++)
        {
            buffer[i][j] = 0;
        }
    }
    for (uint8_t i = 0; i < 16; i++)
    {
        send_command(0xb0 + i);
        send_command(0x00);
        send_command(0x10);
        write(i2c_file, buffer[i], 129);
    }
}
#endif