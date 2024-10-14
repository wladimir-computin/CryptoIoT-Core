/*
* CryptoIoT - System
* 
* (implementation)
*/

#include "System.h"

void System::getSerialID(char * str, bool delim){
	uint8_t buf[6];
	WiFi.macAddress(buf);
	if(!delim){
		snprintf(str, 13, "%02X%02X%02X%02X%02X%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	} else {
		snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	}
}

String System::getSerialID(bool delim){
	String mac = WiFi.macAddress();
	if(!delim){
		mac.replace(":", "");
	}
	return mac;
}
