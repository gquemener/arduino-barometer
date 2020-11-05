#ifndef REAL_TIME_CLOCK_H
#define REAL_TIME_CLOCK_H

#include "Wifi.h"
#include "Logger.h"
#include <RTCZero.h>
#include <Arduino.h>

class RealTimeClock
{
  private:
    RTCZero rtc;
    Wifi *wifi;
    Logger *logger;

  public:
    RealTimeClock(Wifi *wifi, Logger *logger);

    void boot();
    unsigned long getEpoch();
    String getFormattedDate();
};

#endif
