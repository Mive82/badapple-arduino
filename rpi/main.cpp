// Mislav Milinković
//
// Bad Apple RPi project
//
// Projekt iz kolegija Ugrađeni računalni sustavi
//
// Kod za RPi:
//      - OLED driver
//      - Video decoder

#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <chrono>
#include <signal.h>

#include "i2c.h"
#include "lcd.h"

bool kill_flag = 0;

void ubij_program(int sig)
{
    kill_flag = 1;
}

int main(int argc, char *argv[])
{
    std::string input_file_name;

    if (argc < 2)
    {
        printf("Proslijedi ime datoteke.");
        return 0;
    }
    else
    {
        input_file_name = argv[1];
    }

    signal(SIGINT, ubij_program);
    int fps = 0, frame_count = 0;
    uint8_t address{0x3c};
    startI2C(address);

    init_display();
    clear_screen();
    send_command(0xaf);
    //white_screen();
    std::ifstream inputfile;
    if ((init_file(input_file_name, inputfile)) != 1)
    {
        kill_flag = 1;
        return 0;
    }
    for (int i = 0; i < frames; i++)
    {
        if (kill_flag)
        {
            printf("\nPrekid primljen, izlazim...\n");
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        draw_frame(inputfile);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        if (duration.count() < 33)
        {
            usleep((33 - duration.count()) * 1000);
        }
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        fps += duration.count();
        frame_count++;
        //printf("%d: %d\n", i+1, duration.count());
    }

    printf("Average fps: %d\n", ((1000 * frame_count) / fps));
    send_command(0xae);
    clear_screen();
    inputfile.close();
    close(i2c_file);
    return 0;
}