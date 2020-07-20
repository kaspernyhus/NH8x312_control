/*
---------------------------------
CONTROL Module for 8x312+4 project
---------------------------------
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "config.h"
#include "buttons.h"
#include "8x312_display.h"
#include "8x312_relays.h"
#include "statedef.h"
#include "UART.h"
#include "rotEnc.h"


#define BAUD 9600
#define MYUBRR F_CPU/8/BAUD-1 //full dublex


void Init_8x312();

void updateBaseLayout();
void updateScreen();
void updateLEDs();

void SQselect(uint8_t Square, uint8_t active);

void setInsertState(uint8_t SQ, uint8_t ch, uint8_t state);

void drawInsertState();
void setLED(uint8_t LED, uint8_t state);
void drawLEDs();

void check_buttons();
void insertInOut(uint8_t buttons);
void selectState(uint8_t buttons);




//LAYOUT COORDINATES
extern uint8_t SQ[32][2]; // (x,y) for the 32 insert squares



// System variables
extern uint8_t insert_states[32][2]; // [CH insert SQ][0] = insert char (ASCII), [CH insert SQ][1] = insert OUT/IN (Inactive(0)/ OUT (Red(1)) / IN(Green(2)))
extern uint8_t LED_states[8];
volatile uint8_t last_buttons;
volatile uint8_t old_insert_state;


//Timer
volatile uint8_t tick;
volatile uint8_t flash_counter;
volatile uint8_t timer_flash;
volatile uint8_t update_counter;
volatile uint8_t timer_update;
volatile uint8_t buttons_counter;
volatile uint8_t update_buttons;
volatile uint8_t led_counter;
volatile uint8_t update_led;