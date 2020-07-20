/*
---------------------------------
ROTARY ENCODER module for
ARDUINO NANO
---------------------------------



---------------------------------
*/

#include "rotEnc.h"


void Init_rotEnc() {
  itr0_Init();
  itr1_Init();
  DDRD &=~ (1<<2)|(1<<3);
  PORTD |= (1<<2)|(1<<3); // PD2,PD3 input pullup
  interrupt_0_flag = 0;
  interrupt_1_flag = 0;
}


ISR (INT0_vect) {
  interrupt_0_flag = 1;
}

ISR (INT1_vect) {
  interrupt_1_flag = 1;
}