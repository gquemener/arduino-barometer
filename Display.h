#ifndef DISPLAY_H
#define DISPLAY_H

#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
#define RA8875_GREY 0x7BEF

#include "Logger.h"

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

class Display
{
  private:
    Logger *logger;
    Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
    String lines[24];
    String spaces(int count);

  public:
    Display(Logger *logger);

    void boot();
    void writeLine(String line);
};

#endif
