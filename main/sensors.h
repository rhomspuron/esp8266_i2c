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

//class I2CSensor : public BasicSensor{
//  public:
//    I2CSensor(int address, );
//  private:
//    int address;  
//    double readFromHW();
//}


#endif
