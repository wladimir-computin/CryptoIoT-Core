/*
  CryptoIoT - Halloween

  TODO
*/

#pragma once

#include <Ticker.h>
#include <App.h>
#include <PersistentMemory.h>
#include <CIoT_Time.h>
#include "IDimmer.h"

static Param COMMAND_HALLOWEEN_ENABLE_PARAMS[] = {{"value", DATATYPE_BOOL, false, 0, 1}};
static Command COMMAND_HALLOWEEN_ENABLE("enable", ARRAY_LEN(COMMAND_HALLOWEEN_ENABLE_PARAMS), COMMAND_HALLOWEEN_ENABLE_PARAMS);

const char KEY_BASE_BRIGHTNESS[] = "base_brightness";
const char KEY_HIGH_BRIGHTNESS[] = "high_brightness";
const char KEY_LOW_BRIGHTNESS[] = "low_brightness";
const char KEY_RISE_MS_MIN[] = "rise_ms_min";
const char KEY_RISE_MS_MAX[] = "rise_ms_max";
const char KEY_RECOVER_MS_MIN[] = "recover_ms_min";
const char KEY_RECOVER_MS_MAX[] = "recover_ms_max";
const char KEY_FLICKER_PROPABILITY[] = "flicker_propability";
const char KEY_FLICKER_BROWNOUT_PROPABILITY[] = "flicker_brownout_propability";
const char KEY_RESET_LOW_BRIGHTNESS[] = "reset_low_brightness";
const char KEY_RESET_HIGH_BRIGHTNESS[] = "reset_high_brightness";

class Halloween : public App {
 
  private:
    Ticker halloweenTicker;
    static void halloweenTickerTick(void * context);
	void halloweenTickerTick2();
	
	double randomDouble(double low, double high);
	int randomInt(int low, int high);
		
	
	IDimmer &dimmer;
	
	double base_brightness = 0.3;
	double high_brightness = 1;
	double low_brightness = 0;
	int rise_ms_min = 50;
	int rise_ms_max = 200;
	int recover_ms_min = 200;
	int recover_ms_max = 1000;
	double flicker_propability = 0.2;
	double flicker_brownout_propability = 0.75;
	double reset_low_brightness = 0.1;
	double reset_high_brightness = 0.4;
	
	double original_brightness = 0;
	
	bool enabled = false;
	long last_millis = 0;
	int next_wait = 0;

  public:
    //Here we process the plaintext commands and generate an answer for the client.
    Halloween(IDimmer &_dimmer);
    ProcessMessageStruct processMessage(String &message);
    String getName();
    String getStatus();
	String getType();
    void loop();
    void setup();
	void start();
	void stop();
	bool isRunning();
};
