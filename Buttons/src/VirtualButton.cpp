/*
* Buttons - VirtualButton
* 
* (implementation)
*/

#include "VirtualButton.h"

VirtualButton::VirtualButton(int id){
    inputPin = id;
}

bool VirtualButton::getState(){
  return state;
}

void VirtualButton::setState(bool in){
  state = in;
}

