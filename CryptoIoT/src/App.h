/*
* CryptoGarage - App
* 
* TODO
*/

#pragma once

#include "Config.h"
#include "Message.h"
#include "Command.h"
#include "CryptoIoT_API.h"
#include <LogBuffer.h>
#include <PrintDebug.h>
#include <CIoT_Time.h>

const char NO_COMMAND[] = "NO_COMMAND";

class App{
  protected:

    CryptoIoT_API cryptoiot;
    String appname;
    Command * commands[32] = {nullptr};
    int commands_len = 0;
    
    void registerCommand(Command &command){
      if(commands_len < (ARRAY_LEN(commands)-1)){
        commands[commands_len++] = &command;
      }
    }
    
    void println(const char * str){
      cryptoiot.applog->println(str);
      printDebug(str);
    }
    void println(String& str){
      println(str.c_str());
    }
    
    void println2(const char * str) {
      char buffer[strlen(str) + appname.length() + 32];
      snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "[+] %s (%s): %s", appname.c_str(), cryptoiot.time->stringTime().c_str(), str);
      println(buffer);
    }

    void println2(String str) {
      println2(str.c_str());
    }
	
  public:
    virtual ProcessMessageStruct processMessage(String &message){      
        return {ERR, NO_COMMAND};
    }
    virtual String getStatus() = 0;
    virtual String getName() = 0;
	virtual String getType() = 0;
    virtual String getVersion() {
      return "1.0";
    }
    virtual void loop(){};
    virtual void setup(){};
	void giveAPIAccess(CryptoIoT_API c){
		cryptoiot = c;
	}
    
    int getNumCommands(){
      return commands_len;
    }
	
    Command ** getCommands(){
      return commands;
    }
};
