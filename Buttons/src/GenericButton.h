   /*
* Buttons - GenericButton
* 
* Class for controlling the relay shield of a Wemos D1 Mini (Pro)
*/

#pragma once

#include "IButton.h"


class GenericButton: public IButton{
  private:
    bool invert;

  public:
    GenericButton(int pin, bool invert=false);
    bool getState();
};
