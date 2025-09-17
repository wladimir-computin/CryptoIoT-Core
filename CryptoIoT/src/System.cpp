/*
* CryptoIoT - System
* 
* (implementation)
*/

#include "System.h"

void System::getSerialID(char * str, bool delim){
	uint8_t buf[6];
	#if (defined ARDUINO_ARCH_ESP32 && defined ARDUINO_ESP32_CORE_V3)
		Network.macAddress(buf);
	#else
		WiFi.macAddress(buf);
	#endif
	if(!delim){
		snprintf(str, 13, "%02X%02X%02X%02X%02X%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	} else {
		snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	}
}

String System::getSerialID(bool delim){
	char buf[20];
	getSerialID(buf, delim);
	return buf;
}
