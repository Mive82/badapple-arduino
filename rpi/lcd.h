// Mislav Milinković
//
// Bad Apple RPi project
//
// Library za crtanje frameova na OLED sa Raspberry Pi

#include "i2c.h"

#include <iostream>
#include <fstream>

uint8_t read_buffer[2048];
uint8_t header_text[] = {0x62, 0x61, 0x64, 0x75, 0x69, 0x6E, 0x6F}; //baduino
uint8_t width;
uint8_t height;
int buffer_size;
int frames;
uint8_t offset;

// Citanje i provjera header-a
uint8_t init_file(std::string filename, std::ifstream &file)
{
    uint8_t header_buffer[512];
    file.open(filename, std::ios::in | std::ios::binary);
    file.read((char *)header_buffer, 512);
    for (int i = 0; i < 7; i++)
    {

        if (header_buffer[i + 4] != header_text[i])
        {
            printf("Proslijeđena datoteka nema dobar header\n");
            return 0;
        }
    }
    if (header_buffer[510] != 0xF0 || header_buffer[511] != 0x0D)
    {
        printf("Proslijeđena datoteka nema dobar header\n");
        return 0;
    }
    width = header_buffer[0];             // Širina u pixelima
    height = header_buffer[1];            // Visina u stranicama
    buffer_size = width * height;         // Veličina frame-a u bajtovima
    frames = 0 | header_buffer[2];        //
    frames <<= 8;                         //
    frames |= header_buffer[3];           // Broj Frameova
    printf("frames: %d\n", frames);       //
    printf("%dx%d\n", width, height * 8); //
    offset = (16 - height) / 2;           // Pomak za centriranje slike

    return 1;
}

void read_frame(std::ifstream &file)
{
    file.read((char *)read_buffer, buffer_size);
}

void write_frame()
{
    //horizontal_mode();
    uint8_t frame_buffer[16][129];
    for (int i = 0; i < height; i++)
    {
        frame_buffer[i][0] = 0x40;
        for (int j = 1; j < 129; j++)
        {
            frame_buffer[i][j] = read_buffer[i * 128 + j - 1];
        }
    }
    for (uint8_t i = 0; i < height; i++)
    {
        send_command(0xb0 + i + offset);
        //send_command(0x00);
        //send_command(0x10);
        write(i2c_file, frame_buffer[i], 129);
    }
}

void draw_frame(std::ifstream &file)
{
    read_frame(file);
    write_frame();
}