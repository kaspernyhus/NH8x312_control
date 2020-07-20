/*
---------------------------------
Module for ST7735 TFT Display
ARDUINO NANO
---------------------------------

TFT | Function  | AVR
------------------------
1   | Backlight | 5v     
2   | MISO      | 
3   | SCK       | PB5
4   | MOSI      | PB3
5   | TFT sel   | PB2 (CS)
6   | SD sel    | 
7   | D/C       | PB1 (Data/Command) (0=command, 1=data)
8   | Reset     | PB0
9   | VCC       | 5v
10  | GND       | GND
-------------------------
*/

#include "st7735.h"

void startWriteST7735() {
  PORTB &=~ (1<<PB2); //set CS pin (TFT sel) LOW
}

void endWriteST7735() {
  PORTB |= (1<<PB2); //set CS pin (TFT sel) HIGH
}


void writeData(uint8_t data) {
  //PORTB |= (1<<PB1); //Data (assume HIGH)
  startWriteST7735();
  SPI_MasterTransmit(data);
  endWriteST7735();
}


void writeCommand(uint8_t cmd) {
  startWriteST7735();
  PORTB &=~ (1<<PB1); //0=Command
  SPI_MasterTransmit(cmd);
  PORTB |= (1<<PB1); //return pin to HIGH
  endWriteST7735();
}


void resetDisplay() {
  PORTB &=~ (1<<PB0);
  _delay_ms(1);
  PORTB |= (1<<PB0);
  _delay_ms(200);
}


void Init_st7735_SPI(){
  DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2); // OUTPUTS: Reset, D/C, CS

  SPI_init_master();    // initialize SPI

  resetDisplay();       // reset display controller
  writeCommand(SLPOUT); // take display out of sleep mode
  _delay_ms(150);       
  writeCommand(COLMOD); // select color mode:
  writeData(0x05);      // mode 5 = 16bit pixels (RGB565) - [RRRRR GGGGGG BBBBB]
  writeCommand(DISPON); // turn display on
}


// Send 16-bit color data to display. Number of pixels to apply the color to
void setColor(uint16_t color, unsigned int pixels) {   
  for (; pixels>0; pixels--) {
    // writeData(color >> 8);   //hi byte
    // writeData(color & 0xFF); //lo byte
    
    //OPTIMIZATION ??
    startWriteST7735();
    SPDR = (color >> 8);        // hi byte
    while(!(SPSR & (1<<SPIF))); // Wait for transmission complete
    SPDR = (color & 0xFF);      // lo byte
    while(!(SPSR & (1<<SPIF))); // Wait for transmission complete
    endWriteST7735();
  }
}


// Set ractangular active region - column range (x0,x1), row range (y0,y1)
void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  writeCommand(CASET);  //Column Address Set
  writeData(0x00);
  writeData(x0);    //x-start
  writeData(0x00);
  writeData(x1);    //x-end

  writeCommand(RASET);  //Row Address Set
  writeData(0x00);
  writeData(y0);    //y-start
  writeData(0x00);
  writeData(y1);    //y-end

  writeCommand(RAMWR); // write to display RAM memory
}


/*
-------------------------------------
              DRAWING
------------------------------------- */
void clearScreen() {
  fillScreen(BG_COLOR);
}


void fillScreen(uint16_t color) {
  fillRect(0,0,_width,_height,color);
}


// fill a rectangle
void fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color) {
  uint8_t width = x1-x0+1;  // rectangle width
  uint8_t height = y1-y0+1; // rectangle height
  setAddrWindow(x0,y0,x1,y1);
  setColor(color,width*height);
}


void drawPixel(uint8_t x, uint8_t y, uint16_t color) {
  setAddrWindow(x,y,x,y);
  setColor(color,1); //set color for 1 pixel
}


void drawFastHLine(uint8_t y, uint8_t x0, uint8_t x1, uint16_t color) {
  uint8_t width = x1-x0+1;
  setAddrWindow(x0,y,x1,y);
  setColor(color, width);
}

void drawFastVLine(uint8_t x, uint8_t y0, uint8_t y1, uint16_t color) {
  uint8_t height = y1-y0+1;
  setAddrWindow(x,y0,x,y1);
  setColor(color, height);
}

void drawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color) {
  drawFastHLine(y0,x0,x1,color);
  drawFastHLine(y1,x0,x1,color);
  drawFastVLine(x0,y0,y1,color);
  drawFastVLine(x1,y0,y1,color);
}


/*
-------------------------------------
          DRAWING FONT
------------------------------------- */

void drawChar(uint8_t ch, uint8_t x, uint8_t y, uint16_t color, uint16_t bg_color) { // ASCII ch
  uint16_t pixel;
  uint8_t row, bit, ch_data, mask = 0x80;

  setAddrWindow(x,y,x+6,y+6);

  for (row=0; row<7; row++) {
    ch_data = pgm_read_byte(&(FONT_CHARS[ch-32][row])); //Load CH (bitmap) data from program memory

    for (bit=1; bit<8; bit++) {
      if (((ch_data<<bit) & mask) == 0) {
        pixel = bg_color;}
      else {
        pixel = color;}

      writeData(pixel >> 8);   //hi byte
      writeData(pixel & 0xFF); //lo byte
    }
  } 
}


