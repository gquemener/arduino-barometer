#ifndef API_CLIENT_H
#define API_CLIENT_H

#include "Logger.h"
#include "Measure.h"
#include "Wifi.h"
#include <HttpClient.h>

class ApiClient
{
  private:
    const char* token;
    Wifi *wifi;
    Logger *logger;
    HttpClient *http;

  public:
    ApiClient(const char* token, Wifi *wifi, Logger *logger);

    void boot();
    void last24Hours(Measure history[]);
    bool pushMeasure(Measure measure);
};

#endif
