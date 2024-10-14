#pragma once

class EventManager;
class TimerManager;
class Time;
class StatusLED;
class System;
class LogBuffer;

struct CryptoIoT_API{
	StatusLED * statusLED;
	Time * time;
	EventManager * eventManager;
	TimerManager * timerManager;
	System * system;
	LogBuffer * applog;
};
