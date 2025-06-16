/*
  CryptoIoT

  A pracitcal example of a secure IOT device. (still rare nowadays!)

  CryptoIoT uses a challenge-response system over TCP based on AES-GCM and SHA512 for key generation.
  Challenge-response has some major benefits over a rolling code based exchange, for example
  immunity to replay-attacks and easy managment of multiple remote devices, in our case smartphones.

  CryptoIoT *should* be at least equally or more secure than any comparable commercially available system.
  It isn't relying on any closed source / hidden algorithms. (Except for the hardware random generator in
  our ESP8266 for IV generation, which seems to be fine but that's the problem with random, you never know for sure.)
  Replay attacks are not possible and AFAIK the "best" way to get the key is to caputure the encrypted
  HELLO message of the client and try to brute force it. Since we are using AES256 in GCM-mode this
  should take plenty of time and breaking into the garage using dynamite is faster. As always, you MUST
  choose a strong password, 123456 won't last that long. BTW, keep in mind that physical access to the
  device renders any software security useless.

  I personally use CryptoIoT in AP-Mode, not connected to the internet. But I designed the system
  to be secure independent of the surrounding network. Exposing it as a client to the homenetwork should be fine.
  Even exposing it to the internet should work (however persistent denial-of-service attacks may
  render it unusable.) But I wouldn't recommend to make *any* IOT accessible from the internet without VPN.

  I DO NOT GUARANTEE ANYTHING!
  If someone break into your garage and steals your fancy car, I won't buy you a new one.

  If you find bugs, contact me :)
*/

#pragma once

//Build configuration
#include "Config.h"
#include <Arduino.h>

//WiFi AccessPoint and TCP/UDP
#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
	#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <LittleFS.h>

#include <ArduinoJson.h>

#include <PersistentMemory.h>
#include <WiFiManager.h>
#include <Uptime.h>
#include <DualOTA.h>
#include <CIoT_Time.h>
#include <Recovery.h>
#include <LogBuffer.h>
#include <PrintDebug.h>

#include "CryptoIoT_API.h"
#include "System.h"
#include "App.h"
#include "Crypto.h"
#include "ChallengeManager.h"
#include "RateLimit.h"
#include "Message.h"
#include "EventManager.h"
#include "TimerManager.h"
#include "Reboot.h"

#include <StatusLED.h>

//const char COMMAND_SET_DEVICE_PASS[] = "setDevicePass";
//const char COMMAND_SET_WIFI_SSID[] = "setSSID";
//const char COMMAND_SET_WIFI_PASS[] = "setWIFIPass";
//const char COMMAND_SET_WIFI_MODE[] = "setWIFIMode";

static Param COMMAND_READ_SETTING_PARAMS[] = {{"Vault", DATATYPE_STRING, true}, {"Key", DATATYPE_STRING, true}};
static Param COMMAND_WRITE_SETTING_PARAMS[] = {{"Vault", DATATYPE_STRING}, {"Key", DATATYPE_STRING}, {"Value", DATATYPE_STRING}};
static Param COMMAND_RESET_PARAMS[] = {{"Vault", DATATYPE_STRING, true}, {"Key", DATATYPE_STRING, true}};
static Param COMMAND_DELAYED_PARAMS[] = {{"delay_ms", DATATYPE_INT, false, 0, 1000*60*60*24}, {"Command", DATATYPE_STRING}};
static Param COMMAND_API_PARAMS[] = {{"App", DATATYPE_STRING, true}};
static Param COMMAND_HELP_PARAMS[] = {{"App", DATATYPE_STRING, true}};
static Param COMMAND_SETTIME_PARAMS[] = {{"Epoch", DATATYPE_INT}};

static Command COMMAND_APPS("apps");
static Command COMMAND_DEVICES("devices");
static Command COMMAND_API("api", ARRAY_LEN(COMMAND_API_PARAMS), COMMAND_API_PARAMS);
static Command COMMAND_HELP("help", ARRAY_LEN(COMMAND_HELP_PARAMS), COMMAND_HELP_PARAMS);
static Command COMMAND_LOG("applog");
static Command COMMAND_GET_STATUS("status");
static Command COMMAND_READ_SETTING("reads", ARRAY_LEN(COMMAND_READ_SETTING_PARAMS), COMMAND_READ_SETTING_PARAMS);
static Command COMMAND_WRITE_SETTING("writes", ARRAY_LEN(COMMAND_WRITE_SETTING_PARAMS), COMMAND_WRITE_SETTING_PARAMS);
static Command COMMAND_RESET("reset", ARRAY_LEN(COMMAND_RESET_PARAMS), COMMAND_RESET_PARAMS);
static Command COMMAND_REBOOT("reboot");
static Command COMMAND_DELAYED("delayed", ARRAY_LEN(COMMAND_DELAYED_PARAMS), COMMAND_DELAYED_PARAMS);
static Command COMMAND_DISCOVER("discover");
static Command COMMAND_PING("ping");
static Command COMMAND_UPDATE("update");   
static Command COMMAND_WIFISCAN("wifiscan");
static Command COMMAND_WIFIRESULTS("wifiresults");
static Command COMMAND_SETTIME("settime",  ARRAY_LEN(COMMAND_SETTIME_PARAMS), COMMAND_SETTIME_PARAMS);

static Command * commands[] = {&COMMAND_HELP, &COMMAND_APPS, &COMMAND_API, &COMMAND_LOG, &COMMAND_GET_STATUS, &COMMAND_READ_SETTING, &COMMAND_WRITE_SETTING, &COMMAND_RESET, &COMMAND_REBOOT, &COMMAND_DELAYED, &COMMAND_DISCOVER, &COMMAND_PING, &COMMAND_UPDATE, &COMMAND_WIFISCAN, &COMMAND_WIFIRESULTS, &COMMAND_SETTIME};

const char KEY_DEVICEPASS[] = "devicepass";
const char KEY_HOSTNAME[] = "hostname";
const char KEY_CHALLENGE_TIMEOUT[] = "challenge_timeout_seconds";

class CryptoIoT {
  public:
    CryptoIoT(App ** apps, int apps_len);
    void loop();
    void setup();
	void setDeviceType(const char * type);
	void setAppVersion(const char * version);
    ProcessMessageStruct processMessage(String &message);
  
  private:
    String hostname;
	String devicetype = "CryptoGeneric";
	String appversion = "1.0";
    //OTA update
    App ** apps;
    int apps_len;
	
	
#if CRYPTOIOT_ENABLETCP == 1
    //TCP server on port 4646
    WiFiServer tcpserver;
    //TCP and UDP Client, global because we want to keep tcp connections between loops alive
    WiFiClient tcpclient;
#endif
    WiFiUDP udpclient;

    StatusLED led;
    Uptime &uptime = Uptime::instance();
    Time mytime;
	
    System system;
    WiFiManager &wifi = WiFiManager::instance();
	Crypto crypto;
    ChallengeManager challengeManager;
    RateLimit rateLimit;
	Recovery recovery;
    EventManager eventManager;
    TimerManager timerManager;
	Reboot reboot;
    DualOTA ota;
    LogBuffer applog;
    CryptoIoT_API api = {&led, &mytime, &eventManager, &timerManager, &system, &applog};

    void setupWiFi();
    void initSystem();
    void initFS();
    void initHardware();
    void enableOTA();
    void send_Data_UDP(String data, IPAddress ip, int port);
    void send_Data_UDP(String data);
    String receive_Data_UDP();
    void stopClient_UDP();
#if CRYPTOIOT_ENABLETCP == 1
    void send_Data_TCP(WiFiClient &tcpclient, String data);
    String receive_Data_TCP(WiFiClient &tcpclient);
    void stopClient_TCP(WiFiClient &tcpclient);
    void doTCPServerStuff();
#endif
    void doUDPServerStuff();
};
