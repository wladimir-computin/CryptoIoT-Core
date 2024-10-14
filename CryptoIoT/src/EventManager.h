/*
* CryptoIoT - EventManager
* 
* Class for reacting to time and spontanous events
*/

#pragma once

#include <Time.h>
#include <PersistentMemory.h>

#include "App.h"
#include "Message.h"
#include <PrintDebug.h>

const char KEY_TIME_EVENTS_ENABLED[] = "time_events_enabled";
const char KEY_DEVICE_EVENTS_ENABLED[] = "device_events_enabled";
const char KEY_TIME_EVENTS[] = "time_events";
const char KEY_DEVICE_EVENTS[] = "device_events";
const char DEFAULT_TIME_EVENTS[] = "[{\"sunrise\":\"Dimmer:dimfade:0:1000\"}, {\"sunset\":\"Dimmer:dimfade:0:1000\"}]";
const char DEFAULT_DEVICE_EVENTS[] = "[{\"Dimmer:motionevent:1\":\"Dimmer:dimfade:0:1000\"}]";

const int8_t STATIC_TIME = 0;
const int8_t INVALID_TIME = -1;
const int8_t SUNRISE_TIME = -2;
const int8_t SUNSET_TIME = -3;

const char SUNRISE[] = "sunrise";
const char SUNSET[] = "sunset";

struct TimeEvent{
	int8_t type = STATIC_TIME;
	int minutes = INVALID_TIME;
	String command;
	bool done = false;
};

struct AppEvent{
	String event;
	String command;
	bool done = false;
};

class EventManager {
	private:
		TimeEvent time_events[10];
		AppEvent device_events[10];
		App ** apps;
		Time * time;
		int num_apps;
		int lastMillis = -50 * 1000;
		bool time_events_enabled;
		bool device_events_enabled;
		int num_time_events;
		int num_device_events;
		int last_event_index = 0;
		int next_event_index = 0;
		
		void updateDynamicTimes();
		static int sort_events(const void *cmp1, const void *cmp2);
		static int mod(int a, int b);
	  
	public:
		EventManager(App ** apps, int apps_len, Time * time);
		void setup();
		void loop();
		String getStatus();
		void notify(String& event);
};
