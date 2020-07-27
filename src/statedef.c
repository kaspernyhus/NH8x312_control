/*
---------------------------------
State Machine
8x312+4
---------------------------------
*/

#include "statedef.h"


//initial state
state currentState = state1;


// State Matrix
//    CurrentState         EncP              EncM              EncBut    
stateElement stateMatrix[2][4] = {
  {   {state1,noaction},   {state1,incSQ},   {state1,decSQ},   {state2,selActive}   },
  {   {state2,noaction},   {state2,incIns},  {state2,decIns},  {state1,confirmIns}  }
};


//state action control based on events
void stateEval(event e)
{
    stateElement stateEvaluation = stateMatrix[currentState][e];   // table look-up to fetch the relevant next state and the related function pointer
    currentState = stateEvaluation.nextState;
    (*stateEvaluation.actionToDo)();                               // the action execution
}


