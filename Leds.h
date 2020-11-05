#ifndef LEDS_H
#define LEDS_H

#include"Wifi.h"
#include "Logger.h"
#include "Arduino.h"

class Leds
{
  private:
    unsigned long lastUpdateTimestamp = 0;
    Wifi *wifi;
    unsigned int wifiLedPin;
    Logger *logger;
    byte wifiLedState = LOW;

  public:
    Leds(Wifi *wifi, unsigned int wifiLedPin, Logger *logger);

    void boot();
    void tick(unsigned long timestamp);
};

#endif
