#ifndef WIFI_H
#define WIFI_H

#include "Logger.h"

class Wifi
{
  private:
    const char* ssid;
    const char* password;
    Logger * logger;
    bool booted = false;
    unsigned long lastUpdateTimestamp = 0;

  public:
    Wifi(const char* ssid, const char* password, Logger * logger);

    void boot();
    void tick(unsigned long timestamp);

    void connect();
    bool isConnected();
    unsigned long getCurrentTimestamp();
};

#endif
