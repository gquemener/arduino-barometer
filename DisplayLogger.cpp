#include "DisplayLogger.h"

DisplayLogger::DisplayLogger(Display *display)
{
  this->display = display;
}

void DisplayLogger::log(char const* message)
{
  this->display->writeLine(String(message));
}
