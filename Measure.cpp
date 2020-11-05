#include "Measure.h"

Measure::Measure(unsigned long epoch, String date, double pressure)
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

unsigned long Measure::epoch()
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
