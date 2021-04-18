/*
---------------------------------
8x312+4 project

RELAYS CONTROL
---------------------------------
*/

#include "8x312_relays.h"


uint8_t channel_routing[13] = {0x00};


void Init_relays() {
  DDRD |= (1<<LATCH_RELAYS); // LATCH PIN as OUTPUT
  PORTD &=~ (1<<LATCH_RELAYS);
  reset_relays(channel_routing);
}

void latchRelays() {
  PORTD |= (1<<LATCH_RELAYS);
  _delay_ms(1);
  PORTD &=~ (1<<LATCH_RELAYS);
}

void channel_insert_enable(uint8_t CH) {
  channel_routing[0] |= (1<<CH);
}

void channel_insert_disable(uint8_t CH) {
  channel_routing[0] &=~ (1<<CH);  
}

void send_to(uint8_t CH, uint8_t to) {
  for (int i=0; i<4; i++) {
    channel_routing[CH+1] &=~ (1<<i); // reset potential send routings
  }
  channel_routing[CH+1] |= (1<<to);
}

void return_from(uint8_t CH, uint8_t from) {  
  for (int i=0; i<4; i++) {
    channel_routing[CH+1] &=~ (0x10<<i); // reset potential return routings
  }
  // set new return routings
  channel_routing[CH+1] |= (0x10<<from); // 0b00010000
}

void update_relay_states() {
  for (int i=12; i>=0; i--) {                // backwards shift out - from index 12(D) => 0(ENABLE)
    SPI_MasterTransmit(channel_routing[i]); // [Enable, CH1, ... , CH8, A, B, C, D]
  }
  latchRelays();
}

void reset_relays() {
  for (int i=0; i<13; i++) {
    channel_routing[i] = 0x00;
  }
  update_relay_states();
}


/* *************************
        RELAY TESTING
   ************************* */ 
void print_relay_status(uint8_t test[13]) {
  for (int i= 0; i<13; i++) {
    UART0_printBits_nonewline("", test[i],8);
    UART0_puts(" ");
  }
  UART0_puts("\r\n");
}

void set_relays(uint8_t test[13], uint8_t ch_count) {
  for (int i=1; i<=ch_count; i++) {
    SPI_MasterTransmit(test[i]); // [CH1, ... , CH8, A, B, C, D]
  }
  SPI_MasterTransmit(test[0]); // CIE
  latchRelays();
}

void reset_test_array(uint8_t test[13]) {
  for (int i=0; i<13; i++) {
    test[i] = 0x00;
  }
}

void relay_test(uint16_t delay) {
  UART0_puts("CIE      CH1      CH2      CH3      CH4      CH5      CH6      CH7      CH8      A        B        C        D\r\n");
  UART0_puts("--------------------------------------------------------------------------------------------------------------------\r\n");
  uint8_t test[13] = {0x00};
  set_relays(test, 12);
  print_relay_status(test);
  _delay_ms(delay);

  for (int i=0; i<8; i++) {
    test[0] = (0x01<<i);    // CIE
    set_relays(test, 12);
    print_relay_status(test);
    _delay_ms(delay*2);
    
    for (int r=0; r<8; r++) {
      test[i+1] = (0x01<<r);  // channel relays
      set_relays(test, 12);
      print_relay_status(test);
      _delay_ms(delay);
    }
    reset_test_array(test);
  }
  for (int i=0; i<4; i++) {
    for (int r=0; r<4; r++) {
      test[i+9] = (0x01<<r);  // insert relays
      set_relays(test, 12);
      print_relay_status(test);
      _delay_ms(delay);
    }
    reset_test_array(test);
  }
}
