/*
---------------------------------
DISPLAY Module for 8x312+4 project
---------------------------------
*/


#include <avr/io.h>
#include "config.h"
#include "st7735.h"

uint8_t SQlayout[8][4][2];

void drawBaseLayout();
void drawSquare_Char(uint8_t channel, uint8_t slot, uint8_t InsertCHAR, uint16_t color);
void drawSquare(uint8_t channel, uint8_t slot, uint16_t color);


