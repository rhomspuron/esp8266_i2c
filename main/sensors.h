#ifndef SENSOR_H
#define SENSOR_H

#include <ESP8266WiFi.h>
#include <Wire.h>

#define SCLPIN 14
#define MISOPIN 12

#define trigger 13
#define echo 16

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

class MPISensor : public BasicSensor{
  public:
    MPISensor(int address, double a=1, double b=0, 
              double v_min=-127, double v_max=128):
              BasicSensor(a,b,v_min, v_max), 
              address(address){};
  private:
    int address;
    double readFromHW();
};

class USSensor : public BasicSensor{
  public:
    USSensor(int address, double a=1, double b=0, 
              double v_min=-127, double v_max=128):
              BasicSensor(a,b,v_min, v_max), 
              address(address){};
  private:
    int address;
    double readFromHW();
};
#endif
