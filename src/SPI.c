/*
---------------------------------
SPI module for
ARDUINO NANO
---------------------------------

MOSI  =>  PB3 
SCK   =>  PB5


*/

#include "SPI.h"


//       b7   b6  b5   b4   b3   b2   b1   b0
// SPCR: SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0
//
// SPIE - enable SPI interrupt
// SPE  - enable SPI
// DORD - 0=MSB first, 1=LSB first
// MSTR - 0=slave, 1=master
// CPOL - 0=clock starts low, 1=clock starts high
// CPHA - 0=read on rising-edge, 1=read on falling-edge
// SPRx - 00=osc/4, 01=osc/16, 10=osc/64, 11=osc/128 //
// SPCR = 0x50: SPI enabled as Master, mode 0, at 16/4 = 4 MHz

void Init_SPI_master() {
  DDRB |= (1<<PB3) | (1<<PB5);   // OUTPUTs
  SPCR |= (1<<SPE) | (1<<MSTR);  // SPI setup - enable / set as MASTER
  
  //SPCR |= (1<<CPOL); //Clk polarity - "clock starts HIGH" note: CPHA sets Clk phase

  //SPCR |= (1<<SPR1); // SPI Clock Rate Select. (SPR0 = 0, SPR1 = 1 => f_osc / 64) => 16MHz/64 = 250kHz

  SPSR |= (1<<SPI2X); //SPI double speed
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