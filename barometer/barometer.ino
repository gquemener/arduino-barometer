#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#include <HttpClient.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

#include "ArduinoJson.h"

#include "secrets.h"

#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
#define RA8875_GREY 0x7BEF

class Measure {
    uint32_t _epoch;
    String _date;
    double _pressure;
  public:
    Measure(uint32_t, String, double);
    Measure(String, double);
    Measure();
    uint32_t epoch();
    double pressure();
    String hours();
};

Measure::Measure(uint32_t epoch, String date, double pressure)
{
  _epoch = epoch;
  _date = date;
  _pressure = pressure;
}


Measure::Measure(String date, double pressure)
{
  _epoch = 0;
  _date = date;
  _pressure = pressure;
}

Measure::Measure()
{
  _epoch = 0;
  _date = "";
  _pressure = 0.0;
}

uint32_t Measure::epoch()
{
  return _epoch;
}

double Measure::pressure()
{
  return _pressure;
}

String Measure::hours()
{
  if (0 == _date.length()) {
    return "";
  }

  //UTC+1
  return String(_date.substring(11, 13).toInt() + 1);
}

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_BMP3XX bmp;
RTCZero rtc;
WiFiSSLClient ssLclient;

char ssid[] = SSID;
char pass[] = PASS;
int wifiStatus = WL_IDLE_STATUS;

Measure history[24];

void connectToWifi()
{
  if (WiFi.status() == WL_NO_SHIELD) {
    writeLog("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    writeLog("Please upgrade the WIFI firmware");
  }

  while (wifiStatus != WL_CONNECTED) {
    writeLog("Attempting to connect to SSID: ");
    writeLog(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void disconnectFromWifi()
{
  WiFi.end();
  wifiStatus = WL_IDLE_STATUS;
  writeLog("Disconnected from Wifi");
}

void initRtc()
{
  rtc.begin();
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries == maxTries) {
    writeLog("NTP unreachable!!");
    while (1);
  } else {
    writeLog("Epoch received: ");
    writeLog(String(epoch));
    rtc.setEpoch(epoch);
  }
}

void initBmp()
{
  if (!bmp.begin()) {
    writeLog("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  getMeasure();
}

void initScreen()
{
  if (!tft.begin(RA8875_800x480)) {
    writeLog("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

  tft.graphicsMode();
  tft.fillScreen(RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  writeLog("Starting...");

  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);

  tft.touchEnable(true);
}

bool initializing = true;
uint16_t initLineNumber = 0;

void writeLog(String line)
{
  if (initializing) {
    tft.textMode();
    tft.textSetCursor(0, initLineNumber);
    tft.textWrite(line.c_str());
    initLineNumber += 20;
  }
  Serial.println(line.c_str());
}

void drawGraph()
{
  tft.graphicsMode();
  tft.fillScreen(RA8875_BLACK);

  tft.drawFastHLine(0, 430, 800, RA8875_WHITE);

  double minPressure = 1500.0;
  double maxPressure = 500.0;

  for (uint16_t i = 0; i < 24; ++i) {
    tft.graphicsMode();
    tft.drawFastVLine(9 + i * 34, 0, 429, RA8875_GREY);
    tft.drawFastVLine(9 + i * 34, 431, 5, RA8875_WHITE);
    tft.textMode();
    tft.textSetCursor(1 + i * 34, 436);
    tft.textWrite(history[i].hours().c_str());

    double pressure = history[i].pressure();
    if (0.0 == pressure) {
      continue;
    }

    minPressure = min(minPressure, pressure);
    maxPressure = max(maxPressure, pressure);
  }

  tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_BLACK);
  tft.textSetCursor(0, 0);
  tft.textWrite(String(maxPressure).c_str());
  tft.textSetCursor(0, 410);
  tft.textWrite(String(minPressure).c_str());
  tft.textColor(RA8875_WHITE, RA8875_BLACK);

  tft.graphicsMode();
  for (uint16_t i = 1; i < 24; ++i) {
    Measure prevMeasure = history[i - 1];
    Measure measure = history[i];
    if (0.0 == prevMeasure.pressure() || 0.0 == measure.pressure()) {
      continue;
    }
    tft.drawLine(
      9 + (i - 1) * 34,
      map(prevMeasure.pressure() * 100, minPressure * 100, maxPressure * 100, 430, 30),
      9 + i * 34,
      map(measure.pressure() * 100, minPressure * 100, maxPressure * 100, 430, 30),
      RA8875_RED
    );
  }
}

void appendMeasure(Measure newMeasure)
{
  for (uint16_t i = 0; i < 23; ++i) {
    history[i] = history[i + 1];
  }
  history[23] = newMeasure;
}

Measure getMeasure()
{
  if (!bmp.performReading()) {
    writeLog("Failed to perform reading :(");
    while (1);
  }

  char date[25];
  sprintf(
    date,
    "%4d-%02d-%02dT%02d:%02d:%02d+00:00",
    rtc.getYear() + 2000,
    rtc.getMonth(),
    rtc.getDay(),
    rtc.getHours(),
    rtc.getMinutes(),
    rtc.getSeconds()
  );

  return Measure(rtc.getEpoch(), String(date), bmp.pressure / 100.0);
}

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, "weather.gildasquemener.me", 443);
Measure pending[100];
unsigned int currentPendingIndex = 0;

bool syncMeasure(Measure measure)
{
  writeLog("Synchronizing measure...");

  const size_t capacity = 2 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  doc["name"] = "AddWeatherReport";
  JsonObject payload = doc.createNestedObject("payload");
  payload["pressure"] = measure.pressure();
  payload["date"] = measure.epoch();

  String body = "";
  serializeJson(doc, body);

  char token[] = API_TOKEN;
  String authorizationHeader = "Bearer ";
  authorizationHeader.concat(token);

  client.beginRequest();
  client.post("/commands");
  client.sendHeader("Authorization", authorizationHeader);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();

  int statusCode = client.responseStatusCode();

  writeLog(String(statusCode));

  return 201 == statusCode;
}

void fetchHistory()
{
  writeLog("Fetching history...");

  const size_t reqCapacity = JSON_OBJECT_SIZE(0) + JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonDocument reqDoc(reqCapacity);
  reqDoc["name"] = "Last24Hours";
  JsonObject payload = reqDoc.createNestedObject("payload");

  String body = "";
  serializeJson(reqDoc, body);

  char token[] = API_TOKEN;
  String authorizationHeader = "Bearer ";
  authorizationHeader.concat(token);

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

  const size_t resCapacity = JSON_ARRAY_SIZE(24) + 24 * JSON_OBJECT_SIZE(2) + 1070;
  DynamicJsonDocument resDoc(resCapacity);
  deserializeJson(resDoc, response);

  JsonArray arr = resDoc.as<JsonArray>();
  for (uint16_t i = 0; i < arr.size(); ++i) {
    appendMeasure(Measure(arr[i]["date"], arr[i]["pressure"]));
  }
  drawGraph();
}

void setup()
{
  Serial.begin(9600);

  initScreen();
  connectToWifi();
  initRtc();
  initBmp();
  fetchHistory();
  disconnectFromWifi();

  initializing = false;

  drawGraph();
}

unsigned long lastActivity = 0;
const long idleTimeout = 10000;
bool sleeping = false;

void triggerSleep()
{
  if (sleeping) {
    return;
  }

  unsigned long currentMillis = millis();
  if (0 == lastActivity) {
    lastActivity = currentMillis;
  }

  if (currentMillis - lastActivity >= idleTimeout) {
    sleeping = true;
    tft.PWM1out(0);
    tft.sleep(true);

    writeLog("Sleeping!");
  }
}

void triggerWakeUp()
{
  if (!digitalRead(RA8875_INT)) {
    if (tft.touched()) {
      uint16_t tx, ty;
      tft.touchRead(&tx, &ty);
      lastActivity = millis();
      if (sleeping) {
        writeLog("Waking up!");
        sleeping = false;
        tft.sleep(false);
        tft.displayOn(true);
        drawGraph();
        tft.PWM1out(255);
      }
    }
  }
  delay(1);
}

unsigned long lastReport = 0xffffffff;
const unsigned int reportInterval = 30 * 60 * 1000;

void loop()
{
  triggerSleep();
  triggerWakeUp();

  unsigned long currentMillis = millis();

  if (currentMillis - lastReport >= reportInterval || currentMillis < lastReport) {
    Measure measure = getMeasure();
    appendMeasure(measure);
    drawGraph();
    pending[currentPendingIndex] = measure;
    currentPendingIndex++;
    lastReport = currentMillis;
  }

  if (currentPendingIndex > 0) {
    writeLog("Number of measures to sync: ");
    writeLog(String(currentPendingIndex));

    connectToWifi();
    for (int i = currentPendingIndex - 1; i > -1; i--) {
      if (syncMeasure(pending[i])) {
        currentPendingIndex--;
      }
    }
    disconnectFromWifi();
  }
}
