/*
* CryptoGarage - LogBuffer
* 
*/

#pragma once

#include <Arduino.h>
#include <PrintDebug.h>
 
class LogBuffer {
  public:
    LogBuffer(size_t size);
    ~LogBuffer();

    void clear();
    void flush();
    void flushLowMem();
    int length();
    const char * getRaw(int * len, bool from_first_newline=true);
    String getString(bool from_first_newline=true, int max_len = -1);
    String getLastStringLines(int max_lines);
    
    void write(char c);
    void write(const char * str, int len = -1);
    void write(String& str);
    
    void println(String& str);
    void println(const char * str);
    
    //void printf(const char *fmt, ...);
  private:
    size_t size = 0;
    int pos = 0;
    bool overflow = false;
    
    int rewind(int n);
    static int mod(int x,int N);
    char * buffer;
};
