/*
  CryptoGarage - StausLED
  
  Class for controlling the builtin LED
*/

#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include <PrintDebug.h>

class StatusLED {
  
  public:
    enum FadeMode {NONE, SINGLE_ON_OFF, SINGLE_OFF_ON, PERIODIC_FADE};


  private:

    struct Argument {
      int time_ms;
      int ms_passed;
      double old_current;
      double goal;
      FadeMode fademode;
    };

    Ticker ledTicker;
    double current = 0;
    int ledPin;
    Argument argument = {0,0,0,0,NONE};
    static void ledTickerTick(void * context);
    void ledTickerTick2();
    void fadeCompleted();
    const int MAXLED = 1023;
    const int MINLED = 0;
    const int FADEFPS = 60;
    
    double ease(double t);
	double toRelative(int absolute);
    int toAbsolute(double relative);
    
    //const int ESP32_LED_CHANNEL = 4;
    //const int ESP32_LED_FREQ = 5000;
    //const int ESP32_LED_RESOLUTION = 10;


  public:
#ifdef LED_BUILTIN
    StatusLED(int pin=LED_BUILTIN);
#else
    StatusLED(int pin=2);
#endif
    void setState(bool state);
    void setVal(double val);
    double getVal();
    void fade(FadeMode fademode, int time_ms);
    void fadeToVal(double val, int time_ms);
    void fadeStop();
    bool isFading();

};
