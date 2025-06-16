/*
 * C ryptoGara*ge - Command     *
 * 
 * (implementation)
 */

#include "Command.h"

Command::Command(const char * command, unsigned int number_params, Param params[]){
  str_command = command;
  size_params = number_params;
  params_arr = params;
}

Param * Command::getParam(unsigned int num){
  if(num < size_params){
    return &params_arr[num];
  }
  return nullptr;
}

Param * Command::getParam(String& param){
  for(int i = 0; i<size_params; i++){
    if(param == getParam(i)->name){
      return getParam(i);
    }
  }
  return nullptr;
}

String Command::getParamVal(String& message, unsigned int index) {
  if(index < size_params){
    int startindex = 0;
    int endindex = 0;
    bool remaining = (index == size_params-1);
    index++;
    
    for (int i = 0; i < index; i++) {
      startindex = message.indexOf(':', startindex)+1;
      if (startindex == 0) {
        startindex = -1;
        break;
      }
    }
    
    if (startindex != -1) {
      endindex = message.indexOf(':', startindex);
      if (endindex != -1 && !remaining) {
        return message.substring(startindex, endindex);
      } else {
        return message.substring(startindex);
      }
    }
  }
  return "";
}

String Command::getParamVal(String& message, String& param) {
  for(int i = 0; i<size_params; i++){
    if(param == getParam(i)->name){
      return getParamVal(message, i);
    }
  }
  return "";
}

void Command::toJSON(JsonObject &jsonObj){
  if(size_params){
    for (int i = 0; i < size_params; i++) {
      jsonObj[str_command][params_arr[i].name]["type"] = params_arr[i].datatype;
      if(params_arr[i].optional){
        jsonObj[str_command][params_arr[i].name]["optional"] = params_arr[i].optional;
      }
      
      if(params_arr[i].min != INT_MIN){
        jsonObj[str_command][params_arr[i].name]["min"] = params_arr[i].min;
      }
      
      if(params_arr[i].max != INT_MAX){
        jsonObj[str_command][params_arr[i].name]["max"] = params_arr[i].max;
      }
    }
  } else {
    jsonObj[str_command].to<JsonObject>();
  }
}

String Command::toString(){
  return "";
}

bool Command::check(String& message){
  int startindex = 0;
  int endindex = 0;
  endindex = message.indexOf(':', startindex);
  if (endindex != -1) {
    return message.substring(startindex, endindex) == str_command;
  } else {
    return message.substring(startindex) == str_command;
  }
}

Validation Command::validate(String& message){
  if(!check(message)){
    return {1, -1, this};
  }
  
  for(int i = 0; i < size_params; i++){
    Param * param = &params_arr[i];
    String val = getParamVal(message, i);
    if(val == "" && !param->optional){
      return {2, i, this};
    }
    if (strcmp(param->datatype, DATATYPE_BOOL) == 0){
      if(!isBool(val)){
        return {3, i, this};
      }
    } else if (strcmp(param->datatype, DATATYPE_INT) == 0) {
      if(!isNumber(val)){
        return {3, i, this};
      }
      int x = val.toInt();
      if(!(x >= param->min && x <= param->max)){
        return {4, i, this};
      }
    } else if (strcmp(param->datatype, DATATYPE_FLOAT) == 0) {
      if(!isFloat(val)){
        return {3, i, this};
      }
      int x = val.toFloat();
      if(!(x >= param->min && x <= param->max)){
        return {4, i, this};
      }
    } else if (strcmp(param->datatype, DATATYPE_STRING) == 0) {
      if(!(val.length() >= param->min && val.length() <= param->max)){
        return {4, i, this};
      }
    }
  }
  
  return {0, 0, this};
}

bool Command::operator== (Command &rhs){
  return strcmp(rhs.str_command, str_command) == 0;
}

bool Command::operator== (String &rhs){
  return rhs==str_command;
}

bool Command::operator== (const char * rhs){
  return strcmp(rhs, str_command) == 0;
}

bool Command::isNumber(String &value){
  for (char c : value){
    if (!isDigit(c)){
      return false;
    }
  }
  return true;
}

bool Command::isFloat(String &value){
  bool dot_used = false;
  for (char c : value){
    if (!isDigit(c)){
      if(c == '.' && !dot_used){
        dot_used = true;
      } else {
        return false;
      }
    }
  }
  return true;
}

bool Command::isBool(String &value){
  return value == "0" || value == "1";
}
