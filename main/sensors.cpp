#include "sensors.h"


double BasicSensor::getValue(){
  double x;
  x = readFromHW();
  return (a*x+b);

}


double SimulatorSensor::readFromHW(){
  return 3.14;
}



