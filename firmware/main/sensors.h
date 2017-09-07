
/*********************************************************************
ESP8266ARS: Arduino project for ALBA Remote Sensors base on ESP8266.

Author(s): Roberto J. Homs Puron <rhoms@cells.es>, 
           Alberto Rubio Garcia <arubio@cells.es>
           Sergio Astorga Sanchez <sastorga@cells.es>
        
Copyright (C) 2017, CELLS / ALBA Synchrotron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#ifndef SENSOR_H
#define SENSOR_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "max318xx.h"

class BasicSensor{
  public:
    BasicSensor(double a=0, double b=1, 
                 double v_min=-200, double v_max=200):
                 a(a),b(b),v_min(v_min),v_max(v_max){};
    bool isOutOfRange();
    virtual double getValue();
  private:
    double a, b, v_min, v_max;
    virtual double readFromHW(){return 0;};
};

class SimulatorSensor: public BasicSensor{
  public:
    SimulatorSensor(double a=0, double b=1, 
                 double v_min=-200, double v_max=200):
                 BasicSensor(a,b,v_min, v_max){};
  private:
    double readFromHW();
};

class TC74Sensor : public BasicSensor{
  public:
    TC74Sensor(int address, double a=1, double b=0, 
              double v_min=-127, double v_max=128):
              BasicSensor(a,b,v_min, v_max), 
              address(address){};
  private:
    int address;
    double readFromHW();
};

class ADCSensor : public BasicSensor{
  public:
    ADCSensor(int address, double a=1, double b=0, 
              double v_min=0, double v_max=5):
              BasicSensor(a,b,v_min, v_max), 
              adc_channel(address){};
  private:
    int adc_channel;
    double readFromHW();
};

class Max31855Sensor: public BasicSensor{
  public:
    Max31855Sensor(MAX31855 chip, double a=1, double b=0, 
              double v_min=-1000, double v_max=1000):
              BasicSensor(a,b,v_min, v_max),
              sensor(chip){};
  private:
    MAX31855 sensor;
    double readFromHW();
                
};

#endif
