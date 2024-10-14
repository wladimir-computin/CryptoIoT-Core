  /*
* CryptoGarage - Time
* 
* TODO
*/

#pragma once

#include <Arduino.h>
#include <PersistentMemory.h>
#include <time.h>
#include <sunset.h>
#include <PrintDebug.h>

const char SERVER1[] = "fritz.box";
const char SERVER2[] = "pool.ntp.org";
const char TZENV[] = "CET-1CEST,M3.5.0,M10.5.0/3";
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

const char KEY_CUSTOM_SUNRISE_ANGLE[] = "custom_sunrise_angle";
const char KEY_SERVER1[] = "ntp_server1";
const char KEY_SERVER2[] = "ntp_server2";
const char KEY_TZENV[] = "tz_env";
const char KEY_POSLAT[] = "pos_lat";
const char KEY_POSLONG[] = "pos_long";
  

class Time {
  private:
    uint8_t custom_sunrise_angle = 93;
    double position_lat = 51.0899681;
    double position_long = 5.9695307;

    bool ntp_sync_success = false;
	long next_ntp_sync = -1;
	char ntp_server1_buffer[64] = "";

    SunSet getSunSet();
  
  public:
    void setup();
    tm * now(bool utc=false, int offset_seconds=0);
    int minutesSinceMidnight(bool utc=false);
    String stringTime(bool utc=false);
    int sunrise_minutes();
    int sunset_minutes();
    int civil_sunrise_minutes();
    int civil_sunset_minutes();
    int custom_sunrise_minutes(double degree);
    int custom_sunset_minutes(double degree);
    int custom_sunrise_minutes();
    int custom_sunset_minutes();
    
    static String min2str(int minutes);
    static int str2min(String& str);
    int timezone();
    bool isNight();
    double getPosLat();
    double getPosLong();
};
