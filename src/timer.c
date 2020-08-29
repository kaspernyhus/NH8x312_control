/*
---------------------------------
Timer module for
ARDUINO NANO
---------------------------------
*/

#include "timer.h"


void timer1_Init(unsigned int compareValue) {
  TCCR1B |= (1<<CS11) | (1<<CS10); // pre-scaling
  TCCR1B |= (1<<WGM12); // CTC mode, TOP value = OCR1A
  OCR1A = compareValue; // f.eks. for 100ms = compareValue: 6249
  TIMSK1 |= (1<<OCIE1A); // interrupt when TCNT1 = OCR1A value
}

void timer2_Init(unsigned int compareValue) {
  /*----------------------------------------------------------------
              PRE-SCALING 
  ------------------------------------------------------------------*/
  // TCCR2B |= (1<<CS20);                         // no prescaling
  // TCCR2B |= (1<<CS21);                         // clk/8
  // TCCR2B |= (1<<CS21) | (1<<CS20);             // clk/32
  TCCR2B |= (1<<CS22);                         // clk/64
  // TCCR2B |= (1<<CS22) | (1<<CS20);             // clk/128
  // TCCR2B |= (1<<CS22) | (1<<CS21);             // clk/256
  // TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);  // clk/1024


  TCCR2A |= (1<<WGM21); // CTC mode, TOP value = OCR2A
  OCR2A = compareValue;
  TIMSK2 |= (1<<OCIE2A); // interrupt when TCNT2 = OCR2A value
}
