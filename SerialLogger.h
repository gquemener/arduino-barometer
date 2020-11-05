#ifndef SERIAL_LOGGER_H
#define SERIAL_LOGGER_H

#include "Logger.h"

class SerialLogger: public Logger
{
  private:
    long speed;

  public:
    SerialLogger(long speed);

    void log(char const* message);
};

#endif
