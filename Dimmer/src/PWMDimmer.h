/*
  CryptoGarage - PWM Dimmer

  TODO
*/

#pragma once

#include <App.h>
#include <PersistentMemory.h>

#include "IDimmer.h"
#include "GaussCurve_PWM.h"

const char KEY_PWM_PIN[] = "pwm_pin";
const char KEY_CURVE_POINTS[] = "curve_points";

const int MAX_BRIGHTNESS = 1023;
const int MIN_BRIGHTNESS = 0;

class PWMDimmer : public IDimmer {

  private:
    GaussCurve curve;
    int pwmPin;
    double toRelative(int absolute);
    int toAbsolute(double relative);

  public:
    //Here we process the plaintext commands and generate an answer for the client.
    PWMDimmer(const char * name, int pin=-1);
    void loop();
    void setup();
    void setVal(double val);
    double getVal();
};
