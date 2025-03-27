/*
* CryptoGarage - IButton
* 
* Abstract class for controlling buttons.
* If you want to add your own relay, you have to implement the setState() and getState() methods.
* Take a look at LCTechRelay and WemosShieldRelay for examples.
*/

#pragma once

#include <Arduino.h>
#include <PrintDebug.h>

enum ButtonState {BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RISING};
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
    ButtonState currentState = BUTTON_UP;
    bool buttonSamples[3];
    int samplesSize = sizeof(buttonSamples) / sizeof(buttonSamples[0]);
    int currentSample = 0;
    const int WAIT_TIME = 100;
    unsigned long nextMillis = 0;
    unsigned long currentStateSince = 0;
    unsigned long previousStateSince = 0;
    ButtonCallbackFun callback;
    void * context;
    int inputPin;

};
