/*
* CryptoGarage - IButton
* 
* Abstract class for controlling buttons.
*/

#pragma once

#include <Arduino.h>
#include <PrintDebug.h>

enum ButtonState {BUTTON_RELEASED, BUTTON_PUSH, BUTTON_PUSHED, BUTTON_RELEASE};
typedef void (*ButtonCallbackFun)(void * context, int num, ButtonState value, long previousStateMillis);

class IButton{
  protected:

  public:
    void loop();
    void attachCallback(void * context, ButtonCallbackFun callback);
    ButtonState getDebouncedButtonState();
    unsigned long getCurrentStateSince();
    unsigned long getPreviousStateSince();
    virtual bool getState() = 0;

  protected:
    ButtonState currentState = BUTTON_RELEASED;
    bool buttonSamples[3];
    int samplesSize = 3;
    int currentSample = 0;
    const int WAIT_TIME = 100;
    unsigned long nextMillis = 0;
    unsigned long currentStateSince = 0;
    unsigned long previousStateSince = 0;
    ButtonCallbackFun callback;
    void * context;
    int inputPin;

};
