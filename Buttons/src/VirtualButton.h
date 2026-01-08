   /*
* Buttons - VirtualButton
* 
* Class for controlling the relay shield of a Wemos D1 Mini (Pro)
*/

#pragma once

#include "IButton.h"


class VirtualButton: public IButton{
  private:
    bool state;

  public:
    VirtualButton(int id);
    bool getState();
    void setState(bool in);
};
