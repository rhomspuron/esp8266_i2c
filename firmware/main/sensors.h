#ifndef SENSOR_H
#define SENSOR_H

#include <ESP8266WiFi.h>
#include <Wire.h>


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

#endif
