/*
* CryptoGarage - StatusLED
* 
* (implementation)
*/

#include "StatusLED.h"

StatusLED::StatusLED(int pin, bool invert){
#if ENABLE_STATUS_LED == 1
    ledPin = pin;
    pinMode(ledPin, OUTPUT);
    setState(false);
    invert_output = invert;
  #ifdef ARDUINO_ARCH_ESP32
    //ledcSetup(ESP32_LED_CHANNEL, ESP32_LED_FREQ, ESP32_LED_RESOLUTION);
    //ledcAttachPin(ledPin, ESP32_LED_CHANNEL);
    #ifdef ARDUINO_ESP32_CORE_V3
      analogWriteResolution(pin, 10);
    #else
      analogWriteResolution(10);
    #endif
  #endif
#endif
}


void StatusLED::setState(bool state){
#if ENABLE_STATUS_LED == 1
  fadeStop();
  int cur = state ? 1.0 : 0.0;
  setVal(cur);
#endif
}

void StatusLED::setVal(double val){
#if ENABLE_STATUS_LED == 1
  val = constrain(val, 0.0, 1.0);
  analogWrite(ledPin, toAbsolute(val));
  current = val;
#endif
}

double StatusLED::getVal(){
  return current;
}

void StatusLED::fadeToVal(double val, int time_ms){
#if ENABLE_STATUS_LED == 1
  val = constrain(val, 0.0, 1.0);
  argument.old_current = getVal();
  argument.goal = val;
  argument.time_ms = time_ms;
  argument.ms_passed = 0;
  ledTicker.attach_ms(1000/FADEFPS, ledTickerTick, (void*)this);
#endif
}

void StatusLED::fade(FadeMode fademode, int time_ms){
#if ENABLE_STATUS_LED == 1
  argument.fademode = fademode;
  switch(fademode){
    case SINGLE_ON_OFF:
      fadeToVal(1.0, time_ms / 2);
    break;
    case SINGLE_OFF_ON:
      fadeToVal(0.0, time_ms / 2);
    break;
    case PERIODIC_FADE:
      if(getVal() == 0)
        fadeToVal(1.0, time_ms / 2);
      else
        fadeToVal(0.0, time_ms / 2);
    break;
  }
#endif
}

void StatusLED::ledTickerTick(void * context) {
#if ENABLE_STATUS_LED == 1
  (*(StatusLED*)context).ledTickerTick2();
#endif
}

void StatusLED::ledTickerTick2(){
#if ENABLE_STATUS_LED == 1
  if (toAbsolute(argument.goal) == toAbsolute(getVal())) {
    setVal(argument.goal);
    ledTicker.detach();
    fadeCompleted();
  } else {
    double m = ease((double)(argument.ms_passed) / (double)argument.time_ms);
    if (m > 1) {
      m = 1;
    }
    double new_current = argument.old_current + (m * (argument.goal - argument.old_current));
    argument.ms_passed += (1000 / FADEFPS);
    setVal(new_current);
  }
#endif
}

void StatusLED::fadeCompleted(){
#if ENABLE_STATUS_LED == 1
  //if(getVal() == 0.0 || getVal() == 1.0){
  switch(argument.fademode){
    case NONE:

    break;
    
    case SINGLE_ON_OFF:
      argument.fademode = NONE;
      fadeToVal(0.0, argument.time_ms);
    break;
    
    case SINGLE_OFF_ON:
      argument.fademode = NONE;
      fadeToVal(1.0, argument.time_ms);
    break;
    
    case PERIODIC_FADE:
      if(getVal() == 0.0)
        fadeToVal(1.0, argument.time_ms);
      else
        fadeToVal(0.0, argument.time_ms);
    break;
  //}
  }
#endif
}

void StatusLED::fadeStop(){
#if ENABLE_STATUS_LED == 1
  argument.fademode = NONE;
  ledTicker.detach();
#endif
}

bool StatusLED::isFading(){
  return argument.goal != getVal();
}

double StatusLED::ease(double t) {
  double sqt = t * t;
  return sqt / (2.0f * (sqt - t) + 1.0f);
}

double StatusLED::toRelative(int absolute) {
#if ENABLE_STATUS_LED == 1
  double ret;
  if(invert_output){
    ret = sqrt((double)(absolute-1023) / -1023.0);
  } else {
    ret = sqrt((double)(absolute)/1023.0);
  }
  ret = constrain(ret, 0.0, 1.0);
  
  return ret;
#else
  return 0.0;
#endif
}

int StatusLED::toAbsolute(double relative) {
#if ENABLE_STATUS_LED == 1
  int ret = (int)round(1023.0*relative*relative);
  ret = constrain(ret, MINLED, MAXLED);
  
  if(invert_output){
    return 1023 - ret;
  } else {
    return ret;
  }
#else
  return 0;
#endif
}
