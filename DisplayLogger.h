#ifndef DISPLAY_LOGGER_H
#define DISPLAY_LOGGER_H

#include "Logger.h"
#include "Display.h"

class DisplayLogger: public Logger
{
  private:
    Display *display;

  public:
    DisplayLogger(Display *display);

    void log(char const* message);
};

#endif
