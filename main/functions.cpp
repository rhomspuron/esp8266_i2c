#include "functions.h"

String readTemps(BasicSensor* sensors[], int nr_sensors){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  double value;
  for(int i=0; i< nr_sensors; i++){
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    value = sensors[i]->getValue();
    buff.print(value, DEC);
    buff.print(";");
    
  }
  buff.print("\n");
  return str_buff;
}
