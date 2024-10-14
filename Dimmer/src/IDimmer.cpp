/*
 C *ryptoIoT - Dimmer
 
 (implementation)
 */

#include "IDimmer.h"

IDimmer::IDimmer(const char * name) : appname(name){
  registerCommand(COMMAND_DIMFADE);
  registerCommand(COMMAND_DIMSTATE);
}

ProcessMessageStruct IDimmer::processMessage(String &message) {
  
  if (COMMAND_DIMFADE.check(message)) {
    double val = COMMAND_DIMFADE.getParamVal(message, 0).toDouble();
    int duration = COMMAND_DIMFADE.getParamVal(message, 1).toInt();
    if (duration > 0){
      if (val >= 0 && val <= 1) {
        printDebug("[Dimmer] Fading to value " + String(val) + " in " + String(duration) + "ms");
        fadeToVal(val, duration);
      } else {
        return {ERR, "Value must be in [0, 1]"};
      }
    } else {
      printDebug("[Dimmer] Setting to value " + String(val));
      setVal(val);
    }
    return {ACK, ""};
  }
  
  if (COMMAND_DIMSTATE.check(message)) {
    return {DATA, String(getVal()), FLAG_KEEP_ALIVE};
  }
  
  return App::processMessage(message);
}

String IDimmer::getName(){
  return appname;
}

String IDimmer::getStatus() {
  return String("Current Brightness: ") + getVal();
}

String IDimmer::getType() {
  return "DIMMER";
}

void IDimmer::setState(bool state) {
  fadeStop();
  double val = state ? 1 : 0;
  setVal(val);
}

void IDimmer::fadeToVal(double val, int time_ms) {
  val = constrain(val, 0.0, 1.0);
  if(val != getVal() || argument.ms_passed == 0){
    if(!isFading()){
      argument.old_current = getVal();
      argument.goal = val;
      argument.time_ms = time_ms;
      argument.ms_passed = 0;
      ledTicker.attach_ms(1000 / FADEFPS, ledTickerTick, (void*) this);
    } else {
      double x = 0.5;
      argument.time_ms = time_ms*2;
      argument.ms_passed = time_ms;
      argument.goal = val;
      argument.old_current = getVal() - (argument.goal - getVal());
    }
  }
}

void IDimmer::fade(FadeMode fademode, int time_ms) {
  argument.fademode = fademode;
  switch (fademode) {
    case SINGLE_ON_OFF:
      fadeToVal(1.0, time_ms / 2);
      break;
    case SINGLE_OFF_ON:
      fadeToVal(0.0, time_ms / 2);
      break;
    case PERIODIC_FADE:
      if (getVal() == 0)
        fadeToVal(1.0, time_ms / 2);
    else
      fadeToVal(0.0, time_ms / 2);
    break;
  }
}

void IDimmer::ledTickerTick(void * context) {
  (*(IDimmer*)context).ledTickerTick2();
}

void IDimmer::ledTickerTick2() {
  if (toAbsolute(argument.goal) == toAbsolute(getVal())) {
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
}

void IDimmer::fadeCompleted() {
  switch (argument.fademode) {
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
      if (curBrightness == 0)
        fadeToVal(1.0, argument.time_ms);
    else
      fadeToVal(0.0, argument.time_ms);
    break;
  }
}

void IDimmer::fadeStop() {
  argument.fademode = NONE;
  ledTicker.detach();
}

bool IDimmer::isFading(){
  return argument.goal != getVal() && ledTicker.active();
}

double IDimmer::ease(double t) {
  double sqt = t * t;
  return sqt / (2.0f * (sqt - t) + 1.0f);
}
