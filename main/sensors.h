#ifndef SENSOR_H
#define SENSOR_H

#include <ESP8266WiFi.h>
#include <Wire.h>


class BasicSensor{
  public:
    BasicSensor(double a=0, double b=1):a(a),b(b){};
    virtual double getValue();
  private:
    double a, b;
    virtual double readFromHW(){return 0;};
};

class SimulatorSensor: public BasicSensor{
  public:
    SimulatorSensor(double a=0, double b=1):BasicSensor(a,b){};
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
