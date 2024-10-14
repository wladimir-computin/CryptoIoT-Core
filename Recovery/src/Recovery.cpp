/*
 * CryptoGarage - Recovery
 * 
 * (implementation)
 */

#include "Recovery.h"


void Recovery::resetTickerTick(void * context){
	(*(Recovery*)context).stop();
}

void Recovery::start(){
	PersistentMemory pmem("recovery", true);
	if(pmem.readBool(KEY_RECOVERY_ENABLED, recovery_enabled)){
		recovery_counter = pmem.readInt(KEY_RECOVERY_COUNTER, recovery_counter);
		if(recovery_counter >= RECOVERY_COUNTER_MAX - 1){
			PersistentMemory::format();
			ESP.restart();
		} else {
			pmem.writeInt(KEY_RECOVERY_COUNTER, recovery_counter+1);
			resetTicker.once_ms(TIME_TO_RESET_MS, resetTickerTick, (void*)this); //call setState(false) after [RELAY_TRIGGER_TIME_MS] milliseconds passed.
		}
	} else {
		recovery_finished = true;
	}
	pmem.commit();
}

void Recovery::stop(){
	recovery_finished = true;
}

void Recovery::loop(){
	if(!done && recovery_finished){
		PersistentMemory pmem("recovery", false);
		if(pmem.readInt(KEY_RECOVERY_COUNTER, 0) != 0){
			pmem.writeInt(KEY_RECOVERY_COUNTER, 0);
			pmem.commit();
		}
		done = true;
	}
}

