/*
* Relays - GenericRelay
* 
* (implementation)
*/

#include "GenericRelay.h"

GenericRelay::GenericRelay(int pin){
  relayPin = pin;
  pinMode(relayPin, OUTPUT);
}

void GenericRelay::setState(bool state){
  digitalWrite(relayPin, state);
  relayState = state;
}

bool GenericRelay::getState(){
  return relayState;
}

