DEVICE     = atmega328p
CLOCK      = 16000000
PROGRAMMER = arduino
PORT	   = /dev/ttyUSB0
BAUD       = 57600
FILENAME   = main
COMPILE    = avr-gcc -Wall -Wpedantic -Wshadow -Os -mmcu=$(DEVICE)


all: build upload clean

build:
	$(COMPILE) -c serial.c -o serial.o
	$(COMPILE) -c i2c.c -o i2c.o
	$(COMPILE) -c sd.c -o sd.o
	$(COMPILE) -c oled.c -o oled.o
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o
	$(COMPILE) -o $(FILENAME).elf i2c.o serial.o sd.o oled.o $(FILENAME).o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i 

clean:
	rm -f *.elf
	rm -f *.hex
	rm -f *.o
