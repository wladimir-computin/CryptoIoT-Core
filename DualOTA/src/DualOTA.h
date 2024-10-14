/*
* CryptoMX-3 - OTA
* 
* Static class which parses and encrypts/decrypts custom CryptoMX-3 TCP messages.
*/

#pragma once

#include <PrintDebug.h>
#include <ArduinoOTA.h>
#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266HTTPUpdateServer.h>
	#include <ESP8266WebServer.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
	#include <WebServer.h>
	#include <Update.h>
	#include "StreamString.h"
#endif

#include <LittleFS.h>
#include <StatusLED.h>

class DualOTA{
  public:
	DualOTA(StatusLED * led = nullptr);
    void start(const char * hostname, const char * url_path, int http_port, int arduino_port);
    void loop();
	bool started();
	
  private:
	StatusLED * led;
	bool enabled = false;
#ifdef ARDUINO_ARCH_ESP8266
	ESP8266WebServer httpUpdateServer;
    ESP8266HTTPUpdateServer httpUpdater;
#endif
#ifdef ARDUINO_ARCH_ESP32
    String _updateError;
    void setUpdaterError();
#endif
};
