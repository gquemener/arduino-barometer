#include "Wifi.h"
#include <WiFiNINA.h>

Wifi::Wifi(const char* ssid, const char* password, Logger * logger)
{
  this->ssid = ssid;
  this->password = password;
  this->logger = logger;
}

void Wifi::boot()
{
  this->logger->info("Booting Wifi...");

  if (WiFi.status() == WL_NO_MODULE) {
    this->logger->info("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    this->logger->info("WiFiNINA library is not compatible with the current firmware version. Please upgrade the WiFi firmware.");
    while (true);
  }

  this->booted = true;
}

void Wifi::tick(unsigned long timestamp)
{
  if (timestamp - lastUpdateTimestamp > 120000) {
    lastUpdateTimestamp = timestamp;
    if (!this->isConnected()) {
      this->connect();
    }
  }
}

void Wifi::connect()
{
  if (!this->booted) {
    this->logger->info("Wifi is not booted!");
    while (true);
  }

  this->logger->info("Attempting WiFi connection...");
  WiFi.begin(this->ssid, this->password);
  delay(10000);
}

bool Wifi::isConnected()
{
  if (!this->booted) {
    this->logger->info("Wifi is not booted!");
    while (true);
  }

  return WiFi.status() == WL_CONNECTED;
}


unsigned long Wifi::getCurrentTimestamp()
{
  if (!this->booted) {
    this->logger->info("Wifi is not booted!");
    while (true);
  }

  if (!this->isConnected()) {
    this->logger->info("Wifi is not connected!");
    while (true);
  }

  return WiFi.getTime();
}
