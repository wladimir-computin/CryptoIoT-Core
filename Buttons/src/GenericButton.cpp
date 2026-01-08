/*
* Buttons - GenericButton
* 
* (implementation)
*/

#include "GenericButton.h"

GenericButton::GenericButton(int pin, bool invert, int mode){
  inputPin = pin;
  this->invert = invert;
  pinMode(pin, mode);
}

bool GenericButton::getState(){
  return invert ? !digitalRead(inputPin) : digitalRead(inputPin);
}
