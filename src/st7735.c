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

#include "st7735.h"


#define TFT_DC  0 // PB0 [D8]
#define TFT_RST 1 // PB1 [D9]
#define TFT_CS  2 // PB2 [D10]

#define ClearBit(x,y) x &= ~_BV(y) // equivalent to cbi(x,y)
#define SetBit(x,y) x |= _BV(y) // equivalent to sbi(x,y)



void startWriteST7735() {
  PORTB &=~ (1<<TFT_CS);  //set CS pin (TFT sel) LOW
}

void endWriteST7735() {
  PORTB |= (1<<TFT_CS);   //set CS pin (TFT sel) HIGH
}


void writeData(uint8_t data) {
  SPI_MasterTransmit(data);
}


void writeCommand(uint8_t cmd) {
  PORTB &=~ (1<<TFT_DC);  // 0=command, 1=data
  SPI_MasterTransmit(cmd);
  PORTB |= (1<<TFT_DC);   //return pin to HIGH
}


void setColor(uint16_t color, unsigned int pixels) {   
  startWriteST7735();
  for (; pixels>0; pixels--) {
    SPDR = (color >> 8);        // hi byte
    while(!(SPSR & (1<<SPIF))); // Wait for transmission complete
    SPDR = (color & 0xFF);      // lo byte
    while(!(SPSR & (1<<SPIF))); // Wait for transmission complete
  }
  endWriteST7735();
}


void hardwareReset() {
    ClearBit(PORTB, TFT_RST); 
    _delay_ms(1); // 1mS is enough
    SetBit(PORTB, TFT_RST);
    _delay_ms(150); // wait 150mS for reset to finish
}

void softwareReset() {
  writeCommand(SWRESET);
  _delay_ms(150);
}


void InitDisplay()
{
  startWriteST7735();
  
  //hardwareReset(); // initialize display controller
  softwareReset();
  
  writeCommand(SLPOUT); // take display out of sleep mode
  _delay_ms(150); // wait 150mS for TFT driver circuits
  writeCommand(COLMOD); // select color mode:
  writeData(0x05); // mode 5 = 16bit pixels (RGB565)
  writeCommand(DISPON); // turn display on!
  endWriteST7735();
}


void Init_st7735_SPI() {
    DDRB |= (1<<TFT_DC)|(1<<TFT_RST)|(1<<TFT_CS); // OUTPUTS: D/C, Reset, CS
    PORTB |= (1<<TFT_RST); // start with TFT reset line inactive high
    PORTB |= (1<<TFT_CS);  // deselect TFT CS
    Init_SPI_master();
    InitDisplay();
    clearScreen();
}


void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  writeCommand(CASET);      // set column range (x0,x1)
  writeData(0x00);
  writeData(x0);
  writeData(0x00);
  writeData(x1);
  writeCommand(RASET);      // set row range (y0,y1)
  writeData(0x00);
  writeData(y0);
  writeData(0x00);
  writeData(y1);
  writeCommand(RAMWR);
}


void clearScreen() {
  startWriteST7735();
    setAddrWindow(0, 0, XMAX, YMAX); // set window to entire display
    for (unsigned int i = 40960; i > 0; --i) // byte count = 128*160*2
    {
        SPDR = 0; // initiate transfer of 0x00
        while (!(SPSR & 0x80)); // wait for xfer to finish
    }
  endWriteST7735();
}



/*
-------------------------------------
              DRAWING
------------------------------------- */
void fillScreen(uint16_t color) {
  fillRect(0,0,_width,_height,color);
}


// fill a rectangle
void fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color) {
  startWriteST7735();
  uint8_t width = x1-x0+1;  // rectangle width
  uint8_t height = y1-y0+1; // rectangle height
  setAddrWindow(x0,y0,x1,y1);
  setColor(color,width*height);
  endWriteST7735();
}


void drawPixel(uint8_t x, uint8_t y, uint16_t color) {
  startWriteST7735();
  setAddrWindow(x,y,x,y);
  setColor(color,1); //set color for 1 pixel
  endWriteST7735();
}


void drawFastHLine(uint8_t y, uint8_t x0, uint8_t x1, uint16_t color) {
  startWriteST7735();
  uint8_t width = x1-x0+1;
  setAddrWindow(x0,y,x1,y);
  setColor(color, width);
  endWriteST7735();
}

void drawFastVLine(uint8_t x, uint8_t y0, uint8_t y1, uint16_t color) {
  startWriteST7735();
  uint8_t height = y1-y0+1;
  setAddrWindow(x,y0,x,y1);
  setColor(color, height);
  endWriteST7735();
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
  startWriteST7735();

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
  endWriteST7735();
}