#include <Wire.h>
#include <ESP8266WiFi.h>

#include "com.h"
#include "sensors.h"
#include "pwds.h"

//**************************************************************//
//                 Gloabals declarations                        //
//**************************************************************//
#define DEBUG true
#define SERVER_PORT 23
#define GPIO_LED_WIFI_CONNECTED 13  
#define GPIO_LED_ALARM 16  
#define CS1PIN 15
#define CS2PIN 0
#define CS3PIN 2

String readTemps(int presition=100);
String readTempMpi(int presition=100);
String readStates();
void checkStates();

//**************************************************************//
//                Gloabals Variables                            //
//**************************************************************//
WiFiServer server(SERVER_PORT);
WifiCom com;
bool flg_alarm = false;

SimulatorSensor st1(2.0,1.,0.,5.);
BasicSensor st2;
TC74Sensor st3(0x48);
TC74Sensor st4(0x4A);
TC74Sensor st5(0x4B);
TC74Sensor st6(0x4D);
MPISensor st7(CS1PIN);
MPISensor st8(CS2PIN);
MPISensor st9(CS3PIN);
BasicSensor* sensors[] = {&st1,&st2,&st3,&st4,&st5,&st6,&st7,&st8,&st9};
int nr_sensors = 9;

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
    //Configure LED ALARM
  pinMode(GPIO_LED_ALARM, OUTPUT);
  digitalWrite(GPIO_LED_ALARM, LOW); 
  
  // Connect to the Wifi
  // TODO: Activate Wifi LED
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  //Start Telnet server
  server.begin();
  server.setNoDelay(true);
  if (DEBUG){
    Serial.print("Ready!\nUse 'telnet ");
    Serial.print(WiFi.localIP());
    Serial.println(" 23' to connect");
  }
 
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
          case DI: 
            com.sendData(String("DI1 High; DI2 Low;\n")); 
            break;
          case DO:
            com.sendData(String("Not implemented\n"));
            break;
          case STATE:
            com.sendData(readStates());
            break;           
        }
    }
    else has_client = false;
   }
}

//**************************************************************//
//                Functions                                     //
//**************************************************************//
String readTemps(int precision){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  double value;
  for(int i=0; i< nr_sensors; i++){
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    value = sensors[i]->getValue();
    buff.print(int(value),DEC);
    buff.print(".");
    unsigned int frac;
    if(value >= 0)
       frac = (value - int(value)) * precision;
    else
       frac = (int(value)- value ) * precision;
    buff.print(frac,DEC) ;
    buff.print(";");
    
  }
  buff.print("\n");
  return str_buff;
}


//**************************************************************//
String readStates(){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  for(int i=0; i< nr_sensors; i++){
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    const char* value = sensors[i]->isOutOfRange()? "ALARM":"OK";
    buff.print(value);
    buff.print(";"); 
  }
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
  if (!flg_alarm)
    digitalWrite(GPIO_LED_ALARM, LOW);
}
