/*
* CryptoGarage - PersistentMemory
* 
* We need persistent memory to remember settings (like WiFi SSID and passphrase) after reboots.
* This class contains helper/wrapper functions. They simplify the usage of the EEPROM library, which is just some sort of bytewise access to flash memory.
* Don't forget to call .commit() after finishing your memory writing operations, otherwise any changes will be lost after reboot.
*/

#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PrintDebug.h>
 
class PersistentMemory {
  private:
    JsonDocument json;
    String filename;
    bool initsuccess = false;
    bool createifnotfound;
    bool changed = false;
    
  public:

    PersistentMemory(String vault, bool create=false);
    ~PersistentMemory();

    explicit operator bool();

    static void format();
    static bool remove(String &vault);
    static int listVaults(String * arr, int size);
    static String toJSON(String &vault);

    void commit();
    
    String readString(const char * key, String fallback="");
    String readString(String &key, String fallback="");
    void writeString(const char * key, String value);
    void writeString(String &key, String value);
    
    bool readBool(const char * key, bool fallback=false);
    bool readBool(String &key, bool fallback=false);
	void writeBool(const char * key, bool value);
    void writeBool(String &key, bool value);
    
    int readInt(const char * key, int fallback=0);
    int readInt(String &key, int fallback=0);
	void writeInt(const char * key, int value);
    void writeInt(String &key, int value);

    double readDouble(const char * key, double fallback=0.0);
    double readDouble(String &key, double fallback=0.0);
	void writeDouble(const char * key, double value);
    void writeDouble(String &key, double value);
	
	void removeKey(const char * key);
	void removeKey(String &key);
    
};
