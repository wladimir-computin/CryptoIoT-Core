/*
* CryptoMX-3 - Timeout
* 
* (implementation)
*/

#include "Timeout.h"

void Timeout::timeoutTick(void * context) {
  (*(Timeout*)context).reset(true);
}

void Timeout::reset() {
  reset(false);
}

void Timeout::reset(bool finish) {
  timeoutTicker.detach();
  finished = finish;
  state = true;
}

void Timeout::wait(int ms) {
  reset();
  timeoutTicker.once_ms(ms, timeoutTick, (void*)this);
  state = false;
}

bool Timeout::check(){
  return state;
}

bool Timeout::finished_once(){
  return finished;
}
