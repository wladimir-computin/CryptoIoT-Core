/*
* CryptoIoT - WiFiManager
* 
* Class for managing WiFi Access Point and Client Mode
*/

#pragma once

#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
	#include <WiFi.h>
	#include "esp_sntp.h"
#endif
#include <Ticker.h>
#include <PrintDebug.h>

const char KEY_WIFISSID[] = "ssid";
const char KEY_WIFIPASS[] = "pass";
const char KEY_WIFIMODE[] = "mode";
const char KEY_WIFI_HYBRID_SSID[] = "hybrid_ap_ssid";
const char KEY_WIFI_HYBRID_PASS[] = "hybrid_ap_pass";
const char KEY_WIFI_AP_IP[] = "ap_ip";

enum WiFiModes {AP, HYBRID, CLIENT, OFF};

class WiFiManager{

  public:

    void setMode(WiFiModes mode);
    WiFiModes getMode();
    void applyMode();
    bool setCredentials(String &ssid, String &pass, String &hybrid_ap_ssid, String &hybrid_ap_pass);
    bool setCredentials(String &ssid, String &pass);
    bool setApIp(String &ip);
    bool setHostname(String &hostname);
    void init();
    String getIP();
    String getSSID();
    String getStatus();
    String mode2string(WiFiModes m);
    WiFiModes string2mode(String m);
    
    static WiFiManager& instance() {
      static WiFiManager _instance;
      return _instance;
    }
    ~WiFiManager() {}

  private:

    WiFiModes mode = AP;
    String ssid;
    String pass;
    String hybrid_ap_ssid;
    String hybrid_ap_pass;
    String hostname;
    IPAddress ap_ip;
    Ticker watchdog;
	const int watchdog_seconds_alert = 5 * 60;
    int watchdog_seconds;
    static void watchdogTick_static(void * context);
    void watchdogTick();

    WiFiManager() {}
    WiFiManager( const WiFiManager& );
    WiFiManager & operator = (const WiFiManager &);
  
};
