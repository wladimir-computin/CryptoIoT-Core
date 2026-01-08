/*
* CryptoIoT - EventManager
* 
* (implementation)
*/

#include "EventManager.h"

EventManager::EventManager(void * context, ProcessMessageFun processMessageFunction, Time * t) : processMessageFun(processMessageFunction), time(t){
	this->context = context;
}

void EventManager::setup(){
	PersistentMemory pmem("events", true);
	time_events_enabled = pmem.readBool(KEY_TIME_EVENTS_ENABLED, time_events_enabled);
	device_events_enabled = pmem.readBool(KEY_DEVICE_EVENTS_ENABLED, device_events_enabled);
	String time_events_str = pmem.readString(KEY_TIME_EVENTS, DEFAULT_TIME_EVENTS);
	String device_events_str = pmem.readString(KEY_DEVICE_EVENTS, DEFAULT_DEVICE_EVENTS);
	pmem.commit();
	
	if (time_events_enabled) {
		JsonDocument doc;
		deserializeJson(doc, time_events_str);
		JsonArray arr = doc.as<JsonArray>();
		int i = 0;
		num_time_events = 0;
		for (JsonObject obj : arr) {
			for (JsonPair p : obj) {
				String key = p.key().c_str();
				JsonArray cmdArr = p.value().as<JsonArray>();
				if(cmdArr){
					int j = 0;
					for(String cmd : cmdArr){
						time_events[i].command[j++] = cmd;
					}
				} else {
					time_events[i].command[0] = p.value().as<String>();
				}

				if (key == SUNRISE) {
					time_events[i].type = SUNRISE_TIME;
				} else if (key == SUNSET) {
					time_events[i].type = SUNSET_TIME;
				} else {
					time_events[i].type = STATIC_TIME;
					time_events[i].minutes = Time::str2min(key);
				}
			}
			num_time_events++;
			if (++i == sizeof(time_events) / sizeof(time_events[0])) {
				break;
			}
		}
    }
    if(device_events_enabled){
		JsonDocument doc;
		deserializeJson(doc, device_events_str);
		JsonArray arr = doc.as<JsonArray>();
		int i = 0;
		num_device_events = 0;
		for (JsonObject obj : arr) {
			for (JsonPair p : obj) {
				device_events[i].event = p.key().c_str();
				JsonArray cmdArr = p.value().as<JsonArray>();
				if(cmdArr){
					int j = 0;
					for(String cmd : cmdArr){
						device_events[i].command[j++] = cmd;
					}
				} else {
					device_events[i].command[0] = p.value().as<String>();
				}
			}
			num_device_events++;
			if (++i == sizeof(device_events) / sizeof(device_events[0])) {
				break;
			}
		}
	}
}

void EventManager::loop(){
	if (time_events_enabled) {
		long now = millis();
		if (now - lastMillis >= 1000 * 60) {
			int minutes = time->minutesSinceMidnight();
			updateDynamicTimes();
			last_event_index = num_time_events - 1;
			next_event_index = 0;
			for (int i = 0; i < num_time_events; i++) {
				if (time_events[i].minutes > minutes) {
					last_event_index = mod((i - 1), num_time_events);
					next_event_index = mod(i, num_time_events);
					break;
				}
			}

			TimeEvent &event = time_events[last_event_index];
			if (!event.done) {
				for(int i = 0; i < 10; i++){
					if(event.command[i] != ""){
						processMessageFun(context, event.command[i]);
					} else {
						break;
					}
				}
				event.done = true;
				time_events[next_event_index].done = false;
			}

			lastMillis = now;
		}
	}
};

String EventManager::getStatus() {
	String out;
	for (int i = 0; i < num_time_events; i++) {
		if (i == last_event_index) {
			out += "-> ";
		}
		if (time_events[i].type == SUNRISE_TIME) {
			out += "Sunrise ("  + Time::min2str(time_events[i].minutes) + ")";
		} else if (time_events[i].type == SUNSET_TIME) {
			out += "Sunset ("  + Time::min2str(time_events[i].minutes) + ")";
		} else {
			out += Time::min2str(time_events[i].minutes);
		}
		out += " ->\n";
		for (String& cmd : time_events[i].command){
			if(cmd != ""){
				out += "   " + cmd + "\n";
			}
		}
	}
	out += "\n";
	for (int i = 0; i < num_device_events; i++) {
		out += device_events[i].event + " ->\n";
		for (String& cmd : device_events[i].command){
			if(cmd != ""){
				out += "   " + cmd + "\n";
			}
		}

	}
	
	return out;
}

int EventManager::sort_events(const void *cmp1, const void *cmp2) {
  TimeEvent a = *((TimeEvent *)cmp1);
  TimeEvent b = *((TimeEvent *)cmp2);
  return a.minutes < b.minutes ? -1 : (a.minutes > b.minutes ? 1 : 0);
}

void EventManager::updateDynamicTimes() {
  for (int i = 0; i < num_time_events; i++) {
    if (time_events[i].type == SUNRISE_TIME) {
      time_events[i].minutes = time->custom_sunrise_minutes();
    } else if (time_events[i].type == SUNSET_TIME) {
      time_events[i].minutes = time->custom_sunset_minutes();
    }
  }
  qsort(time_events, num_time_events, sizeof(time_events[0]), sort_events);
}

int EventManager::mod(int a, int b) {
  while (a < 0) {
    a = a + b;
  }
  return a % b;
}

void EventManager::notify(String& event){
	if (device_events_enabled) {
		printDebug("[EventMan] Notify: " + event);
		for (int i = 0; i < num_device_events; i++) {
			if (device_events[i].event == event) {
				for(int j = 0; j < 10; j++){
					if(device_events[i].command[j] != ""){
						processMessageFun(context, device_events[i].command[j]);
					} else {
						break;
					}
				}
			}
		}
	}
}
