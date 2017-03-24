#include "sensors.h"


BasicSensor::BasicSensor(Expression* expression){
  expression = expression;
}

double BasicSensor::getValue(){
  double x;
  x = readFromHW();
  
  return expression->calc(x);
}



double SimulatorSensor::readFromHW(){
  return 3.14;
}



