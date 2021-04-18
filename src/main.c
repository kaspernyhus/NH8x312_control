/*
 -----------------
 ARDUINO NANO
 -----------------

 8x312+4 project main.c

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"
#include "8x312_control.h"


int main() {
  Init_8x312();
  sei();

  // updateBaseLayout();

                // CIE    1     2     3     4     A     B     
  // int8_t test[7] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
  // set_relays(test, 6);
  // _delay_ms(1000);
  // test[1] = 0x02;
  // set_relays(test, 6);
  // _delay_ms(1000);
  // test[1] = 0x04;
  // set_relays(test, 6);
  // _delay_ms(1000);
  // test[1] = 0x08;
  // set_relays(test, 6);

  while(1) {
    // updateScreen();
    // check_buttons();
    // check_rotary();
    // updateLEDs();
    // relay_test(1000);
    
                  // CIE    1     2     3     4     5     6     7     8     A    B    C    D
    int8_t test[7] = {0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
    set_relays(test, 6);
    _delay_ms(1000);
    test[0] = 0x01;
    set_relays(test, 6);
    _delay_ms(1000);
  }
}