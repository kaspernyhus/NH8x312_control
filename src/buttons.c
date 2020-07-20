/*
---------------------------------
BUTTONs module for
ARDUINO NANO
---------------------------------

ButtonPin => PD4

74595 Button Scan
CLK   =>  PB5
DATA  =>  PB3
LATCH =>  PD5

74595 LED control
CLK   =>  PB5
DATA  =>  PB3
LATCH =>  PD6
---------------------------------
*/

#include "buttons.h"

void Init_buttons() {
  DDRD &=~ (1<<4); // PD4 high Z input
  PORTD &=~ (1<<4);
  // LATCH PINs
  DDRD |= (1<<LATCH_BUT)|(1<<LATCH_LED);   // set LATCH as OUTPUT
  PORTD &=~ (1<<LATCH_BUT)|(1<<LATCH_LED); // set LATCH pins LOW
}


uint8_t readButtonPin() { //PD4
  uint8_t pinValue = (PIND & (1 << 4));
  return (pinValue >> 4);
}


void shiftOut(uint8_t latchPin, uint8_t data) {
  SPI_MasterTransmit(data);
  PORTD |= (1<<latchPin);
  _delay_us(1);
  PORTD &=~(1<<latchPin);
}


uint8_t scanButtons() {
  uint8_t buttons = 0x00;
  for (int i=0; i<5; i++) {
    uint8_t test = 0x01;
    shiftOut(LATCH_BUT,(test<<i));
    if (readButtonPin() == 0x01) {
      buttons |= (1<<i);
    }
    //_delay_ms(100);
  }
  return buttons; // 0b000DCBAE
}


void setButtonLEDs(uint8_t data) {
  shiftOut(LATCH_LED,data);
}
