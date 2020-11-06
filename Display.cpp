#include "Display.h"

Display::Display(Logger *logger)
{
  this->logger = logger;
}

void Display::boot()
{
  this->logger->info("Booting Display...");

  if (!tft.begin(RA8875_800x480)) {
    this->logger->info("RA8875 Not Found!");
    while (true);
  }

  this->tft.displayOn(true);
  this->tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  this->tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  this->tft.PWM1out(255);

  this->tft.graphicsMode();
  this->tft.fillScreen(RA8875_BLACK);

  this->tft.textMode();
  this->tft.textColor(RA8875_WHITE, RA8875_BLACK);
  this->tft.cursorBlink(32);

  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);

  tft.touchEnable(true);
}

void Display::writeLine(String line)
{
  for (unsigned int i = 0; i < 24; i++) {
    this->lines[i] = this->lines[i + 1];
  }
  this->lines[23] = line + spaces(100 - line.length());

  for (unsigned int i = 0; i < 24; i++) {
    tft.textSetCursor(0, i * 20);
    tft.textWrite(this->lines[i].c_str());
  }
}

String Display::spaces(int count)
{
  String spaces = "";

  if (count < 0) {
    return spaces;
  }

  for (unsigned int i = 0; i < count; i++) {
    spaces.concat(" ");
  }

  return spaces;
}
