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

  updateBaseLayout();


  while(1) {
    updateScreen();
    check_buttons();
    check_rotary();
    //updateLEDs();
  }
}