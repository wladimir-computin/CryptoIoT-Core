/*
* CryptoIoT - Reboot
* 
* Delayed reboot
*/

#pragma once

#include <Arduino.h>
#include <Ticker.h>

#include <PrintDebug.h>

class Reboot{
  protected:
    Ticker rebootTicker;
    static void rebootTickerTick(void * context);
    
  public:
    void reboot(unsigned long delay_ms = 0);
};
