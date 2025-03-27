/*
* CryptoIoT - TimerManager
* 
* (implementation)
*/

#include "TimerManager.h"

TimerManager::TimerManager(App ** apps, int apps_len){
	this->apps = apps;
	num_apps = apps_len;
}

void TimerManager::setup(){
}

void TimerManager::loop(){
	long now = millis();
	if (now - lastMillis >= 1) {
		TimerEvent &event = timer_events[0];
		if(now >= event.timestamp && event.timestamp != UINT32_MAX){
			String appname = Message::getParam(event.command,0);
			for (int i = 0; i < num_apps; i++) {
				if(apps[i]->getName() == appname){
					printDebug("[TimerMan] Timer event -> " + event.command);
					String m = Message::getParam(event.command,1,true);
					apps[i]->processMessage(m);
				}
			}
			event.timestamp = UINT32_MAX;
			event.command = "";
			sortByTimes();
		}
		lastMillis = now;
	}
}

String TimerManager::getStatus() {
	String out;
	for (int i = 0; i < sizeof(timer_events) / sizeof(timer_events[0]); i++) {
		TimerEvent &event = timer_events[i];
		if(event.timestamp != UINT32_MAX){
			out += String(event.timestamp - millis()) + "ms -> " + event.command + "\n";
		}
	}
	return out;
}

int TimerManager::sort_events(const void *cmp1, const void *cmp2) {
	TimerEvent a = *((TimerEvent *)cmp1);
	TimerEvent b = *((TimerEvent *)cmp2);
	return a.timestamp < b.timestamp ? -1 : (a.timestamp > b.timestamp ? 1 : 0);
}

void TimerManager::sortByTimes() {
	qsort(timer_events, sizeof(timer_events) / sizeof(timer_events[0]), sizeof(timer_events[0]), sort_events);
}

int TimerManager::mod(int a, int b) {
  while (a < 0) {
    a = a + b;
  }
  return a % b;
}

void TimerManager::execDelayed(const char * command, int after_ms){
	timer_events[sizeof(timer_events) / sizeof(timer_events[0])-1].command = command;
	timer_events[sizeof(timer_events) / sizeof(timer_events[0])-1].timestamp = millis() + after_ms;
	sortByTimes();
}

void TimerManager::execDelayed(String& command, int after_ms){
	timer_events[sizeof(timer_events) / sizeof(timer_events[0])-1].command = command;
	timer_events[sizeof(timer_events) / sizeof(timer_events[0])-1].timestamp = millis() + after_ms;
	sortByTimes();
}
