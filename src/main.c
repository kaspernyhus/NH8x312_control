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

  setLED(0,1);
  setLED(1,2);
  setLED(2,3);

  updateBaseLayout();


  while(1) {
    updateScreen();
    check_buttons();
    //updateLEDs();
  }
}