#include "Leds.h"

Leds::Leds(Wifi *wifi, unsigned int wifiLedPin, Logger *logger)
{
  this->wifi = wifi;
  this->wifiLedPin = wifiLedPin;
  this->logger = logger;
}

void Leds::boot()
{ 
  this->logger->info("Booting LEDS...");

  pinMode(this->wifiLedPin, OUTPUT);
  digitalWrite(this->wifiLedPin, this->wifiLedState);
}

void Leds::tick(unsigned long timestamp)
{
  if (timestamp - lastUpdateTimestamp > 50) {
    bool connected = this->wifi->isConnected();
    if (connected) {
      this->wifiLedState = HIGH;
      lastUpdateTimestamp = timestamp;
    } else if (true /** Measure buffer is full, we need a wifi connection to sync */) {
      if (timestamp - lastUpdateTimestamp > 500) {
        this->wifiLedState = this->wifiLedState == HIGH ? LOW : HIGH;
        lastUpdateTimestamp = timestamp;
      }
    }
  }
  digitalWrite(this->wifiLedPin, this->wifiLedState);
}
