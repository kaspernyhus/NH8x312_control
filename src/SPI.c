/*
---------------------------------
SPI module for
ARDUINO NANO
---------------------------------

MOSI  =>  PB3 
SCK   =>  PB5


*/

#include "SPI.h"


void SPI_init_master() {
  DDRB |= (1<<PB5);  // OUTPUT
  SPCR |= (1<<SPE) | (1<<MSTR);  // SPI setup - enable / set as MASTER
  
  //SPCR |= (1<<CPOL); //Clk polarity - "clock starts HIGH" note: CPHA sets Clk phase

  //SPCR |= (1<<SPR1); // SPI Clock Rate Select. (SPR0 = 0, SPR1 = 1 => f_osc / 64) => 16MHz/64 = 250kHz
}


void SPI_init_slave() {
  DDRB |= (1<<3); // OUTPUT - PB3: MISO
  SPCR |= (1<<SPE) | (1<<CPOL); // SPI enable / Clk polarity - "clock starts HIGH" note: CPHA sets Clk phase
  PORTB |= (1<<PB0); // set SS HIGH
}


void SPI_MasterTransmit(uint8_t data) {
  SPDR = data; // save to Data Register
  while(!(SPSR & (1<<SPIF))); // Wait for transmission complete
}


uint8_t SPI_SlaveReceive(void) {
  while(!(SPSR & (1<<SPIF))); // Wait for reception complete
  return SPDR; // Return Data Register
}