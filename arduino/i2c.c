// Mislav Milinković
//
// Bad Apple arduino project
//
// I2C library code

#include "i2c.h"

volatile uint8_t i2c_address;
volatile uint8_t received;

void i2c_init(uint8_t address)
{
    TWBR = 1; // Najbrza I2C postavka, 889 kHz - nije najstabilnije, ali inace je pre sporo

    i2c_address = address;
}

void i2c_stop()
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); // Zaustavi komunikaciju
}

void i2c_start(uint8_t rw)
{
    TWCR = 0; // Resetiraj komunikacijski registar

    TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN); // Počni komunikaciju

    // Čekamo dok prijenos ne završi
    while (!(TWCR & (1 << TWINT)))
        ;

    if ((TWSR & 0xF8) != TW_START)
    {
        return; // Dogodila se greška
    }

    TWDR = (i2c_address << 1) | rw;

    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    uint8_t status = (TW_STATUS & 0xF8);

    if (status != TW_MT_SLA_ACK && status != TW_MR_SLA_ACK)
    {
        return; // Dogodila se greška
    }
}

void i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    uint8_t status = (TW_STATUS & 0xF8);

    if (status != TW_MT_DATA_ACK)
    {
        return; // Dogodila se greška
    }
}

void send_byte(uint8_t byte)
{
    i2c_start(0);
    i2c_write(byte);
    i2c_stop();
}
