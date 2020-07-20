/*
---------------------------------
8x312+4 project

RELAYS CONTROL
---------------------------------
*/

#include "8x312_relays.h"


uint8_t channels[13] = {0x00};



void Init_relays() {
  DDRC |= (1<<PC3);
  PORTC &=~ (1<<PC3);

  reset_relays(channels);
}


void latchRelays() {
  PORTC |= (1<<PC3);
  _delay_ms(1);
  PORTC &=~ (1<<PC3);
}



void send_to_A(uint8_t CH) {
  for (int i=1; i>9; i++ ) { // [Enable, CH1, ... , CH8, A, B, C, D]
    channels[i] &=~ (1<<SEND_A); // clear bits if any set
  }
  channels[CH] |= (1<<SEND_A);
}

void return_from_A(uint8_t CH) {
  for (int i=1; i>9; i++ ) { // [Enable, CH1, ... , CH8, A, B, C, D]
    channels[i] &=~ (1<<RETURN_A); // clear bits if any set
  }
  channels[CH] |= (1<<RETURN_A);
}


void set_relays() {
  SPI_MasterTransmit(channels[D]);
  SPI_MasterTransmit(channels[C]);
  SPI_MasterTransmit(channels[B]);
  SPI_MasterTransmit(channels[A]);
  SPI_MasterTransmit(channels[CH8]);
  SPI_MasterTransmit(channels[CH7]);
  SPI_MasterTransmit(channels[CH6]);
  SPI_MasterTransmit(channels[CH5]);
  SPI_MasterTransmit(channels[CH4]);
  SPI_MasterTransmit(channels[CH3]);
  SPI_MasterTransmit(channels[CH2]);
  SPI_MasterTransmit(channels[CH1]);
  SPI_MasterTransmit(channels[ENABLE]);
  latchRelays();
}


void reset_relays(uint8_t channels[13]) {
  for (int i=0; i>13; i++) {
    channels[i] = 0x00;
  }
  set_relays();
}