#include "SerialLogger.h"
#include "NullLogger.h"
#include "DisplayLogger.h"
#include "Wifi.h"
#include "RealTimeClock.h"
#include "Leds.h"
#include "Barometer.h"
#include "Display.h"
#include "ApiClient.h"
#include "WeatherAgent.h"

#include "secrets.h"

#define WIFI_LED_PIN 4

// SerialLogger bootLogger(9600);
NullLogger bootLogger;
Display display(&bootLogger);
DisplayLogger logger(&display);

Wifi wifi(SECRET_SSID, SECRET_PASSWORD, &logger);
RealTimeClock rtc(&wifi, &logger);
Leds leds(&wifi, WIFI_LED_PIN, &logger);
Barometer barometer(&rtc, &logger);

ApiClient apiClient(API_TOKEN, &wifi, &logger);
WeatherAgent weatherAgent(&apiClient, &wifi, &barometer, &logger);

void setup() {
  bootLogger.info("Setting up Arduino Barometer...");

  display.boot();
  leds.boot();
  barometer.boot();
  wifi.boot();
  rtc.boot();

  logger.info("Boot sequence finished!");
}

void loop() {
  unsigned long timestamp = millis();
  leds.tick(timestamp);
  wifi.tick(timestamp);
  weatherAgent.tick(timestamp);
}
