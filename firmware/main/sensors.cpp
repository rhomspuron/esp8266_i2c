
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




