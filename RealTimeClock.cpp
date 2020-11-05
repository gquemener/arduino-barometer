#include "RealTimeClock.h"

RealTimeClock::RealTimeClock(Wifi *wifi, Logger *logger)
{
  this->wifi = wifi;
  this->logger = logger;
}

void RealTimeClock::boot()
{
  this->logger->info("Booting RTC...");

  while (!this->wifi->isConnected()) {
    this->wifi->connect();
  }

  this->rtc.begin();
  this->rtc.setEpoch(this->wifi->getCurrentTimestamp());
}

unsigned long RealTimeClock::getEpoch()
{
  return this->rtc.getEpoch();
}

String RealTimeClock::getFormattedDate()
{
  char date[25];
  
  sprintf(
    date,
    "%4d-%02d-%02dT%02d:%02d:%02d+00:00",
    this->rtc.getYear() + 2000,
    this->rtc.getMonth(),
    this->rtc.getDay(),
    this->rtc.getHours(),
    this->rtc.getMinutes(),
    this->rtc.getSeconds()
  );

  return String(date);
}
