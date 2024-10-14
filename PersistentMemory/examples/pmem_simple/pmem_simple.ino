#include <LittleFS.h>
#include <PersistentMemory.h>

void setup(){
  #ifdef ARDUINO_ARCH_ESP32
    LittleFS.begin(true);
  #else
    LittleFS.begin();
  #endif
  Serial.begin(115200);

  PersistentMemory pmem("config", true);
  int counter = pmem.readInt("counter", 0);
  pmem.writeInt("counter", counter+1);
  pmem.commit();

  Serial.printf("\nThis is my %d boot!\n", counter);
}

void loop(){
}
