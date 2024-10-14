/*
* CryptoIoT - System
* 
* System API routines
*/

#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
	#include <WiFi.h>
#endif

class System {
public:
	void getSerialID(char * str, bool delim=true);
	String getSerialID(bool delim=true);
};
