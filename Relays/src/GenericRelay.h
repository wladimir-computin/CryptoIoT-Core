/*
* Relays - GenericRelay
* 
* Class for controlling the relay shield of a Wemos D1 Mini (Pro)
*/

#pragma once

#include "IRelay.h"


class GenericRelay: public IRelay{
  private:
    bool relayState = false;
	int relayPin;
    
  public:
    GenericRelay(int pin);
    void setState(bool state);
    bool getState();
};
