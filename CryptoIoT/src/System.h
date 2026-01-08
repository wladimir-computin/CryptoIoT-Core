/*
* CryptoIoT - System
* 
* System API routines
*/

#pragma once

#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
	#include <WiFi.h>
#endif
#include "Reboot.h"

class System {

private:
	Reboot reboot_handler;

public:
	void getSerialID(char * str, bool delim=true);
	String getSerialID(bool delim=true);
	void reboot(unsigned long delay_ms = 0);
};
