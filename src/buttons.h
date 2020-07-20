/*
---------------------------------
BUTTONs module for
ARDUINO NANO
---------------------------------

ButtonPin => PD4

74595 Button Scan
CLK   =>  PB5
DATA  =>  PB3
LATCH =>  PD5

74595 LED control
CLK   =>  PB5
DATA  =>  PB3
LATCH =>  PD6
---------------------------------
*/

#define LATCH_BUT 5
#define LATCH_LED 6


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI.h"
#include "config.h"


void Init_buttons();
uint8_t readButtonPin();
uint8_t scanButtons();
void setButtonLEDs(uint8_t buttons);