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
double MPISensor::readFromHW(){
  int value = -2;
  digitalWrite(address,LOW);
  delay(2);
  digitalWrite(address,HIGH);
  delay(220);
  /* Read the chip and return the raw temperature value */
  /* Bring CS pin low to allow us to read the data from
   the conversion process */
  digitalWrite(address,LOW);
  /* Cycle the clock for dummy bit 15 */
  digitalWrite(SCLPIN,HIGH);
  delay(1);
  digitalWrite(SCLPIN,LOW);
  /*
   Read bits 14-3 from MAX6675 for the Temp. Loop for each bit reading
   the value and storing the final value in 'temp'
   */
  for (int i=14; i>=0; i--) {
    digitalWrite(SCLPIN,HIGH);
    value += digitalRead(MISOPIN) << i;
    digitalWrite(SCLPIN,LOW);
  }
  // check bit D2 if HIGH no sensor
  if ((value & 0x04) == 0x04) return -1;
  // shift right three places
  return value >> 3;   
}

double USSensor::readFromHW(){
  long howfar;
  digitalWrite(trigger,LOW);
  delayMicroseconds(5);                                                                              
  digitalWrite(trigger,HIGH);
  delayMicroseconds(15);
  digitalWrite(trigger,LOW);
  howfar=pulseIn(echo,HIGH);
  howfar=howfar*0.01657; //how far away is the object in cm
  return round(howfar);
}
