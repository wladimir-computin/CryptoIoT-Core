/*
  CryptoGarage - Garage

  (implementation)
*/

#include "CryptoIoTClientApp.h"

CryptoIoTClientApp::CryptoIoTClientApp(const char * name){
	appname = name;
}

void callback(String &response, void * context){
  (*(CryptoIoTClientApp*)context).responseReceived(response);
}

void CryptoIoTClientApp::responseReceived(String &response){}

void CryptoIoTClientApp::loop() {
  ciot.loop();
}

void CryptoIoTClientApp::setup() {}


ProcessMessageStruct CryptoIoTClientApp::processMessage(String &message) {

  if (Message::getParam(message, 0) == COMMAND_SEND) {
    String host = Message::getParam(message, 1);
  	int port = Message::getParam(message, 2).toInt();
  	String pass = Message::getParam(message, 3);
  	String command = Message::getParam(message, 4, true);
	String c = "ciot";
    ciot.setup(pass, c);
    ciot.send(host, port, command, callback, (void*)this);
    
    return {ACK, ""};
  }

  return {ERR, "NO_COMMAND"};
}

String CryptoIoTClientApp::getName() {
  return appname;
}

String CryptoIoTClientApp::getStatus() {
  return "";
}

String CryptoIoTClientApp::getType() {
  return "CIOT_CLIENT";
}
