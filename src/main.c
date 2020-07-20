/*
 -----------------
 ARDUINO NANO
 -----------------
 
 DISPLAY TEST

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"
#include "8x312_control.h"


void init() {
  Init_8x312();
  sei();
}


int main() {
  init();

  // setLED(0,1);
  // setLED(1,2);
  // setLED(2,3);
  // setInsertState(0,0x41,1);
  // setInsertState(1,0x43,1);
  // setInsertState(4,0x42,1);

  updateBaseLayout();

  // send_to_A(CH1);
  // return_from_A(CH1);
  // set_relays();

  //SQselect(2,0);

  // drawPixel(0,0,RED);
  // drawPixel(0,160,RED);
  // drawPixel(128,0,RED);
  // drawPixel(128,160,RED);

  while(1) {
    updateScreen();
    check_buttons();
    // updateLEDs();
  }
}