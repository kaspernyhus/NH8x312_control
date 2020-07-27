/*
---------------------------------
8x312+4 project

RELAYS CONTROL
---------------------------------
*/

// #define ENABLE 0
// #define CH1 1
// #define CH2 2
// #define CH3 3
// #define CH4 4
// #define CH5 5
// #define CH6 6
// #define CH7 7
// #define CH8 8
// #define A 9
// #define B 10
// #define C 11
// #define D 12
// #define SEND_A 0
// #define SEND_B 1
// #define SEND_C 2
// #define SEND_D 3
// #define RETURN_A 4
// #define RETURN_B 5
// #define RETURN_C 6
// #define RETURN_D 7

#define insertA 0
#define insertB 1
#define insertC 2
#define insertD 3


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "SPI.h"
#include "config.h"


void Init_relays();
void send_to(uint8_t CH, uint8_t to);
void return_from(uint8_t CH, uint8_t from);
void set_relays();
void reset_relays();

extern uint8_t channels[13];