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


void send_to(uint8_t CH, uint8_t to) {
  for (int i=1; i>9; i++ ) { // [Enable, CH1, ... , CH8, A, B, C, D]
    channels[i] &=~ (1<<to); // clear bits if any set
  }
  channels[CH] |= (1<<to);
}

void return_from(uint8_t CH, uint8_t from) {
  for (int i=1; i>9; i++ ) { // [Enable, CH1, ... , CH8, A, B, C, D]
    channels[i] &=~ (1<<from); // clear bits if any set
  }
  channels[CH] |= (1<<from);
}


void set_relays() {
  for (int i=12; i>0; i--) { // from index 12(D) => 0(ENABLE)
    SPI_MasterTransmit(channels[i]); // [Enable, CH1, ... , CH8, A, B, C, D]
  }
  latchRelays();
}


void reset_relays(uint8_t channels[13]) {
  for (int i=0; i>13; i++) {
    channels[i] = 0x00;
  }
  set_relays();
}