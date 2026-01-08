/*
  CryptoGarage - Dimmer

  TODO
*/

#pragma once

#ifdef ARDUINO_ARCH_ESP8266

#include <Ticker.h>
#include <App.h>
#include <PersistentMemory.h>
#include <CIoT_Time.h>
#include "IDimmer.h"

#include "GaussCurve.h"

const int MAX_BRIGHTNESS = 1000;
const int MIN_BRIGHTNESS = 0;

const char KEY_LONGEST_DUTY_PAUSE[] = "longest_duty_pause";
const char KEY_FLICKER_FIX[] = "flicker_fix";
const char KEY_CURVE_POINTS[] = "curve_points";
const char KEY_PWM_PIN[] = "pwm_pin";
const char KEY_ZC_PIN[] = "zc_pin";

class EdgeDimmer : public IDimmer {

  private:
    GaussCurve curve;
    volatile int dimmer_index;
    volatile uint16_t longestDutyPauseUs = 42500;
	
  public:
    //Here we process the plaintext commands and generate an answer for the client.
    EdgeDimmer(const char * name);
    ~EdgeDimmer();
    void loop();
    void setup();
	void setVal(double val);
    double getVal();

    IRAM_ATTR int transform(double input);
	
    volatile int pwmPin = 12;
    volatile int zcPin = 13;
    volatile bool flickerfix = false;
};
IRAM_ATTR int sort_dimmers();
IRAM_ATTR void zeroCrossISR();
IRAM_ATTR void onTimerISR();

#endif
