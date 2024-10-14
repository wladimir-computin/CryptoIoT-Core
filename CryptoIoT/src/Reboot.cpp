/*
* CryptoIoT - Reboot
* 
* (implementation)
*/

#include "Reboot.h"


void Reboot::rebootTickerTick(void * context){
  (*(Reboot*)context).reboot(0);
}

void Reboot::reboot(unsigned long delay_ms){
  if(delay_ms == 0){
	ESP.restart();
  } else {
	rebootTicker.once_ms(delay_ms, rebootTickerTick, (void*)this); //call reboot() after delay_ms milliseconds passed.
	printfDebug("Rebooting in %ldms...", delay_ms);
  }
}

