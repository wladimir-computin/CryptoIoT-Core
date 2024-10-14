/*
  CryptoIoT - IDimmer

  TODO
*/

#pragma once

#include <Ticker.h>
#include <App.h>

static Param COMMAND_DIMFADE_PARAMS[] = {{"value", DATATYPE_FLOAT, false, 0, 1}, {"fade_ms", DATATYPE_INT, true, 0, 1000*60*60}};
static Command COMMAND_DIMFADE("dimfade", ARRAY_LEN(COMMAND_DIMFADE_PARAMS), COMMAND_DIMFADE_PARAMS);
static Command COMMAND_DIMSTATE("dimstate");

class IDimmer : public App {
  public:
    enum FadeMode {NONE, SINGLE_ON_OFF, SINGLE_OFF_ON, PERIODIC_FADE};

  protected:
	virtual double toRelative(int absolute) = 0;
    virtual int toAbsolute(double relative) = 0;

    struct Argument {
      int time_ms;
      int ms_passed;
      double old_current;
      double goal;
      FadeMode fademode;
    };

    Ticker ledTicker;
    Argument argument = {0, 0, 0, 0, NONE};
    static void ledTickerTick(void * context);
    void ledTickerTick2();
    void fadeCompleted();
    double ease(double t);

    const int FADEFPS = 100;
	String appname;

  public:
    //Here we process the plaintext commands and generate an answer for the client.
    ProcessMessageStruct processMessage(String &message);
	
	IDimmer(const char * name);
    String getName();
    String getStatus();
	String getType();
    virtual void loop() = 0;
    virtual void setup() = 0;

    virtual void setVal(double val) = 0;
    virtual double getVal() = 0;
    void setState(bool state);
    void fade(FadeMode fademode, int time_ms);
    void fadeToVal(double val, int time_ms);
    void fadeStop();
	bool isFading();

    volatile int curBrightness = 0;
};
