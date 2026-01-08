/*
  CryptoGarage - PWM Dimmer

  TODO
*/

#pragma once

#if !efined(ARDUINO_ARCH_ESP32)

#include <App.h>
#include <PersistentMemory.h>

#include "IDimmer.h"
#include "GaussCurve.h"

const char KEY_PWM_PIN[] = "pwm_pin";
const char KEY_CURVE_POINTS[] = "curve_points";

const int MAX_BRIGHTNESS = 16382;
const int MIN_BRIGHTNESS = 0;

class SPWMDimmer : public IDimmer {

  private:
    GaussCurve curve;
    int pwmPin;

  public:
    //Here we process the plaintext commands and generate an answer for the client.
    SPWMDimmer(const char * name, int pin=-1);
    void loop();
    void setup();
    void setVal(double val);
    double getVal();
};
