/*
* CryptoGarage - Uptime
* 
* This class measures the time since the last reboot.
*/

#pragma once

#include <Arduino.h>
#include <Ticker.h>

class Uptime {
  private:
    Uptime() {}           
    Uptime( const Uptime& );
    Uptime & operator = (const Uptime &);
    
    Ticker uptimeTicker;
    static void uptimeTick(void * context);
    uint32_t secsUp;

  public:
    static Uptime& instance() {
      static Uptime _instance;
      return _instance;
    }
    ~Uptime() {}
    
    void start();
    
    String getUptime();
};
