/*
---------------------------------
BUTTONs module for
ARDUINO NANO
---------------------------------

ButtonPin => PD4

74595 Button/LED control
CLK   =>  PB5
DATA  =>  PB3
LATCH =>  PD6

---------------------------------
*/

#include "buttons.h"

void Init_buttons() {
  DDRD &=~ (1<<4); // PD4 high Z input
  PORTD &=~ (1<<4);
  // LATCH PIN
  DDRD |= (1<<LATCH_BUT_LED);   // set LATCH as OUTPUT
  PORTD &=~ (1<<LATCH_BUT_LED); // set LATCH pin LOW
}


uint8_t readButtonPin() { //PD4
  uint8_t pinValue = (PIND & (1 << 4));
  return (pinValue >> 4);
}


void shiftOut(uint8_t latchPin, uint8_t data) {
  SPI_MasterTransmit(data);
  PORTD |= (1<<latchPin);
  _delay_us(1);
  PORTD &=~ (1<<latchPin);
}


uint8_t scanButtons(uint8_t activeLEDs) { 
  uint8_t buttons = 0x00;
  uint8_t _activeLEDs = (activeLEDs<<4);   // 0bLEDSxxxx
  for (int i=0; i<4; i++) {
    uint8_t test_mask = _activeLEDs|(0x01<<i);   // 0bLEDS0001
    shiftOut(LATCH_BUT_LED,test_mask);
    if (readButtonPin() == 0x01) {
      buttons |= (1<<i);
    }
  }
  return buttons; // 0bxxxxDCBA
}


void setButtonLEDs(uint8_t data) {
  shiftOut(LATCH_BUT_LED,(data<<4));
}
