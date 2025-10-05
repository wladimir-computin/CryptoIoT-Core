/*
  CryptoGarage - CryptoIoTClientApp

  TODO
*/

#pragma once

#include "Config.h"

#include "CryptoIoTClient.h"
#include "App.h"
#include <PrintDebug.h>
#include <PersistentMemory.h>

const char COMMAND_SEND[] = "send";
class CryptoIoTClientApp : public App {
  public:
    //Here we process the plaintext commands and generate an answer for the client.
    CryptoIoTClientApp(const char * name);
    ProcessMessageStruct processMessage(String &message);
    String getName();
    String getStatus();
	String getType();
    void loop();
    void setup();
    void responseReceived(Msg& response);

  private:
    CryptoIoTClient ciot;
    String appname;
};
void callback(Msg &response, void * param);
