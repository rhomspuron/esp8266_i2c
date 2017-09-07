
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


//**************************************************************//
// Pin descriptions
// 0 -> Alarm Reley
// 1 -> TODO  
// 2 -> Buzzer
// 3 -> TODO
// 4 -> MISO
// 5 -> CLK
// 6 .. 14-> TOTO  
// 15 -> Switch
// 16 -> LED WiFi
//**************************************************************//

#include <Wire.h>
#include <ESP8266WiFi.h>

#include "com.h"
#include "sensors.h"
#include "pwds.h"

//**************************************************************//
//                 Gloabals declarations                        //
//**************************************************************//
#define DEVELOP
#define DEBUG true
#define BUZZER_ON LOW
#define BUZZER_OFF HIGH
#define SERVER_PORT 23
#define GPIO_LED_WIFI_CONNECTED 16  
#define GPIO_BUZZ_ALARM 2 
#define GPIO_CS_1 0
#define GPIO_CS_2 15


String double2str(double value, int presition=100);
String readTemps(int presition=100);
String readStates();
void checkStates();
String readNrSensors();
void findStart();
void findStop();
void printDebug(String value, bool endl=false);
void printWifiInfo();


//**************************************************************//
//                Gloabals Variables                            //
//**************************************************************//

WiFiServer server(SERVER_PORT);
WifiCom com;
bool flg_alarm = false;
bool flg_finding = false;

#ifdef DEVELOP
ADCSensor st1(A0);
Pcf8574CS d1(0x39,0);
Pcf8574CS d2(0x39,1);

Max31855Sensor st2(MAX31855(BasicCS(GPIO_CS_1)),1000);
Max31855Sensor st3(MAX31855(BasicCS(GPIO_CS_2)));

//TC74Sensor st4(0x48);
//TC74Sensor st5(0x4A);
//TC74Sensor st6(0x4B);
//TC74Sensor st7(0x4D);


BasicSensor* sensors[] = {&st1,&st2,&st3}; //,&st4,&st5};
int nr_sensors = 3; //5;

#else
Max31855Sensor st1(BasicCS(GPIO_CS_1));
Max31855Sensor st2(BasicCS(GPIO_CS_2));

BasicSensor* sensors[] = {&st1,&st2}; 
int nr_sensors = 2; 
#endif


//**************************************************************//
//                Setup Method                                  //
//**************************************************************//

void setup() {
  bool wifi_searching = true;
  
  // Prepare logger for debug 
  if (DEBUG){
    Serial.begin(115200);
    delay(100);
  }
  
  // Configure LED_WIFI
  pinMode(GPIO_LED_WIFI_CONNECTED, OUTPUT);
 
  // Connect to the Wifi
  // TODO: Activate Wifi LED
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  printDebug("Connecting");
  while(true){
    if (i < 20){ // Nr of intents 
      printDebug(".");
      if (WiFi.status() == WL_CONNECTED) break;
      digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);
      delay(250);
      digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW);
      delay(250);
      i++ ;
    } 
    else{
      digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);
      if (wifi_searching){
        printDebug("Can not connect to:");
        printDebug(ssid, true);
        wifi_searching = false;
    
      } 
    }
  }
  digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW);

  printWifiInfo();
  
  //Start Telnet server
  server.begin();
  server.setNoDelay(true);
  
  
  //Configure LED ALARM
  pinMode(GPIO_BUZZ_ALARM, OUTPUT);
  digitalWrite(GPIO_BUZZ_ALARM, BUZZER_OFF);

  Wire.begin();
  
}

//**************************************************************//
//                Main Loop Method                              //
//**************************************************************//

void loop() {
  Wire.beginTransmission(0X39);
  Wire.write(0XFF);
  Wire.endTransmission();
  bool has_client = false;
  checkStates();
  if (server.hasClient()){
    WiFiClient tmp = server.available();
    com = tmp;
    tmp.stop();
    has_client = true;
  }

  while(has_client){
    if (com.isConnected()){
      checkStates();
      Cmds cmd = com.getCommand();
      if (cmd != NONE)
        switch (cmd){
          case TEMP: 
            com.sendData(readTemps());
            break;
          case CONFIG: 
            com.sendData(String("Not implemented.\n")); 
            break;
          case FIND: 
            findStart();
            com.sendData(String("Done\n"));
            break;
          case FOUND:
            findStop();
            com.sendData(String("Done\n"));
            break;
          case STATE:
            com.sendData(readStates());
            break;           
          case SENSORS:
            com.sendData(readNrSensors());
            break;
          case DON:
            d1.setHigh();
            d2.setHigh();
            //Wire.beginTransmission(0X39);
            //Wire.write(0XFF);
            //Wire.endTransmission();
            com.sendData("Don \n");
            break;
            
          case DOFF:
            d1.setLow();
            //Wire.beginTransmission(0X39);
            //Wire.write(0X00);
            //Wire.endTransmission();
            com.sendData("Doff \n");
            break;
            
        }
    }
    else has_client = false;
   }
   
}


//**************************************************************//
//                Functions                                     //
//**************************************************************//

String double2str(double value, int precision){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  buff.print(int(value),DEC);
  buff.print(".");
  unsigned int frac;
  if(value >= 0)
    frac = (value - int(value)) * precision;
  else
    frac = (int(value)- value ) * precision;
  buff.print(frac,DEC) ;
  
  return str_buff;
}
//**************************************************************//

String readTemps(int precision){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  double value;
  for(int i=0; i< nr_sensors; i++){
    switch(i){
      case 0: break;
      case 1: d1.setLow(); break;
      case 2: d2.setLow(); break;
      
    }
    value = sensors[i]->getValue();
    switch(i){
      case 0: break;
      case 1: d1.setHigh(); break;
      case 2: d2.setHigh(); break;
      
    }
    
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    buff.print(double2str(value, precision));
    buff.print(";");
    
  }
  buff.print("\n");
  printDebug("readTemps: ");
  printDebug(str_buff);
  return str_buff;
}
//**************************************************************//

String readStates(){
  String str_buff("");
  bool alarm = false;
  double value;
  StringStream buff = StringStream(str_buff);
  for(int i=0; i< nr_sensors; i++){
    if (sensors[i]->isOutOfRange()){
      alarm = true;
      buff.print("T");
      buff.print(i+1,DEC);
      buff.print(":ALARM (");
      value = sensors[i]->getValue();
      buff.print(double2str(value));
      buff.print("); ");
      printDebug(str_buff);
    }
    
  }
  if (!alarm)
    buff.print("OK; ");
  
  buff.print("Finding: ");
  if(flg_finding)
    buff.print("True; ");
  else
    buff.print("False; ");
  buff.print("\n");
  printDebug("readStates: ");
  printDebug(str_buff);
  return str_buff;
}
//**************************************************************//
void checkStates(){
  flg_alarm = false;
  for(int i=0; i< nr_sensors; i++){
    if (sensors[i]->isOutOfRange()){
      //digitalWrite(GPIO_LED_ALARM, HIGH);
      flg_alarm = true;
      break;
    } 
  }
   
}
//**************************************************************//
String readNrSensors(){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  buff.print("Nr. Sensors:");
  buff.print(nr_sensors,DEC);
  buff.print("\n");
  
  printDebug("readNrSensors");
  printDebug(str_buff);
  return str_buff;
}
//**************************************************************//
void findStart(){
  flg_finding = true;
  digitalWrite(GPIO_BUZZ_ALARM, BUZZER_ON);
  printDebug("findStart: Buzzer ON", true);
}
//**************************************************************//
void findStop(){
  flg_finding = false;
  digitalWrite(GPIO_BUZZ_ALARM, BUZZER_OFF);
  printDebug("findStop: Buzzer OFF", true);
}
//**************************************************************//
void printDebug(String value, bool endl){
  if (!DEBUG) return;
  Serial.print(value);
  if (endl)
    Serial.print("\n");
}
//**************************************************************//
void printWifiInfo(){
  if(!DEBUG) return;
    
  printDebug("\n\n");
  printDebug("Connected to: ");
  printDebug(ssid, true);
  
  String str_buff;
  byte mac[6];  
  StringStream buff = StringStream(str_buff);
  WiFi.macAddress(mac);
  buff.print("MAC: ");
  for(int i=0; i<6; i++){
    buff.print(mac[i], HEX);
    if (i!=5)
      buff.print(':'); 
  }
  printDebug(str_buff, true);
  printDebug("IP: ");
  printDebug(WiFi.localIP().toString(), true);
  printDebug("To connect use: 'telnet IP 23'", true);
  
}
//**************************************************************//



