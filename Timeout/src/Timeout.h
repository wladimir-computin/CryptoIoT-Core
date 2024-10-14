/*
* CryptoMX-3 - Timeout
* 
* Sometimes you just need a timeout...
*/

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <Ticker.h>

class Timeout {
  private:
    Ticker timeoutTicker;
    static void timeoutTick(void * context);
    volatile bool state = true;
    volatile bool finished = false;

  public:
    void reset();
    void reset(bool finished);
    void wait(int ms);
    bool check();
    bool finished_once();
};

#endif
