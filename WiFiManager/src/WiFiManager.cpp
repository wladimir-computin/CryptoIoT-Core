/*
* CryptoIoT - WiFiManager
* 
* (implementation)
*/

#include "WiFiManager.h"

volatile bool emergency_hybrid_mode = false;
volatile bool skip_disconnect_event = false;

#ifdef ARDUINO_ARCH_ESP32
#define WiFiEventHandler WiFiEventId_t
#endif

WiFiEventHandler StationModeConnected;
WiFiEventHandler StationModeGotIP;
WiFiEventHandler StationModeDisconnected;
WiFiEventHandler SoftAPModeStationConnected;
WiFiEventHandler SoftAPModeStationDisconnected;

void stationModeConnected(){
  printDebug("[WiFi] Connection to AP established!");
  if(emergency_hybrid_mode){
    printDebug("[WiFi] Connection to AP restored, leaving Hybrid-Mode!");
    skip_disconnect_event = true;
    emergency_hybrid_mode = false;
    WiFiManager::instance().setMode(CLIENT);
    WiFiManager::instance().applyMode();
  } else {
    skip_disconnect_event = false;
  }
}

void stationModeGotIP(){
  printDebug("[WiFi] Got IP from DHCP: " + WiFi.localIP().toString());
}

void stationModeDisconnected(){
  if(!emergency_hybrid_mode && !skip_disconnect_event){
    printDebug("[WiFi] Connection to AP lost!");
    emergency_hybrid_mode = true;
    WiFiManager::instance().setMode(HYBRID);
    WiFiManager::instance().applyMode();
  }
}

void softAPModeStationConnected(){
  printDebug("[WiFi] Client connected to AP");
  if(emergency_hybrid_mode){
    WiFi.setAutoReconnect(false);
  }
}

void softAPModeStationDisconnected(){
  printDebug("[WiFi] Client disconnected from AP");
  if(emergency_hybrid_mode){
    WiFi.setAutoReconnect(true);
  }
}

#ifdef ARDUINO_ARCH_ESP32
void onStationModeConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  stationModeConnected();
}

void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  stationModeGotIP();
}

void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  stationModeDisconnected();
}

void onSoftAPModeStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  softAPModeStationConnected();
}

void onSoftAPModeStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  softAPModeStationDisconnected();
}

#else
void onStationModeConnected(const WiFiEventStationModeConnected& event) {
  stationModeConnected();
}

void onStationModeGotIP(const WiFiEventStationModeGotIP& event) {
  stationModeGotIP();
}

void onStationModeDisconnected(const WiFiEventStationModeDisconnected& event) {
  stationModeDisconnected();
}

void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected& event) {
  softAPModeStationConnected();
}

void onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& event) {
  softAPModeStationDisconnected();
}
#endif

bool WiFiManager::setCredentials(String &ssid, String &pass, String &hybrid_ap_ssid, String &hybrid_ap_pass){
  if (!(pass.length() >= 8 && pass.length() <= 63))
    return false;
  if (!(ssid.length() >= 1 && ssid.length() <= 32))
    return false;
  
  if (!((hybrid_ap_pass.length() >= 8 && hybrid_ap_pass.length() <= 63) || hybrid_ap_pass.length() == 0))
    return false;
  if (!((hybrid_ap_ssid.length() >= 1 && hybrid_ap_ssid.length() <= 32) || hybrid_ap_ssid.length() == 0))
    return false;

  this->ssid = ssid;
  this->pass = pass;
  this->hybrid_ap_ssid = hybrid_ap_ssid;
  this->hybrid_ap_pass = hybrid_ap_pass;

  return true;
}

bool WiFiManager::setCredentials(String &ssid, String &pass){
  String empty = "";
  return setCredentials(ssid, pass, empty, empty);
}

bool WiFiManager::setHostname(String &hostname){
  this->hostname = hostname;
  return true;
}

bool WiFiManager::setApIp(String &ip){
  return ap_ip.fromString(ip);
}

void WiFiManager::setMode(WiFiModes mode){
  this->mode = mode;
}

WiFiModes WiFiManager::getMode(){
  return mode;
}

void WiFiManager::applyMode(){
  WiFi.mode(WIFI_OFF);
  delay(1);
  switch(mode){
    case AP:{
    printDebug("[WiFi] Initializing AP-Mode");
      printDebug("[WiFi] Starting WiFi-AP: " + ssid + ":" + pass);
      WiFi.setHostname(hostname.c_str());
      WiFi.mode(WIFI_AP);
	  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
      WiFi.softAP(ssid.c_str(), pass.c_str());
	}
    break;
    case CLIENT:{
      printDebug("[WiFi] Initializing Client-Mode");
      printDebug("[WiFi] Connecting to: " + ssid + ":" + pass);
      WiFi.setHostname(hostname.c_str());
      WiFi.mode(WIFI_STA);
      WiFi.setHostname(hostname.c_str());
      WiFi.begin(ssid.c_str(), pass.c_str());
	}
    break;
    case HYBRID:{
      printDebug("[WiFi] Initializing Hybrid-Mode");
      printDebug("[WiFi] Starting WiFi-AP: " + hostname + ":" + pass);
      printDebug("[WiFi] Connecting to: " + ssid + ":" + pass);
      WiFi.setHostname(hostname.c_str());
      WiFi.mode(WIFI_AP_STA);
	  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
      String * h_ssid = &hostname;
      String * h_pass = &pass;
      if(hybrid_ap_ssid != ""){
        h_ssid = &hybrid_ap_ssid;
      }
      if(hybrid_ap_pass != ""){
        h_pass = &hybrid_ap_pass;
      }
      WiFi.softAP(h_ssid->c_str(), h_pass->c_str());
      WiFi.begin(ssid.c_str(), pass.c_str());
	}
    break;
    case OFF:{
      printDebug("[WiFi] Disabling WiFi");
	}
    break;
  }
}

void WiFiManager::init(){
#ifdef ARDUINO_ARCH_ESP32
  StationModeConnected = WiFi.onEvent(onStationModeConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  StationModeGotIP = WiFi.onEvent(onStationModeGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  StationModeDisconnected = WiFi.onEvent(onStationModeDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  SoftAPModeStationConnected = WiFi.onEvent(onSoftAPModeStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  SoftAPModeStationDisconnected = WiFi.onEvent(onSoftAPModeStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
#else
  StationModeConnected = WiFi.onStationModeConnected(onStationModeConnected);
  StationModeGotIP = WiFi.onStationModeGotIP(onStationModeGotIP);
  StationModeDisconnected = WiFi.onStationModeDisconnected(onStationModeDisconnected);
  SoftAPModeStationConnected = WiFi.onSoftAPModeStationConnected(onSoftAPModeStationConnected);
  SoftAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected(onSoftAPModeStationDisconnected);
#endif
  
  WiFi.persistent(false);
  watchdog.attach_ms(1000, watchdogTick_static, (void*)this);
  #ifdef ARDUINO_ARCH_ESP32
  #else
  WiFi.setPhyMode(WIFI_PHY_MODE_11N); 
  #endif
  applyMode();
}

String WiFiManager::getIP(){
  if (mode == AP){
    return WiFi.softAPIP().toString();
  } else if (mode == CLIENT){
    return WiFi.localIP().toString();
  } else {
    return WiFi.softAPIP().toString();
  }
}

String WiFiManager::getSSID(){
  if (mode == AP){
    return WiFi.softAPSSID();
  } else if (mode == CLIENT){
    return WiFi.SSID();
  } else {
    return WiFi.SSID();
  }
}

String WiFiManager::getMac(){
  char str[20];
  uint8_t buf[6];
  #if (defined ARDUINO_ARCH_ESP32 && defined ARDUINO_ESP32_CORE_V3)
      Network.macAddress(buf);
  #else
      WiFi.macAddress(buf);
  #endif
  snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
  return str;
}

float WiFiManager::getSignalStrength(){
  return min(max(2 * (WiFi.RSSI() + 100), 0), 100) / 100.0;
}

NetworkInfo WiFiManager::getNetworkInfo(){
  NetworkInfo info;
  info.mode = mode2string(mode);

  String wifi_status[] = {"WL_IDLE_STATUS",
                        "WL_NO_SSID_AVAIL",
                        "WL_SCAN_COMPLETED",
                        "WL_CONNECTED",
                        "WL_CONNECT_FAILED",
                        "WL_CONNECTION_LOST",
                        "WL_DISCONNECTED"};
  int status_i = WiFi.status();
  String status_s = "";
  if(status_i >= 0 && status_i <= 6){
    info.connection_state = wifi_status[status_i];
  }

  info.mac = getMac();
  info.hostname = hostname;
  info.sta_ip = WiFi.localIP().toString();
  info.ap_ip = WiFi.softAPIP().toString();
  info.sta_ssid = WiFi.SSID();
  info.ap_ssid = WiFi.softAPSSID();
  info.signal = getSignalStrength();
  return info;
}

String WiFiManager::getStatus(){
  NetworkInfo info = getNetworkInfo();
  
  return "Mode: " + info.mode + "\n" +
         "Status: " + info.connection_state + "\n" +
         "SSID (AP): " + info.ap_ssid + "\n" +
         "SSID (STA): " + info.sta_ssid + "\n" +
         "Signal: " + String((int)(info.signal*100)) + "%\n" +
         "IP (AP): " + info.ap_ip + "\n" +
         "IP (STA): " + info.sta_ip;
}

void WiFiManager::watchdogTick_static(void * context){
  (*(WiFiManager*)context).watchdogTick();
}

void WiFiManager::watchdogTick(){
  int status = WiFi.status();
  if((mode == CLIENT || mode == HYBRID) && status != WL_CONNECTED){
	  if(watchdog_seconds++ >= watchdog_seconds_alert){
		  applyMode();
		  watchdog_seconds = 0;
	  }
  } else {
	  watchdog_seconds = 0;
  }
}

String WiFiManager::mode2string(WiFiModes m){
  switch (m){
   case AP:
	   return "AP";
   case CLIENT:
	   return "CLIENT";
   case HYBRID:
	   return "HYBRID";
   case OFF:
	   return "OFF";
  }
  return "AP";
}

WiFiModes WiFiManager::string2mode(String m){
  m.toUpperCase();
  if(m == "AP"){
    return AP;
  }
  if(m == "HYBRID"){
    return HYBRID;
  }
  if(m == "CLIENT"){
    return CLIENT;
  }
  if(m == "OFF"){
    return OFF;
  }
  return AP;
}




/*

*/
