/*
* CryptoIoT - Config
* Compiletime constants and defines.
* 
* Arduino IDE processes files in alphabetical order, hence the defines MUST be in a file named beginning with an "A"...
*/

#pragma once

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

//#define DEBUG 1   //Enable debug output. If you have a LCTech relay, you must disable this or the relay won't work.
//#define ENABLE_STATUS_LED 1   //Works only if the builtin LED is not connected to the same pin as the relay. Disable when using ESP-01(s) with LCTech relay

const int TCP_SERVER_PORT = 4646;
const int UDP_SERVER_PORT = 4647;
const int ARDUINO_OTA_PORT = 3232;
const int HTTP_OTA_PORT = 8266;
const int MAX_MESSAGE_LEN = 1024;
const int DEFAULT_CHALLENGE_VALIDITY_TIMEOUT = 60; //Seconds which the client has to answer the challenge message.
const int RATE_LIMIT_TIMEOUT_MS = 200; //Milliseconds between communication attempts, set to 0 to disable rate limiting.
const int TCP_TIMEOUT_MS = 100;  //Time before drop incoming transmsission as it takes too long
#ifdef ARDUINO_ARCH_ESP32
const int APPLOG_SIZE = 1024;
#else
const int APPLOG_SIZE = 512;
#endif


const char UPDATE_PATH[] = "/update";

const char DEFAULT_HOSTNAME[] = "CryptoIoT";
const char DEFAULT_WIFISSID[] = "CryptoIoT-Setup";
const char DEFAULT_WIFIPASS[] = "12345670";
const char DEFAULT_DEVICEPASS[] = "TestTest1";
const char DEFAULT_WIFIMODE[] = "AP"; //Access Point
const char DEFAULT_WIFI_HYBRID_SSID[] = "";
const char DEFAULT_WIFI_HYBRID_PASS[] = "";
const char DEFAULT_WIFI_AP_IP[] = "192.168.4.1";

const char KEY_SALT[] = "FTh.!%B$";
const int SHA_ROUNDS = 5000;

const char SYS_VERSION[] = "10.3";

