/*
* CryptoGarage - Recovery
* 
* Abstract class for controlling relays.
* If you want to add your own relay, you have to implement the setState() and getState() methods.
* Take a look at LCTechRelay and WemosShieldRelay for examples.
*/

#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include <PersistentMemory.h>

#include <PrintDebug.h>

const int TIME_TO_RESET_MS = 5000;
const int RECOVERY_COUNTER_MAX = 5;
const char KEY_RECOVERY_ENABLED[] = "recovery_enabled";
const char KEY_RECOVERY_COUNTER[] = "recovery_counter";

class Recovery{
  protected:
    Ticker resetTicker;
    static void resetTickerTick(void * context);
	bool recovery_enabled = true;
	bool recovery_finished = false;
	bool done = false;
	int recovery_counter = 0;
    
  public:
    void start();
	void stop();
	void loop();
};
