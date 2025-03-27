/*
* CryptoGarage - IButton
* 
* (implementation)
*/

#include "IButton.h"

void IButton::attachCallback(void * context, ButtonCallbackFun callback){
  this->context = context;
  this->callback = callback;
}

ButtonState IButton::getDebouncedButtonState(){
  return currentState;
}

unsigned long IButton::getCurrentStateSince(){
  return currentStateSince;
}


void IButton::loop(){
  unsigned long now = millis();
  if(now > nextMillis){
    currentSample %= samplesSize;
    buttonSamples[currentSample++] = getState();

    double buttonValue = 0;
    for(bool sample : buttonSamples){
      buttonValue += sample;
    }
    buttonValue /= samplesSize;

    switch(currentState){
      case BUTTON_UP:
        if(buttonValue < 0.2){
          currentState = BUTTON_FALLING;
          previousStateSince = currentStateSince;
          currentStateSince = now;
          callback(context, inputPin, BUTTON_FALLING, now - previousStateSince);
        }
        nextMillis = 0;
        break;

      case BUTTON_FALLING:
        nextMillis = now + WAIT_TIME;
        currentState = BUTTON_DOWN;
        previousStateSince = currentStateSince;
        currentStateSince = now;
        callback(context, inputPin, BUTTON_DOWN, now - previousStateSince);
        break;

      case BUTTON_DOWN:
        if(buttonValue > 0.8){
          currentState = BUTTON_RISING;
          previousStateSince = currentStateSince;
          currentStateSince = now;
          callback(context, inputPin, BUTTON_RISING, now - previousStateSince);
        }
        nextMillis = 0;
        break;

      case BUTTON_RISING:
        nextMillis = now + WAIT_TIME;
        currentState = BUTTON_UP;
        previousStateSince = currentStateSince;
        currentStateSince = now;
        callback(context, inputPin, BUTTON_UP, now - previousStateSince);
        break;
    }
  }
}
