/*
---------------------------------
Rotary Encoder module for
ARDUINO NANO
---------------------------------
*/

/*

    Previous State - Next State
      (previous AB - next AB)
 ----------------------------------
|   PSNS   |  Valid  |  Direction  |
|–---------------------------------|
|   0000   |    X    |      X      |
|   0001   |  Valid  |     CW      |
|   0010   |  Valid  |     CCW     |
|   0011   |    X    |      X      |
|   0100   |  Valid  |     CCW     |
|   0101   |    X    |      X      |
|   0110   |    X    |      X      |
|   0111   |  Valid  |     CW      |
|   1000   |  Valid  |     CW      |
|   1001   |    X    |      X      |
|   1010   |    X    |      X      |
|   1011   |  Valid  |     CCW     |
|   1100   |    X    |      X      |
|   1101   |  Valid  |     CCW     |
|   1110   |  Valid  |     CW      |
|   1111   |    X    |      X      |
 –---------------------------------

0=X, 1=CW, -1=CCW

rot_enc_table[]= {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};

*/

#include "RotEnc.h"


static int8_t rot_enc_table[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
volatile uint8_t PSNS = 0; // Previous State - Next State
volatile uint8_t store = 0;
volatile uint8_t CCW = 0;
volatile uint8_t CW = 0;


void RotaryAttachOnCW( void (*functionPointer)() ) {
  onCWmove = functionPointer;
}

void RotaryAttachOnCCW( void (*functionPointer)() ) {
  onCCWmove = functionPointer;
}


void Init_RotEnc() {
  itr0_Init(); // PD2
  itr1_Init();
  // Sets RotPins as input pull-up
  PORTD |= (1<<RotPinA)|(1<<RotPinB)|(1<<RotButPin);
}


// // return direction, 0 when no movement
// int8_t read_rotary() {
//   // Previous State - Next State
//   PSNS <<= 2; // shift previous state bits = ABXX
//   if ( !!(PIND & (1<<RotPinA)) ) PSNS |= 0x02; // read pinA; if A=1, set bit 1
//   if ( !!(PIND & (1<<RotPinB)) ) PSNS |= 0x01; // read pinB; if B=1, set bit 0
//   PSNS &= 0x0F; // mask out old bits = 0b0000PSNS
  
//   if (rot_enc_table[PSNS]) { // if 1 or -1
//     store <<= 4;
//     store |= PSNS;
//     if ((store&0xff)==0x2b) {
//       if ( *onCCWmove != 0 ) {
//         (*onCCWmove)();
//       }
//       return -1;
//     }
//     if ((store&0xff)==0x17) {
//       if ( *onCWmove != 0 ) {
//         (*onCWmove)();
//       }
//       return 1;
//     }
//   }
//   return 0;
// }


scan_rotary_encoder() {
  // Previous State - Next State
  PSNS <<= 2; // shift previous state bits = ABXX
  if ( !!(PIND & (1<<RotPinA)) ) PSNS |= 0x02; // read pinA; if A=1, set bit 1
  if ( !!(PIND & (1<<RotPinB)) ) PSNS |= 0x01; // read pinB; if B=1, set bit 0
  PSNS &= 0x0F; // mask out old bits = 0b0000PSNS
  
  if (rot_enc_table[PSNS]) { // if 1 or -1
    store <<= 4;
    store |= PSNS;
    if ((store&0xff)==0x2b) {
      CCW = 1;
    }
    if ((store&0xff)==0x17) {
      CW = 1;
    }
  }
}


uint8_t scan_encoder_button() {
  return !(PIND & (1<<RotButPin));
}


void check_rotary() {
  if (CW) {
    CW = 0;
    (*onCWmove)();
  }
  if (CCW) {
    CCW = 0;
    (*onCCWmove)();
  }
}


ISR(INT0_vect) {
  cli();
  scan_rotary_encoder();
  sei();
}


ISR(INT1_vect) {
  cli();
  scan_rotary_encoder();
  sei();
}
