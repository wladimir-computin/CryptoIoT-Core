#include <CryptoIoTClient.h>

CryptoIoTClient::CryptoIoTClient(){
	
}

void CryptoIoTClient::send(String& host, const int port, String& command, ciot_client_callback callback, void * param){
	argument.command = command;
	argument.host.fromString(host);
	argument.port = port;
	argument.callback = callback;
	argument.param = param;
	
	clientPhase = CONNECT;
	clientTimeout.wait(2000);
}

void CryptoIoTClient::send(String& command, ciot_client_callback callback, void * param){
	argument.command = command;
	argument.callback = callback;
	argument.param = param;
	
	clientPhase = CONNECT;
	clientTimeout.wait(2000);
}
/*
void connectToGarage(){
  wifi.setCredentials("Garage", "KatzMolMiau");
  wifi.setMode(CLIENT);
  WiFi.config(IPAddress(192,168,4,12), IPAddress(192,168,4,1), IPAddress(255,255,0,0));
  wifi.applyMode();
  clientTimeout.wait(5000);
}
*/

void CryptoIoTClient::send_Data_UDP(String data, IPAddress ip, int port) {
	data = Message::wrap(data);
	printDebug("\n[CIOT] Sending:\n" + data + "\n");
	udpclient.beginPacket(ip, port);
	udpclient.write((uint8_t*)data.c_str(), data.length());
	udpclient.endPacket();
}
void CryptoIoTClient::send_Data_UDP(String data) {
	send_Data_UDP(data, udpclient.remoteIP(), udpclient.remotePort());
}
String CryptoIoTClient::receive_Data_UDP() {
	int packetSize = udpclient.parsePacket();
	if (packetSize) {
		char incomingPacket[1024];
		int len = udpclient.read(incomingPacket, sizeof(incomingPacket) - 1);
		if (len > 0) {
			incomingPacket[len] = '\0';
			printDebug("\n[CIOT] Received:\n" + String(incomingPacket));
			return Message::unwrap(incomingPacket);
		}
	}
	return "";
}

void CryptoIoTClient::doUDPClientStuff(){
	if(!clientTimeout.finished_once()){
		if(clientPhase == CONNECT){
			uint8_t challenge_response[CHALLENGE_LEN];
			uint8_t challenge_request[CHALLENGE_LEN];
			challengeManager.generateRandomChallenge(challenge_request);
			send_Data_UDP(Message::encrypt(crypto, HELLO, "", challenge_response, challenge_request, FLAG_NONE), argument.host, argument.port);
			clientPhase = PHASE1;
		} else if (clientPhase == PHASE1){
			String s = receive_Data_UDP();
			if(s != ""){
				Msg msg;
				Message::decrypt(msg, crypto, s, challengeManager);
				uint8_t challenge_request[CHALLENGE_LEN];
				challengeManager.generateRandomChallenge(challenge_request);
				if (msg.type == HELLO) {
					send_Data_UDP(Message::encrypt(crypto, DATA, argument.command, msg.challenge, challenge_request, FLAG_NONE));
					clientPhase = PHASE2;
				} else {
					clientPhase = RETURN;
				}
			}
		
		} else if (clientPhase == PHASE2){
			String s = receive_Data_UDP();
			if(s != ""){
				Msg msg;
				Message::decrypt(msg, crypto, s, challengeManager);
				if (msg.type == ACK || msg.type == DATA) {
					if(argument.callback != NULL){
						argument.callback(msg.data, argument.param);
					}
				}
				clientPhase = RETURN;
			}
		} else if (clientPhase == RETURN){
			clientPhase = OFF;
			clientTimeout.reset();
		}
	} else {
		printDebug("\n[CIOT] Timeout at PHASE " + String(clientPhase));
		clientPhase = OFF;
		clientTimeout.reset();
	}
}

void CryptoIoTClient::loop(){
	if(clientPhase != OFF){
		doUDPClientStuff();
	}
}

void CryptoIoTClient::setup(String& devicepass, String& name){
	crypto.init(devicepass, name.c_str());
	udpclient.begin(random(10000, 65535));
}

void CryptoIoTClient::setup(String& devicepass, String& name, String& host, int port){
	setup(devicepass, name);
	argument.host.fromString(host);
	argument.port = port;
}
