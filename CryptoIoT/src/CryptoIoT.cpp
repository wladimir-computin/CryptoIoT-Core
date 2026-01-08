/*
 *  CryptoIoT - Main
 * 
 *  (implementation)
 */

#include "CryptoIoT.h"
#include "Config.h"

#if CRYPTOIOT_ENABLETCP == 1
#include "TCPCleanup.h"
#endif

String discover = String(ENCRYPTED_CIOTv2_MESSAGE) + COMMAND_DISCOVER.str_command;

CryptoIoT::CryptoIoT(App ** apps, int apps_len) : eventManager(this, processMessageStatic, &mytime), timerManager(apps, apps_len) ,ota(&led), applog(APPLOG_SIZE)
#if CRYPTOIOT_ENABLETCP == 1
, tcpserver(TCP_SERVER_PORT)
#endif
{
	this->apps = apps;
	this->apps_len = apps_len;
}

void CryptoIoT::loop() {
	recovery.loop();
	ota.loop();
	eventManager.loop();
	timerManager.loop();
	
	for (int i = 0; i < apps_len; i++) {
		apps[i]->loop();
	}
	
	doUDPServerStuff();
#if CRYPTOIOT_ENABLETCP == 1
	doTCPServerStuff();
#endif
	
	#if DEBUG == 1 || CRYPTOIOT_UNENCRYPTED_SERIAL == 1
	if (Serial.available()) {
		String cmd = Serial.readStringUntil('\n');
		Serial.println(cmd);
		ProcessMessageStruct out = processMessage(cmd);
		Serial.printf("%c:%s:%s\n", Message::typeToChar(out.responseCode), out.flags.c_str(), out.responseData.c_str());
	}
	#endif
}
void CryptoIoT::setup() {
	initHardware();
	printDebug("\nLoading CryptoIoT Firmware: " + String(SYS_VERSION));
	initFS();
	initSystem();
	setupWiFi();
#if CRYPTOIOT_ENABLETCP == 1
	printDebug("[SYS] Starting TCP server on port " + String(TCP_SERVER_PORT));
	tcpserver.begin();
#endif
	printDebug("[SYS] Starting UDP server on port " + String(UDP_SERVER_PORT));
	udpclient.begin(UDP_SERVER_PORT);
	printDebug("[SYS] Initializing Time/Clock");
	mytime.setup();
	printDebug("[SYS] Initializing EventManager");
	eventManager.setup();
	timerManager.setup();
	for (int i = 0; i < apps_len; i++) {
		printDebug("[SYS] Initializing app: " + apps[i]->getName());
		apps[i]->giveAPIAccess(api);
		apps[i]->setup();
	}
	uptime.start();
	printDebug("[SYS] Free HEAP: " + String(ESP.getFreeHeap()));
	printDebug("Bootsequence finished in " + String(millis()) + "ms" + "!\n");
	led.fade(StatusLED::SINGLE_ON_OFF, 2000);
}

void CryptoIoT::setDeviceType(const char * type){
	devicetype = type;
}

void CryptoIoT::setAppVersion(const char * version){
	appversion = version;
}

void CryptoIoT::setupWiFi() {
	printDebug("[SYS] Initialising WiFi");
	PersistentMemory pmem("wifi", true);
	String ssid = pmem.readString(KEY_WIFISSID, DEFAULT_WIFISSID);
	String pass = pmem.readString(KEY_WIFIPASS, DEFAULT_WIFIPASS);
	String mode = pmem.readString(KEY_WIFIMODE, DEFAULT_WIFIMODE);
	String ssid_hybrid_ap = pmem.readString(KEY_WIFI_HYBRID_SSID, DEFAULT_WIFI_HYBRID_SSID);
	String pass_hybrid_ap = pmem.readString(KEY_WIFI_HYBRID_PASS, DEFAULT_WIFI_HYBRID_PASS);
	String ip = pmem.readString(KEY_WIFI_AP_IP, DEFAULT_WIFI_AP_IP);
	pmem.commit();
	
	wifi.setCredentials(ssid, pass, ssid_hybrid_ap, pass_hybrid_ap);
	wifi.setApIp(ip);
	wifi.setHostname(hostname);
	wifi.setMode(wifi.string2mode(mode));
	wifi.init();
}
void CryptoIoT::initSystem() {
	printDebug("[SYS] Initialising CryptoSystem");
	PersistentMemory pmem("system", true);
	hostname = pmem.readString(KEY_HOSTNAME, DEFAULT_HOSTNAME);
	String devicepass = pmem.readString(KEY_DEVICEPASS, DEFAULT_DEVICEPASS);
	int challenge_validity_timeout = pmem.readInt(KEY_CHALLENGE_TIMEOUT, DEFAULT_CHALLENGE_VALIDITY_TIMEOUT);
	pmem.commit();
	crypto.init(devicepass);
	challengeManager.setChallengeTimeout(challenge_validity_timeout);
}
void CryptoIoT::initFS() {
	printDebug("[SYS] Initialising FileSystem");
	#ifdef ARDUINO_ARCH_ESP32
		LittleFS.begin(true);
	#else
		LittleFS.begin();
	#endif
	recovery.start();
}
void CryptoIoT::initHardware() {
	#if DEBUG == 1 //defined in AllConfig.h
	Serial.begin(115200);
	printDebug("Device running in Debug-Mode!");
	#endif
	#ifdef ARDUINO_ARCH_ESP8266
	analogWriteRange(1023);
	#endif
}

void CryptoIoT::enableOTA() {
	if (!ota.started()) {
		ota.start(hostname.c_str(), UPDATE_PATH, HTTP_OTA_PORT, ARDUINO_OTA_PORT);
	}
}
void CryptoIoT::send_Data_UDP(String data, IPAddress ip, int port) {
	data = Message::wrap(data);
	printDebug("\n[UDP] Sending:\n" + data + "\n");
	udpclient.beginPacket(ip, port);
	udpclient.write((uint8_t*)data.c_str(), data.length());
	udpclient.endPacket();
}
void CryptoIoT::send_Data_UDP(String data) {
	send_Data_UDP(data, udpclient.remoteIP(), udpclient.remotePort());
}
String CryptoIoT::receive_Data_UDP() {
	int packetSize = udpclient.parsePacket();
	if (packetSize) {
		char incomingPacket[MAX_MESSAGE_LEN];
		int len = udpclient.read(incomingPacket, sizeof(incomingPacket) - 1);
		if (len > 0) {
			incomingPacket[len] = '\0';
			printDebug("\n[UDP] Received:\n" + String(incomingPacket));
			return Message::unwrap(incomingPacket);
		}
	}
	return "";
}
void CryptoIoT::stopClient_UDP() {
	rateLimit.setState(BLOCKED);
	yield();
}

#if CRYPTOIOT_ENABLETCP == 1

void CryptoIoT::send_Data_TCP(WiFiClient &tcpclient, String data) {
 data = Message::wrap(data);
 printDebug("\n[TCP] Sending:\n" + data + "\n");
 tcpclient.println(data);
 yield();
}
String CryptoIoT::receive_Data_TCP(WiFiClient &tcpclient) {
 if (tcpclient.available()) {
   String incoming = tcpclient.readStringUntil('\n');
   tcpclient.flush();
   yield();
   printDebug("\n[TCP] Received:\n" + incoming);
   String body = Message::unwrap(incoming);
   return body;
 }
 return "";
}
void CryptoIoT::stopClient_TCP(WiFiClient &tcpclient) {
 rateLimit.setState(BLOCKED);
 tcpclient.stop();
 yield();
}
void CryptoIoT::doTCPServerStuff() {

 if (!tcpclient || !tcpclient.connected()) { //garbage collection
   if (tcpclient) {
     tcpclient.stop();
   }
   // wait for a client to connect
   tcpclient = tcpserver.available();
   tcpclient.setTimeout(TCP_TIMEOUT_MS);
   tcpclient.setNoDelay(true);
   tcpclient.setSync(true);
 }

 if (rateLimit.getState() == OPEN) { //Check if we are ready for a new connection

   String s = receive_Data_TCP(tcpclient);

   if (s != "") {

     Msg message; //Msg defined in Message.h

     Message::decrypt(message, crypto, s, challengeManager);

     if (message.type != NOPE) {

       if (message.type == HELLO) {
         //preparing random challenge secret
         send_Data_TCP(tcpclient, Message::encrypt(crypto, challengeManager, HELLO, "", FLAG_NONE));
         //sending encrypted challenge to client, the client has to prove its knowledge of the correct password by encrypting the next
         //message containing our challenge. We have proven our knowledge by sending the reencrypted client_challenge back.

         //the client send a correctly encrypted second message.
       } else if (message.type == DATA) {
         ProcessMessageStruct ret = processMessage(message.data); //process his data
         //send answer
         send_Data_TCP(tcpclient, Message::encrypt(crypto, challengeManager, ret.responseCode, ret.responseData, ret.flags));

         //for some messages (like setSettings) we know that the client may send a following one very quickly.
         //other messages (like trigger) shouldn't be called that quickly.
         if (!(ret.flags.indexOf(FLAG_KEEP_ALIVE) != -1)) {
           stopClient_TCP(tcpclient);
         }
       } else {
         printDebug("Wrong formatted message received!");
         send_Data_TCP(tcpclient, Message::encrypt(crypto, challengeManager, ERR, "Nope!", FLAG_NONE));
         stopClient_TCP(tcpclient);
       }

     } else {
       printDebug("Decryption failed!");
       challengeManager.resetChallenge();
       send_Data_TCP(tcpclient, Message::encrypt(crypto, challengeManager, ERR, "Nope!", FLAG_NONE));
       stopClient_TCP(tcpclient);
     }
     printDebug("-------------------------------");
   }
 } else {
   tcpclient.flush();
   tcpclient.stop();
   yield();
   tcpCleanup();
 }
}
 
#endif

void CryptoIoT::doUDPServerStuff() {
	
	if (rateLimit.getState() == OPEN) { //Check if we are ready for a new connection
		
		String s = receive_Data_UDP();
		
		if (s != "") {
			printDebug(s);
			if (s == discover) {
				char out[64] = {0};
				snprintf(out, ARRAY_LEN(out), "%s%s:%s:%s", ENCRYPTED_CIOTv2_MESSAGE, devicetype.c_str(), hostname.c_str(), system.getSerialID(false).c_str());
				send_Data_UDP(String(out));
				stopClient_UDP();
				return;
			}
			
			Msg message; //Msg defined in Message.h
			
			Message::decrypt(message, crypto, s, challengeManager);
			if (message.type != NOPE) {
				printfDebug("%c:%s", Message::typeToChar(message.type), message.data.c_str());
				//check if the client send a correctly encrypted message.
				
				if (message.type == HELLO || (message.type == ERR && message.data == "Challenge missmatch!")) {
					send_Data_UDP(Message::encrypt(crypto, challengeManager, HELLO, "", FLAG_NONE));
					//sending  challenge to client, the client has to prove its knowledge of the correct password by encrypting the next
					//message containing our challenge. We have proven our knowledge by sending the reencrypted client_challenge back.
					
				} else if (message.type == DATA) {
					ProcessMessageStruct ret = processMessage(message.data); //process his data
					
					send_Data_UDP(Message::encrypt(crypto, challengeManager, ret.responseCode, ret.responseData, ret.flags)); //send answer
					
					//for some messages (like setSettings) we know that the client may send a following one very quickly.
					//other messages (like trigger) shouldn't be called that quickly.
					if (ret.flags.indexOf(FLAG_KEEP_ALIVE) == -1) {
						stopClient_UDP();
					}
				} else {
					printDebug("Wrong formatted message received!");
					send_Data_UDP(Message::encrypt(crypto, challengeManager, ERR, "Nope!", FLAG_NONE));
					stopClient_UDP();
				}
				
			} else {
				printDebug("Decryption failed!");
				challengeManager.resetChallenge();
				send_Data_UDP(Message::encrypt(crypto, challengeManager, ERR, "Nope!", FLAG_NONE));
				stopClient_UDP();
			}
			printDebug("-------------------------------");
		}
	}
}
ProcessMessageStruct CryptoIoT::processMessage(String &message) {
	String appname = Message::getParam(message,0);
	for (int i = 0; i < apps_len; i++) {
		if(apps[i]->getName() == appname){
			String m = Message::getParam(message,1,true);
			ProcessMessageStruct p = apps[i]->processMessage(m);
			if (!(p.responseCode == ERR && p.responseData == NO_COMMAND)) {
				return p;
			}
		}
	}

	if (COMMAND_PING.check(message)) {
		return {ACK, ""};
	}
	
	if (COMMAND_DISCOVER.check(message)) {
		return {DATA, devicetype + ":" + hostname + ":" + system.getSerialID(false)};
	}
	
	if (COMMAND_GET_STATUS.check(message)) {
		String appstatus = "";
		for (int i = 0; i < apps_len; i++) {
			appstatus += "\n[" + apps[i]->getName() + "]\n" + apps[i]->getStatus() + "\n";
		}
		
		char formats[] =
		"Hostname: %s\n"
		"System-Version: %s\n"
		"App-Version: %s\n"
		"Updatemode: %d\n"
		"Free Heap: %dByte\n"
		"Heap Fragmentation: %d%%\n"
		"Uptime: %s\n"
		"\n"
		"[Time]\n"
		"Time: %s\n"
		"Sunrise: %s\n"
		"Sunset: %s\n"
		"\n"
		"[WiFi]\n%s\n"
		"MAC: %s\n"
		"\n"
		"[EventManager]\n%s\n"
		"[TimerManager]\n%s\n"
		"%s";
		
		char buf[1024];
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), formats,
				hostname.c_str(),
				SYS_VERSION,
				appversion.c_str(),
				ota.started(),
				ESP.getFreeHeap(),
#ifdef ARDUINO_ARCH_ESP8266
				ESP.getHeapFragmentation(),
#endif
#ifdef ARDUINO_ARCH_ESP32
				0,
#endif
				uptime.getUptime().c_str(),
				mytime.stringTime().c_str(),
				Time::min2str(mytime.custom_sunrise_minutes()).c_str(),
				Time::min2str(mytime.custom_sunset_minutes()).c_str(),
				wifi.getStatus().c_str(),
				system.getSerialID().c_str(),
				eventManager.getStatus().c_str(),
				timerManager.getStatus().c_str(),
				appstatus.c_str()
		);
		return {DATA, buf};
	}
	
	if (COMMAND_WIFISCAN.check(message)){
		printDebug("[WiFi] Performing WiFi scan...");
		int scanResult = WiFi.scanNetworks(/*async=*/true, /*hidden=*/true);
		delay(1);
		return {ACK, ""};
	}
	
	if (COMMAND_WIFIRESULTS.check(message)){
		printDebug("[WiFi] Performing WiFi scan...");
		int scanResult = WiFi.scanComplete();

		if (scanResult == 0) {
			printDebug("[WiFi] No networks found");
		} else if (scanResult == -1) {
			printDebug("[WiFi] WiFi Scan still ongoing");
			return {ERR, "WiFi Scan not complete"};
		} else if (scanResult == -2) {
			printDebug("[WiFi] WiFi Scan not started");
			return {ERR, "WiFi Scan not started"};
		} else if (scanResult > 0) {
			JsonDocument json;
			JsonArray jsonArray = json.to<JsonArray>();
			String ssid;
			ssid.reserve(32);
			int32_t rssi;
			uint8_t encryptionType;
			uint8_t* bssid;
			int32_t channel;
			bool hidden;
			
			for (int i = 0; i < min(scanResult, 10); i++) {
#ifdef ARDUINO_ARCH_ESP8266
				WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
#endif
#ifdef ARDUINO_ARCH_ESP32
				WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel);
#endif			
				JsonObject jsonObj = jsonArray.add<JsonObject>();
				jsonObj["ssid"] = ssid;
				jsonObj["bssid"] = WiFi.BSSIDstr(i);
				jsonObj["enc"] = encryptionType;
				jsonObj["ch"] = channel;
				jsonObj["rssi"] = rssi;
				jsonObj["hidden"] = hidden;
			}
			WiFi.scanDelete();
			String out;
			out.reserve(128*scanResult);
			serializeJson(json, out);
			out.replace("},{", "},\n{");
			return {DATA, out};
		} else {
			printfDebug("WiFi scan error: %d", scanResult);
		}
		return {DATA, "[]"};
	}
	
	if (COMMAND_REBOOT.check(message)) {
		system.reboot(1000);
		return {ACK, ""};
	}
	
	if (COMMAND_LOG.check(message)) {
		return {DATA, applog.getString(), FLAG_KEEP_ALIVE};
	}
	
	if (COMMAND_READ_SETTING.check(message)) {
		String vault = COMMAND_READ_SETTING.getParamVal(message, 0);
		String key = COMMAND_READ_SETTING.getParamVal(message, 1);
		
		if (vault != "" && key != "") {
			PersistentMemory pmem(vault);
			if (pmem) {
				String value = pmem.readString(key, "EMPTY");
				if (value != "EMPTY") {
					return {DATA, value, FLAG_KEEP_ALIVE};
				} else {
					return {ERR, "Key not found"};
				}
			} else {
				return {ERR, "Vault not found"};
			}
			
		} else if (vault != "") {
			String json = PersistentMemory::toJSON(vault);
			if (json != "") {
				return {DATA, json, FLAG_KEEP_ALIVE};
			} else {
				return {ERR, "Vault not found"};
			}
			
		} else {
			String arr[20];
			int len = PersistentMemory::listVaults(arr, sizeof(arr) / sizeof(arr[0]));
			String out = "";
			for (int i = 0; i < len; i++) {
				out += arr[i] + "\n";
			}
			return {DATA, out, FLAG_KEEP_ALIVE};
		}
		return {ERR, "Unknown error"};
	}
	
	if (COMMAND_WRITE_SETTING.check(message)) {
		String vault = COMMAND_WRITE_SETTING.getParamVal(message, 0);
		String key = COMMAND_WRITE_SETTING.getParamVal(message, 1);
		String value = COMMAND_WRITE_SETTING.getParamVal(message, 2);
		if (vault && key && value) {
			PersistentMemory pmem(vault);
			if (pmem) {
				String test = pmem.readString(key, "EMPTY");
				if (test != "EMPTY") {
					pmem.writeString(key, value);
					pmem.commit();
					return {ACK, "", FLAG_KEEP_ALIVE};
				} else {
					return {ERR, "Key not found"};
				}
			} else {
				return {ERR, "Vault not found"};
			}
		}
		return {ERR, "Expected format: Vault:Key:Value"};
	}
	
	if (COMMAND_RESET.check(message)) {
		String vault = COMMAND_RESET.getParamVal(message, 0);
		String key = COMMAND_RESET.getParamVal(message, 1);
		
		if (vault != "" && key != "") {
			PersistentMemory pmem(vault);
			if (pmem) {
				pmem.removeKey(key);
				pmem.commit();
				return {ACK, "", FLAG_KEEP_ALIVE};
			} else {
				return {ERR, "Vault not found"};
			}
		} else if (vault != "") {
			if (PersistentMemory::remove(vault)) {
				return {ACK, ""};
			} else {
				return {ERR, "Vault not found"};
			}
		} else {
			printDebug("Clearing PMEM!");
			PersistentMemory::format();
			return {ACK, ""};
		}
	}
	
	if (COMMAND_UPDATE.check(message)) {
		enableOTA();
		return {DATA, "http://" + wifi.getIP() + ":" + HTTP_OTA_PORT + UPDATE_PATH};
	}
	
	if (COMMAND_DELAYED.check(message)) {
		int after_ms = COMMAND_DELAYED.getParamVal(message, 0).toInt();
		String command = COMMAND_DELAYED.getParamVal(message, 1);
		timerManager.execDelayed(command, after_ms);
		return {ACK, ""};
	}
	
	if (COMMAND_DEVICES.check(message) || COMMAND_APPS.check(message)) {
		JsonDocument json;
		JsonArray jsonArray = json.to<JsonArray>();
		for (int i = 0; i < apps_len; i++) {
			JsonObject jsonObj = jsonArray.add<JsonObject>();
			jsonObj[apps[i]->getName()] = apps[i]->getType();
		}
		String out;
		out.reserve(128*apps_len);
		serializeJson(json, out);
		out.replace("},{", "},\n{");
		return {DATA, out};
	}
	
	if (COMMAND_API.check(message)) {
		String appname = COMMAND_API.getParamVal(message, 0);
		JsonDocument json;
		JsonArray jsonArr = json.to<JsonArray>();
		if(appname == ""){
			for (int cmd = 0; cmd < ARRAY_LEN(commands); cmd++){
				JsonObject test = jsonArr.add<JsonObject>();
				commands[cmd]->toJSON(test);
			}
		} else {
			for (int i = 0; i < apps_len; i++) {
				if(apps[i]->getName() == appname){
					for (int cmd = 0; cmd < apps[i]->getNumCommands(); cmd++){
						JsonObject test = jsonArr.add<JsonObject>();
						apps[i]->getCommands()[cmd]->toJSON(test);
					}
				}
			}
		}
		String out;
		serializeJson(json, out);
		out.replace("},{", "},\n{");
		return {DATA, out};
	}
	
	if (COMMAND_HELP.check(message)) {
		String appname = COMMAND_HELP.getParamVal(message, 0);
		String out;
		out.reserve(512);
		char formatparam[] = ":[%s|%s]";
		char formatparamoptional[] = ":?[%s|%s]";
		char buf[128];
		if(appname == ""){
			for (int cmd = 0; cmd < ARRAY_LEN(commands); cmd++){
				out += commands[cmd]->str_command;
				for (int param = 0; param < commands[cmd]->size_params; param++){
					if(!commands[cmd]->params_arr[param].optional){
						snprintf(buf, ARRAY_LEN(buf), formatparam, commands[cmd]->params_arr[param].name, commands[cmd]->params_arr[param].datatype);
					} else {
						snprintf(buf, ARRAY_LEN(buf), formatparamoptional, commands[cmd]->params_arr[param].name, commands[cmd]->params_arr[param].datatype);
					}
					out += buf;
				}
				out += "\n";
			}
		} else {
			char formatcommand[] = "%s:%s";
			for (int i = 0; i < apps_len; i++) {
				if(apps[i]->getName() == appname){
					for (int cmd = 0; cmd < apps[i]->getNumCommands(); cmd++){
						snprintf(buf, ARRAY_LEN(buf), formatcommand, appname.c_str(), apps[i]->getCommands()[cmd]->str_command);
						out += buf;
						for (int param = 0; param < apps[i]->getCommands()[cmd]->size_params; param++){
							if(!apps[i]->getCommands()[cmd]->params_arr[param].optional){
								snprintf(buf, ARRAY_LEN(buf), formatparam, apps[i]->getCommands()[cmd]->params_arr[param].name, apps[i]->getCommands()[cmd]->params_arr[param].datatype);
							} else {
								snprintf(buf, ARRAY_LEN(buf), formatparamoptional, apps[i]->getCommands()[cmd]->params_arr[param].name, apps[i]->getCommands()[cmd]->params_arr[param].datatype);
							}
							out += buf;
						}
						out += "\n";
					}
				}
			}
		}
		return {DATA, out};
	}

	if (COMMAND_SETTIME.check(message)) {
		int epoch = COMMAND_SETTIME.getParamVal(message, 0).toInt();
		mytime.setTime(epoch);
		return {ACK, ""};
	}

	if (COMMAND_VERSION.check(message)) {
		JsonDocument json;
		JsonObject jsonObj = json.to<JsonObject>();
		jsonObj["CryptoIoT"] = SYS_VERSION;

		for (int i = 0; i < apps_len; i++) {
			jsonObj[apps[i]->getName()] = apps[i]->getVersion();
		}

		String out;
		out.reserve(64*apps_len);
		serializeJson(json, out);
		out.replace("},{", "},\n{");
		return {DATA, out};
	}

	if (COMMAND_BUILDENV.check(message)) {
		return {DATA, PIOENV};
	}
	
	printDebug("Received unknown command:" + message);
	return {ERR, "Unknown Command"};
}

void CryptoIoT::processMessageStatic(void * context, String& message) {
	((CryptoIoT*)context)->processMessage(message);
}
