/*
 -----------------
 Configuration header file
 -----------------
 
 NH312+4 Control
 ST7735 Display

-----------------------------
Arduino Nano 
CONNECTIONS
-----------------------------
SPI (TFT/LED/RELAYs/BUTTONS)
-----------------------------
PB5 => CLOCK
PB3 => DATA

PB2 => CS (TFT)
PB1 => D/C (TFT)
PB0 => TFT RESET

PD5 => BUTTON LATCH
PD6 => LED LATCH
PD7 => RELAYs LATCH
-----------------------------
BUTTONS
-----------------------------
PD4 => ButtonScanPin
PD2 => EncA
PD3 => EncB
-----------------------------
ADC
-----------------------------
PC0 (ADC0)    => CH1
PC1 (ADC1)    => CH2
PC2 (ADC2)    => CH3
PC3 (ADC3)    => CH4
PC4 (ADC4)    => CH5
PC5 (ADC5)    => CH6
pin19 (ADC6)  => CH7
pin22 (ADC7)  => CH8

-----------------------------
UNUSED
-----------------------------
PB4 (SPI MISO)
PB6
PB7
PD0 (RX)
PD1 (TX)
PC6 (RESET)

*/

#include <avr/io.h>


#define BG_COLOR 0x196F
#define TEXT_COLOR 0xFFFF
#define FLASH_RATE 999 




// ----------------- //
//    LAYOUT
// ----------------- //


// Color constants
#define BLACK 0x0000
#define BLUE 0x047E
#define LIGHTBLUE 0x05FE
#define RED 0xE8E4
#define GREEN 0x3DA9
#define LIME 0x07E0
#define CYAN0 x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define DARKGRAY 0xA514
#define LIGHTGRAY 0xBDD8


// ----------------- //
//    STATE MACHINE
// ----------------- //
extern uint8_t SQselected;
extern uint8_t InsSelected;
extern uint8_t SelectActive;
