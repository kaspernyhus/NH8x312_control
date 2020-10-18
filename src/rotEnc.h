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


#define RotPinA 2 // PD2
#define RotPinB 3 // PD3
#define RotButPin 5 // PD5


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "external_interrupts.h"
#include "UART.h"


void (*onCWmove)(void);
void (*onCCWmove)(void);


void Init_RotEnc();
void RotaryAttachOnCW( void (*functionPointer)() );
void RotaryAttachOnCCW( void (*functionPointer)() );
//int8_t read_rotary();
void check_rotary();
uint8_t read_enc_button();
void scan_rotary();