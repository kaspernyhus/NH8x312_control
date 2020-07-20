/*
---------------------------------
ROTARY ENCODER module for
ARDUINO NANO
---------------------------------



---------------------------------
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "external_interrupts.h"


void Init_rotEnc();


volatile uint8_t interrupt_0_flag;
volatile uint8_t interrupt_1_flag;