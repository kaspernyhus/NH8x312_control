/*
---------------------------------
CONTROL Module for 8x312+4 project
---------------------------------
*/

#include "8x312_control.h"



/*
-------------------------------------------
PRINTF DEBUGING
-------------------------------------------*/
void print_SLOTactive() {
  for (int Channel=0; Channel<8; Channel++) {
    for (int Slot=0; Slot<4; Slot++) {
      char string[12];
      sprintf(string,"CH%d/Slot%d: ",Channel,Slot);
      UART0_printBits(string, SLOTactive[Channel][Slot],8);
    }
  }
}
void print_InsertStates() {
  UART0_puts("---------InsertStates---------\r\n");
  for (int Channel=0; Channel<8; Channel++) {
    for (int Slot=0; Slot<4; Slot++) {
      char string[12];
      sprintf(string,"CH%d/Slot%d: \r\n",Channel,Slot);
      UART0_puts(string);
      UART0_printBits("Insert: ", InsertStates[Channel].Slots[Slot].Insert,4);
      UART0_printBits("State:  ", InsertStates[Channel].Slots[Slot].State,4);
      UART0_puts("------------------\r\n");
    }
  }
}
void print_channel_routing() {
  static int first = 1;
  if (first) {
    UART0_puts("CIE      CH1      CH2      CH3      CH4      CH5      CH6      CH7      CH8      A        B        C        D\r\n");
    UART0_puts("--------------------------------------------------------------------------------------------------------------------\r\n");
    first = 0;
  }
  
  for (int i= 0; i<13; i++) {
    UART0_printBits_nonewline("", channel_routing[i],8);
    UART0_puts(" ");
  }
  UART0_puts("\r\n");
  
  // UART0_puts("------------------RELAY ROUTING------------------\r\n");
  
  // for (int Channel=0; Channel<13; Channel++) {
  //   char string[12];
  //   if (Channel == 0) {
  //     sprintf(string,"CIE \r\n");
  //   }
  //   else if (Channel<9) {
  //     sprintf(string,"CH%d \r\n",Channel);
  //   }
  //   else if (Channel == 9) {
  //     sprintf(string,"A \r\n");
  //   }
  //   else if (Channel == 10) {
  //     sprintf(string,"B \r\n");
  //   }
  //   else if (Channel == 11) {
  //     sprintf(string,"C \r\n");
  //   }
  //   else if (Channel == 12) {
  //     sprintf(string,"D \r\n");
  //   }
    
  //   UART0_puts(string);
  //   UART0_printBits("", channel_routing[Channel],8);
  //   UART0_puts("------------------\r\n");
  // }
}
void stopWatch_Start(char string[]) {
  UART0_puts(string);
  StopWatch_timer = 0;
}
void stopWatch_End() {
  uint16_t time = StopWatch_timer;
  bob("StopWatch (ms): ", time);
}
//-------------------------------------------//





/*
-------------------------------------------
INITIALIZATION
-------------------------------------------*/
volatile struct channel InsertStates[8] = {0x00};


void Init_SLOTactivation() { // Make first slot active for each channel 
  for (int i=0; i<8; i++) {
    SLOTactive[i][0] = 1;
    for (int j=1; j<4; j++) {
      SLOTactive[i][j] = 0;
    }
  }
}


void Reset_InsertStates() {
  for (int i=0; i<8; i++) {
    InsertStates[i].CH_enable = 0;
    for (int j=0; j<4; j++) {
      InsertStates[i].Slots[j].Insert = 0;
      InsertStates[i].Slots[j].State = 0;
    }
  }
}


void Init_8x312() {
  UART0_Init(MYUBRR);
  UART0_puts("UART initialized\r\n");

  timer2_Init(249); //tick @ 1ms
  
  Init_st7735_SPI();
  fillScreen(BG_COLOR);
  
  Init_RotEnc();
  RotaryAttachOnCW(EncoderCW);
  RotaryAttachOnCCW(EncoderCCW);

  Init_buttons();
  setButtonLEDs(0x00);
  last_buttons = 0x00;
  encButLongPress = 0;

  Init_relays();
  //print_channel_routing();

  SelectActive = 0;
  CHselected = 0;
  SLOTselected = 0;
  InsSelected = 0;
  
  Init_SLOTactivation();
}



/*
-------------------------------------------
DISPLAY UPDATES
-------------------------------------------*/
void updateBaseLayout() {
  drawBaseLayout();
  drawInactiveSquares();
}


void updateScreen() {
  if (update_screen_timer == 1) {
    update_screen_timer = 0;
    //stopWatch_Start("----- Update Screen ------\r\n");
    drawInsertStates();
    
    
    // Update inactive squares only every 1000ms
    // if (update_basescreen_counter == 4) {
    //   drawInactiveSquares();
    //   update_basescreen_counter = 0;
    // } 
    // else {
    //   update_basescreen_counter++;
    // }
    //stopWatch_End();
  } 
}
//-------------------------------------------//



uint8_t checkNextSlot(uint8_t Channel, uint8_t Slot) {
  if (Slot < 3) { // if not last slot
    return SLOTactive[Channel][Slot+1]; // activation state of next slot
  }
  return 0;
}


uint8_t checkPreviousSlot(uint8_t Channel, uint8_t Slot) {
  if (Slot > 0) { // if not first slot
    return SLOTactive[Channel][Slot-1]; // activation state of previous slot
  }
  return 0;
}


void activateNextSlot(uint8_t Channel, uint8_t Slot) {
  if (Slot < 3) {
    SLOTactive[Channel][Slot+1] = 1;
  }
  drawInactiveSquares();
}

void deactivateNextSlot(uint8_t Channel, uint8_t Slot) {
  if (Slot < 3) {
    SLOTactive[Channel][Slot+1] = 0;
  }
  drawInactiveSquares();
}


void clearOldInsert(uint8_t currentChannel, uint8_t currentSlot, uint8_t NewInsert) {
  for (int channel=0; channel<8; channel++) {
    for (int slot=0; slot<4;slot++) {
      if (InsertStates[channel].Slots[slot].Insert == NewInsert) { // if insert used elsewhere
        if (channel != currentChannel) {
          InsertStates[channel].Slots[slot].Insert = 0; // 0=OFF
          InsertStates[channel].Slots[slot].State = 0;  // 0=unused
          deactivateNextSlot(channel,slot);
        }
      }
    }
  }
}


void setNewInsertState(uint8_t Channel, uint8_t Slot, uint8_t Insert, uint8_t State) {
  InsertStates[Channel].CH_enable = 1;                 // 0=ALL-INSERT-BYPASS, 1=IN
  InsertStates[Channel].Slots[Slot].Insert = Insert;   // 0=OFF, 1=A, 2=B, 3=C, 4=D
  
  if (Insert == 0) {
    InsertStates[Channel].Slots[Slot].State = 0;       // 0=unused, 1=OUT, 2=IN
    deactivateNextSlot(Channel,Slot);
  }
  else {
    InsertStates[Channel].Slots[Slot].State = State;   // 0=unused, 1=OUT, 2=IN
    activateNextSlot(Channel, Slot);
  }
}


uint8_t getInsertChar(uint8_t Insert) {
  if (Insert != 0x00) {
    return 0x40 + Insert; // A/B/C/D
  }
  return 0x20; // space
}


void drawInactiveSquares() {
  for (int channel=0; channel<8; channel++) {   // Channel 0-7
    for (int slot=0; slot<4; slot++) {          // Channel slot 0-3
      if (SLOTactive[channel][slot] == 0) { // SQ inactive
        drawSquare(channel,slot,DARKERGRAY);
      }
    }
  }  
}


void drawInsertStates() {
  uint16_t SQcolor;

  for (int channel=0; channel<8; channel++) {   // Channel 0-7
    for (int slot=0; slot<4; slot++) {          // Channel slot 0-3
      switch (InsertStates[channel].Slots[slot].State) {
        case 0: SQcolor = DARKGRAY; break; //inactive
        case 1: SQcolor = RED; break;      //OUT
        case 2: SQcolor = GREEN; break;    //IN
        default: SQcolor = DARKGRAY; break;
      }
    
      uint8_t InsertCHAR = getInsertChar(InsertStates[channel].Slots[slot].Insert);

      if (channel == CHselected && slot == SLOTselected) { // check if this correspondant square on the display is selected
        if ((timer_flash == 1) && (SelectActive == 0)) { //if flash AND not selection active
          drawSquare_Char(channel,slot,InsertCHAR,LIGHTBLUE);
        }
        else if (SelectActive == 1) { // if selection active = no flashing
          drawSquare_Char(channel,slot,InsertCHAR,BLUE);
        }
        else { //Flash-effect off
          drawSquare_Char(channel,slot,InsertCHAR,SQcolor);
        }
      }
      else if (SLOTactive[channel][slot] == 0) { // SQ inactive
        //drawSquare_Char(channel,slot,InsertCHAR,DARKERGRAY); //Do nothing
      }
      else { // SQ not selected
        drawSquare_Char(channel,slot,InsertCHAR,SQcolor);
      }

    }  
  }
}




/*
-------------------------------------------
                SIG LEDs
-------------------------------------------*/
void updateLEDs() {
  if (update_led == 1) {
    drawLEDs();
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
                RELAYs
-------------------------------------------*/

void setAllRelays() {
  uint8_t insert_1 = 0x00;
  uint8_t insert_2 = 0x00;
  uint8_t insert_3 = 0x00;
  uint8_t insert_4 = 0x00;

  for (int Channel=0; Channel<8; Channel++) {             // CH 1-8
    if (InsertStates[Channel].Slots[0].Insert != 0) {     // if an insert is assigned to first slot
      channel_insert_enable(Channel);                     // CIE - Channel Insert Enable
      
      // Insert slot 1
      insert_1 = InsertStates[Channel].Slots[0].Insert-1; // A:0x00 // B:0x01 / C:0x02 / D:0x03
      send_to(Channel, insert_1);
      
      // Insert slot 2
      if (InsertStates[Channel].Slots[1].Insert != 0) {   // if insert assigned to slot 2
        insert_2 = InsertStates[Channel].Slots[1].Insert-1;
        send_to(8+insert_1, insert_2);                    // send insert 1 to insert 2

        // Insert slot 3
        if (InsertStates[Channel].Slots[2].Insert != 0) { // if insert assigned to slot 3
          insert_3 = InsertStates[Channel].Slots[2].Insert-1;
          send_to(8+insert_2, insert_3);                  // send insert 2 to insert 3
        
          // Insert slot 4
          if (InsertStates[Channel].Slots[3].Insert != 0) { // if insert assigned to slot 4
            insert_4 = InsertStates[Channel].Slots[3].Insert-1;
            send_to(8+insert_3, insert_4);                // send insert 3 to insert 4
            
            // return from insert 4
            return_from(Channel, insert_4);
          }
          // return from insert 3
          else {
            return_from(Channel, insert_3);
          }
        }
        // return from insert 2
        else {
          return_from(Channel, insert_2);
        }
      }
      // return from insert 1
      else {
        return_from(Channel, insert_1);
      }
    }
    else {
      channel_insert_disable(Channel); // if no insert assignet to slot 1 -> disable channel inserts
    }
  }
  update_relay_states();
}




/*
-------------------------------------------
            BUTTONS + ENCODER
-------------------------------------------*/
void resetFlash() {
  timer_flash = 1;
  flash_counter = 0;
}

void EncoderCW() {
  resetFlash();
  eventOccured = EncP;
  stateEval((event)eventOccured); // Update current state based on what event occured
  // bob("CHselected: ", CHselected);
  // bob("SLOTselected: ", SLOTselected);
}

void EncoderCCW() {
  resetFlash();
  eventOccured = EncM;
  stateEval((event)eventOccured); // Update current state based on what event occured
  // bob("CHselected: ", CHselected);
  // bob("SLOTselected: ", SLOTselected);
}

void check_buttons() {
  if (update_buttons) {
    //--------------------//
    //   Bypass buttons   //
    //--------------------//
    uint8_t current_LED_states = get_current_button_LED_stated();
    uint8_t new_buttons = scanButtons(current_LED_states); // scan for new button pushes
    if (new_buttons != last_buttons) {   // if new push detected
      uint8_t new_state = (current_LED_states ^= new_buttons); // flip bit in old state - LATCHING
      insertInOut((new_state));
      last_buttons = new_buttons;
    }

    //--------------------//
    //   Encoder button   //
    //--------------------//
    uint8_t newEncBut = read_enc_button();
    if (newEncBut != lastEncBut) {
      if (newEncBut == 0x01) {
        eventOccured = EncBut;
        stateEval((event)eventOccured);
        lastEncBut = 0x01;
        encButLongPress = 1;
      }
      else {
        lastEncBut = 0x00;
      }
    }
    update_buttons = 0;
  }
}

void changeInsertState(uint8_t Insert, uint8_t State) { // 1=OUT, 2=IN
  for (int Channel=0; Channel<8; Channel++) {
    for (int Slot=0; Slot<4; Slot++) {
      if (InsertStates[Channel].Slots[Slot].Insert == Insert) { // find where that insert is inserted - change state
        InsertStates[Channel].Slots[Slot].State = State;
      }
    }
  }
}


// Bypass buttons
void insertInOut(uint8_t buttons) { // 0b0000DCBA 
  changeInsertState(1,((buttons>>0) & 0x01)+1); // Insert A
  changeInsertState(2,((buttons>>1) & 0x01)+1); // Insert B
  changeInsertState(3,((buttons>>2) & 0x01)+1); // Insert C
  changeInsertState(4,((buttons>>3) & 0x01)+1); // Insert D
}

uint8_t get_current_button_LED_stated() {
  uint8_t state = 0;
  uint8_t states = 0;
  for (int channel=0; channel<8; channel++) {
    for (int slot=0; slot<4;slot++) {
      if (InsertStates[channel].Slots[slot].Insert == 0x01) {      // if insert = A
          state = InsertStates[channel].Slots[slot].State>>1;      // active insert = 0b00000010
          states |= state<<0; 
      }
      else if (InsertStates[channel].Slots[slot].Insert == 0x02) { // if insert = B
          state = InsertStates[channel].Slots[slot].State>>1;      // active insert = 0b00000010
          states |= state<<1; 
      }
      else if (InsertStates[channel].Slots[slot].Insert == 0x03) { // if insert = C
          state = InsertStates[channel].Slots[slot].State>>1;      // active insert = 0b00000010
          states |= state<<2; 
      }
      else if (InsertStates[channel].Slots[slot].Insert == 0x04) { // if insert = D
          state = InsertStates[channel].Slots[slot].State>>1;      // active insert = 0b00000010
          states |= state<<3; 
      }
    }
  }
  return states;
}


/*
-------------------------------------------
STATE MACHINE ACTIONS
-------------------------------------------*/
// action function definitions
void noaction() {
}

void incSQ() {
  uint8_t nextSlotActive = checkNextSlot(CHselected,SLOTselected);

  if (CHselected == 7 && SLOTselected == 3) {
    CHselected = 0;
    SLOTselected = 0;
  }
  else if (SLOTselected == 3) {
    CHselected++;
    SLOTselected = 0;
  }
  else if (nextSlotActive) {
    SLOTselected++;
  }
  else if (CHselected == 7){
    CHselected = 0;
    SLOTselected = 0;
  }
  else {
    CHselected++;
    SLOTselected = 0;
  }
}


void decSQ() {
  uint8_t previousSlotActive = checkPreviousSlot(CHselected,SLOTselected);

  if (previousSlotActive) {
    SLOTselected--;
  }

  else if (CHselected > 0) {           // if not first channel
    if (SLOTactive[CHselected-1][3]) { // and last slot of previous channel is active
      CHselected--;
      SLOTselected = 3;
    }
    else if (SLOTactive[CHselected-1][2]) {
      CHselected--;
      SLOTselected = 2;
    }
    else if (SLOTactive[CHselected-1][1]) {
      CHselected--;
      SLOTselected = 1;
    }
    else {
      CHselected--;
      SLOTselected = 0;
    }
  }
  else { // channel == 0
    CHselected = 7;
    SLOTselected = 0;

    if (SLOTactive[7][3]) {
      SLOTselected = 3;
    }
    else if (SLOTactive[7][2]) {
      SLOTselected = 2;
    }
    else if (SLOTactive[7][1]) {
      SLOTselected = 1;
    }
  }
}


void selActive() {
  SelectActive = 1;
  encButLongPress = 0;
}


void incIns() {
  if (InsSelected == 4) {
    InsSelected = 0;
  }
  else {
    InsSelected++;
  }
  setNewInsertState(CHselected,SLOTselected,InsSelected,2);
}


void decIns() {
  if (InsSelected == 0) {
    InsSelected = 4;
  }
  else {
    InsSelected--;
  }
  setNewInsertState(CHselected,SLOTselected,InsSelected,2);
}


void confirmIns() {
  SelectActive = 0;
  
  clearOldInsert(CHselected,SLOTselected,InsSelected);

  incSQ(); // increment selection by 1
  encButLongPress = 0;

  setAllRelays();
  print_channel_routing();
}


void resetInserts() {
  Init_SLOTactivation();
  Reset_InsertStates();

  SelectActive = 0;
  CHselected = 0;
  SLOTselected = 0;
  InsSelected = 0;

  drawInsertStates();
  drawInactiveSquares();

  reset_relays();
  setAllRelays();
}



/*
-------------------------------------------
TIMER SERVICE ROUTINE
-------------------------------------------*/

ISR(TIMER2_COMPA_vect) {
  // StopWatch
  StopWatch_timer++;

  // Screen refresh rate
  if (update_screen_counter == SCREEN_REFRESH_RATE-1) {
    update_screen_timer = 1;
    update_screen_counter = 0;
  }
  else {
    update_screen_counter++;
  }

  // Flashing cursor speed
  if (flash_counter == FLASH_RATE-1) {
    if (timer_flash == 0) {
      timer_flash = 1;
    }
    else {
      timer_flash = 0;
    }
    flash_counter = 0;
  }
  else {
    flash_counter++;
  }

  // Button update
  if (button_scan_counter == BUTTON_SCAN_RATE-1) {
    update_buttons = 1;
    button_scan_counter = 0;
  }
  else {
    button_scan_counter++;
  }

  // Encoder button long press
  if (encButLongPress > 0) {
    if (encButLongPress == ENC_BUT_LONG_PRESS-1) {
      if (read_enc_button()) { // if button still pressed after a while
        eventOccured = EncButLong;
        stateEval((event)eventOccured);
        encButLongPress = 0;
      }
      else {
        encButLongPress = 0; // if not pressed reset counter
      }
    }
    else {
      encButLongPress++;
    }
  }


  // LED refresh rate
  if (led_counter == SCREEN_REFRESH_RATE-1) {
    update_led = 1;
    led_counter = 0;
  }
  else {
    led_counter++;
  }
}




