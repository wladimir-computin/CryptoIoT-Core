/*
  CryptoIoT - Halloween

  (implementation)
*/

#include "Halloween.h"

Halloween::Halloween(IDimmer &_dimmer) : dimmer(_dimmer){
	registerCommand(COMMAND_HALLOWEEN_ENABLE);
}


void Halloween::loop() {
	if(enabled){
		long now = millis();
		if (last_millis <= (now - next_wait)){
			if (dimmer.getVal() == 0){
				for(int i = 0; i<5; i++){
					dimmer.setVal(0);
					delay(100);
					dimmer.setVal(1);
					delay(100);
				}
				next_wait = 500;
			} else if (randomDouble(0,1) <= flicker_propability){
				double next_brightness = 0;
				if(randomDouble(0,1) <= flicker_brownout_propability){
					next_brightness = randomDouble(0,base_brightness);
				} else {
					next_brightness = randomDouble(base_brightness,high_brightness);
				}
				int fade_time = randomInt(rise_ms_min,rise_ms_max);
				dimmer.fadeToVal(next_brightness, fade_time);
				next_wait = fade_time;
				
			} else if (dimmer.getVal() > reset_high_brightness || dimmer.getVal() < reset_low_brightness){
				int fade_time = randomInt(recover_ms_min,recover_ms_max);
				dimmer.fadeToVal(base_brightness, fade_time);
				next_wait = fade_time;
			} else {
				double current = dimmer.getVal();
				double next_brightness = randomDouble(current-0.05,current + 0.05);
				dimmer.setVal(next_brightness);
				next_wait = 20;
			}
			last_millis = now;
		}
	}
}

void Halloween::start(){
	original_brightness = dimmer.getVal();
	dimmer.setVal(1);
	delay(100);
	dimmer.fadeToVal(0, 100);
	next_wait = 1000;
	last_millis = millis();
	enabled = true;
}

void Halloween::stop(){
	dimmer.setVal(0);
	dimmer.fadeToVal(original_brightness, 4000);
	enabled = false;
}

bool Halloween::isRunning(){
	return enabled;
}

void Halloween::setup() {
	PersistentMemory pmem("Halloween", true);
	base_brightness = pmem.readDouble(KEY_BASE_BRIGHTNESS, base_brightness);
	high_brightness = pmem.readDouble(KEY_HIGH_BRIGHTNESS, high_brightness);
	low_brightness = pmem.readDouble(KEY_LOW_BRIGHTNESS, low_brightness);
	rise_ms_min = pmem.readInt(KEY_RISE_MS_MIN, rise_ms_min);
	rise_ms_max = pmem.readInt(KEY_RISE_MS_MAX, rise_ms_max);
	recover_ms_min = pmem.readInt(KEY_RECOVER_MS_MIN, recover_ms_min);
	recover_ms_max = pmem.readInt(KEY_RECOVER_MS_MAX, recover_ms_max);
	flicker_propability = pmem.readDouble(KEY_FLICKER_PROPABILITY, flicker_propability);
	flicker_brownout_propability = pmem.readDouble(KEY_FLICKER_BROWNOUT_PROPABILITY, flicker_brownout_propability);
	reset_low_brightness = pmem.readDouble(KEY_RESET_LOW_BRIGHTNESS, reset_low_brightness);
	reset_high_brightness = pmem.readDouble(KEY_RESET_HIGH_BRIGHTNESS, reset_high_brightness);
	pmem.commit();
}

ProcessMessageStruct Halloween::processMessage(String &message) {
  if (COMMAND_HALLOWEEN_ENABLE.check(message)) {
    
	if(Message::getParam(message, 1).toInt()){
		start();
	} else {
		stop();
	}
    return {ACK, ""};
  }
  
  return {ERR, "NO_COMMAND"};
}

String Halloween::getName() {
  return "Halloween";
}

String Halloween::getStatus() {
  return String("Enabled: ") + enabled;
}

String Halloween::getType() {
  return "SPECIAL";
}

double Halloween::randomDouble(double low, double high){
	int low_int = low*1000;
	int high_int = high*1000;
	int rnd = random(low_int, high_int);
	return (double)rnd / 1000.0;
}

int Halloween::randomInt(int low, int high){
	int rnd = random(low, high);
	return rnd;
}
