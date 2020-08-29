/*
---------------------------------
Module for ST7735 TFT Display
ARDUINO NANO
---------------------------------

TFT | Function  | AVR
------------------------
1   | Backlight | 5v     
2   | MISO      | 
3   | SCK       | PB5 [D13]
4   | MOSI      | PB3 (SDA) [D11]
5   | TFT sel   | PB2 (CS)  [D10]
6   | SD sel    | 
7   | D/C       | PB0 (Data/Command) (0=command, 1=data) [D8]
8   | Reset     | PB1 [D9]
9   | VCC       | 5v
10  | GND       | GND
-------------------------

    Phisical pins - Top view
 ____________________________
| 9 10 10 nc nc nc 3 4 7 8 5 |
|----------------------------|
|                            |
|                            |
|                            |
|                            |


http://w8bh.net/avr/AvrTFT.pdf
*/


#define TFT_DC  0 // PB0 [D8]
#define TFT_RST 1 // PB1 [D9]
#define TFT_CS  2 // PB2 [D10]


/* 
---------------------------------
    ST7735 ROUTINES
--------------------------------- */
#define SWRESET 0x01  // software reset
#define SLPOUT  0x11   // sleep out
#define DISPOFF 0x28  // display off
#define DISPON  0x29   // display on
#define CASET   0x2A    // column address set
#define RASET   0x2B    // row address set
#define RAMWR   0x2C    // RAM write
#define MADCTL  0x36   // axis control
#define COLMOD  0x3A   // color mode

// 1.8" TFT display constants
#define _width 128
#define _height 160
#define XMAX _width-1
#define YMAX _height-1

// ---------------------------------

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "config.h"
#include "SPI.h"



void Init_st7735_SPI();

void clearScreen();
void fillScreen(uint16_t color);
void fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);
void drawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawFastHLine(uint8_t y, uint8_t x0, uint8_t x1, uint16_t color);
void drawFastVLine(uint8_t x, uint8_t y0, uint8_t y1, uint16_t color);
void drawChar(uint8_t ch, uint8_t x, uint8_t y, uint16_t color, uint16_t bg_color);



/*
---------------------------------
            FONT
--------------------------------- */
// Row Major Order
    
/*
A:
0 0 0 1 1 1 0 0 - 0x1C
0 0 1 1 0 1 1 0 - 0x36
0 1 1 0 0 0 1 1 - 0x63
0 1 1 0 0 0 1 1 - 0x63
0 1 1 1 1 1 1 1 - 0x7F
0 1 1 0 0 0 1 1 - 0x63
0 1 1 0 0 0 1 1 - 0x63
*/

static const uint8_t FONT_CHARS[96][7] PROGMEM = 
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // (space) 
    {},{},{},{},{},      // ASCII chars not used
    {},{},{},{},
    { 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00 }, // +
    {},{},{},{},{},
    { 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E }, // 0
    { 0x0C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E }, // 1
    { 0x3E, 0x63, 0x06, 0x1C, 0x30, 0x60, 0x7F }, // 2
    { 0x3E, 0x63, 0x03, 0x1E, 0x03, 0x63, 0x3E }, // 3
    { 0x0E, 0x1E, 0x36, 0x66, 0x7F, 0x06, 0x06 }, // 4
    { 0x7E, 0x60, 0x7E, 0x03, 0x03, 0x63, 0x3E }, // 5
    { 0x3E, 0x63, 0x60, 0x7E, 0x63, 0x63, 0x3E }, // 6
    { 0x7F, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18 }, // 7
    { 0x3E, 0x63, 0x63, 0x3E, 0x63, 0x63, 0x3E }, // 8
    { 0x3E, 0x63, 0x63, 0x3F, 0x03, 0x06, 0x1C }, // 9
    {},{},{},{},{},{},{},
    { 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63 }, // A
    { 0x7E, 0x63, 0x63, 0x7E, 0x63, 0x63, 0x7E }, // B
    { 0x3E, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3E }, // C
    { 0x7E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x7E }, // D
    { 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E }, // E
    { 0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60 }, // F
    { 0x3E, 0x63, 0x60, 0x67, 0x63, 0x63, 0x3E }, // G
    { 0x63, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x63 }, // H
    { 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E }, // I
    { 0x1F, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C }, // J
    { 0x63, 0x66, 0x6C, 0x78, 0x7C, 0x66, 0x63 }, // K
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7F }, // L
    { 0x41, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63 }, // M
    { 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63 }, // N
    { 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E }, // O
    { 0x7E, 0x63, 0x63, 0x7E, 0x60, 0x60, 0x60 }, // P
    { 0x3E, 0x63, 0x63, 0x63, 0x6F, 0x66, 0x3B }, // Q
    { 0x7E, 0x63, 0x63, 0x7E, 0x6C, 0x66, 0x63 }, // R
    { 0x3E, 0x63, 0x60, 0x3E, 0x03, 0x63, 0x3E }, // S
    { 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 }, // T
    {  }, // U
    {  }, // V
    {  }, // W
    {  }, // X
    {  }, // Y
    {  }, // Z
    {},{},{},{},{},{},
    {},{},{},{},{},{},{},{},{},{},
    {},{},{},{},{},{},{},{},{},{},
    {},{},{},
    { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63 }, // x



};

