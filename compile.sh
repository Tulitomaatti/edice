#!/bin/bash

avr-gcc -g -Os -c edice.c encoders.c max7221.c serial.c random.c tinymt32.c;
avr-gcc -g -mmcu=atmega328 -o edice.elf edice.o max7221.o encoders.o serial.o random.o tinymt32.o;
avr-objcopy -j .text -j .data -O ihex edice.elf edice.hex;
avrdude -c avrispmkii -p atmega328 -U flash:w:edice.hex -v -v -v -v -B 15;
