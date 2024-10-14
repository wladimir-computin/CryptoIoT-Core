# PersistentMemory
Permanent key-value based storage of settings for ESP8266 and ESP32 MCUs based on LittleFS and ArduinoJson. No more EEPROM hacking!

## Dependencies
* LittleFS
* ArduinoJson

## Usage
```cpp
// 2nd parameter: create entry on read if entry doesn't exist yet.
PersistentMemory pmem("config", true);

// return 0 if entry does not exist
int value = pmem.readInt("some_value", 0)
// this message survives reboots and fw updates
pmem.writeString("some_other_value", "I was here ;)");
// write changes
pmem.commit();
```

## Examples
Look into [/examples](https://github.com/wladimir-computin/PersistentMemory/blob/main/examples/) :)

## API

### Constructors:
```cpp
PersistentMemory(String vault, bool create=false);
```

### Static methods:
```cpp
static void format();
static bool remove(String &vault);
static int listVaults(String * arr, int size);
static String toJSON(String &vault);
```

### Methods:
```cpp
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
```
