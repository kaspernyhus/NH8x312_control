/*
---------------------------------
State Machine
8x312+4
---------------------------------
*/

#include "statedef.h"


//initial state
state currentState = scrollState;


// State Matrix
//    CurrentState              EncP                   EncM                   EncBut                      EncButLongPress               //
stateElement stateMatrix[2][5] = {
  {   {scrollState,noaction},   {scrollState,incSQ},   {scrollState,decSQ},   {insState,selActive},       {scrollState,resetInserts}    },
  {   {insState,noaction},      {insState,incIns},     {insState,decIns},     {scrollState,confirmIns},   {scrollState,resetInserts}    }
};


//state action control based on events
void stateEval(event e)
{
    stateElement stateEvaluation = stateMatrix[currentState][e];   // table look-up to fetch the relevant next state and the related function pointer
    currentState = stateEvaluation.nextState;
    (*stateEvaluation.actionToDo)();                               // the action execution
}


