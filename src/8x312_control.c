/*
---------------------------------
CONTROL Module for 8x312+4 project
---------------------------------
*/

#include "8x312_control.h"


uint8_t SQselected = 0;
volatile uint8_t SelectActive = 0;
volatile uint8_t InsSelected = 0;


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
  UART0_puts("------------------\r\n");
  for (int Channel=0; Channel<8; Channel++) {
    for (int Slot=0; Slot<4; Slot++) {
      char string[12];
      sprintf(string,"CH%d/Slot%d: \r\n",Channel,Slot);
      UART0_puts(string);
      UART0_printBits("Insert: ", InsertStates[Channel].Slots[Slot].Insert,4);
      UART0_printBits("State: ", InsertStates[Channel].Slots[Slot].State,4);
      UART0_puts("------------------\r\n");
    }
  }
}
void print_Channels() {
  UART0_puts("------------------\r\n");
  for (int Channel=0; Channel<12; Channel++) {
    char string[12];
    sprintf(string,"CH%d: \r\n",Channel);
    UART0_puts(string);
    UART0_printBits("Relays: ", channels[Channel],8);
    UART0_puts("------------------\r\n");
  }
}
//-------------------------------------------//





/*
-------------------------------------------
INITIALIZATION
-------------------------------------------*/
volatile struct channel InsertStates[8] = {0x00};


void Init_SLOTactivation() { // Make first slot for each channel active
  SLOTactive[8][4] = 0x00;
  for (int i=0; i<8; i++) {
    SLOTactive[i][0] = 1;
  }
}


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

  SelectActive = 0;
  CHselected = 0;
  SLOTselected = 0;
  
  Init_SLOTactivation();

  // DDRC |= (1<<PC2); //test LED
}



/*
-------------------------------------------
DISPLAY UPDATES
-------------------------------------------*/
void updateBaseLayout() {
  drawBaseLayout();
}


void updateScreen() {
  if (timer_update == 1) {
    timer_update = 0;
    drawInsertStates();
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
}

void deactivateNextSlot(uint8_t Channel, uint8_t Slot) {
  if (Slot < 3) {
    SLOTactive[Channel][Slot+1] = 0;
  }
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
    InsertStates[Channel].Slots[Slot].State = 0;
    deactivateNextSlot(Channel,Slot);
  }
  else {
    InsertStates[Channel].Slots[Slot].State = State;     // 0=unused, 1=OUT, 2=IN
    activateNextSlot(Channel, Slot);
  }
}


uint8_t getInsertChar(uint8_t Insert) {
  if (Insert != 0x00) {
    return 0x40 + Insert; // A/B/C/D
  }
  return 0x20; // space
}


void drawInsertStates() {
  uint16_t SQcolor;

  for (int channel=0; channel<8; channel++) {   // Channel 0-7
    for (int slot=0; slot<4; slot++) {           // Channel slot 0-3
      switch (InsertStates[channel].Slots[slot].State) {
        case 0: SQcolor = DARKGRAY; break; //inactive
        case 1: SQcolor = RED; break;      //OUT
        case 2: SQcolor = GREEN; break;    //IN
        default: SQcolor = DARKGRAY; break;
      }
    
      uint8_t InsertCHAR = getInsertChar(InsertStates[channel].Slots[slot].Insert);

      if (channel == CHselected && slot == SLOTselected) { // check if this correspondant square on the display is selected
        if ((timer_flash == 1) && (SelectActive == 0)) { //if flash AND not selection active
          drawSquare(channel,slot,InsertCHAR,LIGHTBLUE);
        }
        else if (SelectActive == 1) { // if selection active = no flashing
          drawSquare(channel,slot,InsertCHAR,BLUE);
        }
        else { //Flash-effect off
          drawSquare(channel,slot,InsertCHAR,SQcolor);
        }
      }
      else if (SLOTactive[channel][slot] == 0) { // SQ active
        drawSquare(channel,slot,InsertCHAR,DARKERGRAY);
      }
      else { // SQ not selected
        drawSquare(channel,slot,InsertCHAR,SQcolor);
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
  
  for (int Channel=0; Channel<8; Channel++) { // CH1-8
    for (int Slot=0; Slot<4; Slot++) { // Slot 1-4 per channel
      if (InsertStates[Channel].Slots[Slot].Insert != 0) { // if an insert is assigned
        uint8_t CIE = InsertStates[Channel].CH_enable;
        channels[0] = (CIE<<Channel);
        uint8_t Insert = InsertStates[Channel].Slots[Slot].Insert;
        // Send
        channels[Channel+1] = (1<<(Insert-1));
        // Return
        channels[Channel+1] |= (1<<((Insert-1)+4)); //setting same return as send as default
      }
    }
  }
  
  //set_relays();
}





/*
-------------------------------------------
            BUTTONS + ENCODER
-------------------------------------------*/
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
      bob("CHselected: ", CHselected);
      bob("SLOTselected: ", SLOTselected);
      interrupt_0_flag = 0;
    }
    if (interrupt_1_flag) {
      eventOccured = EncM;
      stateEval((event)eventOccured); // Update current state based on what event occured
      bob("CHselected: ", CHselected);
      bob("SLOTselected: ", SLOTselected);
      interrupt_1_flag = 0;
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


  // print_InsertStates();

  setAllRelays();
  print_Channels();
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
  if (flash_counter == FLASH_RATE) {
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
  if (led_counter == 80) {
    update_led = 1;
    led_counter = 0;
  }
  else {
    led_counter++;
  }
}