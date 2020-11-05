#include "ApiClient.h"
#include "ArduinoJson.h"
#include "Measure.h"
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

ApiClient::ApiClient(const char* token, Wifi *wifi, Logger *logger)
{
  this->token = token;
  this->wifi = wifi;
  this->logger = logger;
}

void ApiClient::last24Hours(Measure history[])
{
  return;
  this->logger->info("Fetching last 24 hours measures...");

  if (!this->wifi->isConnected()) {
    this->logger->warn("WiFi is not connected! Aborting.");

    return;
  }

  // Client is recreated on each request because request are not sent when using a client as a class attribute.
  // I have no idea why, which is the reason why I leave this ugly comment.
  // Also, it is up to the caller side to make sure that WiFi is connected before sending http request.
  WiFiSSLClient wifi;
  HttpClient client = HttpClient(wifi, "weather.gildasquemener.me", 443);

  const size_t reqCapacity = JSON_OBJECT_SIZE(0) + JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonDocument reqDoc(reqCapacity);
  reqDoc["name"] = "Last24Hours";
  JsonObject payload = reqDoc.createNestedObject("payload");

  String body = "";
  serializeJson(reqDoc, body);

  String authorizationHeader = "Bearer ";
  authorizationHeader.concat(this->token);

  client.beginRequest();
  client.post("/queries");
  client.sendHeader("Authorization", authorizationHeader);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  this->logger->info(response.c_str());

  const size_t resCapacity = JSON_ARRAY_SIZE(24) + 24 * JSON_OBJECT_SIZE(2) + 1070;
  DynamicJsonDocument resDoc(resCapacity);
  deserializeJson(resDoc, response);

  JsonArray arr = resDoc.as<JsonArray>();
  for (uint16_t i = 0; i < arr.size(); ++i) {
    // TODO: This is incorrect, measure must be appended, not prepended (See WeatherAgent.cpp)
    history[i] = Measure(arr[i]["date"], arr[i]["pressure"]);
    this->logger->info(history[i].epoch());
    this->logger->info(history[i].pressure());
  }
}

bool ApiClient::pushMeasure(Measure measure)
{
  this->logger->info("Pushing measure...");

  if (!this->wifi->isConnected()) {
    this->logger->warn("WiFi is not connected! Aborting.");

    return false;
  }

  WiFiSSLClient wifi;
  HttpClient client = HttpClient(wifi, "weather.gildasquemener.me", 443);

  const size_t capacity = 2 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  doc["name"] = "AddWeatherReport";
  JsonObject payload = doc.createNestedObject("payload");
  payload["pressure"] = measure.pressure();
  payload["date"] = measure.epoch();

  String body = "";
  serializeJson(doc, body);

  String authorizationHeader = "Bearer ";
  authorizationHeader.concat(this->token);

  this->logger->info(body.c_str());

  client.beginRequest();
  client.post("/commands");
  client.sendHeader("Authorization", authorizationHeader);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();

  int statusCode = client.responseStatusCode();

  this->logger->info(String("Response status code: " + String(statusCode)));

  return 201 == statusCode;
}
