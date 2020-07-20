/*
---------------------------------
CONTROL Module for 8x312+4 project
---------------------------------
*/

#include "8x312_control.h"



void Init_8x312() {
  UART0_Init(MYUBRR);
  UART0_puts("UART initialized\r\n");

  timer2_Init(254); //tick @ 30Hz
  
  Init_st7735_SPI();
  fillScreen(BG_COLOR);
  
  Init_rotEnc();

  Init_buttons();
  last_buttons = 0x00;

  //Init_relays();

  SQselected = 0x00;
  SelectActive = 0x00;

  DDRC |= (1<<PC2); //test LED
}



/*
-------------------------------------------
DISPLAY
-------------------------------------------*/
void updateBaseLayout() {
  drawBaseLayout();
}


void updateScreen() {
  if (timer_update == 1) {
    timer_update = 0;
    drawInsertState();
  } 
}


void updateLEDs() {
  if (update_led == 1) {
    drawLEDs();
  }
}


void SQselect(uint8_t Square, uint8_t active) {
  SQselected = Square;
  SelectActive = active;
}


void setInsertState(uint8_t Square, uint8_t ch, uint8_t state) {
  insert_states[Square][0] = ch + 0x41; // 0x41=A
  insert_states[Square][1] = state; // GRAY/RED/GREEN
}


void drawInsertState() {
  uint16_t SQcolor;
  for (int i=0; i<32; i++) {
    switch (insert_states[i][1]) {
    case 0: SQcolor = DARKGRAY; break; //inactive
    case 1: SQcolor = RED; break; //OUT
    case 2: SQcolor = GREEN; break; //IN
    default: SQcolor = DARKGRAY; break;
    }
    
    if (SQselected == i) { // check if SQ is selected
      if ((timer_flash == 1) && (SelectActive == 0)) { //if flash AND not selection active
        fillRect(SQ[i][0],SQ[i][1],SQ[i][0]+12,SQ[i][1]+13,LIGHTBLUE);
        drawChar(insert_states[i][0],SQ[i][0]+3,SQ[i][1]+4,TEXT_COLOR,LIGHTBLUE);
      }
      else if (SelectActive == 1) { // if selection active = no flashing
        fillRect(SQ[i][0],SQ[i][1],SQ[i][0]+12,SQ[i][1]+13,BLUE);
        drawChar(insert_states[i][0],SQ[i][0]+3,SQ[i][1]+4,TEXT_COLOR,BLUE);
      }
      else { //Flash-effect off
        fillRect(SQ[i][0],SQ[i][1],SQ[i][0]+12,SQ[i][1]+13,SQcolor);
        drawChar(insert_states[i][0],SQ[i][0]+3,SQ[i][1]+4,TEXT_COLOR,SQcolor);
      }
    }
    else { // SQ not selected
      fillRect(SQ[i][0],SQ[i][1],SQ[i][0]+12,SQ[i][1]+13,SQcolor);
      drawChar(insert_states[i][0],SQ[i][0]+3,SQ[i][1]+4,TEXT_COLOR,SQcolor);
    }
  }
}


void setLED(uint8_t LED, uint8_t state) {
  LED_states[LED] = state;
}


// draw LEDs based on led_states[]
void drawLEDs() {
  uint16_t LEDcolor;
  for (int i=0; i<8; i++) {
    switch (LED_states[i]) {
    case 0: LEDcolor = LIGHTGRAY; break;
    case 1: LEDcolor = GREEN; break;
    case 2: LEDcolor = YELLOW; break;
    case 3: LEDcolor = RED; break;
    default: LEDcolor = LIGHTGRAY; break;
    }
    fillRect(99,37+(15*i),116,44+(15*i),LEDcolor);
  }
}




/*
-------------------------------------------
            BUTTONS + ENCODER
-------------------------------------------*/
// void check_buttons() {
//   if (update_buttons) {
//     uint8_t new_buttons = read_buttons(); // 0bDCBA00E0
    
//     if (new_buttons != last_buttons) { // if new button push detected
//       uint8_t new_state = (old_insert_state ^= new_buttons); // flip bit in old state - LATCHING
//       selectState(new_state); //update selection state
//       insertInOut(new_state); //update insert bypass state
//       setButtonLEDs(new_state); //set button LEDs accordingly
//       old_insert_state = new_state;
//       last_buttons = new_buttons;
//     }
//   }
// }


void check_buttons() {
  if (update_buttons) {
    eventOccured = NILEVENT;
    uint8_t new_buttons = scanButtons(); // scan for button pushes
    if (new_buttons != last_buttons) {   // if new push detected
      uint8_t new_state = (old_insert_state ^= new_buttons); // flip bit in old state - LATCHING
      
      if (new_buttons & (1<<0)) { // 0b000DCBAE
        eventOccured = EncBut;
        stateEval((event)eventOccured); // Update current state based on what event occured
        bob("State: ", currentState);
      }
    
      insertInOut((new_state>>1)); // 0b0000DCBA
      setButtonLEDs((new_state>>1));

      old_insert_state = new_state;
      last_buttons = new_buttons;
    }

    // ROTARY ENCODER INTERRUPT FLAGS
    if (interrupt_0_flag) {
      eventOccured = EncP;
      stateEval((event)eventOccured); // Update current state based on what event occured
      bob("SQselected: ", SQselected);
      bob("InsSelected: ", InsSelected);
      interrupt_0_flag = 0;
    }
    if (interrupt_1_flag) {
      eventOccured = EncM;
      stateEval((event)eventOccured); // Update current state based on what event occured
      bob("SQselected: ", SQselected);
      bob("InsSelected: ", InsSelected);
      interrupt_1_flag = 0;
    }

    update_buttons = 0;
  }
}


// Bypass buttons
void insertInOut(uint8_t buttons) { // 0b0000DCBA 
  for (int i=0; i<32; i++ ) {
    if (insert_states[i][0] == 0x41) { // Insert A
      insert_states[i][1] = ((buttons>>0) & 0x01)+1; // 1=OUT, 2=IN
    }
    if (insert_states[i][0] == 0x42) { // Insert B
      insert_states[i][1] = ((buttons>>1) & 0x01)+1; // 1=OUT, 2=IN
    }
    if (insert_states[i][0] == 0x43) { // Insert C
      insert_states[i][1] = ((buttons>>2) & 0x01)+1; // 1=OUT, 2=IN
    }
    if (insert_states[i][0] == 0x44) { // Insert D
      insert_states[i][1] = ((buttons>>3) & 0x01)+1; // 1=OUT, 2=IN
    }
  }
}


// void selectState(uint8_t buttons) {  // 0bDCBA00E0
//   uint8_t state = ((buttons>>1) & 0x01);  // 0b00000010
//   SQselect(2,state);
// }



/*
-------------------------------------------
STATE MACHINE ACTIONS
-------------------------------------------*/
// action function definitions
void noaction() {
}


void incSQ() {
  if (SQselected == 31) {
    SQselected = 0;
  }
  else {
    SQselected++;
  }
  SQselect(SQselected,0);
}


void decSQ() {
  if (SQselected == 0) {
    SQselected = 31;
  }
  else {
    SQselected--;
  }
  SQselect(SQselected,0);
}


void selActive() {
  // SelectActive = 1;
  SQselect(SQselected,1);
}


void incIns() {
  if (InsSelected == 4) {
    InsSelected = 0;
  }
  else {
    InsSelected++;
  }
  setInsertState(SQselected,InsSelected,2);
}


void decIns() {
  if (InsSelected == 0) {
    InsSelected = 4;
  }
  else {
    InsSelected--;
  }
  setInsertState(SQselected,InsSelected,2);
}


void confirmIns() {
  // SelectActive = 0;
  SQselect(SQselected+1,0);
}




/*
-------------------------------------------
TIMER SERVICE ROUTINE
-------------------------------------------*/

ISR(TIMER2_COMPA_vect) {
  //Screen refresh rate
  if (update_counter == 5) {
    timer_update = 1;
    update_counter = 0;
  }
  else {
    update_counter++;
  }

  //Flashing cursor speed
  if (flash_counter == 15) {
    if (timer_flash == 0) {
      timer_flash = 1;
      PORTC |= (1<<PC2);
    }
    else {
      timer_flash = 0;
      PORTC &=~ (1<<PC2);
    }
    flash_counter = 0;
  }
  else {
    flash_counter++;
  }

  //Button update
  if (buttons_counter == 10) {
    update_buttons = 1;
    buttons_counter = 0;
  }
  else {
    buttons_counter++;
  }

  //LED refresh rate
  if (led_counter == 20) {
    update_led = 1;
    led_counter = 0;
  }
  else {
    led_counter++;
  }
}