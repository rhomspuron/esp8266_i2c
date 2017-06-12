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
#define DEBUG false
#define BUZZER_ON LOW
#define BUZZER_OFF HIGH
#define SERVER_PORT 23
#define GPIO_LED_WIFI_CONNECTED 5  
#define GPIO_BUZZ_ALARM 2  


String double2str(double value, int presition=100);
String readTemps(int presition=100);
String readStates();
void checkStates();
String readNrSensors();
void findStart();
void findStop();

//**************************************************************//
//                Gloabals Variables                            //
//**************************************************************//

WiFiServer server(SERVER_PORT);
WifiCom com;
bool flg_alarm = false;
bool flg_finding = false;

ADCSensor st1(A0);
TC74Sensor st2(0x48);
TC74Sensor st3(0x4A);
TC74Sensor st4(0x4B);
TC74Sensor st5(0x4D);



BasicSensor* sensors[] = {&st1,&st2,&st3,&st4,&st5};
int nr_sensors = 5;


//**************************************************************//
//                Setup Method                                  //
//**************************************************************//

void setup() {
  bool wifi_searching = true;
  
  // Prepare logger for debug 
  if (DEBUG){
    Serial.begin(115200);
    delay(50);
    Serial.print('Connecting to: ');
    Serial.println(ssid);

  }
  // Configure LED_WIFI
  pinMode(GPIO_LED_WIFI_CONNECTED, OUTPUT);
  digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW);
  
  // Connect to the Wifi
  // TODO: Activate Wifi LED
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while(true){
    if (i++ < 20){ // Nr of intents 
      if (WiFi.status() == WL_CONNECTED) break;
      //digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);
      delay(500);
      //digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW); 
    } 
 
  }
  //digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);

  //Start Telnet server
  server.begin();
  server.setNoDelay(true);
  if (DEBUG){
    Serial.print("Ready!\nUse 'telnet ");
    Serial.print(WiFi.localIP());
    Serial.println(" 23' to connect");
  }


  //Configure LED ALARM
  pinMode(GPIO_BUZZ_ALARM, OUTPUT);
  digitalWrite(GPIO_BUZZ_ALARM, LOW);
    
}

//**************************************************************//
//                Main Loop Method                              //
//**************************************************************//

void loop() { 
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
            break;
          case FOUND:
            findStop();
            break;
          case STATE:
            com.sendData(readStates());
            break;           
          case SENSORS:
            com.sendData(readNrSensors());
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
    value = sensors[i]->getValue();
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    buff.print(double2str(value, precision));
    buff.print(";");
    
  }
  buff.print("\n");
  return str_buff;
}
//**************************************************************//

String readStates(){
  String str_buff;
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
    }
    
  }
  if (!alarm)
    buff.print('OK');
  if(flg_finding)
    buff.print("; Finding: True");
  else
    buff.print("; Finding: False");
    
  buff.print("\n");
  return str_buff;
}
//**************************************************************//
void checkStates(){
  flg_alarm = false;
  for(int i=0; i< nr_sensors; i++){
    if (sensors[i]->isOutOfRange()){
      if(DEBUG){
        Serial.print("T");
        Serial.print(i+1, DEC);
        Serial.println("OutOfRange");
        Serial.print(readTemps());
      }
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
  return str_buff;
}
//**************************************************************//
void findStart(){
  flg_finding = true;
  digitalWrite(GPIO_BUZZ_ALARM, BUZZER_ON);
}
//**************************************************************//
void findStop(){
  flg_finding = false;
  digitalWrite(GPIO_BUZZ_ALARM, BUZZER_OFF);
}






