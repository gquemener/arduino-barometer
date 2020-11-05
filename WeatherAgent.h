#ifndef WEATHER_AGENT_H
#define WEATHER_AGENT_H

#include "Measure.h"
#include "ApiClient.h"
#include "Wifi.h"
#include "RealTimeClock.h"
#include "Logger.h"
#include "Barometer.h"

class WeatherAgent
{
  private:
    ApiClient *apiClient;
    Wifi *wifi;
    Barometer *barometer;
    Logger *logger;
    bool firstTick = true;
    unsigned long lastMeasureTimestamp = 0;
    unsigned long lastPushTimestamp = 0;
    Measure history[24];
    Measure pendingSync[100];
    int currentPendingSyncIndex = 0;
    bool pendingBufferFull = false;

  public:
    WeatherAgent(ApiClient *apiClient, Wifi *wifi, Barometer *barometer, Logger *logger);

    void tick(unsigned long timestamp);
};

#endif
