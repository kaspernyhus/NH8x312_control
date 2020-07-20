/*
---------------------------------
SPI module for
ARDUINO NANO
---------------------------------

MISO  =>  PB4
MOSI  =>  PB3 
SCK   =>  PB5

*/  

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>


extern void SPI_init_master();
extern void SPI_init_slave();
extern void SPI_MasterTransmit(uint8_t data);
extern uint8_t SPI_SlaveReceive(void);