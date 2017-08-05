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

double TC74Sensor::readFromHW(){
  //start the communication with IC with the address xx
  Wire.begin();
  Wire.beginTransmission((uint8_t)address); 
  //send a bit and ask for register zero
  Wire.write((uint8_t) 0);
  //end transmission
  int error = Wire.endTransmission();
  if (error != 0){
    return -1e9;
  }
  //request 1 byte from address xx
  Wire.requestFrom((uint8_t)address, (uint8_t)1);

  //To detect the error.
  int temp = 1e9;
  for (int i=0; i < 3; i++){
    if (Wire.available() == 0){
      delay(10);    
    }
    else{
      temp = Wire.read();
      if (temp > 127)
        temp -= 256;
    }
  }
  return double(temp);
    
}

double Max31855Sensor::readFromHW(){
  return sensor.readThermocouple();
}

double ADCSensor::readFromHW(){
  return analogRead(adc_channel); 
}




