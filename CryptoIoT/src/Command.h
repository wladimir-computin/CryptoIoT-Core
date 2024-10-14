/*
* CryptoGarage - Command
* 
* Static class for defining CryptoIoT App Commands
*/

#pragma once

#include <Arduino.h>
#include "Config.h"
#include <ArduinoJson.h>
#include <climits>
#include <PrintDebug.h>

const char DATATYPE_BOOL[] = "bool";
const char DATATYPE_INT[] = "int";
const char DATATYPE_FLOAT[] = "float";
const char DATATYPE_STRING[] = "string";

const char VALIDATION_ERROR_STRING[][24] = {
  "OK",
  "WRONG_COMMAND", 
  "PARAMETER_MISSING", 
  "PARAMETER_TYPE_MISMATCH", 
  "PARAMETER_OUT_OF_BOUNDS"};

struct Param{
  const char * name;
  const char * datatype;
  bool optional;
  int min;
  int max;

  Param(const char * n, const char * d, bool o = false, int mi = INT_MIN, int ma = INT_MAX): name(n), datatype(d), optional(o), min(mi), max(ma) {
    if (strcmp(datatype, DATATYPE_BOOL) == 0){
      min = 0;
      max = 1;
    }
  }
};

struct Validation{
  int errorcode;
  int param;
  void * command;
};

class Command {
  public:
    const char * str_command;
    int size_params = 0;
    Param * params_arr;

    Command(const char * command, unsigned int number_params = 0, Param * params = nullptr);
    
    Param * getParam(unsigned int index);
    Param * getParam(String& param);
    
    String getParamVal(String& message, unsigned int index);
    String getParamVal(String& message, String& param);
    
    void toJSON(JsonObject &jsonObj);
    String toString();
    
    bool check(String& message);
    Validation validate(String& message);
    
    bool operator== (Command &rhs);
    bool operator== (String &rhs);
    bool operator== (const char * rhs);
    
private:
    static bool isNumber(String &value);
    static bool isFloat(String &value);
    static bool isBool(String &value);
};
