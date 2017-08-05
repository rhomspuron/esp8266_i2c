#include "max31855.h"

MAX31855::MAX31855(int8_t cs, int8_t sck, int8_t miso):
                   cs(cs), sck(sck), miso(miso){

  // Software SPI
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);

  pinMode(sck, OUTPUT);
  pinMode(miso, INPUT);
}

MAX31855::MAX31855(int8_t cs): cs(cs), sck(-1), miso(-1){
  
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
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
  digitalWrite(cs, LOW);
  delay(1);
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
  digitalWrite(cs, HIGH);

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
 
