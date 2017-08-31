
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

#include "max318xx.h"

BasicCS::BasicCS(int gpio):gpio(gpio){
  pinMode(gpio, OUTPUT);
  setHigh();
}

void BasicCS::setValue(bool value){
  digitalWrite(gpio, value);
  delay(1);
}

Pcf8574CS::Pcf8574CS(int addr, uint8_t pin): 
  addr(addr), pin(pin)
{
    Wire.begin();
}

void Pcf8574CS::setValue(bool value){
  uint8_t data;
  Wire.beginTransmission(addr);
  Wire.requestFrom(addr, 1);
  data = Wire.read();
  if(value)
    data &= ~(1<<pin);
  else
    data |= (1<<pin);
  Wire.write(data);
  Wire.endTransmission();
}

MAX31855::MAX31855(BasicCS cs, int8_t sck, int8_t miso):
                   cs(cs), sck(sck), miso(miso){

  // Software SPI
  pinMode(sck, OUTPUT);
  pinMode(miso, INPUT);
}

MAX31855::MAX31855(BasicCS cs): cs(cs), sck(-1), miso(-1){
  
  SPI.begin();
  
}

double MAX31855::readThermocouple(){
  updateValues();
  return thermocouple;  
}

double MAX31855::readInternal(){
  updateValues();
  return internal;
}
bool MAX31855::isShortVcc(){
  updateValues();
  return short_vcc;
}

bool MAX31855::isShortGnd(){
  updateValues();
  return short_gnd;
}
bool MAX31855::isOpenCircuit(){
  updateValues();
  return open_circuit;
}

void MAX31855::updateValues(){
  int i;
  uint32_t data = 0;

  // Read 32 bits data
  cs.setLow();
  if (sck == -1){
    // Hardware SPI
    SPI.beginTransaction(SPISettings(MAX_FREQ, BITS_ORDER, SPI_MODE));
    data = SPI.transfer(0);
    data <<= 8;
    data |= SPI.transfer(0);
    data <<= 8;
    data |= SPI.transfer(0);
    data <<= 8;
    data |= SPI.transfer(0);
    SPI.endTransaction();
  } else {
    // Software SPI

    digitalWrite(sck, HIGH);
    delay(1);

    for (i=31; i>=0; i--) {
      digitalWrite(sck, LOW);
      delay(0.001);
      data <<= 1;
      if (digitalRead(miso)) {
        data |= 1;
      }
      digitalWrite(sck, HIGH);
      delay(0.001);
    }
  }
  cs.setHigh();
  
  /* Process data bits:
   *  31 -> 18: Thermocouple temperature 31=sign, 30=2^10, 18=2^-2
   *  17: No used
   *  16: Fault
   *  15 -> 4: Internal temperature 15=sign, 14=2^6, 4=2^-4
   *  2: Short to Vcc
   *  1: Short to GND
   *  0: Open circuit
   */

  open_circuit = (data & 0x01) >> 1; 
  short_gnd = (data & 0x02) >> 2;
  short_vcc = (data & 0x04) >> 3;
  
  uint16_t tmp = (data & 0x3FF8) >> 4;
  internal = tmp & 0x07FF;
  if (tmp & 0x0800){
    internal *= -1;
  }
  internal *= 0.0625;  
 
  fault = (data & 0x8000) >> 16;

  tmp = (data & 0x0FFFFC00) >> 18;
  thermocouple = tmp & 0x01FFF;
  if (tmp & 0x2000){
    thermocouple *= -1;
  }
  thermocouple *= 0.25;  
  
}
 
