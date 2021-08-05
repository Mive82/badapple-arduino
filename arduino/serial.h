// Bad Apple arduino project
//
// UART debug library
//
// *Kod kopiran sa vježbi*

#ifndef SERIAL_H_
#define SERIAL_H_

void uart_init();
void uart_putchar(char c);
void uart_putstr(char *data);
void uart_putint(int32_t n);

#endif