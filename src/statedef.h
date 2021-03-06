/*
---------------------------------
State Machine
8x312+4
---------------------------------
*/


#include "config.h"




// States
typedef enum {
  scrollState,
  insState
} state;

// Events
typedef enum {
  NILEVENT,
  EncP,
  EncM,
  EncBut,
  EncButLong
} event;


// function pointer
typedef void (*action)();

//the nextState and action definition
typedef struct {
    state nextState;
    action actionToDo;
} stateElement;



// Action function definitions
void noaction();
void incSQ();
void decSQ();
void selActive();
void incIns();
void decIns();
void confirmIns();
void resetInserts();


//function which test on the event at shift states
void stateEval(event e);
event	eventOccured;


stateElement stateMatrix[2][5];

extern state currentState;
