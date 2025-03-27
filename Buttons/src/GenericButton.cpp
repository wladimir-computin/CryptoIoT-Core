/*
* Buttons - GenericButton
* 
* (implementation)
*/

#include "GenericButton.h"

GenericButton::GenericButton(int pin, bool invert){
  inputPin = pin;
  this->invert = invert;
}

bool GenericButton::getState(){
  return invert ? !digitalRead(inputPin) : digitalRead(inputPin);
}

