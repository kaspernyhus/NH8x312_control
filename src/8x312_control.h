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
#include "RotEnc.h"


#define BAUD 9600
#define MYUBRR F_CPU/8/BAUD-1 //full dublex


void Init_8x312();

void updateBaseLayout();
void updateScreen();
void updateLEDs();

void SQselect(uint8_t Square, uint8_t active);

void setInsertState(uint8_t Square, uint8_t ch, uint8_t state);

void drawInsertState();
void drawInactiveSquares();
void setLED(uint8_t LED, uint8_t state);
void drawLEDs();

void EncoderCW();
void EncoderCCW();
void check_buttons();
void insertInOut(uint8_t buttons);
void selectState(uint8_t buttons);




//LAYOUT COORDINATES
extern uint8_t SQ[32][2]; // (x,y) for the 32 insert squares



extern uint8_t SQlayout[8][4][2];




//---------------------------------------------------//
struct Slot
{
  uint8_t Insert;
  uint8_t State;
};

struct channel
{
  uint8_t CH_enable; // CIE - Channel Insert Enable
  struct Slot Slots[4];
};

volatile struct channel InsertStates[8];


volatile uint8_t InsSelected;
volatile uint8_t SelectActive;

volatile uint8_t CHselected;
volatile uint8_t SLOTselected;
volatile uint8_t SLOTactive[8][4];


void setNewInsertState(uint8_t Channel, uint8_t Slot, uint8_t Insert, uint8_t State);
void drawInsertStates();
//---------------------------------------------------//

// System variables
extern uint8_t LED_states[8];
volatile uint8_t last_buttons;
volatile uint8_t lastEncBut;
volatile uint8_t old_insert_state;


//Timer
volatile uint8_t tick;
volatile uint16_t flash_counter;
volatile uint8_t timer_flash;
volatile uint16_t update_screen_counter;
volatile uint8_t update_screen_timer;
volatile uint16_t update_basescreen_counter;
volatile uint8_t update_basescreen_timer;
volatile uint16_t buttons_counter;
volatile uint8_t update_buttons;
volatile uint16_t led_counter;
volatile uint8_t update_led;
volatile uint16_t StopWatch_timer;