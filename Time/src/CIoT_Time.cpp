/*
* CryptoIoT - Time
* 
* (implementation)
*/

#include "CIoT_Time.h"

void Time::setup(){
  PersistentMemory pmem("time", true);
  
  String server1 = pmem.readString(KEY_SERVER1, SERVER1);
  String tzenv = pmem.readString(KEY_TZENV, TZENV);
  custom_sunrise_angle = pmem.readInt(KEY_CUSTOM_SUNRISE_ANGLE, custom_sunrise_angle);
  position_lat = pmem.readDouble(KEY_POSLAT, position_lat);
  position_long = pmem.readDouble(KEY_POSLONG, position_long);
  pmem.commit();
  
  int max_buf = sizeof(ntp_server1_buffer)/sizeof(ntp_server1_buffer[0]);
  
  if(server1.length() < max_buf){
	strncpy(ntp_server1_buffer, server1.c_str(), server1.length());
	ntp_server1_buffer[server1.length()] = '\0';
	configTzTime(tzenv.c_str(), ntp_server1_buffer);
  }
}

void Time::setTime(unsigned long epoch, int ms) const {
  struct timeval tv;
  if (epoch > 2082758399){
    tv.tv_sec = epoch - 2082758399;  // epoch time (seconds)
  } else {
    tv.tv_sec = epoch;  // epoch time (seconds)
  }
  tv.tv_usec = ms;    // microseconds
  settimeofday(&tv, nullptr);
}

tm * Time::now(bool utc, int offset_seconds){
  time_t t = time(nullptr) + offset_seconds;
  if(utc){
    return gmtime(&t);
  } else {
    return localtime(&t);
  }
}
  

int Time::minutesSinceMidnight(bool utc){
  tm * t = now(utc);
  return t->tm_hour*60 + t->tm_min;
}

String Time::stringTime(bool utc){
  char buffer[] = "01.01.2000 23:59:59";

  strftime (buffer, sizeof(buffer) / sizeof(char), "%d.%m.%Y %H:%M:%S", now(utc));
  return buffer;
}

int Time::timezone(){
  tm * t = now(false);
  int offset = -1 * (_timezone/3600 - t->tm_isdst);
  return offset;
}

SunSet Time::getSunSet(){
  tm * t = now();
  SunSet sun;
  sun.setPosition(position_lat, position_long, timezone());
  sun.setCurrentDate(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
  return sun;
}

int Time::sunrise_minutes(){
  return getSunSet().calcSunrise();
}

int Time::sunset_minutes(){
  return getSunSet().calcSunset();
}

int Time::civil_sunrise_minutes(){
  return getSunSet().calcCivilSunrise();
}

int Time::civil_sunset_minutes(){
  return getSunSet().calcCivilSunset();
}

int Time::custom_sunrise_minutes(double degree){
  return getSunSet().calcCustomSunrise(degree);
}

int Time::custom_sunset_minutes(double degree){
  return getSunSet().calcCustomSunset(degree);
}

int Time::custom_sunrise_minutes(){
  return getSunSet().calcCustomSunrise(custom_sunrise_angle);
}

int Time::custom_sunset_minutes(){
  return getSunSet().calcCustomSunset(custom_sunrise_angle);
}

bool Time::isNight(){
  int now = minutesSinceMidnight();
  if (now < custom_sunrise_minutes(custom_sunrise_angle) || now > custom_sunset_minutes(custom_sunrise_angle)){
    return true;
  } else {
    return false;
  }
}

String Time::min2str(int minutes){
  int hh = minutes / 60;
  int mm = minutes % 60;
  char buf[] = "00:00";
  sprintf(buf,"%02d:%02d",hh, mm);
  return buf;
}

int Time::str2min(String& str){
  if(str.length() == 5 && str.indexOf(':') == 2){
    int hours = str.substring(0,2).toInt();
    int minutes = str.substring(3).toInt();
    return hours*60+minutes;
  }
  return -1;
}

double Time::getPosLat(){
  return position_lat;
}

double Time::getPosLong(){
  return position_long;
}
