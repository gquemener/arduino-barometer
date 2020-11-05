#ifndef MEASURE_H
#define MEASURE_H

#include "Arduino.h"

class Measure {
    unsigned long _epoch;
    String _date;
    double _pressure;
  public:
    Measure(unsigned long, String, double);
    Measure(String, double);
    Measure();
    
    unsigned long epoch();
    double pressure();
    String hours();
};

#endif
