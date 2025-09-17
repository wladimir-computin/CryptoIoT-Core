/*
* CryptoGarage - IRelay
* 
* (implementation)
*/

#include "IRelay.h"


void IRelay::relayTickerTick(void * context){
  (*(IRelay*)context).setState(false);
}

void IRelay::trigger(int duration_ms){
  setState(true);
  relayTicker.once_ms(duration_ms, relayTickerTick, (void*)this); //call setState(false) after [duration_ms] milliseconds passed.
}

