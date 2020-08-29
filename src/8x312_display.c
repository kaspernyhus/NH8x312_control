/*
---------------------------------
DISPLAY Module for 8x312+4 project
---------------------------------
*/


#include "8x312_display.h"

/*
-------------------------------------------
              DISPLAY
-------------------------------------------
LAYOUT drawing
8x312+4.psd
------------------------------------------- */

void drawHeadline() {
  drawChar(0x4E,25,6,TEXT_COLOR,BG_COLOR); // N
  drawChar(0x48,34,6,TEXT_COLOR,BG_COLOR); // H
  drawChar(0x38,45,6,TEXT_COLOR,BG_COLOR); // 8
  drawChar(0x78,54,6,TEXT_COLOR,BG_COLOR); // x
  drawChar(0x33,63,6,TEXT_COLOR,BG_COLOR); // 3
  drawChar(0x31,70,6,TEXT_COLOR,BG_COLOR); // 1
  drawChar(0x32,78,6,TEXT_COLOR,BG_COLOR); // 2
  drawChar(0x2A,87,6,TEXT_COLOR,BG_COLOR); // +
  drawChar(0x34,95,6,TEXT_COLOR,BG_COLOR); // 4
}


void drawGrid_CH(uint16_t color) {
  //Horizontal lines
  drawFastHLine(19,5, 28, color);

  for (int i=0; i<9; i++) {
    drawFastHLine(33+(15*i),5, 28, color);
  }
  //Vertical lines
  drawFastVLine(5,19,153,color);
  drawFastVLine(28,19,153,color);

  //Text
  drawChar(0x43,9,23,TEXT_COLOR,BG_COLOR);   // C
  drawChar(0x48,18,23,TEXT_COLOR,BG_COLOR);  // H
  drawChar(0x31,13,38,TEXT_COLOR,BG_COLOR);  // 1
  drawChar(0x32,13,53,TEXT_COLOR,BG_COLOR);  // 2
  drawChar(0x33,13,68,TEXT_COLOR,BG_COLOR);  // 3
  drawChar(0x34,13,83,TEXT_COLOR,BG_COLOR);  // 4
  drawChar(0x35,13,98,TEXT_COLOR,BG_COLOR);  // 5
  drawChar(0x36,13,113,TEXT_COLOR,BG_COLOR); // 6
  drawChar(0x37,13,128,TEXT_COLOR,BG_COLOR); // 7
  drawChar(0x38,13,143,TEXT_COLOR,BG_COLOR); // 8
}


void drawGrid_Insert(uint16_t color) {
  //fill
  fillRect(32,33,88,153,DARKGRAY);

  //Horizontal lines
  drawFastHLine(19,32,88, color);
  for (int i=0; i<9; i++) {
    drawFastHLine(33+(i*15),32,88, color);
  }
  //Vertical lines long
  drawFastVLine(32,19,153,color);
  drawFastVLine(88,19,153,color);

  //short
  drawFastVLine(46,33,153,color);
  drawFastVLine(60,33,153,color);
  drawFastVLine(74,33,153,color);

  //TEXT
  drawChar(0x49,36,23,TEXT_COLOR,BG_COLOR); // I
  drawChar(0x4E,44,23,TEXT_COLOR,BG_COLOR); // N
  drawChar(0x53,53,23,TEXT_COLOR,BG_COLOR); // S
  drawChar(0x45,62,23,TEXT_COLOR,BG_COLOR); // E
  drawChar(0x52,70,23,TEXT_COLOR,BG_COLOR); // R
  drawChar(0x54,79,23,TEXT_COLOR,BG_COLOR); // T
}


void drawGrid_Sig(uint16_t color) {
  //fill
  fillRect(93,33,123,153,0xA514);

  //Horizontal lines
  drawFastHLine(19,92,123, color);

  for (int i=0; i<9; i++) {
    drawFastHLine(33+(i*15),92,123, color);
  }
  //Vertical lines long
  drawFastVLine(92,19,153,color);
  drawFastVLine(123,19,153,color);

  //8x LED slots
  for (int i=0; i<8; i++) {
    drawRect(98,36+(15*i),117,45+(15*i),DARKERGRAY);
    fillRect(99,37+(15*i),116,44+(15*i),LIGHTGRAY);
  }

  //Text
  drawChar(0x53,96,23,TEXT_COLOR,BG_COLOR);  // S
  drawChar(0x49,105,23,TEXT_COLOR,BG_COLOR); // I
  drawChar(0x47,113,23,TEXT_COLOR,BG_COLOR); // G
}


void drawBaseLayout() {
  drawHeadline();
  drawGrid_CH(LIGHTGRAY);
  drawGrid_Insert(LIGHTGRAY);
  drawGrid_Sig(LIGHTGRAY);
}



/*
-------------------------------------------
LED colors
-------------------------------------------*/
uint8_t LED_states[8] = {0x00};




/*
-------------------------------------------
LAYOUT COORDINATES
-------------------------------------------*/
uint8_t SQlayout[8][4][2] = {
  { {33,34}, {47,34}, {61,34}, {75,34} }, // CH1: { slot1{x,y}, slot2{x,y}, slot3{x,y}, slot4{x,y} } ...
  { {33,49}, {47,49}, {61,49}, {75,49} }, // CH2:
  { {33,64}, {47,64}, {61,64}, {75,64} },
  { {33,79}, {47,79}, {61,79}, {75,79} },
  { {33,94}, {47,94}, {61,94}, {75,94} },
  { {33,109}, {47,109}, {61,109}, {75,109} },
  { {33,124}, {47,124}, {61,124}, {75,124} },
  { {33,139}, {47,139}, {61,139}, {75,139} },
};


/*
-------------------------------------------
Insert Squares
-------------------------------------------*/
void drawSquare_Char(uint8_t channel, uint8_t slot, uint8_t InsertCHAR, uint16_t color) {
  fillRect(SQlayout[channel][slot][0],SQlayout[channel][slot][1],SQlayout[channel][slot][0]+12,SQlayout[channel][slot][1]+13,color);
  drawChar(InsertCHAR,SQlayout[channel][slot][0]+3,SQlayout[channel][slot][1]+4,TEXT_COLOR,color);
}

void drawSquare(uint8_t channel, uint8_t slot, uint16_t color) {
  fillRect(SQlayout[channel][slot][0],SQlayout[channel][slot][1],SQlayout[channel][slot][0]+12,SQlayout[channel][slot][1]+13,color);
}