/*
---------------------------------
Interrupt module for
ARDUINO NANO
---------------------------------
*/

#include "external_interrupts.h"


void itr0_Init() {
  // initialize external interrupt INT0
  EICRA |= (1<<ISC00);
  
  EIMSK |= (1<<INT0);   // enable INT0
}

void itr1_Init() {
  // initialize external interrupt INT1
  EICRA |= (1<<ISC10);
 
  EIMSK |= (1<<INT1);   // enable INT1
}