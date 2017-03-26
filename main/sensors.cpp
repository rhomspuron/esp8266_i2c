#include "sensors.h"


double BasicSensor::getValue(){
  double x;
  x = readFromHW();
  return (a*x+b);

}

bool BasicSensor::isOutOfRange(){
  double value = getValue();
  if (value < v_min or value > v_max)
    return true;
  return false;
  
}

double SimulatorSensor::readFromHW(){
  return 3.14;
}

double I2CSensor::readFromHW(){
  char buff[nrbytes];
  int value =0;       
  Wire.beginTransmission(address);
  Wire.requestFrom(address, nrbytes);
  for(int i=0; i< nrbytes; i++){
    buff[i]=Wire.read();
  }
  Wire.endTransmission();
  //TODO convert char to int
  
  return double(value); 
}




