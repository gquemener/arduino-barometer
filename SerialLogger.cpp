#include "SerialLogger.h"
#include "SPI.h"

SerialLogger::SerialLogger(long speed)
{
  this->speed = speed;
}

void SerialLogger::log(char const* message)
{
  if (!Serial) {
    Serial.begin(this->speed);
  }

  while (!Serial) {
    ;
  }

  Serial.println(message);
}
