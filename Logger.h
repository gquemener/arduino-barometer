#ifndef LOGGER_H
#define LOGGER_H

#include "Arduino.h"

class Logger
{
  public:
    virtual void log(char const* message) = 0;
    
    void info(char const* message) {
      log(String(String("[INFO] ") + String(message)).c_str());
    }
    void info(const String& message) {
      info(message.c_str());
    }
    void info(unsigned long message) {
      info((char) message);
    }

    void warn(char const* message) {
      log(String(String("[WARNING] ") + String(message)).c_str());
    }
    void warn(const String& message) {
      info(message.c_str());
    }
    void warn(unsigned long message) {
      info((char) message);
    }    
};

#endif
