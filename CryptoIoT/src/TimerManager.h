/*
* CryptoIoT - TimerManager
* 
* Class for reacting to timer events
*/

#pragma once

#include <CIoT_Time.h>

#include "App.h"
#include "Message.h"
#include <PrintDebug.h>

struct TimerEvent{
	uint32_t timestamp = UINT32_MAX;
	String command;
};

class TimerManager {
	private:
		TimerEvent timer_events[10];
		App ** apps;
		int num_apps;
		uint32_t lastMillis = 0;
		
		void sortByTimes();
		static int sort_events(const void *cmp1, const void *cmp2);
		static int mod(int a, int b);
	  
	public:
		TimerManager(App ** apps, int apps_len);
		void setup();
		void loop();
		String getStatus();
		void execDelayed(String& command, int after_ms);
		void execDelayed(const char * command, int after_ms);
		void clear(String& command);
		void clear(const char * command);
		void clearAll();
};
