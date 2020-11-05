#ifndef BAROMETER_H
#define BAROMETER_H

#include "Logger.h"
#include "RealTimeClock.h"
#include "Measure.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

class Barometer
{
  private:
    Adafruit_BMP3XX bmp;
    RealTimeClock *rtc;
    Logger *logger;

  public:
    Barometer(RealTimeClock *rtc, Logger *logger);

    void boot();
    Measure measure();
};

#endif
