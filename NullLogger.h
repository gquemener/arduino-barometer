#ifndef NULL_LOGGER_H
#define NULL_LOGGER_H

#include "Logger.h"

class NullLogger: public Logger
{
  public:
    void log(char const* message);
};

#endif
