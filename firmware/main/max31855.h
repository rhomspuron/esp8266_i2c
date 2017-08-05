#ifndef MAX31855_H
#define MAX31855_H

#include <SPI.h>

#define MAX_FREQ 8000000 // 8MHZ
#define SPI_MODE SPI_MODE0
#define BITS_ORDER MSBFIRST

class MAX31855{
  public:
    MAX31855(int8_t cs, int8_t sck, int8_t miso);
    MAX31855(int8_t cs);
   
    double readThermocouple();
    double readInternal();
    bool isShortVcc();
    bool isShortGnd();
    bool isOpenCircuit();
    bool isFault(); 
    
  private:
    int8_t cs, sck, miso;
    double thermocouple, internal;
    bool short_vcc;
    bool short_gnd;
    bool open_circuit;
    bool fault;
    void updateValues();
  
  
};
#endif
