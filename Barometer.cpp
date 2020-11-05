#include "Barometer.h"

Barometer::Barometer(RealTimeClock *rtc, Logger *logger)
{
  this->rtc = rtc;
  this->logger = logger;
}

void Barometer::boot()
{
  this->logger->info("Booting BMP...");

  if (!this->bmp.begin()) {
    this->logger->info("Could not find a valid BMP3 sensor, check wiring!");
    while (true);
  }
  this->bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  this->bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  this->bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  // First reading is always wrong
  this->measure();
}

Measure Barometer::measure()
{
  if (!this->bmp.performReading()) {
    this->logger->info("Failed to perform reading :(");
  }

  return Measure(
    this->rtc->getEpoch(),
    this->rtc->getFormattedDate(),
    this->bmp.pressure / 100.0
  );
}
