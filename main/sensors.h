#ifndef SENSOR_H
#define SENSOR_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "mathexpression.h"

class BasicSensor{
  public:
    BasicSensor(Expression* expression);
    double getValue();
  private:
    Expression* expression;
    virtual double readFromHW(){return 0;};
};

class SimulatorSensor: public BasicSensor{
  public:
    SimulatorSensor(Expression* expression):BasicSensor(expression){};
  private:
    double readFromHW();
};

//class I2CSensor : public BasicSensor{
//  public:
//    I2CSensor(int address, Expression* expresion);
//  private:
//    int address;  
//    double readFromHW();
//}


#endif
