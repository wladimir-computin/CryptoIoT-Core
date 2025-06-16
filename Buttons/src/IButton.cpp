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
      case BUTTON_RELEASED:
        if(buttonValue > 0.8){
          currentState = BUTTON_PUSH;
          previousStateSince = currentStateSince;
          currentStateSince = now;
          callback(context, inputPin, BUTTON_PUSH, now - previousStateSince);
        }
        nextMillis = 0;
        break;

      case BUTTON_PUSH:
        nextMillis = now + WAIT_TIME;
        currentState = BUTTON_PUSHED;
        previousStateSince = currentStateSince;
        currentStateSince = now;
        callback(context, inputPin, BUTTON_PUSHED, now - previousStateSince);
        break;

      case BUTTON_PUSHED:
        if(buttonValue < 0.2){
          currentState = BUTTON_RELEASE;
          previousStateSince = currentStateSince;
          currentStateSince = now;
          callback(context, inputPin, BUTTON_RELEASE, now - previousStateSince);
        }
        nextMillis = 0;
        break;

      case BUTTON_RELEASE:
        nextMillis = now + WAIT_TIME;
        currentState = BUTTON_RELEASED;
        previousStateSince = currentStateSince;
        currentStateSince = now;
        callback(context, inputPin, BUTTON_RELEASED, now - previousStateSince);
        break;
    }
  }
}
