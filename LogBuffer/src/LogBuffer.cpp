/*
  CryptoGarage - LogBuffer

  (implementation)
*/

#include "LogBuffer.h"


LogBuffer::LogBuffer(size_t buf_size){
  size = buf_size;
  buffer = (char*)calloc(size+1, sizeof(char));
  clear();
}

LogBuffer::~LogBuffer(){
  free(buffer);
}

void LogBuffer::clear() {
  pos = 0;
  overflow = false;
  memset(buffer, '\0', size);
}

int LogBuffer::length() {
  if(!overflow){
    return pos;
  } else {
    return size;
  }
}

int LogBuffer::rewind(int n) {
  if(pos != 0 && overflow){
    n = min(pos, n);
    char tmp[n];
    memcpy(tmp, buffer, n);
    memcpy(buffer, buffer+n, size-n);
    memcpy(buffer+(size-n), tmp, n);
    pos -= n;
    return n;
  }
return 0;
}

void LogBuffer::flush() {
  rewind(pos);
}

void LogBuffer::flushLowMem() {
  while(rewind(128));
}

void LogBuffer::write(char c){
  if(pos == size-1){
    overflow = true;
  }
  buffer[pos] = c;
  pos = mod(pos+1, size);
}

void LogBuffer::write(const char * str, int len){
  int i = 0;
  while(len != 0 && str[i] != '\0'){
    write(str[i++]);
    len--;
  }
}

void LogBuffer::write(String& str){
  write(str.c_str(), str.length());
}


void LogBuffer::println(const char * str){
  write(str);
  write('\n');
}

void LogBuffer::println(String &str){
  println(str.c_str());
}

const char * LogBuffer::getRaw(int * len, bool from_first_newline){
  flushLowMem();
  int skip = 0;
  if(from_first_newline && length() == size){
    while(buffer[skip] != '\n'){
      skip++;
    }
    while(buffer[skip] == '\n'){
      skip++;
    }
  }
  *len = size-skip;
  return buffer+skip;
}

String LogBuffer::getString(bool from_first_newline, int max_len){
  int len;
  int skip = 0;
  const char * temp = getRaw(&len, from_first_newline);
  if(max_len != -1 && max_len < len){
    skip = len - max_len;
  }
  return String(temp);
}

String LogBuffer::getLastStringLines(int max_lines){
  int len;
  int skip = 0;
  const char * temp = getRaw(&len, false);

  int i = len-1;  
  bool linestart = false;
  int linecount = 0;
  while(i >= 0 && linecount < max_lines){
    char c = temp[i--];
    if(c != '\n'){
      linestart = true;
      continue;
    }
    if(c == '\n' && linestart){
      linestart = false;
      linecount++;
    }
  }
  return String(temp+i);
}

int LogBuffer::mod(int x,int N){
    return (x % N + N) %N;
}
